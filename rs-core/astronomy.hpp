#pragma once

#include "rs-core/common.hpp"
#include "rs-core/float.hpp"
#include <cmath>
#include <cstdlib>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <ostream>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS::Astro {

    // Physical constants

    #define RS_CONSTEXPR_SIGMA (2 * c_pow(pi, 5) * c_pow(k_B, 4) / (15 * c_pow(h, 3) * c_pow(c, 2)))

    constexpr double m_u        = 1.660'539'040e-27,   atomic_mass_unit           = m_u;        // kg              Best available value
    constexpr double N_A        = 6.022'140'76e23,     avogadro_constant          = N_A;        // mol^-1          Exact value fixed by SI 2019
    constexpr double k_B        = 1.380'649e-23,       boltzmann_constant         = k_B;        // J K^-1          Exact value fixed by SI 2019
    constexpr double m_e        = 9.109'383'56e-31,    electron_mass              = m_e;        // kg              Best available value
    constexpr double ec         = 1.602'176'634e-19,   elementary_charge          = ec;         // C               Exact value fixed by SI 2019
    constexpr double G          = 6.674'08e-11,        gravitational_constant     = G;          // m^3 kg^-1 s^-2  Best available value
    constexpr double mu_0       = 1.256'637'061e-6,    magnetic_constant          = mu_0;       // N A^-2          Best available value
    constexpr double h          = 6.626'070'15e-34,    planck_constant            = h;          // J s             Exact value fixed by SI 2019
    constexpr double R_inf      = 10'973'731.568'508,  rydberg_constant           = R_inf;      // m^-1            Best available value
    constexpr double c          = 299'792'458.0,       speed_of_light             = c;          // m s^-1          Exact value fixed by SI 2019
    constexpr double Z_0        = 376.730'313'461'77,  vacuum_impedance           = Z_0;        // ohm             Best available value
    constexpr double epsilon_0  = 8.854'187'817e-12,   vacuum_permittivity        = epsilon_0;  // F m^-1          Best available value
    constexpr double F          = ec * N_A,            faraday_constant           = F;          // C mol^-1        Exact value derived from SI 2019
    constexpr double K_J        = 2 * ec / h,          josephson_constant         = K_J;        // Hz/V            Exact value derived from SI 2019
    constexpr double R          = k_B * N_A,           molar_gas_constant         = R;          // J mol^-1 K^-1   Exact value derived from SI 2019
    constexpr double M_u        = m_u * N_A,           molar_mass_constant        = M_u;        // kg mol^-1       Best available value
    constexpr double sigma      = RS_CONSTEXPR_SIGMA,  stefan_boltzmann_constant  = sigma;      // W m^-2 K^-4     Exact value derived from SI 2019
    constexpr double R_K        = h / (ec * ec),       von_klitzing_constant      = R_K;        // ohm             Exact value derived from SI 2019
    constexpr double c1_L       = 2 * h * c * c,       first_radiation_constant   = c1_L;       // m^4 kg s        Exact value derived from SI 2019
    constexpr double c2         = h * c / k_B,         second_radiation_constant  = c2;         // m K             Exact value derived from SI 2019
    constexpr double b_W        = 2.897'772'9e-3,      wien_constant              = b_W;        // m K             Best available value

    // Astronomical constants

    constexpr double d_sid    = 86'164.09053,         sidereal_day                    = d_sid;    // s       Best available value
    constexpr double jd       = 86'400.0,             julian_day                      = jd;       // s       Exact by definition
    constexpr double y_trop   = 31'556'925.2,         tropical_year                   = y_trop;   // s       Best available value
    constexpr double jy       = 365.25l * jd,         julian_year                     = jy;       // s       Exact by definition
    constexpr double y_sid    = 31'558'149.8,         sidereal_year                   = y_sid;    // s       Best available value
    constexpr double R_earth  = 6.371'0e6,            earth_radius                    = R_earth;  // m       Best available value
    constexpr double R_jup    = 6.991'1e7,            jupiter_radius                  = R_jup;    // m       Best available value
    constexpr double R_sun    = 6.957e8,              solar_radius                    = R_sun;    // m       IAU adopted value
    constexpr double au       = 149'597'870'700,      astronomical_unit               = au;       // m       IAU adopted value
    constexpr double ly       = c * jy,               light_year                      = ly;       // m       Exact value derived from SI
    constexpr double pc       = 3.085'677'581'49e16,  parsec                          = pc;       // m       Best available value
    constexpr double M_earth  = 5.972'4e24,           earth_mass                      = M_earth;  // kg      Best available value
    constexpr double M_jup    = 1.898'2e27,           jupiter_mass                    = M_jup;    // kg      Best available value
    constexpr double M_sun    = 1.988'48e30,          solar_mass                      = M_sun;    // kg      Best available value
    constexpr double g_earth  = 9.806'65,             earth_gravity                   = g_earth;  // m s^-2  Standard adopted value
    constexpr double L_0      = 3.012'8e28,           bolometric_luminosity_constant  = L_0;      // W       IAU adopted value
    constexpr double L_sun    = 3.828e26,             solar_luminosity                = L_sun;    // W       IAU adopted value
    constexpr double T_sun    = 5772.0,               solar_temperature               = T_sun;    // K       IAU adopted value

    // Blackbody radiation

    class BlackbodySpectrum {
    public:
        BlackbodySpectrum() noexcept: temp_(solar_temperature) {}
        explicit BlackbodySpectrum(double temp) noexcept: temp_(temp) {}
        double peak() const noexcept;
        double power() const noexcept;
        double operator()(double lambda) const noexcept;
    private:
        double temp_;
    };

    // Coordinate systems

    constexpr double degrees_to_radians(double deg, double min = 0, double sec = 0) noexcept { return (deg + min / 60.0 + sec / 3600) * pi / 180; }
    constexpr double hours_to_radians(double hrs, double min = 0, double sec = 0) noexcept { return (hrs + min / 60.0 + sec / 3600) * pi / 12; }
    constexpr double radians_to_degrees(double rad) noexcept { return rad * 180 / pi; }
    void radians_to_degrees(double rad, int& deg, double& min) noexcept;
    void radians_to_degrees(double rad, int& deg, int& min, double& sec) noexcept;
    constexpr double radians_to_hours(double rad) noexcept { return rad * 12 / pi; }
    void radians_to_hours(double rad, int& hrs, double& min) noexcept;
    void radians_to_hours(double rad, int& hrs, int& min, double& sec) noexcept;
    Ustring format_angle(double rad, Uview fmt = "dms");
    double parse_angle(const Ustring& str, Uview fmt = {});

    // Solar system data

    struct SunData {
        double mass         = solar_mass;         // mass                           (kg)
        double luminosity   = solar_luminosity;   // luminosity                     (W)
        double radius       = solar_radius;       // radius                         (m)
        double temperature  = solar_temperature;  // temperature                    (K)
        double V            = -26.75;             // apparent visual magnitude      (mag)
        double M_U          = 5.66;               // absolute U magnitude           (mag)
        double M_B          = 5.47;               // absolute B magnitude           (mag)
        double M_V          = 4.82;               // absolute V magnitude           (mag)
        double M_R          = 4.28;               // absolute R magnitude           (mag)
        double M_I          = 3.94;               // absolute I magnitude           (mag)
        double M_bol        = 4.74;               // absolute bolometric magnitude  (mag)
        double BC           = M_bol - M_V;        // bolometric correction          (mag)
        double B_V          = M_B - M_V;          // colour index                   (mag)
    };

    constexpr SunData Sun = {};

    RS_ENUM(Planet, int, 1,
        Mercury,
        Venus,
        Earth,
        Mars,
        Jupiter,
        Saturn,
        Uranus,
        Neptune
    );

    struct PlanetData {
        Ustring name;              // name                      --
        double distance;           // semi-major axis           (m)
        double period;             // orbital period            (s)
        double eccentriciy;        // orbital eccentricity      --
        double inclination;        // orbital inclination       (rad)
        double mass;               // mass                      (kg)
        double radius;             // mean radius               (m)
        double equatorial_radius;  // equatorial radius         (m)
        double polar_radius;       // polar radius              (m)
        double density;            // mean density              (kg m^-3)
        double rotation;           // sidereal rotation period  (s)
        double axial_inclination;  // axial inclination         (rad)
        int satellites;            // satellites                --
    };

    const std::vector<PlanetData>& planet_table();
    inline const std::vector<PlanetData>& planets = planet_table();

    struct DwarfPlanetData {
        int number;          // minor planet number       --
        Ustring name;        // name                      --
        double distance;     // semi-major axis           (m)
        double period;       // orbital period            (s)
        double eccentriciy;  // orbital eccentricity      --
        double inclination;  // orbital inclination       (rad)
        double mass;         // mass                      (kg)
        double radius;       // mean radius               (m)
        double density;      // mean density              (kg m^-3)
        double rotation;     // sidereal rotation period  (s)
        int satellites;      // satellites                --
    };

    const std::vector<DwarfPlanetData>& dwarf_planet_table();
    inline const std::vector<DwarfPlanetData>& dwarf_planets = dwarf_planet_table();

    struct SatelliteData {
        int planet;       // planet number         (1-based)
        int satellite;    // satellite number      (1-based)
        Ustring label;    // discovery label       --
        Ustring name;     // name                  --
        double distance;  // semi-major axis       (m)
        double period;    // orbital period        (s)
        double ecc;       // orbital eccentricity  --
        double incl;      // orbital inclination   (rad)
        double mass;      // mass                  (kg)
        double radius;    // mean radius           (m)
        double density;   // mean density          (kg m^-3)
    };

    const std::vector<SatelliteData>& satellite_table();
    inline const std::vector<SatelliteData>& satellites = satellite_table();

    // Stellar property relations

    double magnitude_to_ratio(double delta_mag) noexcept;
    double ratio_to_magnitude(double ratio) noexcept;
    double magnitude_to_luminosity(double M_bol) noexcept;
    double luminosity_to_magnitude(double L) noexcept;
    double magnitude_to_visual_luminosity(double M_V) noexcept;
    double visual_luminosity_to_magnitude(double L_V) noexcept;
    double luminosity_radius_to_temperature(double lum, double rad) noexcept;
    double luminosity_temperature_to_radius(double lum, double temp) noexcept;
    double radius_temperature_to_luminosity(double rad, double temp) noexcept;
    double solar_luminosity_radius_to_temperature(double lum, double rad) noexcept;
    double solar_luminosity_temperature_to_radius(double lum, double temp) noexcept;
    double solar_radius_temperature_to_luminosity(double rad, double temp) noexcept;

    // Spectral classification

    RS_ENUM_CLASS(LC, uint8_t, 0, Ia, Ib, II, III, IV, V, VI, VII, none);
    RS_ENUM_CLASS(Sp, uint8_t, 0,
        O, B, A, F, G, K, M,         // Main sequence
        L, T, Y,                     // Brown dwarfs
        DA, DB, DC, DO, DQ, DX, DZ,  // White dwarfs
        WC, WN, WNh, WO,             // Wolf-Rayet stars
        C, S,                        // Carbon stars
        none
    );

    constexpr bool is_main_sequence(Sp sp) noexcept { return sp >= Sp::O && sp <= Sp::M; }
    constexpr bool is_brown_dwarf(Sp sp) noexcept { return sp >= Sp::L && sp <= Sp::Y; }
    constexpr bool is_white_dwarf(Sp sp) noexcept { return sp >= Sp::DA && sp <= Sp::DZ; }
    constexpr bool is_wolf_rayet(Sp sp) noexcept { return sp >= Sp::WC && sp <= Sp::WO; }
    constexpr bool is_carbon_star(Sp sp) noexcept { return sp >= Sp::C && sp <= Sp::S; }

    struct Spectrum:
    LessThanComparable<Spectrum> {
        LC lc = LC::none;
        Sp sp = Sp::none;
        uint8_t sub = 0;
        constexpr Spectrum() = default;
        constexpr Spectrum(LC l, Sp s, uint8_t n = 5) noexcept: lc(l), sp(s), sub(n) {}
        constexpr Spectrum(LC l, int index) noexcept: Spectrum(l, Sp(index / 10), uint8_t(index % 10)) {}
        constexpr Spectrum(Sp s, uint8_t n = 5) noexcept: Spectrum(LC::none, s, n) { update_lc(); }
        explicit constexpr Spectrum(int index) noexcept: Spectrum(LC::none, index) { update_lc(); }
        explicit Spectrum(Uview str);
        constexpr int index() const noexcept { return 10 * int(sp) + int(sub); }
        Ustring str() const;
        bool try_parse(Uview str) noexcept;
        size_t hash() const noexcept { return std::hash<uint32_t>()((uint32_t(lc) << 16) + (uint32_t(sp) << 8) + sub); }
        constexpr void update_lc() noexcept;
    };

    constexpr void Spectrum::update_lc() noexcept {
        if (is_main_sequence(sp)) {
            if (lc == LC::none)
                lc = LC::V;
        } else if (is_white_dwarf(sp)) {
            lc = LC::VII;
        } else {
            lc = LC::none;
        }
    }

    constexpr bool operator==(const Spectrum& lhs, const Spectrum& rhs) noexcept {
        return std::tie(lhs.lc, lhs.sp, lhs.sub) == std::tie(rhs.lc, rhs.sp, rhs.sub);
    }

    constexpr bool operator<(const Spectrum& lhs, const Spectrum& rhs) noexcept {
        return std::tie(lhs.lc, lhs.sp, lhs.sub) < std::tie(rhs.lc, rhs.sp, rhs.sub);
    }

    inline Ustring to_str(const Spectrum& spc) {
        return spc.str();
    }

    inline std::ostream& operator<<(std::ostream& out, const Spectrum& spc) {
        return out << spc.str();
    }

    // Hertzsprung-Russell diagram

    struct ClassData {
        Spectrum spectrum;       // Sp
        double mass = 0;         // M_sun
        double luminosity = 0;   // L_sun
        double radius = 0;       // R_sun
        double temperature = 0;  // K
        double M_V = 0;          // mag
        double M_bol = 0;        // mag
        double B_V = 0;          // mag
        double BC = 0;           // mag
        constexpr ClassData() = default;
        explicit ClassData(const Spectrum& spc);
    };

    // Interpolation table

    template <bool LogX = false, bool LogY = false>
    class InterpolationTable {
    public:
        InterpolationTable() = default;
        InterpolationTable(std::initializer_list<std::pair<double, double>> list);
        void insert(double x, double y);
        double operator()(double x) const;
    private:
        std::map<double, double> map_;
    };

        template <bool LogX, bool LogY>
        InterpolationTable<LogX, LogY>::InterpolationTable(std::initializer_list<std::pair<double, double>> list) {
            for (auto [x,y]: list)
                insert(x, y);
        }

        template <bool LogX, bool LogY>
        void InterpolationTable<LogX, LogY>::insert(double x, double y) {
            if constexpr (LogX)
                x = std::log(x);
            if constexpr (LogY)
                y = std::log(y);
            map_[x] = y;
        }

        template <bool LogX, bool LogY>
        double InterpolationTable<LogX, LogY>::operator()(double x) const {
            if (map_.empty())
                return 0;
            if (map_.size() == 1)
                return map_.begin()->second;
            if constexpr (LogX)
                x = std::log(x);
            double y = 0;
            auto i = map_.lower_bound(x), j = i;
            if (i == map_.end()) {
                --j;
                i = std::prev(j);
            } else if (i->first == x) {
                y = i->second;
            } else if (i == map_.begin()) {
                ++j;
            } else {
                --i;
            }
            if (i != j)
                y = interpolate(i->first, i->second, j->first, j->second, x);
            if constexpr (LogY)
                y = std::exp(y);
            return y;
        }

}

RS_DEFINE_STD_HASH(RS::Astro::Spectrum);
