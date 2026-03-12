#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QSettings>
#include <QOperatingSystemVersion>

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

    QSettings settings;

    static constexpr auto styleKey = "style";

    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(settings.value(styleKey).toString());
    }

    // première exécution : stocker un style par défaut
    if (settings.value(styleKey).toString().isEmpty()) {
        settings.setValue(styleKey, QQuickStyle::name());
    }

    // Liste des styles Built-in à laquelle on rajoute les styles de l'OS en question
    QStringList builtInStyles = {
        "Basic",
        "Fusion",
        "Imagine",
        "Material",
        "Universal",
        "FluentWinUI3"
    };
    if constexpr (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::MacOS)
        builtInStyles << "macOS" << "iOS";
    else if constexpr (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::IOS)
        builtInStyles << "iOS";
    else if constexpr (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::Windows)
        builtInStyles << "Windows";


    // Props à passer au QML initialement
    engine.setInitialProperties({
        { "builtInStyles", builtInStyles }
    });


    // Generation du template QML
    engine.loadFromModule("SubSurfaceApp", "Main");

    return app.exec();
}
