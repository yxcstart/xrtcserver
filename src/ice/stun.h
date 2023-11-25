#ifndef __ICE_STUN_H_
#define __ICE_STUN_H_

#include <rtc_base/byte_buffer.h>
#include <memory>
#include <string>
#include <vector>
namespace xrtc {

const size_t k_stun_header_size = 20;
const size_t k_stun_attribute_header_size = 4;
const size_t k_stun_transaction_id_offset = 8;
const size_t k_stun_transaction_id_length = 12;
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
    bool read(rtc::ByteBufferReader* buf);

private:
    std::unique_ptr<StunAttribute> _create_attribute(uint16_t type, uint16_t length);

private:
    uint16_t _type;
    uint16_t _length;
    std::string _transaction_id;
    std::vector<std::unique_ptr<StunAttribute>> _attrs;
};

class StunAttribute {
public:
    StunAttribute(uint16_t type, uint16_t length);
    virtual ~StunAttribute();  // 父类析构，析构具体子类

    virtual bool read(rtc::ByteBufferReader* buf) = 0;

private:
    uint16_t _type;
    uint16_t _length;
};

class StunUInt32Attribute : public StunAttribute {
public:
    static const size_t SIZE = 4;
};

}  // namespace xrtc

#endif  //__ICE_STUN_H_