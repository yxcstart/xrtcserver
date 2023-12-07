#ifndef __STUN_REQUEST_H_
#define __STUN_REQUEST_H_

#include "ice/stun.h"

namespace xrtc {
class StunRequest;
class StunRequestManager {
    StunRequestManager() = default;
    ~StunRequestManager() = default;

    void send(StunRequest* request);
};

class StunRequest {
public:
    StunRequest(StunMessage* request);
    virtual ~StunRequest();

    const std::string& id() { return _msg->transaction_id(); }
    void construct();

protected:
    virtual void prepare(StunMessage*) {}

private:
    StunMessage* _msg;
};

}  // namespace xrtc

#endif  //__STUN_REQUEST_H_