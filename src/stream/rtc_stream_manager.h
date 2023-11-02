#ifndef _RTC_STREAM_MANAGER_H_
#define _RTC_STREAM_MANAGER_H_
#include <string>
#include "base/event_loop.h"

namespace xrtc {
class RtcStreamManager {
public:
    RtcStreamManager(EventLoop* el);

    ~RtcStreamManager();

    int create_push_stream(uint64_t uid, const std::string& stream_name, bool audio, bool video, uint32_t log_id,
                           std::string& offer);

private:
    EventLoop* _el;
};

}  // namespace xrtc

#endif  // _RTC_STREAM_MANAGER_H_
