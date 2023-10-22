

#include "server/rtc_server.h"
#include <rtc_base/logging.h>
#include <unistd.h>
#include <yaml-cpp/yaml.h>

namespace xrtc {

void rtc_server_recv_notify(EventLoop* /*el*/, IOWatcher* /*w*/, int fd, int /*events*/, void* data) {
    int msg;
    if (read(fd, &msg, sizeof(int)) != sizeof(int)) {
        RTC_LOG(LS_WARNING) << "read from pipe error: " << strerror(errno) << ", errno: " << errno;
        return;
    }

    RtcServer* server = (RtcServer*)data;
    server->_process_notify(msg);
}
RtcServer::RtcServer() : _el(new EventLoop(this)) {}
RtcServer::~RtcServer() {}

int RtcServer::init(const char* conf_file) {
    if (!conf_file) {
        RTC_LOG(LS_WARNING) << "conf_file is null";
        return -1;
    }

    try {
        YAML::Node config = YAML::LoadFile(conf_file);
        RTC_LOG(LS_INFO) << "rtc server options:\n" << config;
        _options.worker_num = config["worker_num"].as<int>();
    } catch (YAML::Exception e) {
        RTC_LOG(LS_WARNING) << "rtc server load conf file error: " << e.msg;
        return -1;
    }

    int fds[2];
    if (pipe(fds)) {
        RTC_LOG(LS_WARNING) << "create pipe error: " << strerror(errno) << ", errno: " << errno;
        return -1;
    }

    _notify_recv_fd = fds[0];
    _notify_send_fd = fds[1];

    _pipe_watcher = _el->create_io_event(rtc_server_recv_notify, this);
    _el->start_io_event(_pipe_watcher, _notify_recv_fd, EventLoop::READ);

    return 0;
}

bool RtcServer::start() {
    if (_thread) {
        RTC_LOG(LS_WARNING) << "rtc server already start";
        return false;
    }
    _thread = new std::thread([=]() {
        RTC_LOG(LS_INFO) << "rtc server event loop start";
        _el->start();
        RTC_LOG(LS_INFO) << "rtc server event loop stop";
    });

    return true;
}

void RtcServer::stop() { notify(QUIT); }

int RtcServer::notify(int msg) {
    int written = write(_notify_send_fd, &msg, sizeof(int));
    return written == sizeof(int) ? 0 : -1;
}

void RtcServer::join() {
    if (!_thread && _thread->joinable()) {
        _thread->join();
    }
}

void RtcServer::push_msg(std::shared_ptr<RtcMsg> msg) {
    std::unique_lock<std::mutex> lock(_q_msg_mtx);
    _q_msg.push(msg);
}

std::shared_ptr<RtcMsg> RtcServer::pop_msg() {
    std::unique_lock<std::mutex> lock(_q_msg_mtx);
    if (_q_msg.empty()) {
        return nullptr;
    }

    std::shared_ptr<RtcMsg> msg = _q_msg.front();
    _q_msg.pop();
    return msg;
}

void RtcServer::_stop() {
    _el->delete_io_event(_pipe_watcher);
    _el->stop();
    close(_notify_recv_fd);
    close(_notify_send_fd);
    RTC_LOG(LS_INFO) << "rtc server stop";
}

int RtcServer::send_rtc_msg(std::shared_ptr<RtcMsg> msg) {
    push_msg(msg);
    return notify(RTC_MSG);
}

void RtcServer::_process_rtc_msg() {
    std::shared_ptr<RtcMsg> msg = pop_msg();
    if (!msg) {
        return;
    }
    RTC_LOG(LS_INFO) << "===========cmdno: " << msg->cmdno << ", uid: " << msg->uid;
}

void RtcServer::_process_notify(int msg) {
    switch (msg) {
        case QUIT:
            _stop();
            break;
        case RTC_MSG:
            _process_rtc_msg();
            break;
        default:
            RTC_LOG(LS_WARNING) << "unknown msg: " << msg;
            break;
    }
}

}  // namespace xrtc