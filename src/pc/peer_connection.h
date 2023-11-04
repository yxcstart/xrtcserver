#ifndef __PEER_CONNECTION_H_
#define __PEER_CONNECTION_H_

#include <string.h>
#include <memory>
#include "base/event_loop.h"
#include "pc/session_description.h"

namespace xrtc {

struct RTCOfferAnswerOptions {
    bool recv_video = true;
    bool recv_audio = true;
};

class PeerConnection {
public:
    PeerConnection(EventLoop* el);
    ~PeerConnection();

    std::string create_offer(const RTCOfferAnswerOptions& options);

private:
    EventLoop* _el;

    std::unique_ptr<SessionDescription> _local_desc;
};

}  // namespace xrtc

#endif  // __PEER_CONNECTION_H_
