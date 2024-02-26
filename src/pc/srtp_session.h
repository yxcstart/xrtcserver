#ifndef __SRTP_SESSION_H_
#define __SRTP_SESSION_H_

#include <string>
#include <vector>

namespace xrtc {

class SrtpSession {
public:
    SrtpSession();
    ~SrtpSession();

    bool set_send(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids);
    bool set_recv(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids);
    bool update_send(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids);
    bool update_recv(int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids);

private:
    bool _set_key(int type, int cs, const uint8_t* key, size_t key_len, const std::vector<int>& extension_ids);
};
}  // namespace xrtc

#endif  //__SRTP_SESSION_H_
