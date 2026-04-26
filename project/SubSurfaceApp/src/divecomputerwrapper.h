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

class DiveComputerWrapper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QJsonArray supportedDevices READ getSupportedDevices NOTIFY supportedDevicesChanged)
    Q_PROPERTY(QStringList availablePorts READ getAvailablePorts NOTIFY availablePortsChanged)
    Q_PROPERTY(bool isImporting READ isImporting NOTIFY isImportingChanged)

public:
    explicit DiveComputerWrapper(QObject *parent = nullptr);
    ~DiveComputerWrapper();

    Q_INVOKABLE bool connectToDevice(const QString& vendor,
                                     const QString& product,
                                     const QString& connectionMode,
                                     const QString& port);

    Q_INVOKABLE void disconnectDevice();

    Q_INVOKABLE void updateSupportedDevices();

    Q_INVOKABLE void importDives();
    Q_INVOKABLE void importDivesAsync();

    Q_INVOKABLE void refreshPorts(QString transport);
    QStringList getAvailablePorts() const { return availablePorts; }


    bool isConnected() const { return connected; }
    bool isImporting() const { return m_isImporting; }

    QJsonArray getSupportedDevices() const { return supportedDevices; }

signals:
    void connectedChanged();
    void supportedDevicesChanged();
    void availablePortsChanged();
    void importationDone();
    void isImportingChanged();

private:
    DiveDatabase* db;

    QString vendor;
    QString product;
    QByteArray last_fingerprint;

    dc_context_t* context = nullptr;
    dc_iostream_t* iostream = nullptr;
    dc_device_t* device = nullptr;
    dc_descriptor_t* descriptor = nullptr;

    bool connected = false;
    bool m_isImporting = false;

    QJsonArray supportedDevices;
    QStringList availablePorts;

    bool openSerial(const QString& port);
    bool findDescriptor(const std::string& vendor,
                        const std::string& product);
    void setImporting(bool importing) {
        if (m_isImporting == importing) return;
        m_isImporting = importing;
        emit isImportingChanged();
    }
};

#endif // DIVECOMPUTERWRAPPER_H
