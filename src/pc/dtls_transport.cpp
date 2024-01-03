
#include "pc/dtls_transport.h"
#include <rtc_base/logging.h>

namespace xrtc {

const size_t k_dtls_record_header_len = 13;
const size_t k_max_dtls_packet_len = 2048;
const size_t k_max_pending_packets = 2;  // 存放包的个数
bool is_dtls_packet(const char* buf, size_t len) {
    const uint8_t* u = reinterpret_cast<const uint8_t*>(buf);
    return len >= k_dtls_record_header_len && (u[0] > 19 && u[0] < 64);
}

bool is_dtls_client_hello_packet(const char* buf, size_t len) {
    if (!is_dtls_packet(buf, len)) {
        return false;
    }
    const uint8_t* u = reinterpret_cast<const uint8_t*>(buf);
    return len > 17 && (u[0] == 22 && u[13] == 1);
}

StreamInterfaceChannel::StreamInterfaceChannel(IceTransportChannel* ice_channel)
    : _ice_channel(ice_channel), _packets(k_max_pending_packets, k_max_dtls_packet_len) {}

bool StreamInterfaceChannel::on_received_packet(const char* data, size_t size) {
    // 客户端发送数据过来，进入这个方法
    if (_packets.size() > 0) {
        RTC_LOG(LS_INFO) << ": Packet already in buffer queue";
    }

    if (!_packets.WriteBack(data, size, NULL)) {
        RTC_LOG(LS_WARNING) << ": Failed to write packet to queue";
    }

    // 发送信号给ssl adapter处理
    SignalEvent(this, rtc::SE_READ, 0);

    return true;
}

rtc::StreamState StreamInterfaceChannel::GetState() const { return _state; }

rtc::StreamResult StreamInterfaceChannel::Read(void* buffer, size_t buffer_len, size_t* read, int* error) {
    if (_state == rtc::SS_CLOSED) {
        return rtc::SR_EOS;
    }

    if (_state == rtc::SS_OPENING) {
        return rtc::SR_BLOCK;
    }

    if (!_packets.ReadFront(buffer, buffer_len, read)) {
        return rtc::SR_BLOCK;
    }

    return rtc::SR_SUCCESS;
}

rtc::StreamResult StreamInterfaceChannel::Write(const void* data, size_t data_len, size_t* written, int* error) {}

void StreamInterfaceChannel::Close() {}

DtlsTransport::DtlsTransport(IceTransportChannel* channel) : _ice_channel(channel) {
    _ice_channel->signal_read_packet.connect(this, &DtlsTransport::_on_read_packet);
    _ice_channel->signal_writable_state.connect(this, &DtlsTransport::_on_writable_state);
}

DtlsTransport::~DtlsTransport() {}

void DtlsTransport::_on_read_packet(IceTransportChannel* channel, const char* buf, size_t len, int64_t ts) {
    // RTC_LOG(LS_INFO) << "=============DTLS packet: " << len;
    switch (_dtls_state) {
        case DtlsTransportState::k_new:
            if (_dtls) {
                RTC_LOG(LS_INFO) << to_string() << ": Received packet before DTLS started.";
            } else {
                RTC_LOG(LS_WARNING) << to_string() << ": Received packet before we know if "
                                    << "we are doing DTLS or not";
            }
            if (is_dtls_client_hello_packet(buf, len)) {
                RTC_LOG(LS_INFO) << to_string() << ": Caching DTLS ClientHello packet until "
                                 << "DTLS started";
                _cached_client_hello.SetData(buf, len);
                if (!_dtls && _local_certificate) {
                    _setup_dtls();
                }
            } else {
                RTC_LOG(LS_WARNING) << to_string() << ": Not a DTLS ClientHello packet, "
                                    << "dropping";
            }

            break;
    }
}

void DtlsTransport::_on_writable_state(IceTransportChannel* channel) {
    RTC_LOG(LS_INFO) << to_string() << ": IceTransportChannel writable changed to " << channel->writable();

    if (!_dtls_active) {
        _set_writable_state(channel->writable());
        return;
    }

    switch (_dtls_state) {
        case DtlsTransportState::k_new:
            _maybe_start_dtls();
            break;
        case DtlsTransportState::k_connected:
            _set_writable_state(channel->writable());
            break;
        default:
            break;
    }
}

bool DtlsTransport::set_local_certificate(rtc::RTCCertificate* cert) {
    if (_dtls_active) {
        if (cert == _local_certificate) {
            RTC_LOG(LS_INFO) << to_string() << ": Ingnoring identical DTLS cert";
            return true;
        } else {
            RTC_LOG(LS_WARNING) << to_string() << ": Cannot change cert in this state";
            return false;
        }
    }

    if (cert) {
        _local_certificate = cert;
        _dtls_active = true;
    }

    return true;
}

bool DtlsTransport::set_remote_fingerprint(const std::string& digest_alg, const uint8_t* digest, size_t digest_len) {
    rtc::Buffer remote_fingerprint_value(digest, digest_len);

    if (_dtls_active && _remote_fingerprint_value == remote_fingerprint_value && !digest_alg.empty()) {
        RTC_LOG(LS_INFO) << to_string() << ": Ignoring identical remote fingerprint";
        return true;
    }

    if (digest_alg.empty()) {
        RTC_LOG(LS_WARNING) << to_string() << ": Other sides not support DTLS";
        _dtls_active = false;
        return false;
    }

    if (!_dtls_active) {
        RTC_LOG(LS_WARNING) << to_string() << ": Cannot set remote fingerpint in this state";
        return false;
    }

    bool fingerprint_change = _remote_fingerprint_value.size() > 0u;
    _remote_fingerprint_value = std::move(remote_fingerprint_value);
    _remote_fingerprint_alg = digest_alg;

    // ClientHello packet先到，answer sdp后到
    if (_dtls && !fingerprint_change) {
        rtc::SSLPeerCertificateDigestError err;
        if (!_dtls->SetPeerCertificateDigest(digest_alg, (const unsigned char*)digest, digest_len, &err)) {
            RTC_LOG(LS_WARNING) << to_string() << ": Failed to set peer certificate digest";
            _set_dtls_state(DtlsTransportState::k_failed);
            return err == rtc::SSLPeerCertificateDigestError::VERIFICATION_FAILED;
        }
        return true;
    }

    if (_dtls && fingerprint_change) {
        _dtls.reset(nullptr);
        _set_dtls_state(DtlsTransportState::k_new);
        _set_writable_state(false);
    }

    if (!_setup_dtls()) {
        RTC_LOG(LS_WARNING) << to_string() << ": Failed to setup DTLS";
        _set_dtls_state(DtlsTransportState::k_failed);
        return false;
    }

    return true;
}

void DtlsTransport::_set_dtls_state(DtlsTransportState state) {
    if (_dtls_state == state) {
        return;
    }
    RTC_LOG(LS_INFO) << to_string() << ": Change dtls state from " << _dtls_state << " to " << state;

    _dtls_state = state;
    signal_dtls_state(this, state);
}

void DtlsTransport::_set_writable_state(bool writable) {
    if (_writable == writable) {
        return;
    }
    RTC_LOG(LS_INFO) << to_string() << ": set DTLS writable to " << writable;
    _writable = writable;
    signal_writable_state(this);
}

bool DtlsTransport::_setup_dtls() {
    auto downward = std::make_unique<StreamInterfaceChannel>(_ice_channel);
    StreamInterfaceChannel* downward_ptr = downward.get();

    _dtls = rtc::SSLStreamAdapter::Create(std::move(downward));
    if (!_dtls) {
        RTC_LOG(LS_WARNING) << to_string() << ": Failed to create SSLStreamAdapter";
        return false;
    }

    _downward = downward_ptr;

    _dtls->SetIdentity(_local_certificate->identity()->Clone());
    _dtls->SetMode(rtc::SSL_MODE_DTLS);
    _dtls->SetMaxProtocolVersion(rtc::SSL_PROTOCOL_DTLS_12);
    _dtls->SetServerRole(rtc::SSL_SERVER);

    if (_remote_fingerprint_value.size() & !_dtls->SetPeerCertificateDigest(_remote_fingerprint_alg,
                                                                            _remote_fingerprint_value.data(),
                                                                            _remote_fingerprint_value.size())) {
        RTC_LOG(LS_WARNING) << to_string() << ": Failed to set remote fingerprint";
        return false;
    }

    RTC_LOG(LS_INFO) << to_string() << ": Setup DTLS complete";

    _maybe_start_dtls();

    return true;
}

void DtlsTransport::_maybe_start_dtls() {
    if (_dtls && _ice_channel->writable()) {
        if (_dtls->StartSSL()) {
            RTC_LOG(LS_WARNING) << to_string() << ": Failed to StartSSL.";
            _set_dtls_state(DtlsTransportState::k_failed);
            return;
        }

        RTC_LOG(LS_INFO) << to_string() << ": Started DTLS.";
        _set_dtls_state(DtlsTransportState::k_connecting);

        if (_cached_client_hello.size()) {
            if (!_handle_dtls_packet(_cached_client_hello.data<char>(), _cached_client_hello.size())) {
                RTC_LOG(LS_WARNING) << to_string() << ": Handling dtls packet failed.";
                _set_dtls_state(DtlsTransportState::k_failed);
            }
            _cached_client_hello.Clear();
        }
    }
}

bool DtlsTransport::_handle_dtls_packet(const char* data, size_t size) {
    const uint8_t* tmp_data = reinterpret_cast<const uint8_t*>(data);
    size_t tmp_size = size;

    while (tmp_size > 0) {
        if (tmp_size < k_dtls_record_header_len) {
            return false;
        }

        size_t record_len = (tmp_data[11] << 8) | tmp_data[12];
        if (record_len + k_dtls_record_header_len > tmp_size) {
            return false;
        }

        tmp_data += k_dtls_record_header_len + record_len;
        tmp_size -= k_dtls_record_header_len + record_len;
    }

    return _downward->on_received_packet(data, size);
}

std::string DtlsTransport::to_string() {
    std::stringstream ss;
    absl::string_view RECEIVING[2] = {"-", "R"};
    absl::string_view WRITABLE[2] = {"-", "W"};

    ss << "DtlsTransport[" << transport_name() << "]" << (int)component() << "|" << RECEIVING[_receiving] << "|"
       << WRITABLE[_writable] << "]";
    return ss.str();
}

}  // namespace xrtc
