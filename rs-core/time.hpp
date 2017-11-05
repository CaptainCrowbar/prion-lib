#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <ratio>
#include <stdexcept>
#include <type_traits>

namespace RS {

    // Time and date types

    using Dseconds = std::chrono::duration<double>;
    using Ddays = std::chrono::duration<double, std::ratio<86400>>;
    using Dyears = std::chrono::duration<double, std::ratio<31557600>>;
    using ReliableClock = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;

    // Constants

    constexpr auto utc_zone    = uint32_t(1) << 0;
    constexpr auto local_zone  = uint32_t(1) << 1;
    constexpr auto ymd_order   = uint32_t(1) << 2;
    constexpr auto dmy_order   = uint32_t(1) << 3;
    constexpr auto mdy_order   = uint32_t(1) << 4;

    // General time and date operations

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

    inline std::chrono::system_clock::time_point make_date(int year, int month, int day, int hour = 0, int min = 0, double sec = 0, uint32_t flags = utc_zone) {
        using namespace std::chrono;
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(zone) > 1 || flags - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
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
        if (zone == local_zone)
            t = mktime(&stm);
        else
            #ifdef _XOPEN_SOURCE
                t = timegm(&stm);
            #else
                t = _mkgmtime(&stm);
            #endif
        system_clock::time_point::rep extra(int64_t(fsec * system_clock::time_point::duration(seconds(1)).count()));
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

    inline U8string format_date(std::chrono::system_clock::time_point tp, const U8string& format, uint32_t flags = utc_zone) {
        using namespace std::chrono;
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(zone) > 1 || flags - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
        if (format.empty())
            return {};
        auto t = system_clock::to_time_t(tp);
        tm stm = zone == local_zone ? *localtime(&t) : *gmtime(&t);
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

    inline U8string format_date(std::chrono::system_clock::time_point tp, int prec = 0, uint32_t flags = utc_zone) {
        using namespace std::chrono;
        using namespace std::literals;
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(zone) > 1 || flags - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
        U8string result = format_date(tp, "%Y-%m-%d %H:%M:%S"s, zone);
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

    // Time and date parsing

    namespace RS_Detail {

        inline void date_skip_punct(const char*& ptr) {
            while (ascii_ispunct(*ptr) || ascii_isspace(*ptr))
                ++ptr;
        };

        inline bool date_read_number(const char*& ptr, int& result) {
            date_skip_punct(ptr);
            if (! ascii_isdigit(*ptr))
                return false;
            char* next = nullptr;
            result = int(std::strtoul(ptr, &next, 10));
            ptr = next;
            return true;
        };

        inline bool date_read_number(const char*& ptr, double& result) {
            date_skip_punct(ptr);
            if (! ascii_isdigit(*ptr))
                return false;
            char* next = nullptr;
            result = std::strtod(ptr, &next);
            ptr = next;
            return true;
        };

        inline bool date_read_month(const char*& ptr, int& result) {
            date_skip_punct(ptr);
            if (ascii_isdigit(*ptr))
                return date_read_number(ptr, result);
            else if (! ascii_isalpha(*ptr) || ! ptr[1] || ! ptr[2])
                return false;
            U8string mon(ptr, 3);
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
            ptr += 3;
            while (ascii_isalpha(*ptr))
                ++ptr;
            return true;
        };

    }

    inline std::chrono::system_clock::time_point parse_date(const U8string& s, uint32_t flags = utc_zone | ymd_order) {
        using namespace RS_Detail;
        using namespace std::chrono;
        uint32_t order = flags & (ymd_order | dmy_order | mdy_order);
        uint32_t zone = flags & (utc_zone | local_zone);
        if (ibits(order) > 1 || ibits(zone) > 1 || flags - order - zone)
            throw std::invalid_argument("Invalid date flags: 0x" + hex(flags, 1));
        int year = 0, month = 0, day = 0, hour = 0, min = 0;
        double sec = 0;
        const char* ptr = s.data();
        bool ok = true;
        switch (order) {
            case dmy_order:  ok = date_read_number(ptr, day) && date_read_month(ptr, month) && date_read_number(ptr, year); break;
            case mdy_order:  ok = date_read_month(ptr, month) && date_read_number(ptr, day) && date_read_number(ptr, year); break;
            default:         ok = date_read_number(ptr, year) && date_read_month(ptr, month) && date_read_number(ptr, day); break;
        }
        if (! ok)
            throw std::invalid_argument("Invalid date: " + quote(s));
        date_skip_punct(ptr);
        if (*ptr == 'T' || *ptr == 't')
            ++ptr;
        date_read_number(ptr, hour);
        date_read_number(ptr, min);
        date_read_number(ptr, sec);
        return make_date(year, month, day, hour, min, sec, zone);
    }

    template <typename R, typename P>
    void parse_time(const U8string& s, std::chrono::duration<R, P>& t) {
        using namespace std::chrono;
        using namespace std::literals;
        using duration_type = duration<R, P>;
        static constexpr double jyear = 31'557'600;
        U8string ns = replace(s, " "s, ""s);
        const char* ptr = ns.data();
        const char* end = ptr + ns.size();
        char sign = '+';
        if (*ptr == '+' || *ptr == '-')
            sign = *ptr++;
        if (ptr == end)
            throw std::invalid_argument("Invalid time: " + quote(s));
        double count = 0, seconds = 0;
        char* next = nullptr;
        U8string unit;
        while (ptr != end) {
            if (! ascii_isdigit(*ptr))
                throw std::invalid_argument("Invalid time: " + quote(s));
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
            else                                  throw std::invalid_argument("Invalid time: " + quote(s));
            seconds += count;
            ptr = next;
        }
        if (sign == '-')
            seconds = - seconds;
        t = duration_cast<duration_type>(Dseconds(seconds));
    }

    template <typename D>
    D parse_time(const U8string& str) {
        D time;
        parse_time(str, time);
        return time;
    }

    // Timing utilities

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

}
