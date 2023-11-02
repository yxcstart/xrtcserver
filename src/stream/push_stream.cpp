

#include "stream/push_stream.h"

namespace xrtc {

PushStream::PushStream(EventLoop* el, uint64_t uid, const std::string& stream_name, bool audio, bool video,
                       uint32_t log_id)
    : RtcStream(el, uid, stream_name, audio, video, log_id) {}
PushStream::~PushStream() {}
}  // namespace xrtc