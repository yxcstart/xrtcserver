#include "server/tcp_connection.h"

namespace xrtc {
TcpConnection::TcpConnection(int fd) : fd(fd), querybuf(sdsempty()) {}

TcpConnection::~TcpConnection() {}

}  // namespace xrtc