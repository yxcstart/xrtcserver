#ifndef __RTC_STREAM_H_
#define __RTC_STREAM_H_

#include <rtc_base/rtc_certificate.h>
#include <memory>
#include <string>
#include "base/event_loop.h"
#include "pc/peer_connection.h"

namespace xrtc {

class RtcStream {
public:
    RtcStream(EventLoop* el, PortAllocator* allocator, uint64_t uid, const std::string& stream_name, bool audio,
              bool video, uint32_t log_id);
    // 必须定义为虚析构，否则删除父类指针，子类无法析构释放，造成内存泄露
    virtual ~RtcStream();

    int start(rtc::RTCCertificate* certificate);
    int set_remote_sdp(const std::string& sdp);

    virtual std::string create_offer() = 0;

protected:
    EventLoop* el;
    uint64_t uid;
    std::string stream_name;
    bool audio;
    bool video;
    uint32_t log_id;

    std::unique_ptr<PeerConnection> pc;

    friend class RtcStreamManager;
};

}  // namespace xrtc

#endif  //__RTC_STREAM_H_