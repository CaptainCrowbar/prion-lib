#include "rs-core/thread-pool.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <mutex>
#include <ostream>
#include <random>
#include <thread>

using namespace RS;
using namespace std::chrono;

void test_core_thread_pool_class() {

    ThreadPool pool;
    Ustring expect, log;
    std::mutex mutex;
    std::mt19937 rng(42);
    std::uniform_int_distribution<int> dist(1, 100);

    TEST_EQUAL(pool.size(), std::thread::hardware_concurrency());
    TEST_EQUAL(pool.pending(), 0);

    auto f = [&] (char c, int t) {
        std::this_thread::sleep_for(milliseconds(t));
        auto lock = make_lock(mutex);
        log += c;
    };

    for (char c = 'a'; c <= 'z'; ++c) {
        expect += c;
        auto t = dist(rng);
        TRY(pool.insert([=] { f(c, t); }));
    }

    TEST(pool.wait_for(5s));
    TEST_EQUAL(pool.pending(), 0);
    TEST_EQUAL(log.size(), expect.size());
    TEST_COMPARE(log, !=, expect);
    std::sort(log.begin(), log.end());
    TEST_EQUAL(log, expect);

    TRY(pool.clear());
    TEST_EQUAL(pool.size(), std::thread::hardware_concurrency());
    TEST_EQUAL(pool.pending(), 0);
    log.clear();

    for (char c = 'a'; c <= 'z'; ++c) {
        auto t = dist(rng);
        TRY(pool.insert([=] { f(c, t); }));
    }

    TEST(pool.wait_for(5s));
    TEST_EQUAL(pool.pending(), 0);
    TEST_EQUAL(log.size(), expect.size());
    TEST_COMPARE(log, !=, expect);
    std::sort(log.begin(), log.end());
    TEST_EQUAL(log, expect);

}

void test_core_thread_pool_timing() {

    static constexpr int iterations = 100'000;

    ThreadPool pool;
    auto task = [&] {};

    auto start = system_clock::now();
    for (int i = 0; i < iterations; ++i)
        TRY(pool.insert(task));
    TEST(pool.wait_for(5s));
    auto stop = system_clock::now();

    double total = double(duration_cast<nanoseconds>(stop - start).count());
    double each = total / double(iterations);
    double rate = 1e9 / each;
    (void)rate;
    // std::cout << "... Calls per second = " << fp_format(rate, 'f', 0) << std::endl;

}
