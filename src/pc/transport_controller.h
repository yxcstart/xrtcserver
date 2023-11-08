#ifndef __TRANSPORT_CONTROLLER_H_
#define __TRANSPORT_CONTROLLER_H_

#include "ice/ice_agent.h"
#include "pc/session_description.h"

namespace xrtc {

class TransportController {
public:
    TransportController(EventLoop* el, PortAllocator* allocator);
    ~TransportController();

    int set_local_description(SessionDescription* desc);

private:
    EventLoop* _el;
    IceAgent* _ice_agent;
};

}  // namespace xrtc

#endif  // __TRANSPORT_CONTROLLER_H_