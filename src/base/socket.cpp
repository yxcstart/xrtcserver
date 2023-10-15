#include <arpa/inet.h>
#include <rtc_base/logging.h>
#include <sys/socket.h>
#include <unistd.h>
namespace xrtc {

int create_tcp_server(const char* addr, int port) {
    // 1. 创建socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock) {
        RTC_LOG(LS_WARNING) << "create socket error, errno: " << errno << ", error: " << strerror(errno);
        return -1;
    }

    // 2. 设置SO_REUSEADDR  在该端口有客户端连接的情况下，仍然可以绑定端口
    int on = 1;
    int ret = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if (-1 == ret) {
        RTC_LOG(LS_WARNING) << "setsockopt SO_REUSEADDR error, errno: " << errno << ", error: " << strerror(errno);
        close(sock);
        return -1;
    }

    // 3. 创建addr
    struct sockaddr_in sa;
    sa.sin_family = AF_INET;                 // 表示使用IPv4协议。
    sa.sin_port = htons(port);               // htons函数将16位的主机字节序转换为网络字节序
    sa.sin_addr.s_addr = htonl(INADDR_ANY);  // htonl函数将32位的主机字节序转换为网络字节序

    if (addr &&
        inet_aton(addr, &sa.sin_addr) == 0)  // inet_aton是一个将字符串IP地址转换为网络字节序的32位二进制地址的函数
    {
        RTC_LOG(LS_WARNING) << "invalid address";
        close(sock);
        return -1;
    }
    // 4. bind
    ret = bind(sock, (struct sockaddr*)&sa, sizeof(sa));
    if (-1 == ret) {
        RTC_LOG(LS_WARNING) << "bind error, errno: " << errno << ", error: " << strerror(errno);
        close(sock);
        return -1;
    }

    // 5. listen
    ret = listen(sock, 4095);  // accept等待队列长度
    if (-1 == ret) {
        RTC_LOG(LS_WARNING) << "listen error, errno: " << errno << ", error: " << strerror(errno);
        close(sock);
        return -1;
    }

    return sock;
}

int generic_accept(int sock, struct sockaddr* sa, socklen_t* len) {
    int fd = -1;
    while (1) {
        fd = accept(sock, sa, len);
        if (-1 == fd) {
            if (EINTR == errno) {
                continue;
            } else {
                RTC_LOG(LS_WARNING) << "tcp accept error: " << strerror(errno) << ", errno: " << errno;
                return -1;
            }
        }

        break;
    }

    return fd;
}

int tcp_accept(int sock, char* host, int* port) {
    struct sockaddr_in sa;
    socklen_t salen = sizeof(sa);
    int fd = generic_accept(sock, (struct sockaddr*)&sa, &salen);
    if (-1 == fd) {
        return -1;
    }
    if (host) {
        strcpy(host, inet_ntoa(sa.sin_addr));
    }
    if (port) {
        *port = ntohs(sa.sin_port);
    }
    return fd;
}

}  // namespace xrtc