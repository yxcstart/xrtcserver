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

enum StunMessageType {
    STUN_BINDING_REQUEST = 0x0001,
};

// https://datatracker.ietf.org/doc/html/rfc5389
enum StunAttributeType {
    STUN_ATTR_USERNAME = 0x0006,
    STUN_ATTR_MESSAGE_INTEGRITY = 0x0008,
    STUN_ATTR_FINGERPRINT = 0x8028,
};

enum StunAttributeValueType {
    STUN_VALUE_UNKNOWN = 0,
    STUN_VALUE_UINT32,
    STUN_VALUE_BYTE_STRING,
};

class StunAttribute;
class StunByteStringAttribute;

class StunMessage {
public:
    StunMessage();
    ~StunMessage();

    int type() { return _type; }
    size_t length() { return _length; }

    static bool validate_fingerprint(const char* data, size_t len);
    StunAttributeValueType get_attribute_value_type(int type);
    bool read(rtc::ByteBufferReader* buf);

    const StunByteStringAttribute* get_byte_string(uint16_t type);

private:
    StunAttribute* _create_attribute(uint16_t type, uint16_t length);
    const StunAttribute* _get_attribute(uint16_t type);

private:
    uint16_t _type;
    uint16_t _length;
    std::string _transaction_id;
    std::vector<std::unique_ptr<StunAttribute>> _attrs;
};

class StunAttribute {
public:
    virtual ~StunAttribute();  // 父类析构，析构具体子类

    int type() { return _type; }
    size_t length() { return _length; }

    static StunAttribute* create(StunAttributeValueType value_type, uint16_t type, uint16_t length, void* owner);
    virtual bool read(rtc::ByteBufferReader* buf) = 0;

protected:
    StunAttribute(uint16_t type, uint16_t length);
    void consume_padding(rtc::ByteBufferReader* buf);

private:
    uint16_t _type;
    uint16_t _length;
};

class StunUInt32Attribute : public StunAttribute {
public:
    static const size_t SIZE = 4;
};

class StunByteStringAttribute : public StunAttribute {
public:
    StunByteStringAttribute(uint16_t type, uint16_t length);
    ~StunByteStringAttribute() override;

    bool read(rtc::ByteBufferReader* buf) override;

private:
    char* _bytes = nullptr;
};

}  // namespace xrtc

#endif  //__ICE_STUN_H_