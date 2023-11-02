
#include "stream/rtc_stream.h"

namespace xrtc {
RtcStream::RtcStream(EventLoop* el, uint64_t uid, const std::string& stream_name, bool audio, bool video,
                     uint32_t log_id)
    : el(el), uid(uid), stream_name(stream_name), audio(audio), video(video), log_id(log_id) {}

RtcStream::~RtcStream() {}
}  // namespace xrtc