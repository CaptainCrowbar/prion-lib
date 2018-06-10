#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <ratio>
#include <type_traits>

namespace RS {

    // Time and date types

    using Dseconds = std::chrono::duration<double>;
    using Ddays = std::chrono::duration<double, std::ratio<86400>>;
    using Dyears = std::chrono::duration<double, std::ratio<31557600>>;
    using ReliableClock = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

    // Constants

    constexpr uint32_t ymd_order = 4;
    constexpr uint32_t dmy_order = 8;
    constexpr uint32_t mdy_order = 16;

    // General time and date operations

    namespace RS_Detail {

        template <typename C1, typename D1, typename C2, typename D2>
        struct ConvertTimePointHelper {
            std::chrono::time_point<C2, D2> operator()(std::chrono::time_point<C1, D1> t) const {
                auto now1 = C1::now();
                auto now2 = C2::now();
                auto now3 = C1::now();
                now1 += (now3 - now1) / 2;
                return std::chrono::time_point_cast<D2>(now2 + (t - now1));
            }
        };

        template <typename C, typename D1, typename D2>
        struct ConvertTimePointHelper<C, D1, C, D2> {
            std::chrono::time_point<C, D2> operator()(std::chrono::time_point<C, D1> t) const {
                return std::chrono::time_point_cast<D2>(t);
            }
        };

    }

    template <typename C1, typename D1, typename C2, typename D2>
    void convert_time_point(std::chrono::time_point<C1, D1> src, std::chrono::time_point<C2, D2>& dst) {
        dst = RS_Detail::ConvertTimePointHelper<C1, D1, C2, D2>()(src);
    }

    template <typename TP2, typename C1, typename D1>
    TP2 convert_time_point(std::chrono::time_point<C1, D1> src) {
        return RS_Detail::ConvertTimePointHelper<C1, D1, typename TP2::clock, typename TP2::duration>()(src);
    }

    // System specific time and date conversions

    timespec timepoint_to_timespec(const std::chrono::system_clock::time_point& tp) noexcept;
    timeval timepoint_to_timeval(const std::chrono::system_clock::time_point& tp) noexcept;
    std::chrono::system_clock::time_point timespec_to_timepoint(const timespec& ts) noexcept;
    std::chrono::system_clock::time_point timeval_to_timepoint(const timeval& tv) noexcept;

    template <typename R, typename P>
    timespec duration_to_timespec(const std::chrono::duration<R, P>& d) noexcept {
        using namespace RS::Literals;
        using namespace std::chrono;
        static constexpr int64_t G = 1'000'000'000ll;
        int64_t nsec = duration_cast<nanoseconds>(d).count();
        return {time_t(nsec / G), long(nsec % G)};
    }

    template <typename R, typename P>
    timeval duration_to_timeval(const std::chrono::duration<R, P>& d) noexcept {
        using namespace RS::Literals;
        using namespace std::chrono;
        #ifdef _XOPEN_SOURCE
            using sec_type = time_t;
            using usec_type = suseconds_t;
        #else
            using sec_type = long;
            using usec_type = long;
        #endif
        static constexpr int64_t M = 1'000'000;
        int64_t usec = duration_cast<microseconds>(d).count();
        return {sec_type(usec / M), usec_type(usec % M)};
    }

    template <typename R, typename P>
    void timespec_to_duration(const timespec& ts, std::chrono::duration<R, P>& d) noexcept {
        using namespace std::chrono;
        using D = duration<R, P>;
        d = duration_cast<D>(seconds(ts.tv_sec)) + duration_cast<D>(nanoseconds(ts.tv_nsec));
    }

    template <typename R, typename P>
    void timeval_to_duration(const timeval& tv, std::chrono::duration<R, P>& d) noexcept {
        using namespace std::chrono;
        using D = duration<R, P>;
        d = duration_cast<D>(seconds(tv.tv_sec)) + duration_cast<D>(microseconds(tv.tv_usec));
    }

    #ifdef _WIN32

        std::chrono::system_clock::time_point filetime_to_timepoint(const FILETIME& ft) noexcept;
        FILETIME timepoint_to_filetime(const std::chrono::system_clock::time_point& tp) noexcept;

    #endif

    // Time and date parsing

    namespace RS_Detail {

        double parse_time_as_seconds(Uview str);

    }

    std::chrono::system_clock::time_point parse_date(Uview str, uint32_t flags = utc_zone | ymd_order);

    template <typename R, typename P>
    void parse_time(Uview str, std::chrono::duration<R, P>& t) {
        using namespace std::chrono;
        double seconds = RS_Detail::parse_time_as_seconds(str);
        t = duration_cast<duration<R, P>>(Dseconds(seconds));
    }

    template <typename D>
    D parse_time(Uview str) {
        D time;
        parse_time(str, time);
        return time;
    }

    // Timing base classes

    class Wait {
    public:
        using clock_type = ReliableClock;
        using duration = clock_type::duration;
        using time_point = clock_type::time_point;
        virtual ~Wait() noexcept = 0;
        virtual bool is_shared() const noexcept { return false; }
        virtual bool poll() { return do_wait_for({}); }
        virtual void wait() { while (! do_wait_for(std::chrono::seconds(1))) {} }
        template <typename R, typename P> bool wait_for(std::chrono::duration<R, P> t) { return do_wait_for(std::chrono::duration_cast<duration>(t)); }
        template <typename C, typename D> bool wait_until(std::chrono::time_point<C, D> t) { return do_wait_until(convert_time_point<time_point>(t)); }
    protected:
        Wait() noexcept {}
        virtual bool do_wait_for(duration t) { return do_wait_until(clock_type::now() + t); }
        virtual bool do_wait_until(time_point t) { return do_wait_for(t - clock_type::now()); }
    };

        inline Wait::~Wait() noexcept {}

    class PollWait:
    public Wait {
    public:
        virtual bool poll() override = 0;
        virtual void wait() override;
        duration min_interval() const noexcept { return min_wait; }
        duration max_interval() const noexcept { return max_wait; }
        template <typename R, typename P> void set_interval(std::chrono::duration<R, P> t) { set_interval(t, t); }
        template <typename R1, typename P1, typename R2, typename P2> void set_interval(std::chrono::duration<R1, P1> t1, std::chrono::duration<R2, P2> t2);
    protected:
        PollWait() noexcept {}
        virtual bool do_wait_until(time_point t) final;
    private:
        duration min_wait = std::chrono::microseconds(10);
        duration max_wait = std::chrono::milliseconds(10);
    };

        template <typename R1, typename P1, typename R2, typename P2>
        void PollWait::set_interval(std::chrono::duration<R1, P1> t1, std::chrono::duration<R2, P2> t2) {
            min_wait = std::max(std::chrono::duration_cast<duration>(t1), duration(1));
            max_wait = std::max(std::chrono::duration_cast<duration>(t2), min_wait);
        }

    // Timing utilities

    class PollCondition:
    public PollWait {
    public:
        template <typename Predicate> explicit PollCondition(Predicate p): pred(p) {}
        virtual bool poll() final { return pred(); }
    private:
        std::function<bool()> pred;
    };

    class Stopwatch {
    public:
        RS_NO_COPY_MOVE(Stopwatch)
        explicit Stopwatch(Uview name, int precision = 3) noexcept;
        ~Stopwatch() noexcept;
    private:
        Ustring prefix;
        int prec;
        ReliableClock::time_point start;
    };

    template <typename D> class Timer;

    template <typename R, typename P>
    class Timer<std::chrono::duration<R, P>> {
    public:
        RS_MOVE_ONLY(Timer);
        using duration = std::chrono::duration<R, P>;
        Timer() { reset(); }
        ~Timer() = default;
        operator duration() const { return get(); }
        duration get() const { return std::chrono::duration_cast<duration>(ReliableClock::now() - start); }
        void reset() { start = ReliableClock::now(); }
    private:
        ReliableClock::time_point start;
    };

}
