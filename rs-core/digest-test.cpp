#include "rs-core/digest.hpp"
#include "rs-core/unit-test.hpp"
#include <array>
#include <stdexcept>
#include <string_view>

using namespace RS;
using namespace std::literals;

namespace {

    constexpr auto text1 = "Hello world"sv;
    constexpr auto text2 =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "
        "minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum."sv;

    template <size_t N>
    std::array<uint8_t, N> hex_array(std::string_view hex) {
        if (hex.size() != 2 * N)
            throw std::length_error("Hex string length does not match array");
        std::array<uint8_t, N> array;
        for (size_t i = 0; i < N; ++i)
            array[i] = uint8_t(hexnum(hex.substr(2 * i, 2)));
        return array;
    }

    template <typename H, typename R>
    void run_hash_tests(R result1, R result2) {

        H hash;
        R result = {};

        TRY(result = digest<H>(text1));
        TEST_EQUAL(result, result1);

        TRY(result = digest<H>(text2));
        TEST_EQUAL(result, result2);

        size_t block = 42;
        TRY(hash.clear());
        for (size_t pos = 0; pos < text2.size(); pos += block) {
            auto part = text2.substr(pos, block);
            TRY(hash.add(part.data(), part.size()));
        }
        TRY(result = hash.get());
        TEST_EQUAL(result, result2);

        block = 1;
        TRY(hash.clear());
        for (size_t pos = 0; pos < text1.size(); pos += block) {
            auto part = text1.substr(pos, block);
            TRY(hash.add(part.data(), part.size()));
        }
        TRY(result = hash.get());
        TEST_EQUAL(result, result1);

        TRY(hash.clear());
        for (size_t pos = 0; pos < text2.size(); pos += block) {
            auto part = text2.substr(pos, block);
            TRY(hash.add(part.data(), part.size()));
        }
        TRY(result = hash.get());
        TEST_EQUAL(result, result2);

    }

}

void test_core_digest_bernstein() {
    run_hash_tests<Bernstein>(uint32_t(0x89bb20a1), uint32_t(0x3d7c7bf6));
}

void test_core_digest_kernighan() {
    run_hash_tests<Kernighan>(uint32_t(0xce598aa4), uint32_t(0xca48acfd));
}

void test_core_digest_crc32() {
    run_hash_tests<Crc32>(uint32_t(0x8bd69e52), uint32_t(0x8f92322f));
}

void test_core_digest_djb2a() {
    run_hash_tests<Djb2a>(uint32_t(0x33c13465), uint32_t(0x8087909a));
}

void test_core_digest_fnv1a_32() {
    run_hash_tests<Fnv1a_32>(uint32_t(0x594d29c7), uint32_t(0xb7c3ecf4));
}

void test_core_digest_fnv1a_64() {
    run_hash_tests<Fnv1a_64>(uint64_t(0x2713f785a33764c7ull), uint64_t(0x72133ab33927ecf4ull));
}

void test_core_digest_murmur3_32() {
    run_hash_tests<Murmur3_32>(uint32_t(0xad91570c), uint32_t(0xce67ea22));
}

void test_core_digest_md5() {
    run_hash_tests<Md5>(hex_array<16>("3e25960a79dbc69b674cd4ec67a72c62"), hex_array<16>("fa5c89f3c88b81bfd5e821b0316569af"));
}

void test_core_digest_sha1() {
    run_hash_tests<Sha1>(hex_array<20>("7b502c3a1f48c8609ae212cdfb639dee39673f5e"), hex_array<20>("19afa2a4a37462c7b940a6c4c61363d49c3a35f4"));
}

void test_core_digest_sha256() {
    run_hash_tests<Sha256>(
        hex_array<32>("64ec88ca00b268e5ba1a35678a1b5316d212f4f366b2477232534a8aeca37f3c"),
        hex_array<32>("2c7c3d5f244f1a40069a32224215e0cf9b42485c99d80f357d76f006359c7a18")
    );
}

void test_core_digest_sha512() {
    run_hash_tests<Sha512>(
        hex_array<64>("b7f783baed8297f0db917462184ff4f08e69c2d5e5f79a942600f9725f58ce1f29c18139bf80b06c0fff2bdd34738452ecf40c488c22a7e3d80cdf6f9c1c0d47"),
        hex_array<64>("f41d92bc9fc1157a0d1387e67f3d0893b70f7039d3d46d8115b5079d45ad601159398c79c281681e2da09bf7d9f8c23b41d1a0a3c5b528a7f2735933a4353194")
    );
}
