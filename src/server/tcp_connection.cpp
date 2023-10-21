#include "server/tcp_connection.h"

namespace xrtc {
TcpConnection::TcpConnection(int fd) : fd(fd) {}

TcpConnection::~TcpConnection() {}

}  // namespace xrtc