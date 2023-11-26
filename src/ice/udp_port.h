#ifndef __UDP_PORT_H_
#define __UDP_PORT_H_

#include <map>
#include <memory>
#include <string>
#include <vector>
#include "base/async_udp_socket.h"
#include "base/event_loop.h"
#include "base/network.h"
#include "ice/candidate.h"
#include "ice/ice_connection.h"
#include "ice/ice_credentials.h"
#include "ice/ice_def.h"
#include "ice/stun.h"
#include "rtc_base/socket_address.h"
namespace xrtc {

typedef std::map<rtc::SocketAddress, IceConnection*> AddressMap;

class UDPPort : public sigslot::has_slots<> {
public:
    UDPPort(EventLoop* el, const std::string& transport_name, IceCandidateComponent compoent, IceParameters ice_params);
    ~UDPPort();

    int create_ice_candidate(Network* network, int min_port, int max_port, Candidate& c);
    bool get_stun_message(const char* data, size_t len, const rtc::SocketAddress& addr,
                          std::unique_ptr<StunMessage>* out_msg, std::string* out_username);
    void send_binding_error_response(StunMessage* stun_msg, const rtc::SocketAddress& addr, int err_code,
                                     const std::string& reason);
    IceConnection* create_connection(const Candidate& candidate);
    std::string to_string();

    sigslot::signal4<UDPPort*, const rtc::SocketAddress&, StunMessage*, const std::string&> signal_unknown_address;

private:
    void _on_read_packet(AsyncUdpSocket* socket, char* buf, size_t size, const rtc::SocketAddress& addr, int64_t ts);
    bool _parse_stun_username(StunMessage* stun_msg, std::string* local_ufrag, std::string* remote_ufrag);

private:
    EventLoop* _el;
    std::string _transport_name;
    IceCandidateComponent _component;
    IceParameters _ice_params;
    int _socket = -1;
    std::unique_ptr<AsyncUdpSocket> _async_socket;
    rtc::SocketAddress _local_addr;
    std::vector<Candidate> _candidates;
    AddressMap _connections;
};

}  // namespace xrtc

#endif  //__UDP_PORT_H_
