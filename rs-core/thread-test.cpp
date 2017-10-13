#include "rs-core/thread.hpp"
#include "rs-core/time.hpp"
#include "rs-core/unit-test.hpp"
#include <chrono>
#include <stdexcept>

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

void test_core_thread_class() {

    TEST_COMPARE(Thread::cpu_threads(), >=, 1);
    TRY(Thread::yield());

    {
        Thread t;
        TRY(t.wait());
        TEST(t.poll());
    }

    {
        Thread t(nullptr);
        TRY(t.wait());
        TEST(t.poll());
    }

    {
        U8string s;
        Thread t;
        TRY(t = Thread([&] { s = "Neddie"; }));
        TEST_COMPARE(t.get_id(), !=, Thread::current());
        TRY(t.wait());
        TEST(t.poll());
        TEST_EQUAL(s, "Neddie");
    }

    {
        Thread t;
        TRY(t = Thread([&] { throw std::runtime_error("Hello"); }));
        TEST_THROW_MATCH(t.wait(), std::runtime_error, "Hello");
        TRY(t.wait());
    }

}

void test_core_thread_synchronisation_objects() {

    {
        Mutex m;
        ConditionVariable cv;
        int n = 0;
        MutexLock lock;
        TRY(lock = make_lock(m));
        TEST(! cv.wait_for(lock, milliseconds(50), [&] { return n > 0; }));
        n = 42;
        TEST(cv.wait_for(lock, milliseconds(50), [&] { return n > 0; }));
    }

    {
        Mutex m;
        ConditionVariable cv;
        U8string s;
        auto f = [&] {
            MutexLock lock;
            TRY(lock = make_lock(m));
            TRY(cv.wait(lock, [&] { return ! s.empty(); }));
            s += "Seagoon";
        };
        Thread t(f);
        TEST(! t.poll());
        s = "Neddie";
        TRY(cv.notify_all());
        TRY(t.wait());
        TEST(t.poll());
        TEST_EQUAL(s, "NeddieSeagoon");
    }

    {
        Mutex m;
        ConditionVariable cv;
        U8string s;
        auto f1 = [&] {
            MutexLock lock;
            TRY(lock = make_lock(m));
            TRY(cv.wait(lock));
            s += "Seagoon";
        };
        auto f2 = [&] {
            TRY(sleep_for(0.05));
            MutexLock lock;
            TRY(lock = make_lock(m));
            s += "Neddie";
            TRY(cv.notify_all());
        };
        Thread t1(f1);
        Thread t2(f2);
        TRY(t1.wait());
        TRY(t2.wait());
        TEST_EQUAL(s, "NeddieSeagoon");
    }

    {
        Mutex m;
        ConditionVariable cv;
        U8string s;
        auto f1 = [&] {
            MutexLock lock;
            TRY(lock = make_lock(m));
            TRY(cv.wait(lock, [&] { return ! s.empty(); }));
            s += "Seagoon";
        };
        auto f2 = [&] {
            TRY(cv.notify_one());
            TRY(sleep_for(0.05));
            TRY(cv.notify_one());
            {
                MutexLock lock;
                TRY(lock = make_lock(m));
                s += "Neddie";
            }
            TRY(cv.notify_one());
        };
        Thread t1(f1);
        Thread t2(f2);
        TRY(t1.wait());
        TRY(t2.wait());
        TEST_EQUAL(s, "NeddieSeagoon");
    }

}
