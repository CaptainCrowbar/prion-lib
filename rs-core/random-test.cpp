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

namespace {

    const double inf = std::numeric_limits<double>::infinity();
    const double max32 = double(~ uint32_t(0));
    const double max64 = double(~ uint64_t(0));
    const double mean32 = max32 / 2;
    const double mean64 = max64 / 2;
    const double sd32 = sqrt((ldexp(1.0, 64) - 1) / 12);
    const double sd64 = sqrt((ldexp(1.0, 128) - 1) / 12);

    #define CHECK_RANDOM_GENERATOR(gen, xmin, xmax, xmean, xsd) \
        do { \
            using T = std::decay_t<decltype(gen())>; \
            double epsilon, lo = inf, hi = - inf, sum = 0, sum2 = 0; \
            if (xmin > - inf && xmax < inf) \
                epsilon = (xmax - xmin) / sqrt(double(iterations)); \
            else \
                epsilon = 2 * xsd / sqrt(double(iterations)); \
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
            double s = sqrt(sum2 / iterations - m * m); \
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

    auto rf1 = [&] { return random_float<double>(rng); };
    CHECK_RANDOM_GENERATOR(rf1, 0, 1, 0.5, 0.288661);
    auto rf2 = [&] { return random_float(rng, -100.0, 100.0); };
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

    static constexpr size_t pop_size = 100;
    static constexpr size_t sample_iterations = 100;
    const double expect_mean = double(pop_size + 1) / 2;
    const double expect_sd = sqrt(double(pop_size * pop_size - 1) / 12);
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
                double sd = sqrt((sum2 - count * mean * mean) / (count - 1));
                TEST_NEAR_EPSILON(sd, expect_sd, 2);
            }
        }
    }
    TEST_THROW(random_sample(rng, pop, 101), std::length_error);

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
        sd[i] = sqrt(sum2[i] / iterations - mean[i] * mean[i]);
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
        sd[i] = sqrt(sum2[i] / iterations - mean[i] * mean[i]);
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
        ++census[int(floor(v2.r()))];
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
        ++census[int(floor(v3.r()))];
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
        ++census[int(floor(v4.r()))];
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
    std::map<U8string, int> census;
    std::mt19937 rng(42);
    auto freq = [&] (const U8string& s) { return double(census[s]) / double(iterations); };

    {
        WeightedChoice<U8string> choice;
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
        WeightedChoice<U8string, int> choice;
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

void test_core_random_algorithms() {

    static constexpr size_t n = 10000;

    std::mt19937 rng(42);
    uint32_t u32 = 0;
    uint64_t u64 = 0;
    double x = 0, sum = 0, sum2 = 0, mean = 0, sd = 0;

    for (size_t i = 0; i < n; ++i) {
        TRY(random_bytes(rng, &u32, 4));
        x = ldexp(u32, -32);
        sum += x;
        sum2 += x * x;
    }
    mean = sum / n;
    sd = sqrt(sum2 / n - mean * mean);
    TEST_NEAR_EPSILON(mean, 0.5, 0.05);
    TEST_NEAR_EPSILON(sd, 0.288675, 0.05);

    sum = sum2 = 0;
    for (size_t i = 0; i < n; ++i) {
        TRY(random_bytes(rng, &u64, 8));
        x = ldexp(u64, -64);
        sum += x;
        sum2 += x * x;
    }
    mean = sum / n;
    sd = sqrt(sum2 / n - mean * mean);
    TEST_NEAR_EPSILON(mean, 0.5, 0.05);
    TEST_NEAR_EPSILON(sd, 0.288675, 0.05);

    U8string s;

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

void test_core_random_text_generators() {

    #ifdef _MSC_VER
        static constexpr int max_iterations = 20;
    #else
        static constexpr int max_iterations = 70;
    #endif

    Xoroshiro rng(42);
    U8string s;
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
