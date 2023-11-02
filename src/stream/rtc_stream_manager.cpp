#include "stream/rtc_stream_manager.h"

namespace xrtc {

RtcStreamManager::RtcStreamManager(EventLoop* el) : _el(el) {}

RtcStreamManager::~RtcStreamManager() {}

int RtcStreamManager::create_push_stream(uint64_t uid, const std::string& stream_name, bool audio, bool video,
                                         uint32_t log_id, std::string& offer) {
    PushStream* stream = new PushStream(_el, uid, stream_name, audio, video, log_id);

    return 0;
}

}  // namespace xrtc
