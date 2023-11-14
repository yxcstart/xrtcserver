#ifndef __UDP_PORT_H_
#define __UDP_PORT_H_

#include <string>
#include <vector>
#include "base/event_loop.h"
#include "base/network.h"
#include "ice/candidate.h"
#include "ice/ice_credentials.h"
#include "ice/ice_def.h"
#include "rtc_base/socket_address.h"

namespace xrtc {

class UDPPort {
public:
    UDPPort(EventLoop* el, const std::string& transport_name, IceCandidateComponent compoent, IceParameters ice_params);
    ~UDPPort();

    int create_ice_candidate(Network* network, int min_port, int max_port, Candidate& c);

private:
    EventLoop* _el;
    std::string _transport_name;
    IceCandidateComponent _component;
    IceParameters _ice_params;
    int _socket = -1;
    rtc::SocketAddress _local_addr;
    std::vector<Candidate> _candidates;
};

}  // namespace xrtc

#endif  //__UDP_PORT_H_
