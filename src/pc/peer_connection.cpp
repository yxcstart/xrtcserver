#include "pc/peer_connection.h"
#include <rtc_base/logging.h>
#include "ice/ice_credentials.h"

namespace xrtc {

static RtpDirection get_direction(bool send, bool recv) {
    if (send && recv) {
        return RtpDirection::k_send_recv;
    } else if (send && !recv) {
        return RtpDirection::k_send_only;
    } else if (!send && recv) {
        return RtpDirection::k_recv_only;
    } else {
        return RtpDirection::k_inactive;
    }
}

PeerConnection::PeerConnection(EventLoop* el) : _el(el) {}
PeerConnection::~PeerConnection() {}

int PeerConnection::init(rtc::RTCCertificate* certificate) {
    _certificate = certificate;
    return 0;
}

std::string PeerConnection::create_offer(const RTCOfferAnswerOptions& options) {
    if (options.dtls_on && !_certificate) {
        RTC_LOG(LS_WARNING) << "certificate is null";
        return "";
    }

    _local_desc = std::make_unique<SessionDescription>(SdpType::k_offer);

    IceParameters ice_param = IceCredentials::create_random_ice_credentials();

    if (options.recv_audio) {
        auto audio = std::make_shared<AudioContentDescription>();
        audio->set_direction(get_direction(options.send_audio, options.recv_audio));
        audio->set_rtcp_mux(options.use_rtcp_mux);
        _local_desc->add_content(audio);
        _local_desc->add_transport_info(audio->mid(), ice_param, _certificate);
    }

    if (options.recv_video) {
        auto video = std::make_shared<VideoContentDescription>();
        video->set_direction(get_direction(options.send_video, options.recv_video));
        video->set_rtcp_mux(options.use_rtcp_mux);
        _local_desc->add_content(video);
        _local_desc->add_transport_info(video->mid(), ice_param, _certificate);
    }

    if (options.use_rtp_mux) {
        ContentGroup offer_bundle("BUNDLE");
        for (auto content : _local_desc->contents()) {
            offer_bundle.add_content_name(content->mid());
        }

        if (!offer_bundle.content_names().empty()) {
            _local_desc->add_group(offer_bundle);
        }
    }

    return _local_desc->to_string();
}

}  // namespace xrtc