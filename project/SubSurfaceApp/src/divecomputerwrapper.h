#ifndef DIVECOMPUTERWRAPPER_H
#define DIVECOMPUTERWRAPPER_H

#include <QObject>   // ✅ ADD THIS

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
#include <QtConcurrent>

#include <libdivecomputer/context.h>
#include <libdivecomputer/device.h>
#include <libdivecomputer/descriptor.h>
#include <libdivecomputer/iostream.h>
#include <libdivecomputer/serial.h>
#include <libdivecomputer/bluetooth.h>
#include <libdivecomputer/parser.h>
#include <iostream>
#include <stdlib.h>



#include "DiveDataStructure.h"
#include "divelistmodel.h"
#include "samplemodel.h"

/** Wrapper de libDiveComputer, construction par DiveComputerWrapper::instance() (Singleton)
 */
class DiveComputerWrapper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QJsonArray supportedDevices READ getSupportedDevices NOTIFY supportedDevicesChanged)
    Q_PROPERTY(QStringList availablePorts READ getAvailablePorts NOTIFY availablePortsChanged)
    Q_PROPERTY(bool isImporting READ isImporting NOTIFY isImportingChanged)

public:
    /** Méthode pour accèder à l'instance unique de DiveComputerWrapper.
     *
     * @return l'instance.
     */
    static DiveComputerWrapper& instance();

    /** Destructeur
     *
     */
    ~DiveComputerWrapper();

    /** Etablit la connection à l'appareil spécifié par les paramètres
     *
     * @param vendor vendeur du produit
     * @param product produit en question
     * @param connectionMode mode de communication ( bluetooth, serial, ...)
     * @param port port correspondant à l'ordianteur de plongée branché ou connecté à l'appareil
     * @return true si la connection est réussie, false sinon.
     */
    Q_INVOKABLE bool connectToDevice(const QString& vendor,
                                     const QString& product,
                                     const QString& connectionMode,
                                     const QString& port);

    /** Deconnection de l'appareil actuellement connecté
     */
    Q_INVOKABLE void disconnectDevice();

    /** Actualise la liste des appareils supportés par libdivecomputer (au cas ou ca change)
     */
    Q_INVOKABLE void updateSupportedDevices();
    QJsonArray getSupportedDevices() const { return supportedDevices; }

    /** Lance une routine parallèle appelant importDives(), ainsi l'application n'est pas bloquée
     *  par une importation qui peut être un petit peu longue.
     */
    Q_INVOKABLE void importDivesAsync();

    /** Actualise la liste des ports disponibles pour le mode de transport passé en arguments
     *
     * @param transport
     */
    Q_INVOKABLE void refreshPorts(QString transport);
    QStringList getAvailablePorts() const { return availablePorts; }


    bool isConnected() const { return connected; }
    bool isImporting() const { return m_isImporting; }

signals:
    void connectedChanged();
    void supportedDevicesChanged();
    void availablePortsChanged();
    void importationDone();
    void isImportingChanged();

private:
    // constructeur privé et interdictions de copie
    explicit DiveComputerWrapper(QObject *parent = nullptr);
    DiveComputerWrapper(const DiveComputerWrapper&) = delete;
    DiveComputerWrapper& operator=(const DiveComputerWrapper&) = delete;

    /** Instance de la base de donnée
     */
    DiveDatabase* db;

    /** vendeur de l'appareil de plongée actuellement connecté.
     */
    QString vendor;
    /** produit de l'appareil de plongée actuellement connecté.
     */
    QString product;
    /** dernier fingerprint trouvé en base pour l'appreil connecté si dessus.
     */
    QByteArray last_fingerprint;

    // Objets de fonctionnements pour libDC
    dc_context_t* context = nullptr;
    dc_iostream_t* iostream = nullptr;
    dc_device_t* device = nullptr;
    dc_descriptor_t* descriptor = nullptr;

    bool connected = false;
    bool m_isImporting = false;

    /** Liste des appareils supportés.
     */
    QJsonArray supportedDevices;
    /** Liste des ports trouvés.
     */
    QStringList availablePorts;

    /** Ouvre une communication série sur le port renseigné. -> en théorie il faudrait rendre ca générique pour ouvrir
     *                                                          une connunication pour tout les modes de transprts.
     *
     * @param port
     * @return true si la connection est résussi, false sinon.
     */
    bool openSerial(const QString& port);

    /** Trouve le descripteur d'ordinateur de plongée qui correspond au modèle renseigné.
     *
     * @param vendor
     * @param product
     * @return
     */
    bool findDescriptor(const std::string& vendor,
                        const std::string& product);

    /** Actualise la valeur de m_isImporting.
     *
     * @param importing
     */
    void setImporting(bool importing) {
        if (m_isImporting == importing) return;
        m_isImporting = importing;
        emit isImportingChanged();
    }

    /** Importe les plongées présentes dans l'appareil actuellement connecté et les insère en base
     */
    void importDives();
};

#endif // DIVECOMPUTERWRAPPER_H
