#ifndef __TCP_CONNECTION_H_
#define __TCP_CONNECTION_H_

#include "base/event_loop.h"

namespace xrtc {
class TcpConnection {
public:
    TcpConnection(int fd);
    ~TcpConnection();

public:
    int fd;
    char ip[64];
    int port;
    IOWatcher* io_watcher = nullptr;
};
}  // namespace xrtc
#endif  //__TCP_CONNECTION_H_