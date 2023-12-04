#ifndef __ICE_TRANSPORT_CHANNEL_H_
#define __ICE_TRANSPORT_CHANNEL_H_

#include <rtc_base/third_party/sigslot/sigslot.h>
#include <string>
#include <vector>
#include "base/event_loop.h"
#include "ice/candidate.h"
#include "ice/ice_controller.h"
#include "ice/ice_credentials.h"
#include "ice/port_allocator.h"
#include "ice/stun.h"
#include "ice/udp_port.h"
#include "ice_def.h"

namespace xrtc {

class IceTransportChannel : public sigslot::has_slots<> {
public:
    IceTransportChannel(EventLoop* el, PortAllocator* allocator, const std::string& transport_name,
                        IceCandidateComponent component);
    virtual ~IceTransportChannel();

    std::string transport_name() { return _transport_name; }
    IceCandidateComponent component() { return _component; }

    void set_ice_params(const IceParameters& ice_params);
    void set_remote_ice_params(const IceParameters& ice_params);
    void gathering_candidate();

    std::string to_string();

    sigslot::signal2<IceTransportChannel*, const std::vector<Candidate>&> signal_candidate_allocate_done;

private:
    void _on_unknown_address(UDPPort* port, const rtc::SocketAddress& addr, StunMessage* msg,
                             const std::string& remote_ufrag);
    void _add_connection(IceConnection* conn);
    void _sort_connections_and_update_state();
    void _maybe_start_pinging();

private:
    EventLoop* _el;
    std::string _transport_name;
    IceCandidateComponent _component;
    PortAllocator* _allocator;
    IceParameters _ice_params;
    IceParameters _remote_ice_params;
    std::vector<Candidate> _local_candidates;
    std::unique_ptr<IceController> _ice_controller;
    bool _start_pinging = false;
};
}  // namespace xrtc

#endif  //__ICE_TRANSPORT_CHANNEL_H_