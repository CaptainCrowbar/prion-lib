#include "rs-core/random.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/statistics.hpp"
#include "rs-core/unit-test.hpp"
#include "rs-core/vector.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

#ifdef _MSC_VER
    #pragma warning(disable: 4127) // conditional expression is constant
#endif

using namespace RS;
using namespace RS::Literals;

namespace {

    const double inf = std::numeric_limits<double>::infinity();
    const double max32 = double(~ uint32_t(0));
    const double max64 = double(~ uint64_t(0));
    const double mean32 = max32 / 2;
    const double mean64 = max64 / 2;
    const double sd32 = std::sqrt((std::ldexp(1.0, 64) - 1) / 12);
    const double sd64 = std::sqrt((std::ldexp(1.0, 128) - 1) / 12);

    #define CHECK_RANDOM_GENERATOR(gen, xmin, xmax, xmean, xsd) { \
        using T = std::decay_t<decltype(gen())>; \
        double range; \
        if (xmin > - inf && xmax < inf) \
            range = xmax - xmin; \
        else \
            range = 2 * xsd; \
        double epsilon = 2 * range / std::sqrt(double(iterations)); \
        double min = inf, max = - inf, sum = 0, sum2 = 0; \
        T t = {}; \
        for (size_t i = 0; i < iterations; ++i) { \
            TRY(t = gen()); \
            auto x = double(t); \
            min = std::min(min, x); \
            max = std::max(max, x); \
            sum += x; \
            sum2 += x * x; \
        } \
        TEST_COMPARE(min, >=, xmin); \
        TEST_COMPARE(max, <=, xmax); \
        double mean = sum / iterations; \
        double sd = std::sqrt(sum2 / iterations - mean * mean); \
        TEST_NEAR_EPSILON(mean, xmean, epsilon); \
        TEST_NEAR_EPSILON(sd, xsd, epsilon); \
    }

}

void test_core_random_lcg() {

    static constexpr size_t iterations = 1'000'000;

    Lcg32 lcgrng32;
    Lcg64 lcgrng64;

    CHECK_RANDOM_GENERATOR(lcgrng32, 0, max32, mean32, sd32);
    CHECK_RANDOM_GENERATOR(lcgrng64, 0, max64, mean64, sd64);

}

void test_core_random_isaac32() {

    static constexpr uint32_t expect[512] = {
        0xe76dd339_u32, 0xd91aa738_u32, 0xd32956e5_u32, 0xa264e933_u32, 0x3dd20023_u32, 0x456f1edd_u32, 0x96c59cf4_u32, 0xd3a388fc_u32,
        0x5d89a1c1_u32, 0x49f413fc_u32, 0x4873d91e_u32, 0x2f840e43_u32, 0xed6abd62_u32, 0x10c8a5ab_u32, 0x43c1a57b_u32, 0xd70b1e10_u32,
        0x8173dc04_u32, 0x53f78eaa_u32, 0x89651d84_u32, 0xd5be7ffa_u32, 0x6f57e7ce_u32, 0x9c8ac447_u32, 0x0073e342_u32, 0x365aa66c_u32,
        0x97661c5e_u32, 0xaf90e49e_u32, 0x8c090a23_u32, 0x0e864542_u32, 0x3de0df39_u32, 0xbf7af48e_u32, 0x13871be7_u32, 0x3e7a8b49_u32,
        0x4207d36f_u32, 0x4a8ec630_u32, 0x0a06b272_u32, 0x66907654_u32, 0x9ded8244_u32, 0x32028bb8_u32, 0x4c3901c6_u32, 0xb6470834_u32,
        0x621bd7ca_u32, 0xf313adec_u32, 0x2f659883_u32, 0x9e979702_u32, 0xbbadd184_u32, 0x03dd14a5_u32, 0x43abc21c_u32, 0x565e2229_u32,
        0x89dae927_u32, 0x60060afb_u32, 0xca890b31_u32, 0xd5253552_u32, 0xad2ba480_u32, 0xe1194b0e_u32, 0x2d845c0d_u32, 0x97a48e53_u32,
        0xc7efbc2f_u32, 0x9971e496_u32, 0x80d8c2de_u32, 0xa0e5710b_u32, 0x0f955969_u32, 0xb566e14c_u32, 0x0b0cd6da_u32, 0xe5f489f6_u32,
        0x2c907ade_u32, 0x7be3f02e_u32, 0x71f63b07_u32, 0x09e6e57f_u32, 0x4c373e7a_u32, 0xc99a2639_u32, 0x54713d17_u32, 0x11f0831a_u32,
        0x76aa1c4e_u32, 0xdbf57f01_u32, 0x8b2160a8_u32, 0x2b2f4c09_u32, 0xcc5e6974_u32, 0xfde283c4_u32, 0x8b9e6b94_u32, 0x801a89ca_u32,
        0x651b11c0_u32, 0x360d41cd_u32, 0xbf2443d6_u32, 0x79801fd5_u32, 0xd95f25b3_u32, 0xb849d785_u32, 0x5fc7c5f3_u32, 0x368d3d00_u32,
        0x85c5eec2_u32, 0x7e2f5520_u32, 0x41ba2adc_u32, 0x5c509848_u32, 0x0761dc80_u32, 0x872783a4_u32, 0x71ce907c_u32, 0x52801fa8_u32,
        0x64e8e3f4_u32, 0x5d21d51e_u32, 0xf4930dba_u32, 0x3b347002_u32, 0x4eb6bd32_u32, 0xf9585a3a_u32, 0x7a60fabd_u32, 0x49ecf97a_u32,
        0x82fccaeb_u32, 0xffb8ab5c_u32, 0x1844cee2_u32, 0x161f5ca6_u32, 0xaf5cfcab_u32, 0x332c95d6_u32, 0xa5b5981d_u32, 0x99d8d9e6_u32,
        0x6095efcb_u32, 0xade6e428_u32, 0x290a1140_u32, 0x67822b45_u32, 0x359ac366_u32, 0xfb253454_u32, 0x8b8e753c_u32, 0x7cab12f6_u32,
        0xe15000a6_u32, 0x4af40896_u32, 0x5b6fb132_u32, 0xfd84b19b_u32, 0xa43a1d02_u32, 0x672c3725_u32, 0x9d21ac42_u32, 0x3ff4cc70_u32,
        0xe41ea465_u32, 0xe3bb2064_u32, 0x44c4d79d_u32, 0x44d6e893_u32, 0x436385c6_u32, 0xb310d06e_u32, 0xdf8be548_u32, 0x2992dfc6_u32,
        0xb2cbd9e4_u32, 0x80b4b915_u32, 0xb3fc5c66_u32, 0x7a725bb2_u32, 0x4c655b84_u32, 0x1ad1d764_u32, 0x1d3043eb_u32, 0xb0cc336b_u32,
        0x6b2f4538_u32, 0x3d29669b_u32, 0x6275acf3_u32, 0x08dfd64f_u32, 0x36658201_u32, 0x007e96a3_u32, 0x003a02f5_u32, 0xdbf38a8f_u32,
        0x205530ee_u32, 0xb3310333_u32, 0x7dfc8c7f_u32, 0x10b22a22_u32, 0x9cc17bcc_u32, 0xc6ba1971_u32, 0xc230e2ac_u32, 0xdd5e06c6_u32,
        0x3f07ec49_u32, 0x1ad3d9d5_u32, 0x765f0dba_u32, 0xc5418124_u32, 0x060ded54_u32, 0xfa55319b_u32, 0x5b563101_u32, 0xb831ad14_u32,
        0x9c7dc323_u32, 0xe099baf1_u32, 0x7f081c9a_u32, 0xb7cfa23c_u32, 0xd7e48546_u32, 0x4fbb94be_u32, 0xb10b5d29_u32, 0x7c53c9b2_u32,
        0x0935ad6e_u32, 0xcc5fff02_u32, 0xe34802d5_u32, 0x03359832_u32, 0x4dac20db_u32, 0xbaf0406d_u32, 0x15d565b4_u32, 0x1bdf1fd0_u32,
        0x394b8a4f_u32, 0x2e7ea9ed_u32, 0x0f2969e4_u32, 0x890a740c_u32, 0xde342516_u32, 0x9026d97e_u32, 0x9521825f_u32, 0x6f359a39_u32,
        0x98cb3f8b_u32, 0x02c0d8ab_u32, 0x902b347d_u32, 0x1ffe0e04_u32, 0xf460bdf1_u32, 0xa7dc6e56_u32, 0x17cb7ebf_u32, 0xb026016d_u32,
        0xed6e963b_u32, 0x72eb767b_u32, 0xbef63eea_u32, 0x6f461810_u32, 0x7bba5ead_u32, 0x5d3c00ea_u32, 0x4cfdd254_u32, 0x675cc8ae_u32,
        0xb97ffabe_u32, 0x2492d476_u32, 0x4e83930a_u32, 0x26f0774d_u32, 0x4b90a4de_u32, 0x6be6ce81_u32, 0x19ca2158_u32, 0xa358eb94_u32,
        0x42d19cd7_u32, 0xc7041eae_u32, 0xe8b4f627_u32, 0x02e17b6f_u32, 0xfb8a0e99_u32, 0x9e604c7b_u32, 0xc01eda0d_u32, 0xead4a900_u32,
        0x842131c4_u32, 0x472fd994_u32, 0xc6acd20f_u32, 0x44d3ff79_u32, 0x05c04fce_u32, 0x67648b17_u32, 0xf27c4d0d_u32, 0x0016e86f_u32,
        0x1629cfa4_u32, 0xac450b8f_u32, 0x5ab3eab9_u32, 0xf4c75e24_u32, 0x25c871bd_u32, 0xbae16ffa_u32, 0x6249fa29_u32, 0xc5c29e18_u32,
        0xa658f57a_u32, 0xc90f8fbd_u32, 0x347cb71d_u32, 0x96c33f74_u32, 0xcc4c9afb_u32, 0x894f41c2_u32, 0x52251def_u32, 0x5f071faf_u32,
        0x0fc99c24_u32, 0xb3ffa8b5_u32, 0x995206e9_u32, 0x296fd679_u32, 0xb08acd21_u32, 0x301b6622_u32, 0x300b4a8d_u32, 0x182600f3_u32,
        0xf650e4c8_u32, 0xe448e96d_u32, 0x98db2fb4_u32, 0xf5fad54f_u32, 0x433f1afb_u32, 0xedec154a_u32, 0xd8370487_u32, 0x46ca4f9a_u32,
        0x5de3743e_u32, 0x88381097_u32, 0xf1d444eb_u32, 0x823cedb6_u32, 0x6a83e1e0_u32, 0x4a5f6355_u32, 0xc7442433_u32, 0x25890e2e_u32,
        0x7452e319_u32, 0x57161df6_u32, 0x38a824f3_u32, 0x002ed713_u32, 0x29f55449_u32, 0x51c08d83_u32, 0xd78cb99e_u32, 0xa0cc74f3_u32,
        0x8f651659_u32, 0xcbc8b7c2_u32, 0xf5f71c69_u32, 0x12ad6419_u32, 0xe5792e1b_u32, 0x860536b8_u32, 0x09b3ce98_u32, 0xd45d6d81_u32,
        0xf3b26129_u32, 0x17e38f85_u32, 0x29cf72ce_u32, 0x349947b0_u32, 0xc998f9ff_u32, 0xb5e13dae_u32, 0x32ae2a2b_u32, 0xf7cf814c_u32,
        0x8ebfa303_u32, 0xcf22e064_u32, 0x0b923200_u32, 0xeca4d58a_u32, 0xef53cec4_u32, 0xd0f7b37d_u32, 0x9c411a2a_u32, 0xffdf8a80_u32,
        0xb40e27bc_u32, 0xb4d2f976_u32, 0x44b89b08_u32, 0xf37c71d5_u32, 0x1a70e7e9_u32, 0x0bdb9c30_u32, 0x60dc5207_u32, 0xb3c3f24b_u32,
        0xd7386806_u32, 0x229749b5_u32, 0x4e232cd0_u32, 0x91dabc65_u32, 0xa70e1101_u32, 0x8b87437e_u32, 0x5781414f_u32, 0xcdbc62e2_u32,
        0x8107c9ff_u32, 0x69d2e4ae_u32, 0x3b18e752_u32, 0xb143b688_u32, 0x6f4e0772_u32, 0x95138769_u32, 0x943c3c74_u32, 0xafc17a97_u32,
        0x0fd43963_u32, 0x6a529b0b_u32, 0xd8c58a6a_u32, 0xa8bcc22d_u32, 0x2db35dfe_u32, 0xa7a2f402_u32, 0x6cb167db_u32, 0x538e1f4e_u32,
        0x7275e277_u32, 0x1d3b8e97_u32, 0xecc5dc91_u32, 0x15e3a5b9_u32, 0x03696614_u32, 0x30ab93ec_u32, 0xac9fe69d_u32, 0x7bc76811_u32,
        0x60eda8da_u32, 0x28833522_u32, 0xd5295ebc_u32, 0x5adb60e7_u32, 0xf7e1cdd0_u32, 0x97166d14_u32, 0xb67ec13a_u32, 0x210f3925_u32,
        0x64af0fef_u32, 0x0d028684_u32, 0x3aea3dec_u32, 0xb058bafb_u32, 0xb8b0ccfc_u32, 0xf2b5cc05_u32, 0xe3a662d9_u32, 0x814bc24c_u32,
        0x2364a1aa_u32, 0x37c0ed05_u32, 0x2b36505c_u32, 0x451e7ec8_u32, 0x5d2a542f_u32, 0xe43d0fbb_u32, 0x91c8d925_u32, 0x60d4d5f8_u32,
        0x12a0594b_u32, 0x9e8a51da_u32, 0xcd49ebdb_u32, 0x1b0dcdc1_u32, 0xcd57c7f7_u32, 0xe6344451_u32, 0x7ded386f_u32, 0x2f36fa86_u32,
        0xa6d12101_u32, 0x33bc405d_u32, 0xb388d96c_u32, 0xdb6dbe96_u32, 0xfe29661c_u32, 0x13edc0cb_u32, 0xcb0eee4a_u32, 0x70cc94ae_u32,
        0xde11ed34_u32, 0x0606cf9f_u32, 0x3a6ce389_u32, 0x23d74f4e_u32, 0xa37f63ff_u32, 0x917bdec2_u32, 0xd73f72d4_u32, 0x0e7e0e67_u32,
        0x3d77d9a2_u32, 0x13add922_u32, 0x8891b3db_u32, 0x01a9bd70_u32, 0x56a001e3_u32, 0xd51f093d_u32, 0xcc033ce3_u32, 0x5ad0d3b0_u32,
        0x34105a8c_u32, 0x6a123f57_u32, 0xbd2e5024_u32, 0x7364944b_u32, 0xe89b1a3b_u32, 0x21835c4d_u32, 0x9f39e2d9_u32, 0xd405ded8_u32,
        0x294d37e5_u32, 0xbccaaeed_u32, 0x35a124b5_u32, 0x6708a2bc_u32, 0xb00960ba_u32, 0x2a98121a_u32, 0x4d8fae82_u32, 0x0bb3263f_u32,
        0x12595a19_u32, 0x6a107589_u32, 0x0809e494_u32, 0x21c171ec_u32, 0x884d6825_u32, 0x14c8009b_u32, 0xb0b84e7b_u32, 0x03fb88f4_u32,
        0x28e7cb78_u32, 0x9388b13b_u32, 0xdd2dc1d5_u32, 0x848f520a_u32, 0x07c28cd1_u32, 0x68a39358_u32, 0x72c9137d_u32, 0x127dd430_u32,
        0xc613f157_u32, 0x8c2f0d55_u32, 0xf7d3f39f_u32, 0x309bfb78_u32, 0x8406b137_u32, 0x46c0a6f5_u32, 0x3718d597_u32, 0x08607f04_u32,
        0x76904b6d_u32, 0x04db4e13_u32, 0xcd7411a7_u32, 0xb510ce0e_u32, 0xbfc7f7cc_u32, 0xb83f957a_u32, 0xfdfef62d_u32, 0xc35e4580_u32,
        0x3ff1e524_u32, 0x4112d96c_u32, 0x02c9b944_u32, 0xd5990dfb_u32, 0xe7e26581_u32, 0x0d9c7e7e_u32, 0x826dfa89_u32, 0x66f1e0ab_u32,
        0x30bcc764_u32, 0xeadebeac_u32, 0xed35e5ee_u32, 0x0c571a7d_u32, 0xe4f3a26a_u32, 0xf7f58f7b_u32, 0xadf6bc23_u32, 0x5d023e65_u32,
        0x1ed3ff4e_u32, 0xec46b0b6_u32, 0xd2a93b51_u32, 0xe75b41c9_u32, 0x7e315aeb_u32, 0x61119a5a_u32, 0x53245b79_u32, 0x33f6d7b1_u32,
        0xcae8deba_u32, 0x50fc8194_u32, 0xafa92a6d_u32, 0xc87c8006_u32, 0x4188bfcd_u32, 0x8bace62e_u32, 0x78ffa568_u32, 0x5597ec0f_u32,
        0xb4415f7d_u32, 0x08294766_u32, 0xad567643_u32, 0x09c36f90_u32, 0x3dde9f39_u32, 0x4a0a283c_u32, 0x18080c8e_u32, 0x080c79ec_u32,
        0x79ae4c10_u32, 0xcb9e1563_u32, 0x7cdd662f_u32, 0x62d31911_u32, 0xa4ca0cf1_u32, 0x5cf824cd_u32, 0x3b708f99_u32, 0x1e16614c_u32,
        0xb6b9d766_u32, 0x5de87abb_u32, 0x7229ea81_u32, 0xd5b2d750_u32, 0x56e6cd21_u32, 0xfe1e42d5_u32, 0x96da2655_u32, 0xc2b9aa36_u32,
        0xb8f6fd4a_u32, 0x6a158d10_u32, 0x01913fd3_u32, 0xaf7d1fb8_u32, 0x0b5e435f_u32, 0x90c10757_u32, 0x6554abda_u32, 0x7a68710f_u32,
    };

    Isaac32 rng;
    uint32_t x = 0;
    for (size_t i = 0; i < 512; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect[i]);
    }

    uint32_t a1 = 0, a2 = 0, b1 = 0, b2 = 0, c1 = 0, c2 = 0, d1 = 0, d2 = 0;
    TRY((rng = Isaac32{123, 456, 789}));
    TRY(a1 = rng());
    TRY(b1 = rng());
    TRY(c1 = rng());
    TRY(d1 = rng());
    TRY((rng = Isaac32{123, 456, 7890}));
    TRY(a2 = rng());  TEST_COMPARE(a1, !=, a2);
    TRY(b2 = rng());  TEST_COMPARE(b1, !=, b2);
    TRY(c2 = rng());  TEST_COMPARE(c1, !=, c2);
    TRY(d2 = rng());  TEST_COMPARE(d1, !=, d2);
    TRY(rng.seed({123, 456, 789}));
    TRY(a2 = rng());  TEST_EQUAL(a1, a2);
    TRY(b2 = rng());  TEST_EQUAL(b1, b2);
    TRY(c2 = rng());  TEST_EQUAL(c1, c2);
    TRY(d2 = rng());  TEST_EQUAL(d1, d2);

}

void test_core_random_isaac64() {

    static constexpr uint64_t expect[512] = {
        0x48cbff086ddf285a_u64, 0x99e7afeabe000731_u64, 0x93c42566aef98ffb_u64, 0xa865a54edcc0f019_u64, 0x0d151d86adb73615_u64, 0xdab9fe6525d89021_u64, 0x1b85d488d0f20cc5_u64, 0xf678647e3519ac6e_u64,
        0x5e11e86d5873d484_u64, 0x0ed9b915c66ed37e_u64, 0xb0183db56ffc6a79_u64, 0x506e6744cd974924_u64, 0x881b82a13b51b9e2_u64, 0x9a9632e65904ad3c_u64, 0x742e1e651c60ba83_u64, 0x04feabfbbdb619cb_u64,
        0x284c847b9d887aae_u64, 0x56fd23c8f9715a4c_u64, 0x0cd9a497658a5698_u64, 0x5a110c6058b920a0_u64, 0x04208fe9e8f7f2d6_u64, 0x7a249a57ec0c9ba2_u64, 0x1d1260a51107fe97_u64, 0x722ff175f572c348_u64,
        0x9d1dfa2efc557f73_u64, 0x52ab92beb9613989_u64, 0x528f7c8602c5807b_u64, 0xd941aca44b20a45b_u64, 0x4361c0ca3f692f12_u64, 0x513e5e634c70e331_u64, 0x77a225a07cc2c6bd_u64, 0xa90b24499fcfafb1_u64,
        0x35cab62109dd038a_u64, 0x32095b6d4ab5f9b1_u64, 0x3810e399b6f65ba2_u64, 0x9d1d60e5076f5b6f_u64, 0x7a1ee967d27579e2_u64, 0x68ca39053261169f_u64, 0x8cffa9412eb642c1_u64, 0x40e087931a00930d_u64,
        0x4c0563b89f495ac3_u64, 0x18fcf680573fa594_u64, 0xfcaf55c1bf8a4424_u64, 0x39b0bf7dde437ba2_u64, 0xf3a678cad9a2e38c_u64, 0x7ba2484c8a0fd54e_u64, 0x16b9f7e06c453a21_u64, 0x87d380bda5bf7859_u64,
        0x4f2a5cb07f6a35b3_u64, 0xa2f61bb6e437fdb5_u64, 0xa74049dac312ac71_u64, 0x336f52f8ff4728e7_u64, 0xd95be88cd210ffa7_u64, 0xd7f4f2448c0ceb81_u64, 0xf7a255d83bc373f8_u64, 0xd2b7adeeded1f73f_u64,
        0x3bba57b68871b59d_u64, 0xdf1d9f9d784ba010_u64, 0x94061b871e04df75_u64, 0x9315e5eb3a129ace_u64, 0x08bd35cc38336615_u64, 0xfe9a44e9362f05fa_u64, 0x78e37644e7cad29e_u64, 0xc547f57e42a7444e_u64,
        0x6703df9d2924e97e_u64, 0x8ec97d2917456ed0_u64, 0x9c684cb6c4d24417_u64, 0xfc6a82d64b8655fb_u64, 0xf9b5b7c4acc67c96_u64, 0x69b97db1a4c03dfe_u64, 0xe755178d58fc4e76_u64, 0xa4fc4bd4fc5558ca_u64,
        0x9ae182c8bc9474e8_u64, 0xb05ca3f564268d99_u64, 0xcfc447f1e53c8e1b_u64, 0x4850e73e03eb6064_u64, 0x2c604a7a177326b3_u64, 0x0bf692b38d079f23_u64, 0xde336a2a4bc1c44b_u64, 0xd7288e012aeb8d31_u64,
        0xf793c46702e086a0_u64, 0x763c4a1371b368fd_u64, 0x2df16f761598aa4f_u64, 0x21a007933a522a20_u64, 0xb3819a42abe61c87_u64, 0xb46ee9c5e64a6e7c_u64, 0xc07a3f80c31fb4b4_u64, 0x51039ab7712457c3_u64,
        0x4dc4de189b671a1c_u64, 0x066f70b33fe09017_u64, 0x9da4243de836994f_u64, 0xbce5d2248682c115_u64, 0x11379625747d5af3_u64, 0xf4f076e65f2ce6f0_u64, 0x52593803dff1e840_u64, 0x19afe59ae451497f_u64,
        0xaef3af4a563dfe43_u64, 0x480412bab7f5be2a_u64, 0xaf2042f5cc5c2858_u64, 0xef2f054308f6a2bc_u64, 0x9bc5a38ef729abd4_u64, 0x2d255069f0b7dab3_u64, 0x5648f680f11a2741_u64, 0xc5cc1d89724fa456_u64,
        0x28aed140be0bb7dd_u64, 0x10cff333e0ed804a_u64, 0x91b859e59ecb6350_u64, 0xb415938d7da94e3c_u64, 0x21f08570f420e565_u64, 0xded2d633cad004f6_u64, 0x65942c7b3c7e11ae_u64, 0xa87832d392efee56_u64,
        0xaa969b5c691ccb7a_u64, 0x43539603d6c55602_u64, 0x1bede3a3aef53302_u64, 0xdec468145b7605f6_u64, 0x808bd68e6ac10365_u64, 0xc91800e98fb99929_u64, 0x22fe545401165f1c_u64, 0x7eed120d54cf2dd9_u64,
        0xdd2c5bc84bc8d8fc_u64, 0xae623fd67468aa70_u64, 0xff6712ffcfd75ea1_u64, 0x930f80f4e8eb7462_u64, 0x45f20042f24f1768_u64, 0xbb215798d45df7af_u64, 0xefac4b70633b8f81_u64, 0x56436c9fe1a1aa8d_u64,
        0x73a1921916591cbd_u64, 0x70eb093b15b290cc_u64, 0x920e449535dd359e_u64, 0x043fcae60cc0eba0_u64, 0xa246637cff328532_u64, 0x97d7374c60087b73_u64, 0x86536b8cf3428a8c_u64, 0x799e81f05bc93f31_u64,
        0xede6c87f8477609d_u64, 0x3c79a0ff5580ef7f_u64, 0xf538639ce705b824_u64, 0xcf464cec899a2f8a_u64, 0x4a750a09ce9573f7_u64, 0xb5889c6e15630a75_u64, 0x05a7e8a57db91b77_u64, 0xb9fd7620e7316243_u64,
        0x7b32f7d1e03680ec_u64, 0xef927dbcf00c20f2_u64, 0xdfd395339cdbf4a7_u64, 0x6503080440750644_u64, 0x1881afc9a3a701d6_u64, 0x506aacf489889342_u64, 0x5b9b63eb9ceff80c_u64, 0x2171e64683023a08_u64,
        0x6304d09a0b3738c4_u64, 0x4f80f7a035dafb04_u64, 0x9a74acb964e78cb3_u64, 0x1e1032911fa78984_u64, 0x5bfea5b4712768e9_u64, 0x390e5fb44d01144b_u64, 0xb3f22c3d0b0b38ed_u64, 0x9c1633264db49c89_u64,
        0x8de8dca9f03cc54e_u64, 0xff07f64ef8ed14d0_u64, 0x092237ac237f3859_u64, 0x87bf02c6b49e2ae9_u64, 0x1920c04d47267bbd_u64, 0xae4a9346cc3f7cf2_u64, 0xa366e5b8c54f48b8_u64, 0x87b3e2b2b5c907b1_u64,
        0xf9b89d3e99a075c2_u64, 0x70ac4cd9f04f21f5_u64, 0x9a85ac909a24eaa1_u64, 0xee954d3c7b411f47_u64, 0x72b12c32127fed2b_u64, 0x54b3f4fa5f40d873_u64, 0x8535f040b9744ff1_u64, 0x27e6ad7891165c3f_u64,
        0xe99d662af4243939_u64, 0xa49cd132bfbf7cc4_u64, 0x0ce26c0b95c980d9_u64, 0xbb6e2924f03912ea_u64, 0x24c3c94df9c8d3f6_u64, 0xdabf2ac8201752fc_u64, 0xf145b6beccdea195_u64, 0x14acbaf4777d5776_u64,
        0x79ad695501e7d1e8_u64, 0x8249a47aee0e41f7_u64, 0x637a7780decfc0d9_u64, 0x19fc8a768cf4b6d4_u64, 0x7bcbc38da25a7f3c_u64, 0x5093417aa8a7ed5e_u64, 0x07fb9f855a997142_u64, 0x5355f900c2a82dc7_u64,
        0xec16ca8aea98ad76_u64, 0x63dc359d8d231b78_u64, 0x93c5b5f47356388b_u64, 0x39f890f579f92f88_u64, 0x5f0f4a5898171bb6_u64, 0x42880b0236e4d951_u64, 0x6d2bdcdae2919661_u64, 0x42e240cb63689f2f_u64,
        0x96d693460cc37e5d_u64, 0x4de0b0f40f32a7b8_u64, 0x6568fca92c76a243_u64, 0x11d505d4c351bd7f_u64, 0x7ef48f2b83024e20_u64, 0xb9bc6c87167c33e7_u64, 0x8c74c368081b3075_u64, 0x3253a729b9ba3dde_u64,
        0xfcf7fe8a3430b241_u64, 0x5c82c505db9ab0fa_u64, 0x51ebdc4ab9ba3035_u64, 0x9f74d14f7454a824_u64, 0xbf983fe0fe5d8244_u64, 0xd310a7c2ce9b6555_u64, 0x1fcbacd259bf02e7_u64, 0x18727070f1bd400b_u64,
        0x735e2b97a4c45a23_u64, 0x3575668334a1dd3b_u64, 0x09d1bc9a3dd90a94_u64, 0x637b2b34ff93c040_u64, 0x03488b95b0f1850f_u64, 0xa71b9b83461cbd93_u64, 0x14a68fd73c910841_u64, 0x4c9f34427501b447_u64,
        0x24aa6c514da27500_u64, 0xc9452ca81a09d85d_u64, 0x7b0500ac42047ac4_u64, 0xb4ab30f062b19abf_u64, 0x19f3c751d3e92ae1_u64, 0x87d2074b81d79217_u64, 0x8dbd98a352afd40b_u64, 0xaa649c6ebcfd50fc_u64,
        0x4bb38de5e7219443_u64, 0x331478f3af51bbe6_u64, 0xf3218f1c9510786c_u64, 0x82c7709e781eb7cc_u64, 0x7d11cdb1c3b7adf0_u64, 0x7449bbff801fed0b_u64, 0x679f848f6e8fc971_u64, 0x05d1a1ae85b49aa1_u64,
        0x239f8b2d7ff719cc_u64, 0x5db4832046f3d9e5_u64, 0x011355146fd56395_u64, 0x40bdf15d4a672e32_u64, 0xd021ff5cd13a2ed5_u64, 0x9605d5f0e25ec3b0_u64, 0x1a083822ceafe02d_u64, 0x0d7e765d58755c10_u64,
        0xe83a908ff2fb60ca_u64, 0x0fbbad1f61042279_u64, 0x3290ac3a203001bf_u64, 0x75834465489c0c89_u64, 0x9c15f73e62a76ae2_u64, 0x44db015024623547_u64, 0x2af7398005aaa5c7_u64, 0x9d39247e33776d41_u64,
        0x12a8f216af9418c2_u64, 0xd4490ad526f14431_u64, 0xb49c3b3995091a36_u64, 0x5b45e522e4b1b4ef_u64, 0xa1e9300cd8520548_u64, 0x49787fef17af9924_u64, 0x03219a39ee587a30_u64, 0xebe9ea2adf4321c7_u64,
        0x804456af10f5fb53_u64, 0xd74bbe77e6116ac7_u64, 0x7c0828dd624ec390_u64, 0x14a195640116f336_u64, 0x2eab8ca63ce802d7_u64, 0xc6e57a78fbd986e0_u64, 0x58efc10b06a2068d_u64, 0xabeeddb2dde06ff1_u64,
        0x0b090a7560a968e3_u64, 0x2cf9c8ca052f6e9f_u64, 0x116d0016cb948f09_u64, 0xa59e0bd101731a28_u64, 0x63767572ae3d6174_u64, 0xab4f6451cc1d45ec_u64, 0xc2a1e7b5b459aeb5_u64, 0x2472f6207c2d0484_u64,
        0xe699ed85b0dfb40d_u64, 0xd4347f66ec8941c3_u64, 0xf4d14597e660f855_u64, 0x8b889d624d44885d_u64, 0x258e5a80c7204c4b_u64, 0xaf0c317d32adaa8a_u64, 0x9c4cd6257c5a3603_u64, 0xeb3593803173e0ce_u64,
        0x36f60e2ba4fa6800_u64, 0x38b6525c21a42b0e_u64, 0xf4f5d05c10cab243_u64, 0xcf3f4688801eb9aa_u64, 0x1ddc0325259b27de_u64, 0xb9571fa04dc089c8_u64, 0xd7504dfa8816edbb_u64, 0x1fe2cca76517db90_u64,
        0x261e4e4c0a333a9d_u64, 0x219b97e26ffc81bd_u64, 0x66b4835d9eafea22_u64, 0x4cc317fb9cddd023_u64, 0x50b704cab602c329_u64, 0xedb454e7badc0805_u64, 0x9e17e49642a3e4c1_u64, 0x66c1a2a1a60cd889_u64,
        0x7983eed3740847d5_u64, 0x298af231c85bafab_u64, 0x2680b122baa28d97_u64, 0x734de8181f6ec39a_u64, 0x53898e4c3910da55_u64, 0x1761f93a44d5aefe_u64, 0xe4dbf0634473f5d2_u64, 0x4ed0fe7e9dc91335_u64,
        0xd18d8549d140caea_u64, 0x1cfc8bed0d681639_u64, 0xca1e3785a9e724e5_u64, 0xb67c1fa481680af8_u64, 0xdfea21ea9e7557e3_u64, 0xd6b6d0ecc617c699_u64, 0xfa7e393983325753_u64, 0xa09e8c8c35ab96de_u64,
        0x8fe88b57305e2ab6_u64, 0x89039d79d6fc5c5c_u64, 0x9bfb227ebdf4c5ce_u64, 0x7f7cc39420a3a545_u64, 0x3f6c6af859d80055_u64, 0xc8763c5b08d1908c_u64, 0x469356c504ec9f9d_u64, 0x26e6db8ffdf5adfe_u64,
        0x3a938fee32d29981_u64, 0x2c5e9deb57ef4743_u64, 0x1e99b96e70a9be8b_u64, 0x764dbeae7fa4f3a6_u64, 0xaac40a2703d9bea0_u64, 0x1a8c1e992b941148_u64, 0x73aa8a564fb7ac9e_u64, 0x604d51b25fbf70e2_u64,
        0xdd69a0d8ab3b546d_u64, 0x65ca5b96b7552210_u64, 0x2fd7e4b9e72cd38c_u64, 0x51d2b1ab2ddfb636_u64, 0x9d1d84fcce371425_u64, 0xa44cfe79ae538bbe_u64, 0xde68a2355b93cae6_u64, 0x9fc10d0f989993e0_u64,
        0x94ebc8abcfb56dae_u64, 0xd7a023a73260b45c_u64, 0x72c8834a5957b511_u64, 0x8f8419a348f296bf_u64, 0x1e152328f3318dea_u64, 0x4838d65f6ef6748f_u64, 0xd6bf7baee43cac40_u64, 0x13328503df48229f_u64,
        0x7440fb816508c4fe_u64, 0x9d266d6a1cc0542c_u64, 0x4dda48153c94938a_u64, 0x74c04bf1790c0efe_u64, 0xe1925c71285279f5_u64, 0x8a8e849eb32781a5_u64, 0x073973751f12dd5e_u64, 0xa319ce15b0b4db31_u64,
        0x6dd856d94d259236_u64, 0x67378d8eccef96cb_u64, 0x9fc477de4ed681da_u64, 0xf3b8b6675a6507ff_u64, 0xc3a9dc228caac9e9_u64, 0xc37b45b3f8d6f2ba_u64, 0xb559eb1d04e5e932_u64, 0x1b0cab936e65c744_u64,
        0xaf08da9177dda93d_u64, 0xac12fb171817eee7_u64, 0x1fff7ac80904bf45_u64, 0xa9119b60369ffebd_u64, 0xbfced1b0048eac50_u64, 0xb67b7896167b4c84_u64, 0x9b3cdb65f82ca382_u64, 0xdbc27ab5447822bf_u64,
        0x10dcd78e3851a492_u64, 0xb438c2b67f98e5e9_u64, 0x43954b3252dc25e5_u64, 0xab9090168dd05f34_u64, 0xce68341f79893389_u64, 0x36833336d068f707_u64, 0xdcdd7d20903d0c25_u64, 0xda3a361b1c5157b1_u64,
        0x7f9d1a2e1ebe1327_u64, 0x5d0a12f27ad310d1_u64, 0x3bc36e078f7515d7_u64, 0x4da8979a0041e8a9_u64, 0x950113646d1d6e03_u64, 0x7b4a38e32537df62_u64, 0x8a1b083821f40cb4_u64, 0x3d5774a11d31ab39_u64,
        0x7a76956c3eafb413_u64, 0x7f5126dbba5e0ca7_u64, 0x12153635b2c0cf57_u64, 0x7b3f0195fc6f290f_u64, 0x5544f7d774b14aef_u64, 0x56c074a581ea17fe_u64, 0xe7f28ecd2d49eecd_u64, 0xe479ee5b9930578c_u64,
        0x9ff38fed72e9052f_u64, 0x9f65789a6509a440_u64, 0x0981dcd296a8736d_u64, 0x5873888850659ae7_u64, 0xc678b6d860284a1c_u64, 0x63e22c147b9c3403_u64, 0x92fae24291f2b3f1_u64, 0x829626e3892d95d7_u64,
        0xcffe1939438e9b24_u64, 0x79999cdff70902cb_u64, 0x8547eddfb81ccb94_u64, 0x7b77497b32503b12_u64, 0x97fcaacbf030bc24_u64, 0x6ced1983376fa72b_u64, 0x7e75d99d94a70f4d_u64, 0xd2733c4335c6a72f_u64,
        0xdbc0d2b6ab90a559_u64, 0x94628d38d0c20584_u64, 0x64972d68dee33360_u64, 0xb9c11d5b1e43a07e_u64, 0x2de0966daf2f8b1c_u64, 0x2e18bc1ad9704a68_u64, 0xd4dba84729af48ad_u64, 0xb7a0b174cff6f36e_u64,
        0xe94c39a54a98307f_u64, 0xaa70b5b4f89695a2_u64, 0x3bdbb92c43b17f26_u64, 0xcccb7005c6b9c28d_u64, 0x18a6a990c8b35ebd_u64, 0xfc7c95d827357afa_u64, 0x1fca8a92fd719f85_u64, 0x1dd01aafcd53486a_u64,
        0x49353fea39ba63b1_u64, 0xf85b2b4fbcde44b7_u64, 0xbe7444e39328a0ac_u64, 0x3e2b8bcbf016d66d_u64, 0x964e915cd5e2b207_u64, 0x1725cabfcb045b00_u64, 0x7fbf21ec8a1f45ec_u64, 0x11317ba87905e790_u64,
        0x2fe4b17170e59750_u64, 0xe8d9ecbe2cf3d73f_u64, 0xb57d2e985e1419c7_u64, 0x0572b974f03ce0bb_u64, 0xa8d7e4dab780a08d_u64, 0x4715ed43e8a45c0a_u64, 0xc330de426430f69d_u64, 0x23b70edb1955c4bf_u64,
        0x098954d51fff6580_u64, 0x8107fccf064fcf56_u64, 0x852f54934da55cc9_u64, 0x09c7e552bc76492f_u64, 0xe9f6760e32cd8021_u64, 0xa3bc941d0a5061cb_u64, 0xba89142e007503b8_u64, 0xdc842b7e2819e230_u64,
        0xbbe83f4ecc2bdecb_u64, 0xcd454f8f19c5126a_u64, 0xc62c58f97dd949bf_u64, 0x693501d628297551_u64, 0xb9ab4ce57f2d34f3_u64, 0x9255abb50d532280_u64, 0xebfafa33d7254b59_u64, 0xe9f6082b05542e4e_u64,
        0x35dd37d5871448af_u64, 0xb03031a8b4516e84_u64, 0xb3f256d8aca0b0b9_u64, 0x0fd22063edc29fca_u64, 0xd9a11fbb3d9808e4_u64, 0x3a9bf55ba91f81ca_u64, 0xc8c93882f9475f5f_u64, 0x947ae053ee56e63c_u64,
        0xc7d9f16864a76e94_u64, 0x7bd94e1d8e17debc_u64, 0xd873db391292ed4f_u64, 0x30f5611484119414_u64, 0x565c31f7de89ea27_u64, 0xd0e4366228b03343_u64, 0x325928ee6e6f8794_u64, 0x6f423357e7c6a9f9_u64,
        0x99170a5dc3115544_u64, 0x59b97885e2f2ea28_u64, 0xbc4097b116c524d2_u64, 0x7a13f18bbedc4ff5_u64, 0x071582401c38434d_u64, 0xb422061193d6f6a7_u64, 0xb4b81b3fa97511e2_u64, 0x65d34954daf3cebd_u64,
        0xb344c470397bba52_u64, 0xbac7a9a18531294b_u64, 0xecb53939887e8175_u64, 0x565601c0364e3228_u64, 0xef1955914b609f93_u64, 0x16f50edf91e513af_u64, 0x56963b0dca418fc0_u64, 0xd60f6dcedc314222_u64,
        0x364f6ffa464ee52e_u64, 0x6c3b8e3e336139d3_u64, 0xf943aee7febf21b8_u64, 0x088e049589c432e0_u64, 0xd49503536abca345_u64, 0x3a6c27934e31188a_u64, 0x957baf61700cff4e_u64, 0x37624ae5a48fa6e9_u64,
        0x501f65edb3034d07_u64, 0x907f30421d78c5de_u64, 0x1a804aadb9cfa741_u64, 0x0ce2a38c344a6eed_u64, 0xd363eff5f0977996_u64, 0x2cd16e2abd791e33_u64, 0x58627e1a149bba21_u64, 0x7f9b6af1ebf78baf_u64,
    };

    Isaac64 rng;
    uint64_t x = 0;
    for (size_t i = 0; i < 512; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect[i]);
    }

    uint64_t a1 = 0, a2 = 0, b1 = 0, b2 = 0, c1 = 0, c2 = 0, d1 = 0, d2 = 0;
    TRY((rng = Isaac64{123, 456, 789}));
    TRY(a1 = rng());
    TRY(b1 = rng());
    TRY(c1 = rng());
    TRY(d1 = rng());
    TRY((rng = Isaac64{123, 456, 7890}));
    TRY(a2 = rng());  TEST_COMPARE(a1, !=, a2);
    TRY(b2 = rng());  TEST_COMPARE(b1, !=, b2);
    TRY(c2 = rng());  TEST_COMPARE(c1, !=, c2);
    TRY(d2 = rng());  TEST_COMPARE(d1, !=, d2);
    TRY(rng.seed({123, 456, 789}));
    TRY(a2 = rng());  TEST_EQUAL(a1, a2);
    TRY(b2 = rng());  TEST_EQUAL(b1, b2);
    TRY(c2 = rng());  TEST_EQUAL(c1, c2);
    TRY(d2 = rng());  TEST_EQUAL(d1, d2);

}

void test_core_random_pcg32() {

    static constexpr uint32_t expect1[64] = {
        0x285594ea_u32, 0x190ca349_u32, 0xcbc42ff2_u32, 0xd6508153_u32, 0xc2a8052f_u32, 0x0f55ac5f_u32, 0xd6ff3e32_u32, 0x4f46689c_u32,
        0x64cca611_u32, 0xa07aa05b_u32, 0x55a65cbc_u32, 0xcafd62a0_u32, 0xbe8a223c_u32, 0x79741856_u32, 0x6aadd40e_u32, 0xd922b1c4_u32,
        0xbad2063f_u32, 0xa3b8bcc9_u32, 0xad4dabdc_u32, 0xddbb8195_u32, 0x49cf1bfd_u32, 0x6833c7a9_u32, 0xb88e2dd0_u32, 0xb3716c08_u32,
        0xf36cdf1a_u32, 0x3707a12d_u32, 0xa5879dc6_u32, 0xea427434_u32, 0xae1b79dd_u32, 0x112a5374_u32, 0xca377628_u32, 0x45ce2945_u32,
        0x3cc58b87_u32, 0xfc51cd35_u32, 0x747c67f0_u32, 0x26f8e9a2_u32, 0x16fdabc4_u32, 0x754a6466_u32, 0x762f71be_u32, 0x370b5486_u32,
        0x4641e35c_u32, 0x3a6fe471_u32, 0x813fc7bc_u32, 0x3964850f_u32, 0xe2c1f3ef_u32, 0x71971188_u32, 0xc414d802_u32, 0x176d56d1_u32,
        0xf2e9a9eb_u32, 0x9497380a_u32, 0xcc701aa9_u32, 0xea754304_u32, 0x10cb690e_u32, 0xbdf7f707_u32, 0x1bb50ea5_u32, 0x13375d59_u32,
        0x86dca492_u32, 0xb6c6e447_u32, 0x71e27e2e_u32, 0x1dfd92d0_u32, 0x4a14a45d_u32, 0x670171b2_u32, 0x0b2daa8b_u32, 0xb7dac5bd_u32,
    };

    static constexpr uint32_t expect2[64] = {
        0xf37d0d8c_u32, 0x3785e83f_u32, 0x9da559d4_u32, 0x9615de03_u32, 0x7e664cb1_u32, 0x514a2ac3_u32, 0x61eebd9f_u32, 0x853c1644_u32,
        0xbcd09a93_u32, 0x86154d6c_u32, 0x0869cace_u32, 0x9ec531e5_u32, 0x7dc9cf69_u32, 0xe07a82ae_u32, 0x2e54f2a1_u32, 0x51758f20_u32,
        0x8b0d0a5b_u32, 0xf1c233c8_u32, 0x2a1a80b4_u32, 0x66454fd2_u32, 0x278b120a_u32, 0x1d12b5b3_u32, 0x97dbd108_u32, 0xef117d3b_u32,
        0xa7e00eab_u32, 0x82d03a76_u32, 0x305b293c_u32, 0x36fe743f_u32, 0xe622efd6_u32, 0x30baa20c_u32, 0x9ee8d9f7_u32, 0xf0ca55f8_u32,
        0x3930ed28_u32, 0x7b872021_u32, 0x56072976_u32, 0xa65574a6_u32, 0x57530966_u32, 0x9a68cc9b_u32, 0x074e55fd_u32, 0x135078e7_u32,
        0xbf8846de_u32, 0xe3be0771_u32, 0xb9ffa45d_u32, 0x96febdea_u32, 0x83c8b51c_u32, 0x49ac1b86_u32, 0x340147e0_u32, 0x25030038_u32,
        0x65926cda_u32, 0x915f8c66_u32, 0x2d694949_u32, 0x21709f00_u32, 0x023a9c09_u32, 0x89658dce_u32, 0x70fed5f0_u32, 0xcc1efe69_u32,
        0xbb746068_u32, 0x042b2feb_u32, 0x28405f9d_u32, 0xbca6afa5_u32, 0x6506eb31_u32, 0x3dceac66_u32, 0xc21fe5ed_u32, 0x5ff084aa_u32,
    };

    Pcg32 rng;
    uint32_t x = 0;

    for (size_t i = 0; i < 64; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect1[i]);
    }

    TRY(rng.advance(10000));

    for (size_t i = 0; i < 64; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect2[i]);
    }

    TRY(rng.advance(-10128));

    for (size_t i = 0; i < 64; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect1[i]);
    }

}

void test_core_random_pcg64() {

    static constexpr uint64_t expect1[64] = {
        0xcf7dbe684e0c4045_u64, 0x15642875dfe1e67c_u64, 0x32f049df2f50d811_u64, 0x98c1d0a163e1f856_u64, 0x0743e58f0360d766_u64, 0xfee9e4fc03479bf6_u64, 0xf7cecb1626639417_u64, 0x2df859db95762826_u64,
        0x1c3f93812e148ea4_u64, 0x8af3831f2bb85205_u64, 0x18a8332dd6ffefc9_u64, 0xf4d9e8a9f8a24f80_u64, 0x93ae319fc35b2252_u64, 0x72db88f7e0fc64e7_u64, 0x33c3afd951365990_u64, 0xb466b761b08a33e0_u64,
        0x819bd59c954697d9_u64, 0x1e12dfcf3603d13c_u64, 0xb65d9c1f42631174_u64, 0x879c9407826979cd_u64, 0x2e280ad2288f501c_u64, 0x1c4a7b2394379f32_u64, 0x7f0a705c9e02c72d_u64, 0xbe49b435ae70f9da_u64,
        0xc68541ce0c31758d_u64, 0x906d8d985d14140f_u64, 0xe37687a639e59e4b_u64, 0x4c86f84e1bd6f7a6_u64, 0x8d94ae5bc2ef4fff_u64, 0x399f88cdfc35c9b8_u64, 0xbd2b35c11c2ebda4_u64, 0xfd3b3cc791f4c41b_u64,
        0x4cee57ea75a2c553_u64, 0x3ee40429e17e08cc_u64, 0xd4e8b915a77f55d2_u64, 0x17b0c80b5f477bbd_u64, 0x32eab93502849a9a_u64, 0xe45997b64cd4b70d_u64, 0x344b89a2783b3d44_u64, 0x0dd1cfdd02d36391_u64,
        0x3fac997d87350aa6_u64, 0x42c087c945dd1658_u64, 0x074224ac0bb4ca7f_u64, 0xec19189ce13933f5_u64, 0xdb6f436377243bc7_u64, 0xdd86c64278e57554_u64, 0x97a066f7213010c5_u64, 0x51fee8d406fb0559_u64,
        0x1be04809d240d333_u64, 0xdb426032688020a3_u64, 0x4a23d5455f317b2c_u64, 0x10ef6b205c4b4f76_u64, 0xa44ab2304e80c901_u64, 0xda796dab0f570ff7_u64, 0xebbe0cdee2932dfd_u64, 0xe658e297513b80be_u64,
        0xb2fcbccc4873715b_u64, 0x0fbf3bb2ff94dade_u64, 0xf5c952ff75414f85_u64, 0x94faed41788ec226_u64, 0xb699cd1dbc639cd9_u64, 0xc402a50c737e0a62_u64, 0x95fac59a4269c9ba_u64, 0x84ee24cf64cb80a4_u64,
    };

    static constexpr uint64_t expect2[64] = {
        0x7e6848856818e376_u64, 0xc7a3ca187fec4f97_u64, 0x4bf59a3922b15522_u64, 0x84244cd3df75a1ea_u64, 0xadbfc289c379431e_u64, 0xb86eb9b4b36da00e_u64, 0x474b9debd7a7061c_u64, 0x33540fe3b9639b67_u64,
        0x4625c32bc26b36bf_u64, 0x70211a03b27eee0f_u64, 0x6877db4a31b90b48_u64, 0x7161475b7d80ef34_u64, 0x9b48966ab8d23125_u64, 0x6859d41c6031442c_u64, 0xb1900069cfdc4a61_u64, 0xc186f98c1f49d15b_u64,
        0x2a221618a7166525_u64, 0x6045f3cc7d9b36fb_u64, 0x5e1c6a28a062c630_u64, 0x0fa1e30643142ee7_u64, 0xd3c81d7f6f9a914b_u64, 0xa53ea6514fb62041_u64, 0x2db6d388d913d064_u64, 0xef3001ba12c18df1_u64,
        0x79a0baf73c9979d7_u64, 0xcada9fb5525cabdf_u64, 0x69a17a687c33c840_u64, 0x62226119de06c52d_u64, 0x95dd4f9f110a9364_u64, 0x53fc5e5707f293c2_u64, 0x06136b7d84ba1ac6_u64, 0xd1304cd13f4622e9_u64,
        0x6208fe0669471579_u64, 0xe8bc14ef466a8241_u64, 0x28865467fc3ac6e8_u64, 0x4aa88ba50ab2a873_u64, 0x1eca26cea602cad8_u64, 0xe42dfeb796cab0e0_u64, 0xd8df1f373279c71a_u64, 0x9e164fe39fa91d64_u64,
        0x1c00eed28930bdf9_u64, 0x05e473b7aaa7ae1d_u64, 0xb3356dd9fde7c6bf_u64, 0xd2f33ea8f01975f7_u64, 0x35e4c439c8993246_u64, 0x4e93dab021b56544_u64, 0x92f74cf4f789c20d_u64, 0x1bac6300fa685808_u64,
        0xbe35bb875dbe449a_u64, 0x87ef616ccab2c820_u64, 0x85fbbbe9ad490394_u64, 0xd96176fa0e020ca1_u64, 0x826dddfad7c6c56c_u64, 0x978130b7471ba6f3_u64, 0x5de517ad6dda439d_u64, 0xba1fb2546d0df742_u64,
        0x2468efff49b621c6_u64, 0xfb4067133c556926_u64, 0x6c500e693ac7a271_u64, 0x1481512e2093b0fe_u64, 0x08a84c9076d8e712_u64, 0x8655ad7afe943fe1_u64, 0x184e7e0560174c53_u64, 0x28f8ca20ed34daee_u64,
    };

    Pcg64 rng;
    uint64_t x = 0;

    for (size_t i = 0; i < 64; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect1[i]);
    }

    TRY(rng.advance(10000));

    for (size_t i = 0; i < 64; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect2[i]);
    }

    TRY(rng.advance(-10128));

    for (size_t i = 0; i < 64; ++i) {
        TRY(x = rng());
        TEST_EQUAL(x, expect1[i]);
    }

}

void test_core_random_splitmix64() {

    static constexpr uint64_t expect[] = {
        0xe220a8397b1dcdaf_u64, 0x6e789e6aa1b965f4_u64, 0x06c45d188009454f_u64, 0xf88bb8a8724c81ec_u64, 0x1b39896a51a8749b_u64, 0x53cb9f0c747ea2ea_u64, 0x2c829abe1f4532e1_u64, 0xc584133ac916ab3c_u64,
        0x3ee5789041c98ac3_u64, 0xf3b8488c368cb0a6_u64, 0x657eecdd3cb13d09_u64, 0xc2d326e0055bdef6_u64, 0x8621a03fe0bbdb7b_u64, 0x8e1f7555983aa92f_u64, 0xb54e0f1600cc4d19_u64, 0x84bb3f97971d80ab_u64,
        0x7d29825c75521255_u64, 0xc3cf17102b7f7f86_u64, 0x3466e9a083914f64_u64, 0xd81a8d2b5a4485ac_u64, 0xdb01602b100b9ed7_u64, 0xa9038a921825f10d_u64, 0xedf5f1d90dca2f6a_u64, 0x54496ad67bd2634c_u64,
        0xdd7c01d4f5407269_u64, 0x935e82f1db4c4f7b_u64, 0x69b82ebc92233300_u64, 0x40d29eb57de1d510_u64, 0xa2f09dabb45c6316_u64, 0xee521d7a0f4d3872_u64, 0xf16952ee72f3454f_u64, 0x377d35dea8e40225_u64,
        0x0c7de8064963bab0_u64, 0x05582d37111ac529_u64, 0xd254741f599dc6f7_u64, 0x69630f7593d108c3_u64, 0x417ef96181daa383_u64, 0x3c3c41a3b43343a1_u64, 0x6e19905dcbe531df_u64, 0x4fa9fa7324851729_u64,
        0x84eb4454a792922a_u64, 0x134f7096918175ce_u64, 0x07dc930b302278a8_u64, 0x12c015a97019e937_u64, 0xcc06c31652ebf438_u64, 0xecee65630a691e37_u64, 0x3e84ecb1763e79ad_u64, 0x690ed476743aae49_u64,
        0x774615d7b1a1f2e1_u64, 0x22b353f04f4f52da_u64, 0xe3ddd86ba71a5eb1_u64, 0xdf268adeb6513356_u64, 0x2098eb73d4367d77_u64, 0x03d6845323ce3c71_u64, 0xc952c5620043c714_u64, 0x9b196bca844f1705_u64,
        0x30260345dd9e0ec1_u64, 0xcf448a5882bb9698_u64, 0xf4a578dccbc87656_u64, 0xbfdeaed9a17b3c8f_u64, 0xed79402d1d5c5d7b_u64, 0x55f070ab1cbbf170_u64, 0x3e00a34929a88f1d_u64, 0xe255b237b8bb18fb_u64,
    };

    SplitMix64 rng;
    uint64_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoroshiro64s() {

    static constexpr uint32_t expect[] = {
        0x3795f5d5_u32, 0x2214879f_u32, 0x93bce8f4_u32, 0xeae89f28_u32, 0x043c51be_u32, 0xe135e838_u32, 0xf468a321_u32, 0x37239236_u32,
        0x16763156_u32, 0xac4a190f_u32, 0x2402e88b_u32, 0xe83cd88a_u32, 0x496b43cb_u32, 0x8c5c9931_u32, 0xd9910ad5_u32, 0xd6ebf60b_u32,
        0xda554908_u32, 0x71cd89dc_u32, 0xd190ad83_u32, 0xe499cf7c_u32, 0x0467e2f1_u32, 0xef4f0ccc_u32, 0x64c26321_u32, 0x75ceaad5_u32,
        0x2681f469_u32, 0xb8fda07d_u32, 0x271ca098_u32, 0x0c731aa5_u32, 0x6e917801_u32, 0x67402d28_u32, 0xd2152bdc_u32, 0xe24754a4_u32,
        0xd09b0758_u32, 0xe1b03282_u32, 0x7229dc1e_u32, 0xdb8906c3_u32, 0x0afc1507_u32, 0x3abc2f34_u32, 0x81a785fa_u32, 0xe83f2cf4_u32,
        0xf6a7c913_u32, 0x0b3d2791_u32, 0xae9027c0_u32, 0xbb7c54f2_u32, 0x9b3b4460_u32, 0xf93a0776_u32, 0x6608bb91_u32, 0xcd5b36ec_u32,
        0x3263d76c_u32, 0x5dbcb360_u32, 0x6394c104_u32, 0x7375b635_u32, 0x5e7d1b44_u32, 0x815a730b_u32, 0xb0844eb0_u32, 0xe682396a_u32,
        0xc4998351_u32, 0x70f20464_u32, 0xf0e24b5a_u32, 0x093ae4ad_u32, 0x98d8531f_u32, 0xb3d719c3_u32, 0x397c1b5f_u32, 0xfd21b73b_u32,
    };

    Xoroshiro64s rng;
    uint32_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoroshiro64ss() {

    static constexpr uint32_t expect[] = {
        0xbdb9a53e_u32, 0x4cd4c374_u32, 0x561198da_u32, 0xd1637991_u32, 0xa5b316c0_u32, 0xc1b1238c_u32, 0xc165f536_u32, 0x763b61de_u32,
        0x09ded5ca_u32, 0xae4fa9c9_u32, 0x81d156f4_u32, 0x260756d1_u32, 0xe30a5f0d_u32, 0xb9dfbef5_u32, 0xfaa6c5a7_u32, 0x5379c762_u32,
        0x754da587_u32, 0x207629c6_u32, 0xfa6c7262_u32, 0xe021ae0c_u32, 0xc0edd6a0_u32, 0x91680011_u32, 0xf97df4dc_u32, 0xa12ac566_u32,
        0x1138c1b4_u32, 0x9e844e93_u32, 0x71e45f14_u32, 0xc7f0a725_u32, 0x1aeb00e1_u32, 0x881c393c_u32, 0x4d3b6a02_u32, 0x6c94e70c_u32,
        0x60e49782_u32, 0x0e1f91cc_u32, 0x5a299306_u32, 0x35a43a67_u32, 0xdd8d2465_u32, 0xb59d80a3_u32, 0x08b3bc90_u32, 0x277c1911_u32,
        0x28ddac76_u32, 0x0638baa5_u32, 0x1a18d869_u32, 0x2db517b3_u32, 0x050abc5f_u32, 0xc444aa5b_u32, 0xc5753adc_u32, 0x590253fd_u32,
        0x7e66a39e_u32, 0x95f01c37_u32, 0x3cf8a2bc_u32, 0x2991e166_u32, 0x0e310ab7_u32, 0xd887e730_u32, 0x52b12e6e_u32, 0x1163e2cc_u32,
        0xdff21318_u32, 0x9742bec6_u32, 0x8d6f18d6_u32, 0xc4ceec25_u32, 0x8733f3bf_u32, 0x66701a4e_u32, 0xed911b83_u32, 0x3512857b_u32,
    };

    Xoroshiro64ss rng;
    uint32_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoroshiro128p() {

    static constexpr uint64_t expect[] = {
        0x509946a41cd733a3_u64, 0xd805fcac6824536e_u64, 0xdadc02f3e3cf7be3_u64, 0x622e4dd99d2720e5_u64, 0xaacfd52d630b52bd_u64, 0xa94fc32eb4128023_u64, 0x9ee359839e68f625_u64, 0xd9f180e03b686e4f_u64,
        0xd6825e7d8fc65068_u64, 0x0887f15071c20b9d_u64, 0x6dc39f8336eeaa66_u64, 0x013d17509661b69b_u64, 0xdbe703ea4e61caec_u64, 0x1a4deda7c51c5b7b_u64, 0xe2f2259fb30bafcc_u64, 0x7eb5a4d5f053fcbf_u64,
        0x4704d55257921919_u64, 0xcfeb1c70eacd6734_u64, 0xed98c92a0d6b8b3e_u64, 0x4efb928a052188b7_u64, 0x15617edcea5e98ab_u64, 0x8ac022e71a4d1a40_u64, 0xe0ae2cdf81cc05bf_u64, 0x11ae6d329bc72f19_u64,
        0x5369885a834c1073_u64, 0x7a865692c8495a12_u64, 0xaf752d7df50f6968_u64, 0x4b81c799c968e005_u64, 0x4104e06972751b34_u64, 0x8600214cf598d6f6_u64, 0x444545884a4b0a80_u64, 0x2d13243847e43cfe_u64,
        0x6064921c3b70601c_u64, 0x1b2c2f204185130e_u64, 0xac1e21160f7e90f4_u64, 0xa718d564118e2bca_u64, 0x25fb8750f330bdc1_u64, 0x0cdd8329cb365e06_u64, 0xfdcfbff05c3470e3_u64, 0xcbce143aec5155a5_u64,
        0x01d17b5b1e2c3c21_u64, 0x68fe2fbabc30aa23_u64, 0x19086e8dbd448c02_u64, 0xdb7d8126e6f3d1c6_u64, 0x1865e34fb131a69f_u64, 0xce3be151debb3e9a_u64, 0xdf573313ce569b70_u64, 0x3a7fcf8ef4fd495a_u64,
        0xe26450c5ec487bcc_u64, 0x00e99eaeeb35354e_u64, 0x959e7e6cb8bf55d4_u64, 0x3ba4778a79b1b758_u64, 0x30e4f35a940c2e04_u64, 0x67717bb8a50f2c22_u64, 0xa9b3e9db4934cd8e_u64, 0xe22bc184e5d2ad8d_u64,
        0x7390583f39dfbb76_u64, 0x19e7ba95b2482b72_u64, 0x549b0c65abc1615f_u64, 0x43989e0d7268118a_u64, 0x1376e3b4f7319b9c_u64, 0x41bc4dd69e4a3eca_u64, 0xdb5b777a0a90e830_u64, 0x4885cae86597a2fd_u64,
    };

    Xoroshiro128p rng;
    uint64_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoroshiro128ss() {

    static constexpr uint64_t expect[] = {
        0xdec90d521e93e35d_u64, 0x6d33ac6f18895e08_u64, 0xab21904eec6fa48a_u64, 0x87afdbc188423fbe_u64, 0x64c1fc972d0be37f_u64, 0x747f0f97a0cd98a5_u64, 0x489b61f9632ef474_u64, 0x271460c1092811f0_u64,
        0xff0e6c13c9f16f25_u64, 0xb56a31da37149983_u64, 0x723cf04fbd1ed4ff_u64, 0x56005057f9b3c167_u64, 0x651fdcd95e939ac4_u64, 0xcef2e967455e8ec5_u64, 0xa5d2339bc18f65c2_u64, 0x1d78a2b9225b2d0b_u64,
        0xba1d2f21d5cd5a53_u64, 0x4285a103cfe4d0f4_u64, 0x9e08c676b3bfacd9_u64, 0x98408859d60e39c6_u64, 0xc7fe90ba06b117d7_u64, 0x8d4593fe66f33a21_u64, 0x55cfc98da3a10a09_u64, 0x7be557d58d92032e_u64,
        0x6684625db992b15f_u64, 0xbd2697ac0075de4a_u64, 0x2c22301f50be9f7f_u64, 0xa3213ec8380d1802_u64, 0x4bd7ac2db68daf02_u64, 0x1bf4239726976c9c_u64, 0x533a75a8b9608877_u64, 0xb553616c53cd1d95_u64,
        0x8a0adb01b456085c_u64, 0xf487d847bde8cd13_u64, 0xebe2aef97ce43cbf_u64, 0x5c738c6117fde71a_u64, 0xac04a59edcde7d9f_u64, 0xa3d5368934b5869a_u64, 0xf618422a9cfff4b6_u64, 0xf7107446daa38c0f_u64,
        0xb7af4f8f4bb041c5_u64, 0x593c4eb86ccf1640_u64, 0x7409f19ac95b9690_u64, 0xc22923f095ba2952_u64, 0x637c0241e949fe10_u64, 0xd4e3f6bec873e93d_u64, 0xf589fb02ee70952b_u64, 0x351665af60d237b4_u64,
        0x207e20bf0be9ddff_u64, 0xc9f9d12430ccc59f_u64, 0xda4860db4a3219b8_u64, 0x83929c51fe2345f3_u64, 0xbae2a4f4521110cb_u64, 0xbad3f5abebd97e76_u64, 0x0d817e828a254c05_u64, 0x1be5283403ca04ed_u64,
        0x913c90a1a2896963_u64, 0x7ab003dfe38381c3_u64, 0x24fa75fe2ea53355_u64, 0x48bcf534dc08c41d_u64, 0xb4ed3644df9b9c1d_u64, 0x84532e01d5f62e96_u64, 0x28b18f91ccbe5a7f_u64, 0xb7b4cf4443d7ba70_u64,
    };

    Xoroshiro128ss rng;
    uint64_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoshiro128p() {

    static constexpr uint32_t expect[] = {
        0xed6a4f9b_u32, 0x5808f056_u32, 0xc6c161e8_u32, 0x87837269_u32, 0x926c39cb_u32, 0xaa28e207_u32, 0x1967d667_u32, 0x284a00cd_u32,
        0x1cc16af1_u32, 0x05562c54_u32, 0x2651882e_u32, 0xcbd6a196_u32, 0xe840c9cc_u32, 0xf6aec4ce_u32, 0xc9533372_u32, 0xa8cefc97_u32,
        0xdc0f9422_u32, 0xc97e9a1e_u32, 0x82785f4e_u32, 0xc0e2e5f8_u32, 0x175e882e_u32, 0x4d280126_u32, 0x41948197_u32, 0x5f693e28_u32,
        0x3b398e5b_u32, 0x7671574b_u32, 0xcb7bc615_u32, 0xa119f128_u32, 0xee77b120_u32, 0xdaa38801_u32, 0x3e32fda8_u32, 0xb7938030_u32,
        0x9067bdd9_u32, 0x6e3c4fd6_u32, 0x29d322b6_u32, 0x207e4218_u32, 0x535a1cbd_u32, 0x80b3d77f_u32, 0x9bc4a1a4_u32, 0x2dd85ae6_u32,
        0xa6e415ad_u32, 0x4cb4f461_u32, 0xaf463cbc_u32, 0xbe733f4e_u32, 0x32f70cb7_u32, 0x8874031e_u32, 0x16d40503_u32, 0x8d7fdb92_u32,
        0x368e4e7e_u32, 0x6fc2d778_u32, 0x72bc8eef_u32, 0xe463c0d7_u32, 0xbf7202dc_u32, 0xcdc89669_u32, 0x0b2e9d83_u32, 0x89f53a13_u32,
        0x8de8dcc3_u32, 0xb982c498_u32, 0xd0c937ff_u32, 0xe838e5bb_u32, 0xae0f534f_u32, 0xe794ca4c_u32, 0x0456916e_u32, 0x2e4a3e83_u32,
    };

    Xoshiro128p rng;
    uint32_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoshiro128ss() {

    static constexpr uint32_t expect[] = {
        0x1e93e34b_u32, 0x67aa96d6_u32, 0x60b249b1_u32, 0xe9750dff_u32, 0xb13097b3_u32, 0xe6b95566_u32, 0xa0f96e32_u32, 0x3a8f63d4_u32,
        0x0e0a1364_u32, 0x8ef9a37f_u32, 0xffce15bb_u32, 0x7de23908_u32, 0x3ed7642e_u32, 0xf7354168_u32, 0x7c3facd5_u32, 0xe25c2b99_u32,
        0x2f9b0c30_u32, 0x61f6b941_u32, 0xd2569373_u32, 0x19a60a38_u32, 0xd887a3a2_u32, 0x2784415c_u32, 0xd63b1976_u32, 0x4054d2c8_u32,
        0xcec5f8cf_u32, 0xec4831a8_u32, 0x6effc1df_u32, 0xe6411bd8_u32, 0x49276f78_u32, 0x84147a74_u32, 0x5e5dbe1f_u32, 0x90817412_u32,
        0x75fb6cec_u32, 0x3146f7ed_u32, 0x8931b77b_u32, 0x9398dedf_u32, 0xa93a7549_u32, 0xed484db6_u32, 0xc7d0bdce_u32, 0x460c2f9d_u32,
        0x16b5cd94_u32, 0x7ed1c47c_u32, 0x213bcd58_u32, 0x19a68ec5_u32, 0xeb378961_u32, 0x30cc69a9_u32, 0x66b03488_u32, 0x2ebd705b_u32,
        0x6378d0cb_u32, 0xa8e13335_u32, 0x0744ea53_u32, 0xc7026fbb_u32, 0x4df587cd_u32, 0xa719dc9d_u32, 0xd9a190c2_u32, 0x62df092c_u32,
        0x4c435563_u32, 0xa22e802f_u32, 0x656ddbaf_u32, 0xb9cf9876_u32, 0xda4463ca_u32, 0x6c634b45_u32, 0xaa1163f6_u32, 0x4d2209a4_u32,
    };

    Xoshiro128ss rng;
    uint32_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoshiro256p() {

    static constexpr uint64_t expect[] = {
        0xdaac60e1ed6a4f9b_u64, 0x3156a1da0dc08435_u64, 0xf9ba3e3285d046ab_u64, 0x4fd194611dba7b01_u64, 0x40b78599c31791bf_u64, 0x03b1dd310503d6f4_u64, 0xb238d3a721d5092b_u64, 0x11017bba8a0f8adf_u64,
        0xa6a988bed1f59149_u64, 0xdb4000fb8d550622_u64, 0x5b3947becb71ef9d_u64, 0x53cda86134220dba_u64, 0x95aa43de2a55bfe9_u64, 0x2a6a597cc890c649_u64, 0xa159be94778c6782_u64, 0x057cc5712467f9be_u64,
        0xfd5116670c452eca_u64, 0x5965ef754974bedb_u64, 0x7e6232b5e4d2282d_u64, 0x13bf0121edf46ac4_u64, 0xa94e36da9dd60992_u64, 0xa81fff375842e458_u64, 0xb7231663870094e4_u64, 0x5d9d875117ce4fa6_u64,
        0x48866f1f33861ff6_u64, 0x7d81c7249ff3a3ac_u64, 0xa508fa0999faa2f2_u64, 0xbfc74758a9c688ce_u64, 0x202904f83161ec8f_u64, 0x4deb77e04e9cf9c8_u64, 0x7c01f8423f2707b6_u64, 0x7b54ce2dc2cf13d8_u64,
        0x8bec36638cc8f5f9_u64, 0xa574dc7678d27bc0_u64, 0x9befb631355b1aea_u64, 0xd3d99cdeb778a6ab_u64, 0xc3836774a5dbe8c4_u64, 0x6db2562ecf20a562_u64, 0x6da00089eab2d146_u64, 0x6cac37d84a3031b8_u64,
        0x4174278caa6594c2_u64, 0x690ab3d3ddf0f7be_u64, 0x82c8b2113b18231e_u64, 0x0792bc36e381ab78_u64, 0x7554d278039ea3c5_u64, 0x907ebf5863fac86b_u64, 0xce3f163d82fa3e12_u64, 0x76e4df602f109d6f_u64,
        0xd0393d8f83280007_u64, 0xd1e195d20b1f658e_u64, 0x8f36b79f4151a99b_u64, 0x146ae01666c7119e_u64, 0x54ef0e4b14c985c2_u64, 0xf8a1370e5fb05419_u64, 0x3a50cb142c99e296_u64, 0xb4591e7b9a5550ba_u64,
        0x126633b1e0336f84_u64, 0xa656ad6a8af9ed4f_u64, 0xd7b339bc36a7657e_u64, 0xd5561040dd97530c_u64, 0xde6e5d06840240ba_u64, 0x977f5479ea8214e9_u64, 0xa559406954adbf11_u64, 0x30e7b06bd913ad64_u64,
    };

    Xoshiro256p rng;
    uint64_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_xoshiro256ss() {

    static constexpr uint64_t expect[] = {
        0x99ec5f36cb75f2b4_u64, 0xbf6e1f784956452a_u64, 0x1a5f849d4933e6e0_u64, 0x6aa594f1262d2d2c_u64, 0xbba5ad4a1f842e59_u64, 0xffef8375d9ebcaca_u64, 0x6c160deed2f54c98_u64, 0x8920ad648fc30a3f_u64,
        0xdb032c0ba7539731_u64, 0xeb3a475a3e749a3d_u64, 0x1d42993fa43f2a54_u64, 0x11361bf526a14bb5_u64, 0x1b4f07a5ab3d8e9c_u64, 0xa7a3257f6986db7f_u64, 0x7efdaa95605dfc9c_u64, 0x4bde97c0a78eaab8_u64,
        0xb455eac43518666c_u64, 0x304dbf6c06730690_u64, 0x8cbe7776598a798c_u64, 0x0ecbdf7ffcd727e5_u64, 0x4ff52157533fe270_u64, 0x7e61475b87242f2e_u64, 0x52558c68a9316824_u64, 0xa0bd00c592471176_u64,
        0xfc9b83a3a0c63b9e_u64, 0x4d786c0f0a8b88ef_u64, 0xa52473c4f62f2338_u64, 0xe9dc0037db25d6d9_u64, 0xfce5eba9d25094c3_u64, 0xe3dbe61ee2d64b51_u64, 0x23f62e432b1272df_u64, 0x4ac7443a342c4913_u64,
        0xc31cf1a9658c1991_u64, 0x290c97ffce918b1d_u64, 0xf54455e02e90636a_u64, 0xf57745758bb8f33f_u64, 0xe5e1b685122823d9_u64, 0x2c16cde0fd8097ec_u64, 0x3cdebc44a5bc1936_u64, 0x6833bafa723c2dbd_u64,
        0xb6fa6c4ba1d3d39e_u64, 0xe5b932b656c2edc3_u64, 0x09cf0b6121615c9f_u64, 0x214e25d57fc636d5_u64, 0xcf3d1721806e2537_u64, 0xcf796fc6335ddc02_u64, 0x353c8b86489b0322_u64, 0xfc4865822547b6aa_u64,
        0xe8c93d84ee8b3f8c_u64, 0xd1b42120a323f2d6_u64, 0xa73a11d247ff36b2_u64, 0xae42236958bba58c_u64, 0xb622679e2affcf3a_u64, 0xcc3bab0060f645f4_u64, 0x2e01e45c78f0daa7_u64, 0x08566c5f16be948a_u64,
        0x73beac2187e1f640_u64, 0x8e903d752c1b5d6e_u64, 0x5b34681094d7511d_u64, 0x70ebad382047f5c1_u64, 0xeae5ca1448d4e9cc_u64, 0x3d2d62775b631bd5_u64, 0x8cb72ebc5b4f7dc3_u64, 0x099c2939ea690a80_u64,
    };

    Xoshiro256ss rng;
    uint64_t x = 0;

    for (auto y: expect) {
        TRY(x = rng());
        TEST_EQUAL(x, y);
    }

}

void test_core_random_generic_rng() {

    static constexpr size_t iterations = 1'000'000;

    GenRng32 gen32;
    GenRng64 gen64;
    std::ranlux24 lux24(42);
    std::ranlux48 lux48(42);
    auto u32 = ~ uint32_t(0);
    auto u64 = ~ uint64_t(0);

    TRY(u32 = gen32());  TEST_EQUAL(u32, 0u);
    TRY(u64 = gen64());  TEST_EQUAL(u64, 0u);

    TRY(gen32 = lux24);  CHECK_RANDOM_GENERATOR(gen32, 0, max32, mean32, sd32);
    TRY(gen32 = lux48);  CHECK_RANDOM_GENERATOR(gen32, 0, max32, mean32, sd32);
    TRY(gen64 = lux24);  CHECK_RANDOM_GENERATOR(gen64, 0, max64, mean64, sd64);
    TRY(gen64 = lux48);  CHECK_RANDOM_GENERATOR(gen64, 0, max64, mean64, sd64);

}

void test_core_random_device_rng() {

    static constexpr size_t iterations = 10'000;

    Urandom32 dev32;
    Urandom64 dev64;

    CHECK_RANDOM_GENERATOR(dev32, 0, max32, mean32, sd32);
    CHECK_RANDOM_GENERATOR(dev64, 0, max64, mean64, sd64);

}

void test_core_random_basic_distributions() {

    static constexpr size_t iterations = 100'000;
    static constexpr double xmax64 = 18'446'744'073'709'551'615.0;
    static constexpr double xmean64 = 9'223'372'036'854'775'807.5;
    static constexpr double xsd64 = 5'325'116'328'314'171'700.52;

    std::mt19937 rng(42);

    { auto gen = [&] { return random_integer(100)(rng); };                        CHECK_RANDOM_GENERATOR(gen, 0, 99, 49.5, 28.8661); }
    { auto gen = [&] { return random_integer(101, 200)(rng); };                   CHECK_RANDOM_GENERATOR(gen, 101, 200, 150.5, 28.8661); }
    { auto gen = [&] { return random_integer(int64_t(0), int64_t(1e18))(rng); };  CHECK_RANDOM_GENERATOR(gen, 0, 1e18, 5e17, 2.88661e17); }
    { auto gen = [&] { return random_integer(uint64_t(0), uint64_t(-1))(rng); };  CHECK_RANDOM_GENERATOR(gen, 0, xmax64, xmean64, xsd64); }
    { auto gen = [&] { return random_integer(42, 42)(rng); };                     CHECK_RANDOM_GENERATOR(gen, 42, 42, 42, 0); }
    { auto gen = [&] { return random_boolean()(rng); };                           CHECK_RANDOM_GENERATOR(gen, 0, 1, 0.5, 0.5); }
    { auto gen = [&] { return random_boolean(0.25)(rng); };                       CHECK_RANDOM_GENERATOR(gen, 0, 1, 0.25, 0.433013); }
    { auto gen = [&] { return random_boolean(Ratio(3, 4))(rng); };                  CHECK_RANDOM_GENERATOR(gen, 0, 1, 0.75, 0.433013); }
    { auto gen = [&] { return random_boolean(3, 4)(rng); };                       CHECK_RANDOM_GENERATOR(gen, 0, 1, 0.75, 0.433013); }
    { auto gen = [&] { return random_binomial(Ratio(3, 4), 10)(rng); };             CHECK_RANDOM_GENERATOR(gen, 0, 10, 7.5, 1.369306); }
    { auto gen = [&] { return random_binomial(Ratio(1, 4), 20)(rng); };             CHECK_RANDOM_GENERATOR(gen, 0, 20, 5, 1.936492); }
    { auto gen = [&] { return random_dice<int>()(rng); };                         CHECK_RANDOM_GENERATOR(gen, 1, 6, 3.5, 1.70783); }
    { auto gen = [&] { return random_dice(3)(rng); };                             CHECK_RANDOM_GENERATOR(gen, 3, 18, 10.5, 2.95804); }
    { auto gen = [&] { return random_dice(3, 10)(rng); };                         CHECK_RANDOM_GENERATOR(gen, 3, 30, 16.5, 4.97494); }
    { auto gen = [&] { return random_real<double>()(rng); };                      CHECK_RANDOM_GENERATOR(gen, 0, 1, 0.5, 0.288661); }
    { auto gen = [&] { return random_real(-100.0, 100.0)(rng); };                 CHECK_RANDOM_GENERATOR(gen, -100, 100, 0, 57.7350); }
    { auto gen = [&] { return random_real(42.0, 42.0)(rng); };                    CHECK_RANDOM_GENERATOR(gen, 42, 42, 42, 0); }
    { auto gen = [&] { return random_normal<double>()(rng); };                    CHECK_RANDOM_GENERATOR(gen, - inf, inf, 0, 1); }
    { auto gen = [&] { return random_normal(10.0, 5.0)(rng); };                   CHECK_RANDOM_GENERATOR(gen, - inf, inf, 10, 5); }

}

void test_core_random_discrete_normal_distribution() {

    static constexpr int iterations = 100'000;
    static constexpr int mean = 100;
    static constexpr int sd = 10;

    Statistics<double> stats;
    Xoshiro rng(42);
    int n = 0;
    uint64_t u = 0;

    for (int i = 0; i < iterations; ++i) {
        TRY(n = random_discrete_normal<int>(mean, sd)(rng));
        stats(double(n));
    }

    TEST_NEAR_EPSILON(stats.mean(), double(mean), 0.05);
    TEST_NEAR_EPSILON(stats.sd_bc(), double(sd), 0.05);

    stats.clear();

    for (int i = 0; i < iterations; ++i) {
        TRY(u = random_discrete_normal<uint64_t>(mean, sd)(rng));
        stats(double(u));
    }

    TEST_NEAR_EPSILON(stats.mean(), double(mean), 0.05);
    TEST_NEAR_EPSILON(stats.sd_bc(), double(sd), 0.05);

}

void test_core_random_triangular_distribution() {

    static constexpr int iterations = 100'000;
    const double epsilon = 1 / std::sqrt(double(iterations));

    std::mt19937 rng(42);
    std::map<int, double> census;
    auto gen = random_triangle_integer(5, 0);

    for (int i = 0; i < iterations; ++i) {
        int x = gen(rng);
        ++census[x];
    }

    for (auto& c: census)
        c.second /= iterations;

    TEST_NEAR_EPSILON(census[0], 0.047619, epsilon);
    TEST_NEAR_EPSILON(census[1], 0.095238, epsilon);
    TEST_NEAR_EPSILON(census[2], 0.142857, epsilon);
    TEST_NEAR_EPSILON(census[3], 0.190476, epsilon);
    TEST_NEAR_EPSILON(census[4], 0.238095, epsilon);
    TEST_NEAR_EPSILON(census[5], 0.285714, epsilon);

    census.clear();
    gen = random_triangle_integer(5, 15);

    for (int i = 0; i < iterations; ++i) {
        int x = gen(rng);
        ++census[x];
    }

    for (auto& c: census)
        c.second /= iterations;

    TEST_NEAR_EPSILON(census[5], 0.166667, epsilon);
    TEST_NEAR_EPSILON(census[6], 0.151515, epsilon);
    TEST_NEAR_EPSILON(census[7], 0.136364, epsilon);
    TEST_NEAR_EPSILON(census[8], 0.121212, epsilon);
    TEST_NEAR_EPSILON(census[9], 0.106061, epsilon);
    TEST_NEAR_EPSILON(census[10], 0.090909, epsilon);
    TEST_NEAR_EPSILON(census[11], 0.075758, epsilon);
    TEST_NEAR_EPSILON(census[12], 0.060606, epsilon);
    TEST_NEAR_EPSILON(census[13], 0.045455, epsilon);
    TEST_NEAR_EPSILON(census[14], 0.030303, epsilon);
    TEST_NEAR_EPSILON(census[15], 0.015152, epsilon);

}

void test_core_random_poisson_distribution() {

    static constexpr int iterations = 1'000'000;

    Poisson<int> poi;
    Statistics<double> stats;
    std::mt19937 rng(42);
    int x = 0;

    TRY(poi = Poisson<int>(4));
    TEST_EQUAL(poi.lambda(), 4);
    TEST_EQUAL(poi.mean(), 4);
    TEST_EQUAL(poi.variance(), 4);
    TEST_EQUAL(poi.sd(), 2);

    x = 0;   TEST_NEAR(poi.pdf(x), 0.018316);  TEST_NEAR(poi.cdf(x), 0.018316);  TEST_EQUAL(poi.ccdf(x), 1);
    x = 1;   TEST_NEAR(poi.pdf(x), 0.073263);  TEST_NEAR(poi.cdf(x), 0.091578);  TEST_NEAR(poi.ccdf(x), 0.981684);
    x = 2;   TEST_NEAR(poi.pdf(x), 0.146525);  TEST_NEAR(poi.cdf(x), 0.238103);  TEST_NEAR(poi.ccdf(x), 0.908422);
    x = 3;   TEST_NEAR(poi.pdf(x), 0.195367);  TEST_NEAR(poi.cdf(x), 0.433470);  TEST_NEAR(poi.ccdf(x), 0.761897);
    x = 4;   TEST_NEAR(poi.pdf(x), 0.195367);  TEST_NEAR(poi.cdf(x), 0.628837);  TEST_NEAR(poi.ccdf(x), 0.566530);
    x = 5;   TEST_NEAR(poi.pdf(x), 0.156293);  TEST_NEAR(poi.cdf(x), 0.785130);  TEST_NEAR(poi.ccdf(x), 0.371163);
    x = 6;   TEST_NEAR(poi.pdf(x), 0.104196);  TEST_NEAR(poi.cdf(x), 0.889326);  TEST_NEAR(poi.ccdf(x), 0.214870);
    x = 7;   TEST_NEAR(poi.pdf(x), 0.059540);  TEST_NEAR(poi.cdf(x), 0.948866);  TEST_NEAR(poi.ccdf(x), 0.110674);
    x = 8;   TEST_NEAR(poi.pdf(x), 0.029770);  TEST_NEAR(poi.cdf(x), 0.978637);  TEST_NEAR(poi.ccdf(x), 0.051134);
    x = 9;   TEST_NEAR(poi.pdf(x), 0.013231);  TEST_NEAR(poi.cdf(x), 0.991868);  TEST_NEAR(poi.ccdf(x), 0.021363);
    x = 10;  TEST_NEAR(poi.pdf(x), 0.005292);  TEST_NEAR(poi.cdf(x), 0.997160);  TEST_NEAR(poi.ccdf(x), 0.008132);
    x = 11;  TEST_NEAR(poi.pdf(x), 0.001925);  TEST_NEAR(poi.cdf(x), 0.999085);  TEST_NEAR(poi.ccdf(x), 0.002840);
    x = 12;  TEST_NEAR(poi.pdf(x), 0.000642);  TEST_NEAR(poi.cdf(x), 0.999726);  TEST_NEAR(poi.ccdf(x), 0.000915);
    x = 13;  TEST_NEAR(poi.pdf(x), 0.000197);  TEST_NEAR(poi.cdf(x), 0.999924);  TEST_NEAR(poi.ccdf(x), 0.000274);
    x = 14;  TEST_NEAR(poi.pdf(x), 0.000056);  TEST_NEAR(poi.cdf(x), 0.999980);  TEST_NEAR(poi.ccdf(x), 0.000076);
    x = 15;  TEST_NEAR(poi.pdf(x), 0.000015);  TEST_NEAR(poi.cdf(x), 0.999995);  TEST_NEAR(poi.ccdf(x), 0.000020);

    for (int i = 0; i < iterations; ++i) {
        TRY(x = poi(rng));
        stats(double(x));
    }

    TEST_NEAR_EPSILON(stats.mean(), poi.mean(), 0.001);
    TEST_NEAR_EPSILON(stats.sd_bc(), poi.sd(), 0.001);

    TRY(poi = Poisson<int>(100));
    TEST_EQUAL(poi.lambda(), 100);
    TEST_EQUAL(poi.mean(), 100);
    TEST_EQUAL(poi.variance(), 100);
    TEST_EQUAL(poi.sd(), 10);

    x = 0;    TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_EQUAL(poi.ccdf(x), 1);
    x = 5;    TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 10;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 15;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 20;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 25;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 30;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 35;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 40;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 45;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 50;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000000);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 55;   TEST_NEAR(poi.pdf(x), 0.000000);  TEST_NEAR(poi.cdf(x), 0.000001);  TEST_NEAR(poi.ccdf(x), 1.000000);
    x = 60;   TEST_NEAR(poi.pdf(x), 0.000004);  TEST_NEAR(poi.cdf(x), 0.000011);  TEST_NEAR(poi.ccdf(x), 0.999994);
    x = 65;   TEST_NEAR(poi.pdf(x), 0.000045);  TEST_NEAR(poi.cdf(x), 0.000123);  TEST_NEAR(poi.ccdf(x), 0.999922);
    x = 70;   TEST_NEAR(poi.pdf(x), 0.000311);  TEST_NEAR(poi.cdf(x), 0.000971);  TEST_NEAR(poi.ccdf(x), 0.999339);
    x = 75;   TEST_NEAR(poi.pdf(x), 0.001499);  TEST_NEAR(poi.cdf(x), 0.005473);  TEST_NEAR(poi.ccdf(x), 0.996027);
    x = 80;   TEST_NEAR(poi.pdf(x), 0.005198);  TEST_NEAR(poi.cdf(x), 0.022649);  TEST_NEAR(poi.ccdf(x), 0.982549);
    x = 85;   TEST_NEAR(poi.pdf(x), 0.013205);  TEST_NEAR(poi.cdf(x), 0.070750);  TEST_NEAR(poi.ccdf(x), 0.942455);
    x = 90;   TEST_NEAR(poi.pdf(x), 0.025039);  TEST_NEAR(poi.cdf(x), 0.171385);  TEST_NEAR(poi.ccdf(x), 0.853654);
    x = 95;   TEST_NEAR(poi.pdf(x), 0.036012);  TEST_NEAR(poi.cdf(x), 0.331192);  TEST_NEAR(poi.ccdf(x), 0.704821);
    x = 100;  TEST_NEAR(poi.pdf(x), 0.039861);  TEST_NEAR(poi.cdf(x), 0.526562);  TEST_NEAR(poi.ccdf(x), 0.513299);
    x = 105;  TEST_NEAR(poi.pdf(x), 0.034401);  TEST_NEAR(poi.cdf(x), 0.712808);  TEST_NEAR(poi.ccdf(x), 0.321593);
    x = 110;  TEST_NEAR(poi.pdf(x), 0.023423);  TEST_NEAR(poi.cdf(x), 0.852863);  TEST_NEAR(poi.ccdf(x), 0.170560);
    x = 115;  TEST_NEAR(poi.pdf(x), 0.012718);  TEST_NEAR(poi.cdf(x), 0.936821);  TEST_NEAR(poi.ccdf(x), 0.075897);
    x = 120;  TEST_NEAR(poi.pdf(x), 0.005561);  TEST_NEAR(poi.cdf(x), 0.977331);  TEST_NEAR(poi.ccdf(x), 0.028230);
    x = 125;  TEST_NEAR(poi.pdf(x), 0.001976);  TEST_NEAR(poi.cdf(x), 0.993202);  TEST_NEAR(poi.ccdf(x), 0.008774);
    x = 130;  TEST_NEAR(poi.pdf(x), 0.000575);  TEST_NEAR(poi.cdf(x), 0.998293);  TEST_NEAR(poi.ccdf(x), 0.002282);
    x = 135;  TEST_NEAR(poi.pdf(x), 0.000138);  TEST_NEAR(poi.cdf(x), 0.999640);  TEST_NEAR(poi.ccdf(x), 0.000498);
    x = 140;  TEST_NEAR(poi.pdf(x), 0.000028);  TEST_NEAR(poi.cdf(x), 0.999936);  TEST_NEAR(poi.ccdf(x), 0.000092);
    x = 145;  TEST_NEAR(poi.pdf(x), 0.000005);  TEST_NEAR(poi.cdf(x), 0.999990);  TEST_NEAR(poi.ccdf(x), 0.000014);
    x = 150;  TEST_NEAR(poi.pdf(x), 0.000001);  TEST_NEAR(poi.cdf(x), 0.999999);  TEST_NEAR(poi.ccdf(x), 0.000002);

    stats.clear();

    for (int i = 0; i < iterations; ++i) {
        TRY(x = poi(rng));
        stats(double(x));
    }

    TEST_NEAR_EPSILON(stats.mean(), poi.mean(), 0.1);
    TEST_NEAR_EPSILON(stats.sd_bc(), poi.sd(), 0.1);

}

void test_core_random_beta_distribution() {

    static constexpr int iterations = 100'000;

    Beta<double> beta;
    Statistics<double> stats;
    std::mt19937 rng(42);
    double x = 0, p = 0;

    TRY(beta = Beta<double>(0.5, 0.5));
    TEST_EQUAL(beta.alpha(), 0.5);
    TEST_EQUAL(beta.beta(), 0.5);
    TEST_EQUAL(beta.mean(), 0.5);
    TEST_EQUAL(beta.variance(), 0.125);
    TEST_NEAR(beta.sd(), 0.353553);

    x = 0.0;  /**/                   /**/                     TRY(p = beta.cdf(x));  TEST_EQUAL(p, 0);        TRY(p = beta.ccdf(x));  TEST_EQUAL(p, 1);
    x = 0.1;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 1.061033);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.204833);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.795167);
    x = 0.2;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.795775);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.295167);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.704833);
    x = 0.3;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.694609);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.369010);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.630990);
    x = 0.4;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.649747);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.435906);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.564094);
    x = 0.5;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.636620);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.500000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.500000);
    x = 0.6;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.649747);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.564094);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.435906);
    x = 0.7;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.694609);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.630990);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.369010);
    x = 0.8;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.795775);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.704833);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.295167);
    x = 0.9;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 1.061033);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.795167);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.204833);
    x = 1.0;  /**/                   /**/                     TRY(p = beta.cdf(x));  TEST_EQUAL(p, 1);        TRY(p = beta.ccdf(x));  TEST_EQUAL(p, 0);

    p = 0.0;  TRY(x = beta.quantile(p));  TEST_EQUAL(x, 0);        TRY(x = beta.cquantile(p));  TEST_EQUAL(x, 1);
    p = 0.1;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.024472);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.975528);
    p = 0.2;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.095492);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.904508);
    p = 0.3;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.206107);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.793893);
    p = 0.4;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.345492);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.654508);
    p = 0.5;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.500000);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.500000);
    p = 0.6;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.654508);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.345492);
    p = 0.7;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.793893);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.206107);
    p = 0.8;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.904508);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.095492);
    p = 0.9;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.975528);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.024472);
    p = 1.0;  TRY(x = beta.quantile(p));  TEST_EQUAL(x, 1);        TRY(x = beta.cquantile(p));  TEST_EQUAL(x, 0);

    stats.clear();
    for (int i = 0; i < iterations; ++i) {
        TRY(x = beta(rng));
        stats(x);
    }
    TEST_NEAR_EPSILON(stats.mean(), beta.mean(), 0.005);
    TEST_NEAR_EPSILON(stats.sd_bc(), beta.sd(), 0.005);

    TRY(beta = Beta<double>(1, 3));
    TEST_EQUAL(beta.alpha(), 1);
    TEST_EQUAL(beta.beta(), 3);
    TEST_EQUAL(beta.mean(), 0.25);
    TEST_EQUAL(beta.variance(), 0.0375);
    TEST_NEAR(beta.sd(), 0.193649);

    x = 0.0;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 3.000000);  TRY(p = beta.cdf(x));  TEST_EQUAL(p, 0);        TRY(p = beta.ccdf(x));  TEST_EQUAL(p, 1);
    x = 0.1;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 2.430000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.271000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.729000);
    x = 0.2;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 1.920000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.488000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.512000);
    x = 0.3;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 1.470000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.657000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.343000);
    x = 0.4;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 1.080000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.784000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.216000);
    x = 0.5;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.750000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.875000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.125000);
    x = 0.6;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.480000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.936000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.064000);
    x = 0.7;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.270000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.973000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.027000);
    x = 0.8;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.120000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.992000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.008000);
    x = 0.9;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.030000);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.999000);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.001000);
    x = 1.0;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.000000);  TRY(p = beta.cdf(x));  TEST_EQUAL(p, 1);        TRY(p = beta.ccdf(x));  TEST_EQUAL(p, 0);

    p = 0.0;  TRY(x = beta.quantile(p));  TEST_EQUAL(x, 0);        TRY(x = beta.cquantile(p));  TEST_EQUAL(x, 1);
    p = 0.1;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.034511);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.535841);
    p = 0.2;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.071682);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.415196);
    p = 0.3;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.112096);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.330567);
    p = 0.4;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.156567);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.263194);
    p = 0.5;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.206299);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.206299);
    p = 0.6;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.263194);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.156567);
    p = 0.7;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.330567);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.112096);
    p = 0.8;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.415196);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.071682);
    p = 0.9;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.535841);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.034511);
    p = 1.0;  TRY(x = beta.quantile(p));  TEST_EQUAL(x, 1);        TRY(x = beta.cquantile(p));  TEST_EQUAL(x, 0);

    stats.clear();
    for (int i = 0; i < iterations; ++i) {
        TRY(x = beta(rng));
        stats(x);
    }
    TEST_NEAR_EPSILON(stats.mean(), beta.mean(), 0.005);
    TEST_NEAR_EPSILON(stats.sd_bc(), beta.sd(), 0.005);

    TRY(beta = Beta<double>(2, 5));
    TEST_EQUAL(beta.alpha(), 2);
    TEST_EQUAL(beta.beta(), 5);
    TEST_NEAR(beta.mean(), 0.285714);
    TEST_NEAR(beta.variance(), 0.025510);
    TEST_NEAR(beta.sd(), 0.159719);

    x = 0.0;  TRY(p = beta.pdf(x));  TEST_EQUAL(p, 0);        TRY(p = beta.cdf(x));  TEST_EQUAL(p, 0);        TRY(p = beta.ccdf(x));  TEST_EQUAL(p, 1);
    x = 0.1;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 1.968300);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.114265);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.885735);
    x = 0.2;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 2.457600);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.344640);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.655360);
    x = 0.3;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 2.160900);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.579825);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.420175);
    x = 0.4;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 1.555200);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.766720);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.233280);
    x = 0.5;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.937500);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.890625);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.109375);
    x = 0.6;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.460800);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.959040);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.040960);
    x = 0.7;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.170100);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.989065);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.010935);
    x = 0.8;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.038400);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.998400);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.001600);
    x = 0.9;  TRY(p = beta.pdf(x));  TEST_NEAR(p, 0.002700);  TRY(p = beta.cdf(x));  TEST_NEAR(p, 0.999945);  TRY(p = beta.ccdf(x));  TEST_NEAR(p, 0.000055);
    x = 1.0;  TRY(p = beta.pdf(x));  TEST_EQUAL(p, 0);        TRY(p = beta.cdf(x));  TEST_EQUAL(p, 1);        TRY(p = beta.ccdf(x));  TEST_EQUAL(p, 0);

    p = 0.0;  TRY(x = beta.quantile(p));  TEST_EQUAL(x, 0);        TRY(x = beta.cquantile(p));  TEST_EQUAL(x, 1);
    p = 0.1;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.092595);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.510316);
    p = 0.2;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.139881);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.422448);
    p = 0.3;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.181803);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.360358);
    p = 0.4;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.222584);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.309444);
    p = 0.5;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.264450);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.264450);
    p = 0.6;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.309444);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.222584);
    p = 0.7;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.360358);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.181803);
    p = 0.8;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.422448);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.139881);
    p = 0.9;  TRY(x = beta.quantile(p));  TEST_NEAR(x, 0.510316);  TRY(x = beta.cquantile(p));  TEST_NEAR(x, 0.092595);
    p = 1.0;  TRY(x = beta.quantile(p));  TEST_EQUAL(x, 1);        TRY(x = beta.cquantile(p));  TEST_EQUAL(x, 0);

    stats.clear();
    for (int i = 0; i < iterations; ++i) {
        TRY(x = beta(rng));
        stats(x);
    }
    TEST_NEAR_EPSILON(stats.mean(), beta.mean(), 0.005);
    TEST_NEAR_EPSILON(stats.sd_bc(), beta.sd(), 0.005);

}

void test_core_random_sample() {

    static constexpr size_t pop_size = 100;
    static constexpr size_t sample_iterations = 100;
    const double expect_mean = double(pop_size + 1) / 2;
    const double expect_sd = std::sqrt(double(pop_size * pop_size - 1) / 12);

    std::mt19937 rng(42);
    std::vector<int> pop(pop_size), sample;
    std::iota(pop.begin(), pop.end(), 1);

    for (size_t k = 0; k <= pop_size; ++k) {
        double count = 0, sum = 0, sum2 = 0;
        for (size_t i = 0; i < sample_iterations; ++i) {
            TRY(sample = random_sample_from(pop, k, rng));
            TEST_EQUAL(sample.size(), k);
            TRY(con_sort_unique(sample));
            TEST_EQUAL(sample.size(), k);
            count += sample.size();
            for (auto x: sample) {
                sum += x;
                sum2 += x * x;
            }
        }
        if (k >= 1) {
            double mean = sum / count;
            TEST_NEAR_EPSILON(mean, expect_mean, 4);
            if (k >= 2) {
                double sd = std::sqrt((sum2 - count * mean * mean) / (count - 1));
                TEST_NEAR_EPSILON(sd, expect_sd, 2);
            }
        }
    }

    TEST_THROW(random_sample_from(pop, 101, rng), std::length_error);

}

void test_core_random_uniform_integer_distribution_properties() {

    auto ri = random_integer(1, 8);

    TEST_EQUAL(ri.min(), 1);
    TEST_EQUAL(ri.max(), 8);
    TEST_EQUAL(ri.mean(), Ratio(9,2));
    TEST_EQUAL(ri.variance(), Ratio(21,4));
    TEST_NEAR(ri.sd(), 2.291288);

    TEST_EQUAL(ri.pdf(0), 0);
    TEST_EQUAL(ri.pdf(1), 0.125);
    TEST_EQUAL(ri.pdf(2), 0.125);
    TEST_EQUAL(ri.pdf(3), 0.125);
    TEST_EQUAL(ri.pdf(4), 0.125);
    TEST_EQUAL(ri.pdf(5), 0.125);
    TEST_EQUAL(ri.pdf(6), 0.125);
    TEST_EQUAL(ri.pdf(7), 0.125);
    TEST_EQUAL(ri.pdf(8), 0.125);
    TEST_EQUAL(ri.pdf(9), 0);

    TEST_EQUAL(ri.cdf(0), 0);
    TEST_EQUAL(ri.cdf(1), 0.125);
    TEST_EQUAL(ri.cdf(2), 0.25);
    TEST_EQUAL(ri.cdf(3), 0.375);
    TEST_EQUAL(ri.cdf(4), 0.5);
    TEST_EQUAL(ri.cdf(5), 0.625);
    TEST_EQUAL(ri.cdf(6), 0.75);
    TEST_EQUAL(ri.cdf(7), 0.875);
    TEST_EQUAL(ri.cdf(8), 1);
    TEST_EQUAL(ri.cdf(9), 1);

    TEST_EQUAL(ri.ccdf(0), 1);
    TEST_EQUAL(ri.ccdf(1), 1);
    TEST_EQUAL(ri.ccdf(2), 0.875);
    TEST_EQUAL(ri.ccdf(3), 0.75);
    TEST_EQUAL(ri.ccdf(4), 0.625);
    TEST_EQUAL(ri.ccdf(5), 0.5);
    TEST_EQUAL(ri.ccdf(6), 0.375);
    TEST_EQUAL(ri.ccdf(7), 0.25);
    TEST_EQUAL(ri.ccdf(8), 0.125);
    TEST_EQUAL(ri.ccdf(9), 0);

}

void test_core_random_binomial_distribution_properties() {

    auto bin = random_binomial(Ratio(1,5), 4);

    TEST_EQUAL(bin.mean(), Ratio(4,5));
    TEST_EQUAL(bin.variance(), Ratio(16,25));
    TEST_NEAR(bin.sd(), 0.8);
    TEST_EQUAL(bin.pdf(-1), 0);
    TEST_NEAR(bin.pdf(0), 0.4096);
    TEST_NEAR(bin.pdf(1), 0.4096);
    TEST_NEAR(bin.pdf(2), 0.1536);
    TEST_NEAR(bin.pdf(3), 0.0256);
    TEST_NEAR(bin.pdf(4), 0.0016);
    TEST_EQUAL(bin.pdf(5), 0);
    TEST_EQUAL(bin.cdf(-1), 0);
    TEST_NEAR(bin.cdf(0), 0.4096);
    TEST_NEAR(bin.cdf(1), 0.8192);
    TEST_NEAR(bin.cdf(2), 0.9728);
    TEST_NEAR(bin.cdf(3), 0.9984);
    TEST_EQUAL(bin.cdf(4), 1);
    TEST_EQUAL(bin.cdf(5), 1);
    TEST_EQUAL(bin.ccdf(-1), 1);
    TEST_EQUAL(bin.ccdf(0), 1);
    TEST_NEAR(bin.ccdf(1), 0.5904);
    TEST_NEAR(bin.ccdf(2), 0.1808);
    TEST_NEAR(bin.ccdf(3), 0.0272);
    TEST_NEAR(bin.ccdf(4), 0.0016);
    TEST_EQUAL(bin.ccdf(5), 0);

}

void test_core_random_dice_distribution_properties() {

    Dice<int> dice;

    TEST_EQUAL(dice.mean(), Ratio(7,2));
    TEST_EQUAL(dice.variance(), Ratio(35,12));
    TEST_NEAR(dice.sd(), 1.707825);
    TEST_EQUAL(dice.pdf(0), 0);
    TEST_NEAR(dice.pdf(1), 0.166667);
    TEST_NEAR(dice.pdf(2), 0.166667);
    TEST_NEAR(dice.pdf(3), 0.166667);
    TEST_NEAR(dice.pdf(4), 0.166667);
    TEST_NEAR(dice.pdf(5), 0.166667);
    TEST_NEAR(dice.pdf(6), 0.166667);
    TEST_EQUAL(dice.pdf(7), 0);
    TEST_EQUAL(dice.cdf(0), 0);
    TEST_NEAR(dice.cdf(1), 0.166667);
    TEST_NEAR(dice.cdf(2), 0.333333);
    TEST_NEAR(dice.cdf(3), 0.5);
    TEST_NEAR(dice.cdf(4), 0.666667);
    TEST_NEAR(dice.cdf(5), 0.833333);
    TEST_EQUAL(dice.cdf(6), 1);
    TEST_EQUAL(dice.cdf(7), 1);
    TEST_EQUAL(dice.ccdf(0), 1);
    TEST_EQUAL(dice.ccdf(1), 1);
    TEST_NEAR(dice.ccdf(2), 0.833333);
    TEST_NEAR(dice.ccdf(3), 0.666667);
    TEST_NEAR(dice.ccdf(4), 0.5);
    TEST_NEAR(dice.ccdf(5), 0.333333);
    TEST_NEAR(dice.ccdf(6), 0.166667);
    TEST_EQUAL(dice.ccdf(7), 0);

    dice = random_dice(2, 6);

    TEST_EQUAL(dice.mean(), 7);
    TEST_EQUAL(dice.variance(), Ratio(35,6));
    TEST_NEAR(dice.sd(), 2.415229);
    TEST_EQUAL(dice.pdf(1), 0);
    TEST_NEAR(dice.pdf(2), 0.027778);
    TEST_NEAR(dice.pdf(3), 0.055556);
    TEST_NEAR(dice.pdf(4), 0.083333);
    TEST_NEAR(dice.pdf(5), 0.111111);
    TEST_NEAR(dice.pdf(6), 0.138889);
    TEST_NEAR(dice.pdf(7), 0.166667);
    TEST_NEAR(dice.pdf(8), 0.138889);
    TEST_NEAR(dice.pdf(9), 0.111111);
    TEST_NEAR(dice.pdf(10), 0.083333);
    TEST_NEAR(dice.pdf(11), 0.055556);
    TEST_NEAR(dice.pdf(12), 0.027778);
    TEST_EQUAL(dice.pdf(13), 0);
    TEST_EQUAL(dice.cdf(1), 0);
    TEST_NEAR(dice.cdf(2), 0.027778);
    TEST_NEAR(dice.cdf(3), 0.083333);
    TEST_NEAR(dice.cdf(4), 0.166667);
    TEST_NEAR(dice.cdf(5), 0.277778);
    TEST_NEAR(dice.cdf(6), 0.416667);
    TEST_NEAR(dice.cdf(7), 0.583333);
    TEST_NEAR(dice.cdf(8), 0.722222);
    TEST_NEAR(dice.cdf(9), 0.833333);
    TEST_NEAR(dice.cdf(10), 0.916667);
    TEST_NEAR(dice.cdf(11), 0.972222);
    TEST_EQUAL(dice.cdf(12), 1);
    TEST_EQUAL(dice.cdf(13), 1);
    TEST_EQUAL(dice.ccdf(1), 1);
    TEST_EQUAL(dice.ccdf(2), 1);
    TEST_NEAR(dice.ccdf(3), 0.972222);
    TEST_NEAR(dice.ccdf(4), 0.916667);
    TEST_NEAR(dice.ccdf(5), 0.833333);
    TEST_NEAR(dice.ccdf(6), 0.722222);
    TEST_NEAR(dice.ccdf(7), 0.583333);
    TEST_NEAR(dice.ccdf(8), 0.416667);
    TEST_NEAR(dice.ccdf(9), 0.277778);
    TEST_NEAR(dice.ccdf(10), 0.166667);
    TEST_NEAR(dice.ccdf(11), 0.083333);
    TEST_NEAR(dice.ccdf(12), 0.027778);
    TEST_EQUAL(dice.ccdf(13), 0);

}

void test_core_random_uniform_real_distribution_properties() {

    auto ur = random_real(10.0, 20.0);

    TEST_EQUAL(ur.min(), 10);
    TEST_EQUAL(ur.max(), 20);
    TEST_EQUAL(ur.mean(), 15);
    TEST_NEAR(ur.variance(), 8.333333);
    TEST_NEAR(ur.sd(), 2.886751);

    TEST_EQUAL(ur.pdf(9), 0);
    TEST_EQUAL(ur.pdf(11), 0.1);
    TEST_EQUAL(ur.pdf(13), 0.1);
    TEST_EQUAL(ur.pdf(15), 0.1);
    TEST_EQUAL(ur.pdf(17), 0.1);
    TEST_EQUAL(ur.pdf(19), 0.1);
    TEST_EQUAL(ur.pdf(21), 0);

    TEST_EQUAL(ur.cdf(8), 0);
    TEST_EQUAL(ur.cdf(10), 0);
    TEST_EQUAL(ur.cdf(12), 0.2);
    TEST_EQUAL(ur.cdf(14), 0.4);
    TEST_EQUAL(ur.cdf(16), 0.6);
    TEST_EQUAL(ur.cdf(18), 0.8);
    TEST_EQUAL(ur.cdf(20), 1);
    TEST_EQUAL(ur.cdf(22), 1);

    TEST_EQUAL(ur.ccdf(8), 1);
    TEST_EQUAL(ur.ccdf(10), 1);
    TEST_EQUAL(ur.ccdf(12), 0.8);
    TEST_EQUAL(ur.ccdf(14), 0.6);
    TEST_EQUAL(ur.ccdf(16), 0.4);
    TEST_EQUAL(ur.ccdf(18), 0.2);
    TEST_EQUAL(ur.ccdf(20), 0);
    TEST_EQUAL(ur.ccdf(22), 0);

    TEST_EQUAL(ur.quantile(0), 10);
    TEST_EQUAL(ur.quantile(0.2), 12);
    TEST_EQUAL(ur.quantile(0.4), 14);
    TEST_EQUAL(ur.quantile(0.6), 16);
    TEST_EQUAL(ur.quantile(0.8), 18);
    TEST_EQUAL(ur.quantile(1), 20);

    TEST_EQUAL(ur.cquantile(0), 20);
    TEST_EQUAL(ur.cquantile(0.2), 18);
    TEST_EQUAL(ur.cquantile(0.4), 16);
    TEST_EQUAL(ur.cquantile(0.6), 14);
    TEST_EQUAL(ur.cquantile(0.8), 12);
    TEST_EQUAL(ur.cquantile(1), 10);

}

void test_core_random_normal_distribution_properties() {

    struct sample_type { double z, pdf, cdf; };

    static constexpr sample_type sample_list[] = {
        { -5.00,  0.000001486720,  0.000000286652 },
        { -4.75,  0.000005029507,  0.000001017083 },
        { -4.50,  0.000015983741,  0.000003397673 },
        { -4.25,  0.000047718637,  0.000010688526 },
        { -4.00,  0.000133830226,  0.000031671242 },
        { -3.75,  0.000352595682,  0.000088417285 },
        { -3.50,  0.000872682695,  0.000232629079 },
        { -3.25,  0.002029048057,  0.000577025042 },
        { -3.00,  0.004431848412,  0.001349898032 },
        { -2.75,  0.009093562502,  0.002979763235 },
        { -2.50,  0.017528300494,  0.006209665326 },
        { -2.25,  0.031739651836,  0.012224472655 },
        { -2.00,  0.053990966513,  0.022750131948 },
        { -1.75,  0.086277318827,  0.040059156864 },
        { -1.50,  0.129517595666,  0.066807201269 },
        { -1.25,  0.182649085389,  0.105649773667 },
        { -1.00,  0.241970724519,  0.158655253931 },
        { -0.75,  0.301137432155,  0.226627352377 },
        { -0.50,  0.352065326764,  0.308537538726 },
        { -0.25,  0.386668116803,  0.401293674317 },
        { 0.00,   0.398942280401,  0.500000000000 },
        { 0.25,   0.386668116803,  0.598706325683 },
        { 0.50,   0.352065326764,  0.691462461274 },
        { 0.75,   0.301137432155,  0.773372647623 },
        { 1.00,   0.241970724519,  0.841344746069 },
        { 1.25,   0.182649085389,  0.894350226333 },
        { 1.50,   0.129517595666,  0.933192798731 },
        { 1.75,   0.086277318827,  0.959940843136 },
        { 2.00,   0.053990966513,  0.977249868052 },
        { 2.25,   0.031739651836,  0.987775527345 },
        { 2.50,   0.017528300494,  0.993790334674 },
        { 2.75,   0.009093562502,  0.997020236765 },
        { 3.00,   0.004431848412,  0.998650101968 },
        { 3.25,   0.002029048057,  0.999422974958 },
        { 3.50,   0.000872682695,  0.999767370921 },
        { 3.75,   0.000352595682,  0.999911582715 },
        { 4.00,   0.000133830226,  0.999968328758 },
        { 4.25,   0.000047718637,  0.999989311474 },
        { 4.50,   0.000015983741,  0.999996602327 },
        { 4.75,   0.000005029507,  0.999998982917 },
        { 5.00,   0.000001486720,  0.999999713348 },
    };

    Normal<double> norm;

    TEST_EQUAL(norm.mean(), 0);
    TEST_NEAR(norm.variance(), 1);
    TEST_NEAR(norm.sd(), 1);

    double p, q, y, z;

    for (auto& sample: sample_list) {
        TRY(y = norm.pdf(sample.z));          TEST_NEAR(y, sample.pdf);
        TRY(p = norm.cdf(sample.z));          TEST_NEAR(p, sample.cdf);
        TRY(q = norm.ccdf(sample.z));         TEST_NEAR(q, 1 - sample.cdf);
        TRY(z = norm.quantile(sample.cdf));   TEST_NEAR_EPSILON(z, sample.z, 3e-5);
        TRY(z = norm.cquantile(sample.cdf));  TEST_NEAR_EPSILON(z, - sample.z, 3e-5);
    }

}

void test_core_random_uniform_choice_distribution() {

    static constexpr size_t iterations = 100'000;
    static const std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};

    std::mt19937 rng(42);
    Choice<int> choice;

    {
        auto gen = [&] { return choice(rng); };
        for (int i = 1; i <= 10; ++i)
            TRY(choice.add(i));
        CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661);
        TRY(choice.clear());
        TRY(std::copy(v.begin(), v.end(), append(choice)));
        CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661);
        TRY(choice.clear());
        TRY(choice.append(v));
        CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661);
        TRY(choice.clear());
        TRY(choice.append({1,2,3,4,5,6,7,8,9,10}));
        CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661);
    }

    { auto gen = [&] { return random_choice(v)(rng); };                            CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661); }
    { auto gen = [&] { return random_choice(v.begin(), v.end())(rng); };           CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661); }
    { auto gen = [&] { return random_choice({1,2,3,4,5,6,7,8,9,10})(rng); };       CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661); }
    { auto gen = [&] { return random_choice_from(v, rng); };                       CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661); }
    { auto gen = [&] { return random_choice_from(v.begin(), v.end(), rng); };      CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661); }
    { auto gen = [&] { return random_choice_from({1,2,3,4,5,6,7,8,9,10}, rng); };  CHECK_RANDOM_GENERATOR(gen, 1, 10, 5.5, 2.88661); }

}

void test_core_random_weighted_choice_distribution() {

    const int iterations = 1000000;
    std::map<Ustring, int> census;
    std::mt19937 rng(42);
    auto freq = [&] (const Ustring& s) { return double(census[s]) / double(iterations); };

    {
        WeightedChoice<Ustring> choice;
        TRY((choice = {
            {"alpha", 0.4},
            {"bravo", 0.3},
            {"charlie", 0.2},
            {"delta", 0.1},
        }));
        census.clear();
        for (int i = 0; i < iterations; ++i)
            TRY(++census[choice(rng)]);
        TEST_NEAR_EPSILON(freq("alpha"), 0.4, 0.01);
        TEST_NEAR_EPSILON(freq("bravo"), 0.3, 0.01);
        TEST_NEAR_EPSILON(freq("charlie"), 0.2, 0.01);
        TEST_NEAR_EPSILON(freq("delta"), 0.1, 0.01);
    }

    {
        WeightedChoice<Ustring, int> choice;
        TRY((choice = {
            {"alpha", 40},
            {"bravo", 30},
            {"charlie", 20},
            {"delta", 10},
        }));
        census.clear();
        for (int i = 0; i < iterations; ++i)
            TRY(++census[choice(rng)]);
        TEST_NEAR_EPSILON(freq("alpha"), 0.4, 0.01);
        TEST_NEAR_EPSILON(freq("bravo"), 0.3, 0.01);
        TEST_NEAR_EPSILON(freq("charlie"), 0.2, 0.01);
        TEST_NEAR_EPSILON(freq("delta"), 0.1, 0.01);
    }

}

void test_core_random_vectors() {

    const int iterations = 1000000;
    std::mt19937 rng(42);
    Double3 scale(5, 10, 15), v, low, high, sum, sum2, mean, sd;

    low = Double3(100);
    high = Double3(-100);
    sum = Double3(0);
    sum2 = Double3(0);
    RandomVector<double, 3> rv(scale);
    for (int i = 0; i < iterations; ++i) {
        TRY(v = rv(rng));
        sum += v;
        for (int j = 0; j < 3; ++j) {
            low[j] = std::min(low[j], v[j]);
            high[j] = std::max(high[j], v[j]);
            sum2[j] += v[j] * v[j];
        }
    }
    mean = sum / iterations;
    for (int i = 0; i < 3; ++i)
        sd[i] = std::sqrt(sum2[i] / iterations - mean[i] * mean[i]);
    TEST_COMPARE(low[0], >, 0);
    TEST_COMPARE(low[1], >, 0);
    TEST_COMPARE(low[2], >, 0);
    TEST_COMPARE(high[0], <, 5);
    TEST_COMPARE(high[1], <, 10);
    TEST_COMPARE(high[2], <, 15);
    TEST_NEAR_EPSILON(mean[0], 2.5, 0.01);
    TEST_NEAR_EPSILON(mean[1], 5, 0.01);
    TEST_NEAR_EPSILON(mean[2], 7.5, 0.01);
    TEST_NEAR_EPSILON(sd[0], 1.443376, 0.01);
    TEST_NEAR_EPSILON(sd[1], 2.886751, 0.01);
    TEST_NEAR_EPSILON(sd[2], 4.330127, 0.01);

    low = Double3(100);
    high = Double3(-100);
    sum = Double3(0);
    sum2 = Double3(0);
    SymmetricRandomVector<double, 3> srv(scale);
    for (int i = 0; i < iterations; ++i) {
        TRY(v = srv(rng));
        sum += v;
        for (int j = 0; j < 3; ++j) {
            low[j] = std::min(low[j], v[j]);
            high[j] = std::max(high[j], v[j]);
            sum2[j] += v[j] * v[j];
        }
    }
    mean = sum / iterations;
    for (int i = 0; i < 3; ++i)
        sd[i] = std::sqrt(sum2[i] / iterations - mean[i] * mean[i]);
    TEST_COMPARE(low[0], >, -5);
    TEST_COMPARE(low[1], >, -10);
    TEST_COMPARE(low[2], >, -15);
    TEST_COMPARE(high[0], <, 5);
    TEST_COMPARE(high[1], <, 10);
    TEST_COMPARE(high[2], <, 15);
    TEST_NEAR_EPSILON(mean[0], 0, 0.01);
    TEST_NEAR_EPSILON(mean[1], 0, 0.01);
    TEST_NEAR_EPSILON(mean[2], 0, 0.01);
    TEST_NEAR_EPSILON(sd[0], 2.886751, 0.01);
    TEST_NEAR_EPSILON(sd[1], 5.773503, 0.01);
    TEST_NEAR_EPSILON(sd[2], 8.660254, 0.01);

}

namespace {

    template <typename T, size_t N>
    void random_in_sphere_test() {
        static constexpr int iterations = 100'000;
        std::minstd_rand rng(42);
        RandomInSphere<T, N> gen;
        Vector<T, N> count_sides, point, total;
        T count_inner = 0;
        for (int i = 0; i < iterations; ++i) {
            TRY(point = gen(rng));
            for (size_t j = 0; j < N; ++j)
                if (point[j] > 0)
                    ++count_sides[j];
            if (point.r() < T(0.5))
                ++count_inner;
            total += point;
        }
        T epsilon = 2 / std::sqrt(T(iterations));
        T expect_inner = std::pow(T(2), - T(N));
        for (auto& c: count_sides) {
            c /= iterations;
            TEST_NEAR_EPSILON(c, 0.5, epsilon);
        }
        count_inner /= iterations;
        total /= iterations;
        TEST_NEAR_EPSILON(count_inner, expect_inner, epsilon);
        TEST_NEAR_EPSILON(total.r(), 0, epsilon);
    }

    template <typename T, size_t N>
    void random_on_sphere_test() {
        static constexpr int iterations = 100'000;
        std::minstd_rand rng(42);
        RandomOnSphere<T, N> gen;
        Vector<T, N> count_sides, point, total;
        for (int i = 0; i < iterations; ++i) {
            TRY(point = gen(rng));
            TEST_NEAR(point.r(), 1);
            for (size_t j = 0; j < N; ++j)
                if (point[j] > 0)
                    ++count_sides[j];
            total += point;
        }
        T epsilon = 2 / std::sqrt(T(iterations));
        for (auto& c: count_sides) {
            c /= iterations;
            TEST_NEAR_EPSILON(c, 0.5, epsilon);
        }
        total /= iterations;
        TEST_NEAR_EPSILON(total.r(), 0, 2 * epsilon); // more variance in surface points
    }

}

void test_core_random_random_in_sphere() {

    random_in_sphere_test<double, 1>();
    random_in_sphere_test<double, 2>();
    random_in_sphere_test<double, 3>();
    random_in_sphere_test<double, 4>();
    random_in_sphere_test<double, 5>();
    random_in_sphere_test<double, 6>();
    random_in_sphere_test<double, 7>();
    random_in_sphere_test<double, 8>();
    random_in_sphere_test<double, 9>();
    random_in_sphere_test<double, 10>();

}

void test_core_random_random_on_sphere() {

    random_on_sphere_test<double, 1>();
    random_on_sphere_test<double, 2>();
    random_on_sphere_test<double, 3>();
    random_on_sphere_test<double, 4>();
    random_on_sphere_test<double, 5>();
    random_on_sphere_test<double, 6>();
    random_on_sphere_test<double, 7>();
    random_on_sphere_test<double, 8>();
    random_on_sphere_test<double, 9>();
    random_on_sphere_test<double, 10>();

}

void test_core_random_unique_distribution() {

    const int iterations = 10000;
    std::mt19937 rng(42);
    std::uniform_int_distribution<> int_dist(1, 5);
    auto unique_int = unique_distribution(int_dist);
    for (int i = 0; i < iterations; ++i) {
        std::map<int, int> census;
        for (int j = 1; j <= 5; ++j)
            TRY(++census[unique_int(rng)]);
        for (int j = 1; j <= 5; ++j)
            TEST_EQUAL(census[j], 1);
        TRY(unique_int.clear());
    }

}

void test_core_random_chi_squared_distribution_properties() {

    // Chi-squared table from Wikipedia
    using numbers = std::vector<double>;
    struct table_row { int k; numbers chi2; };
    static const numbers p_values = { 0.95, 0.90, 0.80, 0.70, 0.50, 0.30, 0.20, 0.10, 0.05, 0.01, 0.001 };
    static const std::vector<table_row> table = {
        { 1,   { 0.004,  0.02,  0.06,  0.15,  0.46,  1.07,   1.64,   2.71,   3.84,   6.63,   10.83 }},
        { 2,   { 0.10,   0.21,  0.45,  0.71,  1.39,  2.41,   3.22,   4.61,   5.99,   9.21,   13.82 }},
        { 3,   { 0.35,   0.58,  1.01,  1.42,  2.37,  3.66,   4.64,   6.25,   7.81,   11.34,  16.27 }},
        { 4,   { 0.71,   1.06,  1.65,  2.20,  3.36,  4.88,   5.99,   7.78,   9.49,   13.28,  18.47 }},
        { 5,   { 1.14,   1.61,  2.34,  3.00,  4.35,  6.06,   7.29,   9.24,   11.07,  15.09,  20.52 }},
        { 6,   { 1.63,   2.20,  3.07,  3.83,  5.35,  7.23,   8.56,   10.64,  12.59,  16.81,  22.46 }},
        { 7,   { 2.17,   2.83,  3.82,  4.67,  6.35,  8.38,   9.80,   12.02,  14.07,  18.48,  24.32 }},
        { 8,   { 2.73,   3.49,  4.59,  5.53,  7.34,  9.52,   11.03,  13.36,  15.51,  20.09,  26.12 }},
        { 9,   { 3.32,   4.17,  5.38,  6.39,  8.34,  10.66,  12.24,  14.68,  16.92,  21.67,  27.88 }},
        { 10,  { 3.94,   4.87,  6.18,  7.27,  9.34,  11.78,  13.44,  15.99,  18.31,  23.21,  29.59 }},
    };

    ChiSquared<double> chisq;
    double p_est = 0, x_est = 0, eps_p = 0, eps_x = 0;

    for (auto& row: table) {
        switch (row.k) {
            case 1:   eps_p = 0.1;    eps_x = 0.1;    break;  // Abdel-Aty approximation is poor for k=1
            case 2:   eps_p = 0.005;  eps_x = 0.005;  break;  // Special case closed form for k=2
            default:  eps_p = 0.02;   eps_x = 0.05;   break;  // Good approximation for k>2
        }
        TRY(chisq = ChiSquared<double>(row.k));
        for (int i = 0; i < 10; ++i) {
            double p = p_values[i];
            double x = row.chi2[i];
            TRY(p_est = chisq.ccdf(x));
            TEST_NEAR_EPSILON(p_est, p, eps_p * p);
            TRY(x_est = chisq.cquantile(p));
            TEST_NEAR_EPSILON(x_est, x, eps_x);
        }
    }

}

void test_core_random_bytes() {

    static constexpr size_t n = 10000;

    std::mt19937 rng(42);
    uint32_t u32 = 0;
    uint64_t u64 = 0;
    double x = 0, sum = 0, sum2 = 0, mean = 0, sd = 0;

    for (size_t i = 0; i < n; ++i) {
        TRY(random_bytes(rng, &u32, 4));
        x = std::ldexp(u32, -32);
        sum += x;
        sum2 += x * x;
    }
    mean = sum / n;
    sd = std::sqrt(sum2 / n - mean * mean);
    TEST_NEAR_EPSILON(mean, 0.5, 0.05);
    TEST_NEAR_EPSILON(sd, 0.288675, 0.05);

    sum = sum2 = 0;
    for (size_t i = 0; i < n; ++i) {
        TRY(random_bytes(rng, &u64, 8));
        x = std::ldexp(u64, -64);
        sum += x;
        sum2 += x * x;
    }
    mean = sum / n;
    sd = std::sqrt(sum2 / n - mean * mean);
    TEST_NEAR_EPSILON(mean, 0.5, 0.05);
    TEST_NEAR_EPSILON(sd, 0.288675, 0.05);

}

void test_core_random_shuffle() {

    std::mt19937 rng(42);
    Ustring s;

    TRY(shuffle(rng, s));
    TEST_EQUAL(s, "");

    for (int i = 0; i < 1000; ++i) {
        s = "abcdefghij";
        TRY(shuffle(rng, s));
        TEST_COMPARE(s, >, "abcdefghij");
        TRY(std::sort(s.begin(), s.end()));
        TEST_EQUAL(s, "abcdefghij");
    }

}

void test_core_random_seed_sequence() {

    std::random_device src;
    std::mt19937 dst1, dst2;
    uint32_t x1 = 0, x2 = 0, y1 = 0, y2 = 0, z1 = 0, z2 = 0;

    TRY(seed_from(src, dst1));
    TRY(x1 = dst1());
    TRY(y1 = dst1());
    TRY(z1 = dst1());
    TRY(seed_from(src, dst2));
    TRY(x2 = dst2());
    TRY(y2 = dst2());
    TRY(z2 = dst2());
    TEST_COMPARE(x1, !=, x2);
    TEST_COMPARE(y1, !=, y2);
    TEST_COMPARE(z1, !=, z2);

}

void test_core_random_text_generators() {

    #ifdef _MSC_VER
        static constexpr int max_iterations = 20;
    #else
        static constexpr int max_iterations = 70;
    #endif

    Xoshiro rng(42);
    Ustring s;
    TRY(s = lorem_ipsum(rng(), 0));
    TEST(s.empty());

    for (int i = 1; i <= max_iterations; ++i) {
        int bytes = i * i;
        TRY(s = lorem_ipsum(rng(), bytes));
        int size = int(s.size());
        TEST_COMPARE(size, >=, bytes);
        TEST_COMPARE(size, <=, bytes + 15);
        TEST_MATCH(s, "^"
            "[A-Z][a-z]+(,? [a-z]+)*\\."
            "(( |\\n\\n)[A-Z][a-z]+(,? [a-z]+)*\\.)*"
            "\\n$");
    }

    for (int i = 1; i <= max_iterations; ++i) {
        int bytes = i * i;
        TRY(s = lorem_ipsum(rng(), bytes, false));
        int size = int(s.size());
        TEST_COMPARE(size, >=, bytes);
        TEST_COMPARE(size, <=, bytes + 15);
        TEST_MATCH(s, "^"
            "[A-Z][a-z]+(,? [a-z]+)*\\."
            "( [A-Z][a-z]+(,? [a-z]+)*\\.)*"
            "$");
    }

}
