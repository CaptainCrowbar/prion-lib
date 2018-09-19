#include "rs-core/rational.hpp"
#include "rs-core/unit-test.hpp"
#include <stdexcept>
#include <unordered_set>

using namespace RS;

void test_core_rational_basics() {

    Rat r;

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

    TRY(r = Rat(5, 3));
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

    TRY(r = Rat(-7, 9));
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

}

void test_core_rational_reduction() {

    Rat r;

    TRY(r = Rat(0, 6));    TEST_EQUAL(r.num(), 0);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Rat(0, 6));
    TRY(r = Rat(1, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Rat(1, 6));
    TRY(r = Rat(2, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Rat(2, 6));
    TRY(r = Rat(3, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Rat(3, 6));
    TRY(r = Rat(4, 6));    TEST_EQUAL(r.num(), 2);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Rat(4, 6));
    TRY(r = Rat(5, 6));    TEST_EQUAL(r.num(), 5);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Rat(5, 6));
    TRY(r = Rat(6, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Rat(0, 6));
    TRY(r = Rat(7, 6));    TEST_EQUAL(r.num(), 7);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Rat(1, 6));
    TRY(r = Rat(8, 6));    TEST_EQUAL(r.num(), 4);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Rat(2, 6));
    TRY(r = Rat(9, 6));    TEST_EQUAL(r.num(), 3);    TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Rat(3, 6));
    TRY(r = Rat(10, 6));   TEST_EQUAL(r.num(), 5);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Rat(4, 6));
    TRY(r = Rat(11, 6));   TEST_EQUAL(r.num(), 11);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Rat(5, 6));
    TRY(r = Rat(12, 6));   TEST_EQUAL(r.num(), 2);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 2);   TEST_EQUAL(r.frac(), Rat(0, 6));
    TRY(r = Rat(-1, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Rat(-1, 6));
    TRY(r = Rat(-2, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Rat(-2, 6));
    TRY(r = Rat(-3, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Rat(-3, 6));
    TRY(r = Rat(-4, 6));   TEST_EQUAL(r.num(), -2);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Rat(-4, 6));
    TRY(r = Rat(-5, 6));   TEST_EQUAL(r.num(), -5);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Rat(-5, 6));
    TRY(r = Rat(-6, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Rat(-0, 6));
    TRY(r = Rat(-7, 6));   TEST_EQUAL(r.num(), -7);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Rat(-1, 6));
    TRY(r = Rat(-8, 6));   TEST_EQUAL(r.num(), -4);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Rat(-2, 6));
    TRY(r = Rat(-9, 6));   TEST_EQUAL(r.num(), -3);   TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Rat(-3, 6));
    TRY(r = Rat(-10, 6));  TEST_EQUAL(r.num(), -5);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Rat(-4, 6));
    TRY(r = Rat(-11, 6));  TEST_EQUAL(r.num(), -11);  TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Rat(-5, 6));
    TRY(r = Rat(-12, 6));  TEST_EQUAL(r.num(), -2);   TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), -2);  TEST_EQUAL(r.frac(), Rat(-0, 6));

}

void test_core_rational_arithmetic() {

    Rat r, s, t;

    TRY(r = Rat(5, 3));
    TRY(s = Rat(7, 9));

    TRY(t = r + s);  TEST_EQUAL(t.num(), 22);  TEST_EQUAL(t.den(), 9);
    TRY(t = r - s);  TEST_EQUAL(t.num(), 8);   TEST_EQUAL(t.den(), 9);
    TRY(t = r * s);  TEST_EQUAL(t.num(), 35);  TEST_EQUAL(t.den(), 27);
    TRY(t = r / s);  TEST_EQUAL(t.num(), 15);  TEST_EQUAL(t.den(), 7);

}

void test_core_rational_properties() {

    Rat r, s, t;

    TRY(r = Rat(5, 3));
    TRY(s = Rat(-7, 9));

    TEST_EQUAL(abs(r), Rat(5, 3));  TEST_EQUAL(sign_of(r), 1);
    TEST_EQUAL(abs(s), Rat(7, 9));  TEST_EQUAL(sign_of(s), -1);
    TEST_EQUAL(abs(t), Rat(0));     TEST_EQUAL(sign_of(t), 0);

    TRY(r = Rat(-6, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -2);  TEST_EQUAL(r.round(), -2);
    TRY(r = Rat(-5, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -2);
    TRY(r = Rat(-4, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Rat(-3, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Rat(-2, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), -1);
    TRY(r = Rat(-1, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Rat(0, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Rat(1, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 0);
    TRY(r = Rat(2, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Rat(3, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Rat(4, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 1);
    TRY(r = Rat(5, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Rat(6, 3));   TEST_EQUAL(r.floor(), 2);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Rat(-8, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -2);  TEST_EQUAL(r.round(), -2);
    TRY(r = Rat(-7, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -2);
    TRY(r = Rat(-6, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Rat(-5, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Rat(-4, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
    TRY(r = Rat(-3, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), -1);
    TRY(r = Rat(-2, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Rat(-1, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Rat(0, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
    TRY(r = Rat(1, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 0);
    TRY(r = Rat(2, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Rat(3, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Rat(4, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
    TRY(r = Rat(5, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 1);
    TRY(r = Rat(6, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Rat(7, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
    TRY(r = Rat(8, 4));   TEST_EQUAL(r.floor(), 2);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);

}

void test_core_rational_comparison() {

    Rat r, s;

    TRY(r = Rat(5, 6));   TRY(s = Rat(7, 9));     TEST_COMPARE(r, >, s);   TEST_COMPARE(r, >=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Rat(5, 6));   TRY(s = Rat(8, 9));     TEST_COMPARE(r, <, s);   TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Rat(5, 6));   TRY(s = Rat(10, 12));   TEST_COMPARE(r, ==, s);  TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, >=, s);
    TRY(r = Rat(-5, 6));  TRY(s = Rat(-7, 9));    TEST_COMPARE(r, <, s);   TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Rat(-5, 6));  TRY(s = Rat(-8, 9));    TEST_COMPARE(r, >, s);   TEST_COMPARE(r, >=, s);  TEST_COMPARE(r, !=, s);
    TRY(r = Rat(-5, 6));  TRY(s = Rat(-10, 12));  TEST_COMPARE(r, ==, s);  TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, >=, s);

}

void test_core_rational_mixed() {

    Rat r;

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

    Rat r;

    TRY(r = Rat::parse("0"));       TEST_EQUAL(r.num(), 0);   TEST_EQUAL(r.den(), 1);
    TRY(r = Rat::parse("5"));       TEST_EQUAL(r.num(), 5);   TEST_EQUAL(r.den(), 1);
    TRY(r = Rat::parse("-5"));      TEST_EQUAL(r.num(), -5);  TEST_EQUAL(r.den(), 1);
    TRY(r = Rat::parse("1/3"));     TEST_EQUAL(r.num(), 1);   TEST_EQUAL(r.den(), 3);
    TRY(r = Rat::parse("4/6"));     TEST_EQUAL(r.num(), 2);   TEST_EQUAL(r.den(), 3);
    TRY(r = Rat::parse("-1/3"));    TEST_EQUAL(r.num(), -1);  TEST_EQUAL(r.den(), 3);
    TRY(r = Rat::parse("-4/6"));    TEST_EQUAL(r.num(), -2);  TEST_EQUAL(r.den(), 3);
    TRY(r = Rat::parse("1 2/3"));   TEST_EQUAL(r.num(), 5);   TEST_EQUAL(r.den(), 3);
    TRY(r = Rat::parse("-1 2/3"));  TEST_EQUAL(r.num(), -5);  TEST_EQUAL(r.den(), 3);

    TEST_THROW(r = Rat::parse(""), std::invalid_argument);
    TEST_THROW(r = Rat::parse("1 2"), std::invalid_argument);
    TEST_THROW(r = Rat::parse("1 -2/3"), std::invalid_argument);

}

void test_core_rational_hash_set() {

    std::unordered_set<Rat> ratset;

    TEST(ratset.empty());

    for (int i = 1; i <= 10; ++i)
        TRY(ratset.insert(Rat(i)));

    TEST_EQUAL(ratset.size(), 10);

}

void test_core_rational_numeric_limits() {

    using limits_rat64 = std::numeric_limits<Rat64>;
    using limits_urat64 = std::numeric_limits<Urat64>;

    constexpr uint64_t max63 = 0x7fff'ffff'ffff'ffffull;
    constexpr uint64_t max64 = 0xffff'ffff'ffff'ffffull;

    TEST(limits_rat64::is_specialized);
    TEST(limits_rat64::is_bounded);
    TEST(! limits_rat64::is_integer);
    TEST(limits_rat64::is_signed);
    TEST_EQUAL(limits_rat64::digits, 63);
    TEST_EQUAL(limits_rat64::digits10, 18);
    TEST_EQUAL(limits_rat64::lowest(), Rat64(- int64_t(max63) - 1));
    TEST_EQUAL(limits_rat64::min(), Rat64(1) / Rat64(int64_t(max63)));
    TEST_EQUAL(limits_rat64::max(), Rat64(int64_t(max63)));

    TEST(limits_urat64::is_specialized);
    TEST(limits_urat64::is_bounded);
    TEST(! limits_urat64::is_integer);
    TEST(! limits_urat64::is_signed);
    TEST_EQUAL(limits_urat64::digits, 64);
    TEST_EQUAL(limits_urat64::digits10, 19);
    TEST_EQUAL(limits_urat64::lowest(), Urat64());
    TEST_EQUAL(limits_urat64::min(), Urat64(1) / Urat64(max64));
    TEST_EQUAL(limits_urat64::max(), Urat64(max64));

}
