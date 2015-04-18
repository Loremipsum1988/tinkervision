/*
Tinkervision - Vision Library for https://github.com/Tinkerforge/red-brick
Copyright (C) 2014-2015 philipp.kroos@fh-bielefeld.de

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "convert.hh"

#include <algorithm>

tfv::Image const& tfv::Convert::operator()(tfv::Image const& source) {
    if (not target) {
        size_t width, height, bytesize;
        target_format_ = target_format(source, width, height, bytesize);
        target = new tfv::Image();
        target->width = width;
        target->height = height;
        target->bytesize = bytesize;
        target->format = target_format_;
        target->data = new TFV_ImageData[bytesize];
    }

    convert(source, *target);
    std::cout << "Target bytesize is " << target->bytesize << std::endl;
    target->timestamp = source.timestamp;

    return *target;
}

tfv::ColorSpace tfv::ConvertYUV422ToYUV420::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {
    target_width = source.width;
    target_height = source.height;
    target_bytesize = (source.bytesize >> 2) * 3;
    return tfv::ColorSpace::YV12;
}

// output is in order y-block, v-block, u-block
void tfv::ConvertYUV422ToYUV420::convert_any(tfv::Image const& source,
                                             tfv::Image& target,
                                             TFV_ImageData* u_ptr,
                                             TFV_ImageData* v_ptr) const {

    auto to = target.data;  // moving pointer

    // Y: every second byte
    for (size_t i = 0; i < source.bytesize; i += 2) {
        *to++ = *(source.data + i);
    }

    const size_t width = source.width * 2;  // in byte
    const size_t height = source.height;

    auto copy_u_or_v = [&width, &height, &to](TFV_ImageData const* u_or_v_ptr) {

        auto next_row = u_or_v_ptr + width;

        for (size_t i = 0; i < height; i += 2) {
            for (size_t j = 0; j < width; j += 4) {
                *to++ = (static_cast<int>(u_or_v_ptr[j]) + next_row[j]) / 2;
            }
            // next two rows
            u_or_v_ptr += (width * 2);
            next_row += (width * 2);
        }
    };

    // U and V: averaging values from two rows a time
    copy_u_or_v(v_ptr);
    copy_u_or_v(u_ptr);
}

void tfv::YUVToRGB::target_size(tfv::Image const& source, size_t& target_width,
                                size_t& target_height,
                                size_t& target_bytesize) const {
    target_width = source.width;
    target_height = source.height;
    target_bytesize = (source.width * source.height) * 3;  // 24 bit/pixel
}

int constexpr tfv::YUVToRGB::coeff_r[];
int constexpr tfv::YUVToRGB::coeff_g[];
int constexpr tfv::YUVToRGB::coeff_b[];

template <size_t r, size_t g, size_t b>
void tfv::YUVToRGB::convert(int y, int u, int v, TFV_ImageData* rgb) const {

    // need indices 0,1,2
    static_assert(((r + g + b) == 3) and (r < 3) and (g < 3) and (b < 3) and
                      ((r == 0) or (g == 0) or (b == 0)),
                  "Need to provide rgb channels as 0, 1 and 2");
    y = y - 16;
    u = u - 128;
    v = v - 128;

    // Conversions from http://en.wikipedia.org/wiki/YUV, seem fine.
    // HD:

    *(rgb + r) = clamp_(y + 1.28033 * v);
    *(rgb + g) = clamp_(y - 0.21482 * u - 0.38059 * v);
    *(rgb + b) = clamp_(y + 2.21798 * u);
    // SD:
    /*
    *(rgb + r) = clamp_(y + 1.3983 * v);
    *(rgb + g) = clamp_(y - 0.39465 * u - 0.58060 * v);
    *(rgb + b) = clamp_(y + 2.03211 * u);
    */
    // Kaufmann:
    /*
    *(rgb + r) =
        clamp_((coeff_r[0] * y + coeff_r[1] * u + coeff_r[2] * v) / normalizer);

    *(rgb + g) =
        clamp_((coeff_g[0] * y + coeff_g[1] * u + coeff_g[2] * v) / normalizer);

    *(rgb + b) =
        clamp_((coeff_b[0] * y + coeff_b[1] * u + coeff_b[2] * v) / normalizer);
    */
}

template <size_t r, size_t g, size_t b>
void tfv::YUYVToRGBType::convert(tfv::Image const& source,
                                 tfv::Image& target) const {

    auto to = target.data;

    for (auto src = source.data; src < (source.data + source.bytesize);) {

        int const y1 = static_cast<int>(*src++);
        int const u = static_cast<int>(*src++);
        int const y2 = static_cast<int>(*src++);
        int const v = static_cast<int>(*src++);

        YUVToRGB::convert<r, g, b>(y1, u, v, to);
        to += 3;
        YUVToRGB::convert<r, g, b>(y2, u, v, to);
        to += 3;
    }
}

tfv::ColorSpace tfv::ConvertYUYVToRGB::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {
    target_size(source, target_width, target_height, target_bytesize);
    return tfv::ColorSpace::RGB888;
}

tfv::ColorSpace tfv::ConvertYUYVToBGR::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {
    target_size(source, target_width, target_height, target_bytesize);
    return tfv::ColorSpace::BGR888;
}

template <size_t r, size_t g, size_t b>
void tfv::YV12ToRGBType::convert(tfv::Image const& source,
                                 tfv::Image& target) const {

    auto to = target.data;
    auto const v_plane = source.data + source.width * source.height;
    auto const u_plane = v_plane + ((source.width * source.height) >> 2);
    auto const uv_offset = source.width >> 1;

    // abbreviated version of http://en.wikipedia.org/wiki/YUV
    for (size_t i = 0; i < source.height; i++) {
        auto const row_uv = (i >> 1) * uv_offset;
        auto const row_y = i * source.width;
        for (size_t j = 0; j < source.width; j++) {

            // Every u(v)-value corresponds to one four-block of
            // Y-values,
            // i.e. each two y's from subsequent rows. According to
            // this,
            // the index of the u(v)-value needed is given by:
            // uv_idx = ((i / 2) * (source.width / 2)) + (j / 2)
            // (i.e. like indexing a 2d array with one idx).
            // With the definition of row_uv it can be written:

            auto uv_idx = (j >> 1) + row_uv;

            // std::cout << row_y + j << "," << uv_idx << std::endl;

            int const y = static_cast<int>(source.data[row_y + j]);
            int const u = static_cast<int>(u_plane[uv_idx]);
            int const v = static_cast<int>(v_plane[uv_idx]);

            YUVToRGB::convert<r, g, b>(y, u, v, to);
            to += 3;
        }
    }
}

tfv::ColorSpace tfv::ConvertYV12ToRGB::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {
    target_size(source, target_width, target_height, target_bytesize);
    return tfv::ColorSpace::RGB888;
}

void tfv::ConvertYV12ToRGB::convert(tfv::Image const& source,
                                    tfv::Image& target) const {
    tfv::YV12ToRGBType::convert<0, 1, 2>(source, target);
}

tfv::ColorSpace tfv::ConvertYV12ToBGR::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {
    target_size(source, target_width, target_height, target_bytesize);
    return tfv::ColorSpace::BGR888;
}

void tfv::ConvertYV12ToBGR::convert(tfv::Image const& source,
                                    tfv::Image& target) const {
    tfv::YV12ToRGBType::convert<2, 1, 0>(source, target);
}

void tfv::RGBTypeConversion::target_size(tfv::Image const& source,
                                         size_t& target_width,
                                         size_t& target_height,
                                         size_t& target_bytesize) const {
    target_width = source.width;
    target_height = source.height;
    target_bytesize = source.bytesize;
}

void tfv::RGBFromToBGR::target_size(tfv::Image const& source,
                                    size_t& target_width, size_t& target_height,
                                    size_t& target_bytesize) const {
    target_width = source.width;
    target_height = source.height;
    target_bytesize = source.bytesize;
}

void tfv::RGBFromToBGR::convert(tfv::Image const& source,
                                tfv::Image& target) const {
    auto to = target.data;
    for (size_t i = 0; i < source.bytesize; i += 3) {
        *to++ = source.data[i + 2];
        *to++ = source.data[i + 1];
        *to++ = source.data[i];
    }
}

tfv::ColorSpace tfv::ConvertRGBToBGR::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {

    target_size(source, target_width, target_height, target_bytesize);
    return tfv::ColorSpace::BGR888;
}

tfv::ColorSpace tfv::ConvertBGRToRGB::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {

    target_size(source, target_width, target_height, target_bytesize);
    return tfv::ColorSpace::RGB888;
}

tfv::ColorSpace tfv::ConvertBGRToYV12::target_format(
    tfv::Image const& source, size_t& target_width, size_t& target_height,
    size_t& target_bytesize) const {

    target_width = source.width;
    target_height = source.height;
    target_bytesize = (source.width * source.height * 3) >> 1;

    std::cout << "Setting bytesize to " << target_bytesize << std::endl;
    return tfv::ColorSpace::YV12;
}

void tfv::ConvertBGRToYV12::convert(Image const& source, Image& target) const {
    std::cout << "Converting for bytesize " << target.bytesize << std::endl;
    auto row0 = source.data;
    auto row1 = source.data + source.width * 3;
    auto y0 = target.data;
    auto y1 = target.data + target.width;
    auto v = target.data + target.width * target.height;
    auto u = v + ((target.width * target.height) >> 2);

    for (size_t i = 0; i < source.height; i += 2) {
        for (size_t j = 0; j < source.width; j += 2) {
            *y0++ = 0.299 * row0[2] + 0.587 * row0[1] + 0.114 * row0[0];
            *y0++ = 0.299 * row0[5] + 0.587 * row0[4] + 0.114 * row0[3];
            *y1++ = 0.299 * row1[2] + 0.587 * row1[1] + 0.114 * row1[0];
            *y1++ = 0.299 * row1[5] + 0.587 * row1[4] + 0.114 * row1[3];
            *u++ = ((0.499 * row0[2] - 0.331 * row0[1] - 0.169 * row0[0]) +
                    (0.499 * row1[2] - 0.331 * row1[1] - 0.169 * row1[0]) +
                    (0.499 * row0[5] - 0.331 * row0[4] - 0.169 * row0[3]) +
                    (0.499 * row1[5] - 0.331 * row1[4] - 0.169 * row1[3])) /
                       4.0 +
                   128;
            *v++ = ((-0.0813 * row0[2] - 0.418 * row0[1] + 0.499 * row0[0]) +
                    (-0.0813 * row1[2] - 0.418 * row1[1] + 0.499 * row1[0]) +
                    (-0.0813 * row0[5] - 0.418 * row0[4] + 0.499 * row0[3]) +
                    (-0.0813 * row1[5] - 0.418 * row1[4] + 0.499 * row1[3])) /
                       4.0 +
                   128;

            row0 += 6;
            row1 += 6;
        }

        y0 = y1;
        y1 = y1 + target.width;
        row0 = row1;
        row1 += source.width * 3;
    }
    std::cout << "Written " << (int)(u - target.data) << " byte" << std::endl;
}

tfv::Converter::Converter(tfv::ColorSpace source, tfv::ColorSpace target) {
    auto it = std::find_if(conversions_.begin(), conversions_.end(),
                           [&source, &target](Conversion const& conversion) {

        return (std::get<0>(conversion) == source) and
               (std::get<1>(conversion) == target);
    });

    if (it != conversions_.end()) {
        auto maker = std::get<2>(*it);
        converter_ = (this->*maker)();
    }
}

tfv::Converter::~Converter(void) {
    if (converter_) {
        delete converter_;
    }
}

tfv::Image const& tfv::Converter::operator()(tfv::Image const& source) const {
    if (not converter_) {
        return invalid_image_;
    }

    return (*converter_)(source);
}
