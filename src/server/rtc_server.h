
#ifndef __RTC_SERVER_H_
#define __RTC_SERVER_H_

#include <rtc_base/rtc_certificate.h>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include "base/event_loop.h"
#include "xrtcserver_def.h"

namespace xrtc {

struct RtcServerOptions {
    int worker_num;
};

class RtcWorker;

class RtcServer {
    friend void rtc_server_recv_notify(EventLoop*, IOWatcher*, int, int, void*);

public:
    enum { QUIT = 0, RTC_MSG = 1 };
    RtcServer();
    ~RtcServer();

    int init(const char* conf_file);
    bool start();
    void stop();
    int notify(int msg);
    void join();
    int send_rtc_msg(std::shared_ptr<RtcMsg> msg);
    void push_msg(std::shared_ptr<RtcMsg> msg);
    std::shared_ptr<RtcMsg> pop_msg();

private:
    void _process_notify(int msg);
    void _stop();
    int _create_worker(int worker_id);
    void _process_rtc_msg();
    RtcWorker* _get_worker(const std::string& stream_name);
    int _generate_and_check_certificate();

private:
    EventLoop* _el;
    RtcServerOptions _options;
    std::thread* _thread = nullptr;

    IOWatcher* _pipe_watcher = nullptr;
    int _notify_recv_fd = -1;
    int _notify_send_fd = -1;

    std::queue<std::shared_ptr<RtcMsg>> _q_msg;
    std::mutex _q_msg_mtx;

    std::vector<RtcWorker*> _workers;
    rtc::scoped_refptr<rtc::RTCCertificate> _certificate;
};

}  // namespace xrtc

#endif  //__RTC_SERVER_H_
