#include "ice/ice_controller.h"
#include <rtc_base/logging.h>

namespace xrtc {

void IceController::add_connection(IceConnection* conn) { _connections.push_back(conn); }

bool IceController::has_pingable_connection() {
    for (auto conn : _connections) {
        if (_is_pingable(conn)) {
            return true;
        }
    }
    return false;
}

bool IceController::_is_pingable(IceConnection* conn) {
    const Candidate& remote = conn->remote_candidate();
    // client发送answer和联通性检查的时候，是并行的，谁先到达服务器不确定，如果联通性检查请求先到达服务器，服务器根据peer反射的candidate创建ice
    // connection，但是这个时候answer还没有到服务器，也就没有ufrag和pwd，联通性检查也没办法进行

    if (remote.username.empty() || remote.password.empty()) {
        RTC_LOG(LS_WARNING) << "remote ICE ufrag and pwd is empty, cannot ping.";
        return false;
    }

    if (_weak()) {
        return true;
    }

    return false;
}

}  // namespace xrtc