#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <cstring>
#include <limits>
#include <ostream>
#include <utility>

namespace RS {

    class Uint128 {
    public:
        constexpr Uint128() noexcept: first_(0), second_(0) {}
        constexpr Uint128(uint64_t x) noexcept: first_(0), second_(0) { low() = x; }
        constexpr Uint128(uint64_t h, uint64_t l) noexcept: first_(0), second_(0) { high() = h; low() = l; }
        explicit Uint128(Uview str, int base = 10);
        uint8_t* data() noexcept { return reinterpret_cast<uint8_t*>(&first_); }
        const uint8_t* data() const noexcept { return reinterpret_cast<const uint8_t*>(&first_); }
        constexpr uint64_t& high() noexcept { if constexpr (little_endian_target) return second_; else return first_; }
        constexpr uint64_t high() const noexcept { if constexpr (little_endian_target) return second_; else return first_; }
        constexpr uint64_t& low() noexcept { if constexpr (little_endian_target) return first_; else return second_; }
        constexpr uint64_t low() const noexcept { if constexpr (little_endian_target) return first_; else return second_; }
        size_t hash() const noexcept;
        Ustring str(int base = 10, int digits = 1) const;
        constexpr explicit operator bool() const noexcept { return first_ || second_; }
        template <typename T> constexpr explicit operator T() const noexcept { return static_cast<T>(low()); }
        constexpr Uint128 operator~() const noexcept { return {~ high(), ~ low()}; }
        constexpr Uint128& operator++() noexcept { if (! ++low()) ++high(); return *this; }
        constexpr Uint128 operator++(int) noexcept { auto x = *this; ++*this; return x; }
        constexpr Uint128& operator--() noexcept { if (! low()--) --high(); return *this; }
        constexpr Uint128 operator--(int) noexcept { auto x = *this; --*this; return x; }
        constexpr Uint128& operator+=(Uint128 y) noexcept;
        constexpr Uint128& operator-=(Uint128 y) noexcept;
        constexpr Uint128& operator*=(Uint128 y) noexcept;
        constexpr Uint128& operator/=(Uint128 y) noexcept { return *this = divide(*this, y).first; }
        constexpr Uint128& operator%=(Uint128 y) noexcept { return *this = divide(*this, y).second; }
        constexpr Uint128& operator&=(Uint128 y) noexcept { high() &= y.high(); low() &= y.low(); return *this; }
        constexpr Uint128& operator|=(Uint128 y) noexcept { high() |= y.high(); low() |= y.low(); return *this; }
        constexpr Uint128& operator^=(Uint128 y) noexcept { high() ^= y.high(); low() ^= y.low(); return *this; }
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
        friend constexpr bool operator==(Uint128 x, Uint128 y) noexcept { return x.low() == y.low() && x.high() == y.high(); }
        friend constexpr bool operator!=(Uint128 x, Uint128 y) noexcept { return ! (x == y); }
        friend constexpr bool operator<(Uint128 x, Uint128 y) noexcept { return x.high() == y.high() ? x.low() < y.low() : x.high() < y.high(); }
        friend constexpr bool operator>(Uint128 x, Uint128 y) noexcept { return y < x; }
        friend constexpr bool operator<=(Uint128 x, Uint128 y) noexcept { return ! (y < x); }
        friend constexpr bool operator>=(Uint128 x, Uint128 y) noexcept { return ! (x < y); }
        friend constexpr std::pair<Uint128, Uint128> divide(Uint128 x, Uint128 y) noexcept;
        friend constexpr int ilog2p1(Uint128 x) noexcept { return x.high() ? ilog2p1(x.high()) + 64 : ilog2p1(x.low()); }
        friend constexpr int popcount(Uint128 x) noexcept { return popcount(x.first_) + popcount(x.second_); }
        friend constexpr Uint128 rotl(Uint128 x, int y) noexcept { return (x << y) | (x >> (128 - y)); }
        friend constexpr Uint128 rotr(Uint128 x, int y) noexcept { return (x >> y) | (x << (128 - y)); }
        friend void read_be(const void* src, Uint128& dst) noexcept;
        friend void read_le(const void* src, Uint128& dst) noexcept;
        friend void write_be(Uint128 src, void* dst) noexcept;
        friend void write_le(Uint128 src, void* dst) noexcept;
        friend Ustring bin(Uint128 x, int digits = 128) { return x.str(2, digits); }
        friend Ustring dec(Uint128 x, int digits = 1) { return x.str(10, digits); }
        friend Ustring hex(Uint128 x, int digits = 32) { return x.str(16, digits); }
        friend Ustring to_str(Uint128 x) { return x.str(); }
        friend std::ostream& operator<<(std::ostream& out, Uint128 x) noexcept { return out << x.str(); }
    private:
        uint64_t first_, second_;
    };

        constexpr Uint128& Uint128::operator+=(Uint128 y) noexcept {
            low() += y.low();
            high() += y.high();
            if (low() < y.low())
                ++high();
            return *this;
        }

        constexpr Uint128& Uint128::operator-=(Uint128 y) noexcept {
            if (low() < y.low())
                --high();
            low() -= y.low();
            high() -= y.high();
            return *this;
        }

        constexpr Uint128& Uint128::operator*=(Uint128 y) noexcept {
            constexpr uint64_t mask32 = ~ uint32_t(0);
            uint64_t a = high() >> 32, b = high() & mask32, c = low() >> 32, d = low() & mask32;
            uint64_t e = y.high() >> 32, f = y.high() & mask32, g = y.low() >> 32, h = y.low() & mask32;
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
                high() = (high() << y) | (low() >> (64 - y));
                low() <<= y;
            } else if (y < 128) {
                high() = low() << (y - 64);
                low() = 0;
            } else {
                high() = low() = 0;
            }
            return *this;
        }

        constexpr Uint128& Uint128::operator>>=(int y) noexcept {
            if (y < 0) {
                *this <<= - y;
            } else if (y == 0) {
                // pass
            } else if (y < 64) {
                low() = (low() >> y) | (high() << (64 - y));
                high() >>= y;
            } else if (y < 128) {
                low() = high() >> (y - 64);
                high() = 0;
            } else {
                high() = low() = 0;
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

        inline void read_be(const void* src, Uint128& dst) noexcept {
            std::memcpy(dst.data(), src, 16);
            if constexpr (little_endian_target)
                std::reverse(dst.data(), dst.data() + 16);
        }

        inline void read_le(const void* src, Uint128& dst) noexcept {
            std::memcpy(dst.data(), src, 16);
            if constexpr (big_endian_target)
                std::reverse(dst.data(), dst.data() + 16);
        }

        inline void write_be(Uint128 src, void* dst) noexcept {
            if constexpr (little_endian_target)
                std::reverse(src.data(), src.data() + 16);
            std::memcpy(dst, src.data(), 16);
        }

        inline void write_le(Uint128 src, void* dst) noexcept {
            if constexpr (big_endian_target)
                std::reverse(src.data(), src.data() + 16);
            std::memcpy(dst, src.data(), 16);
        }

    namespace RS_Detail {

        constexpr int u128_digit(char c) noexcept {
            return c >= '0' && c <= '9' ? int(c - '0') :
                c >= 'A' && c <= 'Z' ? int(c - 'A' + 10) :
                c >= 'a' && c <= 'z' ? int(c - 'a' + 10) : 999;
        }

        // MSVC gets confused doing 128 bit arithmetic
        #ifdef _MSC_VER
            #pragma warning(push)
            #pragma warning(disable: 4307) // integral constant overflow
        #endif

        template <int Base, char... Str> struct U128LiteralBase;
        template <int Base, char C, char... Str> struct U128LiteralBase<Base, C, Str...> {
            using next = U128LiteralBase<Base, Str...>;
            static constexpr int digit = u128_digit(C);
            static constexpr Uint128 scale = (digit < Base) ? Uint128(Base) * next::scale : next::scale;
            static constexpr Uint128 value = (digit < Base) ? Uint128(digit) * next::scale + next::value : next::value;
        };
        template <int Base> struct U128LiteralBase<Base> {
            static constexpr Uint128 scale = 1;
            static constexpr Uint128 value = 0;
        };

        template <char... Str> struct U128Literal: U128LiteralBase<10, Str...> {};
        template <char... Str> struct U128Literal<'0', 'b', Str...>: U128LiteralBase<2, Str...> {};
        template <char... Str> struct U128Literal<'0', 'B', Str...>: U128LiteralBase<2, Str...> {};
        template <char... Str> struct U128Literal<'0', 'x', Str...>: U128LiteralBase<16, Str...> {};
        template <char... Str> struct U128Literal<'0', 'X', Str...>: U128LiteralBase<16, Str...> {};

        #ifdef _MSC_VER
            #pragma warning(pop)
        #endif

    }

    namespace Literals {

        template <char... Str> constexpr Uint128 operator""_u128() noexcept {
            return RS_Detail::U128Literal<Str...>::value;
        }

    }

}

namespace std {

    template <>
    class numeric_limits<RS::Uint128>:
    public RS::NumericLimitsBase<RS::Uint128> {
    private:
        using type                               = RS::Uint128;
        using base                               = RS::NumericLimitsBase<type>;
    public:
        static constexpr bool is_bounded         = true;
        static constexpr bool is_exact           = true;
        static constexpr bool is_integer         = true;
        static constexpr bool is_modulo          = true;
        static constexpr bool is_signed          = false;
        static constexpr int digits              = 128;
        static constexpr int digits10            = 38;
        static constexpr int radix               = 2;
        static constexpr type lowest() noexcept  { return type(); }
        static constexpr type max() noexcept     { return ~ type(); }
        static constexpr type min() noexcept     { return type(); }
    };

}

RS_DEFINE_STD_HASH(RS::Uint128);
