#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSettings>
#include <QOperatingSystemVersion>

#include <QDebug>
#include "DCWrapper.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);


    qDebug() << "init";

    DCWrapper dc;
    dc.connect("Mares", "Quad Air");
    dc.importDives();

    QVector<dive_t*> dives = dc.getDives();
    for (auto it = dives[0]->samples.cbegin();
         it != dives[0]->samples.cend(); ++it)
    {
        const dive_sample_t &sample = *it;
        qDebug() << "time:" << sample.time
                 << "depth:" << sample.depth
                 << "temp:" << sample.temperature;
    }

    return app.exec();
}
