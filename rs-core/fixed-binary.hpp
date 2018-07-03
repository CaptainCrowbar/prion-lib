#pragma once

#include "rs-core/common.hpp"
#include "rs-core/random.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <limits>
#include <ostream>
#include <type_traits>

namespace RS {

    template <size_t N> class SmallBinary;
    template <size_t N> class LargeBinary;
    template <size_t N> using Binary = std::conditional_t<N <= 64, SmallBinary<N>, LargeBinary<N>>;

    namespace RS_Detail {

        template <typename T>
        size_t parse_binary(const Ustring& src, int base, T& dst) noexcept {
            dst.clear();
            T tbase = T(uint64_t(base));
            int digit = 0;
            char c = 0, max_uc = char('A' + base - 11), max_lc = char(max_uc + 32);
            size_t i = 0, n = src.size();
            for (; i < n; ++i) {
                c = src[i];
                if (c >= '0' && c <= '9')
                    digit = c - '0';
                else if (c >= 'A' && c <= max_uc)
                    digit = c - 'A' + 10;
                else if (c >= 'a' && c <= max_lc)
                    digit = c - 'a' + 10;
                else
                    break;
                dst = tbase * dst + T(uint64_t(digit));
            }
            return i;
        }

    }

    template <size_t N>
    class SmallBinary:
    public LessThanComparable<SmallBinary<N>> {
    private:
        static_assert(N >= 1 && N <= 64);
        using value_type = std::conditional_t<N <= 8, uint8_t, std::conditional_t<N <= 16, uint16_t, std::conditional_t<N <= 32, uint32_t, uint64_t>>>;
    public:
        static constexpr size_t bits = N;
        static constexpr size_t bytes = sizeof(value_type);
        static constexpr size_t hex_digits = (N + 3) / 4;
        constexpr SmallBinary() noexcept: value(0) {}
        constexpr SmallBinary(uint64_t x) noexcept: value(x & mask) {}
        template <size_t M> constexpr explicit SmallBinary(SmallBinary<M> x) noexcept: SmallBinary(static_cast<uint64_t>(x)) {}
        constexpr explicit SmallBinary(std::initializer_list<uint64_t> init) noexcept: value(init.size() ? *init.begin() & mask : 0) {}
        constexpr explicit SmallBinary(const Ustring& str, int base = 10) noexcept: value() { parse(str, base); }
        Ustring as_binary() const { return RS::bin(value, N); }
        Ustring as_decimal() const { return RS::dec(value); }
        Ustring as_hex() const { return RS::hex(value, hex_digits); }
        double as_double() const noexcept { return double(value); }
        constexpr void clear() noexcept { value = 0; }
        constexpr int compare(SmallBinary y) const noexcept { return value < y.value ? -1 : value > y.value ? 1 : 0; }
        constexpr auto data() noexcept { return reinterpret_cast<uint8_t*>(&value); }
        constexpr auto data() const noexcept { return reinterpret_cast<const uint8_t*>(&value); }
        template <typename T> bool fits_in() const noexcept { return significant_bits() <= std::numeric_limits<T>::digits; }
        size_t parse(const Ustring& str, int base = 10) noexcept { return RS_Detail::parse_binary(str, base, *this); }
        constexpr size_t significant_bits() const noexcept { return ilog2p1(value); }
        constexpr explicit operator bool() const noexcept { return value != 0; }
        template <typename T> constexpr explicit operator T() const noexcept { return static_cast<T>(value); }
        constexpr SmallBinary operator+() const noexcept { return *this; }
        constexpr SmallBinary operator-() const noexcept { auto x = ~ *this; ++x; return x; }
        constexpr SmallBinary operator~() const noexcept { return SmallBinary(~ value); }
        constexpr SmallBinary& operator++() noexcept { ++value; value &= mask; return *this; }
        constexpr SmallBinary operator++(int) noexcept { auto x = *this; ++*this; return x; }
        constexpr SmallBinary& operator--() noexcept { --value; value &= mask; return *this; }
        constexpr SmallBinary operator--(int) noexcept { auto x = *this; --*this; return x; }
        constexpr SmallBinary& operator+=(SmallBinary y) noexcept { value += y.value; value &= mask; return *this; }
        constexpr SmallBinary& operator-=(SmallBinary y) noexcept { value -= y.value; value &= mask; return *this; }
        constexpr SmallBinary& operator*=(SmallBinary y) noexcept { value *= y.value; value &= mask; return *this; }
        constexpr SmallBinary& operator/=(SmallBinary y) noexcept { value /= y.value; return *this; }
        constexpr SmallBinary& operator%=(SmallBinary y) noexcept { value %= y.value; return *this; }
        constexpr SmallBinary& operator&=(SmallBinary y) noexcept { value &= y.value; return *this; }
        constexpr SmallBinary& operator|=(SmallBinary y) noexcept { value |= y.value; return *this; }
        constexpr SmallBinary& operator^=(SmallBinary y) noexcept { value ^= y.value; return *this; }
        constexpr SmallBinary& operator<<=(int y) noexcept { if (y < 0) *this >>= - y; else if (size_t(y) < N) value = (value << y) & mask; else value = 0; return *this; }
        constexpr SmallBinary& operator>>=(int y) noexcept { if (y < 0) *this <<= - y; else if (size_t(y) < N) value >>= y; else value = 0; return *this; }
        static constexpr void divide(SmallBinary x, SmallBinary y, SmallBinary& q, SmallBinary& r) noexcept { q = x / y; r = x % y; }
        static SmallBinary from_double(double x) noexcept { return SmallBinary(uint64_t(x)); }
        static constexpr SmallBinary max() noexcept { return SmallBinary(mask); }
        template <typename RNG> static SmallBinary random(RNG& rng) { return SmallBinary(random_integer(rng, uint64_t(0), uint64_t(mask))); }
        template <typename RNG> static SmallBinary random(RNG& rng, SmallBinary x) { return SmallBinary(x.value > 1 ? random_integer(rng, uint64_t(0), uint64_t(x.value - 1)) : 0); }
        template <typename RNG> static SmallBinary random(RNG& rng, SmallBinary x, SmallBinary y) { return SmallBinary(random_integer(rng, uint64_t(x.value), uint64_t(y.value))); }
        friend constexpr SmallBinary rotl(SmallBinary x, int y) noexcept { y %= int(N); return (x << y) | (x >> (int(N) - y)); }
        friend constexpr SmallBinary rotr(SmallBinary x, int y) noexcept { y %= int(N); return (x >> y) | (x << (int(N) - y)); }
        friend constexpr SmallBinary operator+(SmallBinary x, SmallBinary y) noexcept { auto z = x; z += y; return z; }
        friend constexpr SmallBinary operator-(SmallBinary x, SmallBinary y) noexcept { auto z = x; z -= y; return z; }
        friend constexpr SmallBinary operator*(SmallBinary x, SmallBinary y) noexcept { auto z = x; z *= y; return z; }
        friend constexpr SmallBinary operator/(SmallBinary x, SmallBinary y) noexcept { auto z = x; z /= y; return z; }
        friend constexpr SmallBinary operator%(SmallBinary x, SmallBinary y) noexcept { auto z = x; z %= y; return z; }
        friend constexpr SmallBinary operator&(SmallBinary x, SmallBinary y) noexcept { auto z = x; z &= y; return z; }
        friend constexpr SmallBinary operator|(SmallBinary x, SmallBinary y) noexcept { auto z = x; z |= y; return z; }
        friend constexpr SmallBinary operator^(SmallBinary x, SmallBinary y) noexcept { auto z = x; z ^= y; return z; }
        friend constexpr SmallBinary operator<<(SmallBinary x, int y) noexcept { auto z = x; z <<= y; return z; }
        friend constexpr SmallBinary operator>>(SmallBinary x, int y) noexcept { auto z = x; z >>= y; return z; }
        friend constexpr bool operator==(SmallBinary x, SmallBinary y) noexcept { return x.value == y.value; }
        friend constexpr bool operator<(SmallBinary x, SmallBinary y) noexcept { return x.value < y.value; }
        friend std::ostream& operator<<(std::ostream& out, SmallBinary x) { return out << x.as_decimal(); }
        friend Ustring to_str(SmallBinary x) { return x.as_decimal(); }
    private:
        static constexpr value_type mask = (value_type(1) << N / 2 << (N + 1) / 2) - 1;
        value_type value;
    };

    template <size_t N>
    class LargeBinary:
    public LessThanComparable<LargeBinary<N>> {
    private:
        static_assert(N >= 1);
        using unit_type = uint32_t;
        static constexpr size_t unit_bytes = sizeof(unit_type);
        static constexpr size_t unit_bits = 8 * unit_bytes;
        static constexpr size_t units = (N + unit_bits - 1) / unit_bits;
    public:
        static constexpr size_t bits = N;
        static constexpr size_t bytes = units * unit_bytes;
        static constexpr size_t hex_digits = (N + 3) / 4;
        constexpr LargeBinary() noexcept: array() { clear(); }
        constexpr LargeBinary(uint64_t x) noexcept;
        constexpr explicit LargeBinary(std::initializer_list<uint64_t> init) noexcept;
        constexpr explicit LargeBinary(const Ustring& str) noexcept: array() { parse(str); }
        template <size_t M> constexpr explicit LargeBinary(SmallBinary<M> x) noexcept: LargeBinary(static_cast<uint64_t>(x)) {}
        template <size_t M> constexpr explicit LargeBinary(const LargeBinary<M>& x) noexcept;
        template <size_t M> constexpr explicit operator SmallBinary<M>() const noexcept { return SmallBinary<M>(static_cast<uint64_t>(*this)); }
        Ustring as_binary() const;
        Ustring as_decimal() const;
        Ustring as_hex() const;
        double as_double() const noexcept;
        constexpr void clear() noexcept { for (auto& u: array) u = 0; }
        constexpr int compare(const LargeBinary& y) const noexcept;
        constexpr auto data() noexcept { return reinterpret_cast<uint8_t*>(array); }
        constexpr auto data() const noexcept { return reinterpret_cast<const uint8_t*>(array); }
        template <typename T> bool fits_in() const noexcept { return significant_bits() <= std::numeric_limits<T>::digits; }
        size_t parse(const Ustring& str, int base = 10) noexcept { return RS_Detail::parse_binary(str, base, *this); }
        constexpr size_t significant_bits() const noexcept;
        constexpr explicit operator bool() const noexcept;
        template <typename T> constexpr explicit operator T() const noexcept;
        constexpr LargeBinary operator+() const noexcept { return *this; }
        constexpr LargeBinary operator-() const noexcept { auto x = ~ *this; ++x; return x; }
        constexpr LargeBinary operator~() const noexcept;
        constexpr LargeBinary& operator++() noexcept;
        constexpr LargeBinary operator++(int) noexcept { auto x = *this; ++*this; return x; }
        constexpr LargeBinary& operator--() noexcept;
        constexpr LargeBinary operator--(int) noexcept { auto x = *this; --*this; return x; }
        constexpr LargeBinary& operator+=(const LargeBinary& y) noexcept;
        constexpr LargeBinary& operator-=(const LargeBinary& y) noexcept;
        constexpr LargeBinary& operator*=(const LargeBinary& y) noexcept;
        constexpr LargeBinary& operator/=(const LargeBinary& y) noexcept { LargeBinary q, r; divide(*this, y, q, r); *this = q; return *this; }
        constexpr LargeBinary& operator%=(const LargeBinary& y) noexcept { LargeBinary q, r; divide(*this, y, q, r); *this = r; return *this; }
        constexpr LargeBinary& operator&=(const LargeBinary& y) noexcept;
        constexpr LargeBinary& operator|=(const LargeBinary& y) noexcept;
        constexpr LargeBinary& operator^=(const LargeBinary& y) noexcept;
        constexpr LargeBinary& operator<<=(int y) noexcept;
        constexpr LargeBinary& operator>>=(int y) noexcept;
        static constexpr void divide(const LargeBinary& x, const LargeBinary& y, LargeBinary& q, LargeBinary& r) noexcept;
        static LargeBinary from_double(double x) noexcept;
        static constexpr LargeBinary max() noexcept;
        template <typename RNG> static LargeBinary random(RNG& rng);
        template <typename RNG> static LargeBinary random(RNG& rng, const LargeBinary& x);
        template <typename RNG> static LargeBinary random(RNG& rng, const LargeBinary& x, const LargeBinary& y);
        friend constexpr LargeBinary rotl(const LargeBinary& x, int y) noexcept { y %= int(N); return (x << y) | (x >> (int(N) - y)); }
        friend constexpr LargeBinary rotr(const LargeBinary& x, int y) noexcept { y %= int(N); return (x >> y) | (x << (int(N) - y)); }
        friend constexpr LargeBinary operator+(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z += y; return z; }
        friend constexpr LargeBinary operator-(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z -= y; return z; }
        friend constexpr LargeBinary operator*(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z *= y; return z; }
        friend constexpr LargeBinary operator/(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z /= y; return z; }
        friend constexpr LargeBinary operator%(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z %= y; return z; }
        friend constexpr LargeBinary operator&(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z &= y; return z; }
        friend constexpr LargeBinary operator|(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z |= y; return z; }
        friend constexpr LargeBinary operator^(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z ^= y; return z; }
        friend constexpr LargeBinary operator<<(const LargeBinary& x, int y) noexcept { auto z = x; z <<= y; return z; }
        friend constexpr LargeBinary operator>>(const LargeBinary& x, int y) noexcept { auto z = x; z >>= y; return z; }
        friend constexpr bool operator==(const LargeBinary& x, const LargeBinary& y) noexcept { return x.compare(y) == 0; }
        friend constexpr bool operator<(const LargeBinary& x, const LargeBinary& y) noexcept { return x.compare(y) < 0; }
        friend std::ostream& operator<<(std::ostream& out, const LargeBinary& x) { return out << x.as_decimal(); }
        friend Ustring to_str(LargeBinary x) { return x.as_decimal(); }
    private:
        static constexpr size_t final_bits = N % unit_bits ? N % unit_bits : unit_bits;
        static constexpr size_t final_hex_digits = (final_bits + 3) / 4;
        static constexpr size_t excess_bits = unit_bits - final_bits;
        static constexpr unit_type unit_mask = ~ unit_type(0);
        static constexpr unit_type high_mask = unit_mask >> excess_bits;
        unit_type array[units]; // Little endian order
        constexpr void do_mask() noexcept { array[units - 1] &= high_mask; }
        static constexpr void add_with_carry(unit_type& x, unit_type y, unit_type& c) noexcept;
    };

        template <size_t N>
        constexpr LargeBinary<N>::LargeBinary(uint64_t x) noexcept:
        array() {
            array[0] = unit_type(x);
            if (units > 1)
                array[1] = unit_type(x >> unit_bits);
            for (size_t i = 2; i < units; ++i)
                array[i] = 0;
            do_mask();
        }

        template <size_t N>
        constexpr LargeBinary<N>::LargeBinary(std::initializer_list<uint64_t> init) noexcept:
        array() {
            clear();
            auto ptr = init.begin();
            size_t len = init.size();
            for (size_t i = len - 1, j = 0; i != npos && j < units; --i, j += 2) {
                array[j] = unit_type(ptr[i]);
                if (j + 1 < units)
                    array[j + 1] = unit_type(ptr[i] >> unit_bits);
            }
            do_mask();
        }

        template <size_t N>
        template <size_t M>
        constexpr LargeBinary<N>::LargeBinary(const LargeBinary<M>& x) noexcept:
        array() {
            constexpr size_t common_units = (std::min(M, N) + unit_bits - 1) / unit_bits;
            auto x_array = reinterpret_cast<const unit_type*>(x.data());
            size_t i = 0;
            for (; i < common_units; ++i)
                array[i] = x_array[i];
            for (; i < units; ++i)
                array[i] = 0;
            do_mask();
        }

        template <size_t N>
        Ustring LargeBinary<N>::as_binary() const {
            Ustring s;
            s.reserve(N);
            s = RS::bin(array[units - 1], final_bits);
            for (size_t i = units - 2; i != npos; --i)
                s += RS::bin(array[i]);
            return s;
        }

        template <size_t N>
        Ustring LargeBinary<N>::as_decimal() const {
            Ustring s;
            LargeBinary x(*this), base(10), q, r;
            do {
                divide(x, base, q, r);
                s += char(int(r.array[0]) + '0');
                x = q;
            } while(x);
            std::reverse(s.begin(), s.end());
            return s;
        }

        template <size_t N>
        Ustring LargeBinary<N>::as_hex() const {
            Ustring s;
            s.reserve(hex_digits);
            s = RS::hex(array[units - 1], final_hex_digits);
            for (size_t i = units - 2; i != npos; --i)
                s += RS::hex(array[i]);
            return s;
        }

        template <size_t N>
        double LargeBinary<N>::as_double() const noexcept {
            size_t i = units - 1;
            while (i != npos && array[i] == 0)
                --i;
            if (i == npos)
                return 0;
            double x = std::ldexp(double(array[i]), int(unit_bits * i));
            if (i > 0)
                x += std::ldexp(double(array[i - 1]), int(unit_bits * (i - 1)));
            return x;
        }

        template <size_t N>
        constexpr int LargeBinary<N>::compare(const LargeBinary& y) const noexcept {
            for (size_t i = units - 1; i != npos; --i)
                if (array[i] != y.array[i])
                    return array[i] < y.array[i] ? -1 : 1;
            return 0;
        }

        template <size_t N>
        constexpr size_t LargeBinary<N>::significant_bits() const noexcept {
            size_t i = units - 1;
            while (i != npos && array[i] == 0)
                --i;
            return i == npos ? 0 : unit_bits * i + ilog2p1(array[i]);
        }

        template <size_t N>
        constexpr LargeBinary<N>::operator bool() const noexcept {
            for (auto u: array)
                if (u)
                    return true;
            return false;
        }

        template <size_t N>
        template <typename T>
        constexpr LargeBinary<N>::operator T() const noexcept {
            constexpr size_t t_bits = std::numeric_limits<T>::digits;
            constexpr size_t t_units = (t_bits + unit_bits - 1) / unit_bits;
            constexpr size_t n_units = std::min(t_units, units);
            if (n_units < 2)
                return static_cast<T>(array[0]);
            uintmax_t u = 0;
            for (int i = int(n_units) - 1; i >= 0; --i)
                u = (u << unit_bits) + array[i];
            return static_cast<T>(u);
        }

        template <size_t N>
        constexpr LargeBinary<N> LargeBinary<N>::operator~() const noexcept {
            auto y = *this;
            for (auto& u: y.array)
                u = ~ u;
            y.do_mask();
            return y;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator++() noexcept {
            ++array[0];
            for (size_t i = 1; i < units && array[i - 1] == 0; ++i)
                ++array[i];
            do_mask();
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator--() noexcept {
            --array[0];
            for (size_t i = 1; i < units && array[i - 1] == unit_mask; ++i)
                --array[i];
            do_mask();
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator+=(const LargeBinary& y) noexcept {
            unit_type carry = 0;
            for (size_t i = 0; i < units; ++i)
                add_with_carry(array[i], y.array[i], carry);
            do_mask();
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator-=(const LargeBinary& y) noexcept {
            unit_type borrow = 0, next = 0;
            for (size_t i = 0; i < units; ++i) {
                next = unit_type(array[i] < y.array[i] || (array[i] == y.array[i] && borrow));
                array[i] -= y.array[i] + borrow;
                borrow = next;
            }
            do_mask();
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator*=(const LargeBinary& y) noexcept {
            using long_type = uint64_t;
            LargeBinary z;
            for (size_t i = 0; i < units; ++i) {
                for (size_t j = 0; j < units - i; ++j) {
                    size_t k = i + j;
                    long_type p = long_type(array[i]) * long_type(y.array[j]);
                    unit_type carry = 0;
                    add_with_carry(z.array[k], unit_type(p), carry);
                    if (++k < units) {
                        add_with_carry(z.array[k], unit_type(p >> unit_bits), carry);
                        for (++k; k < units && carry; ++k)
                            add_with_carry(z.array[k], 0, carry);
                    }
                }
            }
            *this = z;
            do_mask();
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator&=(const LargeBinary& y) noexcept {
            for (size_t i = 0; i < units; ++i)
                array[i] &= y.array[i];
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator|=(const LargeBinary& y) noexcept {
            for (size_t i = 0; i < units; ++i)
                array[i] |= y.array[i];
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator^=(const LargeBinary& y) noexcept {
            for (size_t i = 0; i < units; ++i)
                array[i] ^= y.array[i];
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator<<=(int y) noexcept {
            if (y < 0) {
                *this >>= - y;
            } else if (size_t(y) >= N) {
                clear();
            } else {
                size_t skip = size_t(y / unit_bits), keep = units - skip;
                if (skip) {
                    for (size_t i = keep - 1; i != npos; --i)
                        array[i + skip] = array[i];
                    for (size_t i = 0; i < skip; ++i)
                        array[i] = 0;
                    y %= unit_bits;
                }
                unit_type carry = 0, next = 0;
                if (y) {
                    for (size_t i = skip; i < units; ++i) {
                        next = array[i] >> (unit_bits - y);
                        array[i] <<= y;
                        array[i] += carry;
                        carry = next;
                    }
                }
                do_mask();
            }
            return *this;
        }

        template <size_t N>
        constexpr LargeBinary<N>& LargeBinary<N>::operator>>=(int y) noexcept {
            if (y < 0) {
                *this <<= - y;
            } else if (size_t(y) >= N) {
                clear();
            } else {
                size_t skip = size_t(y / unit_bits), keep = units - skip;
                if (skip) {
                    for (size_t i = 0; i < keep; ++i)
                        array[i] = array[i + skip];
                    for (size_t i = keep; i < units; ++i)
                        array[i] = 0;
                    y %= unit_bits;
                }
                unit_type carry = 0, next = 0;
                if (y) {
                    for (size_t i = keep - 1; i != npos; --i) {
                        next = array[i] << (unit_bits - y);
                        array[i] >>= y;
                        array[i] += carry;
                        carry = next;
                    }
                }
            }
            return *this;
        }

        template <size_t N>
        constexpr void LargeBinary<N>::divide(const LargeBinary& x, const LargeBinary& y, LargeBinary& q, LargeBinary& r) noexcept {
            q.clear();
            r = x;
            if (x < y)
                return;
            int shift = int(x.significant_bits()) - int(y.significant_bits());
            auto a = y << shift, b = LargeBinary(1) << shift;
            for (int i = 0; i <= shift && r; ++i, a >>= 1, b >>= 1) {
                if (a <= r) {
                    r -= a;
                    q += b;
                }
            }
        }

        template <size_t N>
        LargeBinary<N> LargeBinary<N>::from_double(double x) noexcept {
            int exp = 0;
            double sig = std::frexp(x, &exp);
            int shift = std::min(int(N), 63);
            sig = std::ldexp(sig, shift);
            exp -= shift;
            auto num = LargeBinary(uint64_t(sig));
            num <<= exp;
            return num;
        }

        template <size_t N>
        constexpr LargeBinary<N> LargeBinary<N>::max() noexcept {
            LargeBinary x;
            for (auto& u: x.array)
                u = unit_mask;
            x.do_mask();
            return x;
        }

        template <size_t N>
        template <typename RNG>
        LargeBinary<N> LargeBinary<N>::random(RNG& rng) {
            static constexpr size_t req_bytes = (N + 7) / 8;
            LargeBinary x;
            random_bytes(rng, x.array, req_bytes);
            x.do_mask();
            return x;
        }

        template <size_t N>
        template <typename RNG>
        LargeBinary<N> LargeBinary<N>::random(RNG& rng, const LargeBinary& x) {
            size_t req_bits = x.significant_bits();
            if (req_bits < 2)
                return {};
            size_t req_bytes = (req_bits + 7) / 8;
            LargeBinary y;
            do {
                random_bytes(rng, y.array, req_bytes);
                y.do_mask();
            } while (y >= x);
            return y;
        }

        template <size_t N>
        template <typename RNG>
        LargeBinary<N> LargeBinary<N>::random(RNG& rng, const LargeBinary& x, const LargeBinary& y) {
            int cmp = x.compare(y);
            if (cmp == 0)
                return {};
            auto p = &x, q = &y;
            if (cmp > 0)
                std::swap(p, q);
            if (! *p && *q == LargeBinary::max())
                return random(rng);
            LargeBinary range = *q - *p + LargeBinary(1);
            return *p + random(rng, range);
        }

        template <size_t N>
        constexpr void LargeBinary<N>::add_with_carry(unit_type& x, unit_type y, unit_type& c) noexcept {
            x += y + c;
            c = unit_type(x < y || (x == y && c));
        }

}
