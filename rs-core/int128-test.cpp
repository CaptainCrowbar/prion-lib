#include "rs-core/int128.hpp"
#include "rs-core/unit-test.hpp"
#include <stdexcept>

using namespace RS;
using namespace RS::Literals;

void test_core_int128_uint128_basics() {

    Uint128 x, y;

    TEST(! x);
    TEST_COMPARE(x, ==, y);
    TEST_COMPARE(x, <=, y);
    TEST_COMPARE(x, >=, y);
    TEST_EQUAL(hex(x), "00000000000000000000000000000000");

    TRY(x = 0);
    TEST(! x);
    TEST_EQUAL(hex(x), "00000000000000000000000000000000");

    TRY(x = 42);
    TEST(bool(x));
    TEST_COMPARE(x, !=, y);
    TEST_COMPARE(x, >, y);
    TEST_COMPARE(x, >=, y);
    TEST_EQUAL(hex(x), "0000000000000000000000000000002a");

    TRY(x = 0x123456789abcdef0_u64);
    TEST(bool(x));
    TEST_EQUAL(hex(x), "0000000000000000123456789abcdef0");

    TRY((x = {0x123456789abcdef0_u64, 0x0fedcba987654321_u64}));
    TEST(bool(x));
    TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654321");

    TRY((x = {0x123456789abcdef0_u64, 0}));
    TEST(bool(x));
    TEST_EQUAL(hex(x), "123456789abcdef00000000000000000");

}

void test_core_int128_uint128_arithmetic() {

    Uint128 x, y, z;

    TRY((x = {0x1234512345123451_u64, 0x2345123451234512_u64}));  TEST_EQUAL(hex(x), "12345123451234512345123451234512");
    TRY((y = {0xabcdefabcdefabcd_u64, 0xefabcdefabcdefab_u64}));  TEST_EQUAL(hex(y), "abcdefabcdefabcdefabcdefabcdefab");

    TRY(z = ~ x);    TEST_EQUAL(hex(z), "edcbaedcbaedcbaedcbaedcbaedcbaed");
    TRY(z = ~ y);    TEST_EQUAL(hex(z), "54321054321054321054321054321054");
    TRY(z = x + y);  TEST_EQUAL(hex(z), "be0240cf1301e01f12f0e023fcf134bd");
    TRY(z = x - y);  TEST_EQUAL(hex(z), "666661777722888333994444a5555567");
    TRY(z = y - x);  TEST_EQUAL(hex(z), "99999e8888dd777ccc66bbbb5aaaaa99");
    TRY(z = x * y);  TEST_EQUAL(hex(z), "05d5ea0be57a6cde807c6ee17574f106");
    TRY(z = x / y);  TEST_EQUAL(hex(z), "00000000000000000000000000000000");
    TRY(z = x % y);  TEST_EQUAL(hex(z), "12345123451234512345123451234512");
    TRY(z = y / x);  TEST_EQUAL(hex(z), "00000000000000000000000000000009");
    TRY(z = y % x);  TEST_EQUAL(hex(z), "07f7156e604bd4f3b23e2a18d1908209");
    TRY(z = x & y);  TEST_EQUAL(hex(z), "02044123450220412301002401014502");
    TRY(z = x | y);  TEST_EQUAL(hex(z), "bbfdffabcdffbfddefefdffffbefefbb");
    TRY(z = x ^ y);  TEST_EQUAL(hex(z), "b9f9be8888fd9f9ccceedfdbfaeeaab9");

    TRY((x = {0x123456789abcdef0_u64, 0x0fedcba987654321_u64}));  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654321");

    TRY(z = x << 0);    TEST_EQUAL(hex(z), "123456789abcdef00fedcba987654321");
    TRY(z = x << 1);    TEST_EQUAL(hex(z), "2468acf13579bde01fdb97530eca8642");
    TRY(z = x << 2);    TEST_EQUAL(hex(z), "48d159e26af37bc03fb72ea61d950c84");
    TRY(z = x << 3);    TEST_EQUAL(hex(z), "91a2b3c4d5e6f7807f6e5d4c3b2a1908");
    TRY(z = x << 4);    TEST_EQUAL(hex(z), "23456789abcdef00fedcba9876543210");
    TRY(z = x << 5);    TEST_EQUAL(hex(z), "468acf13579bde01fdb97530eca86420");
    TRY(z = x << 6);    TEST_EQUAL(hex(z), "8d159e26af37bc03fb72ea61d950c840");
    TRY(z = x << 7);    TEST_EQUAL(hex(z), "1a2b3c4d5e6f7807f6e5d4c3b2a19080");
    TRY(z = x << 8);    TEST_EQUAL(hex(z), "3456789abcdef00fedcba98765432100");
    TRY(z = x << 9);    TEST_EQUAL(hex(z), "68acf13579bde01fdb97530eca864200");
    TRY(z = x << 10);   TEST_EQUAL(hex(z), "d159e26af37bc03fb72ea61d950c8400");
    TRY(z = x << 120);  TEST_EQUAL(hex(z), "21000000000000000000000000000000");
    TRY(z = x << 121);  TEST_EQUAL(hex(z), "42000000000000000000000000000000");
    TRY(z = x << 122);  TEST_EQUAL(hex(z), "84000000000000000000000000000000");
    TRY(z = x << 123);  TEST_EQUAL(hex(z), "08000000000000000000000000000000");
    TRY(z = x << 124);  TEST_EQUAL(hex(z), "10000000000000000000000000000000");
    TRY(z = x << 125);  TEST_EQUAL(hex(z), "20000000000000000000000000000000");
    TRY(z = x << 126);  TEST_EQUAL(hex(z), "40000000000000000000000000000000");
    TRY(z = x << 127);  TEST_EQUAL(hex(z), "80000000000000000000000000000000");
    TRY(z = x << 128);  TEST_EQUAL(hex(z), "00000000000000000000000000000000");

    TRY(z = x >> 0);    TEST_EQUAL(hex(z), "123456789abcdef00fedcba987654321");
    TRY(z = x >> 1);    TEST_EQUAL(hex(z), "091a2b3c4d5e6f7807f6e5d4c3b2a190");
    TRY(z = x >> 2);    TEST_EQUAL(hex(z), "048d159e26af37bc03fb72ea61d950c8");
    TRY(z = x >> 3);    TEST_EQUAL(hex(z), "02468acf13579bde01fdb97530eca864");
    TRY(z = x >> 4);    TEST_EQUAL(hex(z), "0123456789abcdef00fedcba98765432");
    TRY(z = x >> 5);    TEST_EQUAL(hex(z), "0091a2b3c4d5e6f7807f6e5d4c3b2a19");
    TRY(z = x >> 6);    TEST_EQUAL(hex(z), "0048d159e26af37bc03fb72ea61d950c");
    TRY(z = x >> 7);    TEST_EQUAL(hex(z), "002468acf13579bde01fdb97530eca86");
    TRY(z = x >> 8);    TEST_EQUAL(hex(z), "00123456789abcdef00fedcba9876543");
    TRY(z = x >> 9);    TEST_EQUAL(hex(z), "00091a2b3c4d5e6f7807f6e5d4c3b2a1");
    TRY(z = x >> 10);   TEST_EQUAL(hex(z), "00048d159e26af37bc03fb72ea61d950");
    TRY(z = x >> 120);  TEST_EQUAL(hex(z), "00000000000000000000000000000012");
    TRY(z = x >> 121);  TEST_EQUAL(hex(z), "00000000000000000000000000000009");
    TRY(z = x >> 122);  TEST_EQUAL(hex(z), "00000000000000000000000000000004");
    TRY(z = x >> 123);  TEST_EQUAL(hex(z), "00000000000000000000000000000002");
    TRY(z = x >> 124);  TEST_EQUAL(hex(z), "00000000000000000000000000000001");
    TRY(z = x >> 125);  TEST_EQUAL(hex(z), "00000000000000000000000000000000");
    TRY(z = x >> 126);  TEST_EQUAL(hex(z), "00000000000000000000000000000000");
    TRY(z = x >> 127);  TEST_EQUAL(hex(z), "00000000000000000000000000000000");
    TRY(z = x >> 128);  TEST_EQUAL(hex(z), "00000000000000000000000000000000");

    TRY(++x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654322");
    TRY(++x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654323");
    TRY(--x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654322");
    TRY(--x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654321");
    TRY(--x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654320");
    TRY(--x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba98765431f");
    TRY(++x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654320");
    TRY(++x);  TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654321");

    TRY(x = 0);

    TRY(++x);  TEST_EQUAL(hex(x), "00000000000000000000000000000001");
    TRY(++x);  TEST_EQUAL(hex(x), "00000000000000000000000000000002");
    TRY(--x);  TEST_EQUAL(hex(x), "00000000000000000000000000000001");
    TRY(--x);  TEST_EQUAL(hex(x), "00000000000000000000000000000000");
    TRY(--x);  TEST_EQUAL(hex(x), "ffffffffffffffffffffffffffffffff");
    TRY(--x);  TEST_EQUAL(hex(x), "fffffffffffffffffffffffffffffffe");
    TRY(++x);  TEST_EQUAL(hex(x), "ffffffffffffffffffffffffffffffff");
    TRY(++x);  TEST_EQUAL(hex(x), "00000000000000000000000000000000");

}

void test_core_int128_uint128_conversion() {

    Uint128 x;

    TEST_EQUAL(bin(x), "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    TEST_EQUAL(dec(x), "0");
    TEST_EQUAL(hex(x), "00000000000000000000000000000000");
    TEST_EQUAL(dec(x, 0), "");
    TEST_EQUAL(dec(x, 1), "0");
    TEST_EQUAL(dec(x, 2), "00");
    TEST_EQUAL(dec(x, 3), "000");
    TEST_EQUAL(dec(x, 4), "0000");
    TEST_EQUAL(dec(x, 5), "00000");

    TRY(x = 42);
    TEST_EQUAL(bin(x), "00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000101010");
    TEST_EQUAL(dec(x), "42");
    TEST_EQUAL(hex(x), "0000000000000000000000000000002a");
    TEST_EQUAL(dec(x, 0), "42");
    TEST_EQUAL(dec(x, 1), "42");
    TEST_EQUAL(dec(x, 2), "42");
    TEST_EQUAL(dec(x, 3), "042");
    TEST_EQUAL(dec(x, 4), "0042");
    TEST_EQUAL(dec(x, 5), "00042");

    TRY(x = 0x123456789abcdef0_u64);
    TEST_EQUAL(bin(x), "00000000000000000000000000000000000000000000000000000000000000000001001000110100010101100111100010011010101111001101111011110000");
    TEST_EQUAL(dec(x), "1311768467463790320");
    TEST_EQUAL(hex(x), "0000000000000000123456789abcdef0");

    TRY((x = {0x123456789abcdef0_u64, 0x0fedcba987654321_u64}));
    TEST_EQUAL(bin(x), "00010010001101000101011001111000100110101011110011011110111100000000111111101101110010111010100110000111011001010100001100100001");
    TEST_EQUAL(dec(x), "24197857203266734864629346612071973665");
    TEST_EQUAL(hex(x), "123456789abcdef00fedcba987654321");

    TRY((x = {0x123456789abcdef0_u64, 0}));
    TEST_EQUAL(bin(x), "00010010001101000101011001111000100110101011110011011110111100000000000000000000000000000000000000000000000000000000000000000000");
    TEST_EQUAL(dec(x), "24197857203266734863481549203041157120");
    TEST_EQUAL(hex(x), "123456789abcdef00000000000000000");

    TRY((x = {0xffffffffffffffff_u64, 0xffffffffffffffff_u64}));
    TEST_EQUAL(bin(x), "11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111");
    TEST_EQUAL(dec(x), "340282366920938463463374607431768211455");
    TEST_EQUAL(hex(x), "ffffffffffffffffffffffffffffffff");

    TRY(x = Uint128("0"));                                         TEST_EQUAL(dec(x), "0");
    TRY(x = Uint128("42"));                                        TEST_EQUAL(dec(x), "42");
    TRY(x = Uint128("0000000000000000000000000000000000000042"));  TEST_EQUAL(dec(x), "42");
    TRY(x = Uint128("123456789"));                                 TEST_EQUAL(dec(x), "123456789");
    TRY(x = Uint128("123456789123456789"));                        TEST_EQUAL(dec(x), "123456789123456789");
    TRY(x = Uint128("123456789123456789123456789"));               TEST_EQUAL(dec(x), "123456789123456789123456789");
    TRY(x = Uint128("123456789123456789123456789123456789"));      TEST_EQUAL(dec(x), "123456789123456789123456789123456789");
    TRY(x = Uint128("340282366920938463463374607431768211455"));   TEST_EQUAL(dec(x), "340282366920938463463374607431768211455");

    TRY(x = Uint128("0"));                                         TEST_EQUAL(hex(x), "00000000000000000000000000000000");
    TRY(x = Uint128("42"));                                        TEST_EQUAL(hex(x), "0000000000000000000000000000002a");
    TRY(x = Uint128("0000000000000000000000000000000000000042"));  TEST_EQUAL(hex(x), "0000000000000000000000000000002a");
    TRY(x = Uint128("123456789"));                                 TEST_EQUAL(hex(x), "000000000000000000000000075bcd15");
    TRY(x = Uint128("123456789123456789"));                        TEST_EQUAL(hex(x), "000000000000000001b69b4bacd05f15");
    TRY(x = Uint128("123456789123456789123456789"));               TEST_EQUAL(hex(x), "0000000000661efdf2e3b19f7c045f15");
    TRY(x = Uint128("123456789123456789123456789123456789"));      TEST_EQUAL(hex(x), "0017c6e3c032f89045ad746684045f15");
    TRY(x = Uint128("340282366920938463463374607431768211455"));   TEST_EQUAL(hex(x), "ffffffffffffffffffffffffffffffff");

    TEST_THROW(Uint128(""), std::invalid_argument);
    TEST_THROW(Uint128("abc"), std::invalid_argument);
    TEST_THROW(Uint128("2", 2), std::invalid_argument);
    TEST_THROW(Uint128("42", 1), std::invalid_argument);
    TEST_THROW(Uint128("42", 36), std::invalid_argument);

}

void test_core_int128_uint128_literals() {

    Uint128 x;

    TRY(x = 0_u128);                                         TEST_EQUAL(dec(x), "0");
    TRY(x = 42_u128);                                        TEST_EQUAL(dec(x), "42");
    TRY(x = 0000000000000000000000000000000000000042_u128);  TEST_EQUAL(dec(x), "42");
    TRY(x = 123456789_u128);                                 TEST_EQUAL(dec(x), "123456789");
    TRY(x = 123456789123456789_u128);                        TEST_EQUAL(dec(x), "123456789123456789");
    TRY(x = 123456789123456789123456789_u128);               TEST_EQUAL(dec(x), "123456789123456789123456789");
    TRY(x = 123456789123456789123456789123456789_u128);      TEST_EQUAL(dec(x), "123456789123456789123456789123456789");
    TRY(x = 340282366920938463463374607431768211455_u128);   TEST_EQUAL(dec(x), "340282366920938463463374607431768211455");

    TRY(x = 0b0_u128);                                 TEST_EQUAL(hex(x), "00000000000000000000000000000000");
    TRY(x = 0b10000000000000000000000000000001_u128);  TEST_EQUAL(hex(x), "00000000000000000000000080000001");
    TRY(x = 0b11111111111111111111111111111111_u128);  TEST_EQUAL(hex(x), "000000000000000000000000ffffffff");

    TRY(x = 0x0_u128);                                 TEST_EQUAL(hex(x), "00000000000000000000000000000000");
    TRY(x = 0x123456789abcdef0123456789abcdef0_u128);  TEST_EQUAL(hex(x), "123456789abcdef0123456789abcdef0");
    TRY(x = 0xfffffffffffffffffffffffffffffff0_u128);  TEST_EQUAL(hex(x), "fffffffffffffffffffffffffffffff0");

}

void test_core_int128_uint128_numeric_limits() {

    using limits = std::numeric_limits<Uint128>;

    constexpr uint64_t max64 = 0xffff'ffff'ffff'ffffull;

    TEST(limits::is_specialized);
    TEST(limits::is_bounded);
    TEST(limits::is_integer);
    TEST(! limits::is_signed);
    TEST_EQUAL(limits::digits, 128);
    TEST_EQUAL(limits::digits10, 38);
    TEST_EQUAL(limits::min(), Uint128(0));
    TEST_EQUAL(limits::max(), Uint128(max64, max64));

}
