#include "ice/stun_request.h"
#include <rtc_base/logging.h>
#include <rtc_base/string_encode.h>
#include <rtc_base/time_utils.h>
namespace xrtc {

void StunRequestManager::send(StunRequest* request) {
    request->set_manager(this);
    request->construct();
    _requests[request->id()] = request;
    request->send();
}

bool StunRequestManager::check_response(StunMessage* msg) {
    auto iter = _requests.find(msg->transaction_id());
    if (iter == _requests.end()) {
        return false;
    }

    StunRequest* request = iter->second;
    if (msg->type() == get_stun_success_response(request->type())) {
        request->on_request_response(msg);
    } else if (msg->type() == get_stun_error_response(request->type())) {
        request->on_request_error_response(msg);
    } else {
        RTC_LOG(LS_WARNING) << "Received STUN binding response with wrong type=" << msg->type()
                            << ", id=" << rtc::hex_encode(msg->transaction_id());
        return false;
    }

    return true;
}

StunRequest::StunRequest(StunMessage* msg) : _msg(msg) {}
StunRequest::~StunRequest() {}

void StunRequest::construct() { prepare(_msg); }

int StunRequest::elapsed() { return rtc::TimeMillis() - _ts; }

void StunRequest::send() {
    rtc::ByteBufferWriter buf;
    _ts = rtc::TimeMillis();
    if (!_msg->write(&buf)) {
        return;
    }
    _manager->signal_send_packet(this, buf.Data(), buf.Length());
}

}  // namespace xrtc