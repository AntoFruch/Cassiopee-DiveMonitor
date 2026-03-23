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

#include "divedatabase.h"

#define DEBUG 1

class DCWrapper
{
private:
    DiveDatabase *db;

    dc_context_t* context = nullptr;
    dc_iostream_t* iostream = nullptr;
    dc_device_t* device = nullptr;
    dc_descriptor_t* descriptor = nullptr;

    bool connected;

    /** Liste des appareils supportés avec leurs modes de transports.
     */
    QJsonArray supportedDevices;


    bool openSerial();
    bool findDescriptor(const std::string& vendor,
                        const std::string& product);

public:
    DCWrapper(DiveDatabase *db);
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


};

#endif // DCTOOLSWRAPPER_H
