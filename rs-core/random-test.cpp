#include "rs-core/random.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <numeric>
#include <random>
#include <stdexcept>
#include <type_traits>
#include <vector>

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

    #define CHECK_RANDOM_GENERATOR(gen, xmin, xmax, xmean, xsd) \
        do { \
            using T = std::decay_t<decltype(gen())>; \
            double epsilon, lo = inf, hi = - inf, sum = 0, sum2 = 0; \
            if (xmin > - inf && xmax < inf) \
                epsilon = (xmax - xmin) / std::sqrt(double(iterations)); \
            else \
                epsilon = 2 * xsd / std::sqrt(double(iterations)); \
            T t = {}; \
            for (size_t i = 0; i < iterations; ++i) { \
                TRY(t = gen()); \
                auto x = double(t); \
                lo = std::min(lo, x); \
                hi = std::max(hi, x); \
                sum += x; \
                sum2 += x * x; \
            } \
            if (std::is_floating_point<T>::value) { \
                TEST_COMPARE(lo, >, xmin); \
                TEST_COMPARE(hi, <, xmax); \
            } else if (xmax - xmin < 0.1 * iterations) { \
                TEST_EQUAL(lo, xmin); \
                TEST_EQUAL(hi, xmax); \
            } else { \
                TEST_COMPARE(lo, >=, xmin); \
                TEST_COMPARE(hi, <=, xmax); \
            } \
            double m = sum / iterations; \
            double s = std::sqrt(sum2 / iterations - m * m); \
            TEST_NEAR_EPSILON(m, xmean, epsilon); \
            TEST_NEAR_EPSILON(s, xsd, epsilon); \
        } while (false)

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

void test_core_random_xoroshiro() {

    static constexpr uint64_t expect[] = {

        0xd1bcc1a836d606e0ull, 0x31a9ec020430aa62ull, 0x8f8675ab98e71e0aull, 0x7113e550368ee7d6ull, 0x7b2850c603cd8481ull, 0x95f90415825b6823ull, 0xb6ca3ea01ec695e0ull, 0x52f372a0170d28b3ull,
        0xf2e27b444983b5daull, 0x8f1724d4be6a8950ull, 0x88cc05b84d479027ull, 0x43218138d79a0127ull, 0x758afb9a8a357c65ull, 0xee0607eb13a122e4ull, 0x30d47fd0b3b4ed1aull, 0xdf56fa408b6a8082ull,

        0x0adf56c3f919bcb6ull, 0x994e16b24f2e2387ull, 0x9b9dcb6c6e800a7bull, 0x7ed32f4be7e81cecull, 0x42ece17ce0779cc4ull, 0x02ee5dade45d46a2ull, 0x470ca08ffcfb0a85ull, 0x41f168d0ae2a90cdull,
        0xde14300ac09a3a24ull, 0x8c2d99bcbfc6469aull, 0x8c07c516a6b696e2ull, 0x2c00a6e1968e9fecull, 0x8691026432c52499ull, 0x838f8cf41c367a3aull, 0x544078f44660ff41ull, 0x50d82724507881bcull,
        0x734a7d5c8f82f590ull, 0x260b283e50dd1fddull, 0x2cb40476b4c6810aull, 0x737b048b63f55130ull, 0x62548028d94d8857ull, 0x807529e4335f1c47ull, 0x642ca811fafed83bull, 0x09f73dd42caf9ffeull,
        0x296233aa4cff3c12ull, 0xe9ca4adc7d089f9eull, 0x6d8506f6d15b74b2ull, 0x73431bf91cda359bull, 0x7a84e2e1686fbc79ull, 0xb544040388cc6d0dull, 0xe2a275bdf57c89f4ull, 0x7c8040197da2076full,
        0x41ae8ab8bc7acc78ull, 0x9ab356c1835cb32dull, 0xa8d03a92b867dbf7ull, 0x282bb9e2972ed6e1ull, 0x0462702741130241ull, 0x375f8ef460ad9fccull, 0xc704a88305e7d8faull, 0xec46182a4d9cb02aull,
        0x6adf87e5ae36ade9ull, 0xd57f86088ea1dc4cull, 0x2dec1091aab5a4c5ull, 0x9b3606c2fdac2feeull, 0x965aea495e003c80ull, 0x96abc0e9e6b3fac3ull, 0x23ad3380071d5a10ull, 0x547a56ce2892ed58ull,
        0x6af80fe7d7e1a2c3ull, 0x4a276288a57a3d95ull, 0x5451293baf49d436ull, 0x64c0fb06a80d1402ull, 0xfcf13995da9319baull, 0x6f233da81431d137ull, 0xe01d2fc1ea2dcfb1ull, 0x8adbe27bcf0a6dddull,
        0xe258cee72c3db1e2ull, 0x2d56f62f65053154ull, 0x37dca74bdc39f6acull, 0xe9b2eca72620fabcull, 0xc521fbe6c3710bdeull, 0x23142019bbfa927full, 0xefeefad1711ba66dull, 0xc961ea080909cd86ull,
        0xa6822d71a34d3630ull, 0x65bec949f0550b18ull, 0xc992bfd1f6f7c9c9ull, 0x6abdc09788f2b3eaull, 0x2961fe8fbd197729ull, 0xd3eac1fbd8cbb908ull, 0x0e469d054c8464c9ull, 0x4ee5fa51acb06a26ull,
        0x282b2f2718b28477ull, 0xf9ee62490c4cdbd4ull, 0x80f0cd22494f1666ull, 0xf5ae17d7bea724feull, 0xdd6c174b6510b64eull, 0x75a66c4987d44fc0ull, 0xba839f1aeba177c0ull, 0xb7c878e9b409428cull,
        0xc83ddf7be0530af6ull, 0xf1b66ed87feee274ull, 0x0ab003364a531eebull, 0x98524c955b421c55ull, 0xb1cfeef97810bff2ull, 0x5f7ec6a38717f4abull, 0xc82c8a8867a48e28ull, 0xde0707ca51006c2aull,
        0xd62b59baa1ce767aull, 0x62c5e514e524610aull, 0x076aaf5e8cefd517ull, 0x06a3af0d5b65cb7aull, 0x08c3e6d12ae91829ull, 0x9fd67be26e31da25ull, 0x7c9942feb5f421a3ull, 0x92547a099685683eull,
        0x1da620eea7513bcdull, 0x67b696d81dc46e51ull, 0xf0090fa07828eaeeull, 0x2d7a2f58b7a83d38ull, 0xb41120d03264fa88ull, 0x330b7d48d305485dull, 0xefc11de6250f049aull, 0xeefde75aea3a7bc1ull,
        0x9fd511dc7ed66647ull, 0xca052cd3f656d3ecull, 0x11a57e8edbdda17eull, 0x275017051653f070ull, 0x3ca5624edece80b7ull, 0xbbff3ab0fbfb2eacull, 0xba804648892adaadull, 0xc49817dd37202242ull,
        0x32502bd9cc1f0498ull, 0x2ce2e40ee837bd69ull, 0x460c29d11d6e14c2ull, 0x6223b324b94e1ba0ull, 0x4a250948a7a991c2ull, 0x2f01adecfadafc4bull, 0xe86ff1c190570d31ull, 0x3c909488f5568b1full,
        0xdfe5eb8ce57dd216ull, 0x09c46e8f414aedf9ull, 0xd36eb3637a1f5ca1ull, 0xb8aba83f8b7b5b5dull, 0xed8b2f2b44489638ull, 0x22eeae8bdab29c63ull, 0xe69fc9cc899f10ffull, 0xe58ba1b129c73082ull,

        0x008000700001c007ull, 0x033840e0780007e7ull, 0xe0824e9071c5c7eeull, 0x950270d718380c0aull, 0x06f03885dba79908ull, 0x7d3580f0bced0f1cull, 0xb4489b3ba01b03e2ull, 0x5c2718e08923258bull,
        0x021f67b09c4b0bc4ull, 0xe9bb22285e42236bull, 0x558ff481d9690ed9ull, 0xc1be4130e10b7c61ull, 0xb8115f6c5d54c590ull, 0x9535a7febbd67d19ull, 0xd036f3d1a4e23e31ull, 0xc38616aaa58e208eull,
        0x0f8008300020c083ull, 0x4e17d069280093e3ull, 0x4fad3621d15c5477ull, 0x12005279ecea80cfull, 0xb57e22ca753e473eull, 0x77bdec893d79bd92ull, 0x6621b63f0bf0cdaaull, 0xc4051e1e71d9050dull,
        0x55832aad7aaf360full, 0x79aa3cce04cfd401ull, 0x5176c97962a14563ull, 0x17a11a44a2e2fc38ull, 0x0c72ad765e8dda8full, 0xd11e2084b5e0dac4ull, 0x0c4b50558889d3c4ull, 0xafe3072a97f15f51ull,
        0x8680c4f00313cc4eull, 0xc23c491d580e14a8ull, 0x2694c3a7fd139813ull, 0xfb8c0ad3ea4661c5ull, 0x96da85d4b7f655e9ull, 0xa6bffa09fb2bf8d2ull, 0x8d8de302d4e5b481ull, 0x1faeface16906af6ull,
        0x70a3d3bd4fdca089ull, 0x4889b13502b0dbfdull, 0x96e90392b116cd3cull, 0xac88241b84db987cull, 0xefddde77aa8edec8ull, 0x79edc226efe42af0ull, 0xbf56109bb343df16ull, 0x2cef98c7d888fd46ull,
        0x259313304c4df115ull, 0x30acd175895fcaf1ull, 0x7ff5bc7182eb0ab8ull, 0xeeb3122673076f13ull, 0x97aeec02a8c58dadull, 0x41cc09e62329e3acull, 0x57a39e94313e9df8ull, 0x617fb8434b6771feull,
        0x733f079c3037735aull, 0x285ca02e8043bda1ull, 0xef7d50d24fccca18ull, 0xff7b7dc4a4789997ull, 0x153145cf7428c2fdull, 0xd91ff61d71b1ed8cull, 0x9b863dd2133b48a7ull, 0x11ad3599a93d4712ull,
        0xaea33e788cdbf05eull, 0x1c2a63395ebe052eull, 0x3279ed8c844dc969ull, 0xec50f2951992f63dull, 0x0284af160c110592ull, 0x784530c205e0fa35ull, 0xead6f49a0d55911bull, 0x931fd88bb9d0ddbfull,
        0xd267eb3a3b07f4afull, 0x2ada0c837a4a049cull, 0x49535a45e8834c7dull, 0xd7efb116d1760159ull, 0x4061630dced8939full, 0x75cbd20097af5150ull, 0xe2f72477e83701b2ull, 0x156a4638cf8777b4ull,
        0x0d1112f647518c02ull, 0x24da0350e883d428ull, 0x3e861443434063c0ull, 0x4217ec6509d3d9a8ull, 0x26da2c603e522479ull, 0x1d8a10b49f24add1ull, 0xf1a6304f4a9f5819ull, 0x9a927f6c83eec19aull,
        0xaa4a77c4a7226718ull, 0x620b5ad3c8b8b216ull, 0x01f7ccea21d0f214ull, 0x9621c06b410c4e29ull, 0xb1106b5590f4ff01ull, 0x51350b075d26d2f7ull, 0x0f0b15cb110bc253ull, 0x9a33fb95f73b6660ull,
        0xfeb8dc20abed54d5ull, 0x5fc6fe92d21b9e89ull, 0xeef5af47663e1f54ull, 0x1ee6347ceb9773beull, 0xedc53cb34ac13fbbull, 0x6a552c7aab100002ull, 0xfb20c26cfc27f22aull, 0xfe502aa28ad9e239ull,
        0x7bf69ff7d8937ecaull, 0x32a9e2aa2fe3a272ull, 0x41f7f80b731a41fcull, 0xc22d3645a26ae299ull, 0x7acebfdb4a28c4fdull, 0x17e027eaa3182529ull, 0xc04865b6e1341ef9ull, 0xe0981fb762c91de9ull,
        0xa1bee2f9d58456ccull, 0x40ddcf4040b1a420ull, 0xa540ffab15cd5cd7ull, 0xf8371a7ad08a679bull, 0xa9692ad88fd1f235ull, 0xe652f48aec947936ull, 0x814ce4dd2110c7adull, 0xc2135135a4cfd8f6ull,
        0x8116cd30f069c317ull, 0x9c995c3c44c72e88ull, 0x9121be49b224a694ull, 0x6e9490e39593b482ull, 0x843f93939ba8515cull, 0x4ecaaa81e13ebf4dull, 0xe5f5386f8eb970cfull, 0xb0e1173cf140fc23ull,

    };

    Xoroshiro rng;
    size_t i = 0;
    uint64_t s1 = 0, s2 = 0;

    for (size_t k = 0; k < 16; ++k) {
        uint64_t x = rng();
        TEST_EQUAL(x, expect[i++]);
    }

    for (size_t j = 0; j < 8; ++j) {
        s1 = 5 * s1 + 1;
        rng = Xoroshiro(s1);
        for (size_t k = 0; k < 16; ++k) {
            uint64_t x = rng();
            TEST_EQUAL(x, expect[i++]);
        }
    }

    s1 = s2 = 0;
    for (size_t j = 0; j < 8; ++j) {
        s1 = 5 * s2 + 1;
        s2 = 5 * s1 + 1;
        rng = Xoroshiro(s1, s2);
        for (size_t k = 0; k < 16; ++k) {
            uint64_t x = rng();
            TEST_EQUAL(x, expect[i++]);
        }
    }

}

void test_core_random_basic_distributions() {

    static constexpr size_t iterations = 100'000;

    std::mt19937 rng(42);

    auto rbq = [&] { return random_bool(rng); };
    CHECK_RANDOM_GENERATOR(rbq, 0, 1, 0.5, 0.5);
    auto rb2 = [&] { return random_bool(rng, 0.25); };
    CHECK_RANDOM_GENERATOR(rb2, 0, 1, 0.25, 0.433013);
    auto rb3 = [&] { return random_bool(rng, 3, 4); };
    CHECK_RANDOM_GENERATOR(rb3, 0, 1, 0.75, 0.433013);

    auto ri1 = [&] { return random_integer(rng, 100); };
    CHECK_RANDOM_GENERATOR(ri1, 0, 99, 49.5, 28.8661);
    auto ri2 = [&] { return random_integer(rng, 101, 200); };
    CHECK_RANDOM_GENERATOR(ri2, 101, 200, 150.5, 28.8661);
    auto ri3 = [&] { return random_integer(rng, int64_t(0), int64_t(1e18)); };
    CHECK_RANDOM_GENERATOR(ri3, 0, 1e18, 5e17, 2.88661e17);
    auto ri4 = [&] { return random_integer(rng, uint64_t(0), uint64_t(-1)); };
    CHECK_RANDOM_GENERATOR(ri4, 0, 18'446'744'073'709'551'615.0, 9'223'372'036'854'775'807.5, 5'325'116'328'314'171'700.52);

    auto rd1 = [&] { return random_dice<int>(rng); };
    CHECK_RANDOM_GENERATOR(rd1, 1, 6, 3.5, 1.70783);
    auto rd2 = [&] { return random_dice(rng, 3); };
    CHECK_RANDOM_GENERATOR(rd2, 3, 18, 10.5, 2.95804);
    auto rd3 = [&] { return random_dice(rng, 3, 10); };
    CHECK_RANDOM_GENERATOR(rd3, 3, 30, 16.5, 4.97494);

    auto rf1 = [&] { return random_real<double>(rng); };
    CHECK_RANDOM_GENERATOR(rf1, 0, 1, 0.5, 0.288661);
    auto rf2 = [&] { return random_real(rng, -100.0, 100.0); };
    CHECK_RANDOM_GENERATOR(rf2, -100, 100, 0, 57.7350);

    auto rn1 = [&] { return random_normal<double>(rng); };
    CHECK_RANDOM_GENERATOR(rn1, - inf, inf, 0, 1);
    auto rn2 = [&] { return random_normal(rng, 10.0, 5.0); };
    CHECK_RANDOM_GENERATOR(rn2, - inf, inf, 10, 5);

    std::vector<int> v = {1,2,3,4,5,6,7,8,9,10};
    auto rc1 = [&] { return random_choice(rng, v); };
    CHECK_RANDOM_GENERATOR(rc1, 1, 10, 5.5, 2.88661);
    auto rc2 = [&] { return random_choice(rng, {1,2,3,4,5,6,7,8,9,10}); };
    CHECK_RANDOM_GENERATOR(rc2, 1, 10, 5.5, 2.88661);

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
            TRY(sample = random_sample(rng, pop, k));
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

    TEST_THROW(random_sample(rng, pop, 101), std::length_error);

}

void test_core_random_triangular() {

    static constexpr int iterations = 100'000;
    const double epsilon = 1 / std::sqrt(double(iterations));

    std::mt19937 rng(42);
    std::map<int, double> census;

    for (int i = 0; i < iterations; ++i) {
        int x = random_triangle_integer(rng, 5, 0);
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

    for (int i = 0; i < iterations; ++i) {
        int x = random_triangle_integer(rng, 5, 15);
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

void test_core_random_uniform_integer_distribution() {

    UniformIntegerProperties ui(1, 8);

    TEST_EQUAL(ui.min(), 1);
    TEST_EQUAL(ui.max(), 8);
    TEST_EQUAL(ui.mean(), 4.5);
    TEST_NEAR(ui.sd(), 2.291288);
    TEST_EQUAL(ui.variance(), 5.25);

    TEST_EQUAL(ui.pdf(0), 0);
    TEST_EQUAL(ui.pdf(1), 0.125);
    TEST_EQUAL(ui.pdf(2), 0.125);
    TEST_EQUAL(ui.pdf(3), 0.125);
    TEST_EQUAL(ui.pdf(4), 0.125);
    TEST_EQUAL(ui.pdf(5), 0.125);
    TEST_EQUAL(ui.pdf(6), 0.125);
    TEST_EQUAL(ui.pdf(7), 0.125);
    TEST_EQUAL(ui.pdf(8), 0.125);
    TEST_EQUAL(ui.pdf(9), 0);

    TEST_EQUAL(ui.cdf(0), 0);
    TEST_EQUAL(ui.cdf(1), 0.125);
    TEST_EQUAL(ui.cdf(2), 0.25);
    TEST_EQUAL(ui.cdf(3), 0.375);
    TEST_EQUAL(ui.cdf(4), 0.5);
    TEST_EQUAL(ui.cdf(5), 0.625);
    TEST_EQUAL(ui.cdf(6), 0.75);
    TEST_EQUAL(ui.cdf(7), 0.875);
    TEST_EQUAL(ui.cdf(8), 1);
    TEST_EQUAL(ui.cdf(9), 1);

    TEST_EQUAL(ui.ccdf(0), 1);
    TEST_EQUAL(ui.ccdf(1), 1);
    TEST_EQUAL(ui.ccdf(2), 0.875);
    TEST_EQUAL(ui.ccdf(3), 0.75);
    TEST_EQUAL(ui.ccdf(4), 0.625);
    TEST_EQUAL(ui.ccdf(5), 0.5);
    TEST_EQUAL(ui.ccdf(6), 0.375);
    TEST_EQUAL(ui.ccdf(7), 0.25);
    TEST_EQUAL(ui.ccdf(8), 0.125);
    TEST_EQUAL(ui.ccdf(9), 0);

}

void test_core_random_binomial_distribution() {

    BinomialDistributionProperties bin(4, 0.2);

    TEST_NEAR(bin.mean(), 0.8);
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

void test_core_random_dice_distribution() {

    DiceProperties dp;

    TEST_EQUAL(dp.mean(), 3.5);
    TEST_NEAR(dp.sd(), 1.707825);
    TEST_EQUAL(dp.pdf(0), 0);
    TEST_NEAR(dp.pdf(1), 0.166667);
    TEST_NEAR(dp.pdf(2), 0.166667);
    TEST_NEAR(dp.pdf(3), 0.166667);
    TEST_NEAR(dp.pdf(4), 0.166667);
    TEST_NEAR(dp.pdf(5), 0.166667);
    TEST_NEAR(dp.pdf(6), 0.166667);
    TEST_EQUAL(dp.pdf(7), 0);
    TEST_EQUAL(dp.cdf(0), 0);
    TEST_NEAR(dp.cdf(1), 0.166667);
    TEST_NEAR(dp.cdf(2), 0.333333);
    TEST_NEAR(dp.cdf(3), 0.5);
    TEST_NEAR(dp.cdf(4), 0.666667);
    TEST_NEAR(dp.cdf(5), 0.833333);
    TEST_EQUAL(dp.cdf(6), 1);
    TEST_EQUAL(dp.cdf(7), 1);
    TEST_EQUAL(dp.ccdf(0), 1);
    TEST_EQUAL(dp.ccdf(1), 1);
    TEST_NEAR(dp.ccdf(2), 0.833333);
    TEST_NEAR(dp.ccdf(3), 0.666667);
    TEST_NEAR(dp.ccdf(4), 0.5);
    TEST_NEAR(dp.ccdf(5), 0.333333);
    TEST_NEAR(dp.ccdf(6), 0.166667);
    TEST_EQUAL(dp.ccdf(7), 0);

    dp = DiceProperties(2, 6);

    TEST_EQUAL(dp.mean(), 7);
    TEST_NEAR(dp.sd(), 2.415229);
    TEST_EQUAL(dp.pdf(1), 0);
    TEST_NEAR(dp.pdf(2), 0.027778);
    TEST_NEAR(dp.pdf(3), 0.055556);
    TEST_NEAR(dp.pdf(4), 0.083333);
    TEST_NEAR(dp.pdf(5), 0.111111);
    TEST_NEAR(dp.pdf(6), 0.138889);
    TEST_NEAR(dp.pdf(7), 0.166667);
    TEST_NEAR(dp.pdf(8), 0.138889);
    TEST_NEAR(dp.pdf(9), 0.111111);
    TEST_NEAR(dp.pdf(10), 0.083333);
    TEST_NEAR(dp.pdf(11), 0.055556);
    TEST_NEAR(dp.pdf(12), 0.027778);
    TEST_EQUAL(dp.pdf(13), 0);
    TEST_EQUAL(dp.cdf(1), 0);
    TEST_NEAR(dp.cdf(2), 0.027778);
    TEST_NEAR(dp.cdf(3), 0.083333);
    TEST_NEAR(dp.cdf(4), 0.166667);
    TEST_NEAR(dp.cdf(5), 0.277778);
    TEST_NEAR(dp.cdf(6), 0.416667);
    TEST_NEAR(dp.cdf(7), 0.583333);
    TEST_NEAR(dp.cdf(8), 0.722222);
    TEST_NEAR(dp.cdf(9), 0.833333);
    TEST_NEAR(dp.cdf(10), 0.916667);
    TEST_NEAR(dp.cdf(11), 0.972222);
    TEST_EQUAL(dp.cdf(12), 1);
    TEST_EQUAL(dp.cdf(13), 1);
    TEST_EQUAL(dp.ccdf(1), 1);
    TEST_EQUAL(dp.ccdf(2), 1);
    TEST_NEAR(dp.ccdf(3), 0.972222);
    TEST_NEAR(dp.ccdf(4), 0.916667);
    TEST_NEAR(dp.ccdf(5), 0.833333);
    TEST_NEAR(dp.ccdf(6), 0.722222);
    TEST_NEAR(dp.ccdf(7), 0.583333);
    TEST_NEAR(dp.ccdf(8), 0.416667);
    TEST_NEAR(dp.ccdf(9), 0.277778);
    TEST_NEAR(dp.ccdf(10), 0.166667);
    TEST_NEAR(dp.ccdf(11), 0.083333);
    TEST_NEAR(dp.ccdf(12), 0.027778);
    TEST_EQUAL(dp.ccdf(13), 0);

}

void test_core_random_uniform_real_distribution() {

    UniformRealProperties ur(10, 20);

    TEST_EQUAL(ur.min(), 10);
    TEST_EQUAL(ur.max(), 20);
    TEST_EQUAL(ur.mean(), 15);
    TEST_NEAR(ur.sd(), 2.886751);
    TEST_NEAR(ur.variance(), 8.333333);

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

void test_core_random_normal_distribution() {

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

    NormalDistributionProperties norm;

    TEST_EQUAL(norm.mean(), 0);
    TEST_NEAR(norm.sd(), 1);
    TEST_NEAR(norm.variance(), 1);

    double p, q, y, z;

    for (auto& sample: sample_list) {
        TRY(y = norm.pdf(sample.z));          TEST_NEAR(y, sample.pdf);
        TRY(p = norm.cdf(sample.z));          TEST_NEAR(p, sample.cdf);
        TRY(q = norm.ccdf(sample.z));         TEST_NEAR(q, 1 - sample.cdf);
        TRY(z = norm.quantile(sample.cdf));   TEST_NEAR_EPSILON(z, sample.z, 3e-5);
        TRY(z = norm.cquantile(sample.cdf));  TEST_NEAR_EPSILON(z, - sample.z, 3e-5);
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

void test_core_random_spherical_distributions() {

    const int iterations = 100000;
    std::map<int, int> census;
    std::mt19937 rng(42);
    auto freq = [&] (int x) { return double(census[x]) / double(iterations); };
    Double2 v2, sum2;
    Double3 v3, sum3;
    Double4 v4, sum4;

    RandomInSphere<double, 2> inner2(5);
    census.clear();
    sum2 = Double2();
    for (int i = 0; i < iterations; ++i) {
        TRY(v2 = inner2(rng));
        ++census[int(std::floor(v2.r()))];
        sum2 += v2;
    }
    TEST_EQUAL(freq(-1), 0);
    TEST_NEAR_EPSILON(freq(0), 0.04, 0.01);
    TEST_NEAR_EPSILON(freq(1), 0.12, 0.01);
    TEST_NEAR_EPSILON(freq(2), 0.20, 0.01);
    TEST_NEAR_EPSILON(freq(3), 0.28, 0.01);
    TEST_NEAR_EPSILON(freq(4), 0.36, 0.01);
    TEST_EQUAL(freq(5), 0);
    v2 = sum2 / iterations;
    TEST_NEAR_EPSILON_RANGE(v2, Double2(), 0.02);

    RandomInSphere<double, 3> inner3(5);
    census.clear();
    sum3 = Double3();
    for (int i = 0; i < iterations; ++i) {
        TRY(v3 = inner3(rng));
        ++census[int(std::floor(v3.r()))];
        sum3 += v3;
    }
    TEST_EQUAL(freq(-1), 0);
    TEST_NEAR_EPSILON(freq(0), 0.008, 0.005);
    TEST_NEAR_EPSILON(freq(1), 0.056, 0.01);
    TEST_NEAR_EPSILON(freq(2), 0.152, 0.01);
    TEST_NEAR_EPSILON(freq(3), 0.296, 0.01);
    TEST_NEAR_EPSILON(freq(4), 0.488, 0.01);
    TEST_EQUAL(freq(5), 0);
    v3 = sum3 / iterations;
    TEST_NEAR_EPSILON_RANGE(v3, Double3(), 0.02);

    RandomInSphere<double, 4> inner4(5);
    census.clear();
    sum4 = Double4();
    for (int i = 0; i < iterations; ++i) {
        TRY(v4 = inner4(rng));
        ++census[int(std::floor(v4.r()))];
        sum4 += v4;
    }
    TEST_EQUAL(freq(-1), 0);
    TEST_NEAR_EPSILON(freq(0), 0.0016, 0.001);
    TEST_NEAR_EPSILON(freq(1), 0.0240, 0.01);
    TEST_NEAR_EPSILON(freq(2), 0.1040, 0.01);
    TEST_NEAR_EPSILON(freq(3), 0.2800, 0.01);
    TEST_NEAR_EPSILON(freq(4), 0.5904, 0.01);
    TEST_EQUAL(freq(5), 0);
    v4 = sum4 / iterations;
    TEST_NEAR_EPSILON_RANGE(v4, Double4(), 0.02);

    RandomOnSphere<double, 2> outer2(5);
    sum2 = Double2();
    for (int i = 0; i < iterations; ++i) {
        TRY(v2 = outer2(rng));
        TEST_NEAR(v2.r(), 5);
        sum2 += v2;
    }
    v2 = sum2 / iterations;
    TEST_NEAR_EPSILON_RANGE(v2, Double2(), 0.02);

    RandomOnSphere<double, 3> outer3(5);
    sum3 = Double3();
    for (int i = 0; i < iterations; ++i) {
        TRY(v3 = outer3(rng));
        TEST_NEAR(v3.r(), 5);
        sum3 += v3;
    }
    v3 = sum3 / iterations;
    TEST_NEAR_EPSILON_RANGE(v3, Double3(), 0.02);

    RandomOnSphere<double, 4> outer4(5);
    sum4 = Double4();
    for (int i = 0; i < iterations; ++i) {
        TRY(v4 = outer4(rng));
        TEST_NEAR(v4.r(), 5);
        sum4 += v4;
    }
    v4 = sum4 / iterations;
    TEST_NEAR_EPSILON_RANGE(v4, Double4(), 0.02);

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

void test_core_random_weighted_choice_distribution() {

    const int iterations = 1000000;
    std::map<Ustring, int> census;
    std::mt19937 rng(42);
    auto freq = [&] (const Ustring& s) { return double(census[s]) / double(iterations); };

    {
        WeightedChoice<Ustring> choice;
        TEST(choice.empty());
        TEST_EQUAL(choice(rng), "");
        TRY((choice = {
            {"alpha", 0.4},
            {"bravo", 0.3},
            {"charlie", 0.2},
            {"delta", 0.1},
        }));
        TEST(! choice.empty());
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
        TEST(choice.empty());
        TEST_EQUAL(choice(rng), "");
        TRY((choice = {
            {"alpha", 40},
            {"bravo", 30},
            {"charlie", 20},
            {"delta", 10},
        }));
        TEST(! choice.empty());
        census.clear();
        for (int i = 0; i < iterations; ++i)
            TRY(++census[choice(rng)]);
        TEST_NEAR_EPSILON(freq("alpha"), 0.4, 0.01);
        TEST_NEAR_EPSILON(freq("bravo"), 0.3, 0.01);
        TEST_NEAR_EPSILON(freq("charlie"), 0.2, 0.01);
        TEST_NEAR_EPSILON(freq("delta"), 0.1, 0.01);
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

    Xoroshiro rng(42);
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
