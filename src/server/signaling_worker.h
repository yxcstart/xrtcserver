#ifndef __SIGNALING_WORKER_H_
#define __SIGNALING_WORKER_H_

#include <thread>
#include "base/event_loop.h"
#include "base/lock_free_queue.h"

namespace xrtc {

class SignalingWorker {
    friend void signaling_worker_recv_notify(EventLoop* el, IOWatcher* w, int fd, int events, void* data);

   public:
    enum { QUIT = 0, NEW_CONN = 1 };

    SignalingWorker(int worker_id);
    ~SignalingWorker();

    int init();
    bool start();
    void stop();
    int notify(int msg);
    void join();
    int notify_new_conn(int fd);

   private:
    void _process_notify(int msg);
    void _stop();
    void _new_conn(int fd);

   private:
    int _worker_id;
    EventLoop* _el;
    IOWatcher* _pipe_watcher = nullptr;
    int _notify_recv_fd = -1;
    int _notify_send_fd = -1;

    std::thread* _thread = nullptr;
    LockFreeQueue<int> _q_conn;
};

}  // namespace xrtc

#endif  //__SIGNALING_WORKER_H_