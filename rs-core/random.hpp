#pragma once

#include "rs-core/common.hpp"
#include "rs-core/float.hpp"
#include "rs-core/int128.hpp"
#include "rs-core/meta.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/string.hpp"
#include "rs-core/vector.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <map>
#include <random>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4723) // potential divide by 0
#endif

namespace RS {

    // LCG functions

    constexpr uint32_t lcg32(uint32_t x) noexcept { return uint32_t(32310901ul * x + 850757001ul); }
    constexpr uint64_t lcg64(uint64_t x) noexcept { return uint64_t(3935559000370003845ull * x + 8831144850135198739ull); }

    class Lcg32:
    public EqualityComparable<Lcg32> {
    public:
        using result_type = uint32_t;
        constexpr Lcg32() noexcept: x(0) {}
        explicit constexpr Lcg32(uint32_t s) noexcept: x(s) {}
        constexpr uint32_t operator()() noexcept { x = lcg32(x); return x; }
        constexpr bool operator==(const Lcg32& rhs) const noexcept { return x == rhs.x; }
        constexpr void seed(uint32_t s) noexcept { x = s; }
        static constexpr uint32_t min() noexcept { return 0; }
        static constexpr uint32_t max() noexcept { return ~ uint32_t(0); }
    private:
        uint32_t x;
    };

    class Lcg64:
    public EqualityComparable<Lcg64> {
    public:
        using result_type = uint64_t;
        constexpr Lcg64() noexcept: x(0) {}
        explicit constexpr Lcg64(uint64_t s) noexcept: x(s) {}
        uint64_t constexpr operator()() noexcept { x = lcg64(x); return x; }
        bool constexpr operator==(const Lcg64& rhs) const noexcept { return x == rhs.x; }
        void constexpr seed(uint64_t s) noexcept { x = s; }
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        uint64_t x;
    };

    // ISAAC generators

    class Isaac32 {
    public:
        using result_type = uint32_t;
        Isaac32() noexcept { seed(nullptr, 0); }
        explicit Isaac32(uint32_t s) noexcept { seed(&s, 1); }
        Isaac32(const uint32_t* sptr, size_t len) noexcept { seed(sptr, len); }
        Isaac32(std::initializer_list<uint32_t> s) noexcept { seed(s); }
        uint32_t operator()() noexcept;
        void seed() noexcept { seed(nullptr, 0); }
        void seed(uint32_t s) noexcept { seed(&s, 1); }
        void seed(const uint32_t* sptr, size_t len) noexcept;
        void seed(std::initializer_list<uint32_t> s) noexcept;
        static constexpr uint32_t min() noexcept { return 0; }
        static constexpr uint32_t max() noexcept { return ~ uint32_t(0); }
    private:
        uint32_t res[256];
        uint32_t mem[256];
        uint32_t a, b, c, index;
        void next_block() noexcept;
    };

    class Isaac64 {
    public:
        using result_type = uint64_t;
        Isaac64() noexcept { seed(nullptr, 0); }
        explicit Isaac64(uint64_t s) noexcept { seed(&s, 1); }
        Isaac64(const uint64_t* sptr, size_t len) noexcept { seed(sptr, len); }
        Isaac64(std::initializer_list<uint64_t> s) noexcept { seed(s); }
        uint64_t operator()() noexcept;
        void seed() noexcept { seed(nullptr, 0); }
        void seed(uint64_t s) noexcept { seed(&s, 1); }
        void seed(const uint64_t* sptr, size_t len) noexcept;
        void seed(std::initializer_list<uint64_t> s) noexcept;
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        uint64_t res[256];
        uint64_t mem[256];
        uint64_t a, b, c, index;
        void next_block() noexcept;
    };

    // PCG generators

    namespace RS_Detail {

        template <typename S>
        constexpr void pcg_advance(S& state, S a, S b, int64_t offset) noexcept {
            S u = offset < 0 ? ~ S(0) - ~ uint64_t(offset) : S(offset);
            S mul = 1, add = 0;
            while (u) {
                if (u & 1) {
                    mul *= a;
                    add = add * a + b;
                }
                b = (a + 1) * b;
                a *= a;
                u >>= 1;
            }
            state = mul * state + add;
        }

    }

    class Pcg32 {
    public:
        using result_type = uint32_t;
        constexpr Pcg32() noexcept { seed(pcg_default); }
        constexpr explicit Pcg32(uint64_t s) noexcept { seed(s); }
        constexpr uint32_t operator()() noexcept {
            auto s = state;
            state = pcg_a32 * state + pcg_b32;
            return rotr(uint32_t((s ^ (s >> 18)) >> 27), int(s >> 59) & 31);
        }
        constexpr void advance(int64_t offset) noexcept { RS_Detail::pcg_advance(state, pcg_a32, pcg_b32, offset); }
        constexpr void seed(uint64_t s) noexcept { state = pcg_a32 * (s + pcg_b32) + pcg_b32; }
        static constexpr uint32_t min() noexcept { return 0; }
        static constexpr uint32_t max() noexcept { return ~ uint32_t(0); }
    private:
        static constexpr uint64_t pcg_default = 0xcafef00dd15ea5e5ull;
        static constexpr uint64_t pcg_a32 = 0x5851f42d4c957f2dull;
        static constexpr uint64_t pcg_b32 = 0x14057b7ef767814full;
        uint64_t state = 0;
    };

    class Pcg64 {
    public:
        using result_type = uint64_t;
        constexpr Pcg64() noexcept { seed(pcg_default); }
        constexpr explicit Pcg64(Uint128 s) noexcept { seed(s); }
        constexpr explicit Pcg64(uint64_t hi, uint64_t lo) noexcept { seed(hi, lo); }
        constexpr uint64_t operator()() noexcept {
            state = pcg_a64 * state + pcg_b64;
            return rotr(uint64_t(state ^ (state >> 64)), int(state >> 122) & 63);
        }
        constexpr void advance(int64_t offset) noexcept { RS_Detail::pcg_advance(state, pcg_a64, pcg_b64, offset); }
        constexpr void seed(Uint128 s) noexcept { state = pcg_a64 * (s + pcg_b64) + pcg_b64; }
        constexpr void seed(uint64_t hi, uint64_t lo) noexcept { seed(Uint128{hi, lo}); }
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        static constexpr uint64_t pcg_default = 0xcafef00dd15ea5e5ull;
        static constexpr Uint128 pcg_a64{0x2360ed051fc65da4ull, 0x4385df649fccf645ull};
        static constexpr Uint128 pcg_b64{0x5851f42d4c957f2dull, 0x14057b7ef767814full};
        Uint128 state;
    };

    // Xoshiro and related generators

    class SplitMix64 {
    public:
        using result_type = uint64_t;
        constexpr SplitMix64() noexcept: x(0) {}
        constexpr explicit SplitMix64(uint64_t s) noexcept: x (s) {}
        constexpr uint64_t operator()() noexcept {
            using namespace RS::Literals;
            x += 0x9e3779b97f4a7c15_u64;
            uint64_t y = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9_u64;
            y = (y ^ (y >> 27)) * 0x94d049bb133111eb_u64;
            return y ^ (y >> 31);
        }
        constexpr void seed(uint64_t s = 0) noexcept { x = s; }
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        uint64_t x;
    };

    namespace RS_Detail {

        template <typename T>
        constexpr void seed_sm64_1_2(T s, T& a, T& b) noexcept {
            SplitMix64 sm(s);
            a = T(sm());
            b = T(sm());
        }

        template <typename T>
        constexpr void seed_sm64_1_4(T s, T& a, T& b, T& c, T& d) noexcept {
            SplitMix64 sm(s);
            a = T(sm());
            b = T(sm());
            c = T(sm());
            d = T(sm());
        }

        template <typename T>
        constexpr void seed_sm64_2_4(T s, T t, T& a, T& b, T& c, T& d) noexcept {
            SplitMix64 sm(s);
            a = T(sm());
            b = T(sm());
            sm.seed(t);
            c = T(sm());
            d = T(sm());
        }

    }

    class Xoroshiro64s {
    public:
        using result_type = uint32_t;
        constexpr Xoroshiro64s() noexcept { seed(0); }
        constexpr explicit Xoroshiro64s(uint32_t s) noexcept { seed(s); }
        constexpr Xoroshiro64s(uint32_t s, uint32_t t) noexcept { seed(s, t); }
        constexpr uint32_t operator()() noexcept {
            using namespace RS::Literals;
            uint32_t x = a * 0x9e3779bb_u32;
            b ^= a;
            a = rotl(a, 26) ^ b ^ (b << 9);
            b = rotl(b, 13);
            return x;
        }
        constexpr void seed(uint32_t s = 0) noexcept { RS_Detail::seed_sm64_1_2(s, a, b); }
        constexpr void seed(uint32_t s, uint32_t t) noexcept { a = s; b = t; }
        static constexpr uint32_t min() noexcept { return 0; }
        static constexpr uint32_t max() noexcept { return ~ uint32_t(0); }
    private:
        uint32_t a = 0, b = 0;
    };

    class Xoroshiro64ss {
    public:
        using result_type = uint32_t;
        constexpr Xoroshiro64ss() noexcept { seed(0); }
        constexpr explicit Xoroshiro64ss(uint32_t s) noexcept { seed(s); }
        constexpr Xoroshiro64ss(uint32_t s, uint32_t t) noexcept { seed(s, t); }
        constexpr uint32_t operator()() noexcept {
            using namespace RS::Literals;
            uint32_t x = rotl(a * 0x9e3779bb_u32, 5) * 5;
            b ^= a;
            a = rotl(a, 26) ^ b ^ (b << 9);
            b = rotl(b, 13);
            return x;
        }
        constexpr void seed(uint32_t s = 0) noexcept { RS_Detail::seed_sm64_1_2(s, a, b); }
        constexpr void seed(uint32_t s, uint32_t t) noexcept { a = s; b = t; }
        static constexpr uint32_t min() noexcept { return 0; }
        static constexpr uint32_t max() noexcept { return ~ uint32_t(0); }
    private:
        uint32_t a = 0, b = 0;
    };

    class Xoroshiro128p {
    public:
        using result_type = uint64_t;
        constexpr Xoroshiro128p() noexcept { seed(0); }
        constexpr explicit Xoroshiro128p(uint64_t s) noexcept { seed(s); }
        constexpr Xoroshiro128p(uint64_t s, uint64_t t) noexcept { seed(s, t); }
        constexpr uint64_t operator()() noexcept {
            uint64_t x = a + b;
            b ^= a;
            a = rotl(a, 24) ^ b ^ (b << 16);
            b = rotl(b, 37);
            return x;
        }
        constexpr void seed(uint64_t s = 0) noexcept { RS_Detail::seed_sm64_1_2(s, a, b); }
        constexpr void seed(uint64_t s, uint64_t t) noexcept { a = s; b = t; }
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        uint64_t a = 0, b = 0;
    };

    class Xoroshiro128ss {
    public:
        using result_type = uint64_t;
        constexpr Xoroshiro128ss() noexcept { seed(0); }
        constexpr explicit Xoroshiro128ss(uint64_t s) noexcept { seed(s); }
        constexpr Xoroshiro128ss(uint64_t s, uint64_t t) noexcept { seed(s, t); }
        constexpr uint64_t operator()() noexcept {
            uint64_t x = rotl(a * 5, 7) * 9;
            b ^= a;
            a = rotl(a, 24) ^ b ^ (b << 16);
            b = rotl(b, 37);
            return x;
        }
        constexpr void seed(uint64_t s = 0) noexcept { RS_Detail::seed_sm64_1_2(s, a, b); }
        constexpr void seed(uint64_t s, uint64_t t) noexcept { a = s; b = t; }
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        uint64_t a = 0, b = 0;
    };

    class Xoshiro128p {
    public:
        using result_type = uint32_t;
        constexpr Xoshiro128p() noexcept { seed(0); }
        constexpr explicit Xoshiro128p(uint32_t s) noexcept { seed(s); }
        constexpr Xoshiro128p(uint32_t s, uint32_t t) noexcept { seed(s, t); }
        constexpr Xoshiro128p(uint32_t s, uint32_t t, uint32_t u, uint32_t v) noexcept { seed(s, t, u, v); }
        constexpr uint32_t operator()() noexcept {
            uint32_t x = a + d, y = b << 9;
            c ^= a; d ^= b; b ^= c; a ^= d; c ^= y;
            d = rotl(d, 11);
            return x;
        }
        constexpr void seed(uint32_t s = 0) noexcept { RS_Detail::seed_sm64_1_4(s, a, b, c, d); }
        constexpr void seed(uint32_t s, uint32_t t) noexcept { RS_Detail::seed_sm64_2_4(s, t, a, b, c, d); }
        constexpr void seed(uint32_t s, uint32_t t, uint32_t u, uint32_t v) noexcept { a = s; b = t; c = u; d = v; }
        static constexpr uint32_t min() noexcept { return 0; }
        static constexpr uint32_t max() noexcept { return ~ uint32_t(0); }
    private:
        uint32_t a = 0, b = 0, c = 0, d = 0;
    };

    class Xoshiro128ss {
    public:
        using result_type = uint32_t;
        constexpr Xoshiro128ss() noexcept { seed(0); }
        constexpr explicit Xoshiro128ss(uint32_t s) noexcept { seed(s); }
        constexpr Xoshiro128ss(uint32_t s, uint32_t t) noexcept { seed(s, t); }
        constexpr Xoshiro128ss(uint32_t s, uint32_t t, uint32_t u, uint32_t v) noexcept { seed(s, t, u, v); }
        constexpr uint32_t operator()() noexcept {
            uint32_t x = rotl(a * 5, 7) * 9, y = b << 9;
            c ^= a; d ^= b; b ^= c; a ^= d; c ^= y;
            d = rotl(d, 11);
            return x;
        }
        constexpr void seed(uint32_t s = 0) noexcept { RS_Detail::seed_sm64_1_4(s, a, b, c, d); }
        constexpr void seed(uint32_t s, uint32_t t) noexcept { RS_Detail::seed_sm64_2_4(s, t, a, b, c, d); }
        constexpr void seed(uint32_t s, uint32_t t, uint32_t u, uint32_t v) noexcept { a = s; b = t; c = u; d = v; }
        static constexpr uint32_t min() noexcept { return 0; }
        static constexpr uint32_t max() noexcept { return ~ uint32_t(0); }
    private:
        uint32_t a = 0, b = 0, c = 0, d = 0;
    };

    class Xoshiro256p {
    public:
        using result_type = uint64_t;
        constexpr Xoshiro256p() noexcept { seed(0); }
        constexpr explicit Xoshiro256p(uint64_t s) noexcept { seed(s); }
        constexpr Xoshiro256p(uint64_t s, uint64_t t) noexcept { seed(s, t); }
        constexpr Xoshiro256p(uint64_t s, uint64_t t, uint64_t u, uint64_t v) noexcept { seed(s, t, u, v); }
        constexpr uint64_t operator()() noexcept {
            uint64_t x = a + d, y = b << 17;
            c ^= a; d ^= b; b ^= c; a ^= d; c ^= y;
            d = rotl(d, 45);
            return x;
        }
        constexpr void seed(uint64_t s = 0) noexcept { RS_Detail::seed_sm64_1_4(s, a, b, c, d); }
        constexpr void seed(uint64_t s, uint64_t t) noexcept { RS_Detail::seed_sm64_2_4(s, t, a, b, c, d); }
        constexpr void seed(uint64_t s, uint64_t t, uint64_t u, uint64_t v) noexcept { a = s; b = t; c = u; d = v; }
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        uint64_t a = 0, b = 0, c = 0, d = 0;
    };

    class Xoshiro256ss {
    public:
        using result_type = uint64_t;
        constexpr Xoshiro256ss() noexcept { seed(0); }
        constexpr explicit Xoshiro256ss(uint64_t s) noexcept { seed(s); }
        constexpr Xoshiro256ss(uint64_t s, uint64_t t) noexcept { seed(s, t); }
        constexpr Xoshiro256ss(uint64_t s, uint64_t t, uint64_t u, uint64_t v) noexcept { seed(s, t, u, v); }
        constexpr uint64_t operator()() noexcept {
            uint64_t x = rotl(b * 5, 7) * 9, y = b << 17;
            c ^= a; d ^= b; b ^= c; a ^= d; c ^= y;
            d = rotl(d, 45);
            return x;
        }
        constexpr void seed(uint64_t s = 0) noexcept { RS_Detail::seed_sm64_1_4(s, a, b, c, d); }
        constexpr void seed(uint64_t s, uint64_t t) noexcept { RS_Detail::seed_sm64_2_4(s, t, a, b, c, d); }
        constexpr void seed(uint64_t s, uint64_t t, uint64_t u, uint64_t v) noexcept { a = s; b = t; c = u; d = v; }
        static constexpr uint64_t min() noexcept { return 0; }
        static constexpr uint64_t max() noexcept { return ~ uint64_t(0); }
    private:
        uint64_t a = 0, b = 0, c = 0, d = 0;
    };

    using Xoshiro = Xoshiro256ss;

    // Basic random distributions

    template <typename T, typename RNG>
    T general_random_integer(RNG& rng, T min, T max) {
        // We need an unsigned integer type big enough for both the RNG and
        // output ranges.
        static_assert(std::is_integral<T>::value);
        using output_range_type = std::make_unsigned_t<T>;
        using rng_result_type = typename RNG::result_type;
        using working_type = std::common_type_t<output_range_type, rng_result_type, unsigned>;
        // Sanity check the arguments.
        if (min == max)
            return min;
        if (min > max)
            std::swap(min, max);
        // Compare the input range (max-min of the values generated by the
        // RNG) with the output range (max-min of the possible results).
        working_type rng_min = working_type(rng.min());
        working_type rng_range = working_type(rng.max()) - rng_min;
        working_type out_range = working_type(output_range_type(max) - output_range_type(min));
        working_type result;
        if (out_range < rng_range) {
            // The RNG range is larger than the output range. Divide the
            // output of the RNG by the rounded down quotient of the ranges.
            // If one range is not an exact multiple of the other, this may
            // yield a value too large; try again.
            working_type ratio = (rng_range - out_range) / (out_range + 1) + 1;
            working_type limit = ratio * (out_range + 1) - 1;
            do result = working_type(rng() - rng_min);
                while (result > limit);
            result /= ratio;
        } else if (out_range == rng_range) {
            // The trivial case where the two ranges are equal.
            result = working_type(rng() - rng_min);
        } else {
            // The output range is larger than the RNG range. Split the output
            // range into a quotient and remainder modulo the RNG range +1;
            // call this function recursively for the quotient, then call the
            // RNG directly for the remainder. Try again if the result is too
            // large.
            working_type high = 0, low = 0;
            working_type ratio = (out_range - rng_range) / (rng_range + 1);
            do {
                high = general_random_integer(rng, working_type(0), ratio) * (rng_range + 1);
                low = working_type(rng() - rng_min);
            } while (low > out_range - high);
            result = high + low;
        }
        return min + T(result);
    }

    template <typename T>
    class RandomInteger {
    public:
        static_assert(std::is_integral_v<T>);
        using result_type = T;
        RandomInteger() = default;
        RandomInteger(T a, T b) noexcept: min_(a), max_(b) { if (min_ > max_) std::swap(min_, max_); }
        template <typename RNG> T operator()(RNG& rng) const { return general_random_integer(rng, min_, max_); }
        T min() const noexcept { return min_; }
        T max() const noexcept { return max_; }
    private:
        T min_ = T(0);
        T max_ = T(0);
    };

    template <typename T> RandomInteger<T> random_integer(T t) { if (t <= T(1)) return {}; else return {T(0), t - T(1)}; }
    template <typename T> RandomInteger<T> random_integer(T a, T b) { return {a, b}; }

    class RandomBoolean {
    public:
        using result_type = bool;
        RandomBoolean() = default;
        explicit RandomBoolean(Rat p) noexcept: prob_(std::clamp(p, Rat(0), Rat(1))) {}
        template <typename RNG> bool operator()(RNG& rng) const { return random_integer(prob_.den())(rng) < prob_.num(); }
        Rat prob() const noexcept { return prob_; }
    private:
        Rat prob_ = {1,2};
    };

    inline RandomBoolean random_boolean() { return {}; }
    inline RandomBoolean random_boolean(Rat p) { return RandomBoolean(p); }
    inline RandomBoolean random_boolean(int a, int b) { return RandomBoolean(Rat(a, b)); }

    inline RandomBoolean random_boolean(double p) {
        static constexpr int maxint = std::numeric_limits<int>::max();
        Rat r;
        if (p <= 0)
            r = 0;
        else if (p >= 1)
            r = 1;
        else
            r = {int(maxint * p), maxint};
        return RandomBoolean(r);
    }

    template <typename T>
    class RandomBinomial {
    public:
        static_assert(std::is_integral_v<T>);
        using result_type = T;
        RandomBinomial() = default;
        RandomBinomial(const Rational<T>& p, T n) noexcept:
            prob_(std::clamp(p, Rational<T>(0), Rational<T>(1))), num_(std::max(n, T(0))), gen_(0, prob_.den() - 1) {}
        template <typename RNG> T operator()(RNG& rng) const {
            T count = 0;
            for (T i = 0; i < num_; ++i)
                if (gen_(rng) < prob_.num())
                    ++count;
            return count;
        }
        Rational<T> prob() const noexcept { return prob_; }
        T number() const noexcept { return num_; }
    private:
        Rational<T> prob_;
        T num_ = 0;
        RandomInteger<T> gen_;
    };

    template <typename T> RandomBinomial<T> random_binomial(const Rational<T>& p, T n) { return {p, n}; }

    template <typename T>
    class RandomDice {
    public:
        static_assert(std::is_integral_v<T>);
        using result_type = T;
        RandomDice() = default;
        RandomDice(T n, T faces) noexcept: num_(n), faces_(faces) {}
        template <typename RNG> T operator()(RNG& rng) const {
            if (num_ < T(1) || faces_ < T(1))
                return T(0);
            RandomInteger<T> gen(1, faces_);
            T sum = T(0);
            for (T i = T(0); i < num_; ++i)
                sum += gen(rng);
            return sum;
        }
        T number() const noexcept { return num_; }
        T faces() const noexcept { return faces_; }
    private:
        T num_ = T(1);
        T faces_ = T(6);
    };

    template <typename T> RandomDice<T> random_dice(T n = 1, T faces = 6) { return {n, faces}; }

    template <typename T>
    class RandomTriangleInteger {
    public:
        static_assert(std::is_integral_v<T>);
        using result_type = T;
        RandomTriangleInteger() = default;
        RandomTriangleInteger(T hi, T lo) noexcept:
            lo_(lo), hi_(hi), gen_() {
                T diff = hi_ > lo_ ? hi - lo : lo - hi;
                T range = (diff + T(1)) * (diff + T(2)) / T(2);
                gen_ = {T(0), T(range) - T(1)};
            }
        template <typename RNG> T operator()(RNG& rng) const {
            if (hi_ == lo_)
                return lo_;
            T x = gen_(rng);
            T y = (int_sqrt(T(8) * x + T(1)) - T(1)) / T(2);
            return hi_ > lo_ ? lo_ + y : lo_ - y;
        }
        T high_end() const noexcept { return hi_; }
        T low_end() const noexcept { return lo_; }
    private:
        T lo_ = T(0);
        T hi_ = T(0);
        RandomInteger<T> gen_;
    };

    template <typename T> RandomTriangleInteger<T> random_triangle_integer(T hi, T lo) { return {hi, lo}; }

    template <typename T>
    class RandomReal {
    public:
        static_assert(std::is_floating_point_v<T>);
        using result_type = T;
        RandomReal() = default;
        RandomReal(T a, T b) noexcept: min_(a), max_(b) { if (min_ > max_) std::swap(min_, max_); }
        template <typename RNG> T operator()(RNG& rng) const {
            return min_ + (max_ - min_) * (T(rng() - rng.min()) / (T(rng.max() - rng.min()) + 1));
        }
        T min() const noexcept { return min_; }
        T max() const noexcept { return max_; }
    private:
        T min_ = 0;
        T max_ = 1;
    };

    template <typename T> RandomReal<T> random_real() { return {}; }
    template <typename T> RandomReal<T> random_real(T t) { return {0, t}; }
    template <typename T> RandomReal<T> random_real(T a, T b) { return {a, b}; }

    template <typename T>
    class RandomNormal {
    public:
        static_assert(std::is_floating_point_v<T>);
        using result_type = T;
        RandomNormal() = default;
        RandomNormal(T mean, T sd) noexcept: mean_(mean), sd_(std::abs(sd)) {}
        template <typename RNG> T operator()(RNG& rng) const {
            RandomReal<T> unit;
            T u1 = unit(rng);
            T u2 = unit(rng);
            T z = std::sqrt(-2 * std::log(u1)) * std::cos(2 * pi_c<T> * u2);
            return mean_ + z * sd_;
        }
        T mean() const noexcept { return mean_; }
        T sd() const noexcept { return sd_; }
    private:
        T mean_ = 0;
        T sd_ = 1;
    };

    template <typename T> RandomNormal<T> random_normal() { return {}; }
    template <typename T> RandomNormal<T> random_normal(T mean, T sd) { return {mean, sd}; }

    template <typename T>
    class RandomDiscreteNormal {
    public:
        static_assert(std::is_integral_v<T>);
        using result_type = T;
        RandomDiscreteNormal(): RandomDiscreteNormal(0, 1) {}
        RandomDiscreteNormal(const Rational<T>& mean, const Rational<T>& sd);
        template <typename RNG> T operator()(RNG& rng) const;
        Rational<T> mean() const noexcept { return mean_; }
        Rational<T> sd() const noexcept { return sd_; }
    private:
        Rational<T> mean_;
        Rational<T> sd_;
        RandomInteger<T> gen_;
        int num_;
        Rational<T> scale_;
        Rational<T> offset_;
    };

    template <typename T>
    RandomDiscreteNormal<T>::RandomDiscreteNormal(const Rational<T>& mean, const Rational<T>& sd) {
        static constexpr int sqrt_n = 3;
        static const Rational<T> k = 100;
        static const Rational<T> sqrt_12 = {97,28};
        mean_ = mean;
        sd_ = sd.abs();
        T max = (k * sqrt_12 * sd_).round();
        gen_ = {0, max};
        num_ = sqrt_n * sqrt_n;
        scale_ = 1 / (k * sqrt_n);
        offset_ = mean_ - scale_ * num_ * max / 2;
    }

    template <typename T>
    template <typename RNG>
    T RandomDiscreteNormal<T>::operator()(RNG& rng) const {
        T sum = 0;
        for (int i = 0; i < num_; ++i)
            sum += gen_(rng);
        auto r = scale_ * sum + offset_;
        return r.round();
    }

    template <typename T> RandomDiscreteNormal<T> random_discrete_normal(const Rational<T>& mean, const Rational<T>& sd) { return {mean,sd}; }

    template <typename T>
    class RandomChoice {
    public:
        using result_type = T;
        RandomChoice() = default;
        template <typename InputIterator> RandomChoice(InputIterator i, InputIterator j): list_(i, j) {}
        template <typename InputRange> explicit RandomChoice(const InputRange& list) {
            using std::begin;
            using std::end;
            list_.assign(begin(list), end(list));
        }
        RandomChoice(std::initializer_list<T> list): list_(list) {}
        template <typename RNG> const T& operator()(RNG& rng) { return list_[random_integer(list_.size())(rng)]; }
        bool empty() const noexcept { return list_.empty(); }
        size_t size() const noexcept { return list_.size(); }
    private:
        std::vector<T> list_;
    };

    template <typename InputIterator> RandomChoice<Meta::IteratorValue<InputIterator>> random_choice(InputIterator i, InputIterator j) {
        using RC = RandomChoice<Meta::IteratorValue<InputIterator>>;
        return RC(i, j);
    }

    template <typename InputRange> RandomChoice<Meta::RangeValue<InputRange>> random_choice(const InputRange& list) {
        using RC = RandomChoice<Meta::RangeValue<InputRange>>;
        return RC(list);
    }

    template <typename T> RandomChoice<T> random_choice(std::initializer_list<T> list) {
        using RC = RandomChoice<T>;
        return RC(list);
    }

    template <typename ForwardIterator, typename RNG> const auto& random_choice_from(ForwardIterator i, ForwardIterator j, RNG& rng) {
        auto n = std::distance(i, j);
        auto x = random_integer(n)(rng);
        std::advance(i, x);
        return *i;
    }

    template <typename ForwardRange, typename RNG> const auto& random_choice_from(const ForwardRange& list, RNG& rng) {
        using std::begin;
        using std::end;
        return random_choice_from(begin(list), end(list), rng);
    }

    template <typename T, typename RNG> const T& random_choice_from(std::initializer_list<T> list, RNG& rng) {
        return list.begin()[random_integer(list.size())(rng)];
    }

    // Random distribution properties

    class UniformIntegerProperties {
    public:
        explicit UniformIntegerProperties(int n) noexcept: UniformIntegerProperties(0, std::max(n, 1) - 1) {}
        explicit UniformIntegerProperties(int a, int b) noexcept: lo(std::min(a, b)), hi(std::max(a, b)) {}
        int min() const noexcept { return lo; }
        int max() const noexcept { return hi; }
        double mean() const noexcept { return (double(lo) + double(hi)) / 2; }
        double sd() const noexcept { return sqrt(variance()); }
        double variance() const noexcept { double r = hi - lo + 1; return (r * r - 1) / 12; }
        double pdf(int x) const noexcept;
        double cdf(int x) const noexcept;
        double ccdf(int x) const noexcept;
    private:
        int lo, hi;
    };

    class BinomialDistributionProperties {
    public:
        BinomialDistributionProperties(int t, double p) noexcept: tests(t), prob(p) {}
        int t() const noexcept { return tests; }
        double p() const noexcept { return prob; }
        int min() const noexcept { return 0; }
        int max() const noexcept { return tests; }
        double mean() const noexcept { return t() * p(); }
        double sd() const noexcept { return sqrt(variance()); }
        double variance() const noexcept { return t() * p() * (1 - p()); }
        double pdf(int x) const noexcept;
        double cdf(int x) const noexcept;
        double ccdf(int x) const noexcept;
    private:
        int tests;
        double prob;
    };

    class DiceProperties {
    public:
        DiceProperties() = default;
        explicit DiceProperties(int number, int faces = 6) noexcept: num(number), fac(faces) {}
        int number() const noexcept { return num; }
        int faces() const noexcept { return fac; }
        int min() const noexcept { return num; }
        int max() const noexcept { return num * fac; }
        double mean() const noexcept { return num * (double(fac) + 1) / 2; }
        double sd() const noexcept { return sqrt(variance()); }
        double variance() const noexcept { double f = fac; return num * (f * f - 1) / 12; }
        double pdf(int x) const noexcept;
        double cdf(int x) const noexcept { return ccdf(num * (fac + 1) - x); }
        double ccdf(int x) const noexcept;
    private:
        int num = 1, fac = 6;
    };

    class UniformRealProperties {
    public:
        UniformRealProperties() = default;
        explicit UniformRealProperties(double a, double b = 0) noexcept: lo(std::min(a, b)), hi(std::max(a, b)) {}
        double min() const noexcept { return lo; }
        double max() const noexcept { return hi; }
        double mean() const noexcept { return (lo + hi) / 2; }
        double sd() const noexcept;
        double variance() const noexcept { double r = hi - lo; return r * r / 12; }
        double pdf(double x) const noexcept;
        double cdf(double x) const noexcept;
        double ccdf(double x) const noexcept;
        double quantile(double p) const noexcept { return lo + p * (hi - lo); }
        double cquantile(double q) const noexcept { return hi - q * (hi - lo); }
    private:
        double lo = 0, hi = 1;
    };

    class NormalDistributionProperties {
    public:
        NormalDistributionProperties() = default;
        explicit NormalDistributionProperties(double m, double s) noexcept: xm(m), xs(std::abs(s)) {}
        double min() const noexcept { return {}; }
        double max() const noexcept { return {}; }
        double mean() const noexcept { return xm; }
        double sd() const noexcept { return xs; }
        double variance() const noexcept { return xs * xs; }
        double pdf(double x) const noexcept { return pdf_z((x - xm) / xs); }
        double cdf(double x) const noexcept { return cdf_z((x - xm) / xs); }
        double ccdf(double x) const noexcept { return cdf_z((xm - x) / xs); }
        double quantile(double p) const noexcept;
        double cquantile(double q) const noexcept;
    private:
        double xm = 0, xs = 1;
        double pdf_z(double z) const noexcept;
        double cdf_z(double z) const noexcept;
        double cquantile_z(double q) const noexcept;
    };

    // Random samples

    template <typename ForwardRange, typename RNG>
    std::vector<Meta::RangeValue<ForwardRange>> random_sample_from(const ForwardRange& range, size_t k, RNG& rng) {
        using T = Meta::RangeValue<ForwardRange>;
        using std::begin;
        using std::end;
        auto b = begin(range), e = end(range);
        size_t n = std::distance(b, e);
        if (k > n)
            throw std::length_error("Sample size requested is larger than the population");
        auto p = b;
        std::advance(p, k);
        std::vector<T> sample(b, p);
        for (size_t i = k; i < n; ++i, ++p) {
            auto j = random_integer(i)(rng);
            if (j < k)
                sample[j] = *p;
        }
        return sample;
    }

    // Spatial distributions

    template <typename T, size_t N>
    class RandomVector {
    public:
        using result_type = Vector<T, N>;
        using scalar_type = T;
        static constexpr size_t dim = N;
        RandomVector(): vec(T(1)) {}
        explicit RandomVector(T t): vec(t) {}
        explicit RandomVector(const Vector<T, N>& v): vec(v) {}
        template <typename RNG> Vector<T, N> operator()(RNG& rng) const;
        Vector<T, N> scale() const { return vec; }
    private:
        result_type vec;
    };

        template <typename T, size_t N>
        template <typename RNG>
        Vector<T, N> RandomVector<T, N>::operator()(RNG& rng) const {
            Vector<T, N> v;
            for (size_t i = 0; i < N; ++i)
                v[i] = RandomReal<T>(0, vec[i])(rng);
            return v;
        }

    template <typename T, size_t N>
    class SymmetricRandomVector {
    public:
        using result_type = Vector<T, N>;
        using scalar_type = T;
        static constexpr size_t dim = N;
        SymmetricRandomVector(): vec(T(1)) {}
        explicit SymmetricRandomVector(T t): vec(t) {}
        explicit SymmetricRandomVector(const Vector<T, N>& v): vec(v) {}
        template <typename RNG> Vector<T, N> operator()(RNG& rng) const;
        Vector<T, N> scale() const { return vec; }
    private:
        result_type vec;
    };

        template <typename T, size_t N>
        template <typename RNG>
        Vector<T, N> SymmetricRandomVector<T, N>::operator()(RNG& rng) const {
            Vector<T, N> v;
            for (size_t i = 0; i < N; ++i)
                v[i] = RandomReal<T>(- vec[i], vec[i])(rng);
            return v;
        }

    template <typename T, size_t N>
    class RandomInSphere {
    public:
        static_assert(N > 0);
        using result_type = Vector<T, N>;
        using scalar_type = T;
        static constexpr size_t dim = N;
        RandomInSphere(): rad(1) {}
        explicit RandomInSphere(T r): rad(std::fabs(r)) {}
        template <typename RNG> Vector<T, N> operator()(RNG& rng) const;
        T radius() const noexcept { return rad; }
    private:
        T rad;
    };

        template <typename T, size_t N>
        template <typename RNG>
        Vector<T, N> RandomInSphere<T, N>::operator()(RNG& rng) const {
            Vector<T, N> v;
            if constexpr (N == 1) {
                v[0] = RandomReal<T>(- rad, rad)(rng);
            } else if constexpr (N <= 4) {
                do {
                    for (auto& x: v)
                        x = RandomReal<T>(-1, 1)(rng);
                } while (v.r2() > 1);
                v *= rad;
            } else {
                for (size_t i = 0; i < N; i += 2) {
                    T a = std::sqrt(-2 * std::log(RandomReal<T>()(rng)));
                    T b = 2 * pi_c<T> * RandomReal<T>()(rng);
                    v[i] = a * std::cos(b);
                    if (i + 1 < N)
                        v[i + 1] = a * std::sin(b);
                }
                v *= rad * std::pow(RandomReal<T>()(rng), T(1) / T(N)) / v.r();
            }
            return v;
        }

    template <typename T, size_t N>
    class RandomOnSphere {
    public:
        static_assert(N > 0);
        using result_type = Vector<T, N>;
        using scalar_type = T;
        static constexpr size_t dim = N;
        RandomOnSphere(): rad(1) {}
        explicit RandomOnSphere(T r): rad(std::fabs(r)) {}
        template <typename RNG> Vector<T, N> operator()(RNG& rng) const;
        T radius() const noexcept { return rad; }
    private:
        T rad;
    };

        template <typename T, size_t N>
        template <typename RNG>
        Vector<T, N> RandomOnSphere<T, N>::operator()(RNG& rng) const {
            Vector<T, N> v;
            if constexpr (N == 1) {
                v[0] = RandomBoolean()(rng) ? rad : - rad;
            } else if constexpr (N == 2) {
                T phi = RandomReal<T>(0, 2 * pi_c<T>)(rng);
                v[0] = rad * std::cos(phi);
                v[1] = rad * std::sin(phi);
            } else if constexpr (N == 3) {
                T phi = RandomReal<T>(0, 2 * pi_c<T>)(rng);
                v[2] = RandomReal<T>(- rad, rad)(rng);
                T r = std::sqrt(rad * rad - v[2] * v[2]);
                v[0] = r * std::cos(phi);
                v[1] = r * std::sin(phi);
            } else {
                for (size_t i = 0; i < N; i += 2) {
                    T a = std::sqrt(-2 * std::log(RandomReal<T>()(rng)));
                    T b = 2 * pi_c<T> * RandomReal<T>()(rng);
                    v[i] = a * std::cos(b);
                    if (i + 1 < N)
                        v[i + 1] = a * std::sin(b);
                }
                v *= rad / v.r();
            }
            return v;
        }

    // Unique distribution

    template <typename T>
    struct UniqueDistribution {
    public:
        using distribution_type = T;
        using result_type = decltype(std::declval<T>()(std::declval<std::minstd_rand&>()));
        explicit UniqueDistribution(T& t): base(&t), log() {}
        template <typename RNG> result_type operator()(RNG& rng);
        void clear() noexcept { log.clear(); }
        bool empty() const noexcept { return log.empty(); }
        result_type min() const { return base->min(); }
        result_type max() const { return base->max(); }
        size_t size() const noexcept { return log.size(); }
    private:
        T* base;
        std::set<result_type> log;
    };

        template <typename T>
        template <typename RNG>
        typename UniqueDistribution<T>::result_type UniqueDistribution<T>::operator()(RNG& rng) {
            result_type x;
            do x = (*base)(rng);
                while (log.count(x));
            log.insert(x);
            return x;
        }

    template <typename T> inline UniqueDistribution<T> unique_distribution(T& t) { return UniqueDistribution<T>(t); }

    // Weighted choice distribution

    namespace RS_Detail {

        template <typename F, bool Int = std::is_integral<F>::value>
        struct WeightRange;

        template <typename F>
        struct WeightRange<F, true> {
            template <typename RNG> F operator()(RNG& rng, F range) const { return random_integer(range)(rng); }
        };

        template <typename F>
        struct WeightRange<F, false> {
            template <typename RNG> F operator()(RNG& rng, F range) const { return random_real(range)(rng); }
        };

    }

    template <typename T, typename F = double>
    class WeightedChoice {
    public:
        using frequency_type = F;
        using result_type = T;
        WeightedChoice() = default;
        WeightedChoice(std::initializer_list<std::pair<T, F>> pairs) { append(pairs); }
        template <typename InputRange> explicit WeightedChoice(const InputRange& pairs) { append(pairs); }
        template <typename RNG> T operator()(RNG& rng) const;
        void add(const T& t, F f);
        void append(std::initializer_list<std::pair<T, F>> pairs);
        template <typename InputRange> void append(const InputRange& pairs);
        bool empty() const noexcept { return total() <= F(0); }
    private:
        static_assert(std::is_arithmetic<F>::value);
        using weight_range = RS_Detail::WeightRange<F>;
        std::map<F, T> table;
        F total() const { return table.empty() ? F() : std::prev(table.end())->first; }
    };

        template <typename T, typename F>
        template <typename RNG>
        T WeightedChoice<T, F>::operator()(RNG& rng) const {
            if (empty())
                return T();
            else
                return table.upper_bound(weight_range()(rng, total()))->second;
        }

        template <typename T, typename F>
        void WeightedChoice<T, F>::add(const T& t, F f) {
            if (f > F(0))
                table.insert(table.end(), {total() + f, t});
        }

        template <typename T, typename F>
        void WeightedChoice<T, F>::append(std::initializer_list<std::pair<T, F>> pairs) {
            std::vector<std::pair<T, F>> vfs{pairs};
            auto sum = total();
            for (auto& vf: vfs) {
                if (vf.second > F(0)) {
                    sum += vf.second;
                    table.insert(table.end(), {sum, vf.first});
                }
            }
        }

        template <typename T, typename F>
        template <typename InputRange>
        void WeightedChoice<T, F>::append(const InputRange& pairs) {
            auto sum = total();
            for (auto& p: pairs) {
                if (p.second > F(0)) {
                    sum += p.second;
                    table.insert(table.end(), {sum, p.first});
                }
            }
        }

    // Other random algorithms

    template <typename RNG>
    void random_bytes(RNG& rng, void* ptr, size_t n) {
        static constexpr auto uint64_max = ~ uint64_t(0);
        if (! ptr || ! n)
            return;
        auto range_m1 = uint64_t(std::min(uintmax_t(RNG::max()) - uintmax_t(RNG::min()), uintmax_t(uint64_max)));
        size_t block;
        if (range_m1 == uint64_max)
            block = 8;
        else
            block = (ilog2p1(range_m1) - 1) / 8;
        auto bp = static_cast<uint8_t*>(ptr);
        if (block) {
            while (n >= block) {
                auto x = rng() - RNG::min();
                memcpy(bp, &x, block);
                bp += block;
                n -= block;
            }
            if (n) {
                auto x = rng() - RNG::min();
                memcpy(bp, &x, n);
            }
        } else {
            auto random_byte = random_integer<int>(256);
            for (size_t i = 0; i < n; ++i)
                bp[i] = uint8_t(random_byte(rng));
        }
    }

    template <typename RNG1, typename RNG2>
    void seed_from(RNG1& src, RNG2& dst) {
        std::array<uint32_t, RNG2::state_size> array;
        std::generate_n(array.data(), array.size(), std::ref(src));
        std::seed_seq seq(array.begin(), array.end());
        dst.seed(seq);
    }

    template <typename RNG, typename RandomAccessIterator>
    void shuffle(RNG& rng, RandomAccessIterator i, RandomAccessIterator j) {
        size_t n = std::distance(i, j);
        for (size_t a = 0; a + 1 < n; ++a) {
            size_t b = random_integer(a, n - 1)(rng);
            if (a != b)
                std::swap(i[a], i[b]);
        }
    }

    template <typename RNG, typename RandomAccessRange>
    void shuffle(RNG& rng, RandomAccessRange& range) {
        using std::begin;
        using std::end;
        shuffle(begin(range), end(range), rng);
    }

    // Text generators

    Ustring lorem_ipsum(uint64_t seed, size_t bytes, bool paras = true);

}

#ifdef _MSC_VER
    #pragma warning(pop)
#endif
