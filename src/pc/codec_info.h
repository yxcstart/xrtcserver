#ifndef __CODEC_INFO_H_
#define __CODEC_INFO_H_

#include <string>

namespace xrtc {

class CodecInfo {
public:
    int id;
    std::string name;
    int clockrate;
};

class AudioCodecInfo : public CodecInfo {
public:
    int channels;
};

class VideoCodecInfo : public CodecInfo {};

}  // namespace xrtc

#endif  //__CODEC_INFO_H_