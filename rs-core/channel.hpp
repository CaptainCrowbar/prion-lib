#pragma once

#include "rs-core/common.hpp"
#include "rs-core/optional.hpp"
#include "rs-core/time.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <exception>
#include <functional>
#include <future>
#include <map>
#include <mutex>
#include <ostream>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>

namespace RS {

    // Forward declarations

    class BufferChannel;
    class Channel;
    class Dispatch;
    class EventChannel;
    class FalseChannel;
    template <typename T> class GeneratorChannel;
    template <typename T> class MessageChannel;
    class Polled;
    template <typename T> class QueueChannel;
    class StreamChannel;
    class ThrottleChannel;
    class TimerChannel;
    class TrueChannel;
    template <typename T> class ValueChannel;

    // Channel base classes

    class Channel {
    public:
        using time_unit = std::chrono::microseconds;
        enum class state { closed = -1, waiting, ready };
        virtual ~Channel() noexcept { drop(); }
        Channel(const Channel&) = delete;
        Channel(Channel&& c) noexcept { owner = std::exchange(c.owner, nullptr); }
        Channel& operator=(const Channel&) = delete;
        Channel& operator=(Channel&& c) noexcept;
        virtual void close() noexcept = 0;
        virtual bool is_closed() const noexcept = 0;
        virtual bool is_async() const noexcept { return true; }
        virtual bool is_shared() const noexcept { return false; }
        virtual state poll() { return do_wait_for({}); }
        virtual state wait();
        template <typename R, typename P> state wait_for(std::chrono::duration<R, P> t);
        template <typename C, typename D> state wait_until(std::chrono::time_point<C, D> t);
    protected:
        virtual state do_wait_for(time_unit t) = 0;
    private:
        friend class Dispatch;
        friend class EventChannel;
        template <typename T> friend class MessageChannel;
        friend class StreamChannel;
        Dispatch* owner = nullptr;
        Channel() = default;
        void drop() noexcept; // Defined after Dispatch
    };

        inline Channel& Channel::operator=(Channel&& c) noexcept {
            if (&c != this) {
                drop();
                owner = std::exchange(c.owner, nullptr);
            }
            return *this;
        }

        inline Channel::state Channel::wait() {
            using namespace std::chrono;
            static const auto t = duration_cast<time_unit>(1min);
            auto cs = state::waiting;
            while (cs == state::waiting)
                cs = do_wait_for(t);
            return cs;
        }

        template <typename R, typename P>
        Channel::state Channel::wait_for(std::chrono::duration<R, P> t) {
            using namespace std::chrono;
            auto u = duration_cast<time_unit>(t);
            return do_wait_for(u);
        }

        template <typename C, typename D>
        Channel::state Channel::wait_until(std::chrono::time_point<C, D> t) {
            return do_wait_for(t - C::now());
        }

        inline std::ostream& operator<<(std::ostream& out, Channel::state cs) {
            switch (cs) {
                case Channel::state::ready:    out << "ready"; break;
                case Channel::state::waiting:  out << "waiting"; break;
                case Channel::state::closed:   out << "closed"; break;
                default:                       out << int(cs); break;
            }
            return out;
        }

        inline bool operator!(Channel::state cs) noexcept {
            return cs == Channel::state::waiting;
        }

    class EventChannel:
    public Channel {
    public:
        using callback = std::function<void()>;
    protected:
        EventChannel() = default;
    };

    template <typename T>
    class MessageChannel:
    public Channel {
    public:
        using callback = std::function<void(const T&)>;
        using value_type = T;
        virtual bool read(T& t) = 0;
        Optional<T> read_opt();
    protected:
        MessageChannel() = default;
    };

        template <typename T>
        Optional<T> MessageChannel<T>::read_opt() {
            T t;
            if (read(t))
                return std::move(t);
            else
                return {};
        }

    class StreamChannel:
    public Channel {
    public:
        using callback = std::function<void(std::string&)>;
        static constexpr size_t default_buffer = 16384;
        virtual size_t read(void* dst, size_t maxlen) = 0;
        size_t buffer() const noexcept { return bytes; }
        void set_buffer(size_t n) noexcept { bytes = n; }
        std::string read_all();
        std::string read_str();
        size_t read_to(std::string& dst);
    protected:
        StreamChannel() = default;
    private:
        size_t bytes = default_buffer;
    };

        inline std::string StreamChannel::read_all() {
            using namespace std::chrono;
            std::string s;
            auto cs = state::waiting;
            while (cs != state::closed) {
                cs = wait_for(seconds(1));
                if (cs == state::ready)
                    read_to(s);
            }
            return s;
        }

        inline std::string StreamChannel::read_str() {
            std::string s(bytes, '\0');
            size_t n = read(&s[0], bytes);
            s.resize(n);
            return s;
        }

        inline size_t StreamChannel::read_to(std::string& dst) {
            size_t pos = dst.size(), n = 0;
            auto guard = scope_exit([&] { dst.resize(pos + n); });
            dst.resize(pos + bytes);
            n = read(&dst[0] + pos, bytes);
            return n;
        }

    // Concrete channel classes

    class TrueChannel:
    public EventChannel {
    public:
        RS_NO_COPY_MOVE(TrueChannel);
        TrueChannel() = default;
        virtual void close() noexcept { open = false; }
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool is_shared() const noexcept { return true; }
    protected:
        virtual state do_wait_for(time_unit /*t*/) { return open ? state::ready : state::closed; }
    private:
        std::atomic<bool> open{true};
    };

    class FalseChannel:
    public EventChannel {
    public:
        RS_NO_COPY_MOVE(FalseChannel);
        FalseChannel() = default;
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool is_shared() const noexcept { return true; }
    protected:
        virtual state do_wait_for(time_unit t);
    private:
        mutable std::mutex mutex;
        std::condition_variable cv;
        bool open = true;
    };

        inline void FalseChannel::close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
            cv.notify_all();
        }

        inline Channel::state FalseChannel::do_wait_for(time_unit t) {
            auto lock = make_lock(mutex);
            if (! open)
                return state::closed;
            else if (t <= time_unit())
                return state::waiting;
            cv.wait_for(lock, t, [&] { return ! open; });
            return open ? state::waiting : state::closed;
        }

    class TimerChannel:
    public EventChannel {
    public:
        RS_NO_COPY_MOVE(TimerChannel);
        template <typename R, typename P> explicit TimerChannel(std::chrono::duration<R, P> t) noexcept;
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool is_shared() const noexcept { return true; }
        void flush() noexcept;
        time_unit interval() const noexcept { return delta; }
        auto next() const noexcept { return next_tick; }
    protected:
        virtual state do_wait_for(time_unit t);
    private:
        using clock_type = ReliableClock;
        using time_point = clock_type::time_point;
        mutable std::mutex mutex;
        std::condition_variable cv;
        time_point next_tick;
        time_unit delta;
        bool open = true;
    };

        template <typename R, typename P>
        TimerChannel::TimerChannel(std::chrono::duration<R, P> t) noexcept {
            using namespace std::chrono;
            delta = std::max(duration_cast<time_unit>(t), time_unit());
            next_tick = clock_type::now() + delta;
        }

        inline void TimerChannel::close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
            cv.notify_all();
        }

        inline void TimerChannel::flush() noexcept {
            auto lock = make_lock(mutex);
            if (! open)
                return;
            auto now = clock_type::now();
            if (now < next_tick)
                return;
            auto skip = (now - next_tick).count() / delta.count();
            next_tick += delta * (skip + 1);
        }

        inline Channel::state TimerChannel::do_wait_for(time_unit t) {
            using namespace std::chrono;
            auto lock = make_lock(mutex);
            if (! open)
                return state::closed;
            auto now = clock_type::now();
            if (next_tick <= now) {
                next_tick += delta;
                return state::ready;
            }
            if (t <= time_unit())
                return state::waiting;
            auto remaining = duration_cast<time_unit>(next_tick - now);
            if (t < remaining) {
                cv.wait_for(lock, t, [&] { return ! open; });
                return open ? state::waiting : state::closed;
            }
            cv.wait_for(lock, remaining, [&] { return ! open; });
            if (! open)
                return state::closed;
            next_tick += delta;
            return state::ready;
        }

    class ThrottleChannel:
    public EventChannel {
    public:
        RS_NO_COPY_MOVE(ThrottleChannel);
        template <typename R, typename P> explicit ThrottleChannel(std::chrono::duration<R, P> t) noexcept;
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool is_shared() const noexcept { return true; }
        time_unit interval() const noexcept { return delta; }
    protected:
        virtual state do_wait_for(time_unit t);
    private:
        using clock_type = ReliableClock;
        using time_point = clock_type::time_point;
        mutable std::mutex mutex;
        std::condition_variable cv;
        time_point next = time_point::min();
        time_unit delta;
        bool open = true;
    };

        template <typename R, typename P>
        ThrottleChannel::ThrottleChannel(std::chrono::duration<R, P> t) noexcept {
            using namespace std::chrono;
            delta = std::max(duration_cast<time_unit>(t), time_unit());
        }

        inline void ThrottleChannel::close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
            cv.notify_all();
        }

        inline Channel::state ThrottleChannel::do_wait_for(time_unit t) {
            using namespace std::chrono;
            auto lock = make_lock(mutex);
            if (! open)
                return state::closed;
            auto now = clock_type::now();
            if (next <= now) {
                next = now + delta;
                return state::ready;
            }
            if (t <= time_unit())
                return state::waiting;
            auto remaining = duration_cast<time_unit>(next - now);
            if (t < remaining) {
                cv.wait_for(lock, t, [&] { return ! open; });
                return open ? state::waiting : state::closed;
            }
            cv.wait_for(lock, remaining, [&] { return ! open; });
            if (! open)
                return state::closed;
            next = clock_type::now() + delta;
            return state::ready;
        }

    template <typename T>
    class GeneratorChannel:
    public MessageChannel<T> {
    public:
        RS_NO_COPY_MOVE(GeneratorChannel);
        using generator = std::function<T()>;
        template <typename F> explicit GeneratorChannel(F f): mutex(), gen(f) {}
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! gen; }
        virtual bool read(T& t);
    protected:
        virtual Channel::state do_wait_for(Channel::time_unit t);
    private:
        std::mutex mutex;
        generator gen;
    };

        template <typename T>
        void GeneratorChannel<T>::close() noexcept {
            auto lock = make_lock(mutex);
            gen = nullptr;
        }

        template <typename T>
        bool GeneratorChannel<T>::read(T& t) {
            auto lock = make_lock(mutex);
            if (gen)
                t = gen();
            return bool(gen);
        }

        template <typename T>
        Channel::state GeneratorChannel<T>::do_wait_for(Channel::time_unit /*t*/) {
            auto lock = make_lock(mutex);
            return gen ? Channel::state::ready : Channel::state::closed;
        }

    template <typename T>
    class QueueChannel:
    public MessageChannel<T> {
    public:
        RS_NO_COPY_MOVE(QueueChannel);
        QueueChannel() = default;
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool is_shared() const noexcept { return true; }
        virtual bool read(T& t);
        void clear() noexcept;
        bool write(const T& t);
        bool write(T&& t);
    protected:
        virtual Channel::state do_wait_for(Channel::time_unit t);
    private:
        std::mutex mutex;
        std::condition_variable cv;
        bool open = true;
        std::deque<T> queue;
        Channel::state get_status() noexcept;
    };

        template <typename T>
        void QueueChannel<T>::close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
            cv.notify_all();
        }

        template <typename T>
        bool QueueChannel<T>::read(T& t) {
            auto lock = make_lock(mutex);
            if (get_status() != Channel::state::ready)
                return false;
            t = queue.front();
            queue.pop_front();
            if (! queue.empty())
                cv.notify_all();
            return true;
        }

        template <typename T>
        void QueueChannel<T>::clear() noexcept {
            auto lock = make_lock(mutex);
            queue.clear();
        }

        template <typename T>
        bool QueueChannel<T>::write(const T& t) {
            auto lock = make_lock(mutex);
            if (! open)
                return false;
            queue.push_back(t);
            cv.notify_all();
            return true;
        }

        template <typename T>
        bool QueueChannel<T>::write(T&& t) {
            auto lock = make_lock(mutex);
            if (! open)
                return false;
            queue.push_back(std::move(t));
            cv.notify_all();
            return true;
        }

        template <typename T>
        Channel::state QueueChannel<T>::do_wait_for(Channel::time_unit t) {
            auto lock = make_lock(mutex);
            if (get_status() == Channel::state::waiting && t > Channel::time_unit())
                cv.wait_for(lock, t, [&] { return get_status() != Channel::state::waiting; });
            return get_status();
        }

        template <typename T>
        Channel::state QueueChannel<T>::get_status() noexcept {
            if (! open)
                return Channel::state::closed;
            else if (queue.empty())
                return Channel::state::waiting;
            else
                return Channel::state::ready;
        }

    template <typename T>
    class ValueChannel:
    public MessageChannel<T> {
    public:
        RS_NO_COPY_MOVE(ValueChannel);
        ValueChannel() = default;
        explicit ValueChannel(const T& t): value(t) {}
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return st == Channel::state::closed; }
        virtual bool is_shared() const noexcept { return true; }
        virtual bool read(T& t);
        void clear() noexcept;
        bool write(const T& t);
        bool write(T&& t);
    protected:
        virtual Channel::state do_wait_for(Channel::time_unit t);
    private:
        std::mutex mutex;
        std::condition_variable cv;
        T value;
        Channel::state st = Channel::state::waiting;
    };

        template <typename T>
        void ValueChannel<T>::close() noexcept {
            auto lock = make_lock(mutex);
            st = Channel::state::closed;
            cv.notify_all();
        }

        template <typename T>
        bool ValueChannel<T>::read(T& t) {
            auto lock = make_lock(mutex);
            if (st != Channel::state::ready)
                return false;
            t = value;
            st = Channel::state::waiting;
            return true;
        }

        template <typename T>
        bool ValueChannel<T>::write(const T& t) {
            auto lock = make_lock(mutex);
            if (st == Channel::state::closed)
                return false;
            if (t == value)
                return true;
            value = t;
            st = Channel::state::ready;
            cv.notify_all();
            return true;
        }

        template <typename T>
        bool ValueChannel<T>::write(T&& t) {
            auto lock = make_lock(mutex);
            if (st == Channel::state::closed)
                return false;
            if (t == value)
                return true;
            value = std::move(t);
            st = Channel::state::ready;
            cv.notify_all();
            return true;
        }

        template <typename T>
        Channel::state ValueChannel<T>::do_wait_for(Channel::time_unit t) {
            auto lock = make_lock(mutex);
            if (st == Channel::state::waiting && t > Channel::time_unit())
                cv.wait_for(lock, t, [&] { return st != Channel::state::waiting; });
            return st;
        }

    class BufferChannel:
    public StreamChannel {
    public:
        RS_NO_COPY_MOVE(BufferChannel);
        BufferChannel() = default;
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! open; }
        virtual size_t read(void* dst, size_t maxlen);
        void clear() noexcept;
        bool write(string_view src) { return write(src.data(), src.size()); }
        bool write(const void* src, size_t len);
    protected:
        virtual state do_wait_for(time_unit t);
    private:
        std::mutex mutex;
        std::condition_variable cv;
        std::string buf;
        size_t ofs = 0;
        bool open = true;
    };

        inline void BufferChannel::close() noexcept {
            auto lock = make_lock(mutex);
            buf.clear();
            ofs = 0;
            open = false;
            cv.notify_all();
        }

        inline size_t BufferChannel::read(void* dst, size_t maxlen) {
            if (! dst || ! maxlen)
                return 0;
            auto lock = make_lock(mutex);
            if (! open)
                return 0;
            size_t n = std::min(buf.size() - ofs, maxlen);
            memcpy(dst, buf.data() + ofs, n);
            ofs += n;
            if (ofs == buf.size()) {
                buf.clear();
                ofs = 0;
            } else if (2 * ofs >= buf.size()) {
                buf.erase(0, ofs);
                ofs = 0;
            }
            return n;
        }

        inline bool BufferChannel::write(const void* src, size_t len) {
            auto lock = make_lock(mutex);
            if (! open)
                return false;
            buf.append(static_cast<const char*>(src), len);
            if (ofs < buf.size())
                cv.notify_all();
            return true;
        }

        inline void BufferChannel::clear() noexcept {
            auto lock = make_lock(mutex);
            buf.clear();
            ofs = 0;
        }

        inline Channel::state BufferChannel::do_wait_for(time_unit t) {
            auto lock = make_lock(mutex);
            if (open && ofs == buf.size() && t > time_unit())
                cv.wait_for(lock, t, [&] { return ! open || ofs < buf.size(); });
            if (! open)
                return state::closed;
            else if (ofs == buf.size())
                return state::waiting;
            else
                return state::ready;
        }

    // Polling mixin class

    class Polled {
    public:
        static constexpr Channel::time_unit default_interval = std::chrono::milliseconds(10);
        virtual ~Polled() = default;
        virtual Channel::state poll() = 0;
        Channel::time_unit interval() const noexcept { return delta; }
        template <typename R, typename P> void set_interval(std::chrono::duration<R, P> t) noexcept;
    protected:
        Polled() = default;
        Channel::state polled_wait(Channel::time_unit t);
    private:
        Channel::time_unit delta = default_interval;
    };

        template <typename R, typename P>
        void Polled::set_interval(std::chrono::duration<R, P> t) noexcept {
            using namespace std::chrono;
            delta = std::max(duration_cast<Channel::time_unit>(t), Channel::time_unit());
        }

        inline Channel::state Polled::polled_wait(Channel::time_unit t) {
            using namespace std::chrono;
            if (t <= Channel::time_unit())
                return poll();
            auto deadline = ReliableClock::now() + t;
            for (;;) {
                Channel::state s = poll();
                if (s != Channel::state::waiting)
                    return s;
                auto now = ReliableClock::now();
                if (now >= deadline)
                    return Channel::state::waiting;
                auto remaining = std::min(duration_cast<Channel::time_unit>(deadline - now), delta);
                std::this_thread::sleep_for(remaining);
            }
        }

    // Dispatch control class

    class Dispatch {
    public:
        RS_NO_COPY_MOVE(Dispatch);
        enum class mode { sync = 1, async };
        enum class reason { empty = 1, closed, error };
        struct result_type {
            Channel* channel = nullptr;
            std::exception_ptr error;
            void rethrow() const { if (error) std::rethrow_exception(error); }
            reason why() const noexcept { return error ? reason::error : channel ? reason::closed : reason::empty; }
        };
        Dispatch() = default;
        ~Dispatch() noexcept { stop(); }
        template <typename F> void add(EventChannel& chan, mode m, F func);
        template <typename T, typename F> void add(MessageChannel<T>& chan, mode m, F func);
        template <typename F> void add(StreamChannel& chan, mode m, F func);
        void drop(Channel& chan) noexcept;
        bool empty() const noexcept { return tasks.empty(); }
        result_type run() noexcept;
        void stop() noexcept;
    private:
        using callback = std::function<void()>;
        struct task_info {
            mode runmode;
            callback call;
            std::future<void> thread;
            std::atomic<bool> done;
            std::exception_ptr error;
        };
        std::map<Channel*, task_info> tasks;
        void add_task(Channel& chan, mode m, callback call);
        template <typename C, typename F> static typename C::callback make_callback(C& /*chan*/, const F& func) { return func; }
    };

        inline std::ostream& operator<<(std::ostream& out, Dispatch::mode dm) {
            switch (dm) {
                case Dispatch::mode::sync:   out << "sync"; break;
                case Dispatch::mode::async:  out << "async"; break;
                default:                     out << int(dm); break;
            }
            return out;
        }

        inline std::ostream& operator<<(std::ostream& out, Dispatch::reason dr) {
            switch (dr) {
                case Dispatch::reason::empty:   out << "empty"; break;
                case Dispatch::reason::closed:  out << "closed"; break;
                case Dispatch::reason::error:   out << "error"; break;
                default:                        out << int(dr); break;
            }
            return out;
        }

        template <typename F>
        void Dispatch::add(EventChannel& chan, mode m, F func) {
            auto handler = make_callback(chan, func);
            if (handler)
                add_task(chan, m, handler);
        }

        template <typename T, typename F>
        void Dispatch::add(MessageChannel<T>& chan, mode m, F func) {
            auto handler = make_callback(chan, func);
            if (! handler)
                return;
            auto call = [&chan,handler,t=T()] () mutable {
                if (chan.read(t))
                    handler(t);
            };
            add_task(chan, m, call);
        }

        template <typename F>
        void Dispatch::add(StreamChannel& chan, mode m, F func) {
            auto handler = make_callback(chan, func);
            if (! handler)
                return;
            auto call = [&chan,handler,s=std::string()] () mutable {
                if (chan.read_to(s))
                    handler(s);
            };
            add_task(chan, m, call);
        }

        inline void Dispatch::drop(Channel& chan) noexcept {
            tasks.erase(&chan);
            chan.owner = nullptr;
        }

        inline Dispatch::result_type Dispatch::run() noexcept {
            using namespace std::chrono;
            using namespace std::literals;
            static constexpr Channel::time_unit min_interval = 1us;
            static constexpr Channel::time_unit max_interval = 1ms;
            result_type rc;
            if (tasks.empty())
                return rc;
            auto guard = scope_exit([&] { if (rc.channel) drop(*rc.channel); });
            int waits = 0;
            auto interval = min_interval;
            for (;;) {
                int calls = 0;
                for (auto& t: tasks) {
                    rc.channel = t.first;
                    if (t.second.runmode == Dispatch::mode::sync) {
                        try {
                            auto cs = t.first->poll();
                            if (cs == Channel::state::closed)
                                return rc;
                            if (cs == Channel::state::ready) {
                                t.second.call();
                                ++calls;
                            }
                        }
                        catch (...) {
                            rc.error = std::current_exception();
                            return rc;
                        }
                    } else if (t.second.done) {
                        rc.error = t.second.error;
                        return rc;
                    }
                }
                if (calls == 0) {
                    if (++waits == 1)
                        interval = min_interval;
                    else
                        interval = std::min(2 * interval, max_interval);
                    std::this_thread::sleep_for(interval);
                } else {
                    waits = 0;
                    std::this_thread::yield();
                }
            }
        }

        inline void Dispatch::stop() noexcept {
            for (auto& t: tasks)
                t.first->close();
            while (! tasks.empty())
                run();
        }

        inline void Dispatch::add_task(Channel& chan, mode m, callback call) {
            using namespace std::chrono;
            if (! chan.is_shared() && tasks.count(&chan))
                throw std::invalid_argument("Dispatch channel is not shareable");
            if (m != Dispatch::mode::sync && m != Dispatch::mode::async)
                throw std::invalid_argument("Invalid dispatch mode");
            if (m == Dispatch::mode::async && ! chan.is_async())
                throw std::invalid_argument("Invalid dispatch mode for channel");
            if (! call)
                throw std::invalid_argument("Dispatch callback is null");
            chan.owner = this;
            auto& task = tasks[&chan];
            task.runmode = m;
            task.call = call;
            if (task.runmode == Dispatch::mode::async) {
                auto payload = [&] () noexcept {
                    try {
                        auto cs = Channel::state::waiting;
                        while (cs != Channel::state::closed) {
                            cs = chan.wait_for(seconds(1));
                            if (cs == Channel::state::ready)
                                task.call();
                        }
                    }
                    catch (...) {
                        task.error = std::current_exception();
                    }
                    task.done = true;
                };
                task.thread = std::async(std::launch::async, payload);
            }
        }

        inline void Channel::drop() noexcept {
            if (owner) {
                owner->drop(*this);
                owner = nullptr;
            }
        }

}
