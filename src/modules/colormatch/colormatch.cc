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

#include <opencv2/opencv.hpp>

#include "colormatch.hh"

DEFINE_VISION_MODULE(Colormatch)

void tfv::Colormatch::execute(tfv::Image const& image) {

    cv::Mat cv_image(image.height, image.width, CV_8UC3);
    std::copy_n(image.data, image.bytesize, cv_image.data);

#ifdef DEBUG
    cv::imshow("Colormatch", cv_image);
    cv::waitKey(2);
#endif
    cv::cvtColor(cv_image, cv_image, CV_BGR2HSV);
    cv::Mat mask(image.height, image.width, CV_8UC3);

    // The hue-range is circular, i.e. it is possible that for a requested color
    // range, the minimum hue > maximum hue. In that case, the range is divided
    // into (min, 180) and (0, max), where 180 is the absolute maximum hue used
    // by opencv (actually, the wrap-around point since the range is circular).
    auto const split = user_min_hue > user_max_hue;

    cv::Scalar low(user_min_hue, min_saturation, min_value);
    cv::Scalar high(split ? max_hue : user_max_hue, max_saturation, max_value);

    if (split) {
        cv::Mat mask0(image.height, image.width, CV_8UC3);
        cv::Mat mask1(image.height, image.width, CV_8UC3);
        cv::inRange(cv_image, low, high, mask0);
        low[0] = min_hue;
        high[0] = user_max_hue;
        cv::inRange(cv_image, low, high, mask1);
        cv::bitwise_or(mask0, mask1, mask);
    } else {
        cv::inRange(cv_image, low, high, mask);
    }

    // Opening
    cv::Mat element = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::morphologyEx(mask, mask, CV_MOP_OPEN, element);

    // find elements
    std::vector<cv::Vec4i> hierarchy;
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask, contours, hierarchy, cv::RETR_EXTERNAL,
                     cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Point> polygon;

    // find largest
    int area = 0;
    cv::Rect rect;
    for (size_t i = 0; i < contours.size(); i++) {
        cv::approxPolyDP(cv::Mat(contours[i]), polygon, 3, true);
        auto tmp = cv::boundingRect(polygon);
        if (not area or (area < (tmp.width * tmp.height))) {
            rect = tmp;
            area = rect.width * rect.height;
        }
    }

    if (contours.size()) {  // call back with center of finding
        Log("COLORMATCH", "Found at ", rect.x, " ", rect.y);
        result_.x = rect.x + (rect.width / 2);
        result_.y = rect.y + (rect.height / 2);
    }
}
