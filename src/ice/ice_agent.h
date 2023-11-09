#ifndef __ICE_AGENT_H_
#define __ICE_AGENT_H_

#include <vector>
#include "base/event_loop.h"
#include "ice_transport_channel.h"

namespace xrtc {

class IceAgent {
public:
    IceAgent(EventLoop* el, PortAllocator* allocator);
    ~IceAgent();

    bool create_channel(EventLoop* el, const std::string& transport_name, IceCandidateComponent component);

    IceTransportChannel* get_channel(const std::string& transport_name, IceCandidateComponent component);

    void set_ice_params(const std::string& transport_name, IceCandidateComponent component,
                        const IceParameters& ice_params);

    void gathering_candidate();

private:
    std::vector<IceTransportChannel*>::iterator _get_channel(const std::string& transport_name,
                                                             IceCandidateComponent component);

private:
    EventLoop* _el;
    std::vector<IceTransportChannel*> _channels;
    PortAllocator* _allocator;
};

}  // namespace xrtc

#endif  //__ICE_AGENT_H_