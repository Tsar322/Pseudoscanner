#include <iostream>
#include <assert.h>
#include "scanner_img_proc.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include "test.h"

template<typename PointType>
void guarantee_counter_clockwise(std::vector<PointType>& contour) {
	if (cv::contourArea(contour, true) < 0) {
		std::reverse(contour.begin(), contour.end());
	}
}

////top-left, bottom-left, bottom-right, top-right
//template<typename PointType>
//bool distribute_points(std::vector<PointType>& points) {
//	//std::assert(points.size() == 4);
//	std::vector<PointType> v_based_sorted(4);
//	std::vector<PointType> h_based_result(4);
//	for (std::size_t i = 0; i < 4; ++i) {
//		v_based_sorted[i] = points[i];
//		h_based_result[i] = points[i];
//	}
//
//	//v-based sorting
//	std::sort(v_based_sorted.begin(), v_based_sorted.end(), [](const PointType& a, const PointType& b) {
//		return a.y < b.y; // from top to bottom
//		});
//	std::vector<PointType> v_based_result{v_based_sorted[0], v_based_sorted[2], v_based_sorted[3], v_based_sorted[1]};
//
//	if (v_based_result[0].x > v_based_result[3].x) {
//		std::swap(v_based_result[0], v_based_result[3]);
//	}
//	if (v_based_result[1].x > v_based_result[2].x) {
//		std::swap(v_based_result[1], v_based_result[2]);
//	}
//
//	//h-based sorting
//	std::sort(h_based_result.begin(), h_based_result.end(), [](const PointType& a, const PointType& b) {
//		return a.x < b.x; // from left to right
//		});
//
//	if (h_based_result[0].y > h_based_result[1].y) {
//		std::swap(h_based_result[0], h_based_result[1]);
//	}
//	if (h_based_result[2].y < h_based_result[3].y) {
//		std::swap(h_based_result[2], h_based_result[3]);
//	}
//
//	// unambiguty check
//	if (h_based_result == v_based_result) {
//		points = h_based_result;
//		return true;
//	}
//	
//	return false;
//}

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
	match_dots
}

int main(){

	std::vector<std::string> files = test::get_files_in_directory("../../../../test_images");
	std::vector<cv::Mat> images = test::load_images(files);
	for (auto& img : images) {
		std::vector<cv::Point> document_polygon = scanner_img_proc::predetermine_quadrangle(img);
		cv::polylines(img, document_polygon, true, cv::Scalar(0, 255, 0), 30);

		cv::Size img_size = img.size();
		//std::vector<cv::Point> img_corners{ cv::Point(0, 0), cv::Point(0, img_size.height - 1), cv::Point(img_size.width - 1, 0), cv::Point(img_size.width - 1, img_size.height - 1) };
		std::vector<cv::Point> img_corners{cv::Point(img_size.width - 1, 0), cv::Point(img_size.width - 1, img_size.height - 1), cv::Point(0, img_size.height - 1), cv::Point(0, 0)};
		//std::vector<cv::Point> img_corners_sorted;
		//cv::convexHull(img_corners, img_corners_sorted);
		if (scanner_img_proc::distribute_points(document_polygon)) {
			cv::putText(img, "0", document_polygon[0], 1, 15, cv::Scalar(0, 0, 255), 30);
			cv::putText(img, "1", document_polygon[1], 1, 15, cv::Scalar(0, 0, 255), 30);
			cv::putText(img, "2", document_polygon[2], 1, 15, cv::Scalar(0, 0, 255), 30);
			cv::putText(img, "3", document_polygon[3], 1, 15, cv::Scalar(0, 0, 255), 30);
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