#pragma once

#include <QObject>
#include <QStringList>
#include <QList>
#include <QImage>
#include <QColorTransform>
#include <QDebug>

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>

class ImageData {
public:
	explicit ImageData(QImage originalImage, QImage processedImage, QString path, cv::Mat img);
	QImage originalImage;
	QImage processedImage;
	QString path;
	cv::Mat img;
};

class AppLogic : public QObject {
	Q_OBJECT
	Q_PROPERTY(int cursor READ cursor WRITE setCursor NOTIFY cursorDataChanged)
public:
	explicit AppLogic(QObject* parent = nullptr);

	Q_INVOKABLE void openImages(const QList<QString>& paths);

	Q_INVOKABLE QImage originalImage(int i) const;
	Q_INVOKABLE QImage processedImage(int i) const;
	Q_INVOKABLE QString path(int i) const;

	int cursor() const;
	int imageDataSize() const;
	void setCursor(int cursor);

private:
	QList<ImageData> imageData;
	int m_cursor{};
signals:
	void cursorDataChanged();
};

//class Worker : public QObject {
//	Q_OBJECT
//public:
//	explicit Worker(QObject* parent = nullptr);
//public slots:
//private:
//	QStringList file_paths;
//};


QImage cvMatToQImage(const cv::Mat& mat);