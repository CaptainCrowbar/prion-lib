#pragma once

#include "rs-core/common.hpp"
#include "rs-core/random.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
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
        SmallBinary(uint64_t x) noexcept: value(x & mask) {}
        template <size_t M> explicit SmallBinary(SmallBinary<M> x) noexcept: SmallBinary(static_cast<uint64_t>(x)) {}
        explicit SmallBinary(std::initializer_list<uint64_t> init) noexcept: value(init.size() ? *init.begin() & mask : 0) {}
        explicit SmallBinary(const Ustring& str, int base = 10) noexcept: value() { parse(str, base); }
        Ustring as_binary() const { return RS::bin(value, N); }
        Ustring as_decimal() const { return RS::dec(value); }
        Ustring as_hex() const { return RS::hex(value, hex_digits); }
        double as_double() const noexcept { return double(value); }
        void clear() noexcept { value = 0; }
        int compare(SmallBinary y) const noexcept { return value < y.value ? -1 : value > y.value ? 1 : 0; }
        constexpr auto data() noexcept { return reinterpret_cast<uint8_t*>(&value); }
        constexpr auto data() const noexcept { return reinterpret_cast<const uint8_t*>(&value); }
        template <typename T> bool fits_in() const noexcept { return significant_bits() <= std::numeric_limits<T>::digits; }
        size_t hash() const noexcept { return std::hash<value_type>()(value); }
        size_t parse(const Ustring& str, int base = 10) noexcept { return RS_Detail::parse_binary(str, base, *this); }
        size_t significant_bits() const noexcept { return ilog2p1(value); }
        explicit operator bool() const noexcept { return value != 0; }
        template <typename T> explicit operator T() const noexcept { return static_cast<T>(value); }
        SmallBinary operator+() const noexcept { return *this; }
        SmallBinary operator-() const noexcept { auto x = ~ *this; ++x; return x; }
        SmallBinary operator~() const noexcept { return SmallBinary(~ uint64_t(value)); }
        SmallBinary& operator++() noexcept { ++value; value &= mask; return *this; }
        SmallBinary operator++(int) noexcept { auto x = *this; ++*this; return x; }
        SmallBinary& operator--() noexcept { --value; value &= mask; return *this; }
        SmallBinary operator--(int) noexcept { auto x = *this; --*this; return x; }
        SmallBinary& operator+=(SmallBinary y) noexcept { value += y.value; value &= mask; return *this; }
        SmallBinary& operator-=(SmallBinary y) noexcept { value -= y.value; value &= mask; return *this; }
        SmallBinary& operator*=(SmallBinary y) noexcept { value *= y.value; value &= mask; return *this; }
        SmallBinary& operator/=(SmallBinary y) noexcept { value /= y.value; return *this; }
        SmallBinary& operator%=(SmallBinary y) noexcept { value %= y.value; return *this; }
        SmallBinary& operator&=(SmallBinary y) noexcept { value &= y.value; return *this; }
        SmallBinary& operator|=(SmallBinary y) noexcept { value |= y.value; return *this; }
        SmallBinary& operator^=(SmallBinary y) noexcept { value ^= y.value; return *this; }
        SmallBinary& operator<<=(int y) noexcept { if (y < 0) *this >>= - y; else if (size_t(y) < N) value = (value << y) & mask; else value = 0; return *this; }
        SmallBinary& operator>>=(int y) noexcept { if (y < 0) *this <<= - y; else if (size_t(y) < N) value >>= y; else value = 0; return *this; }
        static void divide(SmallBinary x, SmallBinary y, SmallBinary& q, SmallBinary& r) noexcept { q = x / y; r = x % y; }
        static SmallBinary from_double(double x) noexcept { return SmallBinary(uint64_t(x)); }
        static SmallBinary max() noexcept { return SmallBinary(mask); }
        template <typename RNG> static SmallBinary random(RNG& rng) { return SmallBinary(random_integer(rng, uint64_t(0), uint64_t(mask))); }
        template <typename RNG> static SmallBinary random(RNG& rng, SmallBinary x) { return SmallBinary(x.value > 1 ? random_integer(rng, uint64_t(0), uint64_t(x.value - 1)) : 0); }
        template <typename RNG> static SmallBinary random(RNG& rng, SmallBinary x, SmallBinary y) { return SmallBinary(random_integer(rng, uint64_t(x.value), uint64_t(y.value))); }
        friend SmallBinary rotl(SmallBinary x, int y) noexcept { y %= int(N); return (x << y) | (x >> (int(N) - y)); }
        friend SmallBinary rotr(SmallBinary x, int y) noexcept { y %= int(N); return (x >> y) | (x << (int(N) - y)); }
        friend SmallBinary operator+(SmallBinary x, SmallBinary y) noexcept { auto z = x; z += y; return z; }
        friend SmallBinary operator-(SmallBinary x, SmallBinary y) noexcept { auto z = x; z -= y; return z; }
        friend SmallBinary operator*(SmallBinary x, SmallBinary y) noexcept { auto z = x; z *= y; return z; }
        friend SmallBinary operator/(SmallBinary x, SmallBinary y) noexcept { auto z = x; z /= y; return z; }
        friend SmallBinary operator%(SmallBinary x, SmallBinary y) noexcept { auto z = x; z %= y; return z; }
        friend SmallBinary operator&(SmallBinary x, SmallBinary y) noexcept { auto z = x; z &= y; return z; }
        friend SmallBinary operator|(SmallBinary x, SmallBinary y) noexcept { auto z = x; z |= y; return z; }
        friend SmallBinary operator^(SmallBinary x, SmallBinary y) noexcept { auto z = x; z ^= y; return z; }
        friend SmallBinary operator<<(SmallBinary x, int y) noexcept { auto z = x; z <<= y; return z; }
        friend SmallBinary operator>>(SmallBinary x, int y) noexcept { auto z = x; z >>= y; return z; }
        friend bool operator==(SmallBinary x, SmallBinary y) noexcept { return x.value == y.value; }
        friend bool operator<(SmallBinary x, SmallBinary y) noexcept { return x.value < y.value; }
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
        constexpr LargeBinary() noexcept {}
        LargeBinary(uint64_t x) noexcept;
        explicit LargeBinary(std::initializer_list<uint64_t> init) noexcept;
        explicit LargeBinary(const Ustring& str) noexcept { parse(str); }
        template <size_t M> explicit LargeBinary(SmallBinary<M> x) noexcept: LargeBinary(static_cast<uint64_t>(x)) {}
        template <size_t M> explicit LargeBinary(const LargeBinary<M>& x) noexcept;
        template <size_t M> explicit operator SmallBinary<M>() const noexcept { return SmallBinary<M>(static_cast<uint64_t>(*this)); }
        Ustring as_binary() const;
        Ustring as_decimal() const;
        Ustring as_hex() const;
        double as_double() const noexcept;
        void clear() noexcept { store = {}; }
        int compare(const LargeBinary& y) const noexcept;
        constexpr auto data() noexcept { return reinterpret_cast<uint8_t*>(store.data()); }
        constexpr auto data() const noexcept { return reinterpret_cast<const uint8_t*>(store.data()); }
        template <typename T> bool fits_in() const noexcept { return significant_bits() <= std::numeric_limits<T>::digits; }
        size_t hash() const noexcept { return hash_range(store); }
        size_t parse(const Ustring& str, int base = 10) noexcept { return RS_Detail::parse_binary(str, base, *this); }
        size_t significant_bits() const noexcept;
        explicit operator bool() const noexcept;
        template <typename T> explicit operator T() const noexcept;
        LargeBinary operator+() const noexcept { return *this; }
        LargeBinary operator-() const noexcept { auto x = ~ *this; ++x; return x; }
        LargeBinary operator~() const noexcept;
        LargeBinary& operator++() noexcept;
        LargeBinary operator++(int) noexcept { auto x = *this; ++*this; return x; }
        LargeBinary& operator--() noexcept;
        LargeBinary operator--(int) noexcept { auto x = *this; --*this; return x; }
        LargeBinary& operator+=(const LargeBinary& y) noexcept;
        LargeBinary& operator-=(const LargeBinary& y) noexcept;
        LargeBinary& operator*=(const LargeBinary& y) noexcept;
        LargeBinary& operator/=(const LargeBinary& y) noexcept { LargeBinary q, r; divide(*this, y, q, r); *this = q; return *this; }
        LargeBinary& operator%=(const LargeBinary& y) noexcept { LargeBinary q, r; divide(*this, y, q, r); *this = r; return *this; }
        LargeBinary& operator&=(const LargeBinary& y) noexcept;
        LargeBinary& operator|=(const LargeBinary& y) noexcept;
        LargeBinary& operator^=(const LargeBinary& y) noexcept;
        LargeBinary& operator<<=(int y) noexcept;
        LargeBinary& operator>>=(int y) noexcept;
        static void divide(const LargeBinary& x, const LargeBinary& y, LargeBinary& q, LargeBinary& r) noexcept;
        static LargeBinary from_double(double x) noexcept;
        static LargeBinary max() noexcept;
        template <typename RNG> static LargeBinary random(RNG& rng);
        template <typename RNG> static LargeBinary random(RNG& rng, const LargeBinary& x);
        template <typename RNG> static LargeBinary random(RNG& rng, const LargeBinary& x, const LargeBinary& y);
        friend LargeBinary rotl(const LargeBinary& x, int y) noexcept { y %= int(N); return (x << y) | (x >> (int(N) - y)); }
        friend LargeBinary rotr(const LargeBinary& x, int y) noexcept { y %= int(N); return (x >> y) | (x << (int(N) - y)); }
        friend LargeBinary operator+(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z += y; return z; }
        friend LargeBinary operator-(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z -= y; return z; }
        friend LargeBinary operator*(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z *= y; return z; }
        friend LargeBinary operator/(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z /= y; return z; }
        friend LargeBinary operator%(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z %= y; return z; }
        friend LargeBinary operator&(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z &= y; return z; }
        friend LargeBinary operator|(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z |= y; return z; }
        friend LargeBinary operator^(const LargeBinary& x, const LargeBinary& y) noexcept { auto z = x; z ^= y; return z; }
        friend LargeBinary operator<<(const LargeBinary& x, int y) noexcept { auto z = x; z <<= y; return z; }
        friend LargeBinary operator>>(const LargeBinary& x, int y) noexcept { auto z = x; z >>= y; return z; }
        friend bool operator==(const LargeBinary& x, const LargeBinary& y) noexcept { return x.compare(y) == 0; }
        friend bool operator<(const LargeBinary& x, const LargeBinary& y) noexcept { return x.compare(y) < 0; }
        friend std::ostream& operator<<(std::ostream& out, const LargeBinary& x) { return out << x.as_decimal(); }
        friend Ustring to_str(LargeBinary x) { return x.as_decimal(); }
    private:
        static constexpr size_t final_bits = N % unit_bits ? N % unit_bits : unit_bits;
        static constexpr size_t final_hex_digits = (final_bits + 3) / 4;
        static constexpr size_t excess_bits = unit_bits - final_bits;
        static constexpr unit_type unit_mask = ~ unit_type(0);
        static constexpr unit_type high_mask = unit_mask >> excess_bits;
        std::array<unit_type, units> store = {}; // Little endian order
        void do_mask() noexcept { store[units - 1] &= high_mask; }
        static void add_with_carry(unit_type& x, unit_type y, unit_type& c) noexcept;
    };

        template <size_t N>
        LargeBinary<N>::LargeBinary(uint64_t x) noexcept {
            store[0] = unit_type(x);
            if constexpr (units > 1)
                store[1] = unit_type(x >> unit_bits);
            for (size_t i = 2; i < units; ++i)
                store[i] = 0;
            do_mask();
        }

        template <size_t N>
        LargeBinary<N>::LargeBinary(std::initializer_list<uint64_t> init) noexcept {
            clear();
            auto ptr = init.begin();
            size_t len = init.size();
            for (size_t i = len - 1, j = 0; i != npos && j < units; --i, j += 2) {
                store[j] = unit_type(ptr[i]);
                if (j + 1 < units)
                    store[j + 1] = unit_type(ptr[i] >> unit_bits);
            }
            do_mask();
        }

        template <size_t N>
        template <size_t M>
        LargeBinary<N>::LargeBinary(const LargeBinary<M>& x) noexcept {
            size_t common_units = (std::min(M, N) + unit_bits - 1) / unit_bits;
            auto xdata = reinterpret_cast<const unit_type*>(x.data());
            size_t i = 0;
            for (; i < common_units; ++i)
                store[i] = xdata[i];
            for (; i < units; ++i)
                store[i] = 0;
            do_mask();
        }

        template <size_t N>
        Ustring LargeBinary<N>::as_binary() const {
            Ustring s;
            s.reserve(N);
            s = RS::bin(store[units - 1], final_bits);
            for (size_t i = units - 2; i != npos; --i)
                s += RS::bin(store[i]);
            return s;
        }

        template <size_t N>
        Ustring LargeBinary<N>::as_decimal() const {
            Ustring s;
            LargeBinary x(*this), base(10), q, r;
            do {
                divide(x, base, q, r);
                s += char(int(r.store[0]) + '0');
                x = q;
            } while(x);
            std::reverse(s.begin(), s.end());
            return s;
        }

        template <size_t N>
        Ustring LargeBinary<N>::as_hex() const {
            Ustring s;
            s.reserve(hex_digits);
            s = RS::hex(store[units - 1], final_hex_digits);
            for (size_t i = units - 2; i != npos; --i)
                s += RS::hex(store[i]);
            return s;
        }

        template <size_t N>
        double LargeBinary<N>::as_double() const noexcept {
            size_t i = units - 1;
            while (i != npos && store[i] == 0)
                --i;
            if (i == npos)
                return 0;
            double x = std::ldexp(double(store[i]), int(unit_bits * i));
            if (i > 0)
                x += std::ldexp(double(store[i - 1]), int(unit_bits * (i - 1)));
            return x;
        }

        template <size_t N>
        int LargeBinary<N>::compare(const LargeBinary& y) const noexcept {
            for (size_t i = units - 1; i != npos; --i)
                if (store[i] != y.store[i])
                    return store[i] < y.store[i] ? -1 : 1;
            return 0;
        }

        template <size_t N>
        size_t LargeBinary<N>::significant_bits() const noexcept {
            size_t i = units - 1;
            while (i != npos && store[i] == 0)
                --i;
            return i == npos ? 0 : unit_bits * i + ilog2p1(store[i]);
        }

        template <size_t N>
        LargeBinary<N>::operator bool() const noexcept {
            for (auto u: store)
                if (u)
                    return true;
            return false;
        }

        template <size_t N>
        template <typename T>
        LargeBinary<N>::operator T() const noexcept {
            static constexpr size_t t_bits = std::numeric_limits<T>::digits;
            static constexpr size_t t_units = (t_bits + unit_bits - 1) / unit_bits;
            static constexpr size_t n_units = std::min(t_units, units);
            if constexpr (n_units < 2) {
                return static_cast<T>(store[0]);
            } else {
                uintmax_t u = 0;
                for (int i = int(n_units) - 1; i >= 0; --i)
                    u = (u << unit_bits) + store[i];
                return static_cast<T>(u);
            }
        }

        template <size_t N>
        LargeBinary<N> LargeBinary<N>::operator~() const noexcept {
            auto y = *this;
            for (auto& u: y.store)
                u = ~ u;
            y.do_mask();
            return y;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator++() noexcept {
            ++store[0];
            for (size_t i = 1; i < units && store[i - 1] == 0; ++i)
                ++store[i];
            do_mask();
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator--() noexcept {
            --store[0];
            for (size_t i = 1; i < units && store[i - 1] == unit_mask; ++i)
                --store[i];
            do_mask();
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator+=(const LargeBinary& y) noexcept {
            unit_type carry = 0;
            for (size_t i = 0; i < units; ++i)
                add_with_carry(store[i], y.store[i], carry);
            do_mask();
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator-=(const LargeBinary& y) noexcept {
            unit_type borrow = 0, next = 0;
            for (size_t i = 0; i < units; ++i) {
                next = unit_type(store[i] < y.store[i] || (store[i] == y.store[i] && borrow));
                store[i] -= y.store[i] + borrow;
                borrow = next;
            }
            do_mask();
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator*=(const LargeBinary& y) noexcept {
            using long_type = uint64_t;
            LargeBinary z;
            for (size_t i = 0; i < units; ++i) {
                for (size_t j = 0; j < units - i; ++j) {
                    size_t k = i + j;
                    long_type p = long_type(store[i]) * long_type(y.store[j]);
                    unit_type carry = 0;
                    add_with_carry(z.store[k], unit_type(p), carry);
                    if (++k < units) {
                        add_with_carry(z.store[k], unit_type(p >> unit_bits), carry);
                        for (++k; k < units && carry; ++k)
                            add_with_carry(z.store[k], 0, carry);
                    }
                }
            }
            *this = z;
            do_mask();
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator&=(const LargeBinary& y) noexcept {
            for (size_t i = 0; i < units; ++i)
                store[i] &= y.store[i];
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator|=(const LargeBinary& y) noexcept {
            for (size_t i = 0; i < units; ++i)
                store[i] |= y.store[i];
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator^=(const LargeBinary& y) noexcept {
            for (size_t i = 0; i < units; ++i)
                store[i] ^= y.store[i];
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator<<=(int y) noexcept {
            if (y < 0) {
                *this >>= - y;
            } else if (size_t(y) >= N) {
                clear();
            } else {
                size_t skip = size_t(y / unit_bits), keep = units - skip;
                if (skip) {
                    for (size_t i = keep - 1; i != npos; --i)
                        store[i + skip] = store[i];
                    for (size_t i = 0; i < skip; ++i)
                        store[i] = 0;
                    y %= unit_bits;
                }
                unit_type carry = 0, next = 0;
                if (y) {
                    for (size_t i = skip; i < units; ++i) {
                        next = store[i] >> (unit_bits - y);
                        store[i] <<= y;
                        store[i] += carry;
                        carry = next;
                    }
                }
                do_mask();
            }
            return *this;
        }

        template <size_t N>
        LargeBinary<N>& LargeBinary<N>::operator>>=(int y) noexcept {
            if (y < 0) {
                *this <<= - y;
            } else if (size_t(y) >= N) {
                clear();
            } else {
                size_t skip = size_t(y / unit_bits), keep = units - skip;
                if (skip) {
                    for (size_t i = 0; i < keep; ++i)
                        store[i] = store[i + skip];
                    for (size_t i = keep; i < units; ++i)
                        store[i] = 0;
                    y %= unit_bits;
                }
                unit_type carry = 0, next = 0;
                if (y) {
                    for (size_t i = keep - 1; i != npos; --i) {
                        next = store[i] << (unit_bits - y);
                        store[i] >>= y;
                        store[i] += carry;
                        carry = next;
                    }
                }
            }
            return *this;
        }

        template <size_t N>
        void LargeBinary<N>::divide(const LargeBinary& x, const LargeBinary& y, LargeBinary& q, LargeBinary& r) noexcept {
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
        LargeBinary<N> LargeBinary<N>::max() noexcept {
            LargeBinary x;
            for (auto& u: x.store)
                u = unit_mask;
            x.do_mask();
            return x;
        }

        template <size_t N>
        template <typename RNG>
        LargeBinary<N> LargeBinary<N>::random(RNG& rng) {
            static constexpr size_t req_bytes = (N + 7) / 8;
            LargeBinary x;
            random_bytes(rng, x.store.data(), req_bytes);
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
                random_bytes(rng, y.store.data(), req_bytes);
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
        void LargeBinary<N>::add_with_carry(unit_type& x, unit_type y, unit_type& c) noexcept {
            x += y + c;
            c = unit_type(x < y || (x == y && c));
        }

}

namespace std {

    template <size_t N>
    struct hash<RS::SmallBinary<N>> {
        size_t operator()(const RS::SmallBinary<N>& x) const noexcept { return x.hash(); }
    };

    template <size_t N>
    struct hash<RS::LargeBinary<N>> {
        size_t operator()(const RS::LargeBinary<N>& x) const noexcept { return x.hash(); }
    };

    template <size_t N>
    class numeric_limits<RS::SmallBinary<N>>:
    public RS::NumericLimitsBase<RS::SmallBinary<N>> {
    private:
        using type                               = RS::SmallBinary<N>;
        using base                               = RS::NumericLimitsBase<type>;
    public:
        static constexpr bool is_bounded         = true;
        static constexpr bool is_exact           = true;
        static constexpr bool is_integer         = true;
        static constexpr bool is_modulo          = true;
        static constexpr bool is_signed          = false;
        static constexpr int digits              = N;
        static constexpr int digits10            = base::digits2_to_10(N);
        static constexpr int radix               = 2;
        static constexpr type lowest() noexcept  { return type(); }
        static constexpr type max() noexcept     { return ~ type(); }
        static constexpr type min() noexcept     { return type(); }
    };

    template <size_t N>
    class numeric_limits<RS::LargeBinary<N>>:
    public RS::NumericLimitsBase<RS::LargeBinary<N>> {
    private:
        using type                               = RS::LargeBinary<N>;
        using base                               = RS::NumericLimitsBase<type>;
    public:
        static constexpr bool is_bounded         = true;
        static constexpr bool is_exact           = true;
        static constexpr bool is_integer         = true;
        static constexpr bool is_modulo          = true;
        static constexpr bool is_signed          = false;
        static constexpr int digits              = N;
        static constexpr int digits10            = base::digits2_to_10(N);
        static constexpr int radix               = 2;
        static constexpr type lowest() noexcept  { return type(); }
        static constexpr type max() noexcept     { return ~ type(); }
        static constexpr type min() noexcept     { return type(); }
    };

}
