#include "rs-core/rational.hpp"
#include "rs-core/unit-test.hpp"
#include <limits>
#include <stdexcept>
#include <unordered_set>

using namespace RS;

void test_core_rational_basics() {

    Ratio r;

    TEST_EQUAL(r.num(), 0);
    TEST_EQUAL(r.den(), 1);
    TEST(! bool(r));
    TEST(! r);
    TEST_EQUAL(int(r), 0);
    TEST_EQUAL(double(r), 0);
    TEST(r.is_integer());
    TEST_EQUAL(r.whole(), 0);
    TEST_EQUAL(r.frac().num(), 0);
    TEST_EQUAL(r.frac().den(), 1);
    TEST_EQUAL(r.str(), "0");
    TEST_EQUAL(r.mixed(), "0");
    TEST_EQUAL(r.simple(), "0/1");
    TEST_EQUAL(to_str(r), "0");

    TRY(r = Ratio(5, 3));
    TEST_EQUAL(r.num(), 5);
    TEST_EQUAL(r.den(), 3);
    TEST_EQUAL(int(r), 1);
    TEST_NEAR(double(r), 1.666667);
    TEST(! r.is_integer());
    TEST_EQUAL(r.whole(), 1);
    TEST_EQUAL(r.frac().num(), 2);
    TEST_EQUAL(r.frac().den(), 3);
    TEST_EQUAL(r.str(), "5/3");
    TEST_EQUAL(r.mixed(), "1 2/3");
    TEST_EQUAL(r.simple(), "5/3");
    TEST_EQUAL(to_str(r), "5/3");

    TRY(r = Ratio(-7, 9));
    TEST_EQUAL(r.num(), -7);
    TEST_EQUAL(r.den(), 9);
    TEST_EQUAL(int(r), 0);
    TEST_NEAR(double(r), -0.777778);
    TEST(! r.is_integer());
    TEST_EQUAL(r.whole(), 0);
    TEST_EQUAL(r.frac().num(), -7);
    TEST_EQUAL(r.frac().den(), 9);
    TEST_EQUAL(r.str(), "-7/9");
    TEST_EQUAL(r.mixed(), "-7/9");
    TEST_EQUAL(r.simple(), "-7/9");
    TEST_EQUAL(to_str(r), "-7/9");

    TEST_THROW(r = Ratio(1, 0), std::domain_error);

}

void test_core_rational_reduction() {

    Ratio r;

    TRY(r = Ratio(0, 6));    TEST_EQUAL(r.num(), 0);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratio(0, 6));
    TRY(r = Ratio(1, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratio(1, 6));
    TRY(r = Ratio(2, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratio(2, 6));
    TRY(r = Ratio(3, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratio(3, 6));
    TRY(r = Ratio(4, 6));    TEST_EQUAL(r.num(), 2);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratio(4, 6));
    TRY(r = Ratio(5, 6));    TEST_EQUAL(r.num(), 5);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratio(5, 6));
    TRY(r = Ratio(6, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratio(0, 6));
    TRY(r = Ratio(7, 6));    TEST_EQUAL(r.num(), 7);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratio(1, 6));
    TRY(r = Ratio(8, 6));    TEST_EQUAL(r.num(), 4);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratio(2, 6));
    TRY(r = Ratio(9, 6));    TEST_EQUAL(r.num(), 3);    TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratio(3, 6));
    TRY(r = Ratio(10, 6));   TEST_EQUAL(r.num(), 5);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratio(4, 6));
    TRY(r = Ratio(11, 6));   TEST_EQUAL(r.num(), 11);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratio(5, 6));
    TRY(r = Ratio(12, 6));   TEST_EQUAL(r.num(), 2);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 2);   TEST_EQUAL(r.frac(), Ratio(0, 6));
    TRY(r = Ratio(-1, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratio(-1, 6));
    TRY(r = Ratio(-2, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratio(-2, 6));
    TRY(r = Ratio(-3, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratio(-3, 6));
    TRY(r = Ratio(-4, 6));   TEST_EQUAL(r.num(), -2);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratio(-4, 6));
    TRY(r = Ratio(-5, 6));   TEST_EQUAL(r.num(), -5);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratio(-5, 6));
    TRY(r = Ratio(-6, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratio(-0, 6));
    TRY(r = Ratio(-7, 6));   TEST_EQUAL(r.num(), -7);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratio(-1, 6));
    TRY(r = Ratio(-8, 6));   TEST_EQUAL(r.num(), -4);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratio(-2, 6));
    TRY(r = Ratio(-9, 6));   TEST_EQUAL(r.num(), -3);   TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratio(-3, 6));
    TRY(r = Ratio(-10, 6));  TEST_EQUAL(r.num(), -5);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratio(-4, 6));
    TRY(r = Ratio(-11, 6));  TEST_EQUAL(r.num(), -11);  TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratio(-5, 6));
    TRY(r = Ratio(-12, 6));  TEST_EQUAL(r.num(), -2);   TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), -2);  TEST_EQUAL(r.frac(), Ratio(-0, 6));

}

void test_core_rational_arithmetic() {

    Ratio r, s, t;

    TRY(r = Ratio(5, 3));
    TRY(s = Ratio(7, 9));

    TRY(t = + r);    TEST_EQUAL(t.num(), 5);    TEST_EQUAL(t.den(), 3);
    TRY(t = - r);    TEST_EQUAL(t.num(), -5);   TEST_EQUAL(t.den(), 3);
    TRY(t = r + s);  TEST_EQUAL(t.num(), 22);   TEST_EQUAL(t.den(), 9);
    TRY(t = r - s);  TEST_EQUAL(t.num(), 8);    TEST_EQUAL(t.den(), 9);
    TRY(t = r * s);  TEST_EQUAL(t.num(), 35);   TEST_EQUAL(t.den(), 27);
    TRY(t = r / s);  TEST_EQUAL(t.num(), 15);   TEST_EQUAL(t.den(), 7);
    TRY(t = s + 6);  TEST_EQUAL(t.num(), 61);   TEST_EQUAL(t.den(), 9);
    TRY(t = 6 + s);  TEST_EQUAL(t.num(), 61);   TEST_EQUAL(t.den(), 9);
    TRY(t = s - 6);  TEST_EQUAL(t.num(), -47);  TEST_EQUAL(t.den(), 9);
    TRY(t = 6 - s);  TEST_EQUAL(t.num(), 47);   TEST_EQUAL(t.den(), 9);
    TRY(t = s * 6);  TEST_EQUAL(t.num(), 14);   TEST_EQUAL(t.den(), 3);
    TRY(t = 6 * s);  TEST_EQUAL(t.num(), 14);   TEST_EQUAL(t.den(), 3);
    TRY(t = s / 6);  TEST_EQUAL(t.num(), 7);    TEST_EQUAL(t.den(), 54);
    TRY(t = 6 / s);  TEST_EQUAL(t.num(), 54);   TEST_EQUAL(t.den(), 7);

    TRY(s = 0);
    TEST_THROW(t = r / 0, std::domain_error);
    TEST_THROW(t = r / s, std::domain_error);

    TRY(t = r.reciprocal());  TEST_EQUAL(t.num(), 3);  TEST_EQUAL(t.den(), 5);

    TEST_THROW(t = s.reciprocal(), std::domain_error);

}

void test_core_rational_properties() {

    Ratio r, s, t;

    TRY(r = Ratio(5, 3));
    TRY(s = Ratio(-7, 9));

    TEST_EQUAL(abs(r), Ratio(5, 3));  TEST_EQUAL(sign_of(r), 1);
    TEST_EQUAL(abs(s), Ratio(7, 9));  TEST_EQUAL(sign_of(s), -1);
    TEST_EQUAL(abs(t), Ratio(0));     TEST_EQUAL(sign_of(t), 0);

    TRY(r = Ratio(-6, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -2);  TEST_EQUAL(r.round(), -2);
    TRY(r = Ratio(-5, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -2);
    TRY(r = Ratio(-4, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Ratio(-3, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Ratio(-2, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), -1);
    TRY(r = Ratio(-1, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Ratio(0, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Ratio(1, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 0);
    TRY(r = Ratio(2, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Ratio(3, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Ratio(4, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 1);
    TRY(r = Ratio(5, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Ratio(6, 3));   TEST_EQUAL(r.floor(), 2);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Ratio(-8, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -2);  TEST_EQUAL(r.round(), -2);
    TRY(r = Ratio(-7, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -2);
    TRY(r = Ratio(-6, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Ratio(-5, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Ratio(-4, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Ratio(-3, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), -1);
    TRY(r = Ratio(-2, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Ratio(-1, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Ratio(0, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Ratio(1, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 0);
    TRY(r = Ratio(2, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Ratio(3, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Ratio(4, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Ratio(5, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 1);
    TRY(r = Ratio(6, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Ratio(7, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Ratio(8, 4));   TEST_EQUAL(r.floor(), 2);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);

}

void test_core_rational_comparison() {

    Ratio r, s;

    TRY(r = Ratio(5, 6));   TRY(s = Ratio(7, 9));     TEST_COMPARE(r, >, s);   TEST_COMPARE(r, >=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Ratio(5, 6));   TRY(s = Ratio(8, 9));     TEST_COMPARE(r, <, s);   TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Ratio(5, 6));   TRY(s = Ratio(10, 12));   TEST_COMPARE(r, ==, s);  TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, >=, s);
    TRY(r = Ratio(-5, 6));  TRY(s = Ratio(-7, 9));    TEST_COMPARE(r, <, s);   TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Ratio(-5, 6));  TRY(s = Ratio(-8, 9));    TEST_COMPARE(r, >, s);   TEST_COMPARE(r, >=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Ratio(-5, 6));  TRY(s = Ratio(-10, 12));  TEST_COMPARE(r, ==, s);  TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, >=, s);

}

void test_core_rational_mixed() {

    Ratio r;

    TRY(r = 42);    TEST_EQUAL(r.num(), 42);   TEST_EQUAL(r.den(), 1);
    TRY(r += 100);  TEST_EQUAL(r.num(), 142);  TEST_EQUAL(r.den(), 1);
    TRY(r *= 2);    TEST_EQUAL(r.num(), 284);  TEST_EQUAL(r.den(), 1);

    TRY(r = 42);
    TEST_COMPARE(r, ==, 42);
    TEST_COMPARE(r, <=, 42);
    TEST_COMPARE(r, >=, 42);
    TEST_COMPARE(r, !=, 100);
    TEST_COMPARE(r, <, 100);
    TEST_COMPARE(r, <=, 100);

}

void test_core_rational_parsing() {

    Ratio r;

    TRY(r = Ratio::parse("0"));       TEST_EQUAL(r.num(), 0);   TEST_EQUAL(r.den(), 1);
    TRY(r = Ratio::parse("5"));       TEST_EQUAL(r.num(), 5);   TEST_EQUAL(r.den(), 1);
    TRY(r = Ratio::parse("-5"));      TEST_EQUAL(r.num(), -5);  TEST_EQUAL(r.den(), 1);
    TRY(r = Ratio::parse("1/3"));     TEST_EQUAL(r.num(), 1);   TEST_EQUAL(r.den(), 3);
    TRY(r = Ratio::parse("4/6"));     TEST_EQUAL(r.num(), 2);   TEST_EQUAL(r.den(), 3);
    TRY(r = Ratio::parse("-1/3"));    TEST_EQUAL(r.num(), -1);  TEST_EQUAL(r.den(), 3);
    TRY(r = Ratio::parse("-4/6"));    TEST_EQUAL(r.num(), -2);  TEST_EQUAL(r.den(), 3);
    TRY(r = Ratio::parse("1 2/3"));   TEST_EQUAL(r.num(), 5);   TEST_EQUAL(r.den(), 3);
    TRY(r = Ratio::parse("-1 2/3"));  TEST_EQUAL(r.num(), -5);  TEST_EQUAL(r.den(), 3);

    TEST_THROW(r = Ratio::parse(""), std::invalid_argument);
    TEST_THROW(r = Ratio::parse("1 2"), std::invalid_argument);
    TEST_THROW(r = Ratio::parse("1 -2/3"), std::invalid_argument);

}

void test_core_rational_hash_set() {

    std::unordered_set<Ratio> ratset;

    TEST(ratset.empty());

    for (int i = 1; i <= 10; ++i)
        TRY(ratset.insert(Ratio(i)));

    TEST_EQUAL(ratset.size(), 10);

}

void test_core_rational_numeric_limits() {

    using limits_rat64 = std::numeric_limits<Ratio64>;
    using limits_urat64 = std::numeric_limits<Uratio64>;

    constexpr uint64_t max63 = 0x7fff'ffff'ffff'ffffull;
    constexpr uint64_t max64 = 0xffff'ffff'ffff'ffffull;

    TEST(limits_rat64::is_specialized);
    TEST(limits_rat64::is_bounded);
    TEST(! limits_rat64::is_integer);
    TEST(limits_rat64::is_signed);
    TEST_EQUAL(limits_rat64::digits, 63);
    TEST_EQUAL(limits_rat64::digits10, 18);
    TEST_EQUAL(limits_rat64::lowest(), Ratio64(- int64_t(max63) - 1));
    TEST_EQUAL(limits_rat64::min(), Ratio64(1) / Ratio64(int64_t(max63)));
    TEST_EQUAL(limits_rat64::max(), Ratio64(int64_t(max63)));

    TEST(limits_urat64::is_specialized);
    TEST(limits_urat64::is_bounded);
    TEST(! limits_urat64::is_integer);
    TEST(! limits_urat64::is_signed);
    TEST_EQUAL(limits_urat64::digits, 64);
    TEST_EQUAL(limits_urat64::digits10, 19);
    TEST_EQUAL(limits_urat64::lowest(), Uratio64());
    TEST_EQUAL(limits_urat64::min(), Uratio64(1) / Uratio64(max64));
    TEST_EQUAL(limits_urat64::max(), Uratio64(max64));

}
