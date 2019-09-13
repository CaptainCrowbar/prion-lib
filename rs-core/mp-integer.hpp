#pragma once

#include "rs-core/common.hpp"
#include "rs-core/rational.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <ostream>
#include <random>
#include <utility>
#include <vector>

namespace RS {

    // Unsigned integers

    class Mpuint:
    public LessThanComparable<Mpuint> {
    public:
        Mpuint() = default;
        Mpuint(uint64_t x);
        explicit Mpuint(Uview s, int base = 0) { init(s, base); }
        template <typename T> explicit operator T() const;
        explicit operator bool() const noexcept { return ! rep.empty(); }
        bool operator!() const noexcept { return ! bool(*this); }
        Mpuint operator+() const { return *this; }
        Mpuint& operator++() { return *this += 1; }
        Mpuint operator++(int) { auto x = *this; ++*this; return x; }
        Mpuint& operator--() { return *this -= 1; }
        Mpuint operator--(int) { auto x = *this; --*this; return x; }
        Mpuint& operator+=(const Mpuint& rhs);
        Mpuint& operator-=(const Mpuint& rhs);
        Mpuint& operator*=(const Mpuint& rhs) { Mpuint z; do_multiply(*this, rhs, z); rep.swap(z.rep); return *this; }
        Mpuint& operator/=(const Mpuint& rhs) { Mpuint q, r; do_divide(*this, rhs, q, r); rep.swap(q.rep); return *this; }
        Mpuint& operator%=(const Mpuint& rhs) { Mpuint q, r; do_divide(*this, rhs, q, r); rep.swap(r.rep); return *this; }
        Mpuint& operator&=(const Mpuint& rhs);
        Mpuint& operator|=(const Mpuint& rhs);
        Mpuint& operator^=(const Mpuint& rhs);
        Mpuint& operator<<=(ptrdiff_t rhs);
        Mpuint& operator>>=(ptrdiff_t rhs);
        size_t bits() const noexcept;
        size_t bits_set() const noexcept;
        size_t bytes() const noexcept;
        int compare(const Mpuint& rhs) const noexcept;
        bool get_bit(size_t i) const noexcept;
        uint8_t get_byte(size_t i) const noexcept;
        void set_bit(size_t i, bool b = true);
        void set_byte(size_t i, uint8_t b);
        void flip_bit(size_t i);
        size_t hash() const noexcept { return hash_range(rep); }
        bool is_even() const noexcept { return rep.empty() || (rep.front() & 1) == 0; }
        bool is_odd() const noexcept { return ! is_even(); }
        Mpuint pow(const Mpuint& n) const;
        int sign() const noexcept { return int(bool(*this)); }
        Ustring str(int base = 10, size_t digits = 1) const;
        void write_be(void* ptr, size_t n) const noexcept;
        void write_le(void* ptr, size_t n) const noexcept;
        static Mpuint from_double(double x);
        template <typename RNG> static Mpuint random(RNG& rng, const Mpuint& n);
        static Mpuint read_be(const void* ptr, size_t n);
        static Mpuint read_le(const void* ptr, size_t n);
        friend Mpuint operator*(const Mpuint& lhs, const Mpuint& rhs) { Mpuint z; Mpuint::do_multiply(lhs, rhs, z); return z; }
        friend Mpuint operator/(const Mpuint& lhs, const Mpuint& rhs) { Mpuint q, r; Mpuint::do_divide(lhs, rhs, q, r); return q; }
        friend Mpuint operator%(const Mpuint& lhs, const Mpuint& rhs) { Mpuint q, r; Mpuint::do_divide(lhs, rhs, q, r); return r; }
        friend std::pair<Mpuint, Mpuint> divide(const Mpuint& lhs, const Mpuint& rhs) { Mpuint q, r; Mpuint::do_divide(lhs, rhs, q, r); return {q, r}; }
    private:
        friend class Mpint;
        static constexpr auto mask32 = ~ uint32_t(0);
        std::vector<uint32_t> rep; // Least significant word first
        void init(Uview s, int base);
        void trim() noexcept;
        static void do_divide(const Mpuint& x, const Mpuint& y, Mpuint& q, Mpuint& r);
        static void do_multiply(const Mpuint& x, const Mpuint& y, Mpuint& z);
    };

    inline Mpuint operator+(const Mpuint& lhs, const Mpuint& rhs) { auto z = lhs; z += rhs; return z; }
    inline Mpuint operator-(const Mpuint& lhs, const Mpuint& rhs) { auto z = lhs; z -= rhs; return z; }
    inline Mpuint operator&(const Mpuint& lhs, const Mpuint& rhs) { auto z = lhs; z &= rhs; return z; }
    inline Mpuint operator|(const Mpuint& lhs, const Mpuint& rhs) { auto z = lhs; z |= rhs; return z; }
    inline Mpuint operator^(const Mpuint& lhs, const Mpuint& rhs) { auto z = lhs; z ^= rhs; return z; }
    inline Mpuint operator<<(const Mpuint& lhs, size_t rhs) { auto z = lhs; z <<= rhs; return z; }
    inline Mpuint operator>>(const Mpuint& lhs, size_t rhs) { auto z = lhs; z >>= rhs; return z; }
    inline bool operator==(const Mpuint& lhs, const Mpuint& rhs) noexcept { return lhs.compare(rhs) == 0; }
    inline bool operator<(const Mpuint& lhs, const Mpuint& rhs) noexcept { return lhs.compare(rhs) == -1; }

    template <typename T>
    Mpuint::operator T() const {
        T t = 0;
        int bit = 0;
        for (auto w: rep) {
            t += shift_left(T(w), bit);
            bit += 32;
        }
        return t;
    }

    template <typename RNG>
    Mpuint Mpuint::random(RNG& rng, const Mpuint& n) {
        size_t words = n.rep.size();
        std::uniform_int_distribution<uint32_t> head(0, n.rep.back()), tail;
        Mpuint x;
        do {
            x.rep.resize(words);
            std::generate_n(x.rep.begin(), words - 1, [&] { return tail(rng); });
            x.rep.back() = head(rng);
            x.trim();
        } while (x >= n);
        return x;
    }

    inline Mpuint abs(const Mpuint& x) { return x; }
    inline int sign_of(const Mpuint& x) noexcept { return x.sign(); }
    inline Mpuint quo(const Mpuint& lhs, const Mpuint& rhs) { return lhs / rhs; }
    inline Mpuint rem(const Mpuint& lhs, const Mpuint& rhs) { return lhs % rhs; }
    inline Ustring bin(const Mpuint& x, size_t digits = 1) { return x.str(2, digits); }
    inline Ustring dec(const Mpuint& x, size_t digits = 1) { return x.str(10, digits); }
    inline Ustring hex(const Mpuint& x, size_t digits = 1) { return x.str(16, digits); }
    inline Ustring to_str(const Mpuint& x) { return x.str(); }
    inline std::ostream& operator<<(std::ostream& out, const Mpuint& x) { return out << x.str(); }

    // Signed integers

    class Mpint:
    public LessThanComparable<Mpint> {
    public:
        Mpint() = default;
        Mpint(int64_t x): mag(uint64_t(std::abs(x))), neg(x < 0) {}
        Mpint(const Mpuint& x): mag(x), neg(false) {}
        explicit Mpint(Uview s, int base = 0) { init(s, base); }
        template <typename T> explicit operator T() const;
        explicit operator Mpuint() const { return mag; }
        explicit operator bool() const noexcept { return bool(mag); }
        bool operator!() const noexcept { return ! bool(*this); }
        Mpint operator+() const { return *this; }
        Mpint operator-() const { Mpint z = *this; if (z) z.neg = ! z.neg; return z; }
        Mpint& operator++() { return *this += 1; }
        Mpint operator++(int) { auto x = *this; ++*this; return x; }
        Mpint& operator--() { return *this -= 1; }
        Mpint operator--(int) { auto x = *this; --*this; return x; }
        Mpint& operator+=(const Mpint& rhs);
        Mpint& operator-=(const Mpint& rhs) { return *this += - rhs; }
        Mpint& operator*=(const Mpint& rhs) { Mpint z; do_multiply(*this, rhs, z); std::swap(*this, z); return *this; }
        Mpint& operator/=(const Mpint& rhs) { Mpint q, r; do_divide(*this, rhs, q, r); std::swap(*this, q); return *this; }
        Mpint& operator%=(const Mpint& rhs) { Mpint q, r; do_divide(*this, rhs, q, r); std::swap(*this, r); return *this; }
        Mpuint abs() const { return mag; }
        int compare(const Mpint& rhs) const noexcept;
        size_t hash() const noexcept { return hash_value(mag, neg); }
        bool is_even() const noexcept { return mag.is_even(); }
        bool is_odd() const noexcept { return mag.is_odd(); }
        Mpint pow(const Mpint& n) const;
        int sign() const noexcept { return neg ? -1 : mag.sign(); }
        Ustring str(int base = 10, size_t digits = 1, bool sign = false) const;
        static Mpint from_double(double x);
        template <typename RNG> static Mpint random(RNG& rng, const Mpint& n) { return Mpint(Mpuint::random(rng, n.mag)); }
        friend Mpint operator*(const Mpint& lhs, const Mpint& rhs) { Mpint z; Mpint::do_multiply(lhs, rhs, z); return z; }
        friend Mpint operator/(const Mpint& lhs, const Mpint& rhs) { Mpint q, r; Mpint::do_divide(lhs, rhs, q, r); return q; }
        friend Mpint operator%(const Mpint& lhs, const Mpint& rhs) { Mpint q, r; Mpint::do_divide(lhs, rhs, q, r); return r; }
        friend std::pair<Mpint, Mpint> divide(const Mpint& lhs, const Mpint& rhs) { Mpint q, r; Mpint::do_divide(lhs, rhs, q, r); return {q, r}; }
    private:
        Mpuint mag;
        bool neg = false;
        void init(Uview s, int base);
        static void do_divide(const Mpint& x, const Mpint& y, Mpint& q, Mpint& r);
        static void do_multiply(const Mpint& x, const Mpint& y, Mpint& z);
    };

    inline Mpint operator+(const Mpint& lhs, const Mpint& rhs) { auto z = lhs; z += rhs; return z; }
    inline Mpint operator-(const Mpint& lhs, const Mpint& rhs) { auto z = lhs; z -= rhs; return z; }
    inline bool operator==(const Mpint& lhs, const Mpint& rhs) noexcept { return lhs.compare(rhs) == 0; }
    inline bool operator<(const Mpint& lhs, const Mpint& rhs) noexcept { return lhs.compare(rhs) == -1; }

    template <typename T>
    Mpint::operator T() const {
        auto t = T(mag);
        if (neg)
            t = T(0) - t;
        return t;
    }

    inline Mpint abs(const Mpint& x) { return x.abs(); }
    inline int sign_of(const Mpint& x) noexcept{ return x.sign(); }
    inline Mpint quo(const Mpint& lhs, const Mpint& rhs) { return lhs / rhs; }
    inline Mpint rem(const Mpint& lhs, const Mpint& rhs) { return lhs % rhs; }
    inline Ustring bin(const Mpint& x, size_t digits = 1) { return x.str(2, digits); }
    inline Ustring dec(const Mpint& x, size_t digits = 1) { return x.str(10, digits); }
    inline Ustring hex(const Mpint& x, size_t digits = 1) { return x.str(16, digits); }
    inline Ustring to_str(const Mpint& x) { return x.str(); }
    inline std::ostream& operator<<(std::ostream& out, const Mpint& x) { return out << x.str(); }

    // Related types

    using Mpratio = Rational<Mpint>;

    // Integer literals

    namespace Literals {

        inline Mpint operator""_mpi(const char* raw) { return Mpint(raw); }
        inline Mpuint operator""_mpu(const char* raw) { return Mpuint(raw); }

    }

    // Random distributions

    class RandomMpint {
    public:
        using result_type = Mpint;
        RandomMpint(): base(0), range(1) {}
        RandomMpint(Mpint a, Mpint b) { if (a > b) std::swap(a, b); base = a; range = (b - a + 1).abs(); }
        template <typename RNG> Mpint operator()(RNG& rng) { return base + Mpint(Mpuint::random(rng, range)); }
        Mpint min() const { return base; }
        Mpint max() const { return base + range - 1; }
    private:
        Mpint base;
        Mpuint range;
    };

    class RandomMpuint {
    public:
        using result_type = Mpuint;
        RandomMpuint(): base(0), range(1) {}
        RandomMpuint(Mpuint a, Mpuint b) { if (a > b) std::swap(a, b); base = a; range = b - a + 1; }
        template <typename RNG> Mpuint operator()(RNG& rng) { return base + Mpuint::random(rng, range); }
        Mpuint min() const { return base; }
        Mpuint max() const { return base + range - 1; }
    private:
        Mpuint base, range;
    };

}

namespace std {

    template <>
    class numeric_limits<RS::Mpint>:
    public RS::NumericLimitsBase<RS::Mpint> {
    private:
        using type                        = RS::Mpint;
        using base                        = RS::NumericLimitsBase<type>;
    public:
        static constexpr bool is_bounded  = false;
        static constexpr bool is_exact    = true;
        static constexpr bool is_integer  = true;
        static constexpr bool is_modulo   = false;
        static constexpr bool is_signed   = true;
        static constexpr int radix        = 2;
    };

    template <>
    class numeric_limits<RS::Mpuint>:
    public RS::NumericLimitsBase<RS::Mpuint> {
    private:
        using type                        = RS::Mpuint;
        using base                        = RS::NumericLimitsBase<type>;
    public:
        static constexpr bool is_bounded  = false;
        static constexpr bool is_exact    = true;
        static constexpr bool is_integer  = true;
        static constexpr bool is_modulo   = false;
        static constexpr bool is_signed   = false;
        static constexpr int radix        = 2;
    };

}

RS_DEFINE_STD_HASH(RS::Mpint);
RS_DEFINE_STD_HASH(RS::Mpuint);
