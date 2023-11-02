#ifndef __RTC_STREAM_H_
#define __RTC_STREAM_H_

#include <string>
#include "base/event_loop.h"
namespace xrtc {

class RtcStream {
public:
    RtcStream(EventLoop* el, uint64_t uid, const std::string& stream_name, bool audio, bool video, uint32_t log_id);
    // 必须定义为虚析构，否则子类析构释放的时候，父类不释放
    virtual ~RtcStream();

protected:
    EventLoop* el;
    uint64_t uid;
    std::string stream_name;
    bool audio;
    bool video;
    uint32_t log_id;
};

}  // namespace xrtc

#endif __RTC_STREAM_H_