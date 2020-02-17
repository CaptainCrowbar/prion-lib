#include "rs-core/time.hpp"
#include "rs-core/unit-test.hpp"
#include <chrono>
#include <cmath>
#include <ratio>
#include <stdexcept>
#include <thread>

#ifdef _XOPEN_SOURCE
    #include <sys/time.h>
#endif

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

namespace {

    class TestWaitFor:
    public Wait {
    public:
        TestWaitFor() = default;
    protected:
        virtual bool do_wait_for(duration t) final {
            std::this_thread::sleep_for(t);
            return true;
        }
    };

    class TestWaitUntil:
    public Wait {
    public:
        TestWaitUntil() = default;
    protected:
        virtual bool do_wait_until(time_point t) final {
            std::this_thread::sleep_until(t);
            return true;
        }
    };

    class TestPoll:
    public PollWait {
    public:
        TestPoll() = default;
        virtual bool poll() final { return ++count >= 14; }
        void reset() noexcept { count = 0; }
    private:
        int count = 0;
    };

}

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

void test_core_time_convert_time_point() {

    using Systp = system_clock::time_point;
    using Hrctp = high_resolution_clock::time_point;

    Systp sys1, sys2, sys3;
    Hrctp hrc1, hrc2, hrc3;
    milliseconds ms;

    sys1 = system_clock::now();
    hrc1 = high_resolution_clock::now();
    sys2 = sys1 + 1min;
    hrc2 = hrc1 + 1min;

    convert_time_point(sys2, sys3);          ms = duration_cast<milliseconds>(sys3 - sys1);  TEST_EQUAL(ms.count(), 60'000);
    convert_time_point(hrc2, hrc3);          ms = duration_cast<milliseconds>(hrc3 - hrc1);  TEST_EQUAL(ms.count(), 60'000);
    convert_time_point(hrc2, sys3);          ms = duration_cast<milliseconds>(sys3 - sys1);  TEST_NEAR_EPSILON(ms.count(), 60'000, 50);
    convert_time_point(sys2, hrc3);          ms = duration_cast<milliseconds>(hrc3 - hrc1);  TEST_NEAR_EPSILON(ms.count(), 60'000, 50);
    sys3 = convert_time_point<Systp>(sys2);  ms = duration_cast<milliseconds>(sys3 - sys1);  TEST_EQUAL(ms.count(), 60'000);
    hrc3 = convert_time_point<Hrctp>(hrc2);  ms = duration_cast<milliseconds>(hrc3 - hrc1);  TEST_EQUAL(ms.count(), 60'000);
    sys3 = convert_time_point<Systp>(hrc2);  ms = duration_cast<milliseconds>(sys3 - sys1);  TEST_NEAR_EPSILON(ms.count(), 60'000, 50);
    hrc3 = convert_time_point<Hrctp>(sys2);  ms = duration_cast<milliseconds>(hrc3 - hrc1);  TEST_NEAR_EPSILON(ms.count(), 60'000, 50);

}

void test_core_time_general_operations() {

    hours h = {};
    minutes m = {};
    seconds s = {};
    milliseconds ms = {};

    TRY(from_seconds(7200.0, h));   TEST_EQUAL(h.count(), 2);
    TRY(from_seconds(7200.0, m));   TEST_EQUAL(m.count(), 120);
    TRY(from_seconds(7200.0, s));   TEST_EQUAL(s.count(), 7200);
    TRY(from_seconds(7200.0, ms));  TEST_EQUAL(ms.count(), 7'200'000);

    TEST_EQUAL(to_seconds(hours(10)), 36000);
    TEST_EQUAL(to_seconds(minutes(10)), 600);
    TEST_EQUAL(to_seconds(seconds(10)), 10);
    TEST_NEAR(to_seconds(milliseconds(10)), 0.01);

}

void test_core_time_system_specific_conversions() {

    using IntMsec = duration<int64_t, std::ratio<1, 1000>>;
    using IntSec = duration<int64_t>;
    using IntDays = duration<int64_t, std::ratio<86400>>;

    IntMsec ims = {};
    IntSec is = {};
    IntDays id = {};
    Dseconds fs = {};
    timespec ts = {};
    timeval tv = {};

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
        FILETIME ft = {}, ft2 = {};
        system_clock::time_point tp = {}, tp2 = {};
        system_clock::duration d = {};

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

void test_core_time_date_format() {

    system_clock::time_point d;
    std::string s;

    TRY(d = make_date(2020, 2, 3, 4, 5, 6.7899));

    TRY(s = date_format(d, "d/m/yyyy"));                   TEST_EQUAL(s, "3/2/2020");
    TRY(s = date_format(d, "yyyy-mm-dd HH:MM:SS"));        TEST_EQUAL(s, "2020-02-03 04:05:06");
    TRY(s = date_format(d, "www d mmm yy"));               TEST_EQUAL(s, "mon 3 feb 20");
    TRY(s = date_format(d, "Www d Mmm yy"));               TEST_EQUAL(s, "Mon 3 Feb 20");
    TRY(s = date_format(d, "WWW d MMM yy"));               TEST_EQUAL(s, "MON 3 FEB 20");
    TRY(s = date_format(d, "H:MM:SS"));                    TEST_EQUAL(s, "4:05:06");
    TRY(s = date_format(d, "H:MM:SS.s"));                  TEST_EQUAL(s, "4:05:06.7");
    TRY(s = date_format(d, "H:MM:SS.ss"));                 TEST_EQUAL(s, "4:05:06.78");
    TRY(s = date_format(d, "H:MM:SS.sss"));                TEST_EQUAL(s, "4:05:06.789");
    TRY(s = date_format(d, "H:MM:SS.ssss"));               TEST_EQUAL(s, "4:05:06.7899");
    TRY(s = date_format(d, "H:MM:SS.sssss"));              TEST_EQUAL(s, "4:05:06.78990");
    TRY(s = date_format(d, "H:MM:SS.ssssss"));             TEST_EQUAL(s, "4:05:06.789900");
    TRY(s = date_format(d, "yyyy-mm-dd HH:MM:SS +ZZZZ"));  TEST_EQUAL(s, "2020-02-03 04:05:06 +0000");

    TRY(d = make_date(2020, 2, 3, 4, 5, 6.7899, local_zone));

    TRY(s = date_format(d, "yyyy-mm-dd HH:MM:SS.sss", local_zone));    TEST_EQUAL(s, "2020-02-03 04:05:06.789");
    TRY(s = date_format(d, "yyyy-mm-dd HH:MM:SS +ZZZZ", local_zone));  TEST_MATCH(s, "^2020-02-03 04:05:06 [+-]\\d{4}$");

}

void test_core_time_parse_date() {

    system_clock::time_point date = {};
    Ustring str;

    TRY(date = parse_date("2017-11-04"));                     TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 00:00:00.000");
    TRY(date = parse_date("2017 Nov 4"));                     TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 00:00:00.000");
    TRY(date = parse_date("2017-11-04 12"));                  TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 12:00:00.000");
    TRY(date = parse_date("2017-11-04 12:34"));               TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 12:34:00.000");
    TRY(date = parse_date("2017-11-04 12:34:56"));            TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 12:34:56.000");
    TRY(date = parse_date("2017-11-04 12:34:56.789"));        TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 12:34:56.789");
    TRY(date = parse_date("2017-11-04T12:34:56"));            TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 12:34:56.000");
    TRY(date = parse_date("4 Nov 2017", dmy_order));          TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 00:00:00.000");
    TRY(date = parse_date("Nov 4 2017", mdy_order));          TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 00:00:00.000");
    TRY(date = parse_date("4/11/2017/12/34/56", dmy_order));  TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 12:34:56.000");
    TRY(date = parse_date("11/4/2017/12/34/56", mdy_order));  TRY(str = format_date(date, 3));  TEST_EQUAL(str, "2017-11-04 12:34:56.000");

    TEST_THROW(parse_date(""), std::invalid_argument);
    TEST_THROW(parse_date("Nov 4, 2017"), std::invalid_argument);
    TEST_THROW(parse_date("Nov 2017", mdy_order), std::invalid_argument);

}

void test_core_time_parse_time() {

    Dseconds dsec = {};
    seconds sec = {};
    microseconds usec = {};

    TRY(dsec = parse_time<Dseconds>("0s"));                    TEST_EQUAL(dsec.count(), 0);
    TRY(dsec = parse_time<Dseconds>("42s"));                   TEST_EQUAL(dsec.count(), 42);
    TRY(dsec = parse_time<Dseconds>("4200s"));                 TEST_EQUAL(dsec.count(), 4200);
    TRY(dsec = parse_time<Dseconds>("1y234d5h6m7.89s"));       TEST_NEAR(dsec.count(), 51'793'567.89);
    TRY(dsec = parse_time<Dseconds>("-42s"));                  TEST_EQUAL(dsec.count(), -42);
    TRY(dsec = parse_time<Dseconds>("-4200s"));                TEST_EQUAL(dsec.count(), -4200);
    TRY(dsec = parse_time<Dseconds>("-1y234d5h6m7.89s"));      TEST_NEAR(dsec.count(), -51'793'567.89);
    TRY(sec = parse_time<seconds>("0s"));                      TEST_EQUAL(sec.count(), 0);
    TRY(sec = parse_time<seconds>("42s"));                     TEST_EQUAL(sec.count(), 42);
    TRY(sec = parse_time<seconds>("4200s"));                   TEST_EQUAL(sec.count(), 4200);
    TRY(sec = parse_time<seconds>("1y234d5h6m7.89s"));         TEST_EQUAL(sec.count(), 51'793'567);
    TRY(sec = parse_time<seconds>("-42s"));                    TEST_EQUAL(sec.count(), -42);
    TRY(sec = parse_time<seconds>("-4200s"));                  TEST_EQUAL(sec.count(), -4200);
    TRY(sec = parse_time<seconds>("-1y234d5h6m7.89s"));        TEST_EQUAL(sec.count(), -51'793'567);
    TRY(usec = parse_time<microseconds>("0s"));                TEST_EQUAL(usec.count(), 0);
    TRY(usec = parse_time<microseconds>("42s"));               TEST_EQUAL(usec.count(), 42'000'000);
    TRY(usec = parse_time<microseconds>("4200s"));             TEST_EQUAL(usec.count(), 4'200'000'000ll);
    TRY(usec = parse_time<microseconds>("1y234d5h6m7.89s"));   TEST_EQUAL(usec.count(), 51'793'567'890'000ll);
    TRY(usec = parse_time<microseconds>("-42s"));              TEST_EQUAL(usec.count(), -42'000'000);
    TRY(usec = parse_time<microseconds>("-4200s"));            TEST_EQUAL(usec.count(), -4'200'000'000ll);
    TRY(usec = parse_time<microseconds>("-1y234d5h6m7.89s"));  TEST_EQUAL(usec.count(), -51'793'567'890'000ll);
    TRY(usec = parse_time<microseconds>("42ms"));              TEST_EQUAL(usec.count(), 42'000);
    TRY(usec = parse_time<microseconds>("42us"));              TEST_EQUAL(usec.count(), 42);
    TRY(usec = parse_time<microseconds>("42ns"));              TEST_EQUAL(usec.count(), 0);
    TRY(sec = parse_time<seconds>("1y"));                      TEST_EQUAL(sec.count(), 31'557'600);
    TRY(sec = parse_time<seconds>("1ky"));                     TEST_EQUAL(sec.count(), 31'557'600'000ll);
    TRY(sec = parse_time<seconds>("1My"));                     TEST_EQUAL(sec.count(), 31'557'600'000'000ll);
    TRY(sec = parse_time<seconds>("1Gy"));                     TEST_EQUAL(sec.count(), 31'557'600'000'000'000ll);

    TEST_THROW(parse_time<seconds>(""), std::invalid_argument);
    TEST_THROW(parse_time<seconds>("12345"), std::invalid_argument);
    TEST_THROW(parse_time<seconds>("sec"), std::invalid_argument);

}

void test_core_time_wait_base() {

    TestWaitFor twf;
    TestWaitUntil twu;
    Timer<milliseconds> timer;

    timer.reset();
    TEST(twf.wait_for(100ms));
    TEST_NEAR_EPSILON(timer.get().count(), 100, 50);

    timer.reset();
    TEST(twu.wait_for(100ms));
    TEST_NEAR_EPSILON(timer.get().count(), 100, 50);

}

void test_core_time_poll_base() {

    TestPoll tp;
    Timer<milliseconds> timer;

    TEST_EQUAL(duration_cast<microseconds>(tp.min_interval()).count(), 10);
    TEST_EQUAL(duration_cast<microseconds>(tp.max_interval()).count(), 10'000);
    TRY(tp.set_interval(5us, 1s));
    TEST_EQUAL(duration_cast<microseconds>(tp.min_interval()).count(), 5);
    TEST_EQUAL(duration_cast<microseconds>(tp.max_interval()).count(), 1'000'000);
    timer.reset();
    TEST(tp.wait_for(1s));
    TEST_NEAR_EPSILON(timer.get().count(), 82, 50);

    tp.reset();
    timer.reset();
    TRY(tp.wait());
    TEST_NEAR_EPSILON(timer.get().count(), 82, 50);

}

void test_core_time_poll_condition() {

    bool flag = false;
    PollCondition pc([&] { return flag; });
    Timer<milliseconds> timer;

    timer.reset();
    TEST(! pc.wait_for(100ms));
    TEST_NEAR_EPSILON(timer.get().count(), 100, 50);

    flag = true;
    timer.reset();
    TEST(pc.wait_for(100ms));
    TEST_NEAR_EPSILON(timer.get().count(), 0, 50);

}

void test_core_time_timer() {

    Timer<milliseconds> timer;
    milliseconds t;

    std::this_thread::sleep_for(100ms);
    TRY(t = timer);
    TEST_NEAR_EPSILON(t.count(), 100, 50);
    std::this_thread::sleep_for(100ms);
    TRY(t = timer);
    TEST_NEAR_EPSILON(t.count(), 200, 50);
    TRY(timer.reset());
    std::this_thread::sleep_for(100ms);
    TRY(t = timer);
    TEST_NEAR_EPSILON(t.count(), 100, 50);

}
