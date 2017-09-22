#include "rs-core/mp-integer.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <random>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace RS;

namespace {

    using ByteVec = std::vector<uint8_t>;

    U8string hexv(const ByteVec& v) { return hexdump(v.data(), v.size()); }

    void check_natural_conversion() {

        Nat x;
        U8string s;

        TEST_EQUAL(uint64_t(x), 0);
        TEST_EQUAL(double(x), 0);
        TRY(s = x.str(2));   TEST_EQUAL(s, "0");
        TRY(s = x.str(10));  TEST_EQUAL(s, "0");
        TRY(s = x.str(16));  TEST_EQUAL(s, "0");
        TRY(s = x.str(36));  TEST_EQUAL(s, "0");

        TRY(x = 0x12345678ul);
        TEST_EQUAL(x.bits(), 29);
        TEST_EQUAL(uint64_t(x), 0x12345678ul);
        TEST_EQUAL(double(x), 305419896);
        TRY(s = x.str(2));   TEST_EQUAL(s, "10010001101000101011001111000");
        TRY(s = x.str(10));  TEST_EQUAL(s, "305419896");
        TRY(s = x.str(16));  TEST_EQUAL(s, "12345678");
        TRY(s = x.str(36));  TEST_EQUAL(s, "51u7i0");

        TRY(x = 0x123456789abcdef0ull);
        TEST_EQUAL(x.bits(), 61);
        TEST_EQUAL(uint64_t(x), 0x123456789abcdef0ull);
        TEST_NEAR_EPSILON(double(x), 1.311768e18, 1e12);
        TRY(s = x.str(2));   TEST_EQUAL(s, "1001000110100010101100111100010011010101111001101111011110000");
        TRY(s = x.str(10));  TEST_EQUAL(s, "1311768467463790320");
        TRY(s = x.str(16));  TEST_EQUAL(s, "123456789abcdef0");
        TRY(s = x.str(36));  TEST_EQUAL(s, "9ys742pps3qo");

        TRY(x = Nat("110011000011110111111011111001011100011101100011001111101111100000001000101111100010101", 2));
        TEST_NEAR_EPSILON(double(x), 1.234568e26, 1e20);
        TRY(s = x.str(2));   TEST_EQUAL(s, "110011000011110111111011111001011100011101100011001111101111100000001000101111100010101");
        TRY(s = x.str(10));  TEST_EQUAL(s, "123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "661efdf2e3b19f7c045f15");

        TRY(x = Nat("123456789123456789123456789", 10));
        TEST_NEAR_EPSILON(double(x), 1.234568e26, 1e20);
        TRY(s = x.str(2));   TEST_EQUAL(s, "110011000011110111111011111001011100011101100011001111101111100000001000101111100010101");
        TRY(s = x.str(10));  TEST_EQUAL(s, "123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "661efdf2e3b19f7c045f15");

        TRY(x = Nat("661efdf2e3b19f7c045f15", 16));
        TEST_NEAR_EPSILON(double(x), 1.234568e26, 1e20);
        TRY(s = x.str(2));   TEST_EQUAL(s, "110011000011110111111011111001011100011101100011001111101111100000001000101111100010101");
        TRY(s = x.str(10));  TEST_EQUAL(s, "123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "661efdf2e3b19f7c045f15");

        TRY(x = Nat("123456789123456789123456789", 0));
        TEST_NEAR_EPSILON(double(x), 1.234568e26, 1e20);
        TRY(s = x.str(2));   TEST_EQUAL(s, "110011000011110111111011111001011100011101100011001111101111100000001000101111100010101");
        TRY(s = x.str(10));  TEST_EQUAL(s, "123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "661efdf2e3b19f7c045f15");

        TRY(x = Nat("0x661efdf2e3b19f7c045f15", 0));
        TEST_NEAR_EPSILON(double(x), 1.234568e26, 1e20);
        TRY(s = x.str(2));   TEST_EQUAL(s, "110011000011110111111011111001011100011101100011001111101111100000001000101111100010101");
        TRY(s = x.str(10));  TEST_EQUAL(s, "123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "661efdf2e3b19f7c045f15");

        TRY(x = Nat::from_double(0.0));            TRY(s = x.str());  TEST_EQUAL(s, "0");
        TRY(x = Nat::from_double(0.99));           TRY(s = x.str());  TEST_EQUAL(s, "0");
        TRY(x = Nat::from_double(1.0));            TRY(s = x.str());  TEST_EQUAL(s, "1");
        TRY(x = Nat::from_double(123456789.0));    TRY(s = x.str());  TEST_EQUAL(s, "123456789");
        TRY(x = Nat::from_double(1.23456789e40));  TRY(s = x.str());  TEST_MATCH(s, "^12345678\\d{33}$");

    }

    void check_natural_arithmetic() {

        Nat x, y, z, q, r;
        U8string s;

        TRY(x = 0);
        TEST_EQUAL(x.bits(), 0);
        TRY(s = x.str(2));   TEST_EQUAL(s, "0");
        TRY(s = x.str(10));  TEST_EQUAL(s, "0");
        TRY(s = x.str(16));  TEST_EQUAL(s, "0");

        TRY(y = x + 15);
        TEST_EQUAL(y.bits(), 4);
        TRY(s = to_str(y));  TEST_EQUAL(s, "15");
        TRY(s = y.str(16));  TEST_EQUAL(s, "f");

        TRY(y = 15 - x);
        TEST_EQUAL(y.bits(), 4);
        TRY(s = to_str(y));  TEST_EQUAL(s, "15");
        TRY(s = y.str(16));  TEST_EQUAL(s, "f");

        TRY(x = 0x123456789abcdef0ull);
        TRY(y = 0xffffffffffffffffull);
        TEST_EQUAL(x.bits(), 61);
        TEST_EQUAL(y.bits(), 64);
        TRY(s = to_str(x));  TEST_EQUAL(s, "1311768467463790320");
        TRY(s = to_str(y));  TEST_EQUAL(s, "18446744073709551615");

        TRY(z = x + 15);
        TEST_EQUAL(z.bits(), 61);
        TRY(s = to_str(z));  TEST_EQUAL(s, "1311768467463790335");
        TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdeff");

        TRY(z = x + y);
        TEST_EQUAL(z.bits(), 65);
        TRY(s = to_str(z));  TEST_EQUAL(s, "19758512541173341935");
        TRY(s = z.str(16));  TEST_EQUAL(s, "1123456789abcdeef");

        TRY(z = y - 15);
        TEST_EQUAL(z.bits(), 64);
        TRY(s = to_str(z));  TEST_EQUAL(s, "18446744073709551600");
        TRY(s = z.str(16));  TEST_EQUAL(s, "fffffffffffffff0");

        TRY(z = y - x);
        TEST_EQUAL(z.bits(), 64);
        TRY(s = to_str(z));  TEST_EQUAL(s, "17134975606245761295");
        TRY(s = z.str(16));  TEST_EQUAL(s, "edcba9876543210f");

        TRY(z = x * y);
        TEST_EQUAL(z.bits(), 125);
        TRY(s = to_str(z));  TEST_EQUAL(s, "24197857203266734862169780735577366800");
        TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdeefedcba98765432110");

        TRY(x = Nat("123456789123456789123456789123456789123456789"));
        TRY(y = Nat("123456789123456789123456789123456789"));
        TRY(z = x - y);
        TEST_EQUAL(z, Nat("123456789000000000000000000000000000000000000"));
        TRY(y = Nat("123456789123456789123456789123456789000000000"));
        TRY(z = x - y);
        TEST_EQUAL(z, Nat("123456789"));
        TRY(x = Nat("123456789123456789123456789123456789123456789"));
        TRY(y = Nat("1357913579135791357913579"));
        TRY(z = x - y);
        TEST_EQUAL(z, Nat("123456789123456789122098875544320997765543210"));

        TRY(x = Nat("123456789123456789123456789123456789123456789"));
        TRY(y = Nat("123456789"));
        TRY(q = x / y);
        TRY(r = x % y);
        TEST_EQUAL(q, Nat("1000000001000000001000000001000000001"));
        TEST_EQUAL(r, Nat("0"));
        TRY(y = Nat("987654321"));
        TRY(q = x / y);
        TRY(r = x % y);
        TEST_EQUAL(q, Nat("124999998985937499000175780249997801"));
        TEST_EQUAL(r, Nat("725308668"));
        TRY(y = Nat("987654321987654321987654321987654321987654321"));
        TRY(q = x / y);
        TRY(r = x % y);
        TEST_EQUAL(q, Nat("0"));
        TEST_EQUAL(r, Nat("123456789123456789123456789123456789123456789"));
        TRY(y = {});
        TEST_THROW(x / y, std::domain_error);
        TEST_THROW(x % y, std::domain_error);

        TRY(x = 0);   TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = 0);   TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "0");
        TRY(x = 0);   TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "0");
        TRY(x = 0);   TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "0");
        TRY(x = 1);   TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = 1);   TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "1");
        TRY(x = 1);   TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "1");
        TRY(x = 1);   TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "1");
        TRY(x = 10);  TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = 10);  TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "10");
        TRY(x = 10);  TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "100");
        TRY(x = 10);  TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "1000");
        TRY(x = 10);  TRY(y = x.pow(4));   TEST_EQUAL(y.str(), "10000");
        TRY(x = 10);  TRY(y = x.pow(5));   TEST_EQUAL(y.str(), "100000");
        TRY(x = 10);  TRY(y = x.pow(6));   TEST_EQUAL(y.str(), "1000000");
        TRY(x = 10);  TRY(y = x.pow(7));   TEST_EQUAL(y.str(), "10000000");
        TRY(x = 10);  TRY(y = x.pow(8));   TEST_EQUAL(y.str(), "100000000");
        TRY(x = 10);  TRY(y = x.pow(9));   TEST_EQUAL(y.str(), "1000000000");
        TRY(x = 10);  TRY(y = x.pow(10));  TEST_EQUAL(y.str(), "10000000000");
        TRY(x = 10);  TRY(y = x.pow(11));  TEST_EQUAL(y.str(), "100000000000");
        TRY(x = 10);  TRY(y = x.pow(12));  TEST_EQUAL(y.str(), "1000000000000");
        TRY(x = 10);  TRY(y = x.pow(13));  TEST_EQUAL(y.str(), "10000000000000");
        TRY(x = 10);  TRY(y = x.pow(14));  TEST_EQUAL(y.str(), "100000000000000");
        TRY(x = 10);  TRY(y = x.pow(15));  TEST_EQUAL(y.str(), "1000000000000000");
        TRY(x = 10);  TRY(y = x.pow(16));  TEST_EQUAL(y.str(), "10000000000000000");
        TRY(x = 10);  TRY(y = x.pow(17));  TEST_EQUAL(y.str(), "100000000000000000");
        TRY(x = 10);  TRY(y = x.pow(18));  TEST_EQUAL(y.str(), "1000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(19));  TEST_EQUAL(y.str(), "10000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(20));  TEST_EQUAL(y.str(), "100000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(21));  TEST_EQUAL(y.str(), "1000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(22));  TEST_EQUAL(y.str(), "10000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(23));  TEST_EQUAL(y.str(), "100000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(24));  TEST_EQUAL(y.str(), "1000000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(25));  TEST_EQUAL(y.str(), "10000000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(26));  TEST_EQUAL(y.str(), "100000000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(27));  TEST_EQUAL(y.str(), "1000000000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(28));  TEST_EQUAL(y.str(), "10000000000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(29));  TEST_EQUAL(y.str(), "100000000000000000000000000000");
        TRY(x = 10);  TRY(y = x.pow(30));  TEST_EQUAL(y.str(), "1000000000000000000000000000000");

    }

    void check_natural_bit_operations() {

        Nat x, y, z;
        U8string s;

        TEST_EQUAL(x.bits_set(), 0);
        TEST(x.is_even());
        TEST(! x.is_odd());

        TRY(x = 0x123456789abcdef0ull);
        TRY(y = 0xffffffffffffffffull);
        TEST_EQUAL(x.bits(), 61);
        TEST_EQUAL(y.bits(), 64);
        TEST_EQUAL(x.bits_set(), 32);
        TEST_EQUAL(y.bits_set(), 64);
        TEST(x.is_even());
        TEST(! x.is_odd());
        TEST(! y.is_even());
        TEST(y.is_odd());
        TRY(s = to_str(x));  TEST_EQUAL(s, "1311768467463790320");
        TRY(s = to_str(y));  TEST_EQUAL(s, "18446744073709551615");

        TRY(z = x & y);
        TEST_EQUAL(z.bits(), 61);
        TRY(s = to_str(z));  TEST_EQUAL(s, "1311768467463790320");
        TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef0");

        TRY(z = x | y);
        TEST_EQUAL(z.bits(), 64);
        TRY(s = to_str(z));  TEST_EQUAL(s, "18446744073709551615");
        TRY(s = z.str(16));  TEST_EQUAL(s, "ffffffffffffffff");

        TRY(z = x ^ y);
        TEST_EQUAL(z.bits(), 64);
        TRY(s = to_str(z));  TEST_EQUAL(s, "17134975606245761295");
        TRY(s = z.str(16));  TEST_EQUAL(s, "edcba9876543210f");

        TRY(z = x >> 0);    TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef0");
        TRY(z = x >> 1);    TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f78");
        TRY(z = x >> 2);    TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc");
        TRY(z = x >> 3);    TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde");
        TRY(z = x >> 31);   TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf1");
        TRY(z = x >> 32);   TRY(s = z.str(16));  TEST_EQUAL(s, "12345678");
        TRY(z = x >> 33);   TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c");
        TRY(z = x >> 58);   TRY(s = z.str(16));  TEST_EQUAL(s, "4");
        TRY(z = x >> 59);   TRY(s = z.str(16));  TEST_EQUAL(s, "2");
        TRY(z = x >> 60);   TRY(s = z.str(16));  TEST_EQUAL(s, "1");
        TRY(z = x >> 61);   TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x >> 62);   TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x >> 63);   TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x >> 64);   TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x >> 65);   TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x >> -1);   TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde0");
        TRY(z = x >> -2);   TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc0");
        TRY(z = x >> -3);   TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f780");
        TRY(z = x >> -4);   TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef00");
        TRY(z = x >> -5);   TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde00");
        TRY(z = x >> -6);   TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc00");
        TRY(z = x >> -7);   TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f7800");
        TRY(z = x >> -8);   TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef000");
        TRY(z = x >> -9);   TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde000");
        TRY(z = x >> -10);  TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc000");
        TRY(z = x >> -11);  TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f78000");
        TRY(z = x >> -12);  TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef0000");
        TRY(z = x >> -13);  TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde0000");
        TRY(z = x >> -14);  TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc0000");
        TRY(z = x >> -15);  TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f780000");
        TRY(z = x >> -16);  TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef00000");
        TRY(z = x >> -17);  TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde00000");
        TRY(z = x >> -18);  TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc00000");
        TRY(z = x >> -19);  TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f7800000");
        TRY(z = x >> -20);  TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef000000");

        TRY(z = x << 0);    TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef0");
        TRY(z = x << 1);    TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde0");
        TRY(z = x << 2);    TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc0");
        TRY(z = x << 3);    TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f780");
        TRY(z = x << 4);    TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef00");
        TRY(z = x << 5);    TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde00");
        TRY(z = x << 6);    TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc00");
        TRY(z = x << 7);    TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f7800");
        TRY(z = x << 8);    TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef000");
        TRY(z = x << 9);    TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde000");
        TRY(z = x << 10);   TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc000");
        TRY(z = x << 11);   TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f78000");
        TRY(z = x << 12);   TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef0000");
        TRY(z = x << 13);   TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde0000");
        TRY(z = x << 14);   TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc0000");
        TRY(z = x << 15);   TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f780000");
        TRY(z = x << 16);   TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef00000");
        TRY(z = x << 17);   TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde00000");
        TRY(z = x << 18);   TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc00000");
        TRY(z = x << 19);   TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f7800000");
        TRY(z = x << 20);   TRY(s = z.str(16));  TEST_EQUAL(s, "123456789abcdef000000");
        TRY(z = x << -1);   TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c4d5e6f78");
        TRY(z = x << -2);   TRY(s = z.str(16));  TEST_EQUAL(s, "48d159e26af37bc");
        TRY(z = x << -3);   TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf13579bde");
        TRY(z = x << -31);  TRY(s = z.str(16));  TEST_EQUAL(s, "2468acf1");
        TRY(z = x << -32);  TRY(s = z.str(16));  TEST_EQUAL(s, "12345678");
        TRY(z = x << -33);  TRY(s = z.str(16));  TEST_EQUAL(s, "91a2b3c");
        TRY(z = x << -58);  TRY(s = z.str(16));  TEST_EQUAL(s, "4");
        TRY(z = x << -59);  TRY(s = z.str(16));  TEST_EQUAL(s, "2");
        TRY(z = x << -60);  TRY(s = z.str(16));  TEST_EQUAL(s, "1");
        TRY(z = x << -61);  TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x << -62);  TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x << -63);  TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x << -64);  TRY(s = z.str(16));  TEST_EQUAL(s, "0");
        TRY(z = x << -65);  TRY(s = z.str(16));  TEST_EQUAL(s, "0");

        TRY(x = {});
        TEST(! x.get_bit(0));
        TEST(! x.get_bit(100));

        TRY(x.set_bit(16));
        TEST_EQUAL(x, Nat("0x10000"));
        TEST(! x.get_bit(15));
        TEST(x.get_bit(16));
        TEST(! x.get_bit(17));

        TRY(x.set_bit(80));
        TEST_EQUAL(x, Nat("0x100000000000000010000"));
        TEST(! x.get_bit(79));
        TEST(x.get_bit(80));
        TEST(! x.get_bit(81));

        TRY(x.set_bit(80, false));
        TEST_EQUAL(x, Nat("0x10000"));
        TEST(! x.get_bit(80));

        TRY(x.flip_bit(80));
        TEST_EQUAL(x, Nat("0x100000000000000010000"));
        TEST(x.get_bit(80));

        TRY(x.flip_bit(80));
        TEST_EQUAL(x, Nat("0x10000"));
        TEST(! x.get_bit(80));

    }

    void check_natural_byte_operations() {

        Nat a, b;
        std::vector<uint8_t> v;

        TEST_EQUAL(a.bytes(), 0);

        TRY(a = Nat("0x12"));                    TEST_EQUAL(a.bytes(), 1);
        TRY(a = Nat("0x1234"));                  TEST_EQUAL(a.bytes(), 2);
        TRY(a = Nat("0x123456"));                TEST_EQUAL(a.bytes(), 3);
        TRY(a = Nat("0x12345678"));              TEST_EQUAL(a.bytes(), 4);
        TRY(a = Nat("0x123456789a"));            TEST_EQUAL(a.bytes(), 5);
        TRY(a = Nat("0x123456789abc"));          TEST_EQUAL(a.bytes(), 6);
        TRY(a = Nat("0x123456789abcde"));        TEST_EQUAL(a.bytes(), 7);
        TRY(a = Nat("0x123456789abcdef1"));      TEST_EQUAL(a.bytes(), 8);
        TRY(a = Nat("0x123456789abcdef123"));    TEST_EQUAL(a.bytes(), 9);
        TRY(a = Nat("0x123456789abcdef12345"));  TEST_EQUAL(a.bytes(), 10);

        TEST_EQUAL(a.get_byte(0), 0x45);
        TEST_EQUAL(a.get_byte(1), 0x23);
        TEST_EQUAL(a.get_byte(2), 0xf1);
        TEST_EQUAL(a.get_byte(3), 0xde);
        TEST_EQUAL(a.get_byte(4), 0xbc);
        TEST_EQUAL(a.get_byte(5), 0x9a);
        TEST_EQUAL(a.get_byte(6), 0x78);
        TEST_EQUAL(a.get_byte(7), 0x56);
        TEST_EQUAL(a.get_byte(8), 0x34);
        TEST_EQUAL(a.get_byte(9), 0x12);
        TEST_EQUAL(a.get_byte(10), 0);
        TEST_EQUAL(a.get_byte(11), 0);
        TEST_EQUAL(a.get_byte(12), 0);
        TEST_EQUAL(a.get_byte(13), 0);
        TEST_EQUAL(a.get_byte(14), 0);
        TEST_EQUAL(a.get_byte(15), 0);
        TEST_EQUAL(a.get_byte(16), 0);

        TRY(a.set_byte(1, 0xff));   TEST_EQUAL(hex(a), "123456789abcdef1ff45");
        TRY(a.set_byte(3, 0xff));   TEST_EQUAL(hex(a), "123456789abcfff1ff45");
        TRY(a.set_byte(5, 0xff));   TEST_EQUAL(hex(a), "12345678ffbcfff1ff45");
        TRY(a.set_byte(7, 0xff));   TEST_EQUAL(hex(a), "1234ff78ffbcfff1ff45");
        TRY(a.set_byte(9, 0xff));   TEST_EQUAL(hex(a), "ff34ff78ffbcfff1ff45");
        TRY(a.set_byte(11, 0xff));  TEST_EQUAL(hex(a), "ff00ff34ff78ffbcfff1ff45");
        TRY(a.set_byte(13, 0xff));  TEST_EQUAL(hex(a), "ff00ff00ff34ff78ffbcfff1ff45");
        TRY(a.set_byte(15, 0xff));  TEST_EQUAL(hex(a), "ff00ff00ff00ff34ff78ffbcfff1ff45");

        TRY(a = 0);
        TRY(b = Nat("0x123456789abcdef12345"));

        v.resize(7);

        TRY(a.write_be(v.data(), v.size()));  TEST_EQUAL(hexv(v), "00 00 00 00 00 00 00");
        TRY(b.write_be(v.data(), v.size()));  TEST_EQUAL(hexv(v), "78 9a bc de f1 23 45");

        TRY(a.write_le(v.data(), v.size()));  TEST_EQUAL(hexv(v), "00 00 00 00 00 00 00");
        TRY(b.write_le(v.data(), v.size()));  TEST_EQUAL(hexv(v), "45 23 f1 de bc 9a 78");

        v = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};

        TRY(a = Nat::read_be(v.data(), v.size()));  TEST_EQUAL(hex(a), "112233445566778899aabbccddeeff");
        TRY(a = Nat::read_le(v.data(), v.size()));  TEST_EQUAL(hex(a), "ffeeddccbbaa998877665544332211");

    }

    void check_natural_random() {

        std::mt19937 rng(42);
        RandomNat random;
        Nat a, b, lo, hi, sum, sum2, x;
        int n = 10000;

        TRY(a = Nat("123456789123456789123456789123456789"));
        TRY(b = Nat("987654321987654321987654321987654321"));
        TRY(lo = Nat(1) << 1000);
        TRY(random = RandomNat(a, b));

        for (int i = 0; i < n; ++i) {
            TRY(x = random(rng));
            TRY(sum += x);
            TRY(sum2 += x * x);
            TRY(lo = std::min(lo, x));
            TRY(hi = std::max(hi, x));
        }

        TEST_COMPARE(lo, >=, a);
        TEST_COMPARE(hi, <=, b);

        double mean = double(sum) / n;
        double sd = sqrt(double(sum2) / n - mean * mean);

        TEST_NEAR_EPSILON(mean, 5.556e35, 2e34);
        TEST_NEAR_EPSILON(sd, 2.495e35, 2e34);

    }

    void check_integer_conversion() {

        Int x;
        U8string s;

        TEST_EQUAL(x.sign(), 0);
        TEST_EQUAL(int64_t(x), 0);
        TEST_EQUAL(double(x), 0);
        TEST(x.is_even());
        TEST(! x.is_odd());
        TRY(s = to_str(x));  TEST_EQUAL(s, "0");
        TRY(s = x.str(16));  TEST_EQUAL(s, "0");
        TRY(s = x.str(36));  TEST_EQUAL(s, "0");

        TRY(x = 123456789l);
        TEST_EQUAL(x.sign(), 1);
        TEST_EQUAL(int64_t(x), 123456789l);
        TEST_EQUAL(double(x), 123456789.0);
        TEST(! x.is_even());
        TEST(x.is_odd());
        TRY(s = to_str(x));  TEST_EQUAL(s, "123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "75bcd15");
        TRY(s = x.str(36));  TEST_EQUAL(s, "21i3v9");

        TRY(x = - 123456789l);
        TEST_EQUAL(x.sign(), -1);
        TEST_EQUAL(int64_t(x), -123456789l);
        TEST_EQUAL(double(x), -123456789.0);
        TEST(! x.is_even());
        TEST(x.is_odd());
        TRY(s = to_str(x));  TEST_EQUAL(s, "-123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "-75bcd15");
        TRY(s = x.str(36));  TEST_EQUAL(s, "-21i3v9");

        TRY(x = 123456789123456789ll);
        TEST_EQUAL(x.sign(), 1);
        TEST_EQUAL(int64_t(x), 123456789123456789ll);
        TEST_NEAR_EPSILON(double(x), 1.234568e17, 1e11);
        TEST(! x.is_even());
        TEST(x.is_odd());
        TRY(s = to_str(x));  TEST_EQUAL(s, "123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "1b69b4bacd05f15");
        TRY(s = x.str(36));  TEST_EQUAL(s, "xrls1yk9rf9");

        TRY(x = - 123456789123456789ll);
        TEST_EQUAL(x.sign(), -1);
        TEST_EQUAL(int64_t(x), -123456789123456789ll);
        TEST_NEAR_EPSILON(double(x), -1.234568e17, 1e11);
        TEST(! x.is_even());
        TEST(x.is_odd());
        TRY(s = to_str(x));  TEST_EQUAL(s, "-123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "-1b69b4bacd05f15");
        TRY(s = x.str(36));  TEST_EQUAL(s, "-xrls1yk9rf9");

        TRY(x = Int("123456789123456789123456789123456789123456789", 10));
        TEST_EQUAL(x.sign(), 1);
        TEST_NEAR_EPSILON(double(x), 1.234568e44, 1e38);
        TRY(s = to_str(x));  TEST_EQUAL(s, "123456789123456789123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "58936e53d139afefabb2683f150b684045f15");

        TRY(x = Int("123456789abcdef123456789abcdef123456789abcdef123456789abcdef", 16));
        TEST_EQUAL(x.sign(), 1);
        TEST_NEAR_EPSILON(double(x), 1.256425e71, 1e65);
        TRY(s = to_str(x));  TEST_EQUAL(s, "125642457939796217460094503631385345882379387509263401568735420576681455");
        TRY(s = x.str(16));  TEST_EQUAL(s, "123456789abcdef123456789abcdef123456789abcdef123456789abcdef");

        TRY(x = Int("-123456789123456789123456789123456789123456789", 10));
        TEST_EQUAL(x.sign(), -1);
        TEST_NEAR_EPSILON(double(x), -1.234568e44, 1e38);
        TRY(s = to_str(x));  TEST_EQUAL(s, "-123456789123456789123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "-58936e53d139afefabb2683f150b684045f15");

        TRY(x = Int("-123456789abcdef123456789abcdef123456789abcdef123456789abcdef", 16));
        TEST_EQUAL(x.sign(), -1);
        TEST_NEAR_EPSILON(double(x), -1.256425e71, 1e65);
        TRY(s = to_str(x));  TEST_EQUAL(s, "-125642457939796217460094503631385345882379387509263401568735420576681455");
        TRY(s = x.str(16));  TEST_EQUAL(s, "-123456789abcdef123456789abcdef123456789abcdef123456789abcdef");

        TRY(x = Int("123456789123456789123456789123456789123456789", 0));
        TEST_EQUAL(x.sign(), 1);
        TEST_NEAR_EPSILON(double(x), 1.234568e44, 1e38);
        TRY(s = to_str(x));  TEST_EQUAL(s, "123456789123456789123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "58936e53d139afefabb2683f150b684045f15");

        TRY(x = Int("0x123456789abcdef123456789abcdef123456789abcdef123456789abcdef", 0));
        TEST_EQUAL(x.sign(), 1);
        TEST_NEAR_EPSILON(double(x), 1.256425e71, 1e65);
        TRY(s = to_str(x));  TEST_EQUAL(s, "125642457939796217460094503631385345882379387509263401568735420576681455");
        TRY(s = x.str(16));  TEST_EQUAL(s, "123456789abcdef123456789abcdef123456789abcdef123456789abcdef");

        TRY(x = Int("-123456789123456789123456789123456789123456789", 0));
        TEST_EQUAL(x.sign(), -1);
        TEST_NEAR_EPSILON(double(x), -1.234568e44, 1e38);
        TRY(s = to_str(x));  TEST_EQUAL(s, "-123456789123456789123456789123456789123456789");
        TRY(s = x.str(16));  TEST_EQUAL(s, "-58936e53d139afefabb2683f150b684045f15");

        TRY(x = Int("-0x123456789abcdef123456789abcdef123456789abcdef123456789abcdef", 0));
        TEST_EQUAL(x.sign(), -1);
        TEST_NEAR_EPSILON(double(x), -1.256425e71, 1e65);
        TRY(s = to_str(x));  TEST_EQUAL(s, "-125642457939796217460094503631385345882379387509263401568735420576681455");
        TRY(s = x.str(16));  TEST_EQUAL(s, "-123456789abcdef123456789abcdef123456789abcdef123456789abcdef");

        TRY(x = Int::from_double(0.0));             TRY(s = x.str());  TEST_EQUAL(s, "0");
        TRY(x = Int::from_double(0.99));            TRY(s = x.str());  TEST_EQUAL(s, "0");
        TRY(x = Int::from_double(-0.99));           TRY(s = x.str());  TEST_EQUAL(s, "0");
        TRY(x = Int::from_double(1.0));             TRY(s = x.str());  TEST_EQUAL(s, "1");
        TRY(x = Int::from_double(-1.0));            TRY(s = x.str());  TEST_EQUAL(s, "-1");
        TRY(x = Int::from_double(123456789.0));     TRY(s = x.str());  TEST_EQUAL(s, "123456789");
        TRY(x = Int::from_double(-123456789.0));    TRY(s = x.str());  TEST_EQUAL(s, "-123456789");
        TRY(x = Int::from_double(1.23456789e40));   TRY(s = x.str());  TEST_MATCH(s, "^12345678\\d{33}$");
        TRY(x = Int::from_double(-1.23456789e40));  TRY(s = x.str());  TEST_MATCH(s, "^-12345678\\d{33}$");

    }

    void check_integer_arithmetic() {

        Int a, b, c, d, q, r, x, y, z;
        U8string s;

        TRY(z = a + b);  TRY(s = to_str(z));  TEST_EQUAL(s, "0");
        TRY(z = a - b);  TRY(s = to_str(z));  TEST_EQUAL(s, "0");
        TRY(z = a * b);  TRY(s = to_str(z));  TEST_EQUAL(s, "0");

        TEST_THROW(a / b, std::domain_error);
        TEST_THROW(a % b, std::domain_error);

        TRY(b = 10);

        TRY(z = a + b);  TRY(s = to_str(z));  TEST_EQUAL(s, "10");
        TRY(z = a - b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-10");
        TRY(z = b - a);  TRY(s = to_str(z));  TEST_EQUAL(s, "10");
        TRY(z = a * b);  TRY(s = to_str(z));  TEST_EQUAL(s, "0");
        TRY(z = a / b);  TRY(s = to_str(z));  TEST_EQUAL(s, "0");
        TRY(z = a % b);  TRY(s = to_str(z));  TEST_EQUAL(s, "0");

        TRY(a = Int("42"));
        TRY(b = Int("10"));
        TRY(c = - a);
        TRY(d = - b);

        TRY(z = a + b);  TRY(s = to_str(z));  TEST_EQUAL(s, "52");
        TRY(z = a - b);  TRY(s = to_str(z));  TEST_EQUAL(s, "32");
        TRY(z = b - a);  TRY(s = to_str(z));  TEST_EQUAL(s, "-32");
        TRY(z = a * b);  TRY(s = to_str(z));  TEST_EQUAL(s, "420");
        TRY(z = a / b);  TRY(s = to_str(z));  TEST_EQUAL(s, "4");
        TRY(z = a % b);  TRY(s = to_str(z));  TEST_EQUAL(s, "2");

        TRY(z = a + d);  TRY(s = to_str(z));  TEST_EQUAL(s, "32");
        TRY(z = a - d);  TRY(s = to_str(z));  TEST_EQUAL(s, "52");
        TRY(z = d - a);  TRY(s = to_str(z));  TEST_EQUAL(s, "-52");
        TRY(z = a * d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-420");
        TRY(z = a / d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-4");
        TRY(z = a % d);  TRY(s = to_str(z));  TEST_EQUAL(s, "2");

        TRY(z = c + b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-32");
        TRY(z = c - b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-52");
        TRY(z = b - c);  TRY(s = to_str(z));  TEST_EQUAL(s, "52");
        TRY(z = c * b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-420");
        TRY(z = c / b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-5");
        TRY(z = c % b);  TRY(s = to_str(z));  TEST_EQUAL(s, "8");

        TRY(z = c + d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-52");
        TRY(z = c - d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-32");
        TRY(z = d - c);  TRY(s = to_str(z));  TEST_EQUAL(s, "32");
        TRY(z = c * d);  TRY(s = to_str(z));  TEST_EQUAL(s, "420");
        TRY(z = c / d);  TRY(s = to_str(z));  TEST_EQUAL(s, "5");
        TRY(z = c % d);  TRY(s = to_str(z));  TEST_EQUAL(s, "8");

        TRY(z = a / 1);  TEST_EQUAL(z, a);
        TRY(z = b / 1);  TEST_EQUAL(z, b);
        TRY(z = c / 1);  TEST_EQUAL(z, c);
        TRY(z = d / 1);  TEST_EQUAL(z, d);

        TRY(a = -6);  TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -5);  TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -4);  TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = -3);  TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -2);  TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -1);  TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 0);   TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 1);   TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 2);   TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 3);   TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 4);   TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 5);   TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 6);   TRY(b = 3);   TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -6);  TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -5);  TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -4);  TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = -3);  TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -2);  TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -1);  TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 0);   TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 1);   TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 2);   TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 3);   TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 4);   TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 5);   TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 6);   TRY(b = -3);  TRY(q = a / b);  TRY(r = a % b);  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);

        TRY(a = -6);  TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -5);  TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -4);  TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = -3);  TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -2);  TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -1);  TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 0);   TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 1);   TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 2);   TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 3);   TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 4);   TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 5);   TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 6);   TRY(b = 3);   TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -6);  TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -5);  TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -4);  TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 2);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = -3);  TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = -2);  TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = -1);  TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 1);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 0);   TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 1);   TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 2);   TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, 0);   TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 3);   TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);
        TRY(a = 4);   TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 1);  TEST_EQUAL(b * q + r, a);
        TRY(a = 5);   TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -1);  TEST_EQUAL(r, 2);  TEST_EQUAL(b * q + r, a);
        TRY(a = 6);   TRY(b = -3);  TRY(q = quo(a, b));  TRY(r = rem(a, b));  TEST_EQUAL(q, -2);  TEST_EQUAL(r, 0);  TEST_EQUAL(b * q + r, a);

        TRY(a = Int("123456789123456789123456789123456789123456789"));
        TRY(b = Int("1357913579135791357913579"));
        TRY(c = - a);
        TRY(d = - b);

        TRY(z = a + b);  TRY(s = to_str(z));  TEST_EQUAL(s, "123456789123456789124814702702592580481370368");
        TRY(z = a - b);  TRY(s = to_str(z));  TEST_EQUAL(s, "123456789123456789122098875544320997765543210");
        TRY(z = b - a);  TRY(s = to_str(z));  TEST_EQUAL(s, "-123456789123456789122098875544320997765543210");
        TRY(z = a * b);  TRY(s = to_str(z));  TEST_EQUAL(s, "167643650387245846409206049247037049247037049079393398859791202837831");
        TRY(z = a / b);  TRY(s = to_str(z));  TEST_EQUAL(s, "90916528872203810704");
        TRY(z = a % b);  TRY(s = to_str(z));  TEST_EQUAL(s, "603972940850418616307173");

        TRY(z = a + d);  TRY(s = to_str(z));  TEST_EQUAL(s, "123456789123456789122098875544320997765543210");
        TRY(z = a - d);  TRY(s = to_str(z));  TEST_EQUAL(s, "123456789123456789124814702702592580481370368");
        TRY(z = d - a);  TRY(s = to_str(z));  TEST_EQUAL(s, "-123456789123456789124814702702592580481370368");
        TRY(z = a * d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-167643650387245846409206049247037049247037049079393398859791202837831");
        TRY(z = a / d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-90916528872203810704");
        TRY(z = a % d);  TRY(s = to_str(z));  TEST_EQUAL(s, "603972940850418616307173");

        TRY(z = c + b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-123456789123456789122098875544320997765543210");
        TRY(z = c - b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-123456789123456789124814702702592580481370368");
        TRY(z = b - c);  TRY(s = to_str(z));  TEST_EQUAL(s, "123456789123456789124814702702592580481370368");
        TRY(z = c * b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-167643650387245846409206049247037049247037049079393398859791202837831");
        TRY(z = c / b);  TRY(s = to_str(z));  TEST_EQUAL(s, "-90916528872203810705");
        TRY(z = c % b);  TRY(s = to_str(z));  TEST_EQUAL(s, "753940638285372741606406");

        TRY(z = c + d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-123456789123456789124814702702592580481370368");
        TRY(z = c - d);  TRY(s = to_str(z));  TEST_EQUAL(s, "-123456789123456789122098875544320997765543210");
        TRY(z = d - c);  TRY(s = to_str(z));  TEST_EQUAL(s, "123456789123456789122098875544320997765543210");
        TRY(z = c * d);  TRY(s = to_str(z));  TEST_EQUAL(s, "167643650387245846409206049247037049247037049079393398859791202837831");
        TRY(z = c / d);  TRY(s = to_str(z));  TEST_EQUAL(s, "90916528872203810705");
        TRY(z = c % d);  TRY(s = to_str(z));  TEST_EQUAL(s, "753940638285372741606406");

        TRY(x = 1);   TRY(y = 1);   TRY(z = x * y);  TEST_EQUAL(z.str(), "1");
        TRY(x = 1);   TRY(y = 0);   TRY(z = x * y);  TEST_EQUAL(z.str(), "0");
        TRY(x = 1);   TRY(y = -1);  TRY(z = x * y);  TEST_EQUAL(z.str(), "-1");
        TRY(x = 0);   TRY(y = 1);   TRY(z = x * y);  TEST_EQUAL(z.str(), "0");
        TRY(x = 0);   TRY(y = 0);   TRY(z = x * y);  TEST_EQUAL(z.str(), "0");
        TRY(x = 0);   TRY(y = -1);  TRY(z = x * y);  TEST_EQUAL(z.str(), "0");
        TRY(x = -1);  TRY(y = 1);   TRY(z = x * y);  TEST_EQUAL(z.str(), "-1");
        TRY(x = -1);  TRY(y = 0);   TRY(z = x * y);  TEST_EQUAL(z.str(), "0");
        TRY(x = -1);  TRY(y = -1);  TRY(z = x * y);  TEST_EQUAL(z.str(), "1");

        TRY(x = 0);    TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = 0);    TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "0");
        TRY(x = 0);    TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "0");
        TRY(x = 0);    TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "0");
        TRY(x = 1);    TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = 1);    TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "1");
        TRY(x = 1);    TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "1");
        TRY(x = 1);    TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "1");
        TRY(x = -1);   TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = -1);   TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "-1");
        TRY(x = -1);   TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "1");
        TRY(x = -1);   TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "-1");
        TRY(x = 10);   TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = 10);   TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "10");
        TRY(x = 10);   TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "100");
        TRY(x = 10);   TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "1000");
        TRY(x = 10);   TRY(y = x.pow(4));   TEST_EQUAL(y.str(), "10000");
        TRY(x = 10);   TRY(y = x.pow(5));   TEST_EQUAL(y.str(), "100000");
        TRY(x = 10);   TRY(y = x.pow(6));   TEST_EQUAL(y.str(), "1000000");
        TRY(x = 10);   TRY(y = x.pow(7));   TEST_EQUAL(y.str(), "10000000");
        TRY(x = 10);   TRY(y = x.pow(8));   TEST_EQUAL(y.str(), "100000000");
        TRY(x = 10);   TRY(y = x.pow(9));   TEST_EQUAL(y.str(), "1000000000");
        TRY(x = 10);   TRY(y = x.pow(10));  TEST_EQUAL(y.str(), "10000000000");
        TRY(x = 10);   TRY(y = x.pow(11));  TEST_EQUAL(y.str(), "100000000000");
        TRY(x = 10);   TRY(y = x.pow(12));  TEST_EQUAL(y.str(), "1000000000000");
        TRY(x = 10);   TRY(y = x.pow(13));  TEST_EQUAL(y.str(), "10000000000000");
        TRY(x = 10);   TRY(y = x.pow(14));  TEST_EQUAL(y.str(), "100000000000000");
        TRY(x = 10);   TRY(y = x.pow(15));  TEST_EQUAL(y.str(), "1000000000000000");
        TRY(x = 10);   TRY(y = x.pow(16));  TEST_EQUAL(y.str(), "10000000000000000");
        TRY(x = 10);   TRY(y = x.pow(17));  TEST_EQUAL(y.str(), "100000000000000000");
        TRY(x = 10);   TRY(y = x.pow(18));  TEST_EQUAL(y.str(), "1000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(19));  TEST_EQUAL(y.str(), "10000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(20));  TEST_EQUAL(y.str(), "100000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(21));  TEST_EQUAL(y.str(), "1000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(22));  TEST_EQUAL(y.str(), "10000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(23));  TEST_EQUAL(y.str(), "100000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(24));  TEST_EQUAL(y.str(), "1000000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(25));  TEST_EQUAL(y.str(), "10000000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(26));  TEST_EQUAL(y.str(), "100000000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(27));  TEST_EQUAL(y.str(), "1000000000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(28));  TEST_EQUAL(y.str(), "10000000000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(29));  TEST_EQUAL(y.str(), "100000000000000000000000000000");
        TRY(x = 10);   TRY(y = x.pow(30));  TEST_EQUAL(y.str(), "1000000000000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(0));   TEST_EQUAL(y.str(), "1");
        TRY(x = -10);  TRY(y = x.pow(1));   TEST_EQUAL(y.str(), "-10");
        TRY(x = -10);  TRY(y = x.pow(2));   TEST_EQUAL(y.str(), "100");
        TRY(x = -10);  TRY(y = x.pow(3));   TEST_EQUAL(y.str(), "-1000");
        TRY(x = -10);  TRY(y = x.pow(4));   TEST_EQUAL(y.str(), "10000");
        TRY(x = -10);  TRY(y = x.pow(5));   TEST_EQUAL(y.str(), "-100000");
        TRY(x = -10);  TRY(y = x.pow(6));   TEST_EQUAL(y.str(), "1000000");
        TRY(x = -10);  TRY(y = x.pow(7));   TEST_EQUAL(y.str(), "-10000000");
        TRY(x = -10);  TRY(y = x.pow(8));   TEST_EQUAL(y.str(), "100000000");
        TRY(x = -10);  TRY(y = x.pow(9));   TEST_EQUAL(y.str(), "-1000000000");
        TRY(x = -10);  TRY(y = x.pow(10));  TEST_EQUAL(y.str(), "10000000000");
        TRY(x = -10);  TRY(y = x.pow(11));  TEST_EQUAL(y.str(), "-100000000000");
        TRY(x = -10);  TRY(y = x.pow(12));  TEST_EQUAL(y.str(), "1000000000000");
        TRY(x = -10);  TRY(y = x.pow(13));  TEST_EQUAL(y.str(), "-10000000000000");
        TRY(x = -10);  TRY(y = x.pow(14));  TEST_EQUAL(y.str(), "100000000000000");
        TRY(x = -10);  TRY(y = x.pow(15));  TEST_EQUAL(y.str(), "-1000000000000000");
        TRY(x = -10);  TRY(y = x.pow(16));  TEST_EQUAL(y.str(), "10000000000000000");
        TRY(x = -10);  TRY(y = x.pow(17));  TEST_EQUAL(y.str(), "-100000000000000000");
        TRY(x = -10);  TRY(y = x.pow(18));  TEST_EQUAL(y.str(), "1000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(19));  TEST_EQUAL(y.str(), "-10000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(20));  TEST_EQUAL(y.str(), "100000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(21));  TEST_EQUAL(y.str(), "-1000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(22));  TEST_EQUAL(y.str(), "10000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(23));  TEST_EQUAL(y.str(), "-100000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(24));  TEST_EQUAL(y.str(), "1000000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(25));  TEST_EQUAL(y.str(), "-10000000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(26));  TEST_EQUAL(y.str(), "100000000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(27));  TEST_EQUAL(y.str(), "-1000000000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(28));  TEST_EQUAL(y.str(), "10000000000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(29));  TEST_EQUAL(y.str(), "-100000000000000000000000000000");
        TRY(x = -10);  TRY(y = x.pow(30));  TEST_EQUAL(y.str(), "1000000000000000000000000000000");

    }

    void check_integer_random() {

        std::mt19937 rng(42);
        RandomInt random;
        Int a, b, lo, hi, sum, sum2, x;
        int n = 10000;

        TRY(a = Int("-123456789123456789123456789123456789"));
        TRY(b = Int("987654321987654321987654321987654321"));
        TRY(lo = Nat(1) << 1000);
        TRY(hi = - lo);
        TRY(random = RandomInt(a, b));

        for (int i = 0; i < n; ++i) {
            TRY(x = random(rng));
            TRY(sum += x);
            TRY(sum2 += x * x);
            TRY(lo = std::min(lo, x));
            TRY(hi = std::max(hi, x));
        }

        TEST_COMPARE(lo, >=, a);
        TEST_COMPARE(hi, <=, b);

        double mean = double(sum) / n;
        double sd = sqrt(double(sum2) / n - mean * mean);

        TEST_NEAR_EPSILON(mean, 4.321e35, 2e34);
        TEST_NEAR_EPSILON(sd, 3.208e35, 2e34);

    }

    void check_core_functions() {

        Nat a = 42, b = 99, c, d;
        Int w = 42, x = -99, y, z;
        int s;

        TRY(c = clamp(a, 0, 10));  TEST_EQUAL(c, 10);
        TRY(y = clamp(w, 0, 10));  TEST_EQUAL(y, 10);

        TRY(c = quo(b, a));  TEST_EQUAL(c, 2);
        TRY(y = quo(x, w));  TEST_EQUAL(y, -3);
        TRY(c = rem(b, a));  TEST_EQUAL(c, 15);
        TRY(y = rem(x, w));  TEST_EQUAL(y, 27);

        TRY(std::tie(c, d) = divide(b, a));  TEST_EQUAL(c, 2);   TEST_EQUAL(d, 15);
        TRY(std::tie(y, z) = divide(x, w));  TEST_EQUAL(y, -3);  TEST_EQUAL(z, 27);

        TRY(s = sign_of(a));  TEST_EQUAL(s, 1);
        TRY(s = sign_of(w));  TEST_EQUAL(s, 1);
        TRY(s = sign_of(x));  TEST_EQUAL(s, -1);

        TRY(c = gcd(a, b));  TEST_EQUAL(c, 3);
        TRY(y = gcd(w, x));  TEST_EQUAL(y, 3);
        TRY(c = lcm(a, b));  TEST_EQUAL(c, 1386);
        TRY(y = lcm(w, x));  TEST_EQUAL(y, 1386);

    }

    void check_mp_rational_basics() {

        Ratmp r;

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
        TEST_EQUAL(r.simple(), "0/1");
        TEST_EQUAL(r.mixed(), "0");
        TEST_EQUAL(to_str(r), "0");

        TRY(r = Ratmp(5, 3));
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

        TRY(r = Ratmp(-7, 9));
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

    void check_mp_rational_reduction() {

        Ratmp r;

        TRY(r = Ratmp(0, 6));    TEST_EQUAL(r.num(), 0);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratmp(0, 6));
        TRY(r = Ratmp(1, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratmp(1, 6));
        TRY(r = Ratmp(2, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratmp(2, 6));
        TRY(r = Ratmp(3, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratmp(3, 6));
        TRY(r = Ratmp(4, 6));    TEST_EQUAL(r.num(), 2);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratmp(4, 6));
        TRY(r = Ratmp(5, 6));    TEST_EQUAL(r.num(), 5);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 0);   TEST_EQUAL(r.frac(), Ratmp(5, 6));
        TRY(r = Ratmp(6, 6));    TEST_EQUAL(r.num(), 1);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratmp(0, 6));
        TRY(r = Ratmp(7, 6));    TEST_EQUAL(r.num(), 7);    TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratmp(1, 6));
        TRY(r = Ratmp(8, 6));    TEST_EQUAL(r.num(), 4);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratmp(2, 6));
        TRY(r = Ratmp(9, 6));    TEST_EQUAL(r.num(), 3);    TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratmp(3, 6));
        TRY(r = Ratmp(10, 6));   TEST_EQUAL(r.num(), 5);    TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratmp(4, 6));
        TRY(r = Ratmp(11, 6));   TEST_EQUAL(r.num(), 11);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), 1);   TEST_EQUAL(r.frac(), Ratmp(5, 6));
        TRY(r = Ratmp(12, 6));   TEST_EQUAL(r.num(), 2);    TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), 2);   TEST_EQUAL(r.frac(), Ratmp(0, 6));
        TRY(r = Ratmp(-1, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratmp(-1, 6));
        TRY(r = Ratmp(-2, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratmp(-2, 6));
        TRY(r = Ratmp(-3, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratmp(-3, 6));
        TRY(r = Ratmp(-4, 6));   TEST_EQUAL(r.num(), -2);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratmp(-4, 6));
        TRY(r = Ratmp(-5, 6));   TEST_EQUAL(r.num(), -5);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -0);  TEST_EQUAL(r.frac(), Ratmp(-5, 6));
        TRY(r = Ratmp(-6, 6));   TEST_EQUAL(r.num(), -1);   TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratmp(-0, 6));
        TRY(r = Ratmp(-7, 6));   TEST_EQUAL(r.num(), -7);   TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratmp(-1, 6));
        TRY(r = Ratmp(-8, 6));   TEST_EQUAL(r.num(), -4);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratmp(-2, 6));
        TRY(r = Ratmp(-9, 6));   TEST_EQUAL(r.num(), -3);   TEST_EQUAL(r.den(), 2);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratmp(-3, 6));
        TRY(r = Ratmp(-10, 6));  TEST_EQUAL(r.num(), -5);   TEST_EQUAL(r.den(), 3);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratmp(-4, 6));
        TRY(r = Ratmp(-11, 6));  TEST_EQUAL(r.num(), -11);  TEST_EQUAL(r.den(), 6);  TEST_EQUAL(r.whole(), -1);  TEST_EQUAL(r.frac(), Ratmp(-5, 6));
        TRY(r = Ratmp(-12, 6));  TEST_EQUAL(r.num(), -2);   TEST_EQUAL(r.den(), 1);  TEST_EQUAL(r.whole(), -2);  TEST_EQUAL(r.frac(), Ratmp(-0, 6));

    }

    void check_mp_rational_arithmetic() {

        Ratmp r, s, t;

        TRY(r = Ratmp(5, 3));
        TRY(s = Ratmp(7, 9));

        TRY(t = r + s);  TEST_EQUAL(t.num(), 22);  TEST_EQUAL(t.den(), 9);
        TRY(t = r - s);  TEST_EQUAL(t.num(), 8);   TEST_EQUAL(t.den(), 9);
        TRY(t = r * s);  TEST_EQUAL(t.num(), 35);  TEST_EQUAL(t.den(), 27);
        TRY(t = r / s);  TEST_EQUAL(t.num(), 15);  TEST_EQUAL(t.den(), 7);

    }

    void check_mp_rational_properties() {

        Ratmp r, s, t;

        TRY(r = Ratmp(5, 3));
        TRY(s = Ratmp(-7, 9));

        TEST_EQUAL(abs(r), Ratmp(5, 3));  TEST_EQUAL(sign_of(r), 1);
        TEST_EQUAL(abs(s), Ratmp(7, 9));  TEST_EQUAL(sign_of(s), -1);
        TEST_EQUAL(abs(t), Ratmp(0));     TEST_EQUAL(sign_of(t), 0);

        TRY(r = Ratmp(-6, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -2);  TEST_EQUAL(r.round(), -2);
        TRY(r = Ratmp(-5, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -2);
        TRY(r = Ratmp(-4, 3));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
        TRY(r = Ratmp(-3, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
        TRY(r = Ratmp(-2, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), -1);
        TRY(r = Ratmp(-1, 3));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
        TRY(r = Ratmp(0, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
        TRY(r = Ratmp(1, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 0);
        TRY(r = Ratmp(2, 3));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
        TRY(r = Ratmp(3, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
        TRY(r = Ratmp(4, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 1);
        TRY(r = Ratmp(5, 3));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
        TRY(r = Ratmp(6, 3));   TEST_EQUAL(r.floor(), 2);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
        TRY(r = Ratmp(-8, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -2);  TEST_EQUAL(r.round(), -2);
        TRY(r = Ratmp(-7, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -2);
        TRY(r = Ratmp(-6, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
        TRY(r = Ratmp(-5, 4));  TEST_EQUAL(r.floor(), -2);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
        TRY(r = Ratmp(-4, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), -1);  TEST_EQUAL(r.round(), -1);
        TRY(r = Ratmp(-3, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), -1);
        TRY(r = Ratmp(-2, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
        TRY(r = Ratmp(-1, 4));  TEST_EQUAL(r.floor(), -1);  TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
        TRY(r = Ratmp(0, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 0);   TEST_EQUAL(r.round(), 0);
        TRY(r = Ratmp(1, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 0);
        TRY(r = Ratmp(2, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
        TRY(r = Ratmp(3, 4));   TEST_EQUAL(r.floor(), 0);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
        TRY(r = Ratmp(4, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 1);   TEST_EQUAL(r.round(), 1);
        TRY(r = Ratmp(5, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 1);
        TRY(r = Ratmp(6, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
        TRY(r = Ratmp(7, 4));   TEST_EQUAL(r.floor(), 1);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);
        TRY(r = Ratmp(8, 4));   TEST_EQUAL(r.floor(), 2);   TEST_EQUAL(r.ceil(), 2);   TEST_EQUAL(r.round(), 2);

    }

    void check_mp_rational_comparison() {

        Ratmp r, s;

        TRY(r = Ratmp(5, 6));   TRY(s = Ratmp(7, 9));     TEST_COMPARE(r, >, s);   TEST_COMPARE(r, >=, s);  TEST_COMPARE(r, !=, s);
        TRY(r = Ratmp(5, 6));   TRY(s = Ratmp(8, 9));     TEST_COMPARE(r, <, s);   TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, !=, s);
        TRY(r = Ratmp(5, 6));   TRY(s = Ratmp(10, 12));   TEST_COMPARE(r, ==, s);  TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, >=, s);
        TRY(r = Ratmp(-5, 6));  TRY(s = Ratmp(-7, 9));    TEST_COMPARE(r, <, s);   TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, !=, s);
        TRY(r = Ratmp(-5, 6));  TRY(s = Ratmp(-8, 9));    TEST_COMPARE(r, >, s);   TEST_COMPARE(r, >=, s);  TEST_COMPARE(r, !=, s);
        TRY(r = Ratmp(-5, 6));  TRY(s = Ratmp(-10, 12));  TEST_COMPARE(r, ==, s);  TEST_COMPARE(r, <=, s);  TEST_COMPARE(r, >=, s);

    }

    void check_mp_rational_mixed() {

        Ratmp r;

        TRY(r = 42);    TEST_EQUAL(r.num(), 42);   TEST_EQUAL(r.den(), 1);
        TRY(r += 100);  TEST_EQUAL(r.num(), 142);  TEST_EQUAL(r.den(), 1);
        TRY(r *= 2);    TEST_EQUAL(r.num(), 284);  TEST_EQUAL(r.den(), 1);

        TRY(r = Int(42));    TEST_EQUAL(r.num(), 42);   TEST_EQUAL(r.den(), 1);
        TRY(r += Int(100));  TEST_EQUAL(r.num(), 142);  TEST_EQUAL(r.den(), 1);
        TRY(r *= Int(2));    TEST_EQUAL(r.num(), 284);  TEST_EQUAL(r.den(), 1);

        TRY(r = 42);

        TEST_COMPARE(r, ==, 42);   TEST_COMPARE(r, ==, Int(42));
        TEST_COMPARE(r, <=, 42);   TEST_COMPARE(r, <=, Int(42));
        TEST_COMPARE(r, >=, 42);   TEST_COMPARE(r, >=, Int(42));
        TEST_COMPARE(r, !=, 100);  TEST_COMPARE(r, !=, Int(100));
        TEST_COMPARE(r, <, 100);   TEST_COMPARE(r, <, Int(100));
        TEST_COMPARE(r, <=, 100);  TEST_COMPARE(r, <=, Int(100));

    }

    void check_mp_rational_parsing() {

        // TODO - parsing uses strto[u]ll(), need to adapt for large integers

        Ratmp r;

        TRY(r = Ratmp("0"));       TEST_EQUAL(r.num(), 0);   TEST_EQUAL(r.den(), 1);
        TRY(r = Ratmp("5"));       TEST_EQUAL(r.num(), 5);   TEST_EQUAL(r.den(), 1);
        TRY(r = Ratmp("-5"));      TEST_EQUAL(r.num(), -5);  TEST_EQUAL(r.den(), 1);
        TRY(r = Ratmp("1/3"));     TEST_EQUAL(r.num(), 1);   TEST_EQUAL(r.den(), 3);
        TRY(r = Ratmp("4/6"));     TEST_EQUAL(r.num(), 2);   TEST_EQUAL(r.den(), 3);
        TRY(r = Ratmp("-1/3"));    TEST_EQUAL(r.num(), -1);  TEST_EQUAL(r.den(), 3);
        TRY(r = Ratmp("-4/6"));    TEST_EQUAL(r.num(), -2);  TEST_EQUAL(r.den(), 3);
        TRY(r = Ratmp("1 2/3"));   TEST_EQUAL(r.num(), 5);   TEST_EQUAL(r.den(), 3);
        TRY(r = Ratmp("-1 2/3"));  TEST_EQUAL(r.num(), -5);  TEST_EQUAL(r.den(), 3);

        TEST_THROW(r = Ratmp(""), std::invalid_argument);
        TEST_THROW(r = Ratmp("1 2"), std::invalid_argument);
        TEST_THROW(r = Ratmp("1 -2/3"), std::invalid_argument);

    }

}

TEST_MODULE(core, mp_integer) {

    check_natural_conversion();
    check_natural_arithmetic();
    check_natural_bit_operations();
    check_natural_byte_operations();
    check_natural_random();
    check_integer_conversion();
    check_integer_arithmetic();
    check_integer_random();
    check_core_functions();
    check_mp_rational_basics();
    check_mp_rational_reduction();
    check_mp_rational_arithmetic();
    check_mp_rational_properties();
    check_mp_rational_comparison();
    check_mp_rational_mixed();
    check_mp_rational_parsing();

}
