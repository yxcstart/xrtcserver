#include "ice/stun.h"
#include <rtc_base/byte_order.h>
#include <rtc_base/crc32.h>
#include <rtc_base/message_digest.h>

namespace xrtc {

const char EMPTY_TRANSACTION_ID[] = "000000000000";
const size_t STUN_FINGERPRINT_XOR_VALUE = 0x5354554e;

StunMessage::StunMessage() : _type(0), _length(0), _transaction_id(EMPTY_TRANSACTION_ID) {}
StunMessage::~StunMessage() = default;

bool StunMessage::validate_fingerprint(const char* data, size_t len) {
    // 检查stun消息长度
    size_t fingerprint_attr_size = k_stun_attribute_header_size + StunUInt32Attribute::SIZE;
    if (len % 4 != 0 || len < k_stun_header_size + fingerprint_attr_size) {
        return false;
    }

    // 检查magic cookie
    const char* magic_cookie = data + k_stun_transaction_id_offset - k_stun_magic_cookie_length;
    if (rtc::GetBE32(magic_cookie) != k_stun_magic_cookie) {
        return false;
    }

    // 检查attr type和length
    const char* fingerprint_attr_data = data + len - fingerprint_attr_size;
    if (rtc::GetBE16(fingerprint_attr_data) != STUN_ATTR_FINGERPRINT ||
        rtc::GetBE16(fingerprint_attr_data + sizeof(uint16_t)) != StunUInt32Attribute::SIZE) {
        return false;
    }

    // 检查fingerprint的值
    uint32_t fingerprint = rtc::GetBE32(fingerprint_attr_data + k_stun_attribute_header_size);
    return (fingerprint ^ STUN_FINGERPRINT_XOR_VALUE) == rtc::ComputeCrc32(data, len - fingerprint_attr_size);
}

StunMessage::IntegerityStatus StunMessage::validate_message_integrity(const std::string& password) {
    _password = password;
    if (get_byte_string(STUN_ATTR_MESSAGE_INTEGRITY)) {
        if (_validate_message_integrity_of_type(STUN_ATTR_MESSAGE_INTEGRITY, k_stun_message_integrity_size,
                                                _buffer.c_str(), _buffer.length(), password)) {
            _integrity = IntegerityStatus::k_integrity_ok;
        } else {
            _integrity = IntegerityStatus::k_integrity_bad;
        }

    } else {
        _integrity = IntegerityStatus::k_not_set;
    }

    return _integrity;
}

bool StunMessage::_validate_message_integrity_of_type(uint16_t mi_attr_type, size_t mi_attr_size, const char* data,
                                                      size_t size, const std::string& password) {
    if (size % 4 != 0 || size < k_stun_header_size) {
        return false;
    }

    uint16_t length = rtc::GetBE16(&data[2]);
    if (length + k_stun_header_size != size) {
        return false;
    }

    // 查找MI属性的位置
    size_t current_pos = k_stun_header_size;
    bool has_message_integrity = false;
    while (current_pos + k_stun_attribute_header_size <= size) {
        uint16_t attr_type;
        uint16_t attr_length;
        attr_type = rtc::GetBE16(&data[current_pos]);
        attr_length = rtc::GetBE16(&data[current_pos + sizeof(attr_type)]);
        if (attr_type == mi_attr_type) {
            has_message_integrity = true;
            break;
        }

        current_pos += (k_stun_attribute_header_size + attr_length);
        if (attr_length % 4 != 0) {
            current_pos += (4 - (attr_length % 4));
        }
    }

    if (!has_message_integrity) {
        return false;
    }

    size_t mi_pos = current_pos;
    std::unique_ptr<char[]> temp_data(new char[mi_pos]);
    memcpy(temp_data.get(), data, mi_pos);  // MI之前的数据
    if (size > current_pos + k_stun_attribute_header_size + mi_attr_size) {
        size_t extra_pos = mi_pos + k_stun_attribute_header_size + mi_attr_size;
        size_t extra_size = size - extra_pos;
        size_t adjust_new_len = size - extra_size - k_stun_header_size;
        rtc::SetBE16(temp_data.get() + 2, adjust_new_len);  // 重新设置stun message的长度
    }

    char hmac[k_stun_message_integrity_size];
    size_t ret = rtc::ComputeHmac(rtc::DIGEST_SHA_1, password.c_str(), password.length(), temp_data.get(), current_pos,
                                  hmac, sizeof(hmac));

    if (ret != k_stun_message_integrity_size) {
        return false;
    }

    return memcmp(data + mi_pos + k_stun_attribute_header_size, hmac, mi_attr_size) == 0;
}

bool StunMessage::read(rtc::ByteBufferReader* buf) {
    if (!buf) {
        return false;
    }

    _buffer.assign(buf->Data(), buf->Length());

    if (!buf->ReadUInt16(&_type)) {
        return false;
    }

    // rtp/rtcp 10(2)
    if (_type & 0x0800) {
        return false;
    }

    // stun message 的总长度（不包含头部）
    if (!buf->ReadUInt16(&_length)) {
        return false;
    }

    std::string magic_cookie;
    // 读4字节
    if (!buf->ReadString(&magic_cookie, k_stun_magic_cookie_length)) {
        return false;
    }

    std::string transaction_id;
    // 读12字节
    if (!buf->ReadString(&transaction_id, k_stun_transaction_id_length)) {
        return false;
    }

    uint32_t magic_cookie_int;
    memcpy(&magic_cookie_int, magic_cookie.data(), sizeof(magic_cookie_int));
    // RFC3489老规范没有magic_cookie，而是和transaction_id一起共16字节
    if (rtc::NetworkToHost32(magic_cookie_int) != k_stun_magic_cookie) {
        transaction_id.insert(0, magic_cookie);
    }

    _transaction_id = transaction_id;

    if (buf->Length() != _length) {
        return false;
    }

    _attrs.resize(0);
    while (buf->Length() > 0) {
        uint16_t attr_type;
        uint16_t attr_length;
        if (!buf->ReadUInt16(&attr_type)) {
            return false;
        }
        if (!buf->ReadUInt16(&attr_length)) {
            return false;
        }

        std::unique_ptr<StunAttribute> attr(_create_attribute(attr_type, attr_length));
        if (!attr) {
            if (attr_length % 4 != 0) {
                attr_length += (4 - (attr_length % 4));
            }

            if (!buf->Consume(attr_length)) {
                return false;
            }
        } else {
            if (!attr->read(buf)) {
                return false;
            }
            _attrs.push_back(std::move(attr));
        }
    }

    return true;
}

StunAttribute* StunMessage::_create_attribute(uint16_t type, uint16_t length) {
    StunAttributeValueType value_type = get_attribute_value_type(type);
    if (STUN_VALUE_UNKNOWN != value_type) {
        return StunAttribute::create(value_type, type, length, this);
    }
    return nullptr;
}

StunAttributeValueType StunMessage::get_attribute_value_type(int type) {
    switch (type) {
        case STUN_ATTR_USERNAME:
            return STUN_VALUE_BYTE_STRING;
        case STUN_ATTR_MESSAGE_INTEGRITY:
            return STUN_VALUE_BYTE_STRING;
        default:
            return STUN_VALUE_UNKNOWN;
    }
}

const StunByteStringAttribute* StunMessage::get_byte_string(uint16_t type) {
    return static_cast<const StunByteStringAttribute*>(_get_attribute(type));
}

const StunAttribute* StunMessage::_get_attribute(uint16_t type) {
    for (const auto& attr : _attrs) {
        if (attr->type() == type) {
            return attr.get();
        }
    }
    return nullptr;
}

StunAttribute::StunAttribute(uint16_t type, uint16_t length) : _type(type), _length(length) {}

StunAttribute::~StunAttribute() = default;

StunAttribute* StunAttribute::create(StunAttributeValueType value_type, uint16_t type, uint16_t length, void* owner) {
    switch (value_type) {
        case STUN_VALUE_BYTE_STRING:
            return new StunByteStringAttribute(type, length);
        default:
            return nullptr;
    }
}

void StunAttribute::consume_padding(rtc::ByteBufferReader* buf) {
    int remain = length() % 4;
    if (remain > 0) {
        buf->Consume(4 - remain);
    }
}

StunByteStringAttribute::StunByteStringAttribute(uint16_t type, uint16_t length) : StunAttribute(type, length) {}

StunByteStringAttribute::~StunByteStringAttribute() {
    if (_bytes) {
        delete[] _bytes;
        _bytes = nullptr;
    }
}

bool StunByteStringAttribute::read(rtc::ByteBufferReader* buf) {
    _bytes = new char[length()];
    if (!buf->ReadBytes(_bytes, length())) {
        return false;
    }

    consume_padding(buf);

    return true;
}

}  // namespace xrtc