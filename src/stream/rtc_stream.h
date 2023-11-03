#ifndef __RTC_STREAM_H_
#define __RTC_STREAM_H_

#include <memory>
#include <string>
#include "base/event_loop.h"
#include "pc/peer_connection.h"

namespace xrtc {

class RtcStream {
public:
    RtcStream(EventLoop* el, uint64_t uid, const std::string& stream_name, bool audio, bool video, uint32_t log_id);
    // 必须定义为虚析构，否则子类析构释放的时候，父类不释放
    virtual ~RtcStream();

    virtual std::string create_offer() = 0;

protected:
    EventLoop* el;
    uint64_t uid;
    std::string stream_name;
    bool audio;
    bool video;
    uint32_t log_id;

    std::unique_ptr<PeerConnection> pc;
};

}  // namespace xrtc

#endif  //__RTC_STREAM_H_