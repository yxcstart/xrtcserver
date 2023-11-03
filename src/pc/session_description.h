#ifndef __SESSION_DESCRIPTION_H_
#define __SESSION_DESCRIPTION_H_

#include <string>

namespace xrtc {

enum class SdpType {
    k_offer = 0,
    k_answer = -1,
};

class SessionDescription {
public:
    SessionDescription(SdpType type);
    ~SessionDescription();

    std::string to_string();

private:
    SdpType _sdk_type;
};

}  // namespace xrtc
#endif  //__SESSION_DESCRIPTION_H_