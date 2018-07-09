#pragma once

#include "rs-core/common.hpp"
#include <ostream>
#include <utility>

namespace RS {

    class Uint128 {
    public:
        constexpr Uint128() noexcept: lo(0), hi(0) {}
        constexpr Uint128(uint64_t x) noexcept: lo(x), hi(0) {}
        constexpr Uint128(uint64_t high, uint64_t low) noexcept: lo(low), hi(high) {}
        explicit Uint128(Uview str, int base = 10);
        size_t hash() const noexcept;
        Ustring str(int base = 10, int digits = 1) const;
        constexpr explicit operator bool() const noexcept { return lo || hi; }
        template <typename T> constexpr explicit operator T() const noexcept { return static_cast<T>(lo); }
        constexpr Uint128 operator~() const noexcept { return {~ hi, ~ lo}; }
        constexpr Uint128& operator++() noexcept { if (! ++lo) ++hi; return *this; }
        constexpr Uint128 operator++(int) noexcept { auto x = *this; ++*this; return x; }
        constexpr Uint128& operator--() noexcept { if (! lo--) --hi; return *this; }
        constexpr Uint128 operator--(int) noexcept { auto x = *this; --*this; return x; }
        constexpr Uint128& operator+=(Uint128 y) noexcept;
        constexpr Uint128& operator-=(Uint128 y) noexcept;
        constexpr Uint128& operator*=(Uint128 y) noexcept;
        constexpr Uint128& operator/=(Uint128 y) noexcept { return *this = divide(*this, y).first; }
        constexpr Uint128& operator%=(Uint128 y) noexcept { return *this = divide(*this, y).second; }
        constexpr Uint128& operator&=(Uint128 y) noexcept { hi &= y.hi; lo &= y.lo; return *this; }
        constexpr Uint128& operator|=(Uint128 y) noexcept { hi |= y.hi; lo |= y.lo; return *this; }
        constexpr Uint128& operator^=(Uint128 y) noexcept { hi ^= y.hi; lo ^= y.lo; return *this; }
        constexpr Uint128& operator<<=(int y) noexcept;
        constexpr Uint128& operator>>=(int y) noexcept;
        friend constexpr Uint128 operator+(Uint128 x, Uint128 y) noexcept { auto z = x; z += y; return z; }
        friend constexpr Uint128 operator-(Uint128 x, Uint128 y) noexcept { auto z = x; z -= y; return z; }
        friend constexpr Uint128 operator*(Uint128 x, Uint128 y) noexcept { auto z = x; z *= y; return z; }
        friend constexpr Uint128 operator/(Uint128 x, Uint128 y) noexcept { auto z = x; z /= y; return z; }
        friend constexpr Uint128 operator%(Uint128 x, Uint128 y) noexcept { auto z = x; z %= y; return z; }
        friend constexpr Uint128 operator&(Uint128 x, Uint128 y) noexcept { auto z = x; z &= y; return z; }
        friend constexpr Uint128 operator|(Uint128 x, Uint128 y) noexcept { auto z = x; z |= y; return z; }
        friend constexpr Uint128 operator^(Uint128 x, Uint128 y) noexcept { auto z = x; z ^= y; return z; }
        friend constexpr Uint128 operator<<(Uint128 x, int y) noexcept { auto z = x; z <<= y; return z; }
        friend constexpr Uint128 operator>>(Uint128 x, int y) noexcept { auto z = x; z >>= y; return z; }
        friend constexpr bool operator==(Uint128 x, Uint128 y) noexcept { return x.lo == y.lo && x.hi == y.hi; }
        friend constexpr bool operator!=(Uint128 x, Uint128 y) noexcept { return ! (x == y); }
        friend constexpr bool operator<(Uint128 x, Uint128 y) noexcept { return x.hi == y.hi ? x.lo < y.lo : x.hi < y.hi; }
        friend constexpr bool operator>(Uint128 x, Uint128 y) noexcept { return y < x; }
        friend constexpr bool operator<=(Uint128 x, Uint128 y) noexcept { return ! (y < x); }
        friend constexpr bool operator>=(Uint128 x, Uint128 y) noexcept { return ! (x < y); }
        friend constexpr std::pair<Uint128, Uint128> divide(Uint128 x, Uint128 y) noexcept;
        friend constexpr int ilog2p1(Uint128 x) noexcept { return x.hi ? ilog2p1(x.hi) + 64 : ilog2p1(x.lo); }
        friend constexpr Uint128 rotl(Uint128 x, int y) noexcept { return (x << y) | (x >> (128 - y)); }
        friend constexpr Uint128 rotr(Uint128 x, int y) noexcept { return (x >> y) | (x << (128 - y)); }
        friend Ustring bin(Uint128 x, int digits = 128) { return x.str(2, digits); }
        friend Ustring dec(Uint128 x, int digits = 1) { return x.str(10, digits); }
        friend Ustring hex(Uint128 x, int digits = 32) { return x.str(16, digits); }
        friend Ustring to_str(Uint128 x) { return x.str(); }
        friend std::ostream& operator<<(std::ostream& out, Uint128 x) noexcept { return out << x.str(); }
    private:
        uint64_t lo, hi;
    };

        constexpr Uint128& Uint128::operator+=(Uint128 y) noexcept {
            lo += y.lo;
            hi += y.hi;
            if (lo < y.lo)
                ++hi;
            return *this;
        }

        constexpr Uint128& Uint128::operator-=(Uint128 y) noexcept {
            if (lo < y.lo)
                --hi;
            lo -= y.lo;
            hi -= y.hi;
            return *this;
        }

        constexpr Uint128& Uint128::operator*=(Uint128 y) noexcept {
            constexpr uint64_t mask32 = ~ uint32_t(0);
            uint64_t a = hi >> 32, b = hi & mask32, c = lo >> 32, d = lo & mask32;
            uint64_t e = y.hi >> 32, f = y.hi & mask32, g = y.lo >> 32, h = y.lo & mask32;
            Uint128 ah(0, a * h), bg(0, b * g), cf(0, c * f), de(0, d * e);
            *this = (ah + bg + cf + de) << 96;
            Uint128 bh(0, b * h), cg(0, c * g), df(0, d * f);
            *this += (bh + cg + df) << 64;
            Uint128 ch(0, c * h), dg(0, d * g);
            *this += (ch + dg) << 32;
            Uint128 dh(0, d * h);
            return *this += dh;
        }

        constexpr Uint128& Uint128::operator<<=(int y) noexcept {
            if (y < 0) {
                *this >>= - y;
            } else if (y == 0) {
                // pass
            } else if (y < 64) {
                hi = (hi << y) | (lo >> (64 - y));
                lo <<= y;
            } else if (y < 128) {
                hi = lo << (y - 64);
                lo = 0;
            } else {
                hi = lo = 0;
            }
            return *this;
        }

        constexpr Uint128& Uint128::operator>>=(int y) noexcept {
            if (y < 0) {
                *this <<= - y;
            } else if (y == 0) {
                // pass
            } else if (y < 64) {
                lo = (lo >> y) | (hi << (64 - y));
                hi >>= y;
            } else if (y < 128) {
                lo = hi >> (y - 64);
                hi = 0;
            } else {
                hi = lo = 0;
            }
            return *this;
        }

        constexpr std::pair<Uint128, Uint128> divide(Uint128 x, Uint128 y) noexcept {
            if (x < y)
                return {0, x};
            int shift = ilog2p1(x) - ilog2p1(y);
            Uint128 a = y << shift, b = Uint128(1) << shift, q, r = x;
            for (; shift >= 0 && r; --shift, a >>= 1, b >>= 1) {
                if (a <= r) {
                    r -= a;
                    q += b;
                }
            }
            return {q, r};
        }

    namespace Literals {

        inline Uint128 operator""_u128(const char* ptr, size_t len) noexcept { return Uint128(Uview(ptr, len)); }

    }

}

RS_DEFINE_STD_HASH(RS::Uint128);
