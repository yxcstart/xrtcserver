
#ifndef __MODULE_RTP_UTILS_H_
#define __MODULE_RTP_UTILS_H_
#include <api/array_view.h>
namespace xrtc {
enum class RtpPacketType {
    k_rtp,
    k_rtcp,
    k_unknown,
};

RtpPacketType infer_rtp_packet_type(rtc::ArrayView<const char> packet);

}  // namespace xrtc

#endif  //__MODULE_RTP_UTILS_H_