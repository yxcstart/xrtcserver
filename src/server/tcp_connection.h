#ifndef __TCP_CONNECTION_H_
#define __TCP_CONNECTION_H_

#include <rtc_base/sds.h>
#include <rtc_base/slice.h>
#include <list>
#include "base/event_loop.h"
#include "base/xhead.h"

namespace xrtc {
class TcpConnection {
public:
    enum {
        STATE_HEAD = 0,
        STATE_BODY = 1,
    };

    TcpConnection(int fd);
    ~TcpConnection();

public:
    int fd;
    char ip[64];
    int port;
    IOWatcher* io_watcher = nullptr;
    TimeWatcher* timer_watcher = nullptr;
    sds querybuf;
    size_t bytes_expected = XHEAD_SIZE;
    size_t bytes_processed = 0;
    int current_state = STATE_HEAD;
    unsigned long last_interaction = 0;
    std::list<rtc::Slice> reply_list;
    size_t cur_resp_pos = 0;
};
}  // namespace xrtc
#endif  //__TCP_CONNECTION_H_