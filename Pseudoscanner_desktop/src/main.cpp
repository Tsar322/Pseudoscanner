#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "app_logic.h"
#include "image_provider.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AppLogic appLogic;
    engine.rootContext()->setContextProperty("app", &appLogic);

    engine.addImageProvider("images", new ImageProvider(&appLogic));


    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(QUrl("qrc:/Main.qml"));

    return app.exec();
}
