#include "ice/stun_request.h"

namespace xrtc {

void StunRequestManager::send(StunRequest* request) {
    request->set_manager(this);
    request->construct();
    _requests[request->id()] = request;
    request->send();
}

StunRequest::StunRequest(StunMessage* msg) : _msg(msg) {}
StunRequest::~StunRequest() {}

void StunRequest::construct() { prepare(_msg); }

void StunRequest::send() {
    rtc::ByteBufferWriter buf;
    if (!_msg->write(&buf)) {
        return;
    }
    _manager->signal_send_packet(this, buf.Data(), buf.Length());
}

}  // namespace xrtc