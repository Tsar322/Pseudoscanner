#ifndef SCANNER_IMG_PROC_H
#define SCANNER_IMG_PROC_H

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <exception>

namespace scanner_img_proc {
	
	std::vector<cv::Point> predetermine_quadrangle(cv::Mat input_img);

	std::vector<cv::Point2f> find_line_intersections_rho_theta(const std::vector<cv::Vec2f>& lines, cv::Size imageSize);

	bool line_intersection_rho_theta(cv::Vec2f line1, cv::Vec2f line2, cv::Point2f& intersection);

	template<typename PointType>
	cv::Mat create_point_mask(cv::Size size, const std::vector<PointType>& points, int radius);

	template<typename PointType>
	std::vector<PointType> choose_best_quadrangle(const std::vector<PointType>& input_points, cv::Size img_size);

	template<typename PointType, typename Iterator>
	void _pick_best_corners(std::vector<PointType>& points, std::vector<PointType>& best_points, Iterator begin, Iterator end, int& best_perimeter);

	template<typename PointType>
	void _analize_quadrangle(const std::vector<PointType>& points, std::vector<PointType>& best_points, int& best_perimeter);
}

#endif