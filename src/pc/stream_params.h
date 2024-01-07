

#ifndef __PC_STREAM_PARAMS_H_
#define __PC_STREAM_PARAMS_H_

#include <string>
#include <vector>

namespace xrtc {

struct SsrcGroup {
    SsrcGroup(const std::string& semantics, const std::vector<uint32_t>& ssrcs);

    std::string semantics;
    std::vector<uint32_t> ssrcs;
};

}  // namespace xrtc

#endif  //__PC_STREAM_PARAMS_H_