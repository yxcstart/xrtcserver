#ifndef __BASE_SOCKET_H_
#define __BASE_SOCKET_H_

namespace xrtc {
int create_tcp_server(const char* addr, int port);
int tcp_accept(int sock, char* host, int* port);
}  // namespace xrtc

#endif __BASE_SOCKET_H_