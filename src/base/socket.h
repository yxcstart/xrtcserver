#ifndef __BASE_SOCKET_H_
#define __BASE_SOCKET_H_

namespace xrtc {
int create_tcp_server(const char* addr, int port);
int tcp_accept(int sock, char* host, int* port);
int sock_setnonblock(int sock);
int sock_setnodelay(int sock);
int sock_peer_to_str(int sock, char* ip, int* port);
int sock_read_data(int sock, char* buf, size_t len);
int sock_write_data(int sock, const char* buf, size_t len);
}  // namespace xrtc

#endif __BASE_SOCKET_H_