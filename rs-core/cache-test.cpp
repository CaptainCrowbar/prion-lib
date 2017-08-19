#include "rs-core/cache.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;

namespace {

    int count = 0;

    U8string f1(char c) { ++count; return {c}; }
    U8string f2(size_t n, char c) { ++count; return U8string(n, c); }

    void check_cache() {

        Cache<U8string(char)> c1;
        Cache<U8string(size_t, char)> c2;
        U8string s;

        TRY(c1 = memoize(f1, 3));
        TRY(c2 = memoize(f2, 3));

        TRY(s = c1('a'));  TEST_EQUAL(s, "a");  TEST_EQUAL(count, 1);
        TRY(s = c1('b'));  TEST_EQUAL(s, "b");  TEST_EQUAL(count, 2);
        TRY(s = c1('c'));  TEST_EQUAL(s, "c");  TEST_EQUAL(count, 3);
        TRY(s = c1('d'));  TEST_EQUAL(s, "d");  TEST_EQUAL(count, 4);
        TRY(s = c1('e'));  TEST_EQUAL(s, "e");  TEST_EQUAL(count, 5);

        TRY(c1.clear());
        count = 0;

        TRY(s = c1('a'));  TEST_EQUAL(s, "a");  TEST_EQUAL(count, 1);
        TRY(s = c1('b'));  TEST_EQUAL(s, "b");  TEST_EQUAL(count, 2);
        TRY(s = c1('c'));  TEST_EQUAL(s, "c");  TEST_EQUAL(count, 3);
        TRY(s = c1('b'));  TEST_EQUAL(s, "b");  TEST_EQUAL(count, 3);
        TRY(s = c1('a'));  TEST_EQUAL(s, "a");  TEST_EQUAL(count, 3);
        TRY(s = c1('d'));  TEST_EQUAL(s, "d");  TEST_EQUAL(count, 4);
        TRY(s = c1('e'));  TEST_EQUAL(s, "e");  TEST_EQUAL(count, 5);

        count = 0;

        TRY(s = c2(1, 'x'));  TEST_EQUAL(s, "x");      TEST_EQUAL(count, 1);
        TRY(s = c2(2, 'x'));  TEST_EQUAL(s, "xx");     TEST_EQUAL(count, 2);
        TRY(s = c2(3, 'x'));  TEST_EQUAL(s, "xxx");    TEST_EQUAL(count, 3);
        TRY(s = c2(4, 'x'));  TEST_EQUAL(s, "xxxx");   TEST_EQUAL(count, 4);
        TRY(s = c2(5, 'x'));  TEST_EQUAL(s, "xxxxx");  TEST_EQUAL(count, 5);

        TRY(c2.clear());
        count = 0;

        TRY(s = c2(1, 'x'));  TEST_EQUAL(s, "x");    TEST_EQUAL(count, 1);
        TRY(s = c2(2, 'x'));  TEST_EQUAL(s, "xx");   TEST_EQUAL(count, 2);
        TRY(s = c2(3, 'x'));  TEST_EQUAL(s, "xxx");  TEST_EQUAL(count, 3);
        TRY(s = c2(2, 'x'));  TEST_EQUAL(s, "xx");   TEST_EQUAL(count, 3);
        TRY(s = c2(1, 'x'));  TEST_EQUAL(s, "x");    TEST_EQUAL(count, 3);
        TRY(s = c2(3, 'y'));  TEST_EQUAL(s, "yyy");  TEST_EQUAL(count, 4);
        TRY(s = c2(3, 'z'));  TEST_EQUAL(s, "zzz");  TEST_EQUAL(count, 5);

    }

}

TEST_MODULE(core, cache) {

    check_cache();

}
