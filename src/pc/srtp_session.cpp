#include <rtc_base/logging.h>

#include "pc/srtp_session.h"

namespace xrtc {

SrtpSession::SrtpSession() {}

SrtpSession::~SrtpSession() {}

bool SrtpSession::set_send(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids) {}
bool SrtpSession::set_recv(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids) {}
bool SrtpSession::update_send(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids) {}
bool SrtpSession::update_recv(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids) {}

bool SrtpSession::_set_key(int type, int cs, const uint8_t* key, size_t key_len,
                           const std::vector<int>& extension_ids) {}

}  // namespace xrtc