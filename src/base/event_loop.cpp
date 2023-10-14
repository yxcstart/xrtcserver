#include <ev.h>
#include "base/event_loop.h"

#define TRANS_TO_EV_MASK(mask) \
    (((mask) & EventLoop::READ ? EV_READ : 0) | ((mask) & EventLoop::WRITE ? EV_WRITE : 0))

#define TRANS_FROM_EV_MASK(mask) \
    (((mask) & EV_READ ? EventLoop::READ : 0) | ((mask) & EV_WRITE ? EventLoop::WRITE : 0))

namespace xrtc
{
    EventLoop::EventLoop(void *owner)
        : _owner(owner), _loop(ev_loop_new(EVFLAG_AUTO)) {}

    EventLoop::~EventLoop() {}

    void EventLoop::start()
    {
        ev_run(_loop);
    }

    void EventLoop::stop()
    {
        ev_break(_loop, EVBREAK_ALL);
    }

    class IOWatcher
    {
    public:
        EventLoop *el;
        ev_io io;
        io_cb_t cb;
        void *data;

    public:
        IOWatcher(EventLoop *el, io_cb_t cb, void *data)
            : el(el), cb(cb), data(data)
        {
            io.data = this;
        }
        // ~IOWatcher();
    };

    static void generic_io_cb(struct ev_loop * /*loop*/, struct ev_io *io, int events)
    {
        IOWatcher *watcher = (IOWatcher *)(io->data);
        watcher->cb(watcher->el, watcher, io->fd, TRANS_FROM_EV_MASK(events), watcher->data);
    }

    IOWatcher *EventLoop::create_io_event(io_cb_t cb, void *data)
    {
        IOWatcher *w = new IOWatcher(this, cb, data);
        ev_init(&(w->io), generic_io_cb);
        return w;
    }

    void EventLoop::start_io_event(IOWatcher *w, int fd, int mask)
    {
        struct ev_io *io = &(w->io);
        if (ev_is_active(io))
        {
            int active_events = TRANS_FROM_EV_MASK(io->events);
            int events = active_events | mask;
            if (events == active_events)
            {
                return;
            }

            events = TRANS_TO_EV_MASK(events);
            ev_io_stop(_loop, io);
            ev_io_set(io, fd, events);
            ev_io_start(_loop, io);
        }
        else
        {
            int events =TRANS_TO_EV_MASK(mask);
            ev_io_set(io,fd,events);
            ev_io_start(_loop,io);
        }
    }

    void EventLoop::stop_io_event(IOWatcher *w, int fd, int mask)
    {
        struct ev_io *io = &(w->io);
        int active_events = TRANS_FROM_EV_MASK(io->events);
        int events = active_events | ~mask;
        if (events == active_events)
        {
            return;
        }

        events = TRANS_TO_EV_MASK(events);
        ev_io_stop(_loop, io);
        if (events != EV_NONE)
        {
            ev_io_set(io, fd, events);
        }
    }

    void EventLoop::delete_io_event(IOWatcher *w)
    {
        struct ev_io *io = &(w->io);
        ev_io_stop(_loop, io);
        delete w;
    }

    class TimeWatcher
    {
    public:
        EventLoop *el;
        struct ev_timer timer;
        time_cb_t cb;
        void *data;
        bool need_repeat;

    public:
        TimeWatcher(EventLoop *el, time_cb_t cb, void *data, bool need_repeat) : el(el), cb(cb), data(data), need_repeat(need_repeat)
        {
            timer.data = this;
        }
    };

    static void generic_time_cb(struct ev_loop * /*loop*/, struct ev_timer *timer, int /*events*/)
    {
        TimeWatcher *watcher = (TimeWatcher *)(timer->data);
        watcher->cb(watcher->el, watcher, watcher->data);
    }

    TimeWatcher *EventLoop::create_timer(time_cb_t cb, void *data, bool need_repeat)
    {
        TimeWatcher *watcher = new TimeWatcher(this, cb, data, need_repeat);
        ev_init(&(watcher->timer), generic_time_cb);
        return watcher;
    }

    void EventLoop::start_timer(TimeWatcher *w, unsigned int usec)
    {
        struct ev_timer *timer = &(w->timer);
        float sec = float(usec) / 1000000;

        if (!w->need_repeat)
        {
            ev_timer_set(timer, sec, 0);
            ev_timer_start(_loop, timer);
        }
        else
        {
            timer->repeat = sec;
            ev_timer_again(_loop, timer);
        }
    }

    void EventLoop::stop_timer(TimeWatcher *w)
    {
        struct ev_timer *timer = &(w->timer);
        ev_timer_stop(_loop, timer);
    }

    void EventLoop::delete_timer(TimeWatcher *w)
    {
        stop_timer(w);
        delete w;
    }
} // namespace xrtc