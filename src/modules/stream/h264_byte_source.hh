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

#ifndef H264BYTESOURCE_H
#define H264BYTESOURCE_H

#include <queue>

#include <FramedSource.hh>

#include "h264_encoder.hh"

namespace tfv {
class H264ByteSource : public FramedSource {
public:
    static H264ByteSource* createNew(UsageEnvironment& env,
                                     tfv::H264Encoder& encoder);
    static EventTriggerId eventTriggerId;

protected:
    H264ByteSource(UsageEnvironment& env, tfv::H264Encoder& encoder);
    virtual ~H264ByteSource(void);

private:
    unsigned char* frame_ = nullptr;

    virtual void doGetNextFrame();
    static void deliverFrame0(void* clientData);
    void deliverFrame();
    static unsigned referenceCount;
    std::queue<x264::Nal> nals_;
    timeval currentTime;
    H264Encoder& encoder_;
};
}
#endif /* H264BYTESOURCE_H */