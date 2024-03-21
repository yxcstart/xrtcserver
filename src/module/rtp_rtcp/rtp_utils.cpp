
#include "module/rtp_rtcp/rtp_utils.h"

namespace xrtc {

RtpPacketType infer_rtp_packet_type(rtc::ArrayView<const char> packet) { return RtpPacketType::k_unknown; }

}  // namespace xrtc