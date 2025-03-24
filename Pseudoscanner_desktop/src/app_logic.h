#pragma once

#include <QObject>
#include <QStringList>
#include <QList>
#include <QImage>
#include <QColorTransform>
#include <QDebug>
#include <QAbstractListModel>
#include <QUrl>
#include <QDir>
#include <QtConcurrent/QtConcurrentRun>
#include <QMetaObject>

#include <QPixmap>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

#include <scanner_img_proc.h>

#include "image_data.h"

class AppLogic : public QAbstractListModel {
	Q_OBJECT
	Q_PROPERTY(int cursor READ cursor WRITE setCursor NOTIFY cursorDataChanged)
public:
	enum Roles {
		OriginalImageRole = Qt::UserRole + 1,
		ProcessedImageRole,
		NameRole
	};
	QHash<int, QByteArray> roleNames() const;
	Q_ENUM(Roles)

	explicit AppLogic(QObject* parent = nullptr);

	Q_INVOKABLE void openImages(const QList<QString>& paths);

	Q_INVOKABLE QImage originalImage(int i) const;
	Q_INVOKABLE QImage processedImage(int i) const;
	Q_INVOKABLE QString path(int i) const;

	int cursor() const;
	int imageDataSize() const;
	Q_INVOKABLE void setCursor(int index);
	Q_INVOKABLE int getCorner(int index, int corner_index, bool is_x) const;
	Q_INVOKABLE void setCorner(int index, int corner_index, int x, int y);
	Q_INVOKABLE void predetectCorners(int index);
	Q_INVOKABLE void applyTransform(int index);
	Q_INVOKABLE bool saveImage(int index, const QUrl& targetUrl) const;

	// QAbstractListModel overrides
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayPropertyRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
	QList<ImageData> imageData;
	int m_cursor{-1};
signals:
	void cursorDataChanged();
	void processedImageChanged();
	void cornersPredetected(int index);
};


QImage cvMatToQImage(const cv::Mat& mat);