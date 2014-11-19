/*
Tinkervision - Vision Library for https://github.com/Tinkerforge/red-brick
Copyright (C) 2014 philipp.kroos@fh-bielefeld.de

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

#ifndef COLORTRACKING_H
#define COLORTRACKING_H

#include "component.hh"

namespace tinkervision {
class Colortracking : public Component {
private:
    TFV_Byte min_hue_;
    TFV_Byte max_hue_;
    TFV_Callback callback_;
    TFV_Context context_;

public:
    Colortracking(TFV_Id camera_id, TFV_Byte min_hue, TFV_Byte max_hue,
                  TFV_Callback callback, TFV_Context context)
        : Component(camera_id),
          min_hue_(min_hue),
          max_hue_(max_hue),
          callback_(callback),
          context_(context) {}

    virtual ~Colortracking(void) = default;
    Colortracking(Colortracking const& other) = default;
    Colortracking(Colortracking&& other) = delete;
    Colortracking& operator=(Colortracking const& rhs) = default;
    Colortracking& operator=(Colortracking&& rhs) = delete;

    TFV_Byte min_hue(void) const { return min_hue_; }
    TFV_Byte max_hue(void) const { return max_hue_; }

    virtual void execute(TFV_ImageData* data, TFV_Int rows, TFV_Int columns);
};

// called with the exact same parameter list as the constructor minus
// the camera_id, and all args are references. If this is not correct
// here, the 'basecase' in component.hh will be used.
template <>
bool valid<Colortracking>(TFV_Byte& min_hue, TFV_Byte& max_hue,
                          TFV_Callback& callback, TFV_Context& context);
};

#endif /* COLORTRACKING_H */
