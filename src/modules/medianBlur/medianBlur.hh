/// \file medianBlur.hh
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

#ifndef SRC_MODULES_MEDIANBLUR_MEDIANBLUR_HH_
#define SRC_MODULES_MEDIANBLUR_MEDIANBLUR_HH_

#include <limits.h>

#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "tinkervision/module.hh"

using namespace cv;
namespace tv {

// filter for removing salt-and-pepper noise.
class MedianBlur: public Module {
private:
	// Defines the size of the kernel to be used (of width w pixels and height h pixels)
	uint8_t kSize;

public:
	MedianBlur(Environment const& envir) :
			Module("medianBlur",envir) {
		kSize = 3;
	}
	~MedianBlur(void) override = default;

protected:
	// Blur filter function --------------------------
	void execute(tv::ImageHeader const& image, tv::ImageData const* data,
			tv::ImageHeader const& output_header, tv::ImageData* output_data)
					override final;

	ColorSpace input_format(void) const override {
		return (ColorSpace::BGR888);
	}

	// Result Image ----------------------------------
	ImageHeader get_output_image_header(ImageHeader const& ref) override final;

	bool outputs_image(void) const override final {
		return (true);
	}
	bool produces_result(void) const override final {
		return (false);
	}

	// Parameter handling ----------------------------
    void value_changed(std::string const& parameter,
                       int32_t value) override final {
		kSize = static_cast<uint8_t>(value);
	}
};
}

DECLARE_VISION_MODULE(MedianBlur)

#endif /* SRC_MODULES_MEDIANBLUR_MEDIANBLUR_HH_ */
