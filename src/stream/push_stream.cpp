
#include "stream/push_stream.h"
#include <rtc_base/logging.h>

namespace xrtc {

PushStream::PushStream(EventLoop* el, PortAllocator* allocator, uint64_t uid, const std::string& stream_name,
                       bool audio, bool video, uint32_t log_id)
    : RtcStream(el, allocator, uid, stream_name, audio, video, log_id) {}
PushStream::~PushStream() { RTC_LOG(LS_INFO) << to_string() << ": Push stream destroy."; }

std::string PushStream::create_offer() {
    RTCOfferAnswerOptions options;
    options.send_audio = false;
    options.send_video = false;
    options.recv_audio = audio;
    options.recv_video = video;

    return pc->create_offer(options);
}

}  // namespace xrtc