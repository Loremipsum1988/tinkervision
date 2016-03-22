/// \file Cascadeclassifier.cc
/// \author roland.dudko@fh-bielefeld.de
/// \date 2015-2016
///
/// \brief Definition of the module \c Cascadeclassifier.
///
/// This file is part of Tinkervision - Vision Library for Tinkerforge Redbrick
/// \sa https://github.com/Tinkerforge/red-brick
///
/// \copyright
///
/// This program is free software; you can redistribute it and/or
/// modify it under the terms of the GNU General Public License
/// as published by the Free Software Foundation; either version 2
/// of the License, or (at your option) any later version.
///
/// This program is distributed in the hope that it will be useful,
/// but WITHOUT ANY WARRANTY; without even the implied warranty of
/// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
/// GNU General Public License for more details.
///
/// You should have received a copy of the GNU General Public License
/// along with this program; if not, write to the Free Software
/// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
/// USA.

#include "cascadeclassifier.hh"

using namespace cv;
using namespace tv;

DEFINE_VISION_MODULE(Cascadeclassifier)

tv::Cascadeclassifier::~Cascadeclassifier() {
	classifier.clear();
}

void tv::Cascadeclassifier::setup_haarmodel(void) {
	try {
		classifier.clear();
	} catch (...) {
		std::cout << "Error in clear" << std::endl;
	}

	// if path_to_model_ is a single xml file
	if (path_to_model_.substr(path_to_model_.find_last_of(".") + 1) == "xml") {
		classifier.push_back(
				std::unique_ptr < cv::CascadeClassifier
						> (new cv::CascadeClassifier("" + path_to_model_ )));
		return;
	}

	DIR *dir;
	struct dirent *ent;
	const char * path = path_to_model_.c_str();

	if ((dir = opendir(path)) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			std::string file_name = ent->d_name;

			if (file_name.substr(file_name.find_last_of(".") + 1) == "xml") {
				classifier.push_back(
						std::unique_ptr < cv::CascadeClassifier
								> (new cv::CascadeClassifier(
										path_to_model_ + "/" + file_name)));
			}
		}
		closedir(dir);

	} else {/* could not open directory */
		Log("Cascadeclassifier", "Could not open directory " + path_to_model_);
	}
}

void tv::Cascadeclassifier::register_all_parameter() {
	register_parameter("draw_ractangle", 0, 1, 1);
	register_parameter("use_grayscale", 0, 1, 1);
	register_parameter("min_object_size", 10, 100, object_size);
	register_parameter("max_object_size", 10, 100, object_size);
	register_parameter("min_neighbors", 2, 10, min_neighbors_);
	register_parameter("user_image_scale", 1, 5, default_image_scale);
	register_parameter("path_to_model", path_to_model_,
			[this](std::string const& old_path, std::string const& new_path) {
				bool is_xml_file =new_path.substr(new_path.find_last_of(".") + 1) == "xml";
				return ((is_directory(new_path) || is_xml_file) ? true : false);
			});
}

void tv::Cascadeclassifier::value_changed(std::string const& parameter,
		int32_t value) {
	if (parameter == "draw_ractangle") {
		draw_rectangle_ = ((value == 0) ? false : true);

	} else if (parameter == "use_grayscale") {
		use_grayscale_ = ((value == 0) ? false : true);

	} else if (parameter == "min_object_size") {
		user_min_object_size_ = static_cast<uint8_t>(value);

	} else if (parameter == "max_object_size") {
		user_max_object_size_ = static_cast<uint8_t>(value);

	} else if (parameter == "min_neighbors") {
		user_min_neighbors_ = static_cast<uint8_t>(value);

	} else if (parameter == "user_image_scale") {
		user_image_scale_ = 1 + value / 10.0;
	}
}

void tv::Cascadeclassifier::value_changed(std::string const& parameter,
		std::string const& value) {
	if (parameter == "path_to_model") {
		path_to_model_ = value;
		setup_haarmodel();
	}
}

tv::ImageHeader tv::Cascadeclassifier::get_output_image_header(
		ImageHeader const& input) {
	tv::ImageHeader output = input;

	return (output);
}

void tv::Cascadeclassifier::execute(tv::ImageHeader const& header,
		ImageData const* data, tv::ImageHeader const& output_header,
		ImageData* output_data) {

	Log("Cascadeclassifier", "execute");

	if (classifier.size() == 0)
		return;

	// Convert to opencv Image
	Mat cv_src(header.height, header.width, CV_8UC3, (void*) data);
	Mat cv_dest;
	if (draw_rectangle_) {
		std::copy_n(data, header.bytesize, output_data);
		cv_dest = {header.height, header.width, CV_8UC3, (void*) output_data};
	}

	// Convert to grayscale
	Mat frame;
	if (use_grayscale_) {
		cvtColor(cv_src, frame, CV_BGR2GRAY);
		equalizeHist(frame, frame);

	} else {
		frame = cv_src;
	}

	// Apply the classifier to the frame
	std::vector < cv::Rect > found_objects;
	for (unsigned i = 0; i < classifier.size(); i++) {
		(classifier.at(i))->detectMultiScale(frame, found_objects,
				1.5, user_min_neighbors_, 0 | CV_HAAR_SCALE_IMAGE,
				Size(user_min_object_size_, user_max_object_size_));
	}

	// Generate result: draw rectangle
	if (draw_rectangle_ && !found_objects.empty()) {
		for (auto i = found_objects.begin(); i != found_objects.end(); ++i) {
			cv::rectangle(cv_dest, cv::Point(i->x, i->y),
					cv::Point(i->x + i->width, i->y + i->height),
					CV_RGB(255, 0, 0), 2);
		}

		imwrite("found.png", cv_dest);
	}

	if (!found_objects.empty())
		result_.x = found_objects.size();
	else
		result_.x = 0;

	result_.y = 0;
}
