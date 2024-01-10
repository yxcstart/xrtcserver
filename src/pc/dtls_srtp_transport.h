#ifndef __DTLS_SRTP_TRANSPORT_H_
#define __DTLS_SRTP_TRANSPORT_H_

#include <string>
#include "pc/srtp_transport.h"

namespace xrtc {

class DtlsTransport;

class DtlsSrtpTransport : public SrtpTransport {
public:
    DtlsSrtpTransport(const std::string& transport_name, bool rtcp_mux_enabled);

    void set_dtls_transports(DtlsTransport* rtp_dtls_transport, DtlsTransport* rtcp_dtls_transport);

private:
    std::string _transport_name;
    DtlsTransport* _rtp_dtls_transport = nullptr;
    DtlsTransport* _rtcp_dtls_transport = nullptr;
};

}  // namespace xrtc

#endif  //__DTLS_SRTP_TRANSPORT_H_