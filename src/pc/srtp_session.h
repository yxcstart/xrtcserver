#ifndef __SRTP_SESSION_H_
#define __SRTP_SESSION_H_

#include <srtp2/srtp.h>
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
    static bool _increment_libsrtp_usage_count_and_maybe_init();
    static void _event_handle_thunk(srtp_event_data_t* ev);
    void _handle_event(srtp_event_data_t* ev);

private:
    srtp_ctx_t_* _session = nullptr;
    bool _inited = false;
};
}  // namespace xrtc

#endif  //__SRTP_SESSION_H_
