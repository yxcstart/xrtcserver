#ifndef __ICE_CONNECTION_H_
#define __ICE_CONNECTION_H_

#include "base/event_loop.h"
#include "ice/candidate.h"
#include "ice/ice_credentials.h"
#include "ice/stun.h"
namespace xrtc {

class UDPPort;

class IceConnection {
public:
    enum WriteState {
        STATE_WRITABLE = 0,
        STATE_WRITE_UNRELIABLE = 1,
        STATE_WRITE_INIT = 2,
        STATE_WRITE_TIMEOUT = 3,
    };

    IceConnection(EventLoop* el, UDPPort* port, const Candidate& remote_candidate);
    ~IceConnection();

    const Candidate& remote_candidate() const { return _remote_candidate; }

    void handle_stun_binding_request(StunMessage* stun_msg);
    void send_stun_binding_response(StunMessage* stun_msg);
    void send_response_message(const StunMessage& response);

    void on_read_packet(const char* buf, size_t len, int64_t ts);
    void maybe_set_remote_ice_params(const IceParameters& ice_params);

    bool writable() { return _write_state == STATE_WRITABLE; }
    bool receving() { return _receiving; }
    bool weak() { return !(writable() && receving()); }
    bool active() { return _write_state != STATE_WRITE_TIMEOUT; }
    bool stable(int64_t now) const;

    int64_t last_ping_sent() const { return _last_ping_sent; }
    int num_pings_sent() const { return _num_pings_sent; }

    std::string to_string();

private:
    EventLoop* _el;
    UDPPort* _port;
    Candidate _remote_candidate;

    WriteState _write_state = STATE_WRITE_INIT;
    bool _receiving = false;

    int64_t _last_ping_sent = 0;
    int _num_pings_sent = 0;
};

}  // namespace xrtc

#endif  //__ICE_CONNECTION_H_
