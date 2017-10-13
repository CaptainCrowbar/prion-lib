#include "rs-core/kwargs.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;
using namespace std::literals;

namespace {

    constexpr Kwarg<int> kw_alpha;
    constexpr Kwarg<bool> kw_bravo;
    constexpr Kwarg<U8string> kw_charlie;

    struct Kwtest {
        int a = 0;
        bool b = false;
        U8string c;
        template <typename... Args> int fun(const Args&... args) {
            auto i = int(kwget(kw_alpha, a, args...));
            auto j = int(kwget(kw_bravo, b, args...));
            auto k = int(kwget(kw_charlie, c, args...));
            return (i << 2) + (j << 1) + k;
        }
    };

}

void test_core_kwargs_keyword_arguments() {

    Kwtest k;
    TEST_EQUAL(k.fun(), 0b000);
    TEST_EQUAL(k.a, 0);
    TEST_EQUAL(k.b, false);
    TEST_EQUAL(k.c, "");

    k = {};
    TEST_EQUAL(k.fun(kw_alpha = 42), 0b100);
    TEST_EQUAL(k.a, 42);
    TEST_EQUAL(k.b, false);
    TEST_EQUAL(k.c, "");

    k = {};
    TEST_EQUAL(k.fun(kw_bravo = true), 0b010);
    TEST_EQUAL(k.a, 0);
    TEST_EQUAL(k.b, true);
    TEST_EQUAL(k.c, "");

    k = {};
    TEST_EQUAL(k.fun(kw_bravo), 0b010);
    TEST_EQUAL(k.a, 0);
    TEST_EQUAL(k.b, true);
    TEST_EQUAL(k.c, "");

    k = {};
    TEST_EQUAL(k.fun(kw_alpha = 42, kw_bravo = true, kw_charlie = "hello"s), 0b111);
    TEST_EQUAL(k.a, 42);
    TEST_EQUAL(k.b, true);
    TEST_EQUAL(k.c, "hello");

    k = {};
    TEST_EQUAL(k.fun(kw_alpha = 42L, kw_bravo = 1, kw_charlie = "hello"), 0b111);
    TEST_EQUAL(k.a, 42);
    TEST_EQUAL(k.b, true);
    TEST_EQUAL(k.c, "hello");

    k = {};
    TEST_EQUAL(k.fun(kw_alpha = 42, kw_bravo, kw_charlie = "hello"), 0b111);
    TEST_EQUAL(k.a, 42);
    TEST_EQUAL(k.b, true);
    TEST_EQUAL(k.c, "hello");

    k = {};
    TEST_EQUAL(k.fun(kw_charlie = "hello", kw_alpha = 42), 0b101);
    TEST_EQUAL(k.a, 42);
    TEST_EQUAL(k.b, false);
    TEST_EQUAL(k.c, "hello");

}
