#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <ratio>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>

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

    inline timespec timepoint_to_timespec(const std::chrono::system_clock::time_point& tp) noexcept {
        using namespace std::chrono;
        return duration_to_timespec(tp - system_clock::time_point());
    }

    inline timeval timepoint_to_timeval(const std::chrono::system_clock::time_point& tp) noexcept {
        using namespace std::chrono;
        return duration_to_timeval(tp - system_clock::time_point());
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

    inline std::chrono::system_clock::time_point timespec_to_timepoint(const timespec& ts) noexcept {
        using namespace std::chrono;
        system_clock::duration d;
        timespec_to_duration(ts, d);
        return system_clock::time_point() + d;
    }

    inline std::chrono::system_clock::time_point timeval_to_timepoint(const timeval& tv) noexcept {
        using namespace std::chrono;
        system_clock::duration d;
        timeval_to_duration(tv, d);
        return system_clock::time_point() + d;
    }

    #ifdef _WIN32

        inline std::chrono::system_clock::time_point filetime_to_timepoint(const FILETIME& ft) noexcept {
            using namespace std::chrono;
            static constexpr int64_t filetime_freq = 10'000'000;        // FILETIME ticks (100 ns) per second
            static constexpr int64_t windows_epoch = 11'644'473'600ll;  // Windows epoch (1601) to Unix epoch (1970) in seconds
            int64_t ticks = (int64_t(ft.dwHighDateTime) << 32) + int64_t(ft.dwLowDateTime);
            int64_t sec = ticks / filetime_freq - windows_epoch;
            int64_t nsec = 100ll * (ticks % filetime_freq);
            return system_clock::from_time_t(time_t(sec)) + duration_cast<system_clock::duration>(nanoseconds(nsec));
        }

        inline FILETIME timepoint_to_filetime(const std::chrono::system_clock::time_point& tp) noexcept {
            using namespace std::chrono;
            static constexpr uint64_t filetime_freq = 10'000'000;        // FILETIME ticks (100 ns) per second
            static constexpr uint64_t windows_epoch = 11'644'473'600ll;  // Windows epoch (1601) to Unix epoch (1970) in seconds
            auto unix_time = tp - system_clock::from_time_t(0);
            uint64_t nsec = duration_cast<nanoseconds>(unix_time).count();
            uint64_t ticks = nsec / 100ll + filetime_freq * windows_epoch;
            return {uint32_t(ticks & 0xfffffffful), uint32_t(ticks >> 32)};
        }

    #endif

    // Time and date parsing

    namespace RS_Detail {

        inline void date_skip_punct(const char*& ptr, const char* end) {
            while (ptr != end && (ascii_ispunct(*ptr) || ascii_isspace(*ptr)))
                ++ptr;
        };

        inline bool date_read_number(const char*& ptr, const char* end, int& result) {
            date_skip_punct(ptr, end);
            auto next = std::find_if_not(ptr, end, ascii_isdigit);
            if (next == ptr)
                return false;
            Ustring sub(ptr, next);
            char* dummy = nullptr;
            result = int(std::strtoul(sub.data(), &dummy, 10));
            ptr = next;
            return true;
        };

        inline bool date_read_number(const char*& ptr, const char* end, double& result) {
            date_skip_punct(ptr, end);
            if (ptr == end || ! ascii_isdigit(*ptr))
                return false;
            auto next = std::find_if_not(ptr, end, ascii_isdigit);
            if (next != end && *next == '.')
                next = std::find_if_not(next + 1, end, ascii_isdigit);
            Ustring sub(ptr, next);
            char* dummy = nullptr;
            result = std::strtod(sub.data(), &dummy);
            ptr = next;
            return true;
        };

        inline bool date_read_month(const char*& ptr, const char* end, int& result) {
            date_skip_punct(ptr, end);
            if (ptr == end)
                return false;
            else if (ascii_isdigit(*ptr))
                return date_read_number(ptr, end, result);
            else if (end - ptr < 3 || ! ascii_isalpha(*ptr))
                return false;
            Ustring mon(ptr, 3);
            mon = ascii_lowercase(mon);
            if (mon == "jan")       result = 1;
            else if (mon == "feb")  result = 2;
            else if (mon == "mar")  result = 3;
            else if (mon == "apr")  result = 4;
            else if (mon == "may")  result = 5;
            else if (mon == "jun")  result = 6;
            else if (mon == "jul")  result = 7;
            else if (mon == "aug")  result = 8;
            else if (mon == "sep")  result = 9;
            else if (mon == "oct")  result = 10;
            else if (mon == "nov")  result = 11;
            else if (mon == "dec")  result = 12;
            else                    return false;
            ptr = std::find_if_not(ptr + 3, end, ascii_isalpha);
            return true;
        };

    }

    inline std::chrono::system_clock::time_point parse_date(Uview str, uint32_t flags = utc_zone | ymd_order) {
        using namespace RS_Detail;
        using namespace std::chrono;
        uint32_t order = flags & (ymd_order | dmy_order | mdy_order);
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(order) > 1 || ibits(zone) > 1 || flags - order - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
        int year = 0, month = 0, day = 0, hour = 0, min = 0;
        double sec = 0;
        auto ptr = str.data(), end = ptr + str.size();
        bool ok = true;
        switch (order) {
            case dmy_order:  ok = date_read_number(ptr, end, day) && date_read_month(ptr, end, month) && date_read_number(ptr, end, year); break;
            case mdy_order:  ok = date_read_month(ptr, end, month) && date_read_number(ptr, end, day) && date_read_number(ptr, end, year); break;
            default:         ok = date_read_number(ptr, end, year) && date_read_month(ptr, end, month) && date_read_number(ptr, end, day); break;
        }
        if (! ok)
            throw std::invalid_argument("Invalid date: " + quote(str));
        date_skip_punct(ptr, end);
        if (ptr != end && (*ptr == 'T' || *ptr == 't'))
            ++ptr;
        date_read_number(ptr, end, hour);
        date_read_number(ptr, end, min);
        date_read_number(ptr, end, sec);
        return make_date(year, month, day, hour, min, sec, zone);
    }

    template <typename R, typename P>
    void parse_time(Uview str, std::chrono::duration<R, P>& t) {
        using namespace std::chrono;
        using namespace std::literals;
        using duration_type = duration<R, P>;
        static constexpr double jyear = 31'557'600;
        Ustring nstr = replace(str, " "s, ""s);
        auto ptr = nstr.data(), end = ptr + nstr.size();
        char sign = '+';
        if (ptr != end && (*ptr == '+' || *ptr == '-'))
            sign = *ptr++;
        if (ptr == end)
            throw std::invalid_argument("Invalid time: " + quote(str));
        double count = 0, seconds = 0;
        char* next = nullptr;
        Ustring unit;
        while (ptr != end) {
            if (! ascii_isdigit(*ptr))
                throw std::invalid_argument("Invalid time: " + quote(str));
            count = std::strtod(ptr, &next);
            unit.clear();
            while (next != end && ascii_isalpha(*next))
                unit += *next++;
            if (starts_with(unit, "Yy"s))         count *= jyear * 1.0e24;
            else if (starts_with(unit, "Zy"s))    count *= jyear * 1.0e21;
            else if (starts_with(unit, "Ey"s))    count *= jyear * 1.0e18;
            else if (starts_with(unit, "Py"s))    count *= jyear * 1.0e15;
            else if (starts_with(unit, "Ty"s))    count *= jyear * 1.0e12;
            else if (starts_with(unit, "Gy"s))    count *= jyear * 1.0e9;
            else if (starts_with(unit, "My"s))    count *= jyear * 1.0e6;
            else if (starts_with(unit, "ky"s))    count *= jyear * 1.0e3;
            else if (starts_with(unit, "ms"s))    count *= 1.0e-3;
            else if (starts_with(unit, "us"s))    count *= 1.0e-6;
            else if (starts_with(unit, u8"µs"s))  count *= 1.0e-6;
            else if (starts_with(unit, "ns"s))    count *= 1.0e-9;
            else if (starts_with(unit, "ps"s))    count *= 1.0e-12;
            else if (starts_with(unit, "fs"s))    count *= 1.0e-15;
            else if (starts_with(unit, "as"s))    count *= 1.0e-18;
            else if (starts_with(unit, "zs"s))    count *= 1.0e-21;
            else if (starts_with(unit, "ys"s))    count *= 1.0e-24;
            else if (starts_with(unit, "y"s))     count *= jyear;
            else if (starts_with(unit, "d"s))     count *= 86400.0;
            else if (starts_with(unit, "h"s))     count *= 3600.0;
            else if (starts_with(unit, "m"s))     count *= 60.0;
            else if (starts_with(unit, "s"s))     {}
            else                                  throw std::invalid_argument("Invalid time: " + quote(str));
            seconds += count;
            ptr = next;
        }
        if (sign == '-')
            seconds = - seconds;
        t = duration_cast<duration_type>(Dseconds(seconds));
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

        inline void PollWait::wait() {
            duration delta = min_wait;
            while (! poll()) {
                std::this_thread::sleep_for(delta);
                delta = std::min(2 * delta, max_wait);
            }
        }

        template <typename R1, typename P1, typename R2, typename P2>
        void PollWait::set_interval(std::chrono::duration<R1, P1> t1, std::chrono::duration<R2, P2> t2) {
            min_wait = std::max(std::chrono::duration_cast<duration>(t1), duration(1));
            max_wait = std::max(std::chrono::duration_cast<duration>(t2), min_wait);
        }

        inline bool PollWait::do_wait_until(time_point t) {
            duration delta = min_wait;
            while (! poll()) {
                duration remain = t - clock_type::now();
                if (remain <= duration())
                    return false;
                std::this_thread::sleep_for(std::min(delta, remain));
                delta = std::min(2 * delta, max_wait);
            }
            return true;
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

        inline Stopwatch::Stopwatch(Uview name, int precision) noexcept {
            try {
                prefix = Ustring(name) + " : ";
                prec = precision;
                start = ReliableClock::now();
            }
            catch (...) {}
        }

        inline Stopwatch::~Stopwatch() noexcept {
            try {
                auto t = ReliableClock::now() - start;
                logx(prefix + format_time(t, prec));
            }
            catch (...) {}
        }

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
