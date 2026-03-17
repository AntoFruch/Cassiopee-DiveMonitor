#ifndef DCTOOLSWRAPPER_H
#define DCTOOLSWRAPPER_H

#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QStandardPaths>

#include <libdivecomputer/context.h>
#include <libdivecomputer/device.h>
#include <libdivecomputer/descriptor.h>
#include <libdivecomputer/iostream.h>
#include <libdivecomputer/serial.h>
#include <iostream>


class DCWrapper
{
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

    /** Liste des appareils supportés avec leurs modes de transports.
     *  Structure du JSON :
     * [
     *   {
     *     "vendor": "<Vendeur>",            // String: name of the manufacturer
     *     "products": [                        // Array: list of products for this vendor
     *       {
     *         "name": "<Produit>",         // String: product/model name
     *         "transports": [                  // Array: supported communication methods
     *           "<TransportMethod1>",          // e.g. "USB", "Bluetooth", "Serial"
     *           "<TransportMethod2>",
     *           ...
     *         ]
     *       },
     *       ...
     *     ]
     *   },
     *   ...
     * ]
     */
    QJsonArray supportedDevices;

    /** Stocke la liste des appareils supportées par libDC dans un objet JSON : supportedDevices
     */
    void updateSupportedDevices();

private:
    dc_context_t* context = nullptr;
    dc_iostream_t* iostream = nullptr;
    dc_device_t* device = nullptr;
    dc_descriptor_t* descriptor = nullptr;

    bool openSerial();
    bool findDescriptor(const std::string& vendor,
                        const std::string& product);
};

#endif // DCTOOLSWRAPPER_H
