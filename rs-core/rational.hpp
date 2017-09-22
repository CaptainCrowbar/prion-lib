#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cstdlib>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <type_traits>

namespace RS {

    namespace RS_Detail {

        template <typename T>
        struct IntegerParser {
            T operator()(const U8string& s) const noexcept {
                if (std::is_signed<T>::value)
                    return T(strtoll(s.data(), nullptr, 10));
                else
                    return T(strtoull(s.data(), nullptr, 10));
            }
        };

    }

    template <typename T>
    class Rational {
    public:
        using integer_type = T;
        Rational() = default;
        template <typename T2> Rational(T2 t): nm(t), dn(T(1)) {}
        template <typename T2, typename T3> Rational(T2 n, T3 d): nm(n), dn(d) { reduce(); }
        explicit Rational(const U8string& s);
        explicit Rational(const char* s): Rational(cstr(s)) {}
        template <typename T2> Rational& operator=(T2 t) { nm = t; dn = T(1); return *this; }
        explicit operator bool() const noexcept { return nm != T(0); }
        bool operator!() const noexcept { return nm == T(0); }
        template <typename T2> explicit operator T2() const { return T2(nm) / T2(dn); }
        Rational operator+() const { return *this; }
        Rational operator-() const;
        Rational& operator+=(const Rational& rhs) { return *this = *this + rhs; }
        Rational& operator-=(const Rational& rhs) { return *this = *this - rhs; }
        Rational& operator*=(const Rational& rhs) { return *this = *this * rhs; }
        Rational& operator/=(const Rational& rhs) { return *this = *this / rhs; }
        T num() const { return nm; }
        T den() const { return dn; }
        Rational abs() const;
        T floor() const { return quo(nm, dn); }
        T ceil() const;
        T round() const;
        bool is_integer() const noexcept { return dn == T(1); }
        Rational reciprocal() const;
        int sign() const noexcept { return nm > T(0) ? 1 : nm == T(0) ? 0 : -1; }
        T whole() const;
        Rational frac() const;
        U8string str() const;
        U8string mixed() const;
        U8string simple() const { return to_str(nm) + '/' + to_str(dn); }
    private:
        using ldouble = long double;
        T nm = T(0), dn = T(1);
        void reduce();
    };

    using Rat = Rational<int>;
    using Urat = Rational<unsigned>;
    using Rat16 = Rational<int16_t>;
    using Rat32 = Rational<int32_t>;
    using Rat64 = Rational<int64_t>;
    using Urat16 = Rational<uint16_t>;
    using Urat32 = Rational<uint32_t>;
    using Urat64 = Rational<uint64_t>;

    template <typename T>
    Rational<T>::Rational(const U8string& s) {
        static constexpr RS_Detail::IntegerParser<T> make_integer;
        static const std::regex int_pattern("([+-]?)\\s*(\\d+)\\s*");
        static const std::regex rat_pattern("([+-]?)\\s*(?:(\\d+)\\s+)?(\\d+)\\s*/\\s*(\\d+)\\s*");
        U8string ipart, npart, dpart;
        bool neg = false;
        std::smatch match;
        if (std::regex_match(s, match, int_pattern)) {
            ipart = match[2];
            npart.clear();
            dpart.clear();
            neg = match[1] == "-";
        } else if (std::regex_match(s, match, rat_pattern)) {
            ipart = match[2];
            npart = match[3];
            dpart = match[4];
            neg = match[1] == "-";
        } else {
            throw std::invalid_argument("Invalid rational number: " + quote(s));
        }
        if (! npart.empty())
            nm = make_integer(npart);
        if (! dpart.empty())
            dn = make_integer(dpart);
        if (! ipart.empty())
            nm += dn * make_integer(ipart);
        reduce();
        if (neg)
            nm = - nm;
    }

    template <typename T>
    Rational<T> Rational<T>::operator-() const {
        auto r = *this;
        r.nm = - r.nm;
        return r;
    }

    template <typename T>
    Rational<T> operator+(const Rational<T>& lhs, const Rational<T>& rhs) {
        return {lhs.num() * rhs.den() + rhs.num() * lhs.den(), lhs.den() * rhs.den()};
    }

    template <typename T>
    Rational<T> operator-(const Rational<T>& lhs, const Rational<T>& rhs) {
        return {lhs.num() * rhs.den() - rhs.num() * lhs.den(), lhs.den() * rhs.den()};
    }

    template <typename T>
    Rational<T> operator*(const Rational<T>& lhs, const Rational<T>& rhs) {
        return {lhs.num() * rhs.num(), lhs.den() * rhs.den()};
    }

    template <typename T>
    Rational<T> operator/(const Rational<T>& lhs, const Rational<T>& rhs) {
        return {lhs.num() * rhs.den(), lhs.den() * rhs.num()};
    }

    template <typename T>
    Rational<T> Rational<T>::abs() const {
        auto r = *this;
        if (nm < T(0))
            r.nm = - nm;
        return r;
    }

    template <typename T>
    T Rational<T>::ceil() const {
        auto qr = divide(nm, dn);
        T q = qr.first;
        if (qr.second != T(0))
            q += T(1);
        return q;
    }

    template <typename T>
    T Rational<T>::round() const {
        auto qr = divide(nm, dn);
        T q = qr.first;
        if (T(2) * qr.second >= dn)
            q += T(1);
        return q;
    }

    template <typename T>
    Rational<T> Rational<T>::reciprocal() const {
        if (nm == T(0))
            throw std::domain_error("Division by zero");
        auto r = *this;
        std::swap(r.nm, r.dn);
        return r;
    }

    template <typename T>
    T Rational<T>::whole() const {
        if (nm >= T(0))
            return nm / dn;
        else
            return - (- nm / dn);
    }

    template <typename T>
    Rational<T> Rational<T>::frac() const {
        auto r = *this;
        if (nm >= T(0))
            r.nm = nm % dn;
        else
            r.nm = - (- nm % dn);
        return r;
    }

    template <typename T>
    U8string Rational<T>::str() const {
        U8string s = to_str(nm);
        if (dn > T(1))
            s += '/' + to_str(dn);
        return s;
    }

    template <typename T>
    U8string Rational<T>::mixed() const {
        T w = whole();
        auto f = frac();
        bool wx = bool(w), fx = bool(f);
        U8string s;
        if (wx || ! fx) {
            s = to_str(w);
            f = f.abs();
        }
        if (wx && fx)
            s += ' ';
        if (fx)
            s += f.simple();
        return s;
    }

    template <typename T>
    void Rational<T>::reduce() {
        if (dn == T(0))
            throw std::domain_error("Division by zero");
        if (dn < T(0)) {
            nm = - nm;
            dn = - dn;
        }
        T g = gcd(nm, dn);
        nm /= g;
        dn /= g;
    }

    template <typename T1, typename T2> bool operator==(const Rational<T1>& x, const Rational<T2>& y) { return x.num() == y.num() && x.den() == y.den(); }
    template <typename T1, typename T2> bool operator<(const Rational<T1>& x, const Rational<T2>& y) { return x.num() * y.den() < y.num() * x.den(); }
    template <typename T1, typename T2> bool operator==(const Rational<T1>& x, const T2& y) { return x.num() == y && x.den() == T1(1); }
    template <typename T1, typename T2> bool operator<(const Rational<T1>& x, const T2& y) { return x.num() < y * x.den(); }
    template <typename T1, typename T2> bool operator==(const T1& x, const Rational<T2>& y) { return x == y.num() && y.den() == T2(1); }
    template <typename T1, typename T2> bool operator<(const T1& x, const Rational<T2>& y) { return x * y.den() < y.num(); }
    template <typename T1, typename T2> bool operator!=(const Rational<T1>& x, const Rational<T2>& y) { return ! (x == y); }
    template <typename T1, typename T2> bool operator>(const Rational<T1>& x, const Rational<T2>& y) { return y < x; }
    template <typename T1, typename T2> bool operator<=(const Rational<T1>& x, const Rational<T2>& y) { return ! (y < x); }
    template <typename T1, typename T2> bool operator>=(const Rational<T1>& x, const Rational<T2>& y) { return ! (x < y); }
    template <typename T1, typename T2> bool operator!=(const Rational<T1>& x, const T2& y) { return ! (x == y); }
    template <typename T1, typename T2> bool operator>(const Rational<T1>& x, const T2& y) { return y < x; }
    template <typename T1, typename T2> bool operator<=(const Rational<T1>& x, const T2& y) { return ! (y < x); }
    template <typename T1, typename T2> bool operator>=(const Rational<T1>& x, const T2& y) { return ! (x < y); }
    template <typename T1, typename T2> bool operator!=(const T1& x, const Rational<T2>& y) { return ! (x == y); }
    template <typename T1, typename T2> bool operator>(const T1& x, const Rational<T2>& y) { return y < x; }
    template <typename T1, typename T2> bool operator<=(const T1& x, const Rational<T2>& y) { return ! (y < x); }
    template <typename T1, typename T2> bool operator>=(const T1& x, const Rational<T2>& y) { return ! (x < y); }

    template <typename T>
    std::ostream& operator<<(std::ostream& o, const Rational<T>& r) {
        o << r.num();
        if (! r.is_integer())
            o << '/' << r.den();
        return o;
    }

    template <typename T>
    Rational<T> abs(const Rational<T>& r) {
        return r.abs();
    }

    template <typename T>
    int sign_of(const Rational<T>& r) noexcept {
        return r.sign();
    }

}
