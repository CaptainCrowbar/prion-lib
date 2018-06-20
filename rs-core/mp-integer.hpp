#pragma once

#include "rs-core/common.hpp"
#include "rs-core/rational.hpp"
#include <algorithm>
#include <cmath>
#include <ostream>
#include <random>
#include <utility>
#include <vector>

namespace RS {

    // Unsigned integers

    class Nat:
    public LessThanComparable<Nat> {
    public:
        Nat() = default;
        Nat(uint64_t x);
        explicit Nat(Uview s, int base = 0) { init(s, base); }
        template <typename T> explicit operator T() const;
        explicit operator bool() const noexcept { return ! rep.empty(); }
        bool operator!() const noexcept { return ! bool(*this); }
        Nat operator+() const { return *this; }
        Nat& operator++() { return *this += 1; }
        Nat operator++(int) { auto x = *this; ++*this; return x; }
        Nat& operator--() { return *this -= 1; }
        Nat operator--(int) { auto x = *this; --*this; return x; }
        Nat& operator+=(const Nat& rhs);
        Nat& operator-=(const Nat& rhs);
        Nat& operator*=(const Nat& rhs) { Nat z; do_multiply(*this, rhs, z); rep.swap(z.rep); return *this; }
        Nat& operator/=(const Nat& rhs) { Nat q, r; do_divide(*this, rhs, q, r); rep.swap(q.rep); return *this; }
        Nat& operator%=(const Nat& rhs) { Nat q, r; do_divide(*this, rhs, q, r); rep.swap(r.rep); return *this; }
        Nat& operator&=(const Nat& rhs);
        Nat& operator|=(const Nat& rhs);
        Nat& operator^=(const Nat& rhs);
        Nat& operator<<=(ptrdiff_t rhs);
        Nat& operator>>=(ptrdiff_t rhs);
        size_t bits() const noexcept;
        size_t bits_set() const noexcept;
        size_t bytes() const noexcept;
        int compare(const Nat& rhs) const noexcept;
        bool get_bit(size_t i) const noexcept;
        uint8_t get_byte(size_t i) const noexcept;
        void set_bit(size_t i, bool b = true);
        void set_byte(size_t i, uint8_t b);
        void flip_bit(size_t i);
        bool is_even() const noexcept { return rep.empty() || (rep.front() & 1) == 0; }
        bool is_odd() const noexcept { return ! is_even(); }
        Nat pow(const Nat& n) const;
        int sign() const noexcept { return int(bool(*this)); }
        Ustring str(int base = 10, size_t digits = 1) const;
        void write_be(void* ptr, size_t n) const noexcept;
        void write_le(void* ptr, size_t n) const noexcept;
        static Nat from_double(double x);
        template <typename RNG> static Nat random(RNG& rng, const Nat& n);
        static Nat read_be(const void* ptr, size_t n);
        static Nat read_le(const void* ptr, size_t n);
        friend Nat operator*(const Nat& lhs, const Nat& rhs) { Nat z; Nat::do_multiply(lhs, rhs, z); return z; }
        friend Nat operator/(const Nat& lhs, const Nat& rhs) { Nat q, r; Nat::do_divide(lhs, rhs, q, r); return q; }
        friend Nat operator%(const Nat& lhs, const Nat& rhs) { Nat q, r; Nat::do_divide(lhs, rhs, q, r); return r; }
        friend std::pair<Nat, Nat> divide(const Nat& lhs, const Nat& rhs) { Nat q, r; Nat::do_divide(lhs, rhs, q, r); return {q, r}; }
    private:
        friend class Int;
        static constexpr auto mask32 = ~ uint32_t(0);
        std::vector<uint32_t> rep; // Least significant word first
        void init(Uview s, int base);
        void trim() noexcept;
        static constexpr int digit_2(char c) noexcept { return c == '0' ? 0 : c == '1' ? 1 : -1; }
        static constexpr int digit_10(char c) noexcept { return c >= '0' && c <= '9' ? int(c - '0') : -1; }
        static constexpr int digit_16(char c) noexcept
            { return c >= '0' && c <= '9' ? int(c - '0') : c >= 'A' && c <= 'F' ? int(c - 'A') + 10 : c >= 'a' && c <= 'f' ? int(c - 'a') + 10 : -1; }
        static void do_divide(const Nat& x, const Nat& y, Nat& q, Nat& r);
        static void do_multiply(const Nat& x, const Nat& y, Nat& z);
    };

    inline Nat operator+(const Nat& lhs, const Nat& rhs) { auto z = lhs; z += rhs; return z; }
    inline Nat operator-(const Nat& lhs, const Nat& rhs) { auto z = lhs; z -= rhs; return z; }
    inline Nat operator&(const Nat& lhs, const Nat& rhs) { auto z = lhs; z &= rhs; return z; }
    inline Nat operator|(const Nat& lhs, const Nat& rhs) { auto z = lhs; z |= rhs; return z; }
    inline Nat operator^(const Nat& lhs, const Nat& rhs) { auto z = lhs; z ^= rhs; return z; }
    inline Nat operator<<(const Nat& lhs, size_t rhs) { auto z = lhs; z <<= rhs; return z; }
    inline Nat operator>>(const Nat& lhs, size_t rhs) { auto z = lhs; z >>= rhs; return z; }
    inline bool operator==(const Nat& lhs, const Nat& rhs) noexcept { return lhs.compare(rhs) == 0; }
    inline bool operator<(const Nat& lhs, const Nat& rhs) noexcept { return lhs.compare(rhs) == -1; }

    template <typename T>
    Nat::operator T() const {
        T t = 0;
        int bit = 0;
        for (auto w: rep) {
            t += shift_left(T(w), bit);
            bit += 32;
        }
        return t;
    }

    template <typename RNG>
    Nat Nat::random(RNG& rng, const Nat& n) {
        size_t words = n.rep.size();
        std::uniform_int_distribution<uint32_t> head(0, n.rep.back()), tail;
        Nat x;
        do {
            x.rep.resize(words);
            std::generate_n(x.rep.begin(), words - 1, [&] { return tail(rng); });
            x.rep.back() = head(rng);
            x.trim();
        } while (x >= n);
        return x;
    }

    inline Nat abs(const Nat& x) { return x; }
    inline int sign_of(const Nat& x) noexcept { return x.sign(); }
    inline Nat quo(const Nat& lhs, const Nat& rhs) { return lhs / rhs; }
    inline Nat rem(const Nat& lhs, const Nat& rhs) { return lhs % rhs; }
    inline Ustring bin(const Nat& x, size_t digits = 1) { return x.str(2, digits); }
    inline Ustring dec(const Nat& x, size_t digits = 1) { return x.str(10, digits); }
    inline Ustring hex(const Nat& x, size_t digits = 1) { return x.str(16, digits); }
    inline Ustring to_str(const Nat& x) { return x.str(); }
    inline std::ostream& operator<<(std::ostream& out, const Nat& x) { return out << x.str(); }

    // Signed integers

    class Int:
    public LessThanComparable<Int> {
    public:
        Int() = default;
        Int(int64_t x): mag(uint64_t(std::abs(x))), neg(x < 0) {}
        Int(const Nat& x): mag(x), neg(false) {}
        explicit Int(Uview s, int base = 0) { init(s, base); }
        template <typename T> explicit operator T() const;
        explicit operator Nat() const { return mag; }
        explicit operator bool() const noexcept { return bool(mag); }
        bool operator!() const noexcept { return ! bool(*this); }
        Int operator+() const { return *this; }
        Int operator-() const { Int z = *this; if (z) z.neg = ! z.neg; return z; }
        Int& operator++() { return *this += 1; }
        Int operator++(int) { auto x = *this; ++*this; return x; }
        Int& operator--() { return *this -= 1; }
        Int operator--(int) { auto x = *this; --*this; return x; }
        Int& operator+=(const Int& rhs);
        Int& operator-=(const Int& rhs) { return *this += - rhs; }
        Int& operator*=(const Int& rhs) { Int z; do_multiply(*this, rhs, z); std::swap(*this, z); return *this; }
        Int& operator/=(const Int& rhs) { Int q, r; do_divide(*this, rhs, q, r); std::swap(*this, q); return *this; }
        Int& operator%=(const Int& rhs) { Int q, r; do_divide(*this, rhs, q, r); std::swap(*this, r); return *this; }
        Nat abs() const { return mag; }
        int compare(const Int& rhs) const noexcept;
        bool is_even() const noexcept { return mag.is_even(); }
        bool is_odd() const noexcept { return mag.is_odd(); }
        Int pow(const Int& n) const;
        int sign() const noexcept { return neg ? -1 : mag.sign(); }
        Ustring str(int base = 10, size_t digits = 1, bool sign = false) const;
        static Int from_double(double x);
        template <typename RNG> static Int random(RNG& rng, const Int& n) { return Int(Nat::random(rng, n.mag)); }
        friend Int operator*(const Int& lhs, const Int& rhs) { Int z; Int::do_multiply(lhs, rhs, z); return z; }
        friend Int operator/(const Int& lhs, const Int& rhs) { Int q, r; Int::do_divide(lhs, rhs, q, r); return q; }
        friend Int operator%(const Int& lhs, const Int& rhs) { Int q, r; Int::do_divide(lhs, rhs, q, r); return r; }
        friend std::pair<Int, Int> divide(const Int& lhs, const Int& rhs) { Int q, r; Int::do_divide(lhs, rhs, q, r); return {q, r}; }
    private:
        Nat mag;
        bool neg = false;
        void init(Uview s, int base);
        static void do_divide(const Int& x, const Int& y, Int& q, Int& r);
        static void do_multiply(const Int& x, const Int& y, Int& z);
    };

    inline Int operator+(const Int& lhs, const Int& rhs) { auto z = lhs; z += rhs; return z; }
    inline Int operator-(const Int& lhs, const Int& rhs) { auto z = lhs; z -= rhs; return z; }
    inline bool operator==(const Int& lhs, const Int& rhs) noexcept { return lhs.compare(rhs) == 0; }
    inline bool operator<(const Int& lhs, const Int& rhs) noexcept { return lhs.compare(rhs) == -1; }

    template <typename T>
    Int::operator T() const {
        auto t = T(mag);
        if (neg)
            t = T(0) - t;
        return t;
    }

    inline Int abs(const Int& x) { return x.abs(); }
    inline int sign_of(const Int& x) noexcept{ return x.sign(); }
    inline Int quo(const Int& lhs, const Int& rhs) { return lhs / rhs; }
    inline Int rem(const Int& lhs, const Int& rhs) { return lhs % rhs; }
    inline Ustring bin(const Int& x, size_t digits = 1) { return x.str(2, digits); }
    inline Ustring dec(const Int& x, size_t digits = 1) { return x.str(10, digits); }
    inline Ustring hex(const Int& x, size_t digits = 1) { return x.str(16, digits); }
    inline Ustring to_str(const Int& x) { return x.str(); }
    inline std::ostream& operator<<(std::ostream& out, const Int& x) { return out << x.str(); }

    // Related types

    using Uratmp = Rational<Nat>;
    using Ratmp = Rational<Int>;

    // Integer literals

    namespace Literals {

        inline Nat operator""_nat(const char* raw) { return Nat(raw); }
        inline Int operator""_int(const char* raw) { return Int(raw); }

    }

    // Random distributions

    class RandomNat {
    public:
        using result_type = Nat;
        RandomNat(): base(0), range(1) {}
        RandomNat(Nat a, Nat b) { if (a > b) std::swap(a, b); base = a; range = b - a + 1; }
        template <typename RNG> Nat operator()(RNG& rng) { return base + Nat::random(rng, range); }
        Nat min() const { return base; }
        Nat max() const { return base + range - 1; }
    private:
        Nat base, range;
    };

    class RandomInt {
    public:
        using result_type = Int;
        RandomInt(): base(0), range(1) {}
        RandomInt(Int a, Int b) { if (a > b) std::swap(a, b); base = a; range = (b - a + 1).abs(); }
        template <typename RNG> Int operator()(RNG& rng) { return base + Int(Nat::random(rng, range)); }
        Int min() const { return base; }
        Int max() const { return base + range - 1; }
    private:
        Int base;
        Nat range;
    };

}
