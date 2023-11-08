#include "ice/ice_transport_channel.h"
#include <rtc_base/logging.h>

namespace xrtc {

IceTransportChannel::IceTransportChannel(EventLoop* el, PortAllocator* allocator, const std::string& transport_name,
                                         IceCandidateComponent component)
    : _el(el), _transport_name(transport_name), _component(component) {
    RTC_LOG(LS_INFO) << "ice transport channel created, transport_name: " << _transport_name
                     << ", component: " << _component;
}

IceTransportChannel::~IceTransportChannel() {}

void IceTransportChannel::gathering_candidate() {}
}  // namespace xrtc