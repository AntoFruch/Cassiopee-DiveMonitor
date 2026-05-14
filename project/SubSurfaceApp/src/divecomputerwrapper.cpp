#include "divecomputerwrapper.h"

extern "C" {
    JNIEXPORT void JNICALL Java_org_subsurfacedivelog_mobile_SubsurfaceMobileActivity_setUsbDevice(JNIEnv *env, jclass clazz, jobject device) {
        // Optionnel : stocker le device détecté
        qDebug() << "USB Device détecté par le système !";
    }

    JNIEXPORT void JNICALL Java_org_subsurfacedivelog_mobile_SubsurfaceMobileActivity_restartDownload(JNIEnv *env, jclass clazz, jobject device) {
        // C'est ici que tu déclenches l'ouverture du port
        // car on sait que la permission a été accordée.
        qDebug() << "Permission accordée, prêt à ouvrir le flux.";
    }
}

DiveComputerWrapper& DiveComputerWrapper::instance()
{
    static DiveComputerWrapper instance;
    return instance;
}

DiveComputerWrapper::DiveComputerWrapper(QObject *parent) {
    qDebug() << "Creating libdivecomputer context...";
    if (dc_context_new(&context) != DC_STATUS_SUCCESS) {
        qDebug() << "Failed to create context!";
        throw std::runtime_error("Failed to create context");
    }
    qDebug() << "Context created successfully.";

    this->db = &DiveDatabase::instance();

    this->updateSupportedDevices();
}

DiveComputerWrapper::~DiveComputerWrapper(){

}

bool DiveComputerWrapper::connectToDevice(const QString& vendor, const QString& product, const QString& connectionMode, const QString& port){
    if (!openSerial(port))
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


    // On vide la liste actuelle avant de rafraîchir
    availablePorts.clear();

#ifdef Q_OS_ANDROID
    if (transport_mode == DC_TRANSPORT_SERIAL) {
        // On utilise la fonction JNI pour scanner l'USB
        detectedDevices = serial_usb_android_get_devices();
        for (const auto& dev : detectedDevices) {
            availablePorts.append(QString::fromStdString(dev.uiRepresentation));
        }
        emit availablePortsChanged();
        return;
    }
#endif

    dc_iterator_t* iter = nullptr; // Toujours initialiser à nullptr

    const char* (*get_name)(void*) = nullptr;
    void (*device_free)(void*) = nullptr;

    switch (transport_mode) {
    case DC_TRANSPORT_SERIAL:
        if (dc_serial_iterator_new(&iter, context, NULL) != DC_STATUS_SUCCESS) {
            qDebug() << "Failed to create serial iterator!";
            return;
        }
        get_name = reinterpret_cast<const char*(*)(void*)>(dc_serial_device_get_name);
        device_free = reinterpret_cast<void(*)(void*)>(dc_serial_device_free);
        break;

    case DC_TRANSPORT_BLUETOOTH:
        if (dc_bluetooth_iterator_new(&iter, context, NULL) != DC_STATUS_SUCCESS) {
            qDebug() << "Failed to create bluetooth iterator! Check if BT is enabled.";
            return;
        }
        get_name = reinterpret_cast<const char*(*)(void*)>(dc_bluetooth_device_get_name);
        device_free = reinterpret_cast<void(*)(void*)>(dc_bluetooth_device_free);
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
        device_free(dev);
    }
    dc_iterator_free(iter);
    emit availablePortsChanged();
}

bool DiveComputerWrapper::openSerial(const QString& port) {
#ifdef Q_OS_ANDROID
    // Sur Android, 'port' est le nom (uiRepresentation) du device choisi dans la liste
    // On doit retrouver le descripteur USB correspondant

    // 'detectedDevices' doit être un membre de ta classe rempli par refreshPorts()
    android_usb_serial_device_descriptor* selectedDev = nullptr;
    for (auto& dev : detectedDevices) {
        if (QString::fromStdString(dev.uiRepresentation) == port) {
            selectedDev = &dev;
            break;
        }
    }

    if (!selectedDev) {
        qDebug() << "Android: Aucun périphérique USB correspondant à" << port;
        return false;
    }

    // On utilise la fonction de Subsurface pour créer l'iostream CUSTOM
    if (serial_usb_android_open(&iostream, context, selectedDev) == DC_STATUS_SUCCESS) {
        qDebug() << "Android: Flux Custom IO ouvert avec succès";
        return true;
    }

    qDebug() << "Android: Échec de serial_usb_android_open";
    return false;

#else
    // Logique standard pour Windows/Mac/Linux
    if (dc_serial_open(&iostream, context, port.toUtf8().data()) == DC_STATUS_SUCCESS) {
        qDebug() << "Desktop: Port série ouvert sucessfully";
        return true;
    }
    return false;
#endif
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

struct SampleContext {
    QList<DiveEntry>* entries;
    TempEntry current;
};

void dive_sample_callback(dc_sample_type_t type,
                          const dc_sample_value_t *value,
                          void *userdata)
{
    // On récupère notre contexte local à cette plongée
    SampleContext* ctx = static_cast<SampleContext*>(userdata);

    switch(type) {
    case DC_SAMPLE_TIME:
        // Si on a déjà un point en cours de construction, on le valide
        if (ctx->current.time >= 0) {
            DiveEntry e;
            e.time = ctx->current.time;
            e.depth = ctx->current.depth;
            e.temperature = ctx->current.temperature;
            ctx->entries->append(e);
        }

        // Initialisation du nouveau point
        ctx->current.time = value->time / 1000;
        ctx->current.depth = QVariant();
        ctx->current.temperature = QVariant();
        break;

    case DC_SAMPLE_DEPTH:
        ctx->current.depth = value->depth;
        break;

    case DC_SAMPLE_TEMPERATURE:
        ctx->current.temperature = value->temperature;
        break;

    default:
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

    // --- LOGIQUE DE SAMPLES CORRIGÉE ---
    SampleContext sCtx;
    sCtx.entries = &dive.entries;
    sCtx.current = TempEntry(); // Initialise proprement (time = -1)

    dc_parser_samples_foreach(parser, dive_sample_callback, &sCtx);

    // Le callback enregistre le point N quand il reçoit le temps du point N+1.
    // Le dernier point n'a pas de N+1, il faut donc le pousser manuellement ici.
    if (sCtx.current.time >= 0) {
        DiveEntry last;
        last.time = sCtx.current.time;
        last.depth = sCtx.current.depth;
        last.temperature = sCtx.current.temperature;
        dive.entries.append(last);
    }

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
