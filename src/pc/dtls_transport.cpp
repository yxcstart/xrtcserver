
#include "pc/dtls_transport.h"
#include <rtc_base/logging.h>

namespace xrtc {

DtlsTransport::DtlsTransport(IceTransportChannel* channel) : _ice_channel(channel) {
    _ice_channel->signal_read_packet.connect(this, &DtlsTransport::_on_read_packet);
}

DtlsTransport::~DtlsTransport() {}

void DtlsTransport::_on_read_packet(IceTransportChannel* channel, const char* buf, size_t len, int64_t ts) {
    RTC_LOG(LS_INFO) << "=============DTLS packet: " << len;
}

}  // namespace xrtc
