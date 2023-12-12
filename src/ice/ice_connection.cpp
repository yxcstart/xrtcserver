#include "ice/ice_connection.h"
#include "ice/udp_port.h"
#include "rtc_base/logging.h"
namespace xrtc {

ConnectionRequest::ConnectionRequest(IceConnection* conn) : StunRequest(new StunMessage()), _connection(conn) {}

void ConnectionRequest::prepare(StunMessage* msg) {
    msg->set_type(STUN_BINDING_REQUEST);
    std::string username;
    // 积极提名
    _connection->port()->create_stun_username(_connection->remote_candidate().username, &username);
    msg->add_attribute(std::make_unique<StunByteStringAttribute>(STUN_ATTR_USERNAME, username));
    msg->add_attribute(std::make_unique<StunUInt64Attribute>(STUN_ATTR_ICE_CONTROLLING, 0));
    msg->add_attribute(std::make_unique<StunByteStringAttribute>(STUN_ATTR_USERNAME, 0));

    // priority
    int type_pref = ICE_TYPE_PREFERENCE_PRFLX;
    uint32_t prflx_priority = (type_pref << 24) | (_connection->local_candidate().priority & 0x00FFFFFF);
    msg->add_attribute(std::make_unique<StunUInt32Attribute>(STUN_ATTR_PRIORITY, prflx_priority));
    msg->add_message_integrity(_connection->remote_candidate().password);
    msg->add_fingerprint();
}

const Candidate& IceConnection::local_candidate() const { return _port->candidates()[0]; }

IceConnection::IceConnection(EventLoop* el, UDPPort* port, const Candidate& remote_candidate)
    : _el(el), _port(port), _remote_candidate(remote_candidate) {
    _requests.signal_send_packet.connect(this, &IceConnection::_on_stun_send_packet);
}

IceConnection::~IceConnection() {}

void IceConnection::_on_stun_send_packet(StunRequest* request, const char* buf, size_t len) {
    int ret = _port->send_to(buf, len, _remote_candidate.address);
    if (ret < 0) {
        RTC_LOG(LS_WARNING) << to_string() << ": Failed to send STUN binding request: ret=" << ret
                            << ", id=" << rtc::hex_encode(request->id());
    }
}

void IceConnection::handle_stun_binding_request(StunMessage* stun_msg) {
    // role的冲突问题

    // 发送binding response
    send_stun_binding_response(stun_msg);
}

void IceConnection::send_stun_binding_response(StunMessage* stun_msg) {
    const StunByteStringAttribute* username_attr = stun_msg->get_byte_string(STUN_ATTR_USERNAME);
    if (!username_attr) {
        RTC_LOG(LS_WARNING) << "send stun binding response error: no username";
        return;
    }

    StunMessage response;
    response.set_type(STUN_BINDING_RESPONSE);
    response.set_transaction_id(stun_msg->transaction_id());
    response.add_attribute(
        std::make_unique<StunXorAddressAttribute>(STUN_ATTR_XOR_MAPPED_ADDRESS, remote_candidate().address));
    response.add_message_integrity(_port->ice_pwd());
    response.add_fingerprint();

    send_response_message(response);
}

void IceConnection::send_response_message(const StunMessage& response) {
    const rtc::SocketAddress& addr = _remote_candidate.address;
    rtc::ByteBufferWriter buf;
    if (!response.write(&buf)) {
        return;
    }

    int ret = _port->send_to(buf.Data(), buf.Length(), addr);
    if (ret < 0) {
        RTC_LOG(LS_WARNING) << to_string() << ": send " << stun_method_to_string(response.type())
                            << " error, addr=" << addr.ToString()
                            << ", id=" << rtc::hex_encode(response.transaction_id());
        return;
    }
    RTC_LOG(LS_INFO) << to_string() << ": sent " << stun_method_to_string(response.type())
                     << " addr=" << addr.ToString() << ", id=" << rtc::hex_encode(response.transaction_id());
}

void IceConnection::on_read_packet(const char* buf, size_t len, int64_t /*ts*/) {
    std::unique_ptr<StunMessage> stun_msg;
    std::string remote_ufrag;
    const Candidate& remote = _remote_candidate;

    if (!_port->get_stun_message(buf, len, remote.address, &stun_msg, &remote_ufrag)) {
        // 这个不是stun包，可能是其它的比如dtls或者rtp包
    } else if (!stun_msg) {
        // stun包解析空指针，有问题
    } else {
        switch (stun_msg->type()) {
            case STUN_BINDING_REQUEST:  // 保活请求
                if (remote_ufrag != remote.username) {
                    RTC_LOG(LS_WARNING) << to_string() << ": Received " << stun_method_to_string(stun_msg->type())
                                        << " with bad username=" << remote_ufrag
                                        << ", id=" << rtc::hex_encode(stun_msg->transaction_id());
                    _port->send_binding_error_response(stun_msg.get(), remote.address, STUN_ERROR_UNAUTHORIZED,
                                                       STUN_ERROR_REASON_UNAUTHORIZED);
                } else {
                    RTC_LOG(LS_INFO) << to_string() << ": Received " << stun_method_to_string(stun_msg->type())
                                     << ", id=" << rtc::hex_encode(stun_msg->transaction_id());

                    handle_stun_binding_request(stun_msg.get());
                }
                break;
            default:
                break;
        }
    }
}

void IceConnection::maybe_set_remote_ice_params(const IceParameters& ice_params) {
    if (_remote_candidate.username == ice_params.ice_ufrag && _remote_candidate.password.empty()) {
        _remote_candidate.password = ice_params.ice_pwd;
    }
}

bool IceConnection::stable(int64_t now) const {
    // todo
    return false;
}

void IceConnection::ping(int64_t now) {
    ConnectionRequest* request = new ConnectionRequest(this);
    _ping_since_last_response.push_back(SentPing(request->id(), now));
    _requests.send(request);
    _num_pings_sent++;
}

std::string IceConnection::to_string() {
    std::stringstream ss;
    ss << "Conn[" << this << ":" << _port->transport_name() << ":" << _port->component() << ":"
       << _port->local_addr().ToString() << "->" << _remote_candidate.address.ToString();
    return ss.str();
}

}  // namespace xrtc