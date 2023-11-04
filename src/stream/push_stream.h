
#ifndef __PUSH_STREAM_H_
#define __PUSH_STREAM_H_

#include "stream/rtc_stream.h"
namespace xrtc {
class PushStream : public RtcStream {
public:
    PushStream(EventLoop* el, uint64_t uid, const std::string& stream_name, bool audio, bool video, uint32_t log_id);
    ~PushStream() override;

    std::string create_offer() override;
};
}  // namespace xrtc

#endif  //__PUSH_STREAM_H_