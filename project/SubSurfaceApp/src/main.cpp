#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QOperatingSystemVersion>
#include <QQmlContext>

#include <QDebug>
#include "divecomputerwrapper.h"
#include "divelistmodel.h"

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


    engine.rootContext()->setContextProperty("dcWrapper", &DiveComputerWrapper::instance());
    qmlRegisterType<SampleModel>("DiveMonitorCustom", 1, 0, "SampleModel");
    qmlRegisterType<DiveListModel>("DiveMonitorCustom", 1, 0, "DiveListModel");

    // Generation du template QML
    engine.loadFromModule("SubSurfaceApp", "Main");

    return app.exec();
}
