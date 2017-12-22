#include "rs-core/channel.hpp"
#include "rs-core/optional.hpp"
#include "rs-core/string.hpp"
#include "rs-core/time.hpp"
#include "rs-core/unit-test.hpp"
#include <chrono>
#include <deque>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

namespace {

    class TestPoll:
    public MessageChannel<int>,
    public Polled {
    public:
        virtual void close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
        }
        virtual state poll() {
            auto lock = make_lock(mutex);
            if (! open)
                return state::closed;
            else if (queue.empty())
                return state::waiting;
            else
                return state::ready;
        }
        virtual bool read(int& t) {
            auto lock = make_lock(mutex);
            if (! open || queue.empty())
                return false;
            t = queue.front();
            queue.pop_front();
            return true;
        }
        void write(int n) {
            auto lock = make_lock(mutex);
            queue.push_back(n);
        }
    protected:
        virtual state do_wait_for(IntervalBase::time_unit t) {
            return polled_wait(t);
        }
    private:
        std::mutex mutex;
        std::deque<int> queue;
        bool open = true;
    };

    class TestException:
    public std::runtime_error {
    public:
        TestException(): std::runtime_error("Test exception") {}
    };

}

void test_core_channel_true() {

    TrueChannel chan;
    Channel::state cs = Channel::state::closed;

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);

}

void test_core_channel_false() {

    FalseChannel chan;
    Channel::state cs = Channel::state::closed;

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);
    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);

}

void test_core_channel_generator() {

    int i = 0, j = 0;
    GeneratorChannel<int> chan([&i] { return ++i; });
    Channel::state cs = Channel::state::closed;
    Optional<int> oi;

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(j));
    TEST_EQUAL(j, 1);

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(j));
    TEST_EQUAL(j, 2);

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(oi = chan.read_opt());
    TEST(oi);
    TEST_EQUAL(*oi, 3);

    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);
    TEST(! chan.read(j));
    TRY(oi = chan.read_opt());
    TEST(! oi);

}

void test_core_channel_buffer() {

    BufferChannel chan;
    Channel::state cs = Channel::state::closed;
    U8string s;
    size_t n = 0;

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TEST(chan.write_str("Hello"));
    TEST(chan.write_str("World"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 10);
    TEST_EQUAL(s, "HelloWorld");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    s.clear();
    TEST(chan.write_str("Hello"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 5);
    TEST_EQUAL(s, "Hello");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    s.clear();
    TEST(chan.write_str("Hello"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 5);
    TEST_EQUAL(s, "Hello");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);
    TEST(chan.write_str("World"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 5);
    TEST_EQUAL(s, "HelloWorld");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    s.clear();
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 0);
    TEST_EQUAL(s, "");

    s.clear();
    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 0);
    TEST_EQUAL(s, "");

}

void test_core_channel_queue() {

    QueueChannel<U8string> chan;
    Channel::state cs = Channel::state::closed;
    U8string s;

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.write("Hello"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(s));
    TEST_EQUAL(s, "Hello");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.write("Hello"));
    TRY(chan.write("World"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(s));
    TEST_EQUAL(s, "Hello");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(s));
    TEST_EQUAL(s, "World");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);

}

void test_core_channel_value() {

    ValueChannel<U8string> chan;
    Channel::state cs = Channel::state::closed;
    U8string s;

    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.write("Hello"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(s));
    TEST_EQUAL(s, "Hello");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.write("Hello"));
    TRY(chan.write("World"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(s));
    TEST_EQUAL(s, "World");
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.write("World"));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);

}

void test_core_channel_timer() {

    TimerChannel chan(25ms);
    Channel::state cs = Channel::state::closed;

    TRY(cs = chan.wait_for(1ms));
    TEST_EQUAL(cs, Channel::state::waiting);
    TRY(cs = chan.wait_for(100ms));
    TEST_EQUAL(cs, Channel::state::ready);

    TRY(std::this_thread::sleep_for(150ms));
    TRY(cs = chan.wait_for(1ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(cs = chan.wait_for(1ms));
    TEST_EQUAL(cs, Channel::state::ready);

    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);

}

void test_core_channel_throttle() {

    ThrottleChannel chan(25ms);
    Channel::state cs = Channel::state::closed;

    TRY(cs = chan.wait_for(1ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(cs = chan.wait_for(1ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(std::this_thread::sleep_for(150ms));
    TRY(cs = chan.wait_for(1ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(cs = chan.wait_for(1ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    auto t1 = ReliableClock::now();
    for (int i = 0; i < 10; ++i) {
        TRY(cs = chan.wait_for(250ms));
        TEST_EQUAL(cs, Channel::state::ready);
    }
    auto t2 = ReliableClock::now();
    auto ms = duration_cast<milliseconds>(t2 - t1);
    TEST_NEAR_EPSILON(ms.count(), 250, 50);

    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);

}

void test_core_channel_polled() {

    TestPoll chan;
    Channel::state cs = Channel::state::closed;
    int i = 0;

    TRY(cs = chan.poll());
    TEST_EQUAL(cs, Channel::state::waiting);
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.write(1));
    TRY(chan.write(2));
    TRY(chan.write(3));
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(i));
    TEST_EQUAL(i, 1);
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(i));
    TEST_EQUAL(i, 2);
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TEST(chan.read(i));
    TEST_EQUAL(i, 3);
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::waiting);

    TRY(chan.close());
    TRY(cs = chan.wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);

}

void test_core_channel_dispatcher() {

    static constexpr int cycles = 100;
    static constexpr auto time_interval = 1ms;

    auto cs = Channel::state::closed;
    Dispatch::result_type rc;

    {
        // Stopwatch w("Run with no channels");
        Dispatch disp;
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::empty);
        TEST_EQUAL(rc.channel, nullptr);
        TEST(! rc.error);
    }

    {
        // Stopwatch w("Terminate by closing from callback (sync)");
        Dispatch disp;
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(disp.add(chan, Dispatch::mode::sync, [&] { ++n; if (n == cycles) chan.close(); }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
        TEST_EQUAL(n, cycles);
    }

    {
        // Stopwatch w("Terminate by closing from callback (async)");
        Dispatch disp;
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(disp.add(chan, Dispatch::mode::async, [&] { ++n; if (n == cycles) chan.close(); }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
        TEST_EQUAL(n, cycles);
    }

    {
        // Stopwatch w("Terminate by exception (sync)");
        Dispatch disp;
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(disp.add(chan, Dispatch::mode::sync, [&] { ++n; if (n == cycles) throw TestException(); }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::error);
        TEST_EQUAL(rc.channel, &chan);
        TEST_THROW(rethrow(rc.error), TestException);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(n, cycles);
    }

    {
        // Stopwatch w("Terminate by exception (async)");
        Dispatch disp;
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(disp.add(chan, Dispatch::mode::async, [&] { ++n; if (n == cycles) throw TestException(); }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::error);
        TEST_EQUAL(rc.channel, &chan);
        TEST_THROW(rethrow(rc.error), TestException);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(n, cycles);
    }

    {
        // Stopwatch w("Two sync channels, terminate one by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            Dispatch disp;
            TRY(disp.add(chan1, Dispatch::mode::sync, [&] { ++n1; if (n1 == cycles) throw TestException(); }));
            TRY(disp.add(chan2, Dispatch::mode::sync, [&] { ++n2; }));
            TRY(rc = disp.run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan1);
            TEST_THROW(rethrow(rc.error), TestException);
            TRY(cs = chan1.wait_for(1ms));
            TEST_EQUAL(n1, cycles);
        }
        TRY(cs = chan2.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
    }

    {
        // Stopwatch w("Two async channels, terminate one by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            Dispatch disp;
            TRY(disp.add(chan1, Dispatch::mode::async, [&] { ++n1; if (n1 == cycles) throw TestException(); }));
            TRY(disp.add(chan2, Dispatch::mode::async, [&] { ++n2; }));
            TRY(rc = disp.run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan1);
            TEST_THROW(rethrow(rc.error), TestException);
            TRY(cs = chan1.wait_for(1ms));
            TEST_EQUAL(n1, cycles);
        }
        TRY(cs = chan2.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
    }

    {
        // Stopwatch w("Two different channels, terminate sync by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            Dispatch disp;
            TRY(disp.add(chan1, Dispatch::mode::sync, [&] { ++n1; if (n1 == cycles) throw TestException(); }));
            TRY(disp.add(chan2, Dispatch::mode::async, [&] { ++n2; }));
            TRY(rc = disp.run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan1);
            TEST_THROW(rethrow(rc.error), TestException);
            TRY(cs = chan1.wait_for(1ms));
            TEST_EQUAL(n1, cycles);
        }
        TRY(cs = chan2.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
    }

    {
        // Stopwatch w("Two different channels, terminate async by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            Dispatch disp;
            TRY(disp.add(chan1, Dispatch::mode::sync, [&] { ++n1; }));
            TRY(disp.add(chan2, Dispatch::mode::async, [&] { ++n2; if (n2 == cycles) throw TestException(); }));
            TRY(rc = disp.run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan2);
            TEST_THROW(rethrow(rc.error), TestException);
            TRY(cs = chan2.wait_for(1ms));
            TEST_EQUAL(n2, cycles);
        }
        TRY(cs = chan1.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
    }

    {
        // Stopwatch w("Test with message channel (sync)");
        Dispatch disp;
        QueueChannel<int> chan;
        U8string s;
        std::vector<int> v;
        for (int i = 1; i <= 10; ++i)
            TRY(chan.write(i));
        TRY(disp.add(chan, Dispatch::mode::sync, [&] (int i) {
            v.push_back(i);
            if (i >= 5)
                chan.close();
        }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
        s = to_str(v);
        TEST_EQUAL(s, "[1,2,3,4,5]");
    }

    {
        // Stopwatch w("Test with message channel (async)");
        Dispatch disp;
        QueueChannel<int> chan;
        U8string s;
        std::vector<int> v;
        for (int i = 1; i <= 10; ++i)
            TRY(chan.write(i));
        TRY(disp.add(chan, Dispatch::mode::async, [&] (int i) {
            v.push_back(i);
            if (i >= 5)
                chan.close();
        }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
        s = to_str(v);
        TEST_EQUAL(s, "[1,2,3,4,5]");
    }

    {
        // Stopwatch w("Test with stream channel (sync)");
        Dispatch disp;
        BufferChannel chan;
        U8string s;
        TRY(chan.write_str("Hello world\n"));
        TRY(chan.set_buffer(5));
        TRY(disp.add(chan, Dispatch::mode::sync, [&] (std::string& t) {
            s += t;
            t.clear();
            if (s.find('\n') != npos)
                chan.close();
        }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
        TEST_EQUAL(s, "Hello world\n");
    }

    {
        // Stopwatch w("Test with stream channel (async)");
        Dispatch disp;
        BufferChannel chan;
        U8string s;
        TRY(chan.write_str("Hello world\n"));
        TRY(chan.set_buffer(5));
        TRY(disp.add(chan, Dispatch::mode::async, [&] (std::string& t) {
            s += t;
            t.clear();
            if (s.find('\n') != npos)
                chan.close();
        }));
        TRY(rc = disp.run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TRY(cs = chan.wait_for(1ms));
        TEST_EQUAL(cs, Channel::state::closed);
        TEST_EQUAL(s, "Hello world\n");
    }

}
