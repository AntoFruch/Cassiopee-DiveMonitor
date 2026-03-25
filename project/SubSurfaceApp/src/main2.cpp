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

    DCWrapper dc(&db);
    dc.connect("Mares", "Quad Air");

    dc.importDives();

    return app.exec();
}
