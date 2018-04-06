#include "rs-core/digest.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <array>
#include <numeric>
#include <vector>

using namespace RS;

namespace {

    const Ustring sample1 = "";
    const Ustring sample2 = "Hello world";
    const Ustring sample3 =
        "Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do "
        "eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad "
        "minim veniam, quis nostrud exercitation ullamco laboris nisi ut "
        "aliquip ex ea commodo consequat. Duis aute irure dolor in "
        "reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla "
        "pariatur. Excepteur sint occaecat cupidatat non proident, sunt in "
        "culpa qui officia deserunt mollit anim id est laborum.";

    constexpr uint64_t sip24_testvec[] = {
        0x726fdb47dd0e0e31ull, 0x74f839c593dc67fdull, 0x0d6c8009d9a94f5aull, 0x85676696d7fb7e2dull, 0xcf2794e0277187b7ull, 0x18765564cd99a68dull, 0xcbc9466e58fee3ceull, 0xab0200f58b01d137ull,
        0x93f5f5799a932462ull, 0x9e0082df0ba9e4b0ull, 0x7a5dbbc594ddb9f3ull, 0xf4b32f46226bada7ull, 0x751e8fbc860ee5fbull, 0x14ea5627c0843d90ull, 0xf723ca908e7af2eeull, 0xa129ca6149be45e5ull,
        0x3f2acc7f57c29bdbull, 0x699ae9f52cbe4794ull, 0x4bc1b3f0968dd39cull, 0xbb6dc91da77961bdull, 0xbed65cf21aa2ee98ull, 0xd0f2cbb02e3b67c7ull, 0x93536795e3a33e88ull, 0xa80c038ccd5ccec8ull,
        0xb8ad50c6f649af94ull, 0xbce192de8a85b8eaull, 0x17d835b85bbb15f3ull, 0x2f2e6163076bcfadull, 0xde4daaaca71dc9a5ull, 0xa6a2506687956571ull, 0xad87a3535c49ef28ull, 0x32d892fad841c342ull,
        0x7127512f72f27cceull, 0xa7f32346f95978e3ull, 0x12e0b01abb051238ull, 0x15e034d40fa197aeull, 0x314dffbe0815a3b4ull, 0x027990f029623981ull, 0xcadcd4e59ef40c4dull, 0x9abfd8766a33735cull,
        0x0e3ea96b5304a7d0ull, 0xad0c42d6fc585992ull, 0x187306c89bc215a9ull, 0xd4a60abcf3792b95ull, 0xf935451de4f21df2ull, 0xa9538f0419755787ull, 0xdb9acddff56ca510ull, 0xd06c98cd5c0975ebull,
        0xe612a3cb9ecba951ull, 0xc766e62cfcadaf96ull, 0xee64435a9752fe72ull, 0xa192d576b245165aull, 0x0a8787bf8ecb74b2ull, 0x81b3e73d20b49b6full, 0x7fa8220ba3b2eceaull, 0x245731c13ca42499ull,
        0xb78dbfaf3a8d83bdull, 0xea1ad565322a1a0bull, 0x60e61c23a3795013ull, 0x6606d7e446282b93ull, 0x6ca4ecb15c5f91e1ull, 0x9f626da15c9625f3ull, 0xe51b38608ef25f57ull, 0x958a324ceb064572ull,
    };

}

void test_core_digest_adler32() {

    TEST_EQUAL(digest<Adler32>(sample1), 0x00000001);
    TEST_EQUAL(digest<Adler32>(sample2), 0x18ab043d);
    TEST_EQUAL(digest<Adler32>(sample3), 0x55d5a572);

}

void test_core_digest_crc32() {

    TEST_EQUAL(digest<Crc32>(sample1), 0x00000000);
    TEST_EQUAL(digest<Crc32>(sample2), 0x8bd69e52);
    TEST_EQUAL(digest<Crc32>(sample3), 0x8f92322f);

}

void test_core_digest_siphash24() {

    SipHash24 siphash;
    std::vector<uint8_t> key(16), src(64);
    std::iota(key.begin(), key.end(), uint8_t(0));
    std::iota(src.begin(), src.end(), uint8_t(0));
    uint64_t result;

    for (size_t srclen = 0; srclen < 64; ++srclen) {
        for (size_t blocklen = 1; blocklen <= srclen; ++blocklen) {
            TRY(siphash = SipHash24(key.data()));
            for (size_t offset = 0; offset < srclen; offset += blocklen)
                TRY(siphash(src.data() + offset, std::min(srclen - offset, blocklen)));
            TRY(result = siphash);
            TEST_EQUAL(result, sip24_testvec[srclen]);
        }
    }

}

void test_core_digest_md5() {

    using RT = Md5::result_type;

    RT result;
    Ustring hexstr;

    TRY(result = digest<Md5>(sample1));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "d41d8cd98f00b204e9800998ecf8427e");
    TRY(result = digest<Md5>(sample2));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "3e25960a79dbc69b674cd4ec67a72c62");

}

void test_core_digest_sha1() {

    using RT = Sha1::result_type;

    RT result;
    Ustring hexstr;

    TRY(result = digest<Sha1>(sample1));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "da39a3ee5e6b4b0d3255bfef95601890afd80709");
    TRY(result = digest<Sha1>(sample2));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "7b502c3a1f48c8609ae212cdfb639dee39673f5e");

}

void test_core_digest_sha256() {

    using RT = Sha256::result_type;

    RT result;
    Ustring hexstr;

    TRY(result = digest<Sha256>(sample1));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855");
    TRY(result = digest<Sha256>(sample2));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "64ec88ca00b268e5ba1a35678a1b5316d212f4f366b2477232534a8aeca37f3c");

}

void test_core_digest_sha512() {

    using RT = Sha512::result_type;

    RT result;
    Ustring hexstr;

    TRY(result = digest<Sha512>(sample1));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d13c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e");
    TRY(result = digest<Sha512>(sample2));
    TRY(hexstr = hex(result));
    TEST_EQUAL(hexstr, "b7f783baed8297f0db917462184ff4f08e69c2d5e5f79a942600f9725f58ce1f29c18139bf80b06c0fff2bdd34738452ecf40c488c22a7e3d80cdf6f9c1c0d47");

}
