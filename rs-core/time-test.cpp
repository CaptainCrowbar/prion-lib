#include "rs-core/time.hpp"
#include "rs-core/unit-test.hpp"
#include <chrono>
#include <cmath>
#include <ratio>

#ifdef _XOPEN_SOURCE
    #include <sys/time.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

void test_core_time_date_types() {

    auto ds = Dseconds(1.25);
    auto ms = duration_cast<milliseconds>(ds);
    TEST_EQUAL(ms.count(), 1250);

    auto dd = Ddays(7);
    auto s = duration_cast<seconds>(dd);
    TEST_EQUAL(s.count(), 604800);

    auto dy = Dyears(100);
    s = duration_cast<seconds>(dy);
    TEST_EQUAL(s.count(), 3155760000);

    TEST(ReliableClock::is_steady);
    TEST_COMPARE(ReliableClock::now().time_since_epoch().count(), >, 0);

}

void test_core_time_general_operations() {

    hours h;
    minutes m;
    seconds s;
    milliseconds ms;
    system_clock::time_point tp;
    int64_t n1, n2;

    TRY(from_seconds(7200.0, h));   TEST_EQUAL(h.count(), 2);
    TRY(from_seconds(7200.0, m));   TEST_EQUAL(m.count(), 120);
    TRY(from_seconds(7200.0, s));   TEST_EQUAL(s.count(), 7200);
    TRY(from_seconds(7200.0, ms));  TEST_EQUAL(ms.count(), 7'200'000);

    TEST_EQUAL(to_seconds(hours(10)), 36000);
    TEST_EQUAL(to_seconds(minutes(10)), 600);
    TEST_EQUAL(to_seconds(seconds(10)), 10);
    TEST_NEAR(to_seconds(milliseconds(10)), 0.01);

    TRY(tp = make_date(1970, 1, 1, 0, 0, 0));
    TRY(n1 = int64_t(system_clock::to_time_t(tp)));
    TEST_EQUAL(n1, 0);
    TRY(tp = make_date(2000, 1, 2, 3, 4, 5));
    TRY(n1 = int64_t(system_clock::to_time_t(tp)));
    TEST_EQUAL(n1, 946'782'245);
    TRY(tp = make_date(2000, 1, 2, 3, 4, 5, Zone::local));
    TRY(n2 = int64_t(system_clock::to_time_t(tp)));
    TEST_COMPARE(std::abs(n2 - n1), <=, 86400);

}

void test_core_time_date_formatting() {

    system_clock::time_point tp;
    system_clock::duration d;
    U8string str;

    TRY(tp = make_date(2000, 1, 2, 3, 4, 5));
    TRY(str = format_date(tp));
    TEST_EQUAL(str, "2000-01-02 03:04:05");
    TRY(str = format_date(tp, 3));
    TEST_EQUAL(str, "2000-01-02 03:04:05.000");
    TRY(from_seconds(0.12345, d));
    TRY(tp += d);
    TRY(str = format_date(tp));
    TEST_EQUAL(str, "2000-01-02 03:04:05");
    TRY(str = format_date(tp, 3));
    TEST_EQUAL(str, "2000-01-02 03:04:05.123");
    TRY(from_seconds(0.44444, d));
    TRY(tp += d);
    TRY(str = format_date(tp));
    TEST_EQUAL(str, "2000-01-02 03:04:05");
    TRY(str = format_date(tp, 3));
    TEST_EQUAL(str, "2000-01-02 03:04:05.568");
    TRY(str = format_date(tp, "%d/%m/%Y %H:%M"));
    TEST_EQUAL(str, "02/01/2000 03:04");
    TRY(str = format_date(tp, ""));
    TEST_EQUAL(str, "");
    TRY(tp = make_date(2000, 1, 2, 3, 4, 5, Zone::local));
    TRY(str = format_date(tp, 0, Zone::local));
    TEST_EQUAL(str, "2000-01-02 03:04:05");

    TEST_EQUAL(format_time(Dseconds(0)), "0s");
    TEST_EQUAL(format_time(Dseconds(0), 3), "0.000s");
    TEST_EQUAL(format_time(Dseconds(0.25), 3), "0.250s");
    TEST_EQUAL(format_time(Dseconds(0.5), 3), "0.500s");
    TEST_EQUAL(format_time(Dseconds(0.75), 3), "0.750s");
    TEST_EQUAL(format_time(Dseconds(1), 3), "1.000s");
    TEST_EQUAL(format_time(Dseconds(1.25), 3), "1.250s");
    TEST_EQUAL(format_time(Dseconds(59.999), 3), "59.999s");
    TEST_EQUAL(format_time(Dseconds(60), 3), "1m00.000s");
    TEST_EQUAL(format_time(Dseconds(1234), 3), "20m34.000s");
    TEST_EQUAL(format_time(Dseconds(12345), 3), "3h25m45.000s");
    TEST_EQUAL(format_time(Dseconds(123456), 3), "1d10h17m36.000s");
    TEST_EQUAL(format_time(Dseconds(1234567), 3), "14d06h56m07.000s");
    TEST_EQUAL(format_time(Dseconds(12345678), 3), "142d21h21m18.000s");
    TEST_EQUAL(format_time(Dseconds(123456789), 3), "1428d21h33m09.000s");
    TEST_EQUAL(format_time(Dseconds(-0.25), 3), "-0.250s");
    TEST_EQUAL(format_time(Dseconds(-0.5), 3), "-0.500s");
    TEST_EQUAL(format_time(Dseconds(-0.75), 3), "-0.750s");
    TEST_EQUAL(format_time(Dseconds(-1), 3), "-1.000s");
    TEST_EQUAL(format_time(Dseconds(-1.25), 3), "-1.250s");
    TEST_EQUAL(format_time(Dseconds(-59.999), 3), "-59.999s");
    TEST_EQUAL(format_time(Dseconds(-60), 3), "-1m00.000s");
    TEST_EQUAL(format_time(Dseconds(-1234), 3), "-20m34.000s");
    TEST_EQUAL(format_time(Dseconds(-12345), 3), "-3h25m45.000s");
    TEST_EQUAL(format_time(Dseconds(-123456), 3), "-1d10h17m36.000s");
    TEST_EQUAL(format_time(Dseconds(-1234567), 3), "-14d06h56m07.000s");
    TEST_EQUAL(format_time(Dseconds(-12345678), 3), "-142d21h21m18.000s");
    TEST_EQUAL(format_time(Dseconds(-123456789), 3), "-1428d21h33m09.000s");
    TEST_EQUAL(format_time(nanoseconds(1), 10), "0.0000000010s");
    TEST_EQUAL(format_time(microseconds(1), 7), "0.0000010s");
    TEST_EQUAL(format_time(milliseconds(1), 4), "0.0010s");
    TEST_EQUAL(format_time(seconds(1)), "1s");
    TEST_EQUAL(format_time(minutes(1)), "1m00s");
    TEST_EQUAL(format_time(hours(1)), "1h00m00s");
    TEST_EQUAL(format_time(nanoseconds(-1), 10), "-0.0000000010s");
    TEST_EQUAL(format_time(microseconds(-1), 7), "-0.0000010s");
    TEST_EQUAL(format_time(milliseconds(-1), 4), "-0.0010s");
    TEST_EQUAL(format_time(seconds(-1)), "-1s");
    TEST_EQUAL(format_time(minutes(-1)), "-1m00s");
    TEST_EQUAL(format_time(hours(-1)), "-1h00m00s");

}

void test_core_time_system_specific_conversions() {

    using IntMsec = duration<int64_t, std::ratio<1, 1000>>;
    using IntSec = duration<int64_t>;
    using IntDays = duration<int64_t, std::ratio<86400>>;

    IntMsec ims;
    IntSec is;
    IntDays id;
    Dseconds fs;
    timespec ts;
    timeval tv;

    ts = {0, 0};                 TRY(timespec_to_duration(ts, fs));   TEST_EQUAL(fs.count(), 0);
    ts = {0, 0};                 TRY(timespec_to_duration(ts, id));   TEST_EQUAL(id.count(), 0);
    ts = {0, 0};                 TRY(timespec_to_duration(ts, ims));  TEST_EQUAL(ims.count(), 0);
    ts = {0, 0};                 TRY(timespec_to_duration(ts, is));   TEST_EQUAL(is.count(), 0);
    ts = {0, 125'000'000};       TRY(timespec_to_duration(ts, fs));   TEST_EQUAL(fs.count(), 0.125);
    ts = {0, 125'000'000};       TRY(timespec_to_duration(ts, id));   TEST_EQUAL(id.count(), 0);
    ts = {0, 125'000'000};       TRY(timespec_to_duration(ts, ims));  TEST_EQUAL(ims.count(), 125);
    ts = {0, 125'000'000};       TRY(timespec_to_duration(ts, is));   TEST_EQUAL(is.count(), 0);
    ts = {86'400, 0};            TRY(timespec_to_duration(ts, fs));   TEST_EQUAL(fs.count(), 86'400);
    ts = {86'400, 0};            TRY(timespec_to_duration(ts, id));   TEST_EQUAL(id.count(), 1);
    ts = {86'400, 0};            TRY(timespec_to_duration(ts, ims));  TEST_EQUAL(ims.count(), 86'400'000);
    ts = {86'400, 0};            TRY(timespec_to_duration(ts, is));   TEST_EQUAL(is.count(), 86'400);
    ts = {86'400, 125'000'000};  TRY(timespec_to_duration(ts, fs));   TEST_EQUAL(fs.count(), 86'400.125);
    ts = {86'400, 125'000'000};  TRY(timespec_to_duration(ts, id));   TEST_EQUAL(id.count(), 1);
    ts = {86'400, 125'000'000};  TRY(timespec_to_duration(ts, ims));  TEST_EQUAL(ims.count(), 86'400'125);
    ts = {86'400, 125'000'000};  TRY(timespec_to_duration(ts, is));   TEST_EQUAL(is.count(), 86'400);
    tv = {0, 0};                 TRY(timeval_to_duration(tv, fs));    TEST_EQUAL(fs.count(), 0);
    tv = {0, 0};                 TRY(timeval_to_duration(tv, id));    TEST_EQUAL(id.count(), 0);
    tv = {0, 0};                 TRY(timeval_to_duration(tv, ims));   TEST_EQUAL(ims.count(), 0);
    tv = {0, 0};                 TRY(timeval_to_duration(tv, is));    TEST_EQUAL(is.count(), 0);
    tv = {0, 125'000};           TRY(timeval_to_duration(tv, fs));    TEST_EQUAL(fs.count(), 0.125);
    tv = {0, 125'000};           TRY(timeval_to_duration(tv, id));    TEST_EQUAL(id.count(), 0);
    tv = {0, 125'000};           TRY(timeval_to_duration(tv, ims));   TEST_EQUAL(ims.count(), 125);
    tv = {0, 125'000};           TRY(timeval_to_duration(tv, is));    TEST_EQUAL(is.count(), 0);
    tv = {86'400, 0};            TRY(timeval_to_duration(tv, fs));    TEST_EQUAL(fs.count(), 86'400);
    tv = {86'400, 0};            TRY(timeval_to_duration(tv, id));    TEST_EQUAL(id.count(), 1);
    tv = {86'400, 0};            TRY(timeval_to_duration(tv, ims));   TEST_EQUAL(ims.count(), 86'400'000);
    tv = {86'400, 0};            TRY(timeval_to_duration(tv, is));    TEST_EQUAL(is.count(), 86'400);
    tv = {86'400, 125'000};      TRY(timeval_to_duration(tv, fs));    TEST_EQUAL(fs.count(), 86'400.125);
    tv = {86'400, 125'000};      TRY(timeval_to_duration(tv, id));    TEST_EQUAL(id.count(), 1);
    tv = {86'400, 125'000};      TRY(timeval_to_duration(tv, ims));   TEST_EQUAL(ims.count(), 86'400'125);
    tv = {86'400, 125'000};      TRY(timeval_to_duration(tv, is));    TEST_EQUAL(is.count(), 86'400);

    fs = Dseconds(0);           TRY(ts = duration_to_timespec(fs));   TEST_EQUAL(ts.tv_sec, 0);       TEST_EQUAL(ts.tv_nsec, 0);
    id = IntDays(0);            TRY(ts = duration_to_timespec(id));   TEST_EQUAL(ts.tv_sec, 0);       TEST_EQUAL(ts.tv_nsec, 0);
    ims = IntMsec(0);           TRY(ts = duration_to_timespec(ims));  TEST_EQUAL(ts.tv_sec, 0);       TEST_EQUAL(ts.tv_nsec, 0);
    is = IntSec(0);             TRY(ts = duration_to_timespec(is));   TEST_EQUAL(ts.tv_sec, 0);       TEST_EQUAL(ts.tv_nsec, 0);
    fs = Dseconds(0.125);       TRY(ts = duration_to_timespec(fs));   TEST_EQUAL(ts.tv_sec, 0);       TEST_EQUAL(ts.tv_nsec, 125'000'000);
    ims = IntMsec(125);         TRY(ts = duration_to_timespec(ims));  TEST_EQUAL(ts.tv_sec, 0);       TEST_EQUAL(ts.tv_nsec, 125'000'000);
    fs = Dseconds(86'400);      TRY(ts = duration_to_timespec(fs));   TEST_EQUAL(ts.tv_sec, 86'400);  TEST_EQUAL(ts.tv_nsec, 0);
    id = IntDays(1);            TRY(ts = duration_to_timespec(id));   TEST_EQUAL(ts.tv_sec, 86'400);  TEST_EQUAL(ts.tv_nsec, 0);
    ims = IntMsec(86'400'000);  TRY(ts = duration_to_timespec(ims));  TEST_EQUAL(ts.tv_sec, 86'400);  TEST_EQUAL(ts.tv_nsec, 0);
    is = IntSec(86'400);        TRY(ts = duration_to_timespec(is));   TEST_EQUAL(ts.tv_sec, 86'400);  TEST_EQUAL(ts.tv_nsec, 0);
    fs = Dseconds(86'400.125);  TRY(ts = duration_to_timespec(fs));   TEST_EQUAL(ts.tv_sec, 86'400);  TEST_EQUAL(ts.tv_nsec, 125'000'000);
    ims = IntMsec(86'400'125);  TRY(ts = duration_to_timespec(ims));  TEST_EQUAL(ts.tv_sec, 86'400);  TEST_EQUAL(ts.tv_nsec, 125'000'000);
    fs = Dseconds(0);           TRY(tv = duration_to_timeval(fs));    TEST_EQUAL(tv.tv_sec, 0);       TEST_EQUAL(tv.tv_usec, 0);
    id = IntDays(0);            TRY(tv = duration_to_timeval(id));    TEST_EQUAL(tv.tv_sec, 0);       TEST_EQUAL(tv.tv_usec, 0);
    ims = IntMsec(0);           TRY(tv = duration_to_timeval(ims));   TEST_EQUAL(tv.tv_sec, 0);       TEST_EQUAL(tv.tv_usec, 0);
    is = IntSec(0);             TRY(tv = duration_to_timeval(is));    TEST_EQUAL(tv.tv_sec, 0);       TEST_EQUAL(tv.tv_usec, 0);
    fs = Dseconds(0.125);       TRY(tv = duration_to_timeval(fs));    TEST_EQUAL(tv.tv_sec, 0);       TEST_EQUAL(tv.tv_usec, 125'000);
    ims = IntMsec(125);         TRY(tv = duration_to_timeval(ims));   TEST_EQUAL(tv.tv_sec, 0);       TEST_EQUAL(tv.tv_usec, 125'000);
    fs = Dseconds(86'400);      TRY(tv = duration_to_timeval(fs));    TEST_EQUAL(tv.tv_sec, 86'400);  TEST_EQUAL(tv.tv_usec, 0);
    id = IntDays(1);            TRY(tv = duration_to_timeval(id));    TEST_EQUAL(tv.tv_sec, 86'400);  TEST_EQUAL(tv.tv_usec, 0);
    ims = IntMsec(86'400'000);  TRY(tv = duration_to_timeval(ims));   TEST_EQUAL(tv.tv_sec, 86'400);  TEST_EQUAL(tv.tv_usec, 0);
    is = IntSec(86'400);        TRY(tv = duration_to_timeval(is));    TEST_EQUAL(tv.tv_sec, 86'400);  TEST_EQUAL(tv.tv_usec, 0);
    fs = Dseconds(86'400.125);  TRY(tv = duration_to_timeval(fs));    TEST_EQUAL(tv.tv_sec, 86'400);  TEST_EQUAL(tv.tv_usec, 125'000);
    ims = IntMsec(86'400'125);  TRY(tv = duration_to_timeval(ims));   TEST_EQUAL(tv.tv_sec, 86'400);  TEST_EQUAL(tv.tv_usec, 125'000);

    #ifdef _WIN32

        static constexpr int64_t epoch = 11'644'473'600ll;
        static constexpr int64_t freq = 10'000'000ll;

        int64_t n = 0;
        FILETIME ft, ft2;
        system_clock::time_point tp, tp2;
        system_clock::duration d;

        n = epoch * freq;
        ft = {uint32_t(n), uint32_t(n >> 32)};
        TRY(tp = filetime_to_timepoint(ft));
        d = tp - system_clock::from_time_t(0);
        TEST_EQUAL(d.count(), 0);
        TRY(ft2 = timepoint_to_filetime(tp));
        TEST_EQUAL(ft2.dwHighDateTime, ft.dwHighDateTime);
        TEST_EQUAL(ft2.dwLowDateTime, ft.dwLowDateTime);

        n += 86'400 * freq;
        ft = {uint32_t(n), uint32_t(n >> 32)};
        TRY(tp = filetime_to_timepoint(ft));
        d = tp - system_clock::from_time_t(0);
        TEST_EQUAL(duration_cast<IntMsec>(d).count(), 86'400'000);

        tp = system_clock::from_time_t(0);
        TRY(ft = timepoint_to_filetime(tp));
        TRY(tp2 = filetime_to_timepoint(ft));
        TEST_EQUAL(tp2.time_since_epoch().count(), tp.time_since_epoch().count());

        tp = system_clock::from_time_t(1'234'567'890);
        TRY(ft = timepoint_to_filetime(tp));
        TRY(tp2 = filetime_to_timepoint(ft));
        TEST_EQUAL(tp2.time_since_epoch().count(), tp.time_since_epoch().count());

    #endif

}
