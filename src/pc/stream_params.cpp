#include "pc/stream_params.h"

namespace xrtc {

SsrcGroup::SsrcGroup(const std::string& semantics, const std::vector<uint32_t>& ssrcs)
    : semantics(semantics), ssrcs(ssrcs) {}

}  // namespace xrtc