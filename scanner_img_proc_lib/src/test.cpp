#include <iostream>
#include "scanner_img_proc.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include "test.h"


int main(){

	std::vector<std::string> files = test::get_files_in_directory("../../../../test_images");
	std::vector<cv::Mat> images = test::load_images(files);
	for (auto& img : images) {
		std::vector<cv::Point> document_polygon = scanner_img_proc::predetermine_quadrangle(img);
		cv::polylines(img, document_polygon, true, cv::Scalar(0, 0, 255), 20);
	}
	
	
	return 0;
}