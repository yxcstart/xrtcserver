#include "pc/peer_connection.h"

namespace xrtc {

PeerConnection::PeerConnection(EventLoop* el) : _el(el) {}
PeerConnection::~PeerConnection() {}

std::string PeerConnection::create_offer(const RTCOfferAnswerOptions& options) {
    _local_desc = std::make_unique<SessionDescription>(SdpType::k_offer);

    if (options.recv_audio) {
        auto audio = std::make_shared<AudioContentDescription>();
        _local_desc->add_content(audio);
    }

    if (options.recv_video) {
        auto video = std::make_shared<VideoContentDescription>();
        _local_desc->add_content(video);
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