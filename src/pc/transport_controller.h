#ifndef __TRANSPORT_CONTROLLER_H_
#define __TRANSPORT_CONTROLLER_H_

#include <map>
#include "ice/ice_agent.h"
#include "pc/session_description.h"
namespace xrtc {

class DtlsTransport;
class TransportController : public sigslot::has_slots<> {
public:
    TransportController(EventLoop* el, PortAllocator* allocator);
    ~TransportController();

    int set_local_description(SessionDescription* desc);
    int set_remote_description(SessionDescription* desc);
    void set_local_certificate(rtc::RTCCertificate* cert);

    sigslot::signal4<TransportController*, const std::string&, IceCandidateComponent, const std::vector<Candidate>&>
        signal_candidate_allocate_done;

private:
    void on_candidate_allocate_done(IceAgent* agent, const std::string& transport_name, IceCandidateComponent component,
                                    const std::vector<Candidate>& candidates);
    void _add_dtls_transport(DtlsTransport* dtls);

private:
    EventLoop* _el;
    IceAgent* _ice_agent;
    std::map<std::string, DtlsTransport*> _dtls_transport_by_name;
    rtc::RTCCertificate* _local_certificate = nullptr;
};

}  // namespace xrtc

#endif  // __TRANSPORT_CONTROLLER_H_