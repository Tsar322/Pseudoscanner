#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include "app_logic.h"
#include "image_provider.h"

class ColorImageProvider : public QQuickImageProvider
{
public:
    ColorImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Pixmap)
    {
    }

    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override
    {
        int width = 100;
        int height = 50;

        if (size)
            *size = QSize(width, height);
        QPixmap pixmap(requestedSize.width() > 0 ? requestedSize.width() : width,
            requestedSize.height() > 0 ? requestedSize.height() : height);
        pixmap.fill(QColor(id).rgba());
        return pixmap;
    }
};

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    AppLogic appLogic;
    engine.rootContext()->setContextProperty("app", &appLogic);

    engine.addImageProvider("images", new ImageProvider(&appLogic));
    engine.addImageProvider("colors", new ColorImageProvider());

    bool provider_exist = engine.imageProvider("images");

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.load(QUrl("qrc:/Main.qml"));

    return app.exec();
}
