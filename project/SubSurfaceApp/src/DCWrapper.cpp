#include "DCWrapper.h"

DCWrapper::DCWrapper()
{
    qDebug() << "Creating libdivecomputer context...";
    if (dc_context_new(&context) != DC_STATUS_SUCCESS) {
        qDebug() << "Failed to create context!";
        throw std::runtime_error("Failed to create context");
    }
    qDebug() << "Context created successfully.";
}

DCWrapper::~DCWrapper()
{
    qDebug() << "Cleaning up DCWrapper...";
    if (device) {
        qDebug() << "Closing device...";
        dc_device_close(device);
    }

    if (iostream) {
        qDebug() << "Closing IO stream...";
        dc_iostream_close(iostream);
    }

    if (context) {
        qDebug() << "Freeing context...";
        dc_context_free(context);
    }
}

bool DCWrapper::connect(const std::string& vendor,
                        const std::string& product)
{
    qDebug() << "Connecting to device:" << QString::fromStdString(vendor)<< "/" << QString::fromStdString(product);

    if (!openSerial()) {
        qDebug() << "Failed to open serial connection!";
        return false;
    }

    if (!findDescriptor(vendor, product)) {
        qDebug() << "Failed to find device descriptor!";
        return false;
    }

    if (dc_device_open(&device, context, descriptor, iostream)
        != DC_STATUS_SUCCESS) {
        qDebug() << "Failed to open device!";
        return false;
    }
    this->connected = true;

    qDebug() << "Device connected successfully.";
    return true;
}
bool DCWrapper::isConnected() const {
    return this->connected;
}

bool DCWrapper::openSerial()
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
            qDebug() << "USB device found:" << name;

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

bool DCWrapper::findDescriptor(const std::string& vendor,
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

void DCWrapper::updateSupportedDevices(){
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
    return;
}

typedef struct {
    QXmlStreamWriter *xml;
    unsigned int time;
} xml_context_t;


/* ------------------------------------------
   SAMPLE CALLBACK (profil)
------------------------------------------ */
void sample_callback(dc_sample_type_t type,
                    const dc_sample_value_t *value,
                    void *userdata)
{
    xml_context_t *ctx = (xml_context_t *)userdata;

    if (type == DC_SAMPLE_TIME) {
        ctx->time = value->time;
    }

    if (type == DC_SAMPLE_DEPTH) {
        ctx->xml->writeStartElement("sample");
        ctx->xml->writeAttribute("time", QString::number(ctx->time));
        ctx->xml->writeAttribute("depth", QString::number(value->depth));
        ctx->xml->writeEndElement();
    }

    return;
}


/* ------------------------------------------
   MAIN CALLBACK (1 plongée = 1 fichier)
------------------------------------------ */
int dive_callback(const unsigned char *data,
             unsigned int size,
             const unsigned char *fingerprint,
             unsigned int fsize,
             void *userdata)
{
    dc_device_t *device = (dc_device_t *)userdata;
    dc_parser_t *parser = NULL;

    // ---------------------------------------------------------
    // Préparer le dossier : AppDataLocation/dives
    // ---------------------------------------------------------
    QString appDataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(appDataPath);
    if (!dir.exists()) {
        dir.mkpath(".");  // crée AppDataLocation si nécessaire
    }
    if (!dir.exists("dives")) {
        dir.mkdir("dives"); // crée le sous-dossier dives
    }
    dir.cd("dives");

    // ---------------------------------------------------------
    // Nom de fichier = fingerprint en hex
    // ---------------------------------------------------------
    QByteArray fp_array((const char*)fingerprint, fsize);
    QString filename = dir.filePath(fp_array.toHex() + ".xml");

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)) {
        fprintf(stderr, "Failed to open file\n");
        return 1;
    }

    QXmlStreamWriter xml(&file);
    xml.setAutoFormatting(true);

    xml.writeStartDocument();
    xml.writeStartElement("dive");

    /* ---------------------------------------------------------
       Create parser
       --------------------------------------------------------- */
    if (dc_parser_new(&parser, device, data, size) != DC_STATUS_SUCCESS) {
        fprintf(stderr, "Parser error\n");
        return 1;
    }

    /* ---------------------------------------------------------
       METADATA
       --------------------------------------------------------- */
    xml.writeStartElement("metadata");

    unsigned int duration = 0;
    double maxdepth = 0.0;

    if (dc_parser_get_field(parser, DC_FIELD_DIVETIME, 0, &duration) == DC_STATUS_SUCCESS) {
        xml.writeTextElement("duration", QString::number(duration));
    }

    if (dc_parser_get_field(parser, DC_FIELD_MAXDEPTH, 0, &maxdepth) == DC_STATUS_SUCCESS) {
        xml.writeTextElement("maxdepth", QString::number(maxdepth));
    }

    xml.writeEndElement(); // metadata

    /* ---------------------------------------------------------
       PROFILE
       --------------------------------------------------------- */
    xml.writeStartElement("profile");

    xml_context_t ctx;
    ctx.xml = &xml;
    ctx.time = 0;

    dc_parser_samples_foreach(parser, sample_callback, &ctx);

    xml.writeEndElement(); // profile

    /* ---------------------------------------------------------
       END
       --------------------------------------------------------- */
    xml.writeEndElement(); // dive
    xml.writeEndDocument();

    dc_parser_destroy(parser);
    file.close();

    return 1;
}

void DCWrapper::importDives(){
    if (!isConnected()){
        qDebug() << "No device connected";
        return;
    }

    if (dc_device_foreach(device, dive_callback, device) == DC_STATUS_SUCCESS){
        qDebug() << "Error while importing dives";
        return;
    }

    qDebug() << "Dives imported successfully";
    return;
}
