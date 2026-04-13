#include "divecomputerwrapper.h"

DiveComputerWrapper::DiveComputerWrapper(QObject *parent) {
    qDebug() << "Creating libdivecomputer context...";
    if (dc_context_new(&context) != DC_STATUS_SUCCESS) {
        qDebug() << "Failed to create context!";
        throw std::runtime_error("Failed to create context");
    }
    qDebug() << "Context created successfully.";

    this->db = &DiveDatabase::instance();
    this->divesModel = new DiveListModel(this);
    this->samplesModel = new SampleModel(this);

    connect(this, &DiveComputerWrapper::importationDone, this, [this](){
        this->loadAllDives();
    });

    this->updateSupportedDevices();
    this->loadAllDives();
}

DiveComputerWrapper::~DiveComputerWrapper(){

}

bool DiveComputerWrapper::connectToDevice(const QString& vendor, const QString& product){
    if (!openSerial())
        return false;

    if (!findDescriptor(vendor.toStdString(), product.toStdString()))
        return false;

    if (dc_device_open(&device, context, descriptor, iostream)
        != DC_STATUS_SUCCESS)
        return false;

    this->connected = true;
    this->vendor = vendor;
    this->product = product;
    this->last_fingerprint = db->getFingerprint(vendor, product);

    emit connectedChanged();

    return true;
}

void DiveComputerWrapper::disconnectDevice(){
    if (!device) {
        qDebug() << "Disconnect ignored: No device handle exists.";
        return;
    }

    qDebug() << "Disconnected from...";
    if (device) {
        qDebug() << "Closing device...";
        dc_device_close(device);
        device = nullptr;
    }

    if (iostream) {
        qDebug() << "Closing IO stream...";
        dc_iostream_close(iostream);
        iostream = nullptr;
    }

    this->connected = false;
    emit connectedChanged();
    return;
}

dc_transport_t transportStringtoEnum(const QString& transport) {
    static const QMap<QString, dc_transport_t> transportMap = {
        {"Serial", DC_TRANSPORT_SERIAL},
        {"Bluetooth Low Energy", DC_TRANSPORT_BLE},
        {"Bluetooth", DC_TRANSPORT_BLUETOOTH}
    };

    return transportMap.value(transport, (dc_transport_t)0);
}

void DiveComputerWrapper::refreshPorts(QString transport) {
    dc_transport_t transport_mode = transportStringtoEnum(transport);

    dc_iterator_t* iter = nullptr; // Toujours initialiser à nullptr
    const char* (*get_name)(void*) = nullptr;

    // On vide la liste actuelle avant de rafraîchir
    availablePorts.clear();

    switch (transport_mode) {
    case DC_TRANSPORT_SERIAL:
        if (dc_serial_iterator_new(&iter, context, NULL) != DC_STATUS_SUCCESS) {
            qDebug() << "Failed to create serial iterator!";
            return;
        }
        get_name = reinterpret_cast<const char*(*)(void*)>(dc_serial_device_get_name);
        break;

    case DC_TRANSPORT_BLUETOOTH:
        if (dc_bluetooth_iterator_new(&iter, context, NULL) != DC_STATUS_SUCCESS) {
            qDebug() << "Failed to create bluetooth iterator! Check if BT is enabled.";
            return;
        }
        get_name = reinterpret_cast<const char*(*)(void*)>(dc_bluetooth_device_get_name);
        break;

    default:
        qDebug() << "Unsupported transport mode";
        return;
    }

    void* dev = nullptr;
    while (dc_iterator_next(iter, &dev) == DC_STATUS_SUCCESS) {
        const char* name = get_name(dev);
        if (name) {
            availablePorts.append(QString::fromUtf8(name));
            qDebug() << "Found device:" << name;
        }
        // Important : Libérer le device selon la doc de libdivecomputer si nécessaire
        // dc_device_free((dc_device_t*)dev);
    }

    dc_iterator_free(iter); // ⚠️ NE PAS OUBLIER DE LIBÉRER L'ITÉRATEUR
    emit availablePortsChanged(); // Notifier le QML
}

bool DiveComputerWrapper::openSerial()
{
    qDebug() << "Opening serial devices...";
    dc_iterator_t* iter;

    if (dc_serial_iterator_new(&iter, context, NULL)
        != DC_STATUS_SUCCESS) {
        qDebug() << "Failed to create serial iterator!";
        return false;
    }

    void* dev;

    while (dc_iterator_next(iter, &dev) == DC_STATUS_SUCCESS) {
        auto* serial = (dc_serial_device_t*)dev;
        const char* name = dc_serial_device_get_name(serial);
        qDebug() << "Found serial device:" << name;

        if (strstr(name, "usb") || strstr(name, "USB")) {

            if (dc_serial_open(&iostream, context, name)
                == DC_STATUS_SUCCESS) {
                qDebug() << "Serial device opened successfully:" << name;

                dc_serial_device_free(serial);
                dc_iterator_free(iter);
                return true;
            } else {
                qDebug() << "Failed to open serial device:" << name;
            }
        }

        dc_serial_device_free(serial);
    }

    qDebug() << "No suitable serial device found.";
    dc_iterator_free(iter);
    return false;
}

bool DiveComputerWrapper::findDescriptor(const std::string& vendor,
                               const std::string& product)
{
    qDebug() << "Looking for descriptor:" << QString::fromStdString(vendor)
    << "/" << QString::fromStdString(product);

    dc_iterator_t* iter;

    if (dc_descriptor_iterator_new(&iter, context)
        != DC_STATUS_SUCCESS) {
        qDebug() << "Failed to create descriptor iterator!";
        return false;
    }

    void* desc;

    while (dc_iterator_next(iter, &desc) == DC_STATUS_SUCCESS) {
        auto* d = (dc_descriptor_t*)desc;

        const char* devVendor = dc_descriptor_get_vendor(d);
        const char* devProduct = dc_descriptor_get_product(d);

        if (vendor == devVendor && product == devProduct) {
            descriptor = d;
            qDebug() << "Descriptor matched : " << devVendor << "/" << devProduct;;
            dc_iterator_free(iter);
            return true;
        }
    }

    qDebug() << "No matching descriptor found.";
    dc_iterator_free(iter);
    return false;
}

void DiveComputerWrapper::updateSupportedDevices(){
    dc_iterator_t *iterator;
    dc_descriptor_t *descriptor;

    // JSON array to hold all vendors
    QJsonArray vendorsArray;

    // Get iterator over all supported dive computers
    if (dc_descriptor_iterator(&iterator) != DC_STATUS_SUCCESS) {
        fprintf(stderr, "Failed to get descriptors\n");
        return;
    }

    // Map to store vendor -> products
    QMap<QString, QJsonArray> vendorProductsMap;

    // Iterate over all descriptors
    while (dc_iterator_next(iterator, &descriptor) == DC_STATUS_SUCCESS) {
        // Get vendor and product name, and transports
        const char *vendorCStr = dc_descriptor_get_vendor(descriptor);
        const char *productCStr = dc_descriptor_get_product(descriptor);
        unsigned int transports = dc_descriptor_get_transports(descriptor);

        if (!vendorCStr || !productCStr) {
            dc_descriptor_free(descriptor);
            continue;
        }

        QString vendor = QString::fromUtf8(vendorCStr);
        QString product = QString::fromUtf8(productCStr);

        // Convert transport flags to strings
        QStringList transportMethods;
        if (transports & DC_TRANSPORT_USB) transportMethods.append("USB");
        if (transports & DC_TRANSPORT_USBHID) transportMethods.append("USBHID");
        if (transports & DC_TRANSPORT_BLUETOOTH) transportMethods.append("Bluetooth");
        if (transports & DC_TRANSPORT_BLE) transportMethods.append("Bluetooth Low Energy");
        if (transports & DC_TRANSPORT_SERIAL) transportMethods.append("Serial");
        if (transports & DC_TRANSPORT_IRDA) transportMethods.append("IrDA");

        // Create product JSON object
        QJsonObject productObj;
        productObj["name"] = product;
        QJsonArray transportsArray;
        for (const QString &t : transportMethods)
            transportsArray.append(t);
        productObj["transports"] = transportsArray;

        // Add product to vendor
        vendorProductsMap[vendor].append(productObj);

        dc_descriptor_free(descriptor); // free each descriptor after use
    }

    // Convert map to final JSON array
    for (auto it = vendorProductsMap.begin(); it != vendorProductsMap.end(); ++it) {
        QJsonObject vendorObj;
        vendorObj["vendor"] = it.key();
        vendorObj["products"] = it.value();
        vendorsArray.append(vendorObj);
    }

    dc_iterator_free(iterator); // free the iterator

    supportedDevices = vendorsArray;
    emit supportedDevicesChanged();
    return;
}

struct TempEntry {
    double time = -1;
    QVariant depth;
    QVariant temperature;
};

void dive_sample_callback(dc_sample_type_t type,
                          const dc_sample_value_t *value,
                          void *userdata
                          ){
    // userdata est la liste finale des entries
    QList<DiveEntry>* entries = (QList<DiveEntry>*) userdata;

    // static pour garder l’état entre les appels
    static TempEntry current;

    switch(type) {
    case DC_SAMPLE_TIME:
        // Nouveau temps → on pousse l'ancien s'il est valide
        if (current.time >= 0) {
            DiveEntry e;
            e.time = current.time;
            e.depth = current.depth;
            e.temperature = current.temperature;
            entries->append(e);
        }
        // On réinitialise pour le prochain point
        current.time = value->time / 1000;
        current.depth = QVariant();
        current.temperature = QVariant();
        break;

    case DC_SAMPLE_DEPTH:
        current.depth = value->depth;
        break;

    case DC_SAMPLE_TEMPERATURE:
        current.temperature = value->temperature;
        break;

    default:
        // ignorer les autres types pour l'instant
        break;
    }
}

struct CallBackContext{
    dc_device_t* device;
    QList<DiveData> dives;
    QByteArray cur_fp;
    QByteArray new_fp;
#if DEBUG
    int nbDive = 3;
#endif

};

int dive_callback(const unsigned char *data,
                  unsigned int size,
                  const unsigned char *fingerprint,
                  unsigned int fsize,
                  void *userdata
                  ){
    CallBackContext* ctx = (CallBackContext*)userdata;

#if DEBUG
    if (ctx->nbDive <= ctx->dives.length()){
        return 0;
    }
#endif

    DiveData dive;

    dive.fingerprint = QByteArray(reinterpret_cast<const char*>(fingerprint), fsize);

    // si le fingerprint est le meme, on arrete, la plongée est deja importée.
    if (dive.fingerprint == ctx->cur_fp){
        return 0;
    }
    // sinon actualise le nouveau fingerprint et on continue.
    else if (ctx->new_fp.isEmpty()){
        ctx->new_fp = dive.fingerprint;
    }

    dc_parser_t* parser;

    if (dc_parser_new(&parser, ctx->device, data, size) != DC_STATUS_SUCCESS){
        qWarning() << "Error : could not create parser for dive " << fingerprint;
        return 0;
    }

    // META-DONNEES
    unsigned int uval;
    double dval;
    dc_datetime_t dtval;

    // Date
    if (dc_parser_get_datetime(parser, &dtval) == DC_STATUS_SUCCESS){
        QDate date(dtval.year, dtval.month, dtval.day);
        QTime time(dtval.hour, dtval.minute, dtval.second);

        QDateTime qdt(date, time, QTimeZone::UTC);

        // Gestion du fuseau horaire (si valide)
        if (dtval.timezone != DC_TIMEZONE_NONE) {
            qdt = qdt.addSecs(dtval.timezone);
        }

        dive.date_time = qdt;
    }

    // Temps de plongée
    if (dc_parser_get_field(parser, DC_FIELD_DIVETIME, 0, &uval) == DC_STATUS_SUCCESS)
        dive.dive_time = uval ;

    // Profondeur max
    if (dc_parser_get_field(parser, DC_FIELD_MAXDEPTH, 0, &dval) == DC_STATUS_SUCCESS)
        dive.max_depth = dval;

    // Profondeur moyenne
    if (dc_parser_get_field(parser, DC_FIELD_AVGDEPTH, 0, &dval) == DC_STATUS_SUCCESS)
        dive.avg_depth = dval;

    // Pression atmosphérique
    if (dc_parser_get_field(parser, DC_FIELD_ATMOSPHERIC, 0, &dval) == DC_STATUS_SUCCESS)
        dive.atmos_pressure = dval;

    // Température surface
    if (dc_parser_get_field(parser, DC_FIELD_TEMPERATURE_SURFACE, 0, &dval) == DC_STATUS_SUCCESS)
        dive.surface_temperature = dval;

    // Température minimale
    if (dc_parser_get_field(parser, DC_FIELD_TEMPERATURE_MINIMUM, 0, &dval) == DC_STATUS_SUCCESS)
        dive.min_temperature = dval;

    // Température maximale
    if (dc_parser_get_field(parser, DC_FIELD_TEMPERATURE_MAXIMUM, 0, &dval) == DC_STATUS_SUCCESS)
        dive.max_temperature = dval;

    dc_parser_samples_foreach(parser, dive_sample_callback, &dive.entries);
    ctx->dives.append(dive);
    qDebug() << "dive " << ctx->dives.length() <<"  fetched ";
    return 1;
}

void DiveComputerWrapper::importDives(){
    if (!isConnected()){
        qWarning() << "Error : device is not connected";
        return;
    }
    struct CallBackContext ctx;
    ctx.device = device;
    ctx.cur_fp = this->last_fingerprint;
    if (dc_device_foreach(device, dive_callback, &ctx) != DC_STATUS_SUCCESS){
        qWarning() << "Error : could not fetch dives";
        return;
    }

    for (DiveData dive : ctx.dives){
        db->insertDive(dive);
    }

    if (!ctx.new_fp.isEmpty()){
        db->saveFingerprint(this->vendor, this->product, ctx.new_fp);
        this->last_fingerprint = ctx.new_fp;
    }

    qDebug() << ctx.dives.length() << " dives imported successfully";
}

void DiveComputerWrapper::importDivesAsync(){
    setImporting(true);
    QtConcurrent::run([this](){
        this->importDives();
        setImporting(false);
        emit importationDone();
    });
}

void DiveComputerWrapper::loadAllDives(){
    divesModel->loadDives();
}

void DiveComputerWrapper::loadDiveEntries(int id){
    samplesModel->setEntries(id);
    emit samplesModelChanged();
}
