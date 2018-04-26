#include "rs-core/channel.hpp"
#include "rs-core/optional.hpp"
#include "rs-core/string.hpp"
#include "rs-core/time.hpp"
#include "rs-core/unit-test.hpp"
#include <atomic>
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
            open = false;
        }
        virtual bool is_closed() const noexcept {
            return ! open;
        }
        virtual bool poll() {
            auto lock = make_lock(mutex);
            return ! open || ! queue.empty();
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
        virtual bool do_wait_for(time_unit t) {
            return polled_wait(t);
        }
    private:
        std::mutex mutex;
        std::deque<int> queue;
        std::atomic<bool> open{true};
    };

    class TestException:
    public std::runtime_error {
    public:
        TestException(): std::runtime_error("Test exception") {}
    };

}

void test_core_channel_true() {

    TrueChannel chan;

    TEST(chan.wait_for(10ms));
    TEST(chan.wait_for(10ms));
    TEST(chan.wait_for(10ms));
    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());

}

void test_core_channel_false() {

    FalseChannel chan;

    TEST(! chan.wait_for(10ms));
    TEST(! chan.wait_for(10ms));
    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());

}

void test_core_channel_generator() {

    int i = 0, j = 0;
    GeneratorChannel<int> chan([&i] { return ++i; });
    Optional<int> oi;

    TEST(chan.wait_for(10ms));
    TEST(chan.read(j));
    TEST_EQUAL(j, 1);

    TEST(chan.wait_for(10ms));
    TEST(chan.read(j));
    TEST_EQUAL(j, 2);

    TEST(chan.wait_for(10ms));
    TRY(oi = chan.read_opt());
    TEST(oi);
    TEST_EQUAL(*oi, 3);

    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());
    TEST(! chan.read(j));
    TRY(oi = chan.read_opt());
    TEST(! oi);

}

void test_core_channel_buffer() {

    BufferChannel chan;
    Ustring s;
    size_t n = 0;

    TEST(! chan.wait_for(10ms));

    TEST(chan.write("Hello"));
    TEST(chan.write("World"));
    TEST(chan.wait_for(10ms));
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 10);
    TEST_EQUAL(s, "HelloWorld");
    TEST(! chan.wait_for(10ms));

    s.clear();
    TEST(chan.write("Hello"));
    TEST(chan.wait_for(10ms));
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 5);
    TEST_EQUAL(s, "Hello");
    TEST(! chan.wait_for(10ms));

    s.clear();
    TEST(chan.write("Hello"));
    TEST(chan.wait_for(10ms));
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 5);
    TEST_EQUAL(s, "Hello");
    TEST(! chan.wait_for(10ms));
    TEST(chan.write("World"));
    TEST(chan.wait_for(10ms));
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 5);
    TEST_EQUAL(s, "HelloWorld");
    TEST(! chan.wait_for(10ms));

    s.clear();
    TEST(! chan.wait_for(10ms));
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 0);
    TEST_EQUAL(s, "");

    s.clear();
    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());
    TRY(n = chan.read_to(s));
    TEST_EQUAL(n, 0);
    TEST_EQUAL(s, "");

}

void test_core_channel_queue() {

    QueueChannel<Ustring> chan;
    Ustring s;

    TEST(! chan.wait_for(10ms));

    TRY(chan.write("Hello"));
    TEST(chan.wait_for(10ms));
    TEST(chan.read(s));
    TEST_EQUAL(s, "Hello");
    TEST(! chan.wait_for(10ms));

    TRY(chan.write("Hello"));
    TRY(chan.write("World"));
    TEST(chan.wait_for(10ms));
    TEST(chan.read(s));
    TEST_EQUAL(s, "Hello");
    TEST(chan.wait_for(10ms));
    TEST(chan.read(s));
    TEST_EQUAL(s, "World");
    TEST(! chan.wait_for(10ms));

    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());

}

void test_core_channel_value() {

    ValueChannel<Ustring> chan;
    Ustring s;

    TEST(! chan.wait_for(10ms));

    TRY(chan.write("Hello"));
    TEST(chan.wait_for(10ms));
    TEST(chan.read(s));
    TEST_EQUAL(s, "Hello");
    TEST(! chan.wait_for(10ms));

    TRY(chan.write("Hello"));
    TRY(chan.write("World"));
    TEST(chan.wait_for(10ms));
    TEST(chan.read(s));
    TEST_EQUAL(s, "World");
    TEST(! chan.wait_for(10ms));

    TRY(chan.write("World"));
    TEST(! chan.wait_for(10ms));

    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());

}

void test_core_channel_timer() {

    TimerChannel chan(25ms);

    TEST(! chan.wait_for(1ms));
    TEST(chan.wait_for(100ms));

    TRY(std::this_thread::sleep_for(150ms));
    TEST(chan.wait_for(1ms));
    TEST(chan.wait_for(1ms));

    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());

}

void test_core_channel_throttle() {

    ThrottleChannel chan(25ms);

    TEST(chan.wait_for(1ms));
    TEST(! chan.wait_for(1ms));

    TRY(std::this_thread::sleep_for(150ms));
    TEST(chan.wait_for(1ms));
    TEST(! chan.wait_for(1ms));

    auto t1 = ReliableClock::now();
    for (int i = 0; i < 10; ++i)
        TEST(chan.wait_for(250ms));
    auto t2 = ReliableClock::now();
    auto ms = duration_cast<milliseconds>(t2 - t1);
    TEST_NEAR_EPSILON(ms.count(), 250, 50);

    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());

}

void test_core_channel_polled() {

    TestPoll chan;
    int i = 0;

    TEST(! chan.poll());
    TEST(! chan.wait_for(10ms));

    TRY(chan.write(1));
    TRY(chan.write(2));
    TRY(chan.write(3));
    TEST(chan.wait_for(10ms));
    TEST(chan.read(i));
    TEST_EQUAL(i, 1);
    TEST(chan.wait_for(10ms));
    TEST(chan.read(i));
    TEST_EQUAL(i, 2);
    TEST(chan.wait_for(10ms));
    TEST(chan.read(i));
    TEST_EQUAL(i, 3);
    TEST(! chan.wait_for(10ms));

    TRY(chan.close());
    TEST(chan.wait_for(10ms));
    TEST(chan.is_closed());

}

void test_core_channel_dispatch() {

    static constexpr int cycles = 100;
    static constexpr auto time_interval = 1ms;

    Dispatch::result_type rc;

    {
        // Stopwatch w("Run with no channels");
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::empty);
        TEST_EQUAL(rc.channel, nullptr);
        TEST(! rc.error);
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Terminate by closing from callback (sync)");
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(Dispatch::add(chan, Dispatch::mode::sync, [&] { ++n; if (n == cycles) chan.close(); }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TEST(chan.wait_for(1ms));
        TEST(chan.is_closed());
        TEST_EQUAL(n, cycles);
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Terminate by closing from callback (async)");
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(Dispatch::add(chan, Dispatch::mode::async, [&] { ++n; if (n == cycles) chan.close(); }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TEST(chan.wait_for(1ms));
        TEST(chan.is_closed());
        TEST_EQUAL(n, cycles);
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Terminate by exception (sync)");
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(Dispatch::add(chan, Dispatch::mode::sync, [&] { ++n; if (n == cycles) throw TestException(); }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::error);
        TEST_EQUAL(rc.channel, &chan);
        TEST_THROW(rethrow(rc.error), TestException);
        TRY(chan.wait_for(1ms));
        TEST_EQUAL(n, cycles);
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Terminate by exception (async)");
        TimerChannel chan(time_interval);
        int n = 0;
        TRY(Dispatch::add(chan, Dispatch::mode::async, [&] { ++n; if (n == cycles) throw TestException(); }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::error);
        TEST_EQUAL(rc.channel, &chan);
        TEST_THROW(rethrow(rc.error), TestException);
        TRY(chan.wait_for(1ms));
        TEST_EQUAL(n, cycles);
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Two sync channels, terminate one by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            TRY(Dispatch::add(chan1, Dispatch::mode::sync, [&] { ++n1; if (n1 == cycles) throw TestException(); }));
            TRY(Dispatch::add(chan2, Dispatch::mode::sync, [&] { ++n2; }));
            TRY(rc = Dispatch::run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan1);
            TEST_THROW(rethrow(rc.error), TestException);
            TEST(chan1.wait_for(1ms));
            TEST_EQUAL(n1, cycles);
            TRY(Dispatch::stop());
        }
        TEST(chan2.wait_for(1ms));
        TEST(chan2.is_closed());
    }

    {
        // Stopwatch w("Two async channels, terminate one by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            TRY(Dispatch::add(chan1, Dispatch::mode::async, [&] { ++n1; if (n1 == cycles) throw TestException(); }));
            TRY(Dispatch::add(chan2, Dispatch::mode::async, [&] { ++n2; }));
            TRY(rc = Dispatch::run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan1);
            TEST_THROW(rethrow(rc.error), TestException);
            TEST(chan1.wait_for(1ms));
            TEST_EQUAL(n1, cycles);
            TRY(Dispatch::stop());
        }
        TEST(chan2.wait_for(1ms));
        TEST(chan2.is_closed());
    }

    {
        // Stopwatch w("Two different channels, terminate sync by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            TRY(Dispatch::add(chan1, Dispatch::mode::sync, [&] { ++n1; if (n1 == cycles) throw TestException(); }));
            TRY(Dispatch::add(chan2, Dispatch::mode::async, [&] { ++n2; }));
            TRY(rc = Dispatch::run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan1);
            TEST_THROW(rethrow(rc.error), TestException);
            TEST(chan1.wait_for(1ms));
            TEST_EQUAL(n1, cycles);
            TRY(Dispatch::stop());
        }
        TEST(chan2.wait_for(1ms));
        TEST(chan2.is_closed());
    }

    {
        // Stopwatch w("Two different channels, terminate async by exception");
        TimerChannel chan1(time_interval);
        TimerChannel chan2(time_interval);
        int n1 = 0, n2 = 0;
        {
            TRY(Dispatch::add(chan1, Dispatch::mode::sync, [&] { ++n1; }));
            TRY(Dispatch::add(chan2, Dispatch::mode::async, [&] { ++n2; if (n2 == cycles) throw TestException(); }));
            TRY(rc = Dispatch::run());
            TEST_EQUAL(rc.why(), Dispatch::reason::error);
            TEST_EQUAL(rc.channel, &chan2);
            TEST_THROW(rethrow(rc.error), TestException);
            TRY(chan2.wait_for(1ms));
            TEST_EQUAL(n2, cycles);
            TRY(Dispatch::stop());
        }
        TEST(chan1.wait_for(1ms));
        TEST(chan1.is_closed());
    }

    {
        // Stopwatch w("Test with message channel (sync)");
        QueueChannel<int> chan;
        Ustring s;
        std::vector<int> v;
        for (int i = 1; i <= 10; ++i)
            TRY(chan.write(i));
        TRY(Dispatch::add(chan, Dispatch::mode::sync, [&] (int i) {
            v.push_back(i);
            if (i >= 5)
                chan.close();
        }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TEST(chan.wait_for(1ms));
        TEST(chan.is_closed());
        s = to_str(v);
        TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Test with message channel (async)");
        QueueChannel<int> chan;
        Ustring s;
        std::vector<int> v;
        for (int i = 1; i <= 10; ++i)
            TRY(chan.write(i));
        TRY(Dispatch::add(chan, Dispatch::mode::async, [&] (int i) {
            v.push_back(i);
            if (i >= 5)
                chan.close();
        }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TEST(chan.wait_for(1ms));
        TEST(chan.is_closed());
        s = to_str(v);
        TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Test with stream channel (sync)");
        BufferChannel chan;
        Ustring s;
        TRY(chan.write("Hello world\n"));
        TRY(chan.set_buffer(5));
        TRY(Dispatch::add(chan, Dispatch::mode::sync, [&] (std::string& t) {
            s += t;
            t.clear();
            if (s.find('\n') != npos)
                chan.close();
        }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TEST(chan.wait_for(1ms));
        TEST(chan.is_closed());
        TEST_EQUAL(s, "Hello world\n");
        TRY(Dispatch::stop());
    }

    {
        // Stopwatch w("Test with stream channel (async)");
        BufferChannel chan;
        Ustring s;
        TRY(chan.write("Hello world\n"));
        TRY(chan.set_buffer(5));
        TRY(Dispatch::add(chan, Dispatch::mode::async, [&] (std::string& t) {
            s += t;
            t.clear();
            if (s.find('\n') != npos)
                chan.close();
        }));
        TRY(rc = Dispatch::run());
        TEST_EQUAL(rc.why(), Dispatch::reason::closed);
        TEST_EQUAL(rc.channel, &chan);
        TEST(! rc.error);
        TEST(chan.wait_for(1ms));
        TEST(chan.is_closed());
        TEST_EQUAL(s, "Hello world\n");
        TRY(Dispatch::stop());
    }

}
