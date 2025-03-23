#pragma once

#include <QImage>

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

	std::vector<cv::Point> documentCorners;
};