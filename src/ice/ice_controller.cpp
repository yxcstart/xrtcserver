#include "ice/ice_controller.h"
#include <rtc_base/logging.h>
#include <rtc_base/time_utils.h>
#include "ice/ice_def.h"

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

PingResult IceController::select_connection_to_ping(int64_t last_ping_sent_ms) {
    bool need_ping_more_at_weak = false;
    for (auto conn : _connections) {
        if (conn->num_pings_sent() < MIN_PINGS_AT_WEAK_PING_INTERVAL) {
            need_ping_more_at_weak = true;
            break;
        }
    }

    int ping_interval = (_weak() || need_ping_more_at_weak) ? WEAK_PING_INTERVAL : STRONG_PING_INTERVAL;

    int64_t now = rtc::TimeMillis();
    const IceConnection* conn = nullptr;
    if (now >= last_ping_sent_ms + ping_interval) {
        conn = _find_next_pingable_connection(now);
    }

    return PingResult(conn, ping_interval);
}

const IceConnection* IceController::_find_next_pingable_connection(int64_t now) {
    if (_selected_connection && _selected_connection->writable() &&
        _is_connction_past_ping_interval(_selected_connection, now)) {
        return _selected_connection;
    }

    return nullptr;
}

bool IceController::_is_connction_past_ping_interval(const IceConnection* conn, int64_t now) {
    int interval = _get_connection_ping_interval(conn, now);
    return now >= conn->last_ping_sent() + interval;
}

int IceController::_get_connection_ping_interval(const IceConnection* conn, int64_t now) {
    if (conn->num_pings_sent() < MIN_PINGS_AT_WEAK_PING_INTERVAL) {
        return WEAK_PING_INTERVAL;
    }

    if (_weak() || !conn->stable(now)) {
        return STABLING_CONNECTION_PING_INTERVAL;
    }

    return STRONG_PING_INTERVAL;
}

}  // namespace xrtc