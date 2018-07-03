#include "rs-core/fixed-binary.hpp"
#include "rs-core/statistics.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <limits>
#include <random>

using namespace RS;
using namespace RS::Literals;

namespace {

    const auto min_int32 = std::numeric_limits<int32_t>::min();
    const auto max_int32 = std::numeric_limits<int32_t>::max();
    const auto min_int64 = std::numeric_limits<int64_t>::min();
    const auto max_int64 = std::numeric_limits<int64_t>::max();

}

void test_core_fixed_binary_implementation_selection() {

    TEST_TYPE(Binary<1>, SmallBinary<1>);
    TEST_TYPE(Binary<5>, SmallBinary<5>);
    TEST_TYPE(Binary<10>, SmallBinary<10>);
    TEST_TYPE(Binary<20>, SmallBinary<20>);
    TEST_TYPE(Binary<30>, SmallBinary<30>);
    TEST_TYPE(Binary<40>, SmallBinary<40>);
    TEST_TYPE(Binary<50>, SmallBinary<50>);
    TEST_TYPE(Binary<60>, SmallBinary<60>);
    TEST_TYPE(Binary<64>, SmallBinary<64>);
    TEST_TYPE(Binary<65>, LargeBinary<65>);
    TEST_TYPE(Binary<70>, LargeBinary<70>);
    TEST_TYPE(Binary<80>, LargeBinary<80>);
    TEST_TYPE(Binary<90>, LargeBinary<90>);
    TEST_TYPE(Binary<100>, LargeBinary<100>);
    TEST_TYPE(Binary<200>, LargeBinary<200>);
    TEST_TYPE(Binary<500>, LargeBinary<500>);
    TEST_TYPE(Binary<1000>, LargeBinary<1000>);

}

void test_core_fixed_binary_small_binary() {

    {

        using binary = SmallBinary<5>;

        static constexpr binary c(42);
        binary x, y, z;

        TEST_EQUAL(int(binary::max()), 31);

        TEST(c.fits_in<int>());  TEST_EQUAL(int(c), 10);  TEST(c);
        TEST(x.fits_in<int>());  TEST_EQUAL(int(x), 0);   TEST(! x);

        TRY(x = binary(25));
        TRY(y = binary(10));

        TEST_EQUAL(int(x), 25);
        TEST_EQUAL(int(y), 10);
        TEST_EQUAL(int(z), 0);

        TEST_EQUAL(x.as_binary(), "11001");
        TEST_EQUAL(y.as_binary(), "01010");
        TEST_EQUAL(z.as_binary(), "00000");
        TEST_EQUAL(x.as_hex(), "19");
        TEST_EQUAL(y.as_hex(), "0a");
        TEST_EQUAL(z.as_hex(), "00");
        TEST_EQUAL(x.as_decimal(), "25");
        TEST_EQUAL(y.as_decimal(), "10");
        TEST_EQUAL(z.as_decimal(), "0");
        TEST_EQUAL(x.as_double(), 25.0);
        TEST_EQUAL(y.as_double(), 10.0);
        TEST_EQUAL(z.as_double(), 0.0);

        TEST_EQUAL(x.significant_bits(), 5);
        TEST_EQUAL(y.significant_bits(), 4);
        TEST_EQUAL(z.significant_bits(), 0);

        TRY(z = - z);  TEST_EQUAL(int(z), 0);
        TRY(z = - x);  TEST_EQUAL(int(z), 7);
        TRY(z = - y);  TEST_EQUAL(int(z), 22);

        TRY(z.clear());
        TEST(! z);
        TEST_EQUAL(z, binary());

        TRY(z = ~ z);  TEST_EQUAL(int(z), 31);
        TRY(z = ~ x);  TEST_EQUAL(int(z), 6);
        TRY(z = ~ y);  TEST_EQUAL(int(z), 21);

        TRY(z = x + y);  TEST_EQUAL(int(z), 3);
        TRY(z = x - y);  TEST_EQUAL(int(z), 15);
        TRY(z = y - x);  TEST_EQUAL(int(z), 17);
        TRY(z = x * y);  TEST_EQUAL(int(z), 26);
        TRY(z = x / y);  TEST_EQUAL(int(z), 2);
        TRY(z = x % y);  TEST_EQUAL(int(z), 5);
        TRY(z = y / x);  TEST_EQUAL(int(z), 0);
        TRY(z = y % x);  TEST_EQUAL(int(z), 10);
        TRY(z = x & y);  TEST_EQUAL(int(z), 8);
        TRY(z = x | y);  TEST_EQUAL(int(z), 27);
        TRY(z = x ^ y);  TEST_EQUAL(int(z), 19);

        TRY(z = x << 0);   TEST_EQUAL(int(z), 25);  TRY(z = x >> 0);   TEST_EQUAL(int(z), 25);
        TRY(z = x << 1);   TEST_EQUAL(int(z), 18);  TRY(z = x >> 1);   TEST_EQUAL(int(z), 12);
        TRY(z = x << 2);   TEST_EQUAL(int(z), 4);   TRY(z = x >> 2);   TEST_EQUAL(int(z), 6);
        TRY(z = x << 3);   TEST_EQUAL(int(z), 8);   TRY(z = x >> 3);   TEST_EQUAL(int(z), 3);
        TRY(z = x << 4);   TEST_EQUAL(int(z), 16);  TRY(z = x >> 4);   TEST_EQUAL(int(z), 1);
        TRY(z = x << 5);   TEST_EQUAL(int(z), 0);   TRY(z = x >> 5);   TEST_EQUAL(int(z), 0);

        TEST(++x);    TEST_EQUAL(int(x), 26);
        TEST(--x);    TEST_EQUAL(int(x), 25);
        TEST(x--);    TEST_EQUAL(int(x), 24);
        TEST(x++);    TEST_EQUAL(int(x), 25);
        TEST(++y);    TEST_EQUAL(int(y), 11);
        TEST(--y);    TEST_EQUAL(int(y), 10);
        TEST(y--);    TEST_EQUAL(int(y), 9);
        TEST(y++);    TEST_EQUAL(int(y), 10);
        TEST(++z);    TEST_EQUAL(int(z), 1);
        TEST(! --z);  TEST_EQUAL(int(z), 0);
        TEST(! z--);  TEST_EQUAL(int(z), 31);
        TEST(z++);    TEST_EQUAL(int(z), 0);

        TEST_COMPARE(x, !=, y);
        TEST_COMPARE(x, >, y);
        TEST_COMPARE(x, >=, y);
        TEST_COMPARE(y, !=, x);
        TEST_COMPARE(y, <, x);
        TEST_COMPARE(y, <=, x);

        TRY(x = rotl(y, 0));   TEST_EQUAL(int(x), 10);
        TRY(x = rotl(y, 1));   TEST_EQUAL(int(x), 20);
        TRY(x = rotl(y, 2));   TEST_EQUAL(int(x), 9);
        TRY(x = rotl(y, 3));   TEST_EQUAL(int(x), 18);
        TRY(x = rotl(y, 4));   TEST_EQUAL(int(x), 5);
        TRY(x = rotl(y, 5));   TEST_EQUAL(int(x), 10);
        TRY(x = rotl(y, 6));   TEST_EQUAL(int(x), 20);
        TRY(x = rotl(y, 7));   TEST_EQUAL(int(x), 9);
        TRY(x = rotl(y, 8));   TEST_EQUAL(int(x), 18);
        TRY(x = rotl(y, 9));   TEST_EQUAL(int(x), 5);
        TRY(x = rotl(y, 10));  TEST_EQUAL(int(x), 10);

        TRY(x = rotr(y, 0));   TEST_EQUAL(int(x), 10);
        TRY(x = rotr(y, 1));   TEST_EQUAL(int(x), 5);
        TRY(x = rotr(y, 2));   TEST_EQUAL(int(x), 18);
        TRY(x = rotr(y, 3));   TEST_EQUAL(int(x), 9);
        TRY(x = rotr(y, 4));   TEST_EQUAL(int(x), 20);
        TRY(x = rotr(y, 5));   TEST_EQUAL(int(x), 10);
        TRY(x = rotr(y, 6));   TEST_EQUAL(int(x), 5);
        TRY(x = rotr(y, 7));   TEST_EQUAL(int(x), 18);
        TRY(x = rotr(y, 8));   TEST_EQUAL(int(x), 9);
        TRY(x = rotr(y, 9));   TEST_EQUAL(int(x), 20);
        TRY(x = rotr(y, 10));  TEST_EQUAL(int(x), 10);

        TRY(x = binary::from_double(25.0));
        TEST_EQUAL(x.as_decimal(), "25");
        TEST_EQUAL(x.as_double(), 25.0);
        TEST_EQUAL(int(x), 25);

    }

    {

        using binary = SmallBinary<35>;

        static constexpr binary c(42);
        binary x, y, z;

        TEST_EQUAL(uint64_t(binary::max()), 0x7ffffffff_u64);
        TEST_EQUAL(binary::max().as_hex(), "7ffffffff");

        TEST(c.fits_in<int64_t>());  TEST_EQUAL(int64_t(c), 42);  TEST(c);
        TEST(x.fits_in<int64_t>());  TEST_EQUAL(int64_t(x), 0);   TEST(! x);

        TRY(x = binary(0x123456789_u64));
        TRY(y = binary(0xabcdef_u64));

        TEST_EQUAL(int64_t(x), 4886718345);
        TEST_EQUAL(int64_t(y), 11259375);
        TEST_EQUAL(int64_t(z), 0);

        TEST_EQUAL(x.as_binary(), "00100100011010001010110011110001001");
        TEST_EQUAL(y.as_binary(), "00000000000101010111100110111101111");
        TEST_EQUAL(z.as_binary(), "00000000000000000000000000000000000");
        TEST_EQUAL(x.as_hex(), "123456789");
        TEST_EQUAL(y.as_hex(), "000abcdef");
        TEST_EQUAL(z.as_hex(), "000000000");
        TEST_EQUAL(x.as_decimal(), "4886718345");
        TEST_EQUAL(y.as_decimal(), "11259375");
        TEST_EQUAL(z.as_decimal(), "0");
        TEST_EQUAL(x.as_double(), 4886718345.0);
        TEST_EQUAL(y.as_double(), 11259375.0);
        TEST_EQUAL(z.as_double(), 0.0);

        TEST_EQUAL(x.significant_bits(), 33);
        TEST_EQUAL(y.significant_bits(), 24);
        TEST_EQUAL(z.significant_bits(), 0);

        TRY(z = - z);  TEST_EQUAL(uint64_t(z), 0);
        TRY(z = - x);  TEST_EQUAL(uint64_t(z), 0x6dcba9877_u64);
        TRY(z = - y);  TEST_EQUAL(uint64_t(z), 0x7ff543211_u64);

        TRY(z.clear());
        TEST(! z);
        TEST_EQUAL(z, binary());

        TRY(z = ~ z);  TEST_EQUAL(uint64_t(z), 0x7ffffffff_u64);
        TRY(z = ~ x);  TEST_EQUAL(uint64_t(z), 0x6dcba9876_u64);
        TRY(z = ~ y);  TEST_EQUAL(uint64_t(z), 0x7ff543210_u64);

        TRY(z = x + y);  TEST_EQUAL(uint64_t(z), 0x123f13578_u64);
        TRY(z = x - y);  TEST_EQUAL(uint64_t(z), 0x12299999a_u64);
        TRY(z = y - x);  TEST_EQUAL(uint64_t(z), 0x6dd666666_u64);
        TRY(z = x * y);  TEST_EQUAL(uint64_t(z), 0x2aa375de7_u64);
        TRY(z = x / y);  TEST_EQUAL(uint64_t(z), 0x0000001b2_u64);
        TRY(z = x % y);  TEST_EQUAL(uint64_t(z), 0x00002485b_u64);
        TRY(z = y / x);  TEST_EQUAL(uint64_t(z), 0);
        TRY(z = y % x);  TEST_EQUAL(uint64_t(z), 0x000abcdef_u64);
        TRY(z = x & y);  TEST_EQUAL(uint64_t(z), 0x000014589_u64);
        TRY(z = x | y);  TEST_EQUAL(uint64_t(z), 0x123efefef_u64);
        TRY(z = x ^ y);  TEST_EQUAL(uint64_t(z), 0x123eeaa66_u64);

        TRY(z = x << 0);   TEST_EQUAL(uint64_t(z), 0x123456789_u64);  TRY(z = x >> 0);   TEST_EQUAL(uint64_t(z), 0x123456789_u64);
        TRY(z = x << 5);   TEST_EQUAL(uint64_t(z), 0x468acf120_u64);  TRY(z = x >> 5);   TEST_EQUAL(uint64_t(z), 0x0091a2b3c_u64);
        TRY(z = x << 10);  TEST_EQUAL(uint64_t(z), 0x5159e2400_u64);  TRY(z = x >> 10);  TEST_EQUAL(uint64_t(z), 0x00048d159_u64);
        TRY(z = x << 15);  TEST_EQUAL(uint64_t(z), 0x2b3c48000_u64);  TRY(z = x >> 15);  TEST_EQUAL(uint64_t(z), 0x00002468a_u64);
        TRY(z = x << 20);  TEST_EQUAL(uint64_t(z), 0x678900000_u64);  TRY(z = x >> 20);  TEST_EQUAL(uint64_t(z), 0x000001234_u64);
        TRY(z = x << 25);  TEST_EQUAL(uint64_t(z), 0x712000000_u64);  TRY(z = x >> 25);  TEST_EQUAL(uint64_t(z), 0x000000091_u64);
        TRY(z = x << 30);  TEST_EQUAL(uint64_t(z), 0x240000000_u64);  TRY(z = x >> 30);  TEST_EQUAL(uint64_t(z), 0x000000004_u64);
        TRY(z = x << 35);  TEST_EQUAL(uint64_t(z), 0);                TRY(z = x >> 35);  TEST_EQUAL(uint64_t(z), 0);

        TEST(++x);    TEST_EQUAL(uint64_t(x), 0x12345678a_u64);
        TEST(--x);    TEST_EQUAL(uint64_t(x), 0x123456789_u64);
        TEST(x--);    TEST_EQUAL(uint64_t(x), 0x123456788_u64);
        TEST(x++);    TEST_EQUAL(uint64_t(x), 0x123456789_u64);
        TEST(++y);    TEST_EQUAL(uint64_t(y), 0xabcdf0_u64);
        TEST(--y);    TEST_EQUAL(uint64_t(y), 0xabcdef_u64);
        TEST(y--);    TEST_EQUAL(uint64_t(y), 0xabcdee_u64);
        TEST(y++);    TEST_EQUAL(uint64_t(y), 0xabcdef_u64);
        TEST(++z);    TEST_EQUAL(uint64_t(z), 1);
        TEST(! --z);  TEST_EQUAL(uint64_t(z), 0);
        TEST(! z--);  TEST_EQUAL(uint64_t(z), 0x7ffffffff_u64);
        TEST(z++);    TEST_EQUAL(uint64_t(z), 0);

        TEST_COMPARE(x, !=, y);
        TEST_COMPARE(x, >, y);
        TEST_COMPARE(x, >=, y);
        TEST_COMPARE(y, !=, x);
        TEST_COMPARE(y, <, x);
        TEST_COMPARE(y, <=, x);

        TRY(x = rotl(y, 0));   TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotl(y, 5));   TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotl(y, 10));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotl(y, 15));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotl(y, 20));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotl(y, 25));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotl(y, 30));  TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotl(y, 35));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotl(y, 40));  TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotl(y, 45));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotl(y, 50));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotl(y, 55));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotl(y, 60));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotl(y, 65));  TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotl(y, 70));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);

        TRY(x = rotr(y, 0));   TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotr(y, 5));   TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotr(y, 10));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotr(y, 15));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotr(y, 20));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotr(y, 25));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotr(y, 30));  TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotr(y, 35));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotr(y, 40));  TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotr(y, 45));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotr(y, 50));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotr(y, 55));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotr(y, 60));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotr(y, 65));  TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotr(y, 70));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);

        TRY(x = binary::from_double(12345678901.0));
        TEST_EQUAL(x.as_decimal(), "12345678901");
        TEST_EQUAL(x.as_double(), 12345678901.0);
        TEST_EQUAL(uint64_t(x), 12345678901);

        TRY(x = 0ull);      TEST(x.fits_in<int8_t>());    TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 127ull);    TEST(x.fits_in<int8_t>());    TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 128ull);    TEST(! x.fits_in<int8_t>());  TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 32767ull);  TEST(! x.fits_in<int8_t>());  TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 32768ull);  TEST(! x.fits_in<int8_t>());  TEST(! x.fits_in<int16_t>());  TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());

        TRY(x = 0ull);      TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), 0);       TEST_EQUAL(int32_t(x), 0);      TEST_EQUAL(int64_t(x), 0ll);
        TRY(x = 127ull);    TEST_EQUAL(int8_t(x), 127);   TEST_EQUAL(int16_t(x), 127);     TEST_EQUAL(int32_t(x), 127);    TEST_EQUAL(int64_t(x), 127ll);
        TRY(x = 128ull);    TEST_EQUAL(int8_t(x), -128);  TEST_EQUAL(int16_t(x), 128);     TEST_EQUAL(int32_t(x), 128);    TEST_EQUAL(int64_t(x), 128ll);
        TRY(x = 32767ull);  TEST_EQUAL(int8_t(x), -1);    TEST_EQUAL(int16_t(x), 32767);   TEST_EQUAL(int32_t(x), 32767);  TEST_EQUAL(int64_t(x), 32767ll);
        TRY(x = 32768ull);  TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), -32768);  TEST_EQUAL(int32_t(x), 32768);  TEST_EQUAL(int64_t(x), 32768ll);

    }

}

void test_core_fixed_binary_large_binary() {

    {

        using binary = LargeBinary<35>;

        static constexpr binary c(42);
        binary x, y, z;

        TEST_EQUAL(uint64_t(binary::max()), 0x7ffffffff_u64);
        TEST_EQUAL(binary::max().as_hex(), "7ffffffff");

        TEST(c.fits_in<int64_t>());  TEST_EQUAL(int64_t(c), 42);  TEST(c);
        TEST(x.fits_in<int64_t>());  TEST_EQUAL(int64_t(x), 0);   TEST(! x);

        TRY(x = binary(0x123456789_u64));
        TRY(y = binary(0xabcdef_u64));

        TEST_EQUAL(int64_t(x), 4886718345);
        TEST_EQUAL(int64_t(y), 11259375);
        TEST_EQUAL(int64_t(z), 0);

        TEST_EQUAL(x.as_binary(), "00100100011010001010110011110001001");
        TEST_EQUAL(y.as_binary(), "00000000000101010111100110111101111");
        TEST_EQUAL(z.as_binary(), "00000000000000000000000000000000000");
        TEST_EQUAL(x.as_hex(), "123456789");
        TEST_EQUAL(y.as_hex(), "000abcdef");
        TEST_EQUAL(z.as_hex(), "000000000");
        TEST_EQUAL(x.as_decimal(), "4886718345");
        TEST_EQUAL(y.as_decimal(), "11259375");
        TEST_EQUAL(z.as_decimal(), "0");
        TEST_EQUAL(x.as_double(), 4886718345.0);
        TEST_EQUAL(y.as_double(), 11259375.0);
        TEST_EQUAL(z.as_double(), 0.0);

        TEST_EQUAL(x.significant_bits(), 33);
        TEST_EQUAL(y.significant_bits(), 24);
        TEST_EQUAL(z.significant_bits(), 0);

        TRY(z = - z);  TEST_EQUAL(uint64_t(z), 0);
        TRY(z = - x);  TEST_EQUAL(uint64_t(z), 0x6dcba9877_u64);
        TRY(z = - y);  TEST_EQUAL(uint64_t(z), 0x7ff543211_u64);

        TRY(z.clear());
        TEST(! z);
        TEST_EQUAL(z, binary());

        TRY(z = ~ z);  TEST_EQUAL(uint64_t(z), 0x7ffffffff_u64);
        TRY(z = ~ x);  TEST_EQUAL(uint64_t(z), 0x6dcba9876_u64);
        TRY(z = ~ y);  TEST_EQUAL(uint64_t(z), 0x7ff543210_u64);

        TRY(z = x + y);  TEST_EQUAL(uint64_t(z), 0x123f13578_u64);
        TRY(z = x - y);  TEST_EQUAL(uint64_t(z), 0x12299999a_u64);
        TRY(z = y - x);  TEST_EQUAL(uint64_t(z), 0x6dd666666_u64);
        TRY(z = x * y);  TEST_EQUAL(uint64_t(z), 0x2aa375de7_u64);
        TRY(z = x / y);  TEST_EQUAL(uint64_t(z), 0x0000001b2_u64);
        TRY(z = x % y);  TEST_EQUAL(uint64_t(z), 0x00002485b_u64);
        TRY(z = y / x);  TEST_EQUAL(uint64_t(z), 0);
        TRY(z = y % x);  TEST_EQUAL(uint64_t(z), 0x000abcdef_u64);
        TRY(z = x & y);  TEST_EQUAL(uint64_t(z), 0x000014589_u64);
        TRY(z = x | y);  TEST_EQUAL(uint64_t(z), 0x123efefef_u64);
        TRY(z = x ^ y);  TEST_EQUAL(uint64_t(z), 0x123eeaa66_u64);

        TRY(z = x << 0);   TEST_EQUAL(uint64_t(z), 0x123456789_u64);  TRY(z = x >> 0);   TEST_EQUAL(uint64_t(z), 0x123456789_u64);
        TRY(z = x << 5);   TEST_EQUAL(uint64_t(z), 0x468acf120_u64);  TRY(z = x >> 5);   TEST_EQUAL(uint64_t(z), 0x0091a2b3c_u64);
        TRY(z = x << 10);  TEST_EQUAL(uint64_t(z), 0x5159e2400_u64);  TRY(z = x >> 10);  TEST_EQUAL(uint64_t(z), 0x00048d159_u64);
        TRY(z = x << 15);  TEST_EQUAL(uint64_t(z), 0x2b3c48000_u64);  TRY(z = x >> 15);  TEST_EQUAL(uint64_t(z), 0x00002468a_u64);
        TRY(z = x << 20);  TEST_EQUAL(uint64_t(z), 0x678900000_u64);  TRY(z = x >> 20);  TEST_EQUAL(uint64_t(z), 0x000001234_u64);
        TRY(z = x << 25);  TEST_EQUAL(uint64_t(z), 0x712000000_u64);  TRY(z = x >> 25);  TEST_EQUAL(uint64_t(z), 0x000000091_u64);
        TRY(z = x << 30);  TEST_EQUAL(uint64_t(z), 0x240000000_u64);  TRY(z = x >> 30);  TEST_EQUAL(uint64_t(z), 0x000000004_u64);
        TRY(z = x << 35);  TEST_EQUAL(uint64_t(z), 0);                TRY(z = x >> 35);  TEST_EQUAL(uint64_t(z), 0);

        TEST(++x);    TEST_EQUAL(x.as_hex(), "12345678a");
        TEST(--x);    TEST_EQUAL(x.as_hex(), "123456789");
        TEST(x--);    TEST_EQUAL(x.as_hex(), "123456788");
        TEST(x++);    TEST_EQUAL(x.as_hex(), "123456789");
        TEST(++y);    TEST_EQUAL(y.as_hex(), "000abcdf0");
        TEST(--y);    TEST_EQUAL(y.as_hex(), "000abcdef");
        TEST(y--);    TEST_EQUAL(y.as_hex(), "000abcdee");
        TEST(y++);    TEST_EQUAL(y.as_hex(), "000abcdef");
        TEST(++z);    TEST_EQUAL(z.as_hex(), "000000001");
        TEST(! --z);  TEST_EQUAL(z.as_hex(), "000000000");
        TEST(! z--);  TEST_EQUAL(z.as_hex(), "7ffffffff");
        TEST(z++);    TEST_EQUAL(z.as_hex(), "000000000");

        TEST_COMPARE(x, !=, y);
        TEST_COMPARE(x, >, y);
        TEST_COMPARE(x, >=, y);
        TEST_COMPARE(y, !=, x);
        TEST_COMPARE(y, <, x);
        TEST_COMPARE(y, <=, x);

        TEST_EQUAL(x.as_decimal(), "4886718345");
        TEST_EQUAL(y.as_decimal(), "11259375");
        TEST_EQUAL(z.as_decimal(), "0");

        TRY(x = rotl(y, 0));   TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotl(y, 5));   TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotl(y, 10));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotl(y, 15));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotl(y, 20));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotl(y, 25));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotl(y, 30));  TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotl(y, 35));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotl(y, 40));  TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotl(y, 45));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotl(y, 50));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotl(y, 55));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotl(y, 60));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotl(y, 65));  TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotl(y, 70));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);

        TRY(x = rotr(y, 0));   TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotr(y, 5));   TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotr(y, 10));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotr(y, 15));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotr(y, 20));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotr(y, 25));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotr(y, 30));  TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotr(y, 35));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);
        TRY(x = rotr(y, 40));  TEST_EQUAL(uint64_t(x), 0x3c0055e6f_u64);
        TRY(x = rotr(y, 45));  TEST_EQUAL(uint64_t(x), 0x3de002af3_u64);
        TRY(x = rotr(y, 50));  TEST_EQUAL(uint64_t(x), 0x4def00157_u64);
        TRY(x = rotr(y, 55));  TEST_EQUAL(uint64_t(x), 0x5e6f7800a_u64);
        TRY(x = rotr(y, 60));  TEST_EQUAL(uint64_t(x), 0x2af37bc00_u64);
        TRY(x = rotr(y, 65));  TEST_EQUAL(uint64_t(x), 0x01579bde0_u64);
        TRY(x = rotr(y, 70));  TEST_EQUAL(uint64_t(x), 0x000abcdef_u64);

        TRY(x = binary::from_double(12345678901.0));
        TEST_EQUAL(x.as_decimal(), "12345678901");
        TEST_EQUAL(x.as_double(), 12345678901.0);
        TEST_EQUAL(uint64_t(x), 12345678901);

        TRY(x = 0ull);      TEST(x.fits_in<int8_t>());    TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 127ull);    TEST(x.fits_in<int8_t>());    TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 128ull);    TEST(! x.fits_in<int8_t>());  TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 32767ull);  TEST(! x.fits_in<int8_t>());  TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 32768ull);  TEST(! x.fits_in<int8_t>());  TEST(! x.fits_in<int16_t>());  TEST(x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());

        TRY(x = 0ull);      TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), 0);       TEST_EQUAL(int32_t(x), 0);      TEST_EQUAL(int64_t(x), 0ll);
        TRY(x = 127ull);    TEST_EQUAL(int8_t(x), 127);   TEST_EQUAL(int16_t(x), 127);     TEST_EQUAL(int32_t(x), 127);    TEST_EQUAL(int64_t(x), 127ll);
        TRY(x = 128ull);    TEST_EQUAL(int8_t(x), -128);  TEST_EQUAL(int16_t(x), 128);     TEST_EQUAL(int32_t(x), 128);    TEST_EQUAL(int64_t(x), 128ll);
        TRY(x = 32767ull);  TEST_EQUAL(int8_t(x), -1);    TEST_EQUAL(int16_t(x), 32767);   TEST_EQUAL(int32_t(x), 32767);  TEST_EQUAL(int64_t(x), 32767ll);
        TRY(x = 32768ull);  TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), -32768);  TEST_EQUAL(int32_t(x), 32768);  TEST_EQUAL(int64_t(x), 32768ll);

    }

    {

        using binary = LargeBinary<100>;

        static constexpr binary c(42);
        binary x, y, z;

        TEST_EQUAL(binary::max().as_hex(), "fffffffffffffffffffffffff");

        TEST(c.fits_in<int64_t>());  TEST_EQUAL(int64_t(c), 42);  TEST(c);
        TEST(x.fits_in<int64_t>());  TEST_EQUAL(int64_t(x), 0);   TEST(! x);

        TRY((x = binary{0xfedcba987_u64, 0x6543210fedcba987_u64}));
        TRY((y = binary{0x123456789_u64, 0xabcdef0123456789_u64}));

        TEST_EQUAL(uint64_t(x), 0x6543210fedcba987ull);
        TEST_EQUAL(uint64_t(y), 0xabcdef0123456789ull);
        TEST_EQUAL(uint64_t(z), 0);

        TEST_EQUAL(x.as_binary(), "1111111011011100101110101001100001110110010101000011001000010000111111101101110010111010100110000111");
        TEST_EQUAL(y.as_binary(), "0001001000110100010101100111100010011010101111001101111011110000000100100011010001010110011110001001");
        TEST_EQUAL(z.as_binary(), "0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
        TEST_EQUAL(x.as_hex(), "fedcba9876543210fedcba987");
        TEST_EQUAL(y.as_hex(), "123456789abcdef0123456789");
        TEST_EQUAL(z.as_hex(), "0000000000000000000000000");
        TEST_EQUAL(x.as_decimal(), "1262016597560548382007796410759");
        TEST_EQUAL(y.as_decimal(), "90144042682896311822508713865");
        TEST_EQUAL(z.as_decimal(), "0");
        TEST_NEAR_EPSILON(x.as_double(), 1.262016598e30, 1e21);
        TEST_NEAR_EPSILON(y.as_double(), 9.014404268e28, 1e19);
        TEST_EQUAL(z.as_double(), 0.0);

        TEST_EQUAL(x.significant_bits(), 100);
        TEST_EQUAL(y.significant_bits(), 97);
        TEST_EQUAL(z.significant_bits(), 0);

        TRY(z = - z);  TEST_EQUAL(z.as_hex(), "0000000000000000000000000");
        TRY(z = - x);  TEST_EQUAL(z.as_hex(), "0123456789abcdef012345679");
        TRY(z = - y);  TEST_EQUAL(z.as_hex(), "edcba9876543210fedcba9877");

        TRY(z.clear());
        TEST(! z);
        TEST_EQUAL(z, binary());

        TRY(z = ~ z);  TEST_EQUAL(z.as_hex(), "fffffffffffffffffffffffff");
        TRY(z = ~ x);  TEST_EQUAL(z.as_hex(), "0123456789abcdef012345678");
        TRY(z = ~ y);  TEST_EQUAL(z.as_hex(), "edcba9876543210fedcba9876");

        TRY(z = x + y);  TEST_EQUAL(z.as_hex(), "1111111111111101111111110");
        TRY(z = x - y);  TEST_EQUAL(z.as_hex(), "eca8641fdb975320eca8641fe");
        TRY(z = y - x);  TEST_EQUAL(z.as_hex(), "13579be02468acdf13579be02");
        TRY(z = x * y);  TEST_EQUAL(z.as_hex(), "9efd92c744933bccc59960a3f");
        TRY(z = x / y);  TEST_EQUAL(z.as_hex(), "000000000000000000000000e");
        TRY(z = x % y);  TEST_EQUAL(z.as_hex(), "00000000000000f0000000009");
        TRY(z = y / x);  TEST_EQUAL(z.as_hex(), "0000000000000000000000000");
        TRY(z = y % x);  TEST_EQUAL(z.as_hex(), "123456789abcdef0123456789");
        TRY(z = x & y);  TEST_EQUAL(z.as_hex(), "1214121812141210121412181");
        TRY(z = x | y);  TEST_EQUAL(z.as_hex(), "fefcfef8fefcfef0fefcfef8f");
        TRY(z = x ^ y);  TEST_EQUAL(z.as_hex(), "ece8ece0ece8ece0ece8ece0e");

        TRY(z = x << 0);    TEST_EQUAL(z.as_hex(), "fedcba9876543210fedcba987");  TRY(z = x >> 0);    TEST_EQUAL(z.as_hex(), "fedcba9876543210fedcba987");
        TRY(z = x << 10);   TEST_EQUAL(z.as_hex(), "72ea61d950c843fb72ea61c00");  TRY(z = x >> 10);   TEST_EQUAL(z.as_hex(), "003fb72ea61d950c843fb72ea");
        TRY(z = x << 20);   TEST_EQUAL(z.as_hex(), "a9876543210fedcba98700000");  TRY(z = x >> 20);   TEST_EQUAL(z.as_hex(), "00000fedcba9876543210fedc");
        TRY(z = x << 30);   TEST_EQUAL(z.as_hex(), "1d950c843fb72ea61c0000000");  TRY(z = x >> 30);   TEST_EQUAL(z.as_hex(), "00000003fb72ea61d950c843f");
        TRY(z = x << 40);   TEST_EQUAL(z.as_hex(), "543210fedcba9870000000000");  TRY(z = x >> 40);   TEST_EQUAL(z.as_hex(), "0000000000fedcba987654321");
        TRY(z = x << 50);   TEST_EQUAL(z.as_hex(), "c843fb72ea61c000000000000");  TRY(z = x >> 50);   TEST_EQUAL(z.as_hex(), "0000000000003fb72ea61d950");
        TRY(z = x << 60);   TEST_EQUAL(z.as_hex(), "0fedcba987000000000000000");  TRY(z = x >> 60);   TEST_EQUAL(z.as_hex(), "000000000000000fedcba9876");
        TRY(z = x << 70);   TEST_EQUAL(z.as_hex(), "b72ea61c00000000000000000");  TRY(z = x >> 70);   TEST_EQUAL(z.as_hex(), "000000000000000003fb72ea6");
        TRY(z = x << 80);   TEST_EQUAL(z.as_hex(), "ba98700000000000000000000");  TRY(z = x >> 80);   TEST_EQUAL(z.as_hex(), "00000000000000000000fedcb");
        TRY(z = x << 90);   TEST_EQUAL(z.as_hex(), "61c0000000000000000000000");  TRY(z = x >> 90);   TEST_EQUAL(z.as_hex(), "00000000000000000000003fb");
        TRY(z = x << 100);  TEST_EQUAL(z.as_hex(), "0000000000000000000000000");  TRY(z = x >> 100);  TEST_EQUAL(z.as_hex(), "0000000000000000000000000");

        TEST(++x);    TEST_EQUAL(x.as_hex(), "fedcba9876543210fedcba988");
        TEST(--x);    TEST_EQUAL(x.as_hex(), "fedcba9876543210fedcba987");
        TEST(x--);    TEST_EQUAL(x.as_hex(), "fedcba9876543210fedcba986");
        TEST(x++);    TEST_EQUAL(x.as_hex(), "fedcba9876543210fedcba987");
        TEST(++y);    TEST_EQUAL(y.as_hex(), "123456789abcdef012345678a");
        TEST(--y);    TEST_EQUAL(y.as_hex(), "123456789abcdef0123456789");
        TEST(y--);    TEST_EQUAL(y.as_hex(), "123456789abcdef0123456788");
        TEST(y++);    TEST_EQUAL(y.as_hex(), "123456789abcdef0123456789");
        TEST(++z);    TEST_EQUAL(z.as_hex(), "0000000000000000000000001");
        TEST(! --z);  TEST_EQUAL(z.as_hex(), "0000000000000000000000000");
        TEST(! z--);  TEST_EQUAL(z.as_hex(), "fffffffffffffffffffffffff");
        TEST(z++);    TEST_EQUAL(z.as_hex(), "0000000000000000000000000");

        TEST_COMPARE(x, !=, y);
        TEST_COMPARE(x, >, y);
        TEST_COMPARE(x, >=, y);
        TEST_COMPARE(y, !=, x);
        TEST_COMPARE(y, <, x);
        TEST_COMPARE(y, <=, x);

        TEST_EQUAL(x.as_decimal(), "1262016597560548382007796410759");
        TEST_EQUAL(y.as_decimal(), "90144042682896311822508713865");
        TEST_EQUAL(z.as_decimal(), "0");

        TRY(x = rotl(y, 0));    TEST_EQUAL(x.as_hex(), "123456789abcdef0123456789");
        TRY(x = rotl(y, 10));   TEST_EQUAL(x.as_hex(), "d159e26af37bc048d159e2448");
        TRY(x = rotl(y, 20));   TEST_EQUAL(x.as_hex(), "6789abcdef012345678912345");
        TRY(x = rotl(y, 30));   TEST_EQUAL(x.as_hex(), "26af37bc048d159e2448d159e");
        TRY(x = rotl(y, 40));   TEST_EQUAL(x.as_hex(), "bcdef0123456789123456789a");
        TRY(x = rotl(y, 50));   TEST_EQUAL(x.as_hex(), "7bc048d159e2448d159e26af3");
        TRY(x = rotl(y, 60));   TEST_EQUAL(x.as_hex(), "0123456789123456789abcdef");
        TRY(x = rotl(y, 70));   TEST_EQUAL(x.as_hex(), "8d159e2448d159e26af37bc04");
        TRY(x = rotl(y, 80));   TEST_EQUAL(x.as_hex(), "56789123456789abcdef01234");
        TRY(x = rotl(y, 90));   TEST_EQUAL(x.as_hex(), "e2448d159e26af37bc048d159");
        TRY(x = rotl(y, 100));  TEST_EQUAL(x.as_hex(), "123456789abcdef0123456789");
        TRY(x = rotl(y, 110));  TEST_EQUAL(x.as_hex(), "d159e26af37bc048d159e2448");
        TRY(x = rotl(y, 120));  TEST_EQUAL(x.as_hex(), "6789abcdef012345678912345");
        TRY(x = rotl(y, 130));  TEST_EQUAL(x.as_hex(), "26af37bc048d159e2448d159e");
        TRY(x = rotl(y, 140));  TEST_EQUAL(x.as_hex(), "bcdef0123456789123456789a");
        TRY(x = rotl(y, 150));  TEST_EQUAL(x.as_hex(), "7bc048d159e2448d159e26af3");
        TRY(x = rotl(y, 160));  TEST_EQUAL(x.as_hex(), "0123456789123456789abcdef");
        TRY(x = rotl(y, 170));  TEST_EQUAL(x.as_hex(), "8d159e2448d159e26af37bc04");
        TRY(x = rotl(y, 180));  TEST_EQUAL(x.as_hex(), "56789123456789abcdef01234");
        TRY(x = rotl(y, 190));  TEST_EQUAL(x.as_hex(), "e2448d159e26af37bc048d159");
        TRY(x = rotl(y, 200));  TEST_EQUAL(x.as_hex(), "123456789abcdef0123456789");

        TRY(x = rotr(y, 0));    TEST_EQUAL(x.as_hex(), "123456789abcdef0123456789");
        TRY(x = rotr(y, 10));   TEST_EQUAL(x.as_hex(), "e2448d159e26af37bc048d159");
        TRY(x = rotr(y, 20));   TEST_EQUAL(x.as_hex(), "56789123456789abcdef01234");
        TRY(x = rotr(y, 30));   TEST_EQUAL(x.as_hex(), "8d159e2448d159e26af37bc04");
        TRY(x = rotr(y, 40));   TEST_EQUAL(x.as_hex(), "0123456789123456789abcdef");
        TRY(x = rotr(y, 50));   TEST_EQUAL(x.as_hex(), "7bc048d159e2448d159e26af3");
        TRY(x = rotr(y, 60));   TEST_EQUAL(x.as_hex(), "bcdef0123456789123456789a");
        TRY(x = rotr(y, 70));   TEST_EQUAL(x.as_hex(), "26af37bc048d159e2448d159e");
        TRY(x = rotr(y, 80));   TEST_EQUAL(x.as_hex(), "6789abcdef012345678912345");
        TRY(x = rotr(y, 90));   TEST_EQUAL(x.as_hex(), "d159e26af37bc048d159e2448");
        TRY(x = rotr(y, 100));  TEST_EQUAL(x.as_hex(), "123456789abcdef0123456789");
        TRY(x = rotr(y, 110));  TEST_EQUAL(x.as_hex(), "e2448d159e26af37bc048d159");
        TRY(x = rotr(y, 120));  TEST_EQUAL(x.as_hex(), "56789123456789abcdef01234");
        TRY(x = rotr(y, 130));  TEST_EQUAL(x.as_hex(), "8d159e2448d159e26af37bc04");
        TRY(x = rotr(y, 140));  TEST_EQUAL(x.as_hex(), "0123456789123456789abcdef");
        TRY(x = rotr(y, 150));  TEST_EQUAL(x.as_hex(), "7bc048d159e2448d159e26af3");
        TRY(x = rotr(y, 160));  TEST_EQUAL(x.as_hex(), "bcdef0123456789123456789a");
        TRY(x = rotr(y, 170));  TEST_EQUAL(x.as_hex(), "26af37bc048d159e2448d159e");
        TRY(x = rotr(y, 180));  TEST_EQUAL(x.as_hex(), "6789abcdef012345678912345");
        TRY(x = rotr(y, 190));  TEST_EQUAL(x.as_hex(), "d159e26af37bc048d159e2448");
        TRY(x = rotr(y, 200));  TEST_EQUAL(x.as_hex(), "123456789abcdef0123456789");

        TRY(x = binary::from_double(1.23456789e30));
        TEST_MATCH(x.as_decimal(), "^12345678\\d{23}$");
        TEST_NEAR_EPSILON(x.as_double(), 1.23456789e30, 1e21);

        TRY(x = 0ull);                    TEST(x.fits_in<int8_t>());    TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());    TEST(x.fits_in<int64_t>());
        TRY(x = 127ull);                  TEST(x.fits_in<int8_t>());    TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());    TEST(x.fits_in<int64_t>());
        TRY(x = 128ull);                  TEST(! x.fits_in<int8_t>());  TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());    TEST(x.fits_in<int64_t>());
        TRY(x = 32767ull);                TEST(! x.fits_in<int8_t>());  TEST(x.fits_in<int16_t>());    TEST(x.fits_in<int32_t>());    TEST(x.fits_in<int64_t>());
        TRY(x = 32768ull);                TEST(! x.fits_in<int8_t>());  TEST(! x.fits_in<int16_t>());  TEST(x.fits_in<int32_t>());    TEST(x.fits_in<int64_t>());
        TRY(x = 2147483647ull);           TEST(! x.fits_in<int8_t>());  TEST(! x.fits_in<int16_t>());  TEST(x.fits_in<int32_t>());    TEST(x.fits_in<int64_t>());
        TRY(x = 2147483648ull);           TEST(! x.fits_in<int8_t>());  TEST(! x.fits_in<int16_t>());  TEST(! x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 9223372036854775807ull);  TEST(! x.fits_in<int8_t>());  TEST(! x.fits_in<int16_t>());  TEST(! x.fits_in<int32_t>());  TEST(x.fits_in<int64_t>());
        TRY(x = 9223372036854775808ull);  TEST(! x.fits_in<int8_t>());  TEST(! x.fits_in<int16_t>());  TEST(! x.fits_in<int32_t>());  TEST(! x.fits_in<int64_t>());

        TRY(x = 0ull);                    TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), 0);       TEST_EQUAL(int32_t(x), 0);          TEST_EQUAL(int64_t(x), 0);
        TRY(x = 127ull);                  TEST_EQUAL(int8_t(x), 127);   TEST_EQUAL(int16_t(x), 127);     TEST_EQUAL(int32_t(x), 127);        TEST_EQUAL(int64_t(x), 127);
        TRY(x = 128ull);                  TEST_EQUAL(int8_t(x), -128);  TEST_EQUAL(int16_t(x), 128);     TEST_EQUAL(int32_t(x), 128);        TEST_EQUAL(int64_t(x), 128);
        TRY(x = 32767ull);                TEST_EQUAL(int8_t(x), -1);    TEST_EQUAL(int16_t(x), 32767);   TEST_EQUAL(int32_t(x), 32767);      TEST_EQUAL(int64_t(x), 32767);
        TRY(x = 32768ull);                TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), -32768);  TEST_EQUAL(int32_t(x), 32768);      TEST_EQUAL(int64_t(x), 32768);
        TRY(x = 2147483647ull);           TEST_EQUAL(int8_t(x), -1);    TEST_EQUAL(int16_t(x), -1);      TEST_EQUAL(int32_t(x), max_int32);  TEST_EQUAL(int64_t(x), 2147483647);
        TRY(x = 2147483648ull);           TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), 0);       TEST_EQUAL(int32_t(x), min_int32);  TEST_EQUAL(int64_t(x), 2147483648);
        TRY(x = 9223372036854775807ull);  TEST_EQUAL(int8_t(x), -1);    TEST_EQUAL(int16_t(x), -1);      TEST_EQUAL(int32_t(x), -1);         TEST_EQUAL(int64_t(x), max_int64);
        TRY(x = 9223372036854775808ull);  TEST_EQUAL(int8_t(x), 0);     TEST_EQUAL(int16_t(x), 0);       TEST_EQUAL(int32_t(x), 0);          TEST_EQUAL(int64_t(x), min_int64);

    }

}

void test_core_fixed_binary_random_numbers() {

    using SB5 = SmallBinary<5>;
    using SB35 = SmallBinary<35>;
    using LB35 = LargeBinary<35>;
    using LB100 = LargeBinary<100>;

    static constexpr int iterations = 10000;

    std::mt19937 rng(42);
    Statistics<double> stats;

    for (int i = 0; i < iterations; ++i)
        TRY(stats.add(SB5::random(rng).as_double()));
    TEST_COMPARE(stats.min(), >=, 0);
    TEST_COMPARE(stats.max(), <=, 31);
    TEST_NEAR_EPSILON(stats.mean(), 15.5, 0.2);
    TEST_NEAR_EPSILON(stats.stdevs(), 9.24, 0.1);

    stats.clear();
    for (int i = 0; i < iterations; ++i)
        TRY(stats.add(SB35::random(rng).as_double()));
    TEST_COMPARE(stats.min(), >=, 0);
    TEST_COMPARE(stats.max(), <=, 0x7ffffffff_u64);
    TEST_NEAR_EPSILON(stats.mean(), 1.72e10, 2e8);
    TEST_NEAR_EPSILON(stats.stdevs(), 9.92e9, 1e8);

    stats.clear();
    for (int i = 0; i < iterations; ++i)
        TRY(stats.add(LB35::random(rng).as_double()));
    TEST_COMPARE(stats.min(), >=, 0);
    TEST_COMPARE(stats.max(), <=, 0x7ffffffff_u64);
    TEST_NEAR_EPSILON(stats.mean(), 1.72e10, 2e8);
    TEST_NEAR_EPSILON(stats.stdevs(), 9.92e9, 1e8);

    stats.clear();
    for (int i = 0; i < iterations; ++i)
        TRY(stats.add(LB100::random(rng).as_double()));
    TEST_COMPARE(stats.min(), >=, 0);
    TEST_COMPARE(stats.max(), <=, 1.267650600e30);
    TEST_NEAR_EPSILON(stats.mean(), 6.34e29, 1e28);
    TEST_NEAR_EPSILON(stats.stdevs(), 3.66e29, 5e27);

    stats.clear();
    SB35 sb1 = SB35::from_double(1e9);
    SB35 sb2 = SB35::from_double(2e9);
    for (int i = 0; i < iterations; ++i)
        TRY(stats.add(SB35::random(rng, sb1, sb2).as_double()));
    TEST_COMPARE(stats.min(), >=, 1e9);
    TEST_COMPARE(stats.max(), <=, 2e9);
    TEST_NEAR_EPSILON(stats.mean(), 1.5e9, 1e7);
    TEST_NEAR_EPSILON(stats.stdevs(), 2.89e8, 2e6);

    stats.clear();
    LB35 lb1 = LB35::from_double(1e9);
    LB35 lb2 = LB35::from_double(2e9);
    for (int i = 0; i < iterations; ++i)
        TRY(stats.add(LB35::random(rng, lb1, lb2).as_double()));
    TEST_COMPARE(stats.min(), >=, 1e9);
    TEST_COMPARE(stats.max(), <=, 2e9);
    TEST_NEAR_EPSILON(stats.mean(), 1.5e9, 1e7);
    TEST_NEAR_EPSILON(stats.stdevs(), 2.89e8, 2e6);

}

namespace {

    template <typename T>
    void do_random_arithmetic_tests() {

        static constexpr int iterations = 10000;
        static constexpr auto mask = uint64_t(T::max());

        std::mt19937 rng(42);

        uint64_t u, v, w;
        T x, y, z;

        for (int i = 0; i < iterations; ++i) {

            TRY(x = T::random(rng, T(1), T(mask)));  TRY(u = uint64_t(x));
            TRY(y = T::random(rng, T(1), T(mask)));  TRY(v = uint64_t(y));

            TRY(z = x + y);  w = (u + v) & mask;  TEST_EQUAL(uint64_t(z), w);
            TRY(z = x - y);  w = (u - v) & mask;  TEST_EQUAL(uint64_t(z), w);
            TRY(z = x * y);  w = (u * v) & mask;  TEST_EQUAL(uint64_t(z), w);
            TRY(z = x / y);  w = (u / v) & mask;  TEST_EQUAL(uint64_t(z), w);
            TRY(z = x % y);  w = (u % v) & mask;  TEST_EQUAL(uint64_t(z), w);

        }

    }

}

void test_core_fixed_binary_random_arithmetic_tests() {

    do_random_arithmetic_tests<SmallBinary<5>>();
    do_random_arithmetic_tests<SmallBinary<10>>();
    do_random_arithmetic_tests<SmallBinary<15>>();
    do_random_arithmetic_tests<SmallBinary<20>>();
    do_random_arithmetic_tests<SmallBinary<25>>();
    do_random_arithmetic_tests<SmallBinary<30>>();
    do_random_arithmetic_tests<SmallBinary<35>>();
    do_random_arithmetic_tests<SmallBinary<40>>();
    do_random_arithmetic_tests<SmallBinary<45>>();
    do_random_arithmetic_tests<SmallBinary<50>>();
    do_random_arithmetic_tests<SmallBinary<55>>();
    do_random_arithmetic_tests<SmallBinary<60>>();
    do_random_arithmetic_tests<SmallBinary<64>>();

    do_random_arithmetic_tests<LargeBinary<5>>();
    do_random_arithmetic_tests<LargeBinary<10>>();
    do_random_arithmetic_tests<LargeBinary<15>>();
    do_random_arithmetic_tests<LargeBinary<20>>();
    do_random_arithmetic_tests<LargeBinary<25>>();
    do_random_arithmetic_tests<LargeBinary<30>>();
    do_random_arithmetic_tests<LargeBinary<35>>();
    do_random_arithmetic_tests<LargeBinary<40>>();
    do_random_arithmetic_tests<LargeBinary<45>>();
    do_random_arithmetic_tests<LargeBinary<50>>();
    do_random_arithmetic_tests<LargeBinary<55>>();
    do_random_arithmetic_tests<LargeBinary<60>>();
    do_random_arithmetic_tests<LargeBinary<64>>();

}

namespace {

    template <typename T1, typename T2>
    void do_conversion_tests() {

        static constexpr uint64_t ca = 0x0123456789abcdef_u64;
        static constexpr uint64_t cb = ~ ca;
        static constexpr uint64_t mask1 = uint64_t(T1::max());
        static constexpr uint64_t mask2 = uint64_t(T2::max());

        T1 x1, y1, z1;
        T2 x2, y2, z2;

        TRY(x1 = static_cast<T1>(ca));  TEST_EQUAL(uint64_t(x1), ca & mask1);
        TRY(y2 = static_cast<T2>(x1));  TEST_EQUAL(uint64_t(y2), ca & mask1 & mask2);
        TRY(z1 = static_cast<T1>(y2));  TEST_EQUAL(uint64_t(z1), uint64_t(y2));
        TRY(x2 = static_cast<T2>(ca));  TEST_EQUAL(uint64_t(x2), ca & mask2);
        TRY(y1 = static_cast<T1>(x2));  TEST_EQUAL(uint64_t(y1), ca & mask2 & mask1);
        TRY(z2 = static_cast<T2>(y1));  TEST_EQUAL(uint64_t(z2), uint64_t(y1));
        TRY(x1 = static_cast<T1>(cb));  TEST_EQUAL(uint64_t(x1), cb & mask1);
        TRY(y2 = static_cast<T2>(x1));  TEST_EQUAL(uint64_t(y2), cb & mask1 & mask2);
        TRY(z1 = static_cast<T1>(y2));  TEST_EQUAL(uint64_t(z1), uint64_t(y2));
        TRY(x2 = static_cast<T2>(cb));  TEST_EQUAL(uint64_t(x2), cb & mask2);
        TRY(y1 = static_cast<T1>(x2));  TEST_EQUAL(uint64_t(y1), cb & mask2 & mask1);
        TRY(z2 = static_cast<T2>(y1));  TEST_EQUAL(uint64_t(z2), uint64_t(y1));

    }

}

void test_core_fixed_binary_type_conversions() {

    do_conversion_tests<SmallBinary<10>, SmallBinary<31>>();
    do_conversion_tests<SmallBinary<10>, SmallBinary<32>>();
    do_conversion_tests<SmallBinary<10>, SmallBinary<33>>();
    do_conversion_tests<SmallBinary<10>, SmallBinary<63>>();
    do_conversion_tests<SmallBinary<10>, SmallBinary<64>>();
    do_conversion_tests<SmallBinary<10>, LargeBinary<31>>();
    do_conversion_tests<SmallBinary<10>, LargeBinary<32>>();
    do_conversion_tests<SmallBinary<10>, LargeBinary<33>>();
    do_conversion_tests<SmallBinary<10>, LargeBinary<63>>();
    do_conversion_tests<SmallBinary<10>, LargeBinary<64>>();
    do_conversion_tests<LargeBinary<10>, SmallBinary<31>>();
    do_conversion_tests<LargeBinary<10>, SmallBinary<32>>();
    do_conversion_tests<LargeBinary<10>, SmallBinary<33>>();
    do_conversion_tests<LargeBinary<10>, SmallBinary<63>>();
    do_conversion_tests<LargeBinary<10>, SmallBinary<64>>();
    do_conversion_tests<LargeBinary<10>, LargeBinary<31>>();
    do_conversion_tests<LargeBinary<10>, LargeBinary<32>>();
    do_conversion_tests<LargeBinary<10>, LargeBinary<33>>();
    do_conversion_tests<LargeBinary<10>, LargeBinary<63>>();
    do_conversion_tests<LargeBinary<10>, LargeBinary<64>>();

}

void test_core_fixed_binary_string_parsing() {

    SmallBinary<35> x;
    LargeBinary<35> y;
    size_t n = 0;

    TRY(n = x.parse(""));                       TEST_EQUAL(n, 0);   TEST_EQUAL(uint64_t(x), 0);
    TRY(n = x.parse("a"));                      TEST_EQUAL(n, 0);   TEST_EQUAL(uint64_t(x), 0);
    TRY(n = x.parse("0"));                      TEST_EQUAL(n, 1);   TEST_EQUAL(uint64_t(x), 0);
    TRY(n = x.parse("42"));                     TEST_EQUAL(n, 2);   TEST_EQUAL(uint64_t(x), 42);
    TRY(n = x.parse("1234567890"));             TEST_EQUAL(n, 10);  TEST_EQUAL(uint64_t(x), 1234567890_u64);
    TRY(n = x.parse("123456789", 16));          TEST_EQUAL(n, 9);   TEST_EQUAL(uint64_t(x), 0x123456789_u64);
    TRY(n = x.parse("123456789xyz", 16));       TEST_EQUAL(n, 9);   TEST_EQUAL(uint64_t(x), 0x123456789_u64);
    TRY(n = x.parse("101010110011001100", 2));  TEST_EQUAL(n, 18);  TEST_EQUAL(uint64_t(x), 0x00002accc_u64);

    TRY(n = y.parse(""));                       TEST_EQUAL(n, 0);   TEST_EQUAL(uint64_t(y), 0);
    TRY(n = y.parse("a"));                      TEST_EQUAL(n, 0);   TEST_EQUAL(uint64_t(y), 0);
    TRY(n = y.parse("0"));                      TEST_EQUAL(n, 1);   TEST_EQUAL(uint64_t(y), 0);
    TRY(n = y.parse("42"));                     TEST_EQUAL(n, 2);   TEST_EQUAL(uint64_t(y), 42);
    TRY(n = y.parse("1234567890"));             TEST_EQUAL(n, 10);  TEST_EQUAL(uint64_t(y), 1234567890_u64);
    TRY(n = y.parse("123456789", 16));          TEST_EQUAL(n, 9);   TEST_EQUAL(uint64_t(y), 0x123456789_u64);
    TRY(n = y.parse("123456789xyz", 16));       TEST_EQUAL(n, 9);   TEST_EQUAL(uint64_t(y), 0x123456789_u64);
    TRY(n = y.parse("101010110011001100", 2));  TEST_EQUAL(n, 18);  TEST_EQUAL(uint64_t(y), 0x00002accc_u64);

}
