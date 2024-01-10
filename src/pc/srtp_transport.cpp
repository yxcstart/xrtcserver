#include "pc/srtp_transport.h"

namespace xrtc {

SrtpTransport::SrtpTransport(bool rtcp_mux_enabled) : _rtcp_mux_enabled(rtcp_mux_enabled) {}
}  // namespace xrtc
