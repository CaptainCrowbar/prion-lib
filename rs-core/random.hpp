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
    T random_integer(RNG& rng, T min, T max) {
        static_assert(std::is_integral<T>::value);
        // We need an unsigned integer type big enough for both the RNG and
        // output ranges.
        using rng_type = typename RNG::result_type;
        using out_type = std::make_unsigned_t<T>;
        using larger_type = std::conditional_t<(sizeof(out_type) > sizeof(rng_type)), out_type, rng_type>;
        using work_type = std::conditional_t<(sizeof(larger_type) > sizeof(unsigned)), larger_type, unsigned>;
        // Sanity check the arguments.
        if (min == max)
            return min;
        if (min > max)
            std::swap(min, max);
        // Compare the input range (max-min of the values generated by the
        // RNG) with the output range (max-min of the possible results).
        work_type rng_min = work_type(rng.min());
        work_type rng_range = work_type(rng.max()) - rng_min;
        work_type out_range = work_type(out_type(max) - out_type(min));
        work_type result = 0;
        if (out_range < rng_range) {
            // The RNG range is larger than the output range. Divide the
            // output of the RNG by the rounded down quotient of the ranges.
            // If one range is not an exact multiple of the other, this may
            // yield a value too large; try again.
            work_type ratio = (rng_range - out_range) / (out_range + 1) + 1;
            work_type limit = ratio * (out_range + 1) - 1;
            do result = work_type(rng() - rng_min);
                while (result > limit);
            result /= ratio;
        } else if (out_range == rng_range) {
            // The trivial case where the two ranges are equal.
            result = work_type(rng() - rng_min);
        } else {
            // The output range is larger than the RNG range. Split the output
            // range into a quotient and remainder modulo the RNG range +1;
            // call this function recursively for the quotient, then call the
            // RNG directly for the remainder. Try again if the result is too
            // large.
            work_type high = 0, low = 0;
            work_type ratio = (out_range - rng_range) / (rng_range + 1);
            do {
                high = random_integer(rng, work_type(0), ratio) * (rng_range + 1);
                low = work_type(rng() - rng_min);
            } while (low > out_range - high);
            result = high + low;
        }
        return min + T(result);
    }

    template <typename T, typename RNG>
    T random_integer(RNG& rng, T t) {
        static_assert(std::is_integral<T>::value);
        if (t <= T(1))
            return T(0);
        else
            return random_integer(rng, T(0), t - T(1));
    }

    template <typename T, typename RNG>
    T random_dice(RNG& rng, T n = T(1), T faces = T(6)) {
        static_assert(std::is_integral<T>::value);
        if (n < T(1) || faces < T(1))
            return T(0);
        T sum = T(0);
        for (T i = T(0); i < n; ++i)
            sum += random_integer(rng, T(1), faces);
        return sum;
    }

    template <typename T, typename RNG>
    T random_triangle_integer(RNG& rng, T hi, T lo) {
        static_assert(std::is_integral<T>::value);
        if (hi == lo)
            return hi;
        T d = hi > lo ? hi - lo : lo - hi;
        T x = random_integer(rng, (d + T(1)) * (d + T(2)) / T(2));
        T y = (int_sqrt(T(8) * x + T(1)) - T(1)) / T(2);
        return hi > lo ? lo + y : lo - y;
    }

    template <typename RNG>
    double random_unit(RNG& rng) {
        return double(rng() - rng.min()) / (double(rng.max() - rng.min()) + 1.0);
    }

    template <typename T, typename RNG>
    T random_real(RNG& rng, T a = T(1), T b = T(0)) {
        static_assert(std::is_floating_point<T>::value);
        return a + (b - a) * (T(rng() - rng.min()) / (T(rng.max() - rng.min()) + T(1)));
    }

    template <typename T, typename RNG>
    T random_normal(RNG& rng) {
        static_assert(std::is_floating_point<T>::value);
        T u1 = random_real<T>(rng);
        T u2 = random_real<T>(rng);
        return std::sqrt(T(-2) * std::log(u1)) * std::cos(T(2) * pi_c<T> * u2);
    }

    template <typename T, typename RNG>
    T random_normal(RNG& rng, T m, T s) {
        static_assert(std::is_floating_point<T>::value);
        return m + s * random_normal<T>(rng);
    }

    template <typename RNG>
    bool random_bool(RNG& rng) {
        using R = typename RNG::result_type;
        R min = rng.min();
        R range = rng.max() - min;
        R cutoff = min + range / R(2);
        // Test for odd range is reversed because range is one less than the number of values
        bool odd = range % R(1) == R(0);
        R x = R();
        do x = rng();
            while (odd && x == min);
        return x > cutoff;
    }

    template <typename RNG, typename T>
    bool random_bool(RNG& rng, T p, std::enable_if_t<std::is_floating_point_v<T>>* = nullptr) {
        return random_real<T>(rng) <= p;
    }

    template <typename RNG, typename T>
    bool random_bool(RNG& rng, Rational<T> p) {
        return random_integer(rng, p.den()) < p.num();
    }

    template <typename RNG, typename T>
    bool random_bool(RNG& rng, T num, T den) {
        static_assert(std::is_integral<T>::value);
        return random_integer(rng, den) < num;
    }

    template <typename ForwardRange, typename RNG>
    Meta::RangeValue<ForwardRange> random_choice(RNG& rng, const ForwardRange& range) {
        using T = Meta::RangeValue<ForwardRange>;
        using std::begin;
        using std::end;
        auto i = begin(range), j = end(range);
        if (i == j)
            return T();
        size_t n = std::distance(i, j);
        std::advance(i, random_integer(rng, n));
        return *i;
    }

    template <typename T, typename RNG>
    T random_choice(RNG& rng, std::initializer_list<T> list) {
        if (list.size() == 0)
            return T();
        else
            return list.begin()[random_integer(rng, list.size())];
    }

    template <typename ForwardRange, typename RNG>
    std::vector<Meta::RangeValue<ForwardRange>> random_sample(RNG& rng, const ForwardRange& range, size_t k) {
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
            auto j = random_integer(rng, i);
            if (j < k)
                sample[j] = *p;
        }
        return sample;
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
                v[i] = random_real(rng, T(0), vec[i]);
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
                v[i] = random_real(rng, - vec[i], vec[i]);
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
                v[0] = random_real(rng, - rad, rad);
            } else if constexpr (N <= 4) {
                do {
                    for (auto& x: v)
                        x = random_real<T>(rng, -1, 1);
                } while (v.r2() > 1);
                v *= rad;
            } else {
                for (size_t i = 0; i < N; i += 2) {
                    T a = std::sqrt(-2 * std::log(random_real<T>(rng)));
                    T b = 2 * pi_c<T> * random_real<T>(rng);
                    v[i] = a * std::cos(b);
                    if (i + 1 < N)
                        v[i + 1] = a * std::sin(b);
                }
                v *= rad * std::pow(random_real<T>(rng), T(1) / T(N)) / v.r();
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
                v[0] = random_bool(rng) ? rad : - rad;
            } else if constexpr (N == 2) {
                T phi = random_real<T>(rng, 0, 2 * pi_c<T>);
                v[0] = rad * std::cos(phi);
                v[1] = rad * std::sin(phi);
            } else if constexpr (N == 3) {
                T phi = random_real<T>(rng, 0, 2 * pi_c<T>);
                v[2] = random_real<T>(rng, - rad, rad);
                T r = std::sqrt(rad * rad - v[2] * v[2]);
                v[0] = r * std::cos(phi);
                v[1] = r * std::sin(phi);
            } else {
                for (size_t i = 0; i < N; i += 2) {
                    T a = std::sqrt(-2 * std::log(random_real<T>(rng)));
                    T b = 2 * pi_c<T> * random_real<T>(rng);
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
            template <typename RNG> F operator()(RNG& rng, F range) const { return random_integer(rng, range); }
        };

        template <typename F>
        struct WeightRange<F, false> {
            template <typename RNG> F operator()(RNG& rng, F range) const { return random_real(rng, range); }
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
            for (size_t i = 0; i < n; ++i)
                bp[i] = uint8_t(random_integer(rng, 256));
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
            size_t b = random_integer(rng, a, n - 1);
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
