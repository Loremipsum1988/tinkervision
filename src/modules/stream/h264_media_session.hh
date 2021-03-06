/// \file h264_media_session.hh
/// \author philipp.kroos@fh-bielefeld.de
/// \date 2015
///
/// \brief Part of the Tinkervision-Stream module.
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

#ifndef H264MEDIASESSION_H
#define H264MEDIASESSION_H

#include <liveMedia.hh>
#include <OnDemandServerMediaSubsession.hh>

#include "execution_context.hh"
#include "h264_byte_source.hh"
#include "h264_encoder.hh"

namespace tv {
class H264MediaSession : public OnDemandServerMediaSubsession {
public:
    static H264MediaSession* createNew(UsageEnvironment& env,
                                       ExecutionContext& conteext);

    void check_for_aux_sdp_line(void);
    void after_playing_dummy(void);
    void setDone(void) { done_flag_ = 1; }

protected:
    H264MediaSession(UsageEnvironment& env, tv::ExecutionContext& context);
    virtual ~H264MediaSession(void);
    void setDoneFlag() { done_flag_ = ~0; }

protected:
    virtual char const* getAuxSDPLine(RTPSink* rtpSink,
                                      FramedSource* inputSource);
    virtual FramedSource* createNewStreamSource(unsigned clientSessionId,
                                                unsigned& estBitrate);

    virtual RTPSink* createNewRTPSink(Groupsock* rtpGroupsock,
                                      unsigned char rtpPayloadTypeIfDynamic,
                                      FramedSource* inputSource);

private:
    static const bool REUSE_FIRST_SOURCE = true;
    char* aux_SDP_line_;
    char done_flag_;
    RTPSink* dummy_sink_;

    ExecutionContext& context_;
};
}

#endif
