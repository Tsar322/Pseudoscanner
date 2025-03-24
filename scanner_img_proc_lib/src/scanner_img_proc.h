#ifndef SCANNER_IMG_PROC_H
#define SCANNER_IMG_PROC_H

#include <iostream>
#include <algorithm>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d.hpp>
#include <exception>
#include <cmath>

#include <Eigen/Dense>

namespace scanner_img_proc {

	enum class Format;

	double get_aspect_ratio(Format format, bool is_portrait = true);

	double round_aspect_ratio_to_format(double aspect_ratio, double tolerance, bool abs = true);
	
	std::vector<cv::Point> predetermine_quadrangle(const cv::Mat& input_img);

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



	//top-left, bottom-left, bottom-right, top-right
	template<typename PointType>
	bool distribute_points(std::vector<PointType>& points);

	template<typename PointType>
	bool is_landscape(const std::vector<PointType>& rectangle);

	//will return NaN if the quadrangle is a trapezoid, but not a parallelogram
	template<typename PointType>
	double get_aspect_ratio(const std::vector<PointType>& quadrangle, const PointType& origin, double angle_tolerance = 3.0);

	//will return NaN if the quadrangle is a trapezoid or the original shape is not a rectangle
	template<typename PointType>
	double get_aspect_ratio_no_parallel(const std::vector<PointType>& quadrangle, PointType origin);

	template<typename PointType>
	double get_aspect_ratio_parallel(const std::vector<PointType>& quadrangle);

	template<typename PointType>
	double calculate_line_angle_degrees(const PointType& a, const PointType& b);

	template<typename PointType>
	void transform_perspective(cv::InputArray src, cv::OutputArray dst, const std::vector<PointType>& quadrangle, double aspect_ratio = 0.71, int large_side = 0);
	template<typename PointType>
	void transform_perspective(cv::InputArray src, cv::OutputArray dst, const std::vector<PointType>& quadrangle, cv::Size dsize);
}

#endif