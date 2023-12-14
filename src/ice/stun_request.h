#ifndef __STUN_REQUEST_H_
#define __STUN_REQUEST_H_
#include <rtc_base/third_party/sigslot/sigslot.h>
#include <map>
#include "ice/stun.h"
namespace xrtc {

class StunRequest;

class StunRequestManager {
public:
    StunRequestManager() = default;
    ~StunRequestManager() = default;

    void send(StunRequest* request);
    bool check_response(StunMessage* msg);

    sigslot::signal3<StunRequest*, const char*, size_t> signal_send_packet;

private:
    typedef std::map<std::string, StunRequest*> RequestMap;
    RequestMap _requests;  // 缓存ping请求，等待客户端响应
};

class StunRequest {
public:
    StunRequest(StunMessage* request);
    virtual ~StunRequest();

    int type() { return _msg->type(); }
    const std::string& id() { return _msg->transaction_id(); }
    void set_manager(StunRequestManager* manager) { _manager = manager; }

    void construct();
    void send();

protected:
    virtual void prepare(StunMessage*) {}
    virtual void on_response(StunMessage*) {}
    virtual void on_error_response(StunMessage*) {}
    friend class StunRequestManager;

private:
    StunMessage* _msg;
    StunRequestManager* _manager = nullptr;
};

}  // namespace xrtc

#endif  //__STUN_REQUEST_H_