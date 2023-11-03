#include <sstream>
#include "pc/session_description.h"
namespace xrtc {

SessionDescription::SessionDescription(SdpType type) : _sdk_type(type) {}

SessionDescription::~SessionDescription() {}

std::string SessionDescription::to_string() {
    std::stringstream ss;
    ss << "v=0\r\n";

    return ss.str();
}

}  // namespace xrtc