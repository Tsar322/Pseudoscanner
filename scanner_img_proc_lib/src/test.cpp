#include <iostream>
#include <assert.h>
#include "scanner_img_proc.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>

#include <Eigen/Dense>
#include "test.h"





template<typename PointType>
cv::Mat transform_perspective(const cv::Mat& img, const std::vector<PointType>& polygon, double aspect_ratio = 1.414, bool album = false) {
	cv::Size dsize;
	if (album) {
		dsize.width = std::max(img.cols, img.rows * aspect_ratio);
		dsize.height = std::max(img.rows, width / aspect_ratio);
	}
	else {
		dsize.width = std::max(img.cols, img.rows / aspect_ratio);
		dsize.height = std::max(img.rows, width * aspect_ratio);
	}

	cv::Mat dst(dsize, img.type());
	//match_dots
	return dst;
}

//cv::Size get_transformed_size(const cv::Size& size, double aspect_ratio = 1.414, bool landscape = false) {
//	int long_side = std::max(size.height, size.width);
//}

int main(){

	//std::cout << scanner_img_proc::round_aspect_ratio_to_format(1 / 1.4, 0.000001, false);

	std::vector<std::string> files = test::get_files_in_directory("../../../../test_images");
	std::vector<cv::Mat> images = test::load_images(files);
	for (auto& img : images) {
		cv::Mat dst;
		std::vector<cv::Point> document_polygon = scanner_img_proc::predetermine_quadrangle(img);
		cv::polylines(img, document_polygon, true, cv::Scalar(0, 255, 0), 30);

		cv::Size img_size = img.size();
		std::vector<cv::Point> img_corners{cv::Point(img_size.width - 1, 0), cv::Point(img_size.width - 1, img_size.height - 1), cv::Point(0, img_size.height - 1), cv::Point(0, 0)};
		if (scanner_img_proc::distribute_points(document_polygon)) {
			cv::putText(img, "0", document_polygon[0], 1, 15, cv::Scalar(0, 0, 255), 30);
			cv::putText(img, "1", document_polygon[1], 1, 15, cv::Scalar(0, 0, 255), 30);
			cv::putText(img, "2", document_polygon[2], 1, 15, cv::Scalar(0, 0, 255), 30);
			cv::putText(img, "3", document_polygon[3], 1, 15, cv::Scalar(0, 0, 255), 30);
			//cv::Point origin(img.cols / 2, img.rows / 2);
			cv::Point origin(1734, 2312);
			double aspect_ratio = scanner_img_proc::get_aspect_ratio(document_polygon, origin);
			//cv::putText(img, std::to_string(aspect_ratio), cv::Point(img.cols / 2, img.rows / 2), 1, 15, cv::Scalar(0, 0, 255), 30);
			scanner_img_proc::transform_perspective(img, dst, document_polygon, aspect_ratio);
			int a = 0;
		}
		else {
			cv::circle(img, cv::Point(img.cols / 2, img.rows / 2), 100, cv::Scalar(255, 255), 50);
		}


		//cv::Mat homography_mat = cv::findHomography(document_polygon, img_corners);
		//cv::Mat dst(img.size(), img.type());
		//cv::warpPerspective(img, dst, homography_mat, img.size());
	}
	
	
	return 0;
}