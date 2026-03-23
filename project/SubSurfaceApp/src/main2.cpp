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

    auto &db = DiveDatabase::instance("/Users/antonin/Desktop/Cours/2A/Cassiopee-SubSurface/project/SubSurfaceApp/resources/database.db");
    DiveData dive;

    dive.fingerprint = QByteArray("example_fingerprint");

    dive.dive_time = 1800; // 30 minutes
    dive.max_depth = 30.5;
    dive.avg_depth = 18.2;
    dive.atmos_pressure = 1013.25;
    dive.surface_temperature = 22.0;
    dive.min_temperature = 18.5;
    dive.max_temperature = 22.0;

    // Ajout de quelques points de plongée
    dive.entries.append({5.0, 21.5, 0});     // début
    dive.entries.append({15.0, 20.0, 300});  // 5 min
    dive.entries.append({30.5, 18.5, 900});  // profondeur max
    dive.entries.append({10.0, 19.5, 1500}); // remontée
    dive.entries.append({0.0, 22.0, 1800});  // fin

    db.insertDive(dive);

    return app.exec();
}
