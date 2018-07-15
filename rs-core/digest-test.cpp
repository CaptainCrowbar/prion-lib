#include "rs-core/digest.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

using namespace RS;

namespace {

    constexpr const char* text1 = "Hello world";
    constexpr const char* text2 =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "
        "minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";

    template <typename HF>
    void check_similar_strings() {
        using RT = typename HF::result_type;
        static constexpr int bits = 8 * sizeof(RT);
        std::minstd_rand rng(1);
        std::uniform_int_distribution<int> dist(0, 999);
        RT mask = ~ RT(0);
        for (int i = 0; i < 100; ++i) {
            int j = dist(rng);
            auto text = text2 + std::to_string(1000 + j).substr(1, 3);
            RT hash = HF()(text);
            mask &= hash;
        }
        int common = ibits(mask);
        if (common)
            throw std::runtime_error(fmt("Common bits in string hashes: $1/$2", common, bits));
    }

}

void test_core_digest_crc32() {

    uint32_t h = 0;

    TRY(h = Crc32()(text1));  TEST_EQUAL(h, 0x8bd69e52);
    TRY(h = Crc32()(text2));  TEST_EQUAL(h, 0x8f92322f);

    TRY(check_similar_strings<Crc32>());

}

void test_core_digest_djb2a() {

    uint32_t h = 0;

    TRY(h = Djb2a()(text1));  TEST_EQUAL(h, 0x33c13465);
    TRY(h = Djb2a()(text2));  TEST_EQUAL(h, 0x8087909a);

    // TRY(check_similar_strings<Djb2a>());

}

void test_core_digest_fnv1a_32() {

    uint32_t h = 0;

    TRY(h = Fnv1a_32()(text1));  TEST_EQUAL(h, 0x594d29c7);
    TRY(h = Fnv1a_32()(text2));  TEST_EQUAL(h, 0xb7c3ecf4);

    TRY(check_similar_strings<Fnv1a_32>());

}

void test_core_digest_fnv1a_64() {

    uint64_t h = 0;

    TRY(h = Fnv1a_64()(text1));  TEST_EQUAL(h, 0x2713f785a33764c7ull);
    TRY(h = Fnv1a_64()(text2));  TEST_EQUAL(h, 0x72133ab33927ecf4ull);

    // TRY(check_similar_strings<Fnv1a_64>());

}

void test_core_digest_murmur3_32() {

    uint64_t h = 0;

    TRY(h = Murmur3_32()(text1));  TEST_EQUAL(h, 0xad91570c);
    TRY(h = Murmur3_32()(text2));  TEST_EQUAL(h, 0xce67ea22);

    TRY(check_similar_strings<Murmur3_32>());

}

void test_core_digest_siphash_32() {

    uint64_t h = 0;

    TRY(h = SipHash_32()(text1));  TEST_EQUAL(h, 0xd6d081db);
    TRY(h = SipHash_32()(text2));  TEST_EQUAL(h, 0x13628af3);

    TRY(check_similar_strings<SipHash_32>());

}

void test_core_digest_siphash_64() {

    static constexpr uint64_t testvec[] = {
        0x726fdb47dd0e0e31ull, 0x74f839c593dc67fdull, 0x0d6c8009d9a94f5aull, 0x85676696d7fb7e2dull,
        0xcf2794e0277187b7ull, 0x18765564cd99a68dull, 0xcbc9466e58fee3ceull, 0xab0200f58b01d137ull,
        0x93f5f5799a932462ull, 0x9e0082df0ba9e4b0ull, 0x7a5dbbc594ddb9f3ull, 0xf4b32f46226bada7ull,
        0x751e8fbc860ee5fbull, 0x14ea5627c0843d90ull, 0xf723ca908e7af2eeull, 0xa129ca6149be45e5ull,
        0x3f2acc7f57c29bdbull, 0x699ae9f52cbe4794ull, 0x4bc1b3f0968dd39cull, 0xbb6dc91da77961bdull,
        0xbed65cf21aa2ee98ull, 0xd0f2cbb02e3b67c7ull, 0x93536795e3a33e88ull, 0xa80c038ccd5ccec8ull,
        0xb8ad50c6f649af94ull, 0xbce192de8a85b8eaull, 0x17d835b85bbb15f3ull, 0x2f2e6163076bcfadull,
        0xde4daaaca71dc9a5ull, 0xa6a2506687956571ull, 0xad87a3535c49ef28ull, 0x32d892fad841c342ull,
        0x7127512f72f27cceull, 0xa7f32346f95978e3ull, 0x12e0b01abb051238ull, 0x15e034d40fa197aeull,
        0x314dffbe0815a3b4ull, 0x027990f029623981ull, 0xcadcd4e59ef40c4dull, 0x9abfd8766a33735cull,
        0x0e3ea96b5304a7d0ull, 0xad0c42d6fc585992ull, 0x187306c89bc215a9ull, 0xd4a60abcf3792b95ull,
        0xf935451de4f21df2ull, 0xa9538f0419755787ull, 0xdb9acddff56ca510ull, 0xd06c98cd5c0975ebull,
        0xe612a3cb9ecba951ull, 0xc766e62cfcadaf96ull, 0xee64435a9752fe72ull, 0xa192d576b245165aull,
        0x0a8787bf8ecb74b2ull, 0x81b3e73d20b49b6full, 0x7fa8220ba3b2eceaull, 0x245731c13ca42499ull,
        0xb78dbfaf3a8d83bdull, 0xea1ad565322a1a0bull, 0x60e61c23a3795013ull, 0x6606d7e446282b93ull,
        0x6ca4ecb15c5f91e1ull, 0x9f626da15c9625f3ull, 0xe51b38608ef25f57ull, 0x958a324ceb064572ull,
    };

    uint64_t h = 0;

    TRY(h = SipHash_64()(text1));  TEST_EQUAL(h, 0xc9e8a3021f3822d9);
    TRY(h = SipHash_64()(text2));  TEST_EQUAL(h, 0xd034ff22c35675d1);

    TRY(check_similar_strings<SipHash_64>());

    std::vector<uint8_t> key(16), src(64);
    std::iota(key.begin(), key.end(), uint8_t(0));
    std::iota(src.begin(), src.end(), uint8_t(0));

    for (size_t srclen = 0; srclen < 64; ++srclen) {
        TRY(h = SipHash_64(key.data(), key.size())(src.data(), srclen));
        TEST_EQUAL(h, testvec[srclen]);
    }

}

void test_core_digest_md5() {

    Md5::result_type h;

    TRY(h = Md5()(text1));  TEST_EQUAL(hex(h), "3e25960a79dbc69b674cd4ec67a72c62");
    TRY(h = Md5()(text2));  TEST_EQUAL(hex(h), "fa5c89f3c88b81bfd5e821b0316569af");

}

void test_core_digest_sha1() {

    Sha1::result_type h;

    TRY(h = Sha1()(text1));  TEST_EQUAL(hex(h), "7b502c3a1f48c8609ae212cdfb639dee39673f5e");
    TRY(h = Sha1()(text2));  TEST_EQUAL(hex(h), "19afa2a4a37462c7b940a6c4c61363d49c3a35f4");

}

void test_core_digest_sha256() {

    Sha256::result_type h;

    TRY(h = Sha256()(text1));  TEST_EQUAL(hex(h), "64ec88ca00b268e5ba1a35678a1b5316d212f4f366b2477232534a8aeca37f3c");
    TRY(h = Sha256()(text2));  TEST_EQUAL(hex(h), "2c7c3d5f244f1a40069a32224215e0cf9b42485c99d80f357d76f006359c7a18");

}

void test_core_digest_sha512() {

    Sha512::result_type h;

    TRY(h = Sha512()(text1));
    TEST_EQUAL(hex(h), "b7f783baed8297f0db917462184ff4f08e69c2d5e5f79a942600f9725f58ce1f29c18139bf80b06c0fff2bdd34738452ecf40c488c22a7e3d80cdf6f9c1c0d47");
    TRY(h = Sha512()(text2));
    TEST_EQUAL(hex(h), "f41d92bc9fc1157a0d1387e67f3d0893b70f7039d3d46d8115b5079d45ad601159398c79c281681e2da09bf7d9f8c23b41d1a0a3c5b528a7f2735933a4353194");

}
