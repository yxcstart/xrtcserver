#include "server/signaling_worker.h"
#include <rtc_base/logging.h>
#include <unistd.h>
#include "base/socket.h"
#include "server/tcp_connection.h"

namespace xrtc {

void signaling_worker_recv_notify(EventLoop* el, IOWatcher* w, int fd, int events, void* data) {
    int msg;
    if (read(fd, &msg, sizeof(int)) != sizeof(int)) {
        RTC_LOG(LS_WARNING) << "read from pipe error: " << strerror(errno) << ", errno: " << errno;
        return;
    }
    SignalingWorker* worker = (SignalingWorker*)data;
    worker->_process_notify(msg);
}

SignalingWorker::SignalingWorker(int worker_id) : _worker_id(worker_id), _el(new EventLoop(this)) {}

SignalingWorker::~SignalingWorker() {}

int SignalingWorker::init() {
    int fds[2];
    if (pipe(fds)) {
        RTC_LOG(LS_WARNING) << "create pipe error: " << strerror(errno) << ", errno: " << errno;
        return -1;
    }

    _notify_recv_fd = fds[0];
    _notify_send_fd = fds[1];

    _pipe_watcher = _el->create_io_event(signaling_worker_recv_notify, this);
    _el->start_io_event(_pipe_watcher, _notify_recv_fd, EventLoop::READ);

    return 0;
}

bool SignalingWorker::start() {
    if (_thread) {
        RTC_LOG(LS_WARNING) << "signaling worker already start, worker_id: " << _worker_id;
        return false;
    }

    _thread = new std::thread([=]() {
        RTC_LOG(LS_INFO) << "signaling worker event loop start, worker_id: " << _worker_id;
        _el->start();
        RTC_LOG(LS_INFO) << "signaling worker event loop stop, worker_id: " << _worker_id;
    });

    return true;
}

void SignalingWorker::stop() { notify(SignalingWorker::QUIT); }

int SignalingWorker::notify(int msg) {
    int written = write(_notify_send_fd, &msg, sizeof(int));
    return written == sizeof(int) ? 0 : -1;
}

void SignalingWorker::_stop() {
    if (!_thread) {
        RTC_LOG(LS_WARNING) << "signaling worker not running, worker_id: " << _worker_id;
        return;
    }

    _el->delete_io_event(_pipe_watcher);
    _el->stop();

    close(_notify_recv_fd);
    close(_notify_send_fd);
}

void SignalingWorker::_process_notify(int msg) {
    switch (msg) {
        case QUIT:
            _stop();
            break;
        case NEW_CONN:
            int fd;
            if (_q_conn.consume(&fd)) {
                _new_conn(fd);
            }
            break;
        default:
            RTC_LOG(LS_WARNING) << "unknown msg: " << msg;
            break;
    }
}

void SignalingWorker::join() {
    if (_thread && _thread->joinable()) {
        _thread->join();
    }
}

void conn_io_cb(EventLoop* /*el*/, IOWatcher* /*w*/, int fd, int events, void* data) {
    SignalingWorker* worker = (SignalingWorker*)data;
    if (events & EventLoop::READ) {
        worker->_read_query(fd);
    }
}

void SignalingWorker::_new_conn(int fd) {
    RTC_LOG(LS_INFO) << "signaling worker " << _worker_id << ", receive fd: " << fd;
    if (fd < 0) {
        RTC_LOG(LS_WARNING) << "invalid fd: " << fd;
        return;
    }

    sock_setnonblock(fd);
    sock_setnodelay(fd);

    TcpConnection* c = new TcpConnection(fd);
    sock_peer_to_str(fd, c->ip, &(c->port));
    c->io_watcher = _el->create_io_event(conn_io_cb, this);
    _el->start_io_event(c->io_watcher, fd, EventLoop::READ);

    if ((size_t)fd >= _conns.size()) {
        _conns.resize(fd * 2, nullptr);
    }

    _conns[fd] = c;
}

void SignalingWorker::_read_query(int fd) {
    RTC_LOG(LS_INFO) << "signaling worker " << _worker_id << " receive read event, fd: " << fd;
}

int SignalingWorker::notify_new_conn(int fd) {
    _q_conn.produce(fd);
    return notify(SignalingWorker::NEW_CONN);
}
}  // namespace xrtc