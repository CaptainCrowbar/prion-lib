#include "rs-core/random.hpp"

namespace RS {

    // Class UniformIntegerProperties

    double UniformIntegerProperties::pdf(int x) const noexcept {
        if (x >= lo && x <= hi)
            return 1.0 / (hi - lo + 1);
        else
            return 0;
    }

    double UniformIntegerProperties::cdf(int x) const noexcept {
        if (x < lo)
            return 0;
        else if (x < hi)
            return double(x - lo + 1) / (hi - lo + 1);
        else
            return 1;
    }

    double UniformIntegerProperties::ccdf(int x) const noexcept {
        if (x <= lo)
            return 1;
        else if (x <= hi)
            return double(hi - x + 1) / (hi - lo + 1);
        else
            return 0;
    }

    // Class BinomialDistributionProperties

    double BinomialDistributionProperties::pdf(int x) const noexcept {
        if (x >= 0 && x <= tests)
            return xbinomial(tests, x) * std::pow(prob, double(x)) * std::pow(1.0 - prob, double(tests - x));
        else
            return 0;
    }

    double BinomialDistributionProperties::cdf(int x) const noexcept {
        if (x < 0)
            return 0;
        else if (x >= tests)
            return 1;
        double c = 0;
        for (int y = 0; y <= x; ++y)
            c += pdf(y);
        return c;
    }

    double BinomialDistributionProperties::ccdf(int x) const noexcept {
        if (x <= 0)
            return 1;
        else if (x > tests)
            return 0;
        double c = 0;
        for (int y = tests; y >= x; --y)
            c += pdf(y);
        return c;
    }

    // Class DiceProperties

    double DiceProperties::pdf(int x) const noexcept {
        if (x < num || x > max())
            return 0;
        double s = 1, t = 0;
        for (int i = 0, j = x - 1; i < num; ++i, j -= fac, s = - s)
            t += s * xbinomial(j, num - 1) * xbinomial(num, i);
        return t * std::pow(double(fac), - double(num));
    }

    double DiceProperties::ccdf(int x) const noexcept {
        if (x <= num)
            return 1;
        if (x > max())
            return 0;
        double s = 1, t = 0;
        for (int i = 0, j = num * (fac + 1) - x; i < num; ++i, j -= fac, s = - s)
            t += s * xbinomial(j, num) * xbinomial(num, i);
        return t * std::pow(double(fac), - double(num));
    }

    // Class UniformRealProperties

    double UniformRealProperties::sd() const noexcept {
        static const double inv_sqrt12 = sqrt(1.0 / 12.0);
        return inv_sqrt12 * (hi - lo);
    }

    double UniformRealProperties::pdf(double x) const noexcept {
        if (x > lo && x < hi)
            return 1 / (hi - lo);
        else
            return 0;
    }

    double UniformRealProperties::cdf(double x) const noexcept {
        if (x <= lo)
            return 0;
        else if (x < hi)
            return (x - lo) / (hi - lo);
        else
            return 1;
    }

    double UniformRealProperties::ccdf(double x) const noexcept {
        if (x <= lo)
            return 1;
        else if (x < hi)
            return (hi - x) / (hi - lo);
        else
            return 0;
    }

    // Class NormalDistributionProperties

    double NormalDistributionProperties::quantile(double p) const noexcept {
        if (p < 0.5)
            return xm - xs * cquantile_z(p);
        else if (p == 0.5)
            return xm;
        else
            return xm + xs * cquantile_z(1 - p);
    }

    double NormalDistributionProperties::cquantile(double q) const noexcept {
        if (q < 0.5)
            return xm + xs * cquantile_z(q);
        else if (q == 0.5)
            return xm;
        else
            return xm - xs * cquantile_z(1 - q);
    }

    double NormalDistributionProperties::pdf_z(double z) const noexcept {
        return inv_sqrt2_d * inv_sqrtpi_d * exp(- z * z / 2);
    }

    double NormalDistributionProperties::cdf_z(double z) const noexcept {
        return (erf(z / sqrt2_d) + 1) / 2;
    }

    double NormalDistributionProperties::cquantile_z(double q) const noexcept {
        // Beasley-Springer approximation
        // For |z|<3.75, absolute error <1e-6, relative error <2.5e-7
        // For |z|<7.5, absolute error <5e-4, relative error <5e-5
        // This will always be called with 0<q<1/2
        static constexpr double threshold = 0.08;
        static constexpr double a1 = 2.321213;
        static constexpr double a2 = 4.850141;
        static constexpr double a3 = -2.297965;
        static constexpr double a4 = -2.787189;
        static constexpr double b1 = 1.637068;
        static constexpr double b2 = 3.543889;
        static constexpr double c1 = -25.44106;
        static constexpr double c2 = 41.3912;
        static constexpr double c3 = -18.615;
        static constexpr double c4 = 2.506628;
        static constexpr double d1 = 3.130829;
        static constexpr double d2 = -21.06224;
        static constexpr double d3 = 23.08337;
        static constexpr double d4 = -8.473511;
        if (q < threshold) {
            double r = sqrt(- log(q));
            return (((a1 * r + a2) * r + a3) * r + a4) / ((b1 * r + b2) * r + 1);
        } else {
            double q1 = 0.5 - q;
            double r = q1 * q1;
            return q1 * (((c1 * r + c2) * r + c3) * r + c4) / ((((d1 * r + d2) * r + d3) * r + d4) * r + 1);
        }
    }

    // Text generators

    Ustring lorem_ipsum(uint64_t seed, size_t bytes, bool paras) {
        static constexpr const char* classic[] = {
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ",
            "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. ",
            "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. ",
            "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. ",
        };
        static constexpr size_t n_lines = sizeof(classic) / sizeof(classic[0]);
        if (bytes == 0)
            return {};
        Xoroshiro rng(seed, 0x05b6b84c03ae03d2ull);
        Ustring text;
        text.reserve(bytes + 20);
        for (size_t i = 0; i < n_lines && text.size() <= bytes; ++i)
            text += classic[i];
        if (paras)
            text.replace(text.size() - 1, 1, "\n\n");
        while (text.size() <= bytes) {
            size_t n_para = paras ? rng() % 7 + 1 : npos;
            for (size_t i = 0; i < n_para && text.size() <= bytes; ++i)
                text += classic[rng() % n_lines];
            if (paras)
                text.replace(text.size() - 1, 1, "\n\n");
        }
        size_t cut = text.find_first_of("\n .,", bytes);
        if (cut != npos)
            text.resize(cut);
        while (! ascii_isalpha(text.back()))
            text.pop_back();
        text += '.';
        if (paras)
            text += '\n';
        return text;
    }

}
