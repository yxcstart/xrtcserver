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
    void _on_check_and_ping();
    void _on_connection_state_change(IceConnection* conn);
    void _on_connection_destroyed(IceConnection* conn);
    void _ping_connection(IceConnection* conn);
    void _maybe_switch_selected_connection(IceConnection* conn);
    void _switch_selected_connection(IceConnection* conn);
    friend void ice_ping_cb(EventLoop* /*el*/, TimeWatcher* /*w*/, void* data);

private:
    EventLoop* _el;
    PortAllocator* _allocator;
    std::string _transport_name;
    IceCandidateComponent _component;
    IceParameters _ice_params;
    IceParameters _remote_ice_params;
    std::vector<Candidate> _local_candidates;
    std::unique_ptr<IceController> _ice_controller;
    bool _start_pinging = false;
    TimeWatcher* _ping_watcher = nullptr;
    int _cur_ping_interval = WEAK_PING_INTERVAL;
    int64_t _last_ping_sent_ms = 0;
    IceConnection* _selected_connection = nullptr;
};
}  // namespace xrtc

#endif  //__ICE_TRANSPORT_CHANNEL_H_