#ifndef __DTLS_TRANSPORT_H_
#define __DTLS_TRANSPORT_H_

#include <rtc_base/buffer_queue.h>
#include <rtc_base/rtc_certificate.h>
#include <rtc_base/ssl_stream_adapter.h>
#include <memory>
#include "ice/ice_transport_channel.h"

namespace xrtc {

enum class DtlsTransportState {
    k_new,
    k_connecting,
    k_connected,
    k_close,
    k_failed,
    k_num_values,
};

class StreamInterfaceChannel : public rtc::StreamInterface {
public:
    StreamInterfaceChannel(IceTransportChannel* ice_channel);

    bool on_received_packet(const char* data, size_t size);

    rtc::StreamState GetState() const override;
    rtc::StreamResult Read(void* buffer, size_t buffer_len, size_t* read, int* error) override;
    rtc::StreamResult Write(const void* data, size_t data_len, size_t* written, int* error) override;
    void Close() override;

private:
    IceTransportChannel* _ice_channel;
    rtc::BufferQueue _packets;
    rtc::StreamState _state = rtc::SS_OPEN;
};

class DtlsTransport : public sigslot::has_slots<> {
public:
    DtlsTransport(IceTransportChannel* ice_channel);
    ~DtlsTransport();

    const std::string& transport_name() { return _ice_channel->transport_name(); }
    IceCandidateComponent component() { return _ice_channel->component(); }

    bool set_local_certificate(rtc::RTCCertificate* cert);
    bool set_remote_fingerprint(const std::string& digest_alg, const uint8_t* digest, size_t digest_len);

    std::string to_string();

    sigslot::signal2<DtlsTransport*, DtlsTransportState> signal_dtls_state;
    sigslot::signal1<DtlsTransport*> signal_writable_state;

private:
    void _on_read_packet(IceTransportChannel* channel, const char* buf, size_t len, int64_t ts);
    bool _setup_dtls();
    void _maybe_start_dtls();
    void _set_dtls_state(DtlsTransportState state);
    void _set_writable_state(bool writable);
    bool _handle_dtls_packet(const char* data, size_t size);
    void _on_writable_state(IceTransportChannel* channel);

private:
    IceTransportChannel* _ice_channel;
    DtlsTransportState _dtls_state = DtlsTransportState::k_new;
    bool _receiving = false;
    bool _writable = false;
    std::unique_ptr<rtc::SSLStreamAdapter> _dtls;
    StreamInterfaceChannel* _downward = nullptr;
    rtc::Buffer _cached_client_hello;
    rtc::RTCCertificate* _local_certificate = nullptr;
    rtc::Buffer _remote_fingerprint_value;
    std::string _remote_fingerprint_alg;
    bool _dtls_active = false;
};

}  // namespace xrtc

#endif  //__DTLS_TRANSPORT_H_