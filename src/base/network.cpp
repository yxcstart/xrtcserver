#include "base/network.h"
#include <ifaddrs.h>
#include <rtc_base/logging.h>

namespace xrtc {

NetworkManager::NetworkManager() = default;

NetworkManager::~NetworkManager() {
    for (auto network : _network_list) {
        delete network;
    }
    _network_list.clear();
}

int NetworkManager::create_networks() {
    struct ifaddrs* interface;
    int ret = getifaddrs(&interface);
    if (ret != 0) {
        RTC_LOG(LS_WARNING) << "getifaddrs error: " << strerror(errno) << ", errno: " << errno;
        return -1;
    }
    for (auto cur = interface; cur != nullptr; cur = cur->ifa_next) {
        if (cur->ifa_addr->sa_family != AF_INET) {
            continue;
        }
        struct sockaddr_in* addr = (struct sockaddr_in*)(cur->ifa_addr);
        rtc::IPAddress ip_address(addr->sin_addr);
        // if (rtc::IPIsPrivateNetwork(ip_address) || rtc::IPIsLoopback(ip_address)) {
        //     continue;
        // }
        if (rtc::IPIsLoopback(ip_address)) {
            continue;
        }
        Network* network = new Network(cur->ifa_name, ip_address);
        RTC_LOG(LS_INFO) << "gathered network interface: " << network->to_string();
        _network_list.push_back(network);
    }

    freeifaddrs(interface);

    return 0;
}

}  // namespace xrtc