#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <ostream>
#include <stdexcept>
#include <vector>

namespace RS {

    namespace RS_Detail {

        bool parse_rational(Uview s, std::vector<Uview>& parts, bool& neg) noexcept;

    }

    template <typename T>
    class Rational {
    public:
        using integer_type = T;
        Rational() = default;
        template <typename T2> Rational(T2 t): numer(t), denom(T(1)) {}
        template <typename T2, typename T3> Rational(T2 n, T3 d): numer(n), denom(d) { reduce(); }
        template <typename T2> Rational& operator=(T2 t) { numer = t; denom = T(1); return *this; }
        explicit operator bool() const noexcept { return numer != T(0); }
        bool operator!() const noexcept { return numer == T(0); }
        template <typename T2> explicit operator T2() const { return T2(numer) / T2(denom); }
        Rational operator+() const { return *this; }
        Rational operator-() const;
        Rational& operator+=(const Rational& rhs) { return *this = *this + rhs; }
        Rational& operator-=(const Rational& rhs) { return *this = *this - rhs; }
        Rational& operator*=(const Rational& rhs) { return *this = *this * rhs; }
        Rational& operator/=(const Rational& rhs) { return *this = *this / rhs; }
        T num() const { return numer; }
        T den() const { return denom; }
        Rational abs() const;
        T floor() const { return quo(numer, denom); }
        T ceil() const;
        T round() const;
        bool is_integer() const noexcept { return denom == T(1); }
        Rational reciprocal() const;
        int sign() const noexcept { return numer > T(0) ? 1 : numer == T(0) ? 0 : -1; }
        T whole() const;
        Rational frac() const;
        Ustring str() const;
        Ustring mixed() const;
        Ustring simple() const { return to_str(numer) + '/' + to_str(denom); }
        bool try_parse(Uview s);
        static Rational parse(Uview s);
    private:
        using ldouble = long double;
        T numer = T(0), denom = T(1);
        void reduce();
        static T parse_integer(Uview s) noexcept;
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
    Rational<T> Rational<T>::operator-() const {
        auto r = *this;
        r.numer = - r.numer;
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
        if (numer < T(0))
            r.numer = - numer;
        return r;
    }

    template <typename T>
    T Rational<T>::ceil() const {
        auto qr = divide(numer, denom);
        T q = qr.first;
        if (qr.second != T(0))
            q += T(1);
        return q;
    }

    template <typename T>
    T Rational<T>::round() const {
        auto qr = divide(numer, denom);
        T q = qr.first;
        if (T(2) * qr.second >= denom)
            q += T(1);
        return q;
    }

    template <typename T>
    Rational<T> Rational<T>::reciprocal() const {
        if (numer == T(0))
            throw std::domain_error("Division by zero");
        auto r = *this;
        std::swap(r.numer, r.denom);
        return r;
    }

    template <typename T>
    T Rational<T>::whole() const {
        if (numer >= T(0))
            return numer / denom;
        else
            return - (- numer / denom);
    }

    template <typename T>
    Rational<T> Rational<T>::frac() const {
        auto r = *this;
        if (numer >= T(0))
            r.numer = numer % denom;
        else
            r.numer = - (- numer % denom);
        return r;
    }

    template <typename T>
    Ustring Rational<T>::str() const {
        Ustring s = to_str(numer);
        if (denom > T(1))
            s += '/' + to_str(denom);
        return s;
    }

    template <typename T>
    Ustring Rational<T>::mixed() const {
        T w = whole();
        auto f = frac();
        bool wx = bool(w), fx = bool(f);
        Ustring s;
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
    bool Rational<T>::try_parse(Uview s) {
        std::vector<Uview> parts;
        bool neg = false;
        if (! RS_Detail::parse_rational(s, parts, neg))
            return false;
        numer = parse_integer(parts[0]);
        if (parts.size() == 1)
            denom = T(1);
        else
            denom = parse_integer(parts.back());
        if (parts.size() == 3)
            numer = numer * denom + parse_integer(parts[1]);
        if (neg)
            numer = - numer;
        reduce();
        return true;
    }

    template <typename T>
    Rational<T> Rational<T>::parse(Uview s) {
        Rational r;
        if (! r.try_parse(s))
            throw std::invalid_argument("Invalid rational number: " + quote(s));
        return r;
    }

    template <typename T>
    void Rational<T>::reduce() {
        if (denom == T(0))
            throw std::domain_error("Division by zero");
        if (denom < T(0)) {
            numer = - numer;
            denom = - denom;
        }
        T g = gcd(numer, denom);
        numer /= g;
        denom /= g;
    }

    template <typename T>
    T Rational<T>::parse_integer(Uview s) noexcept {
        static const T ten(10);
        T t(0);
        for (char c: s)
            t = ten * t + T(c - '0');
        return t;
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
    template <typename T> Rational<T> abs(const Rational<T>& r) { return r.abs(); }
    template <typename T> int sign_of(const Rational<T>& r) noexcept { return r.sign(); }
    template <typename T> Ustring to_str(const Rational<T>& r) { return r.str(); }
    template <typename T> std::ostream& operator<<(std::ostream& o, const Rational<T>& r) { return o << r.str(); }

}
