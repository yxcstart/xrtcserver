#include "ice/port_allocator.h"

namespace xrtc {

PortAllocator::PortAllocator() : _network_manager(new NetworkManager()) { _network_manager->create_networks(); }

PortAllocator::~PortAllocator() = default;

const std::vector<Network*>& PortAllocator::get_networks() { return _network_manager->get_network(); }

}  // namespace xrtc