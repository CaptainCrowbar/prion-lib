#include "rs-core/range-permutation.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <functional>
#include <numeric>
#include <random>
#include <vector>

using namespace RS;
using namespace RS::Range;
using namespace std::literals;

void test_core_range_permutation_permutations() {

    Ustring s1, s2;

    s1 = "abc";

    s2 = s1;  TRY(s2 >> next_permutation >> overwrite(s1));  TEST_EQUAL(s1, "acb");
    s2 = s1;  TRY(s2 >> next_permutation >> overwrite(s1));  TEST_EQUAL(s1, "bac");
    s2 = s1;  TRY(s2 >> next_permutation >> overwrite(s1));  TEST_EQUAL(s1, "bca");
    s2 = s1;  TRY(s2 >> next_permutation >> overwrite(s1));  TEST_EQUAL(s1, "cab");
    s2 = s1;  TRY(s2 >> next_permutation >> overwrite(s1));  TEST_EQUAL(s1, "cba");
    s2 = s1;  TRY(s2 >> next_permutation >> overwrite(s1));  TEST_EQUAL(s1, "abc");

    s1 = "abc";

    s2 = s1;  TRY(s2 >> prev_permutation >> overwrite(s1));  TEST_EQUAL(s1, "cba");
    s2 = s1;  TRY(s2 >> prev_permutation >> overwrite(s1));  TEST_EQUAL(s1, "cab");
    s2 = s1;  TRY(s2 >> prev_permutation >> overwrite(s1));  TEST_EQUAL(s1, "bca");
    s2 = s1;  TRY(s2 >> prev_permutation >> overwrite(s1));  TEST_EQUAL(s1, "bac");
    s2 = s1;  TRY(s2 >> prev_permutation >> overwrite(s1));  TEST_EQUAL(s1, "acb");
    s2 = s1;  TRY(s2 >> prev_permutation >> overwrite(s1));  TEST_EQUAL(s1, "abc");

    s1 = "abc";

    TRY(s1 << next_permutation);  TEST_EQUAL(s1, "acb");
    TRY(s1 << next_permutation);  TEST_EQUAL(s1, "bac");
    TRY(s1 << next_permutation);  TEST_EQUAL(s1, "bca");
    TRY(s1 << next_permutation);  TEST_EQUAL(s1, "cab");
    TRY(s1 << next_permutation);  TEST_EQUAL(s1, "cba");
    TRY(s1 << next_permutation);  TEST_EQUAL(s1, "abc");

    s1 = "abc";

    TRY(s1 << prev_permutation);  TEST_EQUAL(s1, "cba");
    TRY(s1 << prev_permutation);  TEST_EQUAL(s1, "cab");
    TRY(s1 << prev_permutation);  TEST_EQUAL(s1, "bca");
    TRY(s1 << prev_permutation);  TEST_EQUAL(s1, "bac");
    TRY(s1 << prev_permutation);  TEST_EQUAL(s1, "acb");
    TRY(s1 << prev_permutation);  TEST_EQUAL(s1, "abc");

    s1 = "abc";

    s2 = s1;  TRY(s2 >> next_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "cba");
    s2 = s1;  TRY(s2 >> next_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "cab");
    s2 = s1;  TRY(s2 >> next_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "bca");
    s2 = s1;  TRY(s2 >> next_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "bac");
    s2 = s1;  TRY(s2 >> next_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "acb");
    s2 = s1;  TRY(s2 >> next_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "abc");

    s1 = "abc";

    s2 = s1;  TRY(s2 >> prev_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "acb");
    s2 = s1;  TRY(s2 >> prev_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "bac");
    s2 = s1;  TRY(s2 >> prev_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "bca");
    s2 = s1;  TRY(s2 >> prev_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "cab");
    s2 = s1;  TRY(s2 >> prev_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "cba");
    s2 = s1;  TRY(s2 >> prev_permutation(std::greater<>()) >> overwrite(s1));  TEST_EQUAL(s1, "abc");

    s1 = "abc";

    TRY(s1 << next_permutation(std::greater<>()));  TEST_EQUAL(s1, "cba");
    TRY(s1 << next_permutation(std::greater<>()));  TEST_EQUAL(s1, "cab");
    TRY(s1 << next_permutation(std::greater<>()));  TEST_EQUAL(s1, "bca");
    TRY(s1 << next_permutation(std::greater<>()));  TEST_EQUAL(s1, "bac");
    TRY(s1 << next_permutation(std::greater<>()));  TEST_EQUAL(s1, "acb");
    TRY(s1 << next_permutation(std::greater<>()));  TEST_EQUAL(s1, "abc");

    s1 = "abc";

    TRY(s1 << prev_permutation(std::greater<>()));  TEST_EQUAL(s1, "acb");
    TRY(s1 << prev_permutation(std::greater<>()));  TEST_EQUAL(s1, "bac");
    TRY(s1 << prev_permutation(std::greater<>()));  TEST_EQUAL(s1, "bca");
    TRY(s1 << prev_permutation(std::greater<>()));  TEST_EQUAL(s1, "cab");
    TRY(s1 << prev_permutation(std::greater<>()));  TEST_EQUAL(s1, "cba");
    TRY(s1 << prev_permutation(std::greater<>()));  TEST_EQUAL(s1, "abc");

}

void test_core_range_permutation_reverse() {

    Ustring s;

    TRY("Hello"s >> reverse >> overwrite(s));                                TEST_EQUAL(s, "olleH");
    TRY("Hello"s >> passthrough >> reverse >> passthrough >> overwrite(s));  TEST_EQUAL(s, "olleH");
    TRY("Hello"s >> passthrough * reverse >> overwrite(s));                  TEST_EQUAL(s, "olleH");
    TRY("Hello"s >> reverse * passthrough >> overwrite(s));                  TEST_EQUAL(s, "olleH");
    TRY("Hello"s >> passthrough * reverse * passthrough >> overwrite(s));    TEST_EQUAL(s, "olleH");

    s = "Hello";  TRY(s << reverse);  TEST_EQUAL(s, "olleH");

}

void test_core_range_permutation_shuffle() {

    Ustring s;
    std::vector<int> v1(10), v2;
    std::iota(v1.begin(), v1.end(), 1);
    std::mt19937 rng(42);

    for (int i = 0; i < 1000; ++i) {
        TRY(v1 >> shuffle(rng) >> overwrite(v2));
        TEST(v1 != v2);
        std::sort(v2.begin(), v2.end());
        TEST_EQUAL_RANGE(v1, v2);
    }

    for (int i = 0; i < 1000; ++i) {
        TRY(v1 >> passthrough >> shuffle(rng) >> passthrough >> overwrite(v2));
        TEST(v1 != v2);
        std::sort(v2.begin(), v2.end());
        TEST_EQUAL_RANGE(v1, v2);
    }

    for (int i = 0; i < 1000; ++i) {
        TRY(v1 >> passthrough * shuffle(rng) * passthrough >> overwrite(v2));
        TEST(v1 != v2);
        std::sort(v2.begin(), v2.end());
        TEST_EQUAL_RANGE(v1, v2);
    }

    for (int i = 0; i < 1000; ++i) {
        v2 = v1;
        TRY(v2 << shuffle(rng));
        TEST(v1 != v2);
        std::sort(v2.begin(), v2.end());
        TEST_EQUAL_RANGE(v1, v2);
    }

}

void test_core_range_permutation_sort() {

    Ustring s;
    auto by_case = [] (char a, char b) { return ascii_islower(a) && ascii_isupper(b); };

    TRY("Hello World"s >> sort >> overwrite(s));                                                  TEST_EQUAL(s, " HWdellloor");
    TRY("Hello World"s >> sort(std::greater<>()) >> overwrite(s));                                TEST_EQUAL(s, "roollledWH ");
    TRY("Hello World"s >> passthrough >> sort >> passthrough >> overwrite(s));                    TEST_EQUAL(s, " HWdellloor");
    TRY("Hello World"s >> passthrough >> sort(std::greater<>()) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "roollledWH ");
    TRY("HelloWorld"s >> stable_sort(by_case) >> overwrite(s));                                   TEST_EQUAL(s, "elloorldHW");
    TRY("HelloWorld"s >> passthrough >> stable_sort(by_case) >> passthrough >> overwrite(s));     TEST_EQUAL(s, "elloorldHW");
    TRY("Hello World"s >> passthrough * sort >> overwrite(s));                                    TEST_EQUAL(s, " HWdellloor");
    TRY("Hello World"s >> sort * passthrough >> overwrite(s));                                    TEST_EQUAL(s, " HWdellloor");
    TRY("Hello World"s >> passthrough * sort * passthrough >> overwrite(s));                      TEST_EQUAL(s, " HWdellloor");

    s = "Hello World";  TRY(s << sort);                    TEST_EQUAL(s, " HWdellloor");
    s = "Hello World";  TRY(s << sort(std::greater<>()));  TEST_EQUAL(s, "roollledWH ");
    s = "HelloWorld";   TRY(s << stable_sort(by_case));    TEST_EQUAL(s, "elloorldHW");

}
