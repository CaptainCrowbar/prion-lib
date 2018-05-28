#include "rs-core/range-generation.hpp"
#include "rs-core/range-selection.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <random>
#include <vector>

using namespace RS;
using namespace RS::Range;
using namespace std::literals;

namespace {

    template <typename T>
    void stats(const std::vector<T>& v, double& mean, double& sd) {
        double sum = 0, sum2 = 0;
        for (T t: v) {
            auto x = double(t);
            sum += x;
            sum2 += x * x;
        }
        mean = sum / v.size();
        sd = sqrt(sum2 / v.size() - mean * mean);
    }

}

void test_core_range_generation_epsilon() {

    Ustring s;

    TRY(epsilon<char>() >> overwrite(s));                 TEST_EQUAL(s, "");
    TRY(epsilon<char>() >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");

}

void test_core_range_generation_fill() {

    Ustring s;

    TRY(fill('x', 0) >> overwrite(s));  TEST_EQUAL(s, "");
    TRY(fill('x', 1) >> overwrite(s));  TEST_EQUAL(s, "x");
    TRY(fill('x', 2) >> overwrite(s));  TEST_EQUAL(s, "xx");
    TRY(fill('x', 3) >> overwrite(s));  TEST_EQUAL(s, "xxx");
    TRY(fill('x', 4) >> overwrite(s));  TEST_EQUAL(s, "xxxx");
    TRY(fill('x', 5) >> overwrite(s));  TEST_EQUAL(s, "xxxxx");

    TRY(fill('x', 0) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY(fill('x', 1) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "x");
    TRY(fill('x', 2) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xx");
    TRY(fill('x', 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xxx");
    TRY(fill('x', 4) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xxxx");
    TRY(fill('x', 5) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "xxxxx");

    s.resize(0);  TRY(s << fill('x'));  TEST_EQUAL(s, "");
    s.resize(1);  TRY(s << fill('x'));  TEST_EQUAL(s, "x");
    s.resize(2);  TRY(s << fill('x'));  TEST_EQUAL(s, "xx");
    s.resize(3);  TRY(s << fill('x'));  TEST_EQUAL(s, "xxx");
    s.resize(4);  TRY(s << fill('x'));  TEST_EQUAL(s, "xxxx");
    s.resize(5);  TRY(s << fill('x'));  TEST_EQUAL(s, "xxxxx");

}

void test_core_range_generation_generate() {

    Ustring s;
    int n;
    auto f = [&n] { return char('a' + n++); };

    n = 0;  TRY(generate(f, 0) >> overwrite(s));  TEST_EQUAL(s, "");
    n = 0;  TRY(generate(f, 1) >> overwrite(s));  TEST_EQUAL(s, "a");
    n = 0;  TRY(generate(f, 2) >> overwrite(s));  TEST_EQUAL(s, "ab");
    n = 0;  TRY(generate(f, 3) >> overwrite(s));  TEST_EQUAL(s, "abc");
    n = 0;  TRY(generate(f, 4) >> overwrite(s));  TEST_EQUAL(s, "abcd");
    n = 0;  TRY(generate(f, 5) >> overwrite(s));  TEST_EQUAL(s, "abcde");

    n = 0;  TRY(generate(f, 0) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    n = 0;  TRY(generate(f, 1) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "a");
    n = 0;  TRY(generate(f, 2) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "ab");
    n = 0;  TRY(generate(f, 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "abc");
    n = 0;  TRY(generate(f, 4) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "abcd");
    n = 0;  TRY(generate(f, 5) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "abcde");

    n = 0;  s.resize(0);  TRY(s << generate(f));  TEST_EQUAL(s, "");
    n = 0;  s.resize(1);  TRY(s << generate(f));  TEST_EQUAL(s, "a");
    n = 0;  s.resize(2);  TRY(s << generate(f));  TEST_EQUAL(s, "ab");
    n = 0;  s.resize(3);  TRY(s << generate(f));  TEST_EQUAL(s, "abc");
    n = 0;  s.resize(4);  TRY(s << generate(f));  TEST_EQUAL(s, "abcd");
    n = 0;  s.resize(5);  TRY(s << generate(f));  TEST_EQUAL(s, "abcde");

}

void test_core_range_generation_iota() {

    Ustring s;
    auto f = [] (char c) { return ascii_isupper(c) ? ascii_tolower(c + 1) : ascii_toupper(c + 1); };

    TRY(iota('a') >> take(10) >> overwrite(s));   TEST_EQUAL(s, "abcdefghij");
    TRY(iota('a', char(2), 10) >> overwrite(s));  TEST_EQUAL(s, "acegikmoqs");
    TRY(iota('a', f, 10) >> overwrite(s));        TEST_EQUAL(s, "aBcDeFgHiJ");

    TRY(iota('a') >> passthrough >> take(10) >> overwrite(s));   TEST_EQUAL(s, "abcdefghij");
    TRY(iota('a', char(2), 10) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "acegikmoqs");
    TRY(iota('a', f, 10) >> passthrough >> overwrite(s));        TEST_EQUAL(s, "aBcDeFgHiJ");

    s.resize(10);

    TRY(s << iota('a'));           TEST_EQUAL(s, "abcdefghij");
    TRY(s << iota('a', char(2)));  TEST_EQUAL(s, "acegikmoqs");
    TRY(s << iota('a', f));        TEST_EQUAL(s, "aBcDeFgHiJ");

}

void test_core_range_generation_random() {

    static constexpr size_t n = 100000;

    std::vector<int> v;
    std::uniform_int_distribution<int> uid(0, 100);
    std::mt19937 rng(42);
    double mean = 0, sd = 0;

    TRY(random(uid, rng, 0) >> overwrite(v));
    TEST_EQUAL(v.size(), 0);

    TRY(random(uid, rng, n) >> overwrite(v));
    TEST_EQUAL(v.size(), n);
    TRY(stats(v, mean, sd));
    TEST_NEAR_EPSILON(mean, 50, 0.1);
    TEST_NEAR_EPSILON(sd, 29.15, 0.1);

    TRY(random(uid, rng, n) >> passthrough >> overwrite(v));
    TEST_EQUAL(v.size(), n);
    TRY(stats(v, mean, sd));
    TEST_NEAR_EPSILON(mean, 50, 0.1);
    TEST_NEAR_EPSILON(sd, 29.15, 0.1);

    v.clear();
    v.resize(n, 0);
    TRY(v << random(uid, rng, n));
    TEST_EQUAL(v.size(), n);
    TRY(stats(v, mean, sd));
    TEST_NEAR_EPSILON(mean, 50, 0.1);
    TEST_NEAR_EPSILON(sd, 29.15, 0.1);

}

void test_core_range_generation_single() {

    Ustring s;

    TRY(single('x') >> overwrite(s));                 TEST_EQUAL(s, "x");
    TRY(single('x') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "x");

}
