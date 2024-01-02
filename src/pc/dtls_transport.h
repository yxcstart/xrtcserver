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

    rtc::StreamState GetState() const override;
    rtc::StreamResult Read(void* buffer, size_t buffer_len, size_t* read, int* error) override;
    rtc::StreamResult Write(const void* data, size_t data_len, size_t* written, int* error) override;
    void Close() override;

private:
    IceTransportChannel* _ice_channel;
};

class DtlsTransport : public sigslot::has_slots<> {
public:
    DtlsTransport(IceTransportChannel* ice_channel);
    ~DtlsTransport();

    const std::string& transport_name() { return _ice_channel->transport_name(); }
    IceCandidateComponent component() { return _ice_channel->component(); }

    std::string to_string();

private:
    void _on_read_packet(IceTransportChannel* channel, const char* buf, size_t len, int64_t ts);
    bool _setup_dtls();
    bool _maybe_start_dtls();

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
};

}  // namespace xrtc

#endif  //__DTLS_TRANSPORT_H_