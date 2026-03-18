#ifndef DCTOOLSWRAPPER_H
#define DCTOOLSWRAPPER_H

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QStandardPaths>
#include <QXmlStreamWriter>
#include <QString>
#include <QByteArray>

#include <libdivecomputer/context.h>
#include <libdivecomputer/device.h>
#include <libdivecomputer/descriptor.h>
#include <libdivecomputer/iostream.h>
#include <libdivecomputer/serial.h>
#include <libdivecomputer/parser.h>
#include <iostream>

#define DEBUG 1

typedef struct {
    double time;
    double temperature;
    double depth;
} dive_sample_t;

typedef struct {
    // FINGERPRINT
    QByteArray fingerprint;

    // META DONNEES
    unsigned int dive_time; //DC_FIELD_DIVETIME
    double max_depth;       //DC_FIELD_MAXDEPTH
    double avg_depth;       //DC_FIELD_AVGDEPTH
    double atmos_pressure;  //DC_FIELD_ATMOSPEHRIC
    double surface_temperature; //DC_FIELD_TEMPERATURE_SURFACE
    double min_temperature; //DC_FIELD_TEMPERATURE_MINIMUM
    double max_temperature; //DC_FIELD_TEMPERATURE_MAXIMUM

    // ECHANTILLONS
    QVector<dive_sample_t> samples;
} dive_t;

struct SampleCallbackContext {
    QVector<dive_sample_t> *samples;
    dive_sample_t current;
    bool has_data = false;
};

struct CallBackContext {
    dc_device_t *device;
    QVector<dive_t*> dives;
};

class DCWrapper
{
private:
    dc_context_t* context = nullptr;
    dc_iostream_t* iostream = nullptr;
    dc_device_t* device = nullptr;
    dc_descriptor_t* descriptor = nullptr;

    bool connected;

    /** Liste des appareils supportés avec leurs modes de transports.
     */
    QJsonArray supportedDevices;

    QVector<dive_t*> dives;

    bool openSerial();
    bool findDescriptor(const std::string& vendor,
                        const std::string& product);

public:
    /** Constructeur
     */
    DCWrapper();
    /** Destructeur
     */
    ~DCWrapper();

    /** Connecte un appareil à l'application.
     *  /!\ connexion série uniquement pour l'instant ! /!\
     * @param vendor : nom du vendeur / marque de l'appareil (ex: Mares)
     * @param product : nom du produit (ex: Quad Air)
     */
    bool connect(const std::string& vendor,
                 const std::string& product);

    bool isConnected() const;

    void disconnect();

    /** Stocke la liste des appareils supportées par libDC dans un objet JSON : supportedDevices
     */
    void updateSupportedDevices();

    QJsonArray getSupportedDives(){return supportedDevices;}

    void importDives();

    QVector<dive_t*> getDives(){return dives;}
};

#endif // DCTOOLSWRAPPER_H
