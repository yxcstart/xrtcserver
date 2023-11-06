#ifndef __PEER_CONNECTION_H_
#define __PEER_CONNECTION_H_

#include <rtc_base/rtc_certificate.h>
#include <string.h>
#include <memory>
#include "base/event_loop.h"
#include "pc/session_description.h"
#include "pc/transport_controller.h"

namespace xrtc {

struct RTCOfferAnswerOptions {
    bool recv_video = true;
    bool recv_audio = true;
    bool send_video = true;
    bool send_audio = true;
    bool use_rtp_mux = true;
    bool use_rtcp_mux = true;
    bool dtls_on = true;
};

class PeerConnection {
public:
    PeerConnection(EventLoop* el);
    ~PeerConnection();

    int init(rtc::RTCCertificate* certificate);
    std::string create_offer(const RTCOfferAnswerOptions& options);

private:
    EventLoop* _el;
    std::unique_ptr<SessionDescription> _local_desc;
    rtc::RTCCertificate* _certificate = nullptr;
    std::unique_ptr<TransportController> _transport_controller;
};

}  // namespace xrtc

#endif  // __PEER_CONNECTION_H_
