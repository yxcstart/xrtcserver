
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

uint16_t parse_rtp_sequence_number(rtc::ArrayView<const uint8_t> packet);
uint32_t parse_rtp_ssrc(rtc::ArrayView<const uint8_t> packet);
bool get_rtcp_type(const void* data, size_t len, int* type);
}  // namespace xrtc

#endif  //__MODULE_RTP_UTILS_H_