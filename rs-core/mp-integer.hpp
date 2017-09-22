#pragma once

#include "rs-core/common.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <ostream>
#include <random>
#include <stdexcept>
#include <utility>
#include <vector>

namespace RS {

    // Unsigned integers

    class Nat:
    public LessThanComparable<Nat> {
    public:
        Nat() = default;
        Nat(uint64_t x);
        explicit Nat(const U8string& s, int base = 0) { init(s.data(), base); }
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
        U8string str(int base = 10, size_t digits = 1) const;
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
        void init(const char* s, int base);
        void trim() noexcept;
        static constexpr int clz32(uint32_t x) noexcept { return x == 0 ? 32 : sizeof(int) >= 4 ? __builtin_clz(x) : __builtin_clzl(x); }
        static constexpr int popcount32(uint32_t x) noexcept { return sizeof(int) >= 4 ? __builtin_popcount(x) : __builtin_popcountl(x); }
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

    inline Nat::Nat(uint64_t x) {
        if (x > 0)
            rep.push_back(uint32_t(x));
        if (x > mask32)
            rep.push_back(uint32_t(x >> 32));
    }

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

    inline Nat& Nat::operator+=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(std::max(rep.size(), rhs.rep.size()), 0);
        uint64_t sum = 0;
        for (size_t i = 0; i < common; ++i) {
            sum += uint64_t(rep[i]) + uint64_t(rhs.rep[i]);
            rep[i] = uint32_t(sum & mask32);
            sum >>= 32;
        }
        const auto* rptr = &rep;
        if (rhs.rep.size() > common)
            rptr = &rhs.rep;
        for (size_t i = common; i < rep.size(); ++i) {
            sum += uint64_t((*rptr)[i]);
            rep[i] = uint32_t(sum & mask32);
            sum >>= 32;
        }
        if (sum)
            rep.push_back(sum);
        trim();
        return *this;
    }

    inline Nat& Nat::operator-=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        bool carry = false;
        for (size_t i = 0; i < common; ++i) {
            bool c = rep[i] < rhs.rep[i] || (carry && rep[i] == rhs.rep[i]);
            rep[i] -= rhs.rep[i];
            if (carry)
                --rep[i];
            carry = c;
        }
        for (size_t i = common; carry && i < rep.size(); ++i)
            carry = --rep[i] == mask32;
        trim();
        return *this;
    }

    inline Nat& Nat::operator&=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(common);
        for (size_t i = 0; i < common; ++i)
            rep[i] &= rhs.rep[i];
        trim();
        return *this;
    }

    inline Nat& Nat::operator|=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(std::max(rep.size(), rhs.rep.size()));
        for (size_t i = 0; i < common; ++i)
            rep[i] |= rhs.rep[i];
        return *this;
    }

    inline Nat& Nat::operator^=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(std::max(rep.size(), rhs.rep.size()));
        for (size_t i = 0; i < common; ++i)
            rep[i] ^= rhs.rep[i];
        trim();
        return *this;
    }

    inline Nat& Nat::operator<<=(ptrdiff_t rhs) {
        if (rhs == 0)
            return *this;
        if (rhs < 0)
            return *this >>= - rhs;
        size_t words = rhs / 32;
        int bits = rhs % 32;
        uint32_t prev = 0;
        if (bits > 0) {
            for (auto& word: rep) {
                uint32_t next = word >> (32 - bits);
                word = (word << bits) | prev;
                prev = next;
            }
        }
        if (prev)
            rep.push_back(prev);
        rep.insert(rep.begin(), words, 0);
        return *this;
    }

    inline Nat& Nat::operator>>=(ptrdiff_t rhs) {
        if (rhs == 0)
            return *this;
        if (rhs < 0)
            return *this <<= - rhs;
        size_t words = rhs / 32;
        int bits = rhs % 32;
        if (words >= rep.size()) {
            rep.clear();
        } else {
            rep.erase(rep.begin(), rep.begin() + words);
            if (bits > 0) {
                uint32_t prev = 0;
                for (size_t i = rep.size() - 1; i != npos; --i) {
                    uint32_t next = rep[i] << (32 - bits);
                    rep[i] = (rep[i] >> bits) | prev;
                    prev = next;
                }
            }
        }
        trim();
        return *this;
    }

    inline size_t Nat::bits() const noexcept {
        size_t n = 32 * rep.size();
        if (! rep.empty())
            n -= clz32(rep.back());
        return n;
    }

    inline size_t Nat::bits_set() const noexcept {
        size_t n = 0;
        for (auto i: rep)
            n += popcount32(i);
        return n;
    }

    inline size_t Nat::bytes() const noexcept {
        if (rep.empty())
            return 0;
        else
            return 4 * (rep.size() - 1) + size_t(rep.back() > 0) + size_t(rep.back() > size_t(0xff))
                + size_t(rep.back() > size_t(0xffff)) + size_t(rep.back() > size_t(0xffffff));
    }

    inline int Nat::compare(const Nat& rhs) const noexcept {
        if (rep.size() != rhs.rep.size())
            return rep.size() < rhs.rep.size() ? -1 : 1;
        for (size_t i = rep.size() - 1; i != npos; --i)
            if (rep[i] != rhs.rep[i])
                return rep[i] < rhs.rep[i] ? -1 : 1;
        return 0;
    }

    inline bool Nat::get_bit(size_t i) const noexcept {
        if (i < 32 * rep.size())
            return (rep[i / 32] >> (i % 32)) & 1;
        else
            return false;
    }

    inline uint8_t Nat::get_byte(size_t i) const noexcept {
        if (i < 4 * rep.size())
            return (rep[i / 4] >> (i % 4 * 8)) & 0xff;
        else
            return 0;
    }

    inline void Nat::set_bit(size_t i, bool b) {
        bool in_rep = i < 32 * rep.size();
        if (b) {
            if (! in_rep)
                rep.resize(i / 32 + 1, 0);
            rep[i / 32] |= uint32_t(1) << (i % 32);
        } else if (in_rep) {
            rep[i / 32] &= ~ (uint32_t(1) << (i % 32));
            trim();
        }
    }

    inline void Nat::set_byte(size_t i, uint8_t b) {
        if (i >= 4 * rep.size())
            rep.resize(i / 4 + 1, 0);
        rep[i / 4] |= uint32_t(b) << (i % 4 * 8);
        trim();
    }

    inline void Nat::flip_bit(size_t i) {
        if (i >= 32 * rep.size())
            rep.resize(i / 32 + 1, 0);
        rep[i / 32] ^= uint32_t(1) << (i % 32);
        trim();
    }

    inline Nat Nat::pow(const Nat& n) const {
        Nat x = *this, y = n, z = 1;
        while (y) {
            if (y.is_odd())
                z *= x;
            x *= x;
            y >>= 1;
        }
        return z;
    }

    inline U8string Nat::str(int base, size_t digits) const {
        if (base < 2 || base > 36)
            throw std::invalid_argument("Invalid base: " + RS::dec(base));
        if (rep.empty())
            return U8string(digits, '0');
        U8string s;
        if (base == 2) {
            s = RS::bin(rep.back(), 1);
            for (size_t i = rep.size() - 2; i != npos; --i)
                s += RS::bin(rep[i], 32);
        } else if (base == 16) {
            s = RS::hex(rep.back(), 1);
            for (size_t i = rep.size() - 2; i != npos; --i)
                s += RS::hex(rep[i], 8);
        } else if (base <= 10) {
            Nat b = base, q, r, t = *this;
            while (t) {
                do_divide(t, b, q, r);
                s += char(int(r) + '0');
                t = std::move(q);
            }
            std::reverse(s.begin(), s.end());
        } else {
            Nat b = base, q, r, t = *this;
            while (t) {
                do_divide(t, b, q, r);
                int d = int(r);
                if (d < 10)
                    s += char(d + '0');
                else
                    s += char(d - 10 + 'a');
                t = std::move(q);
            }
            std::reverse(s.begin(), s.end());
        }
        if (s.size() < digits)
            s.insert(0, digits - s.size(), '0');
        return s;
    }

    inline void Nat::write_be(void* ptr, size_t n) const noexcept {
        size_t nb = std::min(n, bytes());
        memset(ptr, 0, n - nb);
        auto bp = static_cast<uint8_t*>(ptr) + n - nb, end = bp + nb;
        while (bp != end)
            *bp++ = get_byte(--nb);
    }

    inline void Nat::write_le(void* ptr, size_t n) const noexcept {
        auto bp = static_cast<uint8_t*>(ptr);
        size_t nb = std::min(n, bytes());
        for (size_t i = 0; i < nb; ++i)
            bp[i] = get_byte(i);
        memset(bp + nb, 0, n - nb);
    }

    inline Nat Nat::from_double(double x) {
        int e = 0;
        double m = frexp(fabs(x), &e);
        Nat n = uint64_t(floor(ldexp(m, 64)));
        n <<= e - 64;
        return n;
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

    inline Nat Nat::read_be(const void* ptr, size_t n) {
        Nat result;
        result.rep.resize((n + 3) / 4);
        auto bp = static_cast<const uint8_t*>(ptr);
        for (size_t i = 0, j = n - 1; i < n; ++i, --j)
            result.set_byte(j, bp[i]);
        result.trim();
        return result;
    }

    inline Nat Nat::read_le(const void* ptr, size_t n) {
        Nat result;
        result.rep.resize((n + 3) / 4);
        auto bp = static_cast<const uint8_t*>(ptr);
        for (size_t i = 0; i < n; ++i)
            result.set_byte(i, bp[i]);
        result.trim();
        return result;
    }

    inline void Nat::do_divide(const Nat& x, const Nat& y, Nat& q, Nat& r) {
        if (! y)
            throw std::domain_error("Division by zero");
        Nat quo, rem = x;
        if (x >= y) {
            size_t shift = x.bits() - y.bits();
            Nat rsub = y;
            rsub <<= shift;
            if (rsub > x) {
                --shift;
                rsub >>= 1;
            }
            Nat qadd = 1;
            qadd <<= shift;
            while (qadd) {
                if (rem >= rsub) {
                    rem -= rsub;
                    quo += qadd;
                }
                rsub >>= 1;
                qadd >>= 1;
            }
        }
        q = std::move(quo);
        r = std::move(rem);
    }

    inline void Nat::do_multiply(const Nat& x, const Nat& y, Nat& z) {
        if (! x || ! y) {
            z.rep.clear();
        } else {
            size_t m = x.rep.size(), n = y.rep.size();
            z.rep.assign(m + n, 0);
            uint64_t sum = 0;
            for (size_t k = 0; k < m + n; ++k) {
                for (size_t i = std::min(k, m - 1), j = k - i; i != npos && j < n; --i, ++j)
                    sum += uint64_t(x.rep[i]) * uint64_t(y.rep[j]);
                z.rep[k] = uint32_t(sum & mask32);
                sum >>= 32;
            }
            z.trim();
        }
    }

    inline void Nat::init(const char* s, int base) {
        if (base != 0 && base != 2 && base != 10 && base != 16)
            throw std::invalid_argument("Invalid base: " + RS::dec(base));
        if (! (s && *s))
            return;
        if (base == 0) {
            if (s[0] != '0')
                base = 10;
            else if (s[1] == 'B' || s[1] == 'b')
                base = 2;
            else if (s[1] == 'X' || s[1] == 'x')
                base = 16;
            else
                base = 10;
            if (base != 10)
                s += 2;
        }
        Nat nbase = base;
        int digit = 0;
        auto digit_f = base == 2 ? digit_2 : base == 16 ? digit_16 : digit_10;
        for (; *s; ++s) {
            if (*s == '\'')
                continue;
            digit = digit_f(*s);
            if (digit == -1)
                break;
            *this *= nbase;
            *this += digit;
        }
    }

    inline void Nat::trim() noexcept {
        size_t i = rep.size() - 1;
        while (i != npos && rep[i] == 0)
            --i;
        rep.resize(i + 1);
    }

    inline std::ostream& operator<<(std::ostream& out, const Nat& x) { return out << x.str(); }
    inline Nat abs(const Nat& x) { return x; }
    inline int sign_of(const Nat& x) noexcept { return x.sign(); }
    inline Nat quo(const Nat& lhs, const Nat& rhs) { return lhs / rhs; }
    inline Nat rem(const Nat& lhs, const Nat& rhs) { return lhs % rhs; }
    inline U8string bin(const Nat& x, size_t digits = 1) { return x.str(2, digits); }
    inline U8string dec(const Nat& x, size_t digits = 1) { return x.str(10, digits); }
    inline U8string hex(const Nat& x, size_t digits = 1) { return x.str(16, digits); }

    // Signed integers

    class Int:
    public LessThanComparable<Int> {
    public:
        Int() = default;
        Int(int64_t x): mag(uint64_t(std::abs(x))), neg(x < 0) {}
        Int(const Nat& x): mag(x), neg(false) {}
        explicit Int(const U8string& s, int base = 0) { init(s.data(), base); }
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
        U8string str(int base = 10, size_t digits = 1, bool sign = false) const;
        static Int from_double(double x);
        template <typename RNG> static Int random(RNG& rng, const Int& n) { return Int(Nat::random(rng, n.mag)); }
        friend Int operator*(const Int& lhs, const Int& rhs) { Int z; Int::do_multiply(lhs, rhs, z); return z; }
        friend Int operator/(const Int& lhs, const Int& rhs) { Int q, r; Int::do_divide(lhs, rhs, q, r); return q; }
        friend Int operator%(const Int& lhs, const Int& rhs) { Int q, r; Int::do_divide(lhs, rhs, q, r); return r; }
        friend std::pair<Int, Int> divide(const Int& lhs, const Int& rhs) { Int q, r; Int::do_divide(lhs, rhs, q, r); return {q, r}; }
    private:
        Nat mag;
        bool neg = false;
        void init(const char* s, int base);
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

    inline Int& Int::operator+=(const Int& rhs) {
        if (! rhs.mag) {
            // do nothing
        } else if (! mag) {
            mag = rhs.mag;
            neg = rhs.neg;
        } else if (neg == rhs.neg) {
            mag += rhs.mag;
        } else {
            switch (mag.compare(rhs.mag)) {
                case -1:
                    mag = rhs.mag - mag;
                    neg = ! neg;
                    break;
                case 1:
                    mag -= rhs.mag;
                    break;
                default:
                    mag = {};
                    neg = false;
                    break;
            }
        }
        return *this;
    }

    inline int Int::compare(const Int& rhs) const noexcept {
        if (neg != rhs.neg)
            return neg ? -1 : 1;
        int c = mag.compare(rhs.mag);
        return neg ? - c : c;
    }

    inline Int Int::pow(const Int& n) const {
        if (n.neg)
            throw std::domain_error("Negative exponent in integer expression");
        Int z;
        z.mag = mag.pow(n.mag);
        z.neg = neg && n.mag.is_odd();
        return z;
    }

    inline U8string Int::str(int base, size_t digits, bool sign) const {
        U8string s = mag.str(base, digits);
        if (neg)
            s.insert(s.begin(), '-');
        else if (sign)
            s.insert(s.begin(), '+');
        return s;
    }

    inline Int Int::from_double(double x) {
        Int i = Nat::from_double(x);
        if (x < 0)
            i = - i;
        return i;
    }

    inline void Int::init(const char* s, int base) {
        if (base != 0 && base != 2 && base != 10 && base != 16)
            throw std::invalid_argument("Invalid base: " + RS::dec(base));
        if (s == nullptr || *s == '\0')
            return;
        neg = *s == '-';
        if (*s == '+' || *s == '-')
            ++s;
        mag.init(s, base);
        neg &= bool(mag);
    }

    inline void Int::do_divide(const Int& x, const Int& y, Int& q, Int& r) {
        Int quo, rem;
        Nat::do_divide(x.mag, y.mag, quo.mag, rem.mag);
        if (rem.mag && (x.neg || y.neg)) {
            if (x.neg) {
                ++quo.mag;
                rem.mag = y.mag - rem.mag;
            }
        }
        quo.neg = bool(quo.mag) && x.neg != y.neg;
        q = std::move(quo);
        r = std::move(rem);
    }

    inline void Int::do_multiply(const Int& x, const Int& y, Int& z) {
        Nat::do_multiply(x.mag, y.mag, z.mag);
        z.neg = bool(x) && bool(y) && x.neg != y.neg;
    }

    inline std::ostream& operator<<(std::ostream& out, const Int& x) { return out << x.str(); }
    inline Int abs(const Int& x) { return x.abs(); }
    inline int sign_of(const Int& x) noexcept{ return x.sign(); }
    inline Int quo(const Int& lhs, const Int& rhs) { return lhs / rhs; }
    inline Int rem(const Int& lhs, const Int& rhs) { return lhs % rhs; }
    inline U8string bin(const Int& x, size_t digits = 1) { return x.str(2, digits); }
    inline U8string dec(const Int& x, size_t digits = 1) { return x.str(10, digits); }
    inline U8string hex(const Int& x, size_t digits = 1) { return x.str(16, digits); }

    // Related types

    namespace RS_Detail {

        template <> struct IntegerParser<Nat> { Nat operator()(const U8string& s) const noexcept { return Nat(s); } };
        template <> struct IntegerParser<Int> { Int operator()(const U8string& s) const noexcept { return Int(s); } };

    }

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
