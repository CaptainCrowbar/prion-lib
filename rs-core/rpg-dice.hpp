#pragma once

#include "rs-core/common.hpp"
#include "rs-core/rational.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ostream>
#include <random>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace RS {

    template <typename T, typename S = double>
    class RpgDice {
    public:
        using integer_type = T;
        using rational_type = Rational<T>;
        using scalar_type = S;
        RpgDice() = default;
        explicit RpgDice(T n, T faces = T(6), const Rational<T>& factor = T(1)) { insert(n, faces, factor); }
        explicit RpgDice(Uview str);
        template <typename RNG> Rational<T> operator()(RNG& rng);
        RpgDice operator+() const { return *this; }
        RpgDice operator-() const;
        RpgDice& operator+=(const RpgDice& rhs);
        RpgDice& operator+=(const Rational<T>& rhs) { modifier_ += rhs; return *this; }
        RpgDice& operator+=(T rhs) { modifier_ += rhs; return *this; }
        RpgDice& operator-=(const RpgDice& rhs);
        RpgDice& operator-=(const Rational<T>& rhs) { modifier_ -= rhs; return *this; }
        RpgDice& operator-=(T rhs) { modifier_ -= rhs; return *this; }
        RpgDice& operator*=(const Rational<T>& rhs);
        RpgDice& operator*=(T rhs) { return *this *= Rational<T>(rhs); }
        RpgDice& operator/=(const Rational<T>& rhs) { return *this *= rhs.reciprocal(); }
        RpgDice& operator/=(T rhs) { return *this *= Rational<T>(T(1), rhs); }
        Rational<T> mean() const noexcept;
        Rational<T> variance() const noexcept;
        S sd() const noexcept;
        Rational<T> min() const noexcept;
        Rational<T> max() const noexcept;
        Ustring str() const;
    private:
        using distribution_type = std::uniform_int_distribution<T>;
        struct dice_group {
            distribution_type one_dice;
            T n_dice;
            Rational<T> factor;
        };
        std::vector<dice_group> groups_;
        Rational<T> modifier_;
        void insert(T n, T faces, const Rational<T>& factor);
    };

        template <typename T, typename S>
        RpgDice<T, S>::RpgDice(Uview str) {
            static const auto parse_integer = [] (const Ustring& str, T def) noexcept {
                if (str.empty())
                    return def;
                T n = 0;
                if (from_str<T>(str, n))
                    return n;
                else
                    return def;
            };
            static const std::regex pattern(
                "([+-])"               // [1] sign
                "(?:"
                    "(?:(\\d+)[*x])?"  // [2] left multiplier
                    "(\\d*)"           // [3] number of dice
                    "d(\\d*)"          // [4] number of faces
                    "(?:[*x](\\d+))?"  // [5] right multiplier
                "|"
                    "(\\d+)"           // [6] fixed modifier
                ")"
                "(?:/(\\d+))?",        // [7] divisor
                std::regex_constants::icase | std::regex_constants::optimize);
            Ustring text(str);
            text.erase(std::remove_if(text.begin(), text.end(), ascii_isspace), text.end());
            if (text.empty())
                return;
            if (text[0] != '+' && text[0] != '-')
                text.insert(0, 1, '+');
            std::smatch match;
            auto begin = text.cbegin(), end = text.cend();
            while (begin != end) {
                if (! std::regex_search(begin, end, match, pattern, std::regex_constants::match_continuous))
                    throw std::invalid_argument("Invalid dice");
                auto sign = *match[0].first == '-' ? -1 : 1;
                auto divisor = parse_integer(match[7], 1);
                if (match[6].matched) {
                    auto factor1 = parse_integer(match[6], 1);
                    modifier_ += Rational<T>(sign * factor1, divisor);
                } else {
                    auto factor1 = parse_integer(match[2], 1);
                    auto n_dice = parse_integer(match[3], 1);
                    auto n_faces = parse_integer(match[4], 6);
                    auto factor2 = parse_integer(match[5], 1);
                    insert(n_dice, n_faces, Rational<T>(sign * factor1 * factor2, divisor));
                }
                begin = match[0].second;
            }
        }

        template <typename T, typename S>
        template <typename RNG>
        Rational<T> RpgDice<T, S>::operator()(RNG& rng) {
            Rational<T> sum = modifier_;
            for (auto& g: groups_) {
                T roll = 0;
                for (T i = 0; i < g.n_dice; ++i)
                    roll += g.one_dice(rng);
                sum += roll * g.factor;
            }
            return sum;
        }

        template <typename T, typename S>
        RpgDice<T, S> RpgDice<T, S>::operator-() const {
            RpgDice d = *this;
            for (auto& g: d.groups_)
                g.factor = - g.factor;
            d.modifier_ = - d.modifier_;
            return d;
        }

        template <typename T, typename S>
        RpgDice<T, S>& RpgDice<T, S>::operator+=(const RpgDice& rhs) {
            RpgDice d = *this;
            for (auto& g: rhs.groups_)
                d.insert(g.n_dice, g.one_dice.b(), g.factor);
            d.modifier_ += rhs.modifier_;
            *this = std::move(d);
            return *this;
        }

        template <typename T, typename S>
        RpgDice<T, S>& RpgDice<T, S>::operator-=(const RpgDice& rhs) {
            RpgDice d = *this;
            for (auto& g: rhs.groups_)
                d.insert(g.n_dice, g.one_dice.b(), - g.factor);
            d.modifier_ -= rhs.modifier_;
            *this = std::move(d);
            return *this;
        }

        template <typename T, typename S>
        RpgDice<T, S>& RpgDice<T, S>::operator*=(const Rational<T>& rhs) {
            if (rhs) {
                for (auto& g: groups_)
                    g.factor *= rhs;
                modifier_ *= rhs;
            } else {
                groups_.clear();
                modifier_ = 0;
            }
            return *this;
        }

        template <typename T, typename S>
        Rational<T> RpgDice<T, S>::mean() const noexcept {
            Rational<T> sum = modifier_;
            for (auto& g: groups_)
                sum += Rational<T>(g.n_dice * (g.one_dice.b() + 1)) * g.factor / Rational<T>(2);
            return sum;
        }

        template <typename T, typename S>
        Rational<T> RpgDice<T, S>::variance() const noexcept {
            Rational<T> sum;
            for (auto& g: groups_)
                sum += Rational<T>(g.n_dice * (g.one_dice.b() * g.one_dice.b() - 1)) * g.factor * g.factor / Rational<T>(12);
            return sum;
        }

        template <typename T, typename S>
        S RpgDice<T, S>::sd() const noexcept {
            using std::sqrt;
            return sqrt(S(variance()));
        }

        template <typename T, typename S>
        Rational<T> RpgDice<T, S>::min() const noexcept {
            Rational<T> sum = modifier_;
            for (auto& g: groups_) {
                if (g.factor > 0)
                    sum += Rational<T>(g.n_dice) * g.factor;
                else
                    sum += Rational<T>(g.n_dice * g.one_dice.b()) * g.factor;
            }
            return sum;
        }

        template <typename T, typename S>
        Rational<T> RpgDice<T, S>::max() const noexcept {
            Rational<T> sum = modifier_;
            for (auto& g: groups_) {
                if (g.factor > 0)
                    sum += Rational<T>(g.n_dice * g.one_dice.b()) * g.factor;
                else
                    sum += Rational<T>(g.n_dice) * g.factor;
            }
            return sum;
        }

        template <typename T, typename S>
        Ustring RpgDice<T, S>::str() const {
            Ustring text;
            for (auto& g: groups_) {
                text += g.factor.sign() == -1 ? '-' : '+';
                if (g.n_dice > 1)
                    text += std::to_string(g.n_dice);
                text += 'd' + std::to_string(g.one_dice.b());
                auto n = std::abs(g.factor.num());
                if (n > 1)
                    text += '*' + std::to_string(n);
                auto d = g.factor.den();
                if (d > 1)
                    text += '/' + std::to_string(d);
            }
            if (modifier_ > 0)
                text += '+';
            if (modifier_)
                text += modifier_.str();
            if (text[0] == '+')
                text.erase(0, 1);
            if (text.empty())
                text = "0";
            return text;
        }

        template <typename T, typename S>
        void RpgDice<T, S>::insert(T n, T faces, const Rational<T>& factor) {
            static const auto match_terms = [] (const dice_group& g1, const dice_group& g2) noexcept {
                return g1.one_dice.b() == g2.one_dice.b() && g1.factor == g2.factor;
            };
            static const auto sort_terms = [] (const dice_group& g1, const dice_group& g2) noexcept {
                return g1.one_dice.b() == g2.one_dice.b() ? g1.factor < g2.factor : g1.one_dice.b() > g2.one_dice.b();
            };
            if (n < 0 || faces < 0)
                throw std::invalid_argument("Invalid dice");
            if (n > 0 && faces > 0 && factor != 0) {
                dice_group g;
                g.one_dice = distribution_type(1, faces);
                g.n_dice = n;
                g.factor = factor;
                auto it = std::lower_bound(groups_.begin(), groups_.end(), g, sort_terms);
                if (it != groups_.end() && match_terms(*it, g))
                    it->n_dice += g.n_dice;
                else
                    groups_.insert(it, g);
            }
        }

    using IntDice = RpgDice<int>;

    template <typename T, typename S> RpgDice<T, S> operator+(const RpgDice<T, S>& lhs, const RpgDice<T, S>& rhs) { auto d = lhs; d += rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator+(const RpgDice<T, S>& lhs, const Rational<T>& rhs) { auto d = lhs; d += rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator+(const RpgDice<T, S>& lhs, T rhs) { auto d = lhs; d += rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator+(const Rational<T>& lhs, const RpgDice<T, S>& rhs) { auto d = rhs; d += lhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator+(T lhs, const RpgDice<T, S>& rhs) { auto d = rhs; d += lhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator-(const RpgDice<T, S>& lhs, const RpgDice<T, S>& rhs) { auto d = lhs; d -= rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator-(const RpgDice<T, S>& lhs, const Rational<T>& rhs) { auto d = lhs; d -= rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator-(const RpgDice<T, S>& lhs, T rhs) { auto d = lhs; d -= rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator-(const Rational<T>& lhs, const RpgDice<T, S>& rhs) { auto d = - rhs; d += lhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator-(T lhs, const RpgDice<T, S>& rhs) { auto d = - rhs; d += lhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator*(const RpgDice<T, S>& lhs, const Rational<T>& rhs) { auto d = lhs; d *= rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator*(const RpgDice<T, S>& lhs, T rhs) { auto d = lhs; d *= rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator*(const Rational<T>& lhs, const RpgDice<T, S>& rhs) { auto d = rhs; d *= lhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator*(T lhs, const RpgDice<T, S>& rhs) { auto d = rhs; d *= lhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator/(const RpgDice<T, S>& lhs, const Rational<T>& rhs) { auto d = lhs; d /= rhs; return d; }
    template <typename T, typename S> RpgDice<T, S> operator/(const RpgDice<T, S>& lhs, T rhs) { auto d = lhs; d /= rhs; return d; }
    template <typename T, typename S> std::ostream& operator<<(std::ostream& out, const RpgDice<T, S>& d) { return out << d.str(); }

    inline IntDice operator""_d4(unsigned long long n) { return IntDice(int(n), 4); }
    inline IntDice operator""_d6(unsigned long long n) { return IntDice(int(n), 6); }
    inline IntDice operator""_d8(unsigned long long n) { return IntDice(int(n), 8); }
    inline IntDice operator""_d10(unsigned long long n) { return IntDice(int(n), 10); }
    inline IntDice operator""_d12(unsigned long long n) { return IntDice(int(n), 12); }
    inline IntDice operator""_d20(unsigned long long n) { return IntDice(int(n), 20); }
    inline IntDice operator""_d100(unsigned long long n) { return IntDice(int(n), 100); }
    inline IntDice operator""_dice(const char* p, size_t n) { return IntDice(Uview(p, n)); }

}
