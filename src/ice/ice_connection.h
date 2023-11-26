#ifndef __ICE_CONNECTION_H_
#define __ICE_CONNECTION_H_

#include "base/event_loop.h"
#include "ice/candidate.h"

namespace xrtc {

class UDPPort;

class IceConnection {
public:
    IceConnection(EventLoop* el, UDPPort* port, const Candidate& remote_candidate);
    ~IceConnection();

    const Candidate& remote_candidate() const { return _remote_candidate; }

private:
    EventLoop* _el;
    UDPPort* _port;
    Candidate _remote_candidate;
};

}  // namespace xrtc

#endif  //__ICE_CONNECTION_H_
