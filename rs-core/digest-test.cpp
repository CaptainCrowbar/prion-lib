#include "rs-core/digest.hpp"
#include "rs-core/int128.hpp"
#include "rs-core/random.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <array>
#include <numeric>
#include <stdexcept>
#include <string_view>
#include <vector>

using namespace RS;
using namespace RS::Literals;
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

namespace {

    const std::vector<uint32_t> vectors_sip32 = {

        0x5b9f'35a9ul, 0xb85a'4727ul, 0x03a6'62faul, 0x04e7'fe8aul, 0x8946'6e2aul, 0x69b6'fac5ul, 0x23fc'6358ul, 0xc563'cf8bul,
        0x8f84'b8d0ul, 0x79e7'06f8ul, 0x3479'b094ul, 0x5030'0808ul, 0x2f87'f057ul, 0xff63'e677ul, 0x7cf8'ffd6ul, 0x972b'fe74ul,
        0x84ac'b5d9ul, 0x5b64'74c4ul, 0x9b8d'5b46ul, 0x87e3'ef7bul, 0x4510'4de3ul, 0xb362'3f61ul, 0xfe67'f370ul, 0xbdb8'ade6ul,
        0x630c'4027ul, 0x7578'7826ul, 0x5f7b'564ful, 0x69e6'b03aul, 0x0040'64b0ul, 0xb40f'67fful, 0x8b33'9e50ul, 0x1a9f'585dul,
        0x1221'e7feul, 0x5932'7533ul, 0x8c4f'436aul, 0x29b7'28feul, 0xecc6'5ce7ul, 0x548d'7e69ul, 0x0f8b'6863ul, 0xb462'0b65ul,
        0x4018'bcb6ul, 0x0545'075dul, 0x2efd'4224ul, 0x3a86'b77bul, 0x48d5'0577ul, 0xb108'52d7ul, 0xc899'd4b6ul, 0x2e20'9208ul,
        0xe32c'e169ul, 0xe580'b58dul, 0xc664'9736ul, 0x0402'6e01ul, 0xd4f3'853bul, 0xbe66'dbfeul, 0x3a2a'691eul, 0xc084'89c6ul,
        0x40b9'c5a5ul, 0x8ce8'e99bul, 0x4081'bc7dul, 0xc58e'077cul, 0x736c'e7d4ul, 0xb9cb'8f42ul, 0x7a99'83bdul, 0x744a'ea59ul,

    };

    const std::vector<uint64_t> vectors_sip64 = {

        0x726f'db47'dd0e'0e31ull, 0x74f8'39c5'93dc'67fdull, 0x0d6c'8009'd9a9'4f5aull, 0x8567'6696'd7fb'7e2dull,
        0xcf27'94e0'2771'87b7ull, 0x1876'5564'cd99'a68dull, 0xcbc9'466e'58fe'e3ceull, 0xab02'00f5'8b01'd137ull,
        0x93f5'f579'9a93'2462ull, 0x9e00'82df'0ba9'e4b0ull, 0x7a5d'bbc5'94dd'b9f3ull, 0xf4b3'2f46'226b'ada7ull,
        0x751e'8fbc'860e'e5fbull, 0x14ea'5627'c084'3d90ull, 0xf723'ca90'8e7a'f2eeull, 0xa129'ca61'49be'45e5ull,
        0x3f2a'cc7f'57c2'9bdbull, 0x699a'e9f5'2cbe'4794ull, 0x4bc1'b3f0'968d'd39cull, 0xbb6d'c91d'a779'61bdull,
        0xbed6'5cf2'1aa2'ee98ull, 0xd0f2'cbb0'2e3b'67c7ull, 0x9353'6795'e3a3'3e88ull, 0xa80c'038c'cd5c'cec8ull,
        0xb8ad'50c6'f649'af94ull, 0xbce1'92de'8a85'b8eaull, 0x17d8'35b8'5bbb'15f3ull, 0x2f2e'6163'076b'cfadull,
        0xde4d'aaac'a71d'c9a5ull, 0xa6a2'5066'8795'6571ull, 0xad87'a353'5c49'ef28ull, 0x32d8'92fa'd841'c342ull,
        0x7127'512f'72f2'7cceull, 0xa7f3'2346'f959'78e3ull, 0x12e0'b01a'bb05'1238ull, 0x15e0'34d4'0fa1'97aeull,
        0x314d'ffbe'0815'a3b4ull, 0x0279'90f0'2962'3981ull, 0xcadc'd4e5'9ef4'0c4dull, 0x9abf'd876'6a33'735cull,
        0x0e3e'a96b'5304'a7d0ull, 0xad0c'42d6'fc58'5992ull, 0x1873'06c8'9bc2'15a9ull, 0xd4a6'0abc'f379'2b95ull,
        0xf935'451d'e4f2'1df2ull, 0xa953'8f04'1975'5787ull, 0xdb9a'cddf'f56c'a510ull, 0xd06c'98cd'5c09'75ebull,
        0xe612'a3cb'9ecb'a951ull, 0xc766'e62c'fcad'af96ull, 0xee64'435a'9752'fe72ull, 0xa192'd576'b245'165aull,
        0x0a87'87bf'8ecb'74b2ull, 0x81b3'e73d'20b4'9b6full, 0x7fa8'220b'a3b2'eceaull, 0x2457'31c1'3ca4'2499ull,
        0xb78d'bfaf'3a8d'83bdull, 0xea1a'd565'322a'1a0bull, 0x60e6'1c23'a379'5013ull, 0x6606'd7e4'4628'2b93ull,
        0x6ca4'ecb1'5c5f'91e1ull, 0x9f62'6da1'5c96'25f3ull, 0xe51b'3860'8ef2'5f57ull, 0x958a'324c'eb06'4572ull,

    };

    const std::vector<Uint128> vectors_sip128 = {

        0x9302'55c7'1472'f66d'e6a8'25ba'047f'81a3_u128, 0x45fc'229b'1159'7634'44af'996b'd8c1'87da_u128,
        0xe4ff'0af6'de8b'a3fc'c75d'a4a4'8d22'7781_u128, 0x51ed'8529'b0b6'335f'4ea9'6752'0cb6'709c_u128,
        0x7955'cd7b'7c6e'0f7d'af8f'9c2d'c164'81f8_u128, 0x2796'0e69'077a'5254'886f'7780'5987'6813_u128,
        0x5ea1'd78f'30a0'5e48'1386'208b'33ca'ee14_u128, 0x3982'f01f'a64a'b8c0'53c1'dbd8'beeb'f1a1_u128,
        0xb497'14f3'64e2'830f'61f5'5862'baa9'623b_u128, 0xed71'6dbb'028b'7fc4'abba'd90a'0699'4426_u128,
        0xbafb'd0f3'd347'54c9'5669'1478'c30d'1100_u128, 0x18dc'e581'6fdc'b4a2'7766'6b38'68c5'5101_u128,
        0x25c1'3285'f64d'6382'58f3'5e90'66b2'26d6_u128, 0xf752'b9c4'4f93'29d0'108b'c0e9'47e2'6998_u128,
        0x0249'49e4'5f48'c77e'9cde'd766'acef'fc31_u128, 0xd9c3'cf97'0fec'087e'11a8'b033'99e9'9354_u128,
        0x7705'2385'bf15'33fd'bb54'b067'caa4'e26e_u128, 0x4077'e47a'c466'c054'98b8'8d73'e806'3d47_u128,
        0x23f7'aefe'81a4'4d29'8548'bf23'e4e5'26a4_u128, 0xb12e'5152'8920'd574'b0fa'65cf'3177'0178_u128,
        0xeb39'38e8'a544'933e'7390'223f'83fc'259e_u128, 0x121d'073e'cd14'228a'215a'52be'5a49'8e56_u128,
        0xae0a'ff8e'5210'9c46'9a6b'd152'45b5'294a_u128, 0x1c69'bf9a'9ae2'8ccf'e0f5'a9d5'dd84'd1c9_u128,
        0xad32'618a'178a'2a88'd850'bd78'ae79'b42d_u128, 0x6f8f'8dcb'eab9'5150'7b44'5e2d'045f'ce8e_u128,
        0x661f'1478'86e0'ae7e'e807'c3b3'b453'0b9c_u128, 0x94eb'9e12'2feb'd3bf'e4ea'a669'af48'f2ab_u128,
        0xf4ae'5873'02f3'35b9'884b'5768'16da'6406_u128, 0xb76a'7c46'3cfd'd40c'e97d'33bf'c49d'4baa_u128,
        0x8722'6d68'd4d7'1a2b'de6b'af1f'477f'5cea_u128, 0x353d'c452'4fde'2317'fcfa'2332'18b0'3929_u128,
        0x68eb'4665'559d'3e36'3efc'ea5e'ca56'397c_u128, 0xcfff'a94e'5f9d'b6b6'321c'f046'7107'c677_u128,
        0xde54'9b30'f1f0'2509'df7e'84b8'6c98'a637_u128, 0xc88c'3c92'2e1a'2407'f9a8'a99d'e6f0'05a7_u128,
        0x1167'4f90'ed76'9e1e'4648'c429'1f7d'c43d_u128, 0x2b69'd3c5'5147'3c0d'1a0e'fce6'01bf'620d_u128,
        0xb5e7'be4b'085e'fde4'9e66'7cca'8b46'038c_u128, 0xd92b'd2d0'e5cc'7344'9c2c'af3b'b95b'8a52_u128,
        0xd83b'91c6'c80c'ae97'ad5d'c995'1e30'6adf_u128, 0xdbb6'705e'2891'35e7'397f'852c'9089'1180_u128,
        0x5b0c'cacc'34ae'5036'bb31'c2c9'6a34'17e6_u128, 0x89df'5aec'dc21'1840'aa21'b7ef'3734'd927_u128,
        0x4273'cc66'b1c9'b1d8'785e'9ced'9d7d'2389_u128, 0x4cb1'50a2'94fa'8911'657d'5ebf'9180'6d4a_u128,
        0x0229'49cf'3d0e'fc3f'89ae'e755'60f9'330e_u128, 0x1b15'63dc'4bd8'c88e'd119'0b72'2b43'1ce6_u128,
        0x169b'2608'a655'9037'cf82'f749'f5ae'e5f7_u128, 0x0364'1a20'adf2'37a8'4fa5'b7d0'0f03'8d43_u128,
        0x3f42'86f2'270d'7e24'e304'bf4f'eed3'90a5_u128, 0x38f5'f9ae'7cd3'5cb1'c493'fe72'a1c1'e25f_u128,
        0x7c01'3a8b'd03d'13b2'6eb3'06bd'5c32'972c_u128, 0x9ed3'2a00'9f65'f09f'94ca'6b7a'2214'c892_u128,
        0x871d'91d6'4108'd5fb'8c32'd80b'1150'e8dc_u128, 0xda83'2592'b52b'e348'1279'dac7'8449'f167_u128,
        0x362a'1da9'6f16'947e'e94e'd572'cff2'3819_u128, 0x8e69'0416'3024'620f'fe49'ed46'961e'4874_u128,
        0x1d8a'3d58'd038'6400'd8d6'a998'dea5'fc57_u128, 0x5953'57d9'7436'76d4'be1c'dcef'1cde'ec9f_u128,
        0x40e7'72d8'cb73'ca66'53f1'28eb'000c'04e3_u128, 0x7a0f'6793'591c'a9cc'fe1d'836a'9a00'9776_u128,
        0xbd59'47f0'a447'd505'a067'f521'2354'5358_u128, 0x7cbd'3f97'9a06'3e50'4a83'502f'77d1'5051_u128,

    };

}

void test_core_digest_siphash() {

    static const size_t tests32 = vectors_sip32.size();
    static const size_t tests64 = vectors_sip64.size();
    static const size_t tests128 = vectors_sip128.size();

    std::array<uint8_t, 16> key_bytes;
    std::vector<uint8_t> in32(tests32);
    std::vector<uint8_t> in64(tests64);
    std::vector<uint8_t> in128(tests128);
    uint32_t out32 = 0;
    uint64_t out64 = 0;
    Uint128 out128;

    std::iota(key_bytes.begin(), key_bytes.end(), uint8_t(0));
    std::iota(in32.begin(), in32.end(), uint8_t(0));
    std::iota(in64.begin(), in64.end(), uint8_t(0));
    std::iota(in128.begin(), in128.end(), uint8_t(0));

    SipHash32 sip32(key_bytes.data());
    SipHash64 sip64(key_bytes.data());
    SipHash128 sip128(key_bytes.data());

    Xoshiro rng(42);

    for (size_t len = 0; len < tests32; ++len) {

        TRY(sip32.clear());
        TRY(sip32.add(in32.data(), len));
        TRY(out32 = sip32.get());
        TEST_EQUAL(out32, vectors_sip32[len]);
        TRY(out32 = sip32.get());
        TEST_EQUAL(out32, vectors_sip32[len]);

        for (size_t i = 1; i < len; ++i) {
            TRY(sip32.clear());
            size_t pos = 0;
            while (pos < len) {
                size_t n = std::min(random_integer(len)(rng), len - pos);
                TRY(sip32.add(in32.data() + pos, n));
                pos += n;
            }
            TRY(out32 = sip32.get());
            TEST_EQUAL(out32, vectors_sip32[len]);
            TRY(out32 = sip32.get());
            TEST_EQUAL(out32, vectors_sip32[len]);
        }

    }

    for (size_t len = 0; len < tests64; ++len) {

        TRY(sip64.clear());
        TRY(sip64.add(in64.data(), len));
        TRY(out64 = sip64.get());
        TEST_EQUAL(out64, vectors_sip64[len]);
        TRY(out64 = sip64.get());
        TEST_EQUAL(out64, vectors_sip64[len]);

        for (size_t i = 1; i < len; ++i) {
            TRY(sip64.clear());
            size_t pos = 0;
            while (pos < len) {
                size_t n = std::min(random_integer(len)(rng), len - pos);
                TRY(sip64.add(in64.data() + pos, n));
                pos += n;
            }
            TRY(out64 = sip64.get());
            TEST_EQUAL(out64, vectors_sip64[len]);
            TRY(out64 = sip64.get());
            TEST_EQUAL(out64, vectors_sip64[len]);
        }

    }

    for (size_t len = 0; len < tests128; ++len) {

        TRY(sip128.clear());
        TRY(sip128.add(in128.data(), len));
        TRY(out128 = sip128.get());
        TEST_EQUAL(out128, vectors_sip128[len]);
        TRY(out128 = sip128.get());
        TEST_EQUAL(out128, vectors_sip128[len]);

        for (size_t i = 1; i < len; ++i) {
            TRY(sip128.clear());
            size_t pos = 0;
            while (pos < len) {
                size_t n = std::min(random_integer(len)(rng), len - pos);
                TRY(sip128.add(in128.data() + pos, n));
                pos += n;
            }
            TRY(out128 = sip128.get());
            TEST_EQUAL(out128, vectors_sip128[len]);
            TRY(out128 = sip128.get());
            TEST_EQUAL(out128, vectors_sip128[len]);
        }

    }

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
