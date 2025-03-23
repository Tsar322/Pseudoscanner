#include "image_data.h"

ImageData::ImageData(QImage originalImage_, QImage processedImage_, QString path_, cv::Mat img_)
    :originalImage(originalImage_), processedImage(processedImage_), path(path_), img(img_),
    documentCorners{ cv::Point(0, 0), cv::Point(0, img_.rows - 1), cv::Point(img_.cols - 1, img_.rows - 1), cv::Point(img_.cols - 1, 0) } {

}