#ifndef __SESSION_DESCRIPTION_H_
#define __SESSION_DESCRIPTION_H_

#include <memory>
#include <string>
#include <vector>

namespace xrtc {

enum class SdpType {
    k_offer = 0,
    k_answer = 1,
};

enum class MediaType {
    MEDIA_TYPE_AUDIO,
    MEDIA_TYPE_VIDEO,
};

class MediaContentDescription {
public:
    ~MediaContentDescription() {}
    virtual MediaType type() = 0;
    virtual std::string mid() = 0;
};

class AudioContentDescription : public MediaContentDescription {
public:
    MediaType type() override { return MediaType::MEDIA_TYPE_AUDIO; }
    std::string mid() override { return "audio"; }
};

class VideoContentDescription : public MediaContentDescription {
public:
    MediaType type() override { return MediaType::MEDIA_TYPE_VIDEO; }
    std::string mid() override { return "video"; }
};

class SessionDescription {
public:
    SessionDescription(SdpType type);
    ~SessionDescription();

    void add_content(std::shared_ptr<MediaContentDescription> content);
    std::string to_string();

private:
    SdpType _sdk_type;
    std::vector<std::shared_ptr<MediaContentDescription>> _contents;
};

}  // namespace xrtc
#endif  //__SESSION_DESCRIPTION_H_