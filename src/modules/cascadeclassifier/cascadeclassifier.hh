/// \file Cascadeclassifier.hh
/// \author roland.dudko@fh-bielefeld.de
/// \date 2014-2016
///
/// \brief Declaration of the class \c Cascadeclassifier.
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

#ifndef CASCADECLASSIFIER_H
#define CASCADECLASSIFIER_H

#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <dirent.h>

#include "module.hh"

namespace tv {

class Cascadeclassifier: public Module {
private:
	// default values
	uint8_t const object_size { 30 };
	double const min_image_scale { 1.1 };
	uint8_t const min_neighbors_ { 2 };

	bool draw_rectangle_ { true };	// draw rectangle around found objects
	bool use_grayscale_ { true };	// process object detection on a grayscale image

	uint8_t user_min_object_size_; 	// user defined min size of objects
	uint8_t user_max_object_size_; 	// user defined max size of objects
	uint8_t user_min_neighbors_; 	// user defined min neighbor count for objects
	double user_image_scale_;

	std::vector<std::unique_ptr<cv::CascadeClassifier>> classifier;
	std::vector < cv::Rect > found_objects;

	// path to haar files
	std::string path_to_model_ { "/home/tf/tv/lib/model/" };

	bool has_result_ { true };
	Result result_;

public:
	Cascadeclassifier(Environment const& envir) :
			Module("cascadeclassifier", envir), user_min_object_size_(object_size), user_max_object_size_(
					object_size), user_min_neighbors_(min_neighbors_), user_image_scale_(
					min_image_scale) {

		register_all_parameter();
		setup_haarmodel();
	}
	~Cascadeclassifier(void) override;

private:
	void register_all_parameter(void);
	void setup_haarmodel(void);

protected:

	ImageHeader get_output_image_header(ImageHeader const& ref) override final;

	/// Execute this module
	void execute(tv::ImageHeader const& header, ImageData const* data,
			tv::ImageHeader const& output_header, ImageData* output_data)
					override;

	/// Store the value of changed parameters internally to have faster access.
	/// \param[in] parameter The name of the changed parameter.
	/// \param[in] value New value
	void value_changed(std::string const& parameter, int32_t value)
			override final;
	/// Store the value of changed parameters internally to have faster access.
	/// \param[in] parameter The name of the changed parameter.
	/// \param[in] value New value
	void value_changed(std::string const& parameter, std::string const& value)
			override final;

	bool has_result(void) const override final {
		return ((found_objects.size() > 0 ? true : false));
	}

	Result const& get_result(void) const override {
		return (result_);
	}

	/// Declare the expected colorspace.
	/// \return ColorSpace::BGR888
	ColorSpace input_format(void) const override {
		return (ColorSpace::BGR888);
	}

	/// This module can modify the image .
	/// \return true id draw_rectangle_ is set to true.
	bool outputs_image(void) const override final {
		return (draw_rectangle_);
	}

	/// Declare that this module can generate a result.
	/// If valid, a result consists of rectangles around detected objects
	/// \return true.
	bool produces_result(void) const override final {
		return (true);
	}
};
}

DECLARE_VISION_MODULE(Cascadeclassifier)

#endif
