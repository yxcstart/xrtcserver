#ifndef __ICE_TRANSPORT_CHANNEL_H_
#define __ICE_TRANSPORT_CHANNEL_H_

#include <string>
#include <vector>
#include "base/event_loop.h"
#include "ice/ice_credentials.h"
#include "ice/port_allocator.h"
#include "ice_def.h"
namespace xrtc {
class IceTransportChannel {
public:
    IceTransportChannel(EventLoop* el, PortAllocator* allocator, const std::string& transport_name,
                        IceCandidateComponent component);
    virtual ~IceTransportChannel();

    std::string transport_name() { return _transport_name; }
    IceCandidateComponent component() { return _component; }

    void set_ice_params(const IceParameters& ice_params);
    void gathering_candidate();

private:
    EventLoop* _el;
    std::string _transport_name;
    IceCandidateComponent _component;
    PortAllocator* _allocator;
    IceParameters _ice_params;
};
}  // namespace xrtc

#endif  //__ICE_TRANSPORT_CHANNEL_H_