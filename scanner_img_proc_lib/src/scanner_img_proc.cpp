#include "scanner_img_proc.h"

namespace scanner_img_proc {

	enum class Format {
		A4,
		Letter,
		Legal,
		A3,
		A5,
		SixteenByNine, // 16:9
		FourByThree,  // 4:3
		Custom
	};

	double get_aspect_ratio(Format format) {
		switch (format)
		{
		case scanner_img_proc::Format::A4:
			return 210.0 / 297.0;
		case scanner_img_proc::Format::Letter:
			return 8.5 / 11.0;
		case scanner_img_proc::Format::Legal:
			return 8.5 / 14.0;
		case scanner_img_proc::Format::A3:
			return 297.0 / 420.0;
		case scanner_img_proc::Format::A5:
			return 148.0 / 210.0;
		case scanner_img_proc::Format::SixteenByNine:
			return 16.0 / 9.0;
		case scanner_img_proc::Format::FourByThree:
			return 4.0 / 3.0;
		case scanner_img_proc::Format::Custom:
			return std::nan("");
		default:
			return std::nan("");
		}
	}


	std::vector<cv::Point> predetermine_quadrangle(cv::Mat input_img)
	{
		cv::Mat img;
		cv::GaussianBlur(input_img, img, cv::Size(5, 5), 0);
		cv::Mat gray;
		cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
		cv::Mat adaptive;
		cv::adaptiveThreshold(gray, adaptive, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 51, 5);

		int diagonal = std::sqrt(img.rows * img.rows + img.cols * img.cols);

		std::vector<cv::Vec2f> lines;
		cv::HoughLines(adaptive, lines, 1, CV_PI / 360, diagonal / 3);

		std::vector<cv::Point2f> intersections = find_line_intersections_rho_theta(lines, img.size());
		cv::Mat mask = create_point_mask(img.size(), intersections, diagonal / 128);

		std::vector<cv::Point> corners;
		cv::goodFeaturesToTrack(gray, corners, 12, 0.01, diagonal / 5, mask, 3);

		std::vector<cv::Point> supposed_quadrangle = choose_best_quadrangle(corners, img.size());

		return supposed_quadrangle;
	}

	std::vector<cv::Point2f> find_line_intersections_rho_theta(const std::vector<cv::Vec2f>& lines, cv::Size imageSize) {
		std::vector<cv::Point2f> intersections;

		for (size_t i = 0; i < lines.size(); ++i) {
			for (size_t j = i + 1; j < lines.size(); ++j) {
				cv::Point2f intersection;
				if (line_intersection_rho_theta(lines[i], lines[j], intersection)) {

					// Validation checks:
					if (intersection.x >= 0 && intersection.x < imageSize.width &&
						intersection.y >= 0 && intersection.y < imageSize.height) {
						intersections.push_back(intersection);
					}
				}
			}
		}

		return intersections;
	}

	bool line_intersection_rho_theta(cv::Vec2f line1, cv::Vec2f line2, cv::Point2f& intersection) {
		float rho1 = line1[0], theta1 = line1[1];
		float rho2 = line2[0], theta2 = line2[1];

		double det = cos(theta2) * sin(theta1) - cos(theta1) * sin(theta2);

		if (abs(det) < 1e-6) { // Use a small tolerance value
			return false; // Lines are (nearly) parallel
		}

		intersection.x = (sin(theta1) * rho2 - sin(theta2) * rho1) / det;
		intersection.y = (cos(theta2) * rho1 - cos(theta1) * rho2) / det;

		return true;
	}

	template<typename PointType>
	cv::Mat create_point_mask(cv::Size size, const std::vector<PointType>& points, int radius) {
		cv::Mat mask(size, CV_8UC1, cv::Scalar(0));
		for (const auto& point : points) {
			cv::circle(mask, point, radius, cv::Scalar(255), -1);
		}
		return mask;
	}

	template<typename PointType>
	std::vector<PointType> choose_best_quadrangle(const std::vector<PointType>& input_points, cv::Size img_size) {
		// filtering points
		std::vector<PointType> points;
		cv::convexHull(input_points, points);

		int best_perimeter = 0;
		std::vector<PointType> best_quadrangle_points;

		std::size_t size = points.size();
		// size < 4 cases
		if (size < 4) {
			std::vector<PointType> img_corners{ PointType(0, 0), PointType(0, img_size.height - 1), PointType(img_size.width - 1, 0), PointType(img_size.width - 1, img_size.height - 1) };
			_pick_best_corners(points, best_quadrangle_points, img_corners.begin(), img_corners.end(), best_perimeter);
			return best_quadrangle_points;
		}
		// size < 4 cases end

		for (std::size_t i1 = 0; i1 < size; ++i1) {
			for (std::size_t i2 = i1 + 1; i2 < size; ++i2) {
				for (std::size_t i3 = i2 + 1; i3 < size; ++i3) {
					for (std::size_t i4 = i3 + 1; i4 < size; ++i4) {
						_analize_quadrangle(std::vector<PointType>{points[i1], points[i2], points[i3], points[i4]}, best_quadrangle_points, best_perimeter);
					}
				}
			}
		}
		return best_quadrangle_points;
	}

	template<typename PointType, typename Iterator>
	void _pick_best_corners(std::vector<PointType>& points, std::vector<PointType>& best_points, Iterator begin, Iterator end, int& best_perimeter) {
		if (points.size() < 4) {
			while (begin != end) {
				points.push_back(*begin);
				++begin;
				_pick_best_corners(points, best_points, begin, end, best_perimeter);
				points.pop_back();
			}
		}
		else
		{
			std::vector<PointType> convex;
			cv::convexHull(points, convex);
			if (convex.size() < 4)
				return;
			int perimeter = cv::arcLength(convex, true);
			if (perimeter > best_perimeter) {
				best_perimeter = perimeter;
				best_points = convex;
			}
		}
	}

	template<typename PointType>
	void _analize_quadrangle(const std::vector<PointType>& points, std::vector<PointType>& best_points, int& best_perimeter) {
		std::vector<PointType> convex;
		cv::convexHull(points, convex);
		int perimeter = cv::arcLength(convex, true);
		if (perimeter > best_perimeter) {
			best_perimeter = perimeter;
			best_points = convex;
		}
	}


	//top-left, bottom-left, bottom-right, top-right
	template<typename PointType>
	bool distribute_points(std::vector<PointType>& points) {
		//std::assert(points.size() == 4);
		std::vector<PointType> v_based_sorted(4);
		std::vector<PointType> h_based_result(4);
		for (std::size_t i = 0; i < 4; ++i) {
			v_based_sorted[i] = points[i];
			h_based_result[i] = points[i];
		}

		//v-based sorting
		std::sort(v_based_sorted.begin(), v_based_sorted.end(), [](const PointType& a, const PointType& b) {
			return a.y < b.y; // from top to bottom
			});
		std::vector<PointType> v_based_result{ v_based_sorted[0], v_based_sorted[2], v_based_sorted[3], v_based_sorted[1] };

		if (v_based_result[0].x > v_based_result[3].x) {
			std::swap(v_based_result[0], v_based_result[3]);
		}
		if (v_based_result[1].x > v_based_result[2].x) {
			std::swap(v_based_result[1], v_based_result[2]);
		}

		//h-based sorting
		std::sort(h_based_result.begin(), h_based_result.end(), [](const PointType& a, const PointType& b) {
			return a.x < b.x; // from left to right
			});

		if (h_based_result[0].y > h_based_result[1].y) {
			std::swap(h_based_result[0], h_based_result[1]);
		}
		if (h_based_result[2].y < h_based_result[3].y) {
			std::swap(h_based_result[2], h_based_result[3]);
		}

		// unambiguty check
		if (h_based_result == v_based_result) {
			points = h_based_result;
			return true;
		}

		return false;
	}
	template bool distribute_points<cv::Point>(std::vector<cv::Point>& points);

	template<typename PointType>
	bool is_landscape(const std::vector<PointType>& rectangle) {
		double l = cv::norm(rectangle[1] - rectangle[0]);
		double r = cv::norm(rectangle[2] - rectangle[3]);
		double t = cv::norm(rectangle[3] - rectangle[0]);
		double b = cv::norm(rectangle[2] - rectangle[1]);
		double v = abs(l * r);
		double h = abs(t * b);
		return h > v;
	}
	template bool is_landscape<cv::Point>(const std::vector<cv::Point>& rectangle);

	template<typename PointType>
	double calculate_line_angle_degrees(const PointType& a, const PointType& b) {
		return std::atan2(b.y - a.y, b.x - a.x) * 180.0 / CV_PI;
	}
	template double calculate_line_angle_degrees<cv::Point>(const cv::Point& a, const cv::Point& b);

	template<typename PointType>
	double get_aspect_ratio(const std::vector<PointType>& quadrangle, const PointType& origin, double angle_tolerance) {
		const PointType& a = quadrangle[0];
		const PointType& b = quadrangle[1];
		const PointType& c = quadrangle[2];
		const PointType& d = quadrangle[3];

		double angle_ab = calculate_line_angle_degrees(a, b);
		double angle_bc = calculate_line_angle_degrees(b, c);
		double angle_dc = calculate_line_angle_degrees(d, c);
		double angle_ad = calculate_line_angle_degrees(a, d);

		bool ab_dc_parallel = abs(angle_ab - angle_dc) < angle_tolerance;
		bool bc_ad_parallel = abs(angle_bc - angle_ad) < angle_tolerance;

		if (ab_dc_parallel && bc_ad_parallel) {
			return get_aspect_ratio_parallel(quadrangle);
		}
		if (ab_dc_parallel || bc_ad_parallel) {
			return std::nan("");
		}
		return get_aspect_ratio_no_parallel(quadrangle, origin);
	}
	template double get_aspect_ratio< cv::Point>(const std::vector<cv::Point>& quadrangle, const cv::Point& origin, double angle_tolerance);

	template<typename PointType>
	double get_aspect_ratio_parallel(const std::vector<PointType>& quadrangle) {
		const PointType& a = quadrangle[0];
		const PointType& b = quadrangle[1];
		const PointType& c = quadrangle[2];
		const PointType& d = quadrangle[3];

		double ab = cv::norm(b - a);
		double bc = cv::norm(c - b);
		double cd = cv::norm(d - c);
		double ad = cv::norm(a - d);

		return (ab + cd) / (bc + ad);
	}
	template double get_aspect_ratio_parallel<cv::Point>(const std::vector<cv::Point>& quadrangle);

	template<typename PointType>
	double get_aspect_ratio_no_parallel(const std::vector<PointType>& quadrangle, PointType origin) {

		PointType a = quadrangle[0] - origin;
		PointType b = quadrangle[1] - origin;
		PointType c = quadrangle[2] - origin;
		PointType d = quadrangle[3] - origin;

		Eigen::Matrix<double, 6, 6> A;
		Eigen::Matrix<double, 6, 1> B;

		A << 1, 0, -b.x, 0, 0, 0,
			0, 1, -b.y, 0, 0, 0,
			0, 0, 0, 1, 0, -d.x,
			0, 0, 0, 0, 1, -d.y,
			1, 0, -c.x, 1, 0, -c.x,
			0, 1, -c.y, 0, 1, -c.y;

		B << b.x - a.x,
			b.y - a.y,
			d.x - a.x,
			d.y - a.y,
			c.x - a.x,
			c.y - a.y;


		B = A.partialPivLu().solve(B);

		double& ux = B(0, 0);
		double& uy = B(1, 0);
		double& uz_div_lambda = B(2, 0);
		double& vx = B(3, 0);
		double& vy = B(4, 0);
		double& vz_div_lambda = B(5, 0);

		double lambda = std::sqrt((-ux * vx - uy * vy) / (uz_div_lambda * vz_div_lambda));

		double uz = uz_div_lambda * lambda;
		double vz = vz_div_lambda * lambda;

		double u_module = std::sqrt(ux * ux + uy * uy + uz * uz);
		double v_module = std::sqrt(vx * vx + vy * vy + vz * vz);

		double aspect_ratio = u_module / v_module;

		return aspect_ratio;
	}
	template double get_aspect_ratio_no_parallel<cv::Point>(const std::vector<cv::Point>& quadrangle, cv::Point origin);
}