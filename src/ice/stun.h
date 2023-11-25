#ifndef __ICE_STUN_H_
#define __ICE_STUN_H_

#include <memory>
#include <string>
#include <vector>
namespace xrtc {

const size_t k_stun_header_size = 20;
const size_t k_stun_attribute_header_size = 4;
const size_t k_stun_transaction_id_offset = 8;
const uint32_t k_stun_magic_cookie = 0x2112A442;
const size_t k_stun_magic_cookie_length = sizeof(k_stun_magic_cookie);

enum StunAttributeValue {
    STUN_ATTR_FINGERPRINT = 0x8028,
};

class StunAttribute;

class StunMessage {
public:
    StunMessage();
    ~StunMessage();

    static bool validate_fingerprint(const char* data, size_t len);

private:
    uint16_t _type;
    uint16_t _length;
    std::string _transaction_id;
    std::vector<std::unique_ptr<StunAttribute>> _attrs;
};

class StunAttribute {};

class StunUInt32Attribute : public StunAttribute {
public:
    static const size_t SIZE = 4;
};

}  // namespace xrtc

#endif  //__ICE_STUN_H_