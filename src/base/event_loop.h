
#ifndef __BASE_EVENT_LOOP_H_
#define __BASE_EVENT_LOOP_H_

struct ev_loop;

namespace xrtc
{
    class EventLoop;
    class IOWatcher;
    class TimeWatcher;

    typedef void (*io_cb_t)(EventLoop *el, IOWatcher *w, int fd, int events, void *data);
    typedef void (*time_cb_t)(EventLoop *el, TimeWatcher *w, void *data);
    class EventLoop
    {
    private:
        void *_owner;
        struct ev_loop *_loop;

    public:
        enum
        {
            READ = 0x1,
            WRITE = 0x2
        };

        EventLoop(void *owner);
        ~EventLoop();
        void start();
        void stop();

        IOWatcher *create_io_event(io_cb_t cb, void *data);
        void start_io_event(IOWatcher *w, int fd, int mask);
        void stop_io_event(IOWatcher *w, int fd, int mask);
        void delete_io_event(IOWatcher *w);

        TimeWatcher *create_timer(time_cb_t cb, void *data, bool need_repeat);
        void start_timer(TimeWatcher *w, unsigned int usec);
        void stop_timer(TimeWatcher *w);
        void delete_timer(TimeWatcher *w);
    };

}

#endif //__BASE_EVENT_LOOP_H_
