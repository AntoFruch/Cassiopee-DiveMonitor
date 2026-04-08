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

#include <libdivecomputer/context.h>
#include <libdivecomputer/device.h>
#include <libdivecomputer/descriptor.h>
#include <libdivecomputer/iostream.h>
#include <libdivecomputer/serial.h>
#include <libdivecomputer/parser.h>
#include <iostream>
#include <stdlib.h>



#include "DiveDataStructure.h"
#include "divelistmodel.h"
#include "samplemodel.h"

class DiveComputerWrapper : public QObject   // ✅ inherit QObject
{
    Q_OBJECT   // ✅ REQUIRED

    // 🔥 Expose properties to QML
    Q_PROPERTY(bool connected READ isConnected NOTIFY connectedChanged)
    Q_PROPERTY(QJsonArray supportedDevices READ getSupportedDevices NOTIFY supportedDevicesChanged)
    Q_PROPERTY(DiveListModel* divesModel READ getListModel NOTIFY divesModelChanged)
    Q_PROPERTY(SampleModel* samplesModel READ getSamplesModel NOTIFY samplesModelChanged)

public:
    explicit DiveComputerWrapper(QObject *parent = nullptr); // ✅ updated ctor
    ~DiveComputerWrapper();

    // 🔥 Make callable from QML
    Q_INVOKABLE bool connectToDevice(const QString& vendor,
                                     const QString& product);

    Q_INVOKABLE void disconnectDevice();

    Q_INVOKABLE void updateSupportedDevices();

    Q_INVOKABLE void importDives();

    Q_INVOKABLE void loadAllDives();
    DiveListModel* getListModel() const { return divesModel; }

    Q_INVOKABLE void loadDiveEntries(int id);
    SampleModel* getSamplesModel() const { return samplesModel; }


    bool isConnected() const { return connected; }

    QJsonArray getSupportedDevices() const { return supportedDevices; }

signals:
    void connectedChanged();
    void supportedDevicesChanged();
    void divesImported();
    void divesModelChanged();
    void samplesModelChanged();

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

    QJsonArray supportedDevices;
    DiveListModel* divesModel;
    SampleModel* samplesModel;

    bool openSerial();
    bool findDescriptor(const std::string& vendor,
                        const std::string& product);
};

#endif // DIVECOMPUTERWRAPPER_H
