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
    template <typename T> class QueueChannel;
    class StreamChannel;
    class ThrottleChannel;
    class TimerChannel;
    class TrueChannel;
    template <typename T> class ValueChannel;

    // Channel base classes

    class Channel:
    public Wait {
    public:
        virtual ~Channel() noexcept { drop(); }
        Channel(const Channel&) = delete;
        Channel(Channel&& c) = default;
        Channel& operator=(const Channel&) = delete;
        Channel& operator=(Channel&& c) noexcept { if (&c != this) drop(); return *this; }
        virtual void close() noexcept = 0;
        virtual bool is_closed() const noexcept = 0;
        virtual bool is_async() const noexcept { return true; }
    private:
        friend class EventChannel;
        template <typename T> friend class MessageChannel;
        friend class StreamChannel;
        Channel() = default;
        void drop() noexcept;
    };

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
            for (;;) {
                if (! wait_for(seconds(1)))
                    continue;
                if (is_closed())
                    break;
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
        virtual bool do_wait_for(duration /*t*/) { return true; }
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
        virtual bool do_wait_for(duration t);
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

        inline bool FalseChannel::do_wait_for(duration t) {
            auto lock = make_lock(mutex);
            if (! open)
                return true;
            else if (t <= duration())
                return false;
            cv.wait_for(lock, t, [&] { return ! open; });
            return ! open;
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
        duration interval() const noexcept { return delta; }
        auto next() const noexcept { return next_tick; }
    protected:
        virtual bool do_wait_for(duration t);
    private:
        using clock_type = ReliableClock;
        using time_point = clock_type::time_point;
        mutable std::mutex mutex;
        std::condition_variable cv;
        time_point next_tick;
        duration delta;
        bool open = true;
    };

        template <typename R, typename P>
        TimerChannel::TimerChannel(std::chrono::duration<R, P> t) noexcept {
            using namespace std::chrono;
            delta = std::max(duration_cast<duration>(t), duration());
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

        inline bool TimerChannel::do_wait_for(duration t) {
            using namespace std::chrono;
            auto lock = make_lock(mutex);
            if (! open)
                return true;
            auto now = clock_type::now();
            if (next_tick <= now) {
                next_tick += delta;
                return true;
            }
            if (t <= duration())
                return false;
            auto remaining = duration_cast<duration>(next_tick - now);
            if (t < remaining) {
                cv.wait_for(lock, t, [&] { return ! open; });
                return ! open;
            }
            cv.wait_for(lock, remaining, [&] { return ! open; });
            if (open)
                next_tick += delta;
            return true;
        }

    class ThrottleChannel:
    public EventChannel {
    public:
        RS_NO_COPY_MOVE(ThrottleChannel);
        template <typename R, typename P> explicit ThrottleChannel(std::chrono::duration<R, P> t) noexcept;
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool is_shared() const noexcept { return true; }
        duration interval() const noexcept { return delta; }
    protected:
        virtual bool do_wait_for(duration t);
    private:
        using clock_type = ReliableClock;
        using time_point = clock_type::time_point;
        mutable std::mutex mutex;
        std::condition_variable cv;
        time_point next = time_point::min();
        duration delta;
        bool open = true;
    };

        template <typename R, typename P>
        ThrottleChannel::ThrottleChannel(std::chrono::duration<R, P> t) noexcept {
            using namespace std::chrono;
            delta = std::max(duration_cast<duration>(t), duration());
        }

        inline void ThrottleChannel::close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
            cv.notify_all();
        }

        inline bool ThrottleChannel::do_wait_for(duration t) {
            using namespace std::chrono;
            auto lock = make_lock(mutex);
            if (! open)
                return true;
            auto now = clock_type::now();
            if (next <= now) {
                next = now + delta;
                return true;
            }
            if (t <= duration())
                return false;
            auto remaining = duration_cast<duration>(next - now);
            if (t < remaining) {
                cv.wait_for(lock, t, [&] { return ! open; });
                return ! open;
            }
            cv.wait_for(lock, remaining, [&] { return ! open; });
            if (open)
                next = clock_type::now() + delta;
            return true;
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
        virtual bool do_wait_for(Wait::duration /*t*/) { return true; }
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
        virtual bool do_wait_for(Wait::duration t);
    private:
        std::mutex mutex;
        std::condition_variable cv;
        bool open = true;
        std::deque<T> queue;
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
            if (! open || queue.empty())
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
        bool QueueChannel<T>::do_wait_for(Wait::duration t) {
            auto lock = make_lock(mutex);
            if (open && queue.empty() && t > Wait::duration())
                cv.wait_for(lock, t, [&] { return ! open || ! queue.empty(); });
            return ! open || ! queue.empty();
        }

    template <typename T>
    class ValueChannel:
    public MessageChannel<T> {
    public:
        RS_NO_COPY_MOVE(ValueChannel);
        ValueChannel() = default;
        explicit ValueChannel(const T& t): value(t) {}
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return status == -1; }
        virtual bool is_shared() const noexcept { return true; }
        virtual bool read(T& t);
        void clear() noexcept;
        bool write(const T& t);
        bool write(T&& t);
    protected:
        virtual bool do_wait_for(Wait::duration t);
    private:
        std::mutex mutex;
        std::condition_variable cv;
        T value;
        int status = 0; // +1 = new value, 0 = no change, -1 = closed
    };

        template <typename T>
        void ValueChannel<T>::close() noexcept {
            auto lock = make_lock(mutex);
            status = -1;
            cv.notify_all();
        }

        template <typename T>
        bool ValueChannel<T>::read(T& t) {
            auto lock = make_lock(mutex);
            if (status != 1)
                return false;
            t = value;
            status = 0;
            return true;
        }

        template <typename T>
        bool ValueChannel<T>::write(const T& t) {
            auto lock = make_lock(mutex);
            if (status == -1)
                return false;
            if (t == value)
                return true;
            value = t;
            status = 1;
            cv.notify_all();
            return true;
        }

        template <typename T>
        bool ValueChannel<T>::write(T&& t) {
            auto lock = make_lock(mutex);
            if (status == -1)
                return false;
            if (t == value)
                return true;
            value = std::move(t);
            status = 1;
            cv.notify_all();
            return true;
        }

        template <typename T>
        bool ValueChannel<T>::do_wait_for(Wait::duration t) {
            auto lock = make_lock(mutex);
            if (status == 0 && t > Wait::duration())
                cv.wait_for(lock, t, [&] { return status != 0; });
            return status;
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
        virtual bool do_wait_for(duration t);
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

        inline bool BufferChannel::do_wait_for(duration t) {
            auto lock = make_lock(mutex);
            if (open && ofs == buf.size() && t > duration())
                cv.wait_for(lock, t, [&] { return ! open || ofs < buf.size(); });
            return ! open || ofs < buf.size();
        }

    // Global dispatch queue

    class Dispatch {
    public:
        RS_NO_INSTANCE(Dispatch);
        enum class mode { sync = 1, async };
        enum class reason { empty = 1, closed, error };
        struct result_type {
            Channel* channel = nullptr;
            std::exception_ptr error;
            void rethrow() const { if (error) std::rethrow_exception(error); }
            reason why() const noexcept { return error ? reason::error : channel ? reason::closed : reason::empty; }
        };
        template <typename F> static void add(EventChannel& chan, mode m, F func);
        template <typename T, typename F> static void add(MessageChannel<T>& chan, mode m, F func);
        template <typename F> static void add(StreamChannel& chan, mode m, F func);
        static void drop(Channel& chan) noexcept { tasks_ref().erase(&chan); }
        static bool empty() noexcept { return tasks_ref().empty(); }
        static result_type run() noexcept;
        static void stop() noexcept;
    private:
        using callback = std::function<void()>;
        struct task_info {
            mode runmode;
            callback call;
            std::future<void> thread;
            std::atomic<bool> done;
            std::exception_ptr error;
        };
        using task_map = std::map<Channel*, task_info>;
        static void add_task(Channel& chan, mode m, callback call);
        static task_map& tasks_ref() noexcept;
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
            EventChannel::callback handler(func);
            if (handler)
                add_task(chan, m, handler);
        }

        template <typename T, typename F>
        void Dispatch::add(MessageChannel<T>& chan, mode m, F func) {
            typename MessageChannel<T>::callback handler(func);
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
            StreamChannel::callback handler(func);
            if (! handler)
                return;
            auto call = [&chan,handler,s=std::string()] () mutable {
                if (chan.read_to(s))
                    handler(s);
            };
            add_task(chan, m, call);
        }

        inline Dispatch::result_type Dispatch::run() noexcept {
            using namespace std::chrono;
            using namespace std::literals;
            static constexpr Wait::duration min_interval = 1us;
            static constexpr Wait::duration max_interval = 1ms;
            result_type rc;
            if (tasks_ref().empty())
                return rc;
            auto guard = scope_exit([&] { if (rc.channel) drop(*rc.channel); });
            int waits = 0;
            auto interval = min_interval;
            for (;;) {
                int calls = 0;
                for (auto& t: tasks_ref()) {
                    rc.channel = t.first;
                    if (t.second.runmode == Dispatch::mode::sync) {
                        try {
                            if (t.first->poll()) {
                                if (t.first->is_closed())
                                    return rc;
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
            for (auto& t: tasks_ref())
                t.first->close();
            while (! tasks_ref().empty())
                run();
        }

        inline void Dispatch::add_task(Channel& chan, mode m, callback call) {
            using namespace std::chrono;
            if (! chan.is_shared() && tasks_ref().count(&chan))
                throw std::invalid_argument("Dispatch channel is not shareable");
            if (m != Dispatch::mode::sync && m != Dispatch::mode::async)
                throw std::invalid_argument("Invalid dispatch mode");
            if (m == Dispatch::mode::async && ! chan.is_async())
                throw std::invalid_argument("Invalid dispatch mode for channel");
            if (! call)
                throw std::invalid_argument("Dispatch callback is null");
            auto& task = tasks_ref()[&chan];
            task.runmode = m;
            task.call = call;
            if (task.runmode == Dispatch::mode::async) {
                auto payload = [&] () noexcept {
                    try {
                        for (;;) {
                            if (! chan.wait_for(seconds(1)))
                                continue;
                            if (chan.is_closed())
                                break;
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

        inline Dispatch::task_map& Dispatch::tasks_ref() noexcept {
            static task_map map;
            return map;
        }

        inline void Channel::drop() noexcept {
            Dispatch::drop(*this);
        }

}
