/// \file medianBlur.cc
/// \author roland.dudko@fh-bielefeld.de
/// \date 2014-2015
///
/// \brief Declaration of the class \c Colormatch.
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

#include "../medianblur/medianblur.hh"

#include <iostream>
#include <fstream>
#include <algorithm>

DEFINE_VISION_MODULE(MedianBlur)

using namespace std;

void tv::MedianBlur::execute(tv::ImageHeader const& image,
		tv::ImageData const* data, tv::ImageHeader const& output_header,
		tv::ImageData* output_data) {

	/// Load/convert the source image
	Mat cv_src(image.height, image.width, CV_8UC3, (void*) data);
	Mat cv_dest(output_header.height, output_header.width, CV_8UC3,(void*) output_data);
	std::copy_n(data, image.bytesize, output_data);

	/// Applying median blur
	cv::medianBlur(cv_src, cv_dest, this->kSize);
}

tv::ImageHeader tv::MedianBlur::get_output_image_header(
		ImageHeader const& input) {
	tv::ImageHeader output = input;

	return (output);
}
