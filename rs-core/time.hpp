#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <ratio>
#include <type_traits>

namespace RS {

    // Time and date types

    using Dseconds = std::chrono::duration<double>;
    using Ddays = std::chrono::duration<double, std::ratio<86400>>;
    using Dyears = std::chrono::duration<double, std::ratio<31557600>>;
    using ReliableClock = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

    // General time and date operations

    RS_ENUM_CLASS(Zone, int, 1, utc, local)

    template <typename R, typename P>
    void from_seconds(double s, std::chrono::duration<R, P>& d) noexcept {
        using namespace std::chrono;
        d = duration_cast<duration<R, P>>(Dseconds(s));
    }

    template <typename R, typename P>
    double to_seconds(const std::chrono::duration<R, P>& d) noexcept {
        using namespace std::chrono;
        return duration_cast<Dseconds>(d).count();
    }

    inline std::chrono::system_clock::time_point make_date(int year, int month, int day, int hour = 0, int min = 0, double sec = 0, Zone z = Zone::utc) noexcept {
        using namespace std::chrono;
        double isec = 0, fsec = modf(sec, &isec);
        if (fsec < 0) {
            isec -= 1;
            fsec += 1;
        }
        tm stm;
        memset(&stm, 0, sizeof(stm));
        stm.tm_sec = int(isec);
        stm.tm_min = min;
        stm.tm_hour = hour;
        stm.tm_mday = day;
        stm.tm_mon = month - 1;
        stm.tm_year = year - 1900;
        stm.tm_isdst = -1;
        time_t t;
        if (z == Zone::local)
            t = mktime(&stm);
        else
            #ifdef _XOPEN_SOURCE
                t = timegm(&stm);
            #else
                t = _mkgmtime(&stm);
            #endif
        system_clock::time_point::rep extra(fsec * system_clock::time_point::duration(seconds(1)).count());
        return system_clock::from_time_t(t) + system_clock::time_point::duration(extra);
    }

    // Time and date formatting

    // Unfortunately strftime() doesn't set errno and simply returns zero on
    // any error. This means that there is no way to distinguish between an
    // invalid format string, an output buffer that is too small, and a
    // legitimately empty result. Here we try first with a reasonable output
    // length, and if that fails, try again with a much larger one; if it
    // still fails, give up. This could theoretically fail in the face of a
    // very long localized date format, but there doesn't seem to be a better
    // solution.

    inline U8string format_date(std::chrono::system_clock::time_point tp, const U8string& format, Zone z = Zone::utc) {
        using namespace std::chrono;
        auto t = system_clock::to_time_t(tp);
        tm stm = z == Zone::local ? *localtime(&t) : *gmtime(&t);
        U8string result(std::max(2 * format.size(), size_t(100)), '\0');
        auto rc = strftime(&result[0], result.size(), format.data(), &stm);
        if (rc == 0) {
            result.resize(10 * result.size(), '\0');
            rc = strftime(&result[0], result.size(), format.data(), &stm);
        }
        result.resize(rc);
        result.shrink_to_fit();
        return result;
    }

    inline U8string format_date(std::chrono::system_clock::time_point tp, int prec = 0, Zone z = Zone::utc) {
        using namespace std::chrono;
        using namespace std::literals;
        U8string result = format_date(tp, "%Y-%m-%d %H:%M:%S"s, z);
        if (prec > 0) {
            double sec = to_seconds(tp.time_since_epoch());
            double isec;
            double fsec = modf(sec, &isec);
            U8string buf(prec + 3, '\0');
            snprintf(&buf[0], buf.size(), "%.*f", prec, fsec);
            result += buf.data() + 1;
        }
        return result;
    }

    template <typename R, typename P>
    U8string format_time(const std::chrono::duration<R, P>& time, int prec = 0) {
        using namespace RS_Detail;
        using namespace std::chrono;
        auto whole = duration_cast<seconds>(time);
        int64_t isec = whole.count();
        auto frac = time - duration_cast<duration<R, P>>(whole);
        double fsec = duration_cast<Dseconds>(frac).count();
        U8string result;
        if (isec < 0 || fsec < 0)
            result += '-';
        isec = std::abs(isec);
        fsec = std::abs(fsec);
        int64_t d = isec / 86400;
        isec -= 86400 * d;
        if (d)
            result += decfmt(d) + 'd';
        int64_t h = isec / 3600;
        isec -= 3600 * h;
        if (d || h)
            result += decfmt(h, d ? 2 : 1) + 'h';
        int64_t m = isec / 60;
        if (d || h || m)
            result += decfmt(m, d || h ? 2 : 1) + 'm';
        isec -= 60 * m;
        result += decfmt(isec, d || h || m ? 2 : 1);
        if (prec > 0) {
            U8string buf(prec + 3, '\0');
            snprintf(&buf[0], buf.size(), "%.*f", prec, fsec);
            result += buf.data() + 1;
        }
        result += 's';
        return result;
    }

    // Timing utilities

    namespace RS_Detail {

        #ifdef _XOPEN_SOURCE

            using SleepUnit = std::chrono::microseconds;

            inline void sleep_for(SleepUnit t) noexcept {
                static constexpr unsigned long M = 1'000'000;
                auto usec = t.count();
                if (usec > 0) {
                    timeval tv;
                    tv.tv_sec = usec / M;
                    tv.tv_usec = usec % M;
                    select(0, nullptr, nullptr, nullptr, &tv);
                } else {
                    sched_yield();
                }
            }

        #else

            using SleepUnit = std::chrono::milliseconds;

            inline void sleep_for(SleepUnit t) noexcept {
                auto msec = t.count();
                if (msec > 0)
                    Sleep(uint32_t(msec));
                else
                    Sleep(0);
            }

        #endif

    }

    class Stopwatch {
    public:
        RS_NO_COPY_MOVE(Stopwatch)
        explicit Stopwatch(const U8string& name, int precision = 3) noexcept: Stopwatch(name.data(), precision) {}
        explicit Stopwatch(const char* name, int precision = 3) noexcept {
            try {
                prefix = name;
                prefix += " : ";
                prec = precision;
                start = ReliableClock::now();
            }
            catch (...) {}
        }
        ~Stopwatch() noexcept {
            try {
                auto t = ReliableClock::now() - start;
                logx(prefix + format_time(t, prec));
            }
            catch (...) {}
        }
    private:
        U8string prefix;
        int prec;
        ReliableClock::time_point start;
    };

    template <typename R, typename P>
    void sleep_for(std::chrono::duration<R, P> t) noexcept {
        using namespace std::chrono;
        RS_Detail::sleep_for(duration_cast<RS_Detail::SleepUnit>(t));
    }

    inline void sleep_for(double t) noexcept {
        using namespace std::chrono;
        RS_Detail::sleep_for(duration_cast<RS_Detail::SleepUnit>(Dseconds(t)));
    }

    template <typename C, typename D>
    void sleep_until(std::chrono::time_point<C, D> t) noexcept {
        sleep_for(t - C::now());
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
            using namespace RS::Literals;
            using namespace std::chrono;
            static constexpr int64_t filetime_freq = 10'000'000; // FILETIME ticks (100 ns) per second
            static constexpr int64_t windows_epoch = 11'644'473'600ll; // Windows epoch (1601) to Unix epoch (1970)
            int64_t ticks = (int64_t(ft.dwHighDateTime) << 32) + int64_t(ft.dwLowDateTime);
            int64_t sec = ticks / filetime_freq - windows_epoch;
            int64_t nsec = 100ll * (ticks % filetime_freq);
            return system_clock::from_time_t(time_t(sec)) + duration_cast<system_clock::duration>(nanoseconds(nsec));
        }

        inline FILETIME timepoint_to_filetime(const std::chrono::system_clock::time_point& tp) noexcept {
            using namespace std::chrono;
            auto unix_time = tp - system_clock::from_time_t(0);
            uint64_t nsec = duration_cast<nanoseconds>(unix_time).count();
            uint64_t ticks = nsec / 100ll;
            return {uint32_t(ticks & 0xfffffffful), uint32_t(ticks >> 32)};
        }

    #endif

}
