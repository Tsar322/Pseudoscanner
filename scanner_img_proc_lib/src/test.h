#pragma once

#include <filesystem>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>

namespace test {
	std::vector<std::string> get_files_in_directory(const std::string& directory) {
		std::vector<std::string> filenames;
		for (const auto& entry : std::filesystem::directory_iterator(directory)) {
			if (entry.is_regular_file()) {
				filenames.push_back(entry.path().string());
			}
		}
		return filenames;
	}

	cv::Mat load_image(const std::string& filename) {
		cv::Mat img = cv::imread(filename, cv::IMREAD_COLOR);
		if (img.empty()) {
			std::cerr << "Error: Could not load image " << filename << std::endl;
		}
		return img;
	}

	std::vector<cv::Mat> load_images(const std::vector<std::string>& filenames) {
		std::vector<cv::Mat> images;
		for (const auto& filename : filenames) {
			images.push_back(load_image(filename));
		}
		return images;
	}
}