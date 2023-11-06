
#include "ice_transport_channel.h"

namespace xrtc {

IceTransportChannel::IceTransportChannel(EventLoop* el, const std::string& transport_name,
                                         IceCandidateComponent component)
    : _el(el), _transport_name(transport_name), _component(component) {}

IceTransportChannel::~IceTransportChannel() {}

}  // namespace xrtc