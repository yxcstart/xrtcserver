#ifndef __ICE_CONNECTION_H_
#define __ICE_CONNECTION_H_

#include "base/event_loop.h"
#include "ice/candidate.h"
#include "ice/ice_connection_info.h"
#include "ice/ice_credentials.h"
#include "ice/stun.h"
#include "ice/stun_request.h"
namespace xrtc {

class UDPPort;
class IceConnection;

class ConnectionRequest : public StunRequest {
public:
    ConnectionRequest(IceConnection* conn);

protected:
    void prepare(StunMessage* msg) override;
    void on_request_response(StunMessage*) override;
    void on_request_error_response(StunMessage*) override;

private:
    IceConnection* _connection;
};

class IceConnection : public sigslot::has_slots<> {
public:
    enum WriteState {
        STATE_WRITABLE = 0,
        STATE_WRITE_UNRELIABLE = 1,
        STATE_WRITE_INIT = 2,
        STATE_WRITE_TIMEOUT = 3,
    };

    struct SentPing {
        SentPing(const std::string& id, int64_t ts) : id(id), sent_time(ts) {}

        std::string id;
        int64_t sent_time;
    };

    IceConnection(EventLoop* el, UDPPort* port, const Candidate& remote_candidate);
    ~IceConnection();

    const Candidate& remote_candidate() const { return _remote_candidate; }
    const Candidate& local_candidate() const;
    UDPPort* port() { return _port; }

    void handle_stun_binding_request(StunMessage* stun_msg);
    void send_stun_binding_response(StunMessage* stun_msg);
    void send_response_message(const StunMessage& response);

    void on_read_packet(const char* buf, size_t len, int64_t ts);

    void on_connection_request_response(ConnectionRequest* request, StunMessage* msg);
    void on_connection_request_error_response(ConnectionRequest* request, StunMessage* msg);
    void maybe_set_remote_ice_params(const IceParameters& ice_params);
    void print_pings_since_last_response(std::string& pings, size_t max);

    void set_write_state(WriteState state);
    WriteState write_state() { return _write_state; }
    bool writable() { return _write_state == STATE_WRITABLE; }
    bool receiving() { return _receiving; }
    bool weak() { return !(writable() && receiving()); }
    bool active() { return _write_state != STATE_WRITE_TIMEOUT; }
    bool stable(int64_t now) const;
    void ping(int64_t now);
    void received_ping_response(int rtt);
    void update_receiving(int64_t now);
    int receiving_timeout();
    uint64_t priority();
    int rtt() { return _rtt; }

    void set_selected(bool value) { _selected = value; }
    bool selected() { return _selected; }

    void set_state(IceCandidatePairState state);
    IceCandidatePairState state() { return _state; }

    void fail_and_destroy();
    void destroy();

    int64_t last_ping_sent() const { return _last_ping_sent; }
    int64_t last_received();
    int num_pings_sent() const { return _num_pings_sent; }

    std::string to_string() const;

    sigslot::signal1<IceConnection*> signal_state_change;
    sigslot::signal1<IceConnection*> signal_connection_destroy;

private:
    void _on_stun_send_packet(StunRequest* request, const char* buf, size_t len);
    bool _miss_response(int64_t now) const;

private:
    EventLoop* _el;
    UDPPort* _port;
    Candidate _remote_candidate;

    WriteState _write_state = STATE_WRITE_INIT;
    bool _receiving = false;
    bool _selected = false;
    int64_t _last_ping_sent = 0;
    int64_t _last_ping_recevied = 0;
    int64_t _last_ping_response_received = 0;
    int64_t _last_data_recevied = 0;
    int _num_pings_sent = 0;
    std::vector<SentPing> _pings_since_last_response;  // 发送ping请求的时候作缓存，只有ping收到响应了才从缓存当中删除
    StunRequestManager _requests;
    int _rtt = 3000;
    int _rtt_samples = 0;
    IceCandidatePairState _state = IceCandidatePairState::WAITING;
};

}  // namespace xrtc

#endif  //__ICE_CONNECTION_H_
