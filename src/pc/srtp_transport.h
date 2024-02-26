#ifndef __SRTP_TRANSPORT_H_
#define __SRTP_TRANSPORT_H_

#include <memory>
#include <string>
#include <vector>
#include "pc/srtp_session.h"

namespace xrtc {

class SrtpTransport {
public:
    SrtpTransport(bool rtcp_mux_enabled);
    virtual ~SrtpTransport() = default;

    bool set_rtp_params(int send_cs, const uint8_t* send_key, size_t send_key_len,
                        const std::vector<int>& send_extension_ids, int recv_cs, const uint8_t* recv_key,
                        size_t recv_key_len, const std::vector<int>& recv_extension_ids);
    void reset_params();

private:
    void _create_srtp_session();

private:
    bool _rtcp_mux_enabled;
    std::unique_ptr<SrtpSession> _send_session;
    std::unique_ptr<SrtpSession> _recv_session;
};

}  // namespace xrtc

#endif  //__SRTP_TRANSPORT_H_