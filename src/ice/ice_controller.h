
#ifndef __ICE_CONTROLLER_H__
#define __ICE_CONTROLLER_H__

#include "ice/ice_connection.h"

namespace xrtc {

class IceTransportChannel;

class IceController {
public:
    IceController(IceTransportChannel* ice_channel) : _ice_channel(ice_channel) {}
    ~IceController() = default;

    void add_connection(IceConnection* conn);
    bool has_pingable_connection();

private:
    bool _is_pingable(IceConnection* conn);
    bool _weak() { return _selected_connection == nullptr || _selected_connection->weak(); }

private:
    IceTransportChannel* _ice_channel;
    IceConnection* _selected_connection = nullptr;
    std::vector<IceConnection*> _connections;
};

}  // namespace xrtc

#endif  //__ICE_CONTROLLER_H__