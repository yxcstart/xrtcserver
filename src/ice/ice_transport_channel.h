#ifndef __ICE_TRANSPORT_CHANNEL_H_
#define __ICE_TRANSPORT_CHANNEL_H_

#include <string>
#include <vector>
#include "base/event_loop.h"
#include "ice_def.h"

namespace xrtc {
class IceTransportChannel {
public:
    IceTransportChannel(EventLoop* el, const std::string& transport_name, IceCandidateComponent component);
    virtual ~IceTransportChannel();

    std::string transport_name() { return _transport_name; }
    IceCandidateComponent component() { return _component; }

private:
    EventLoop* _el;
    std::string _transport_name;
    IceCandidateComponent _component;
};
}  // namespace xrtc

#endif  //__ICE_TRANSPORT_CHANNEL_H_