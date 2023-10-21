#ifndef __TCP_CONNECTION_H_
#define __TCP_CONNECTION_H_

#include <rtc_base/sds.h>
#include "base/event_loop.h"
#include "base/xhead.h"

namespace xrtc {
class TcpConnection {
public:
    enum { STATE_HEAD = 0, STATE_BODY = 1 };

    TcpConnection(int fd);
    ~TcpConnection();

public:
    int fd;
    char ip[64];
    int port;
    IOWatcher* io_watcher = nullptr;
    sds querybuf;
    size_t bytes_expected = XHEAD_SIZE;
    size_t bytes_processed = 0;
    int current_state = STATE_HEAD;
};
}  // namespace xrtc
#endif  //__TCP_CONNECTION_H_