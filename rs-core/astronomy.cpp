#include "rs-core/astronomy.hpp"
#include "rs-core/range.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <limits>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

using namespace RS::Literals;
using namespace RS::Range;
using namespace RS::Unicorn;
using namespace std::literals;

namespace RS::Astro {

    namespace {

        static constexpr double nan = std::numeric_limits<double>::quiet_NaN();

        constexpr bool is_number_char(char c) noexcept { return (c >= '0' && c <= '9') || c == '.'; }

    }

    // Blackbody radiation

    double BlackbodySpectrum::peak() const noexcept {
        return b_W / temp_;
    }

    double BlackbodySpectrum::power() const noexcept {
        return sigma * std::pow(temp_, 4.0);
    }

    double BlackbodySpectrum::operator()(double lambda) const noexcept {
        return c1_L / (std::pow(lambda, 5) * (std::exp(c2 / (lambda * temp_)) - 1));
    }

    // Coordinate systems

    void radians_to_degrees(double rad, int& deg, double& min) noexcept {
        double i = 0;
        double f = std::modf(radians_to_degrees(rad), &i);
        deg = int(i);
        min = 60 * f;
    }

    void radians_to_degrees(double rad, int& deg, int& min, double& sec) noexcept {
        double i = 0;
        double f = std::modf(radians_to_degrees(rad), &i);
        deg = int(i);
        f = std::modf(60 * f, &i);
        min = int(i);
        sec = 60 * f;
    }

    void radians_to_hours(double rad, int& hrs, double& min) noexcept {
        double i = 0;
        double f = std::modf(radians_to_hours(rad), &i);
        hrs = int(i);
        min = 60 * f;
    }

    void radians_to_hours(double rad, int& hrs, int& min, double& sec) noexcept {
        double i = 0;
        double f = std::modf(radians_to_hours(rad), &i);
        hrs = int(i);
        f = std::modf(60 * f, &i);
        min = int(i);
        sec = 60 * f;
    }

    Ustring format_angle(double rad, Uview fmt) {
        static const Regex pattern(R"(
            ( [dh] (?:ms?)? )  # [1] basic format
            ( \+ )?            # [2] sign flag
            ( [0-9]+ )?        # [3] decimal precision
            ( [ips]+ )?        # [4] other flags
            (?:, ( [^,]* ) )?  # [5] degree or hour symbol
            (?:, ( [^,]* ) )?  # [6] minute symbol
            (?:, ( [^,]* ) )?  # [7] second symbol
        )", Regex::extended | Regex::full | Regex::optimize);
        auto match = pattern(fmt);
        if (! match)
            throw std::invalid_argument("Invalid angle format: " + quote(fmt));
        bool hrs_flag = match[1][0] == 'h';
        bool min_flag = match.count(1) >= 2;
        bool sec_flag = match.count(1) >= 3;
        bool sign_flag = match.matched(2);
        int prec = int(decnum(match[3]));
        bool insert_flag = match[4].find('i') != npos;
        bool pad_flag = match[4].find('p') != npos;
        bool std_flag = match[4].find('s') != npos;
        Uview whole_symbol = match[5];
        Uview min_symbol = match[6];
        Uview sec_symbol = match[7];
        if (std_flag) {
            if (! whole_symbol.empty() || ! min_symbol.empty() || ! sec_symbol.empty())
                throw std::invalid_argument("Invalid angle format: " + quote(fmt));
            if (hrs_flag) {
                whole_symbol = "h"sv;
                min_symbol = "m"sv;
                sec_symbol = "s"sv;
            } else {
                whole_symbol = "˚"sv;
                min_symbol = "\'"sv;
                sec_symbol = "\""sv;
            }
        }
        Uview last_symbol;
        if (sec_flag)
            last_symbol = sec_symbol;
        else if (min_flag)
            last_symbol = min_symbol;
        else
            last_symbol = whole_symbol;
        Ustring text;
        if (rad < 0)
            text += '-';
        else if (sign_flag)
            text += '+';
        rad = std::abs(rad);
        double value = rad * 180 / pi;
        if (hrs_flag)
            value /= 15;
        double whole = 0, min = 0, sec = 0;
        double frac = std::modf(value, &whole);
        if (min_flag) {
            frac = std::modf(60 * frac, &min);
            if (sec_flag)
                frac = std::modf(60 * frac, &sec);
        }
        Ustring frac_str;
        bool overflow = false;
        if (prec > 0) {
            frac_str = fp_format(frac, 'f', prec);
            if (frac_str[0] == '1') {
                overflow = true;
                frac_str.assign(prec, '0');
            } else {
                frac_str.erase(0, 2);
            }
        } else {
            overflow = frac >= 0.5;
        }
        if (overflow && sec_flag) {
            sec += 1;
            if (sec == 60)
                sec = 0;
            else
                overflow = false;
        }
        if (overflow && min_flag) {
            min += 1;
            if (min == 60)
                min = 0;
            else
                overflow = false;
        }
        if (overflow)
            whole += 1;
        int digits = 1;
        if (pad_flag) {
            if (hrs_flag)
                digits = 2;
            else
                digits = 3;
        }
        text += dec(int(whole), digits);
        if (min_flag) {
            if (whole_symbol.empty())
                text += ' ';
            else
                text += whole_symbol;
            text += dec(int(min), 2);
            if (sec_flag) {
                if (min_symbol.empty())
                    text += ' ';
                else
                    text += min_symbol;
                text += dec(int(sec), 2);
            }
        }
        if (prec == 0) {
            text += last_symbol;
        } else {
            if (insert_flag)
                text += last_symbol;
            text += '.' + frac_str;
            if (! insert_flag)
                text += last_symbol;
        }
        return text;
    }

    double parse_angle(const Ustring& str, Uview fmt) {
        static const Regex pattern(R"(
            \s* ( [+-] )?                            # [1] sign
            \s* ( \d+ (?: (\D*) \.\d+ )? ) (\D*)     # [2] degrees or hours, [3] or [4] units
            \s* (?: ( \d+ (?: \D* \.\d+ )? ) \D* )?  # [5] minutes
            \s* (?: ( \d+ (?: \D* \.\d+ )? ) \D* )?  # [6] seconds
            \s*
        )", Regex::extended | Regex::full | Regex::optimize);
        check_string(str);
        if (! fmt.empty() && fmt[0] != 'd' && fmt[0] != 'h')
            throw std::invalid_argument("Invalid angle format: " + quote(fmt));
        auto match = pattern(str);
        if (! match)
            throw std::invalid_argument("Invalid formatted angle: " + quote(str));
        bool neg = match.matched(1) && match[1][0] == '-';
        Ustring whole_str(match[2]);
        Ustring unit_str(match.matched(3) ? match[3] : match[4]);
        Ustring min_str(match[5]);
        Ustring sec_str(match[6]);
        whole_str << filter(is_number_char);
        min_str << filter(is_number_char);
        sec_str << filter(is_number_char);
        double whole = fpnum(whole_str);
        double min = fpnum(min_str);
        double sec = fpnum(sec_str);
        char unit = 0;
        if (! unit_str.empty()) {
            char32_t initial = *utf_begin(unit_str);
            if (initial == U'd' || initial == U'D' || initial == U'˚')
                unit = 'd';
            else if (initial == U'h' || initial == U'H')
                unit = 'h';
        }
        if (! fmt.empty()) {
            if (unit == 0)
                unit = fmt[0];
            else if (unit != fmt[0])
                throw std::invalid_argument("Invalid formatted angle: " + quote(str));
        }
        double value = whole + min / 60 + sec / 3600;
        if (unit == 'h')
            value *= 15;
        if (neg)
            value = - value;
        return value * pi / 180;
    }

    // Solar system data

    const std::vector<PlanetData>& planet_table() {
        static const std::vector<PlanetData> table = {
            {},
            // Name       Distance        Period           Ecc         Incl           Mass         Radius     Equ rad    Pol rad    Density    Rotation        Ax incl          Satellites
            { "Mercury",  0.387098*au,    0.240846*jy,     0.205630,   7.005_deg,     3.3011e23,   2439.7e3,  2439.7e3,  2439.7e3,  5.427e3,   58.646*jd,      2.04*arcmin,     0 },
            { "Venus",    0.723332*au,    0.615198*jy,     0.006772,   3.39458_deg,   4.8675e24,   6051.8e3,  6051.8e3,  6051.8e3,  5.243e3,   243.025*jd,     177.36_deg,      0 },
            { "Earth",    1.00000102*au,  1.000017421*jy,  0.0167086,  0.00005_deg,   5.97237e24,  6371.0e3,  6378.1e3,  6356.8e3,  5.514e3,   0.99726968*jd,  23.4392811_deg,  1 },
            { "Mars",     1.523679*au,    1.88082*jy,      0.0934,     1.850_deg,     6.4171e23,   3389.5e3,  3396.2e3,  3376.2e3,  3.9335e3,  1.025957*jd,    25.19_deg,       2 },
            { "Jupiter",  5.2044*au,      11.862*jy,       0.0489,     1.303_deg,     1.8982e27,   69911e3,   71492e3,   66854e3,   1.326e3,   9.925*3600,     3.13_deg,        79 },
            { "Saturn",   9.5826*au,      29.4571*jy,      0.0565,     2.485_deg,     5.6834e26,   58232e3,   60268e3,   54364e3,   0.687e3,   10.55*3600,     26.73_deg,       62 },
            { "Uranus",   19.2184*au,     84.0205*jy,      0.046381,   0.773_deg,     8.6810e25,   25362e3,   25559e3,   24973e3,   1.27e3,    0.71833*jd,     97.77_deg,       27 },
            { "Neptune",  30.11*au,       164.8*jy,        0.009456,   1.767975_deg,  1.02413e26,  24622e3,   24764e3,   24341e3,   1.638e3,   0.6713*jd,      28.32_deg,       14 },
        };
        return table;
    }

    const std::vector<DwarfPlanetData>& dwarf_planet_table() {
        static const std::vector<DwarfPlanetData> table = {
            // Number  Name          Distance    Period      Ecc        Incl           Mass       Radius     Density  Rotation       Sat
            { 1,       "Ceres",      2.7675*au,  4.60*jy,    0.075823,  10.593_deg,    9.393e20,  473e3,     2161,    0.3781*jd,     0 },
            { 90482,   "Orcus",      39.398*au,  247.29*jy,  0.2201,    20.582_deg,    6.41e20,   459e3,     1530,    13.188*3600,   1 },
            { 134340,  "Pluto",      39.48*au,   248.00*jy,  0.2488,    17.16_deg,     1.303e22,  1188.3e3,  1854,    6.387230*jd,   5 },
            { 0 ,      "2002 MS4",   41.717*au,  269.45*jy,  0.14439,   17.677_deg,    nan,       467e3,     nan,     nan,           0 },
            { 120347,  "Salacia",    41.947*au,  271.68*jy,  0.1097,    23.925_deg,    4.38e20,   427e3,     1290,    6.09*3600,     1 },
            { 136108,  "Haumea",     43.218*au,  284.12*jy,  0.19126,   28.19_deg,     4.006e21,  816e3,     2600,    0.163146*jd,   2 },
            { 50000,   "Quaoar",     43.616*au,  288.06*jy,  0.0376,    7.9870_deg,    1.4e21,    555e3,     1990,    17.6788*3600,  1 },
            { 136472,  "Makemake",   45.715*au,  309.09*jy,  0.15586,   29.00685_deg,  4.4e21,    715e3,     2300,    7.771*3600,    1 },
            { 225088,  "2007 OR10",  67.059*au,  549.16*jy,  0.5064,    30.904_deg,    1.75e21,   627e3,     1720,    44.81*3600,    1 },
            { 136199,  "Eris",       67.781*au,  558.04*jy,  0.44068,   44.0445_deg,   1.66e22,   1163e3,    2520,    25.9*3600,     1 },
            { 90377,   "Sedna",      506.8*au,   11400*jy,   0.85491,   11.92872_deg,  nan,       498e3,     nan,     10.273*3600,   0 },
        };
        return table;
    }

    const std::vector<SatelliteData>& satellite_table() {
        static const std::vector<SatelliteData> table = {

            // Pl  Sat  Label       Name            Distance     Period          Ecc        Incl          Mass           Radius      Density
            { 3,   1,   "I",        "Moon",         384399e3,    27.321661*jd,   0.0549,    5.145_deg,    7.342e22,      1737.1e3,   3344 },
            { 4,   1,   "I",        "Phobos",       9376e3,      0.31891023*jd,  0.0151,    1.093_deg,    1.0659e16,     11.2667e3,  1779 },
            { 4,   2,   "II",       "Deimos",       23463.2e3,   1.263*jd,       0.00033,   0.93_deg,     1.4762e15,     6.2e3,      1500 },
            { 5,   1,   "XVI",      "Metis",        128852e3,    0.29880*jd,     0.0077,    2.226_deg,    3.6e16,        22e3,       810 },
            { 5,   2,   "XV",       "Adrastea",     129000e3,    0.30233*jd,     0.0063,    2.217_deg,    0.2e16,        8e3,        930 },
            { 5,   3,   "V",        "Amalthea",     181366e3,    0.50123*jd,     0.0075,    2.565_deg,    208e16,        84e3,       840 },
            { 5,   4,   "XIV",      "Thebe",        222452e3,    0.67780*jd,     0.0180,    2.909_deg,    43e16,         49e3,       870 },
            { 5,   5,   "I",        "Io",           421700e3,    1.7691*jd,      0.0041,    0.050_deg,    8931900e16,    1821.3e3,   3529 },
            { 5,   6,   "II",       "Europa",       671034e3,    3.5512*jd,      0.0094,    0.471_deg,    4800000e16,    1560.8e3,   3014 },
            { 5,   7,   "III",      "Ganymede",     1070412e3,   7.1546*jd,      0.0011,    0.204_deg,    14819000e16,   2631.2e3,   1942 },
            { 5,   8,   "IV",       "Callisto",     1882709e3,   16.689*jd,      0.0074,    0.205_deg,    10759000e16,   2410.3e3,   1834 },
            { 5,   9,   "XVIII",    "Themisto",     7393216e3,   129.87*jd,      0.2115,    45.762_deg,   0.069e16,      4e3,        2600 },
            { 5,   10,  "XIII",     "Leda",         11187781e3,  240.82*jd,      0.1673,    27.562_deg,   0.6e16,        8e3,        2800 },
            { 5,   11,  "VI",       "Himalia",      11451971e3,  250.23*jd,      0.1513,    30.486_deg,   670e16,        85e3,       2600 },
            { 5,   12,  "LXXI",     "S/2018 J 1",   11453004e3,  250.40*jd,      0.0944,    30.606_deg,   0.0015e16,     1e3,        3600 },
            { 5,   13,  "LXV",      "S/2017 J 4",   11494801e3,  251.77*jd,      0.1800,    28.155_deg,   0.0015e16,     1e3,        3600 },
            { 5,   14,  "X",        "Lysithea",     11740560e3,  259.89*jd,      0.1322,    27.006_deg,   6.3e16,        18e3,       2600 },
            { 5,   15,  "VII",      "Elara",        11778034e3,  259.64*jd,      0.1948,    29.691_deg,   87e16,         43e3,       2600 },
            { 5,   16,  "LIII",     "Dia",          12570424e3,  287.93*jd,      0.2058,    27.584_deg,   0.0090e16,     2e3,        2700 },
            { 5,   17,  "XLVI",     "Carpo",        17144873e3,  458.62*jd,      0.2735,    56.001_deg,   0.0045e16,     1.5e3,      3200 },
            { 5,   18,  "",         "S/2003 J 12",  17739539e3,  482.69*jd,      0.4449,    142.680_deg,  0.00015e16,    0.5e3,      2900 },
            { 5,   19,  "LXII",     "Valetudo",     18928095e3,  532.00*jd,      0.2219,    34.014_deg,   0.00015e16,    0.5e3,      3000 },
            { 5,   20,  "XXXIV",    "Euporie",      19088434e3,  538.78*jd,      0.0960,    144.694_deg,  0.0015e16,     1e3,        3600 },
            { 5,   21,  "LX",       "S/2003 J 3",   19621780e3,  561.52*jd,      0.2507,    146.363_deg,  0.0015e16,     1e3,        3600 },
            { 5,   22,  "LV",       "S/2003 J 18",  20219648e3,  587.38*jd,      0.1048,    146.376_deg,  0.0015e16,     1e3,        3600 },
            { 5,   23,  "LII",      "S/2010 J 2",   20307150e3,  588.36*jd,      0.307,     150.4_deg,    0.00015e16,    0.5e3,      3000 },
            { 5,   24,  "XLII",     "Thelxinoe",    20453753e3,  597.61*jd,      0.2684,    151.292_deg,  0.0015e16,     1e3,        3600 },
            { 5,   25,  "XXXIII",   "Euanthe",      20464854e3,  598.09*jd,      0.2000,    143.409_deg,  0.0045e16,     1.5e3,      3200 },
            { 5,   26,  "XLV",      "Helike",       20540266e3,  601.40*jd,      0.1374,    154.586_deg,  0.0090e16,     2e3,        2700 },
            { 5,   27,  "XXXV",     "Orthosie",     20567971e3,  602.62*jd,      0.2433,    142.366_deg,  0.0015e16,     1e3,        3600 },
            { 5,   28,  "LXVIII",   "S/2017 J 7",   20571458e3,  602.77*jd,      0.2147,    143.438_deg,  0.0015e16,     1e3,        3600 },
            { 5,   29,  "LIV",      "S/2016 J 1",   20595483e3,  603.83*jd,      0.1377,    139.839_deg,  0.0015e16,     1.5e3,      1100 },
            { 5,   30,  "LXIV",     "S/2017 J 3",   20639315e3,  605.76*jd,      0.1477,    147.915_deg,  0.0015e16,     1e3,        3600 },
            { 5,   31,  "XXIV",     "Iocaste",      20722566e3,  609.43*jd,      0.2874,    147.248_deg,  0.019e16,      2.5e3,      2900 },
            { 5,   32,  "",         "S/2003 J 16",  20743779e3,  610.36*jd,      0.3184,    150.769_deg,  0.0015e16,     1e3,        3600 },
            { 5,   33,  "XXVII",    "Praxidike",    20823948e3,  613.90*jd,      0.1840,    144.205_deg,  0.043e16,      3.5e3,      2300 },
            { 5,   34,  "XXII",     "Harpalyke",    21063814e3,  624.54*jd,      0.2440,    147.223_deg,  0.012e16,      2e3,        3600 },
            { 5,   35,  "XL",       "Mneme",        21129786e3,  627.48*jd,      0.3169,    149.732_deg,  0.0015e16,     1e3,        3600 },
            { 5,   36,  "XXX",      "Hermippe",     21182086e3,  629.81*jd,      0.2290,    151.242_deg,  0.0090e16,     2e3,        2700 },
            { 5,   37,  "XXIX",     "Thyone",       21405570e3,  639.80*jd,      0.2525,    147.276_deg,  0.0090e16,     2e3,        2700 },
            { 5,   38,  "LXX",      "S/2017 J 9",   21429955e3,  640.90*jd,      0.2288,    152.661_deg,  0.0015e16,     1e3,        3600 },
            { 5,   39,  "XII",      "Ananke",       21454952e3,  640.38*jd,      0.3445,    151.564_deg,  3.0e16,        14e3,       2600 },
            { 5,   40,  "L",        "Herse",        22134306e3,  672.75*jd,      0.2379,    162.490_deg,  0.0015e16,     1e3,        3600 },
            { 5,   41,  "XXXI",     "Aitne",        22285161e3,  679.64*jd,      0.3927,    165.562_deg,  0.0045e16,     1.5e3,      3200 },
            { 5,   42,  "LXVII",    "S/2017 J 6",   22394682e3,  684.66*jd,      0.5569,    155.185_deg,  0.0015e16,     1e3,        3600 },
            { 5,   43,  "LXXII",    "S/2011 J 1",   22401817e3,  694.98*jd,      0.2328,    163.341_deg,  0.00015e16,    0.5e3,      3000 },
            { 5,   44,  "XXXVII",   "Kale",         22409207e3,  685.32*jd,      0.2011,    165.378_deg,  0.0015e16,     1e3,        3600 },
            { 5,   45,  "XX",       "Taygete",      22438648e3,  686.67*jd,      0.3678,    164.890_deg,  0.016e16,      2.5e3,      2400 },
            { 5,   46,  "LXI",      "S/2003 J 19",  22696750e3,  698.55*jd,      0.2572,    166.657_deg,  0.0015e16,     1e3,        3600 },
            { 5,   47,  "XXI",      "Chaldene",     22713444e3,  699.33*jd,      0.2916,    167.070_deg,  0.0075e16,     2e3,        2200 },
            { 5,   48,  "LVIII",    "S/2003 J 15",  22720999e3,  699.68*jd,      0.0932,    141.812_deg,  0.0015e16,     1e3,        3600 },
            { 5,   49,  "",         "S/2003 J 10",  22730813e3,  700.13*jd,      0.3438,    163.813_deg,  0.0015e16,     1e3,        3600 },
            { 5,   50,  "",         "S/2003 J 23",  22739654e3,  700.54*jd,      0.3930,    148.849_deg,  0.0015e16,     1e3,        3600 },
            { 5,   51,  "XXV",      "Erinome",      22986266e3,  711.96*jd,      0.2552,    163.737_deg,  0.0045e16,     1.5e3,      3200 },
            { 5,   52,  "XLI",      "Aoede",        23044175e3,  714.66*jd,      0.4311,    160.482_deg,  0.0090e16,     2e3,        2700 },
            { 5,   53,  "XLIV",     "Kallichore",   23111823e3,  717.81*jd,      0.2041,    164.605_deg,  0.0015e16,     1e3,        3600 },
            { 5,   54,  "LXVI",     "S/2017 J 5",   23169389e3,  720.49*jd,      0.2842,    164.331_deg,  0.0015e16,     1e3,        3600 },
            { 5,   55,  "LXIX",     "S/2017 J 8",   23174446e3,  720.73*jd,      0.3118,    164.782_deg,  0.0015e16,     0.5e3,      2900 },
            { 5,   56,  "XXIII",    "Kalyke",       23180773e3,  721.02*jd,      0.2139,    165.505_deg,  0.019e16,      2.5e3,      2900 },
            { 5,   57,  "XI",       "Carme",        23197992e3,  702.28*jd,      0.2342,    165.047_deg,  13e16,         23e3,       2600 },
            { 5,   58,  "XVII",     "Callirrhoe",   23214986e3,  727.11*jd,      0.2582,    139.849_deg,  0.087e16,      4.5e3,      2300 },
            { 5,   59,  "XXXII",    "Eurydome",     23230858e3,  723.36*jd,      0.3769,    149.324_deg,  0.0045e16,     1.5e3,      3200 },
            { 5,   60,  "LXIII",    "S/2017 J 2",   23240957e3,  723.83*jd,      0.2360,    166.398_deg,  0.0015e16,     1e3,        3600 },
            { 5,   61,  "XXXVIII",  "Pasithee",     23307318e3,  726.93*jd,      0.3288,    165.759_deg,  0.0015e16,     1e3,        3600 },
            { 5,   62,  "LI",       "S/2010 J 1",   23314335e3,  724.34*jd,      0.320,     163.2_deg,    0.0015e16,     1e3,        3000 },
            { 5,   63,  "XLIX",     "Kore",         23345093e3,  723.72*jd,      0.1951,    137.371_deg,  0.0015e16,     1e3,        3600 },
            { 5,   64,  "XLVIII",   "Cyllene",      23396269e3,  731.10*jd,      0.4115,    140.148_deg,  0.0015e16,     1e3,        3600 },
            { 5,   65,  "LVI",      "S/2011 J 2",   23400981e3,  731.32*jd,      0.3321,    148.77_deg,   0.00015e16,    0.5e3,      3000 },
            { 5,   66,  "XLVII",    "Eukelade",     23483694e3,  735.20*jd,      0.2828,    163.996_deg,  0.0090e16,     2e3,        2700 },
            { 5,   67,  "LIX",      "S/2017 J 1",   23483978e3,  734.15*jd,      0.3969,    149.197_deg,  0.0015e16,     1e3,        3600 },
            { 5,   68,  "",         "S/2003 J 4",   23570790e3,  739.29*jd,      0.3003,    147.175_deg,  0.0015e16,     1e3,        3600 },
            { 5,   69,  "VIII",     "Pasiphae",     23609042e3,  739.80*jd,      0.3743,    141.803_deg,  30e16,         30e3,       2700 },
            { 5,   70,  "XXXIX",    "Hegemone",     23702511e3,  745.50*jd,      0.4077,    152.506_deg,  0.0045e16,     1.5e3,      3200 },
            { 5,   71,  "XLIII",    "Arche",        23717051e3,  746.19*jd,      0.1492,    164.587_deg,  0.0045e16,     1.5e3,      3200 },
            { 5,   72,  "XXVI",     "Isonoe",       23800647e3,  750.13*jd,      0.1775,    165.127_deg,  0.0075e16,     2e3,        2200 },
            { 5,   73,  "",         "S/2003 J 9",   23857808e3,  752.84*jd,      0.2761,    164.980_deg,  0.00015e16,    0.5e3,      2900 },
            { 5,   74,  "LVII",     "S/2003 J 5",   23973926e3,  758.34*jd,      0.3070,    165.549_deg,  0.0090e16,     2e3,        2700 },
            { 5,   75,  "IX",       "Sinope",       24057865e3,  739.33*jd,      0.2750,    153.778_deg,  7.5e16,        19e3,       2600 },
            { 5,   76,  "XXXVI",    "Sponde",       24252627e3,  771.60*jd,      0.4431,    154.372_deg,  0.0015e16,     1e3,        3600 },
            { 5,   77,  "XXVIII",   "Autonoe",      24264445e3,  772.17*jd,      0.3690,    151.058_deg,  0.0090e16,     2e3,        2700 },
            { 5,   78,  "XIX",      "Megaclite",    24687239e3,  792.44*jd,      0.3077,    150.398_deg,  0.021e16,      2.5e3,      3200 },
            { 5,   79,  "",         "S/2003 J 2",   28570410e3,  981.55*jd,      0.4074,    153.521_deg,  0.0015e16,     1e3,        3600 },
            { 6,   1,   "",         "S/2009 S 1",   117000e3,    0.47*jd,        0,         0_deg,        0.0001e15,     0.15e3,     7100 },
            { 6,   2,   "XVIII",    "Pan",          133584e3,    0.57505*jd,     0.000035,  0.001_deg,    4.95e15,       10.1e3,     1150 },
            { 6,   3,   "XXXV",     "Daphnis",      136505e3,    0.59408*jd,     0,         0_deg,        0.084e15,      3.8e3,      370 },
            { 6,   4,   "XV",       "Atlas",        137670e3,    0.60169*jd,     0.0012,    0.003_deg,    6.6e15,        15.1e3,     460 },
            { 6,   5,   "XVI",      "Prometheus",   139380e3,    0.61299*jd,     0.0022,    0.008_deg,    159.5e15,      43.1e3,     476 },
            { 6,   6,   "XVII",     "Pandora",      141720e3,    0.62850*jd,     0.0042,    0.050_deg,    137.1e15,      40.7e3,     485 },
            { 6,   7,   "XI",       "Epimetheus",   151422e3,    0.69433*jd,     0.0098,    0.335_deg,    526.6e15,      58.1e3,     641 },
            { 6,   8,   "X",        "Janus",        151472e3,    0.69466*jd,     0.0068,    0.165_deg,    1897.5e15,     89.5e3,     632 },
            { 6,   9,   "LIII",     "Aegaeon",      167500e3,    0.80812*jd,     0.0002,    0.001_deg,    0.0001e15,     0.25e3,     1500 },
            { 6,   10,  "I",        "Mimas",        185404e3,    0.942422*jd,    0.0202,    1.566_deg,    37493e15,      198.2e3,    1150 },
            { 6,   11,  "XXXII",    "Methone",      194440e3,    1.00957*jd,     0.0001,    0.007_deg,    0.02e15,       1.6e3,      1200 },
            { 6,   12,  "XLIX",     "Anthe",        197700e3,    1.05089*jd,     0.0011,    0.1_deg,      0.0015e15,     0.9e3,      490 },
            { 6,   13,  "XXXIII",   "Pallene",      212280e3,    1.15375*jd,     0.0040,    0.181_deg,    0.05e15,       2.5e3,      760 },
            { 6,   14,  "II",       "Enceladus",    237950e3,    1.370218*jd,    0.0047,    0.010_deg,    108022e15,     252.1e3,    1610 },
            { 6,   15,  "III",      "Tethys",       294619e3,    1.887802*jd,    0.0001,    0.168_deg,    617449e15,     531e3,      985 },
            { 6,   16,  "XIII",     "Telesto",      294619e3,    1.887802*jd,    0.000,     1.158_deg,    9.41e15,       12.4e3,     1180 },
            { 6,   17,  "XIV",      "Calypso",      294619e3,    1.887802*jd,    0.000,     1.473_deg,    6.3e15,        10.7e3,     1200 },
            { 6,   18,  "IV",       "Dione",        377396e3,    2.736915*jd,    0.0022,    0.002_deg,    1095452e15,    561.4e3,    1478 },
            { 6,   19,  "XII",      "Helene",       377396e3,    2.736915*jd,    0.0022,    0.212_deg,    24.46e15,      17.6e3,     1070 },
            { 6,   20,  "XXXIV",    "Polydeuces",   377396e3,    2.736915*jd,    0.0192,    0.177_deg,    0.03e15,       1.3e3,      3300 },
            { 6,   21,  "V",        "Rhea",         527108e3,    4.518212*jd,    0.001258,  0.327_deg,    2306518e15,    763.5e3,    1237 },
            { 6,   22,  "VI",       "Titan",        1221930e3,   15.94542*jd,    0.0288,    0.3485_deg,   134520000e15,  2574.73e3,  1881 },
            { 6,   23,  "VII",      "Hyperion",     1481010e3,   21.27661*jd,    0.123006,  0.568_deg,    5620e15,       135e3,      545 },
            { 6,   24,  "VIII",     "Iapetus",      3560820e3,   79.3215*jd,     0.028613,  15.47_deg,    1805635e15,    734.3e3,    1089 },
            { 6,   25,  "XXIV",     "Kiviuq",       11294800e3,  448.16*jd,      0.3288,    49.087_deg,   2.79e15,       8e3,        1300 },
            { 6,   26,  "XXII",     "Ijiraq",       11355316e3,  451.77*jd,      0.3161,    50.212_deg,   1.18e15,       6e3,        1300 },
            { 6,   27,  "IX",       "Phoebe",       12869700e3,  545.09*jd,      0.156242,  173.047_deg,  8292e15,       106.5e3,    1639 },
            { 6,   28,  "XX",       "Paaliaq",      15103400e3,  692.98*jd,      0.3631,    46.151_deg,   7.25e15,       11e3,       1300 },
            { 6,   29,  "XXVII",    "Skathi",       15672500e3,  732.52*jd,      0.246,     149.084_deg,  0.35e15,       4e3,        1300 },
            { 6,   30,  "XXVI",     "Albiorix",     16266700e3,  774.58*jd,      0.477,     38.042_deg,   22.3e15,       16e3,       1300 },
            { 6,   31,  "",         "S/2007 S 2",   16560000e3,  792.96*jd,      0.2418,    176.68_deg,   0.15e15,       3e3,        1300 },
            { 6,   32,  "XXXVII",   "Bebhionn",     17153520e3,  838.77*jd,      0.333,     40.484_deg,   0.15e15,       3e3,        1300 },
            { 6,   33,  "XXVIII",   "Erriapus",     17236900e3,  844.89*jd,      0.4724,    38.109_deg,   0.68e15,       5e3,        1300 },
            { 6,   34,  "XLVII",    "Skoll",        17473800e3,  862.37*jd,      0.418,     155.624_deg,  0.15e15,       3e3,        1300 },
            { 6,   35,  "XXIX",     "Siarnaq",      17776600e3,  884.88*jd,      0.24961,   45.798_deg,   43.5e15,       20e3,       1300 },
            { 6,   36,  "LII",      "Tarqeq",       17910600e3,  894.86*jd,      0.1081,    49.904_deg,   0.23e15,       3.5e3,      1300 },
            { 6,   37,  "",         "S/2004 S 13",  18056300e3,  905.85*jd,      0.261,     167.379_deg,  0.15e15,       3e3,        1300 },
            { 6,   38,  "LI",       "Greip",        18065700e3,  906.56*jd,      0.3735,    172.666_deg,  0.15e15,       3e3,        1300 },
            { 6,   39,  "XLIV",     "Hyrrokkin",    18168300e3,  914.29*jd,      0.3604,    153.272_deg,  0.35e15,       4e3,        1300 },
            { 6,   40,  "L",        "Jarnsaxa",     18556900e3,  943.78*jd,      0.1918,    162.861_deg,  0.15e15,       3e3,        1300 },
            { 6,   41,  "XXI",      "Tarvos",       18562800e3,  944.23*jd,      0.5305,    34.679_deg,   2.3e15,        7.5e3,      1300 },
            { 6,   42,  "XXV",      "Mundilfari",   18725800e3,  956.70*jd,      0.198,     169.378_deg,  0.23e15,       3.5e3,      1300 },
            { 6,   43,  "",         "S/2006 S 1",   18930200e3,  972.41*jd,      0.1303,    154.232_deg,  0.15e15,       3e3,        1300 },
            { 6,   44,  "",         "S/2004 S 17",  19099200e3,  985.45*jd,      0.226,     166.881_deg,  0.05e15,       2e3,        1500 },
            { 6,   45,  "XXXVIII",  "Bergelmir",    19104000e3,  985.83*jd,      0.152,     157.384_deg,  0.15e15,       3e3,        1300 },
            { 6,   46,  "XXXI",     "Narvi",        19395200e3,  1008.45*jd,     0.320,     137.292_deg,  0.23e15,       3.5e3,      1300 },
            { 6,   47,  "XXIII",    "Suttungr",     19579000e3,  1022.82*jd,     0.131,     174.321_deg,  0.23e15,       3.5e3,      1300 },
            { 6,   48,  "XLIII",    "Hati",         19709300e3,  1033.05*jd,     0.291,     163.131_deg,  0.15e15,       3e3,        1300 },
            { 6,   49,  "",         "S/2004 S 12",  19905900e3,  1048.54*jd,     0.396,     164.042_deg,  0.09e15,       2.5e3,      1400 },
            { 6,   50,  "XL",       "Farbauti",     19984800e3,  1054.78*jd,     0.209,     158.361_deg,  0.09e15,       2.5e3,      1400 },
            { 6,   51,  "XXX",      "Thrymr",       20278100e3,  1078.09*jd,     0.453,     174.524_deg,  0.23e15,       3.5e3,      1300 },
            { 6,   52,  "XXXVI",    "Aegir",        20482900e3,  1094.46*jd,     0.237,     167.425_deg,  0.15e15,       3e3,        1300 },
            { 6,   53,  "",         "S/2007 S 3",   20518500e3,  1100*jd,        0.130,     177.22_deg,   0.09e15,       2.5e3,      1400 },
            { 6,   54,  "XXXIX",    "Bestla",       20570000e3,  1101.45*jd,     0.5145,    147.395_deg,  0.23e15,       3.5e3,      1300 },
            { 6,   55,  "",         "S/2004 S 7",   20576700e3,  1101.99*jd,     0.5299,    165.596_deg,  0.15e15,       3e3,        1300 },
            { 6,   56,  "",         "S/2006 S 3",   21076300e3,  1142.37*jd,     0.4710,    150.817_deg,  0.15e15,       3e3,        1300 },
            { 6,   57,  "XLI",      "Fenrir",       21930644e3,  1212.53*jd,     0.131,     162.832_deg,  0.05e15,       2e3,        1500 },
            { 6,   58,  "XLVIII",   "Surtur",       22288916e3,  1242.36*jd,     0.3680,    166.918_deg,  0.15e15,       3e3,        1300 },
            { 6,   59,  "XLV",      "Kari",         22321200e3,  1245.06*jd,     0.3405,    148.384_deg,  0.23e15,       3.5e3,      1300 },
            { 6,   60,  "XIX",      "Ymir",         22429673e3,  1254.15*jd,     0.3349,    172.143_deg,  3.97e15,       9e3,        1300 },
            { 6,   61,  "XLVI",     "Loge",         22984322e3,  1300.95*jd,     0.1390,    166.539_deg,  0.15e15,       3e3,        1300 },
            { 6,   62,  "XLII",     "Fornjot",      24504879e3,  1432.16*jd,     0.186,     167.886_deg,  0.15e15,       3e3,        1300 },
            { 7,   1,   "VI",       "Cordelia",     49770e3,     0.335034*jd,    0.00026,   0.08479_deg,  0.044e18,      20e3,       1300 },
            { 7,   2,   "VII",      "Ophelia",      53790e3,     0.376400*jd,    0.00992,   0.1036_deg,   0.053e18,      22e3,       1200 },
            { 7,   3,   "VIII",     "Bianca",       59170e3,     0.434579*jd,    0.00092,   0.193_deg,    0.092e18,      26e3,       1200 },
            { 7,   4,   "IX",       "Cressida",     61780e3,     0.463570*jd,    0.00036,   0.006_deg,    0.34e18,       40e3,       1300 },
            { 7,   5,   "X",        "Desdemona",    62680e3,     0.473650*jd,    0.00013,   0.11125_deg,  0.18e18,       32e3,       1300 },
            { 7,   6,   "XI",       "Juliet",       64350e3,     0.493065*jd,    0.00066,   0.065_deg,    0.56e18,       47e3,       1300 },
            { 7,   7,   "XII",      "Portia",       66090e3,     0.513196*jd,    0.00005,   0.059_deg,    1.70e18,       68e3,       1300 },
            { 7,   8,   "XIII",     "Rosalind",     69940e3,     0.558460*jd,    0.00011,   0.279_deg,    0.25e18,       36e3,       1300 },
            { 7,   9,   "XXVII",    "Cupid",        74800e3,     0.618*jd,       0.0013,    0.1_deg,      0.0038e18,     9e3,        1200 },
            { 7,   10,  "XIV",      "Belinda",      75260e3,     0.623527*jd,    0.00007,   0.031_deg,    0.49e18,       45e3,       1300 },
            { 7,   11,  "XXV",      "Perdita",      76400e3,     0.638*jd,       0.0012,    0.0_deg,      0.018e18,      15e3,       1300 },
            { 7,   12,  "XV",       "Puck",         86010e3,     0.761833*jd,    0.00012,   0.3192_deg,   2.90e18,       81e3,       1300 },
            { 7,   13,  "XXVI",     "Mab",          97700e3,     0.923*jd,       0.0025,    0.1335_deg,   0.01e18,       13e3,       1100 },
            { 7,   14,  "V",        "Miranda",      129390e3,    1.413479*jd,    0.0013,    4.232_deg,    65.9e18,       235.8e3,    1200 },
            { 7,   15,  "I",        "Ariel",        191020e3,    2.520379*jd,    0.0012,    0.260_deg,    1353e18,       578.9e3,    1665 },
            { 7,   16,  "II",       "Umbriel",      266300e3,    4.144177*jd,    0.0039,    0.205_deg,    1172e18,       584.7e3,    1400 },
            { 7,   17,  "III",      "Titania",      435910e3,    8.705872*jd,    0.0011,    0.340_deg,    3527e18,       788.4e3,    1718 },
            { 7,   18,  "IV",       "Oberon",       583520e3,    13.463239*jd,   0.0014,    0.058_deg,    3014e18,       761.4e3,    1630 },
            { 7,   19,  "XXII",     "Francisco",    4276000e3,   266.56*jd,      0.1459,    147.459_deg,  0.0072e18,     11e3,       1300 },
            { 7,   20,  "XVI",      "Caliban",      7230000e3,   579.50*jd,      0.1587,    139.885_deg,  0.25e18,       36e3,       1300 },
            { 7,   21,  "XX",       "Stephano",     8002000e3,   676.50*jd,      0.2292,    141.873_deg,  0.022e18,      16e3,       1300 },
            { 7,   22,  "XXI",      "Trinculo",     8571000e3,   758.10*jd,      0.2200,    166.252_deg,  0.0039e18,     9e3,        1300 },
            { 7,   23,  "XVII",     "Sycorax",      12179000e3,  1283.4*jd,      0.5224,    152.456_deg,  2.30e18,       83e3,       960 },
            { 7,   24,  "XXIII",    "Margaret",     14345000e3,  1694.8*jd,      0.6608,    51.455_deg,   0.0054e18,     10e3,       1300 },
            { 7,   25,  "XVIII",    "Prospero",     16418000e3,  1992.8*jd,      0.4448,    146.017_deg,  0.085e18,      25e3,       1300 },
            { 7,   26,  "XIX",      "Setebos",      17459000e3,  2202.3*jd,      0.5914,    145.883_deg,  0.075e18,      24e3,       1300 },
            { 7,   27,  "XXIV",     "Ferdinand",    20900000e3,  2823.4*jd,      0.3682,    167.346_deg,  0.0054e18,     10e3,       1300 },
            { 8,   1,   "III",      "Naiad",        48227e3,     0.294*jd,       0.0003,    4.691_deg,    19e16,         33e3,       1300 },
            { 8,   2,   "IV",       "Thalassa",     50074e3,     0.311*jd,       0.0002,    0.135_deg,    35e16,         41e3,       1200 },
            { 8,   3,   "V",        "Despina",      52526e3,     0.335*jd,       0.0002,    0.068_deg,    210e16,        75e3,       1200 },
            { 8,   4,   "VI",       "Galatea",      61953e3,     0.429*jd,       0.0001,    0.034_deg,    375e16,        88e3,       1300 },
            { 8,   5,   "VII",      "Larissa",      73548e3,     0.555*jd,       0.0014,    0.205_deg,    495e16,        97e3,       1300 },
            { 8,   6,   "XIV",      "S/2004 N 1",   105300e3,    0.936*jd,       0.0000,    0.000_deg,    0.5e16,        9e3,        1600 },
            { 8,   7,   "VIII",     "Proteus",      117646e3,    1.122*jd,       0.0005,    0.075_deg,    5035e16,       210e3,      1300 },
            { 8,   8,   "I",        "Triton",       354759e3,    5.877*jd,       0.0000,    156.865_deg,  2140800e16,    1352.6e3,   2065 },
            { 8,   9,   "II",       "Nereid",       5513818e3,   360.13*jd,      0.7507,    7.090_deg,    2700e16,       170e3,      1300 },
            { 8,   10,  "IX",       "Halimede",     16611000e3,  1879.08*jd,     0.2646,    134.1_deg,    16e16,         31e3,       1300 },
            { 8,   11,  "XI",       "Sao",          22228000e3,  2912.72*jd,     0.1365,    49.907_deg,   6e16,          22e3,       1300 },
            { 8,   12,  "XII",      "Laomedeia",    23567000e3,  3171.33*jd,     0.3969,    34.049_deg,   5e16,          21e3,       1300 },
            { 8,   13,  "X",        "Psamathe",     48096000e3,  9074.30*jd,     0.3809,    137.679_deg,  4e16,          20e3,       1200 },
            { 8,   14,  "XIII",     "Neso",         49285000e3,  9740.73*jd,     0.5714,    131.265_deg,  15e16,         30e3,       1300 },

        };
        return table;
    }

    // Stellar property relations

    double magnitude_to_ratio(double delta_mag) noexcept {
        return std::pow(10.0, -0.4 * delta_mag);
    }

    double ratio_to_magnitude(double ratio) noexcept {
        return -2.5 * std::log10(ratio);
    }

    double magnitude_to_luminosity(double M_bol) noexcept {
        return magnitude_to_ratio(M_bol - Sun.M_bol);
    }

    double luminosity_to_magnitude(double L) noexcept {
        return Sun.M_bol + ratio_to_magnitude(L);
    }

    double magnitude_to_visual_luminosity(double M_V) noexcept {
        return magnitude_to_ratio(M_V - Sun.M_V);
    }

    double visual_luminosity_to_magnitude(double L_V) noexcept {
        return Sun.M_V + ratio_to_magnitude(L_V);
    }

    double luminosity_radius_to_temperature(double lum, double rad) noexcept {
        return std::pow(lum / (4.0 * pi * sigma * rad * rad), 0.25);
    }

    double luminosity_temperature_to_radius(double lum, double temp) noexcept {
        return std::sqrt(lum / (4.0 * pi * sigma * std::pow(temp, 4.0)));
    }

    double radius_temperature_to_luminosity(double rad, double temp) noexcept {
        return 4.0 * pi * sigma * rad * rad * std::pow(temp, 4.0);
    }

    double solar_luminosity_radius_to_temperature(double lum, double rad) noexcept {
        return luminosity_radius_to_temperature(lum * solar_luminosity, rad * solar_radius);
    }

    double solar_luminosity_temperature_to_radius(double lum, double temp) noexcept {
        return luminosity_temperature_to_radius(lum * solar_luminosity, temp) / solar_radius;
    }

    double solar_radius_temperature_to_luminosity(double rad, double temp) noexcept {
        return radius_temperature_to_luminosity(rad * solar_radius, temp) / solar_luminosity;
    }

    // Spectral classification

    Spectrum::Spectrum(Uview str) {
        if (! try_parse(str))
            throw std::invalid_argument("Invalid spectral class: " + quote(str));
    }

    Ustring Spectrum::str() const {
        Ustring text;
        if (sp == Sp::WNh)
            text = "WN" + std::to_string(sub) + "h";
        else
            text = unqualify(to_str(sp)) + std::to_string(sub);
        if (sp >= Sp::O && sp <= Sp::M && lc != LC::none)
            text += ' ' + unqualify(to_str(lc));
        return text;
    }

    bool Spectrum::try_parse(Uview str) noexcept {
        try {
            static const Regex pattern(R"(
                ( c | s?[dg] )?  # [1] luminosity prefix
                (                # [2] class
                    [OBAFGKMLTY]
                    | D [ABCOQXZ] [A-Z]*
                    | W [NCO]
                    | [CS]
                )
                ( \d+ [.\d]* )?  # [3] subclass
                ( [a-z]* )       # [4] class suffix
                \s* ([IVab]+)?   # [5] luminosity suffix
            )", Regex::extended | Regex::full | Regex::optimize);
            auto match = pattern(str);
            if (! match)
                return false;
            LC l = LC::none;
            Sp s = Sp::none;
            auto spstr = match[2].substr(0, 2);
            if (! str_to_enum(spstr, s))
                return false;
            if (s == Sp::WN && match[4] == "h")
                s = Sp::WNh;
            uint8_t n = 5;
            if (match.matched(3))
                n = uint8_t(decnum(match[3]));
            if (s >= Sp::O && s <= Sp::M) {
                l = LC::V;
                if (match.matched(5)) {
                    if (! str_to_enum(match[5], l))
                        return false;
                } else if (match.matched(1)) {
                    auto prefix = match[1];
                    if (prefix == "c")        l = LC::Ia;
                    else if (prefix == "g")   l = LC::III;
                    else if (prefix == "sg")  l = LC::IV;
                    else if (prefix == "sd")  l = LC::VI;
                }
            } else if (s >= Sp::DA && s <= Sp::DZ) {
                l = LC::VII;
            }
            lc = l;
            sp = s;
            sub = n;
            return true;
        }
        catch (...) {
            return false;
        }
    }

    // Hertzsprung-Russell diagram

    namespace {

        using DataTable = std::vector<std::vector<double>>;

        template <typename Table>
        Table collect_table(const DataTable& source, size_t column) {
            Table result;
            for (auto& row: source)
                if (! std::isnan(row[column]))
                    result.insert(row[0], row[column]);
            return result;
        }

        double interpolate_lin_log(double x1, double y1, double x2, double y2, double x) noexcept {
            double ly1 = std::log(y1);
            double ly2 = std::log(y2);
            double ly = interpolate(x1, ly1, x2, ly2, x);
            return std::exp(ly);
        }

        ClassData query_main_sequence(const Spectrum& spc) {

            static const DataTable aq_data = {
                // index  V_M    V_MV    III_M  III_MV  I_M    I_MV   T       BV      BC     Sp
                // 0      1      2       3      4       5      6      7       8       9      --
                { 3,      120,   nan,    nan,   nan,    nan,   nan,   nan,    nan,    nan    }, // O3
                { 5,      60,    -5.7,   nan,   nan,    70,    nan,   42000,  -0.33,  -4.40  }, // O5
                { 6,      37,    nan,    nan,   nan,    40,    nan,   nan,    nan,    nan    }, // O6
                { 8,      23,    nan,    nan,   nan,    28,    nan,   nan,    nan,    nan    }, // O8
                { 9,      nan,   -4.5,   nan,   nan,    nan,   -6.5,  34000,  -0.31,  -3.33  }, // O9
                { 10,     17.5,  -4.0,   20,    nan,    25,    nan,   30000,  -0.30,  -3.16  }, // B0
                { 12,     nan,   -2.45,  nan,   nan,    nan,   -6.4,  20900,  -0.24,  -2.35  }, // B2
                { 13,     7.6,   nan,    nan,   nan,    nan,   nan,   nan,    nan,    nan    }, // B3
                { 15,     5.9,   -1.2,   7,     nan,    20,    -6.2,  15200,  -0.17,  -1.46  }, // B5
                { 18,     3.8,   -0.25,  nan,   nan,    nan,   -6.2,  11400,  -0.11,  -0.80  }, // B8
                { 20,     2.9,   +0.65,  4,     nan,    16,    -6.3,  9790,   -0.02,  -0.30  }, // A0
                { 22,     nan,   +1.3,   nan,   nan,    nan,   -6.5,  9000,   +0.05,  -0.20  }, // A2
                { 25,     2.0,   +1.95,  nan,   nan,    13,    -6.6,  8180,   +0.15,  -0.15  }, // A5
                { 30,     1.6,   +2.7,   nan,   nan,    12,    -6.6,  7300,   +0.30,  -0.09  }, // F0
                { 32,     nan,   +3.6,   nan,   nan,    nan,   -6.6,  7000,   +0.35,  -0.11  }, // F2
                { 35,     1.4,   +3.5,   nan,   nan,    10,    -6.6,  6650,   +0.44,  -0.14  }, // F5
                { 38,     nan,   +4.0,   nan,   nan,    nan,   -6.5,  6250,   +0.52,  -0.16  }, // F8
                { 40,     1.05,  +4.4,   1.1,   nan,    10,    -6.4,  5940,   +0.58,  -0.18  }, // G0
                { 42,     nan,   +4.7,   nan,   nan,    nan,   -6.3,  5790,   +0.63,  -0.20  }, // G2
                { 45,     0.92,  +5.1,   1.1,   +0.9,   12,    -6.2,  5560,   +0.68,  -0.21  }, // G5
                { 48,     nan,   +5.5,   nan,   +0.8,   nan,   -6.1,  5310,   +0.74,  -0.40  }, // G8
                { 50,     0.79,  +5.9,   1.1,   +0.7,   13,    -6.0,  5150,   +0.81,  -0.31  }, // K0
                { 52,     nan,   +6.4,   nan,   +0.5,   nan,   -5.9,  4830,   +0.91,  -0.42  }, // K2
                { 55,     0.67,  +7.35,  1.2,   -0.2,   13,    -5.8,  4410,   +1.15,  -0.72  }, // K5
                { 60,     0.51,  +8.8,   1.2,   -0.4,   13,    -5.6,  3840,   +1.40,  -1.38  }, // M0
                { 62,     0.40,  +9.9,   nan,   -0.6,   19,    -5.6,  3520,   +1.49,  -1.89  }, // M2
                { 65,     0.21,  +12.3,  nan,   -0.3,   nan,   -5.6,  3170,   +1.64,  -2.73  }, // M5
                { 68,     0.06,  nan,    nan,   nan,    nan,   nan,   nan,    nan,    nan    }, // M8
            };

            static const auto V_M_table = collect_table<InterpolationTable<false, true>>(aq_data, 1);
            static const auto V_MV_table = collect_table<InterpolationTable<false, false>>(aq_data, 2);
            static const auto III_M_table = collect_table<InterpolationTable<false, true>>(aq_data, 3);
            static const auto III_MV_table = collect_table<InterpolationTable<false, false>>(aq_data, 4);
            static const auto I_M_table = collect_table<InterpolationTable<false, true>>(aq_data, 5);
            static const auto I_MV_table = collect_table<InterpolationTable<false, false>>(aq_data, 6);
            static const auto T_table = collect_table<InterpolationTable<false, true>>(aq_data, 7);
            static const auto BV_table = collect_table<InterpolationTable<false, false>>(aq_data, 8);
            static const auto BC_table = collect_table<InterpolationTable<false, false>>(aq_data, 9);

            ClassData cld;
            cld.spectrum = spc;
            int index = std::clamp(spc.index(), 3, 68);

            if (spc.lc == LC::Ia) {

                cld.mass = I_M_table(index);
                cld.M_V = I_MV_table(index);

            } else if (spc.lc >= LC::V || spc.lc == LC::none) {

                cld.mass = V_M_table(index);
                cld.M_V = V_MV_table(index);
                if (spc.lc == LC::VI)
                    cld.M_V += 1.75;

            } else {

                double m1 = I_M_table(index);
                double v1 = I_MV_table(index);
                double m5 = V_M_table(index);
                double v5 = V_MV_table(index);
                double m3, v3;
                if (index < 10)
                    m3 = interpolate_lin_log(0.0, m1, 5.0, m5, 3.0);
                else
                    m3 = III_M_table(index);
                if (index < 45)
                    v3 = interpolate(0.0, v1, 5.0, v5, 3.0);
                else
                    v3 = III_MV_table(index);

                switch (spc.lc) {
                    case LC::Ib:
                        cld.mass = interpolate_lin_log(0.0, m1, 3.0, m3, 1.0);
                        cld.M_V = interpolate(0.0, v1, 3.0, v3, 1.0);
                        break;
                    case LC::II:
                        cld.mass = interpolate_lin_log(0.0, m1, 3.0, m3, 2.0);
                        cld.M_V = interpolate(0.0, v1, 3.0, v3, 2.0);
                        break;
                    case LC::III:
                        cld.mass = m3;
                        cld.M_V = v3;
                        break;
                    case LC::IV:
                        if (index < 30)
                            cld.mass = interpolate_lin_log(3.0, m3, 5.0, m5, 4.0);
                        else
                            cld.mass = 0.9 * m3;
                        if (index < 50)
                            cld.M_V = interpolate(3.0, v3, 5.0, v5, 4.0);
                        else
                            cld.M_V = v3 + 2.6;
                        break;
                    default:
                        break;
                }

            }

            cld.temperature = T_table(index);
            cld.B_V = BV_table(index);
            cld.BC = BC_table(index);
            cld.M_bol = cld.M_V + cld.BC;
            cld.luminosity = magnitude_to_luminosity(cld.M_bol);
            cld.radius = solar_luminosity_temperature_to_radius(cld.luminosity, cld.temperature);

            return cld;

        }

        ClassData query_brown_dwarf(const Spectrum& spc) {
            ClassData cld;
            cld.spectrum = spc;
            int n = spc.sub;
            switch (spc.sp) {
                case Sp::L:
                    cld.temperature = 2220 - 100 * n;
                    cld.B_V = 2.5 + 0.25 * n;
                    cld.BC = -3.8 - 0.8 * n;
                    break;
                case Sp::T:
                    cld.temperature = 1220 - 50 * n;
                    cld.B_V = 5 + 0.4 * n;
                    cld.BC = -11.8 - 1.4 * n;
                    break;
                case Sp::Y:
                    cld.temperature = 720 - 25 * n;
                    cld.B_V = 9 + 0.4 * n;
                    cld.BC = -25.8 - 1.4 * n;
                    break;
                default:
                    break;
            }
            cld.mass = std::pow(cld.temperature / 17000, 1.2); // age ~= 5 in Burrows & Liebert relation
            cld.radius = 0.1;
            cld.luminosity = solar_radius_temperature_to_luminosity(cld.radius, cld.temperature);
            cld.M_bol = luminosity_to_magnitude(cld.luminosity);
            cld.M_V = cld.M_bol - cld.BC;
            return cld;
        }

        ClassData query_white_dwarf(const Spectrum& spc) {
            ClassData cld;
            cld.spectrum = spc;
            int n = std::max(int(spc.sub), 1);
            cld.mass = 0.6;
            cld.radius = 0.013;
            cld.temperature = 50400.0 / n;
            if (n <= 5)
                cld.B_V = 0.08 * n - 0.42;
            else
                cld.B_V = 0.16 * n - 0.82;
            if (n <= 3)
                cld.BC = -0.75 * n * n + 4.75 * n - 9.25;
            else
                cld.BC = -0.05 * n * n + 0.8 * n - 3.25;
            cld.luminosity = solar_radius_temperature_to_luminosity(cld.radius, cld.temperature);
            cld.M_bol = luminosity_to_magnitude(cld.luminosity);
            cld.M_V = cld.M_bol - cld.BC;
            return cld;
        }

        ClassData query_wolf_rayet(const Spectrum& spc) {

            struct wr_data {
                double temperature;
                double mass;
                double luminosity;
            };

            static const std::map<Sp, std::map<int, wr_data>> wr_table = {
                { Sp::WC, {
                    { 4,  { 117'000,  10,  158'000 }},
                    { 5,  { 83'000,   12,  398'000 }},
                    { 6,  { 78'000,   14,  501'000 }},
                    { 7,  { 71'000,   11,  398'000 }},
                    { 8,  { 60'000,   11,  398'000 }},
                    { 9,  { 44'000,   10,  251'000 }},
                }},
                { Sp::WN, {
                    { 2,  { 141'000,  16,  280'000 }},
                    { 3,  { 85'000,   19,  220'000 }},
                    { 4,  { 70'000,   15,  200'000 }},
                    { 5,  { 60'000,   15,  160'000 }},
                    { 6,  { 56'000,   18,  160'000 }},
                    { 7,  { 50'000,   21,  350'000 }},
                }},
                { Sp::WNh, {
                    { 5,  { 50'000,  200,  5'000'000 }},
                    { 6,  { 45'000,  74,   3'300'000 }},
                    { 7,  { 45'000,  52,   2'000'000 }},
                    { 8,  { 40'000,  39,   1'300'000 }},
                    { 9,  { 35'000,  33,   940'000 }},
                }},
                { Sp::WO, {
                    { 2,  { 200'000,  19,  630'000 }},
                }},
            };

            ClassData cld;
            cld.spectrum = spc;
            auto& subtable = wr_table.find(spc.sp)->second;
            auto it = subtable.lower_bound(int(spc.sub));
            if (it == subtable.end())
                --it;
            cld.mass = it->second.mass;
            cld.luminosity = it->second.luminosity;
            cld.temperature = it->second.temperature;
            cld.B_V = 0.6 - 0.2 * std::log10(cld.temperature);
            cld.BC = 33.75 - 8.25 * std::log10(cld.temperature);
            cld.M_bol = luminosity_to_magnitude(cld.luminosity);
            cld.M_V = cld.M_bol - cld.BC;
            cld.radius = solar_luminosity_temperature_to_radius(cld.luminosity, cld.temperature);
            return cld;

        }

        ClassData query_carbon_star(const Spectrum& spc) {
            int n = spc.sub;
            Spectrum ms;
            ms.lc = LC::II;
            if (spc.sp == Sp::S) {
                ms.sp = Sp::M;
                ms.sub = uint8_t(n);
            } else if (n <= 2) {
                ms.sp = Sp::G;
                ms.sub = uint8_t(2 * n + 5);
            } else if (n <= 5) {
                ms.sp = Sp::K;
                ms.sub = uint8_t(3 * n - 8);
            } else {
                ms.sp = Sp::M;
                ms.sub = uint8_t(2 * n - 11);
            }
            auto cld = query_main_sequence(ms);
            cld.spectrum = spc;
            return cld;
        }

        ClassData query_class_data(const Spectrum& spc) {
            ClassData cld;
            if (is_main_sequence(spc.sp))
                cld = query_main_sequence(spc);
            else if (is_brown_dwarf(spc.sp))
                cld = query_brown_dwarf(spc);
            else if (is_white_dwarf(spc.sp))
                cld = query_white_dwarf(spc);
            else if (is_wolf_rayet(spc.sp))
                cld = query_wolf_rayet(spc);
            else if (is_carbon_star(spc.sp))
                cld = query_carbon_star(spc);
            else
                cld.spectrum = spc;
            return cld;
        }

    }

    ClassData::ClassData(const Spectrum& spc) {
        static std::unordered_map<Spectrum, ClassData> cache;
        static std::shared_mutex mutex;
        {
            auto lock = make_shared_lock(mutex);
            auto it = cache.find(spc);
            if (it != cache.end()) {
                *this = it->second;
                return;
            }
        }
        auto lock = make_lock(mutex);
        auto it = cache.find(spc);
        if (it == cache.end())
            *this = cache[spc] = query_class_data(spc);
        else
            *this = it->second;
    }

}
