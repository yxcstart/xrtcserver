#include "ice/ice_connection.h"

namespace xrtc {

IceConnection::IceConnection(EventLoop* el, UDPPort* port, const Candidate& remote_candidate)
    : _el(el), _port(port), _remote_candidate(remote_candidate) {}

IceConnection::~IceConnection() {}
}  // namespace xrtc