#pragma once

#include <QObject>
#include <QQuickImageProvider>
#include <QImage>
#include <opencv2/core.hpp>

#include "app_logic.h"

class ImageProvider : public QQuickImageProvider {

public:
	explicit ImageProvider(const AppLogic* appLogic_);

	QImage requestImage(const QString& id, QSize* size, const QSize& requestedSize) override;

private:
	const AppLogic* appLogic;
};