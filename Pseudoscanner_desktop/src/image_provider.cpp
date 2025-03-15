#include "image_provider.h"

ImageProvider::ImageProvider(const AppLogic* appLogic_)
	: QQuickImageProvider(QQuickImageProvider::Image), appLogic(appLogic_) {

}

QImage ImageProvider::requestImage(const QString& id, QSize* size, const QSize& requestedSize) {
	QStringList parts = id.split('/');
	if (parts.size() == 2) {
		bool ok;
		int index = parts[0].toInt(&ok);
		if (!ok || index < 0 || index >= appLogic->imageDataSize())
			return QImage();

		QString type = parts[1];

		QImage image;
		if (type == "original") {
			image = appLogic->originalImage(index);
		}
		else if (type == "processed") {
			image = appLogic->processedImage(index);
		}

		if (!image.isNull()) {
			if (size)
				*size = image.size();
			if (requestedSize.width() > 0 && requestedSize.height() > 0) {
				image = image.scaled(requestedSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}
			return image;
		}
	}

	return QImage();
}