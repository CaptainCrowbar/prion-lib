#include "rs-core/range-selection.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

using namespace RS;
using namespace RS::Range;
using namespace std::literals;

void test_core_range_selection_before_etc() {

    Ustring s;

    TRY("Hello world"s >> before_if(ascii_isspace) >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> before_if(ascii_ispunct) >> overwrite(s));  TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> until_if(ascii_isspace) >> overwrite(s));   TEST_EQUAL(s, "Hello ");
    TRY("Hello world"s >> until_if(ascii_ispunct) >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> from_if(ascii_isspace) >> overwrite(s));    TEST_EQUAL(s, " world");
    TRY("Hello world"s >> from_if(ascii_ispunct) >> overwrite(s));    TEST_EQUAL(s, "");
    TRY("Hello world"s >> after_if(ascii_isspace) >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> after_if(ascii_ispunct) >> overwrite(s));   TEST_EQUAL(s, "");

    TRY("Hello world"s >> passthrough >> before_if(ascii_isspace) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> passthrough >> before_if(ascii_ispunct) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough >> until_if(ascii_isspace) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello ");
    TRY("Hello world"s >> passthrough >> until_if(ascii_ispunct) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough >> from_if(ascii_isspace) >> passthrough >> overwrite(s));    TEST_EQUAL(s, " world");
    TRY("Hello world"s >> passthrough >> from_if(ascii_ispunct) >> passthrough >> overwrite(s));    TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough >> after_if(ascii_isspace) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> passthrough >> after_if(ascii_ispunct) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "");

    TRY("Hello world"s >> passthrough * before_if(ascii_isspace) * passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> passthrough * before_if(ascii_ispunct) * passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough * until_if(ascii_isspace) * passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello ");
    TRY("Hello world"s >> passthrough * until_if(ascii_ispunct) * passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough * from_if(ascii_isspace) * passthrough >> overwrite(s));    TEST_EQUAL(s, " world");
    TRY("Hello world"s >> passthrough * from_if(ascii_ispunct) * passthrough >> overwrite(s));    TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough * after_if(ascii_isspace) * passthrough >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> passthrough * after_if(ascii_ispunct) * passthrough >> overwrite(s));   TEST_EQUAL(s, "");

    s = "Hello world";  TRY(s << before_if(ascii_isspace));  TEST_EQUAL(s, "Hello");
    s = "Hello world";  TRY(s << before_if(ascii_ispunct));  TEST_EQUAL(s, "Hello world");
    s = "Hello world";  TRY(s << until_if(ascii_isspace));   TEST_EQUAL(s, "Hello ");
    s = "Hello world";  TRY(s << until_if(ascii_ispunct));   TEST_EQUAL(s, "Hello world");
    s = "Hello world";  TRY(s << from_if(ascii_isspace));    TEST_EQUAL(s, " world");
    s = "Hello world";  TRY(s << from_if(ascii_ispunct));    TEST_EQUAL(s, "");
    s = "Hello world";  TRY(s << after_if(ascii_isspace));   TEST_EQUAL(s, "world");
    s = "Hello world";  TRY(s << after_if(ascii_ispunct));   TEST_EQUAL(s, "");

    TRY("Hello world"s >> before(' ') >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> before('*') >> overwrite(s));  TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> until(' ') >> overwrite(s));   TEST_EQUAL(s, "Hello ");
    TRY("Hello world"s >> until('*') >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> from(' ') >> overwrite(s));    TEST_EQUAL(s, " world");
    TRY("Hello world"s >> from('*') >> overwrite(s));    TEST_EQUAL(s, "");
    TRY("Hello world"s >> after(' ') >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> after('*') >> overwrite(s));   TEST_EQUAL(s, "");

    TRY("Hello world"s >> passthrough >> before(' ') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> passthrough >> before('*') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough >> until(' ') >> passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello ");
    TRY("Hello world"s >> passthrough >> until('*') >> passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough >> from(' ') >> passthrough >> overwrite(s));    TEST_EQUAL(s, " world");
    TRY("Hello world"s >> passthrough >> from('*') >> passthrough >> overwrite(s));    TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough >> after(' ') >> passthrough >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> passthrough >> after('*') >> passthrough >> overwrite(s));   TEST_EQUAL(s, "");

    TRY("Hello world"s >> passthrough * before(' ') * passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> passthrough * before('*') * passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough * until(' ') * passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello ");
    TRY("Hello world"s >> passthrough * until('*') * passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough * from(' ') * passthrough >> overwrite(s));    TEST_EQUAL(s, " world");
    TRY("Hello world"s >> passthrough * from('*') * passthrough >> overwrite(s));    TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough * after(' ') * passthrough >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> passthrough * after('*') * passthrough >> overwrite(s));   TEST_EQUAL(s, "");

    s = "Hello world";  TRY(s << before(' '));  TEST_EQUAL(s, "Hello");
    s = "Hello world";  TRY(s << before('*'));  TEST_EQUAL(s, "Hello world");
    s = "Hello world";  TRY(s << until(' '));   TEST_EQUAL(s, "Hello ");
    s = "Hello world";  TRY(s << until('*'));   TEST_EQUAL(s, "Hello world");
    s = "Hello world";  TRY(s << from(' '));    TEST_EQUAL(s, " world");
    s = "Hello world";  TRY(s << from('*'));    TEST_EQUAL(s, "");
    s = "Hello world";  TRY(s << after(' '));   TEST_EQUAL(s, "world");
    s = "Hello world";  TRY(s << after('*'));   TEST_EQUAL(s, "");

}

void test_core_range_selection_filter() {

    Ustring s;

    TRY("Hello"s >> filter(ascii_isalpha) >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> filter(ascii_isdigit) >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> filter(ascii_isupper) >> overwrite(s));  TEST_EQUAL(s, "H");
    TRY("Hello"s >> filter(ascii_islower) >> overwrite(s));  TEST_EQUAL(s, "ello");

    TRY("Hello"s >> filter_out(ascii_isalpha) >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> filter_out(ascii_isdigit) >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> filter_out(ascii_isupper) >> overwrite(s));  TEST_EQUAL(s, "ello");
    TRY("Hello"s >> filter_out(ascii_islower) >> overwrite(s));  TEST_EQUAL(s, "H");

    TRY("Hello"s >> passthrough >> filter(ascii_isalpha) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> filter(ascii_isdigit) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> passthrough >> filter(ascii_isupper) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "H");
    TRY("Hello"s >> passthrough >> filter(ascii_islower) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "ello");

    TRY("Hello"s >> passthrough >> filter_out(ascii_isalpha) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> passthrough >> filter_out(ascii_isdigit) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> filter_out(ascii_isupper) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "ello");
    TRY("Hello"s >> passthrough >> filter_out(ascii_islower) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "H");

    TRY("Hello"s >> passthrough * filter(ascii_isalpha) * passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough * filter(ascii_isdigit) * passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> passthrough * filter(ascii_isupper) * passthrough >> overwrite(s));  TEST_EQUAL(s, "H");
    TRY("Hello"s >> passthrough * filter(ascii_islower) * passthrough >> overwrite(s));  TEST_EQUAL(s, "ello");

    TRY("Hello"s >> passthrough * filter_out(ascii_isalpha) * passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> passthrough * filter_out(ascii_isdigit) * passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough * filter_out(ascii_isupper) * passthrough >> overwrite(s));  TEST_EQUAL(s, "ello");
    TRY("Hello"s >> passthrough * filter_out(ascii_islower) * passthrough >> overwrite(s));  TEST_EQUAL(s, "H");

    s = "Hello";  TRY(s << filter(ascii_isalpha));  TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(s << filter(ascii_isdigit));  TEST_EQUAL(s, "");
    s = "Hello";  TRY(s << filter(ascii_isupper));  TEST_EQUAL(s, "H");
    s = "Hello";  TRY(s << filter(ascii_islower));  TEST_EQUAL(s, "ello");

    s = "Hello";  TRY(s << filter_out(ascii_isalpha));  TEST_EQUAL(s, "");
    s = "Hello";  TRY(s << filter_out(ascii_isdigit));  TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(s << filter_out(ascii_isupper));  TEST_EQUAL(s, "ello");
    s = "Hello";  TRY(s << filter_out(ascii_islower));  TEST_EQUAL(s, "H");

}

void test_core_range_selection_not_null() {

    std::vector<std::shared_ptr<Ustring>> v = {
        nullptr,
        std::make_shared<Ustring>("Hello"),
        nullptr,
        std::make_shared<Ustring>("World"),
        nullptr,
    };

    int n = 0;
    Ustring s;

    TRY(v >> not_null >> each([&] (auto& t) { TEST(t); ++n; if (t) s += *t; }));
    TEST_EQUAL(n, 2);
    TEST_EQUAL(s, "HelloWorld");

    TRY(v << not_null);
    TEST_EQUAL(v.size(), 2);
    for (auto& x: v)
        TEST(x);

}

void test_core_range_selection_sample() {

    std::vector<int> v1(100), v2, v3;
    std::iota(v1.begin(), v1.end(), 1);
    std::mt19937 rng(42);

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 100; j += 10) {
            TRY(v1 >> sample_k(j, rng) >> overwrite(v2));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_EQUAL(v2.size(), j);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 100; j += 10) {
            TRY(v1 >> passthrough >> collect >> sample_k(j, rng) >> passthrough >> overwrite(v2));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_EQUAL(v2.size(), j);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 100; j += 10) {
            TRY(v1 >> passthrough * collect * sample_k(j, rng) * passthrough >> overwrite(v2));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_EQUAL(v2.size(), j);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 100; j += 10) {
            v2 = v1;
            TRY(v2 << sample_k(j, rng));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_EQUAL(v2.size(), j);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 10; j <= 90; j += 10) {
            double p = double(j) / 100;
            TRY(v1 >> sample_p(p, rng) >> overwrite(v2));
            double q = double(v2.size()) / double(v1.size());
            TEST_NEAR_EPSILON(q, p, 0.2);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 10; j <= 90; j += 10) {
            double p = double(j) / 100;
            TRY(v1 >> passthrough >> collect >> sample_p(p, rng) >> passthrough >> overwrite(v2));
            double q = double(v2.size()) / double(v1.size());
            TEST_NEAR_EPSILON(q, p, 0.2);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 10; j <= 90; j += 10) {
            double p = double(j) / 100;
            TRY(v1 >> passthrough * collect * sample_p(p, rng) * passthrough >> overwrite(v2));
            double q = double(v2.size()) / double(v1.size());
            TEST_NEAR_EPSILON(q, p, 0.2);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 10; j <= 90; j += 10) {
            double p = double(j) / 100;
            v2 = v1;
            TRY(v2 << sample_p(p, rng));
            double q = double(v2.size()) / double(v1.size());
            TEST_NEAR_EPSILON(q, p, 0.2);
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 200; j += 10) {
            TRY(v1 >> sample_replace(j, rng) >> overwrite(v2));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_COMPARE(v2.size(), <=, std::min(j, v1.size()));
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 200; j += 10) {
            TRY(v1 >> passthrough >> collect >> sample_replace(j, rng) >> passthrough >> overwrite(v2));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_COMPARE(v2.size(), <=, std::min(j, v1.size()));
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 200; j += 10) {
            TRY(v1 >> passthrough * collect * sample_replace(j, rng) * passthrough >> overwrite(v2));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_COMPARE(v2.size(), <=, std::min(j, v1.size()));
        }
    }

    for (int i = 0; i < 100; ++i) {
        for (size_t j = 0; j <= 200; j += 10) {
            v2 = v1;
            TRY(v2 << sample_replace(j, rng));
            TEST_EQUAL(v2.size(), j);
            TRY(con_sort_unique(v2));
            TEST_COMPARE(v2.size(), <=, std::min(j, v1.size()));
        }
    }

}

void test_core_range_selection_skip_take() {

    Ustring s;

    TRY("Hello world"s >> skip(0) >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> skip(6) >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> skip(20) >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello world"s >> take(0) >> overwrite(s));   TEST_EQUAL(s, "");
    TRY("Hello world"s >> take(5) >> overwrite(s));   TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> take(20) >> overwrite(s));  TEST_EQUAL(s, "Hello world");

    TRY("Hello world"s >> passthrough >> skip(0) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough >> skip(6) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> passthrough >> skip(20) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough >> take(0) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough >> take(5) >> passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> passthrough >> take(20) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello world");

    TRY("Hello world"s >> passthrough * skip(0) * passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello world");
    TRY("Hello world"s >> passthrough * skip(6) * passthrough >> overwrite(s));   TEST_EQUAL(s, "world");
    TRY("Hello world"s >> passthrough * skip(20) * passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough * take(0) * passthrough >> overwrite(s));   TEST_EQUAL(s, "");
    TRY("Hello world"s >> passthrough * take(5) * passthrough >> overwrite(s));   TEST_EQUAL(s, "Hello");
    TRY("Hello world"s >> passthrough * take(20) * passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello world");

    s = "Hello world";  TRY(s << skip(0));   TEST_EQUAL(s, "Hello world");
    s = "Hello world";  TRY(s << skip(6));   TEST_EQUAL(s, "world");
    s = "Hello world";  TRY(s << skip(20));  TEST_EQUAL(s, "");
    s = "Hello world";  TRY(s << take(0));   TEST_EQUAL(s, "");
    s = "Hello world";  TRY(s << take(5));   TEST_EQUAL(s, "Hello");
    s = "Hello world";  TRY(s << take(20));  TEST_EQUAL(s, "Hello world");

}

void test_core_range_selection_stride() {

    Ustring s;

    TRY("The quick brown fox"s >> stride(1) >> overwrite(s));     TEST_EQUAL(s, "The quick brown fox");
    TRY("The quick brown fox"s >> stride(2) >> overwrite(s));     TEST_EQUAL(s, "h uc rw o");
    TRY("The quick brown fox"s >> stride(3) >> overwrite(s));     TEST_EQUAL(s, "eukrno");
    TRY("The quick brown fox"s >> stride(4) >> overwrite(s));     TEST_EQUAL(s, " cr ");
    TRY("The quick brown fox"s >> stride(5) >> overwrite(s));     TEST_EQUAL(s, "q n");
    TRY("The quick brown fox"s >> stride(0, 3) >> overwrite(s));  TEST_EQUAL(s, "T i o x");
    TRY("The quick brown fox"s >> stride(1, 3) >> overwrite(s));  TEST_EQUAL(s, "hqcbwf");
    TRY("The quick brown fox"s >> stride(2, 3) >> overwrite(s));  TEST_EQUAL(s, "eukrno");
    TRY("The quick brown fox"s >> stride(3, 3) >> overwrite(s));  TEST_EQUAL(s, " i o x");
    TRY("The quick brown fox"s >> stride(4, 3) >> overwrite(s));  TEST_EQUAL(s, "qcbwf");

    TRY("The quick brown fox"s >> passthrough >> stride(1) >> passthrough >> overwrite(s));     TEST_EQUAL(s, "The quick brown fox");
    TRY("The quick brown fox"s >> passthrough >> stride(2) >> passthrough >> overwrite(s));     TEST_EQUAL(s, "h uc rw o");
    TRY("The quick brown fox"s >> passthrough >> stride(3) >> passthrough >> overwrite(s));     TEST_EQUAL(s, "eukrno");
    TRY("The quick brown fox"s >> passthrough >> stride(4) >> passthrough >> overwrite(s));     TEST_EQUAL(s, " cr ");
    TRY("The quick brown fox"s >> passthrough >> stride(5) >> passthrough >> overwrite(s));     TEST_EQUAL(s, "q n");
    TRY("The quick brown fox"s >> passthrough >> stride(0, 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "T i o x");
    TRY("The quick brown fox"s >> passthrough >> stride(1, 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "hqcbwf");
    TRY("The quick brown fox"s >> passthrough >> stride(2, 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "eukrno");
    TRY("The quick brown fox"s >> passthrough >> stride(3, 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, " i o x");
    TRY("The quick brown fox"s >> passthrough >> stride(4, 3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "qcbwf");

    TRY("The quick brown fox"s >> passthrough * stride(1) * passthrough >> overwrite(s));     TEST_EQUAL(s, "The quick brown fox");
    TRY("The quick brown fox"s >> passthrough * stride(2) * passthrough >> overwrite(s));     TEST_EQUAL(s, "h uc rw o");
    TRY("The quick brown fox"s >> passthrough * stride(3) * passthrough >> overwrite(s));     TEST_EQUAL(s, "eukrno");
    TRY("The quick brown fox"s >> passthrough * stride(4) * passthrough >> overwrite(s));     TEST_EQUAL(s, " cr ");
    TRY("The quick brown fox"s >> passthrough * stride(5) * passthrough >> overwrite(s));     TEST_EQUAL(s, "q n");
    TRY("The quick brown fox"s >> passthrough * stride(0, 3) * passthrough >> overwrite(s));  TEST_EQUAL(s, "T i o x");
    TRY("The quick brown fox"s >> passthrough * stride(1, 3) * passthrough >> overwrite(s));  TEST_EQUAL(s, "hqcbwf");
    TRY("The quick brown fox"s >> passthrough * stride(2, 3) * passthrough >> overwrite(s));  TEST_EQUAL(s, "eukrno");
    TRY("The quick brown fox"s >> passthrough * stride(3, 3) * passthrough >> overwrite(s));  TEST_EQUAL(s, " i o x");
    TRY("The quick brown fox"s >> passthrough * stride(4, 3) * passthrough >> overwrite(s));  TEST_EQUAL(s, "qcbwf");

    s = "The quick brown fox";  TRY(s << stride(1));     TEST_EQUAL(s, "The quick brown fox");
    s = "The quick brown fox";  TRY(s << stride(2));     TEST_EQUAL(s, "h uc rw o");
    s = "The quick brown fox";  TRY(s << stride(3));     TEST_EQUAL(s, "eukrno");
    s = "The quick brown fox";  TRY(s << stride(4));     TEST_EQUAL(s, " cr ");
    s = "The quick brown fox";  TRY(s << stride(5));     TEST_EQUAL(s, "q n");
    s = "The quick brown fox";  TRY(s << stride(0, 3));  TEST_EQUAL(s, "T i o x");
    s = "The quick brown fox";  TRY(s << stride(1, 3));  TEST_EQUAL(s, "hqcbwf");
    s = "The quick brown fox";  TRY(s << stride(2, 3));  TEST_EQUAL(s, "eukrno");
    s = "The quick brown fox";  TRY(s << stride(3, 3));  TEST_EQUAL(s, " i o x");
    s = "The quick brown fox";  TRY(s << stride(4, 3));  TEST_EQUAL(s, "qcbwf");

}

void test_core_range_selection_unique() {

    Ustring s;
    auto f = [] (char a, char b) { return ascii_isupper(a) == ascii_isupper(b) && ascii_islower(a) == ascii_islower(b); };

    TRY("a bb ccc dddd eeeee"s >> unique >> overwrite(s));                                TEST_EQUAL(s, "a b c d e");
    TRY("a bb ccc dddd eeeee"s >> passthrough >> unique >> passthrough >> overwrite(s));  TEST_EQUAL(s, "a b c d e");
    TRY("a bb ccc dddd eeeee"s >> passthrough * unique >> overwrite(s));                  TEST_EQUAL(s, "a b c d e");
    TRY("a bb ccc dddd eeeee"s >> unique * passthrough >> overwrite(s));                  TEST_EQUAL(s, "a b c d e");
    TRY("a bb ccc dddd eeeee"s >> passthrough * unique * passthrough >> overwrite(s));    TEST_EQUAL(s, "a b c d e");
    TRY("Hello World"s >> unique(f) >> overwrite(s));                                     TEST_EQUAL(s, "He Wo");
    TRY("Hello World"s >> passthrough >> unique(f) >> passthrough >> overwrite(s));       TEST_EQUAL(s, "He Wo");
    TRY("Hello World"s >> passthrough * unique(f) >> overwrite(s));                       TEST_EQUAL(s, "He Wo");
    TRY("Hello World"s >> unique(f) * passthrough >> overwrite(s));                       TEST_EQUAL(s, "He Wo");
    TRY("Hello World"s >> passthrough * unique(f) * passthrough >> overwrite(s));         TEST_EQUAL(s, "He Wo");

}
