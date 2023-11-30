#ifndef __ICE_CONNECTION_H_
#define __ICE_CONNECTION_H_

#include "base/event_loop.h"
#include "ice/candidate.h"
#include "ice/stun.h"
namespace xrtc {

class UDPPort;

class IceConnection {
public:
    IceConnection(EventLoop* el, UDPPort* port, const Candidate& remote_candidate);
    ~IceConnection();

    const Candidate& remote_candidate() const { return _remote_candidate; }

    void handle_stun_binding_request(StunMessage* stun_msg);
    void send_stun_binding_response(StunMessage* stun_msg);
    void send_response_message(const StunMessage& response);

    void on_read_packet(const char* buf, size_t size, int64_t ts);

    std::string to_string();

private:
    EventLoop* _el;
    UDPPort* _port;
    Candidate _remote_candidate;
};

}  // namespace xrtc

#endif  //__ICE_CONNECTION_H_
