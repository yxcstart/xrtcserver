#include "ice_agent.h"
#include <algorithm>

namespace xrtc {

IceAgent::IceAgent(EventLoop* el) : _el(el) {}
IceAgent::~IceAgent() {}

IceTransportChannel* IceAgent::get_channel(const std::string& transport_name, IceCandidateComponent component) {
    auto iter = _get_channel(transport_name, component);
    return iter == _channels.end() ? nullptr : *iter;
}

bool IceAgent::create_channel(EventLoop* el, const std::string& transport_name, IceCandidateComponent component) {
    if (get_channel(transport_name, component)) {
        return true;
    }

    auto channel = new IceTransportChannel(el, transport_name, component);
    _channels.push_back(channel);
    return true;
}

std::vector<IceTransportChannel*>::iterator IceAgent::_get_channel(const std::string& transport_name,
                                                                   IceCandidateComponent component) {
    return std::find_if(_channels.begin(), _channels.end(), [transport_name, component](IceTransportChannel* channel) {
        return transport_name == channel->transport_name() && component == channel->component();
    });
}

}  // namespace xrtc