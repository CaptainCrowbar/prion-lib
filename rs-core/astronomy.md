# Astronomical Functions and Reference Data #

By Ross Smith

* `#include "rs-core/astronomy.hpp"`
* `namespace RS::Astro`

## Contents ##

[TOC]

## Constants ##

Values in the tables here are quoted to four significant figures for quick
reference (apart from the day and year lengths, which are quoted to the
nearest second); the actual values in the library are expressed to the maximum
available or representable precision.a

### Physical constants ###

Long name                        | Short name       | Value      | Units                                         | Source
---------                        | ----------       | -----      | -----                                         | ------
**`atomic_mass_unit`**           | **`m_u`**        | 1.661e-27  | kg                                            | Best available value
**`avogadro_constant`**          | **`N_A`**        | 6.022e23   | mol<sup>-1</sup>                              | Exact value fixed by SI 2019
**`boltzmann_constant`**         | **`k_B`**        | 1.381e-23  | J K<sup>-1</sup>                              | Exact value fixed by SI 2019
**`electron_mass`**              | **`m_e`**        | 9.109e-31  | kg                                            | Best available value
**`elementary_charge`**          | **`ec`**         | 1.602e-19  | C                                             | Exact value fixed by SI 2019
**`gravitational_constant`**     | **`G`**          | 6.674e-11  | m<sup>3</sup> kg<sup>-1</sup> s<sup>-2</sup>  | Best available value
**`magnetic_constant`**          | **`mu_0`**       | 1.257e-6   | N A<sup>-2</sup>                              | Best available value
**`planck_constant`**            | **`h`**          | 6.626e-34  | J s                                           | Exact value fixed by SI 2019
**`rydberg_constant`**           | **`R_inf`**      | 1.097e7    | m<sup>-1</sup>                                | Best available value
**`speed_of_light`**             | **`c`**          | 2.998e8    | m s<sup>-1</sup>                              | Exact value fixed by SI 2019
**`vacuum_impedance`**           | **`Z_0`**        | 376.7      | ohm                                           | Best available value
**`vacuum_permittivity`**        | **`epsilon_0`**  | 8.854e-12  | F m<sup>-1</sup>                              | Best available value
**`faraday_constant`**           | **`F`**          | 96490      | C mol<sup>-1</sup>                            | Exact value derived from SI 2019
**`josephson_constant`**         | **`K_J`**        | 4.836e14   | Hz/V                                          | Exact value derived from SI 2019
**`molar_gas_constant`**         | **`R`**          | 8.314      | J mol<sup>-1</sup> K<sup>-1</sup>             | Exact value derived from SI 2019
**`molar_mass_constant`**        | **`M_u`**        | 0.001      | kg mol<sup>-1</sup>                           | Best available value
**`stefan_boltzmann_constant`**  | **`sigma`**      | 5.670e-8   | W m<sup>-2</sup> K<sup>-4</sup>               | Exact value derived from SI 2019
**`von_klitzing_constant`**      | **`R_K`**        | 25810      | ohm                                           | Exact value derived from SI 2019
**`first_radiation_constant`**   | **`c1_L`**       | 1.191e-16  | m<sup>4</sup> kg s                            | Exact value derived from SI 2019
**`second_radiation_constant`**  | **`c2`**         | 0.01439    | m K                                           | Exact value derived from SI 2019
**`wien_constant`**              | **`b_W`**        | 2.898e-3   | m K                                           | Best available value

### Astronomical constants ###

Long name                             | Short name     | Value     | Units             | Source
---------                             | ----------     | -----     | -----             | ------
**`sidereal_day`**                    | **`d_sid`**    | 86164     | s                 | Best available value
**`julian_day`**                      | **`jd`**       | 86400     | s                 | Exact by definition
**`tropical_year`**                   | **`y_trop`**   | 31556925  | s                 | Best available value
**`julian_year`**                     | **`jy`**       | 31557600  | s                 | Exact by definition
**`sidereal_year`**                   | **`y_sid`**    | 31558150  | s                 | Best available value
**`earth_radius`**                    | **`R_earth`**  | 6.371e6   | m                 | Best available value
**`jupiter_radius`**                  | **`R_jup`**    | 6.991e7   | m                 | Best available value
**`solar_radius`**                    | **`R_sun`**    | 6.957e8   | m                 | IAU adopted value
**`astronomical_unit`**               | **`au`**       | 1.496e8   | m                 | IAU adopted value
**`light_year`**                      | **`ly`**       | 9.461e15  | m                 | Exact value derived from SI
**`parsec`**                          | **`pc`**       | 3.086e16  | m                 | Best available value
**`earth_mass`**                      | **`M_earth`**  | 5.972e24  | kg                | Best available value
**`jupiter_mass`**                    | **`M_jup`**    | 1.898e27  | kg                | Best available value
**`solar_mass`**                      | **`M_sun`**    | 1.988e30  | kg                | Best available value
**`earth_gravity`**                   | **`g_earth`**  | 9.807     | m s<sup>-2</sup>  | Standard adopted value
**`bolometric_luminosity_constant`**  | **`L_0`**      | 3.013e28  | W                 | IAU adopted value
**`solar_luminosity`**                | **`L_sun`**    | 3.828e26  | W                 | IAU adopted value
**`solar_temperature`**               | **`T_sun`**    | 5772      | K                 | IAU adopted value

## Blackbody radiation ##

* `class` **`BlackbodySpectrum`**
    * `BlackbodySpectrum::`**`BlackbodySpectrum`**`() noexcept`
    * `explicit BlackbodySpectrum::`**`BlackbodySpectrum`**`(double temp) noexcept`
    * `double BlackbodySpectrum::`**`peak`**`() const noexcept`
    * `double BlackbodySpectrum::`**`power`**`() const noexcept`
    * `double BlackbodySpectrum::`**`operator()`**`(double lambda) const noexcept`

Properties of the blackbody spectrum for a given temperature:

* `peak()` = wavelength of peak emission
* `power()` = total emission (W m<sup>-2</sup>)
* `operator()()` = spectral radiance for a given wavelength,
    per unit surface area, per unit solid angle, per unit wavelength
    (W m<sup>-2</sup> sr<sup>-1</sup> m<sup>-1</sup> = kg m<sup>-1</sup> s<sup>-3</sup> sr<sup>-1</sup>)

Temperature is in kelvins; wavelength is in metres.

## Coordinate systems ##

* `constexpr double` **`degrees_to_radians`**`(double deg, double min = 0, double sec = 0) noexcept`
* `constexpr double` **`hours_to_radians`**`(double hrs, double min = 0, double sec = 0) noexcept`
* `constexpr double` **`radians_to_degrees`**`(double rad) noexcept`
* `void` **`radians_to_degrees`**`(double rad, int& deg, double& min) noexcept`
* `void` **`radians_to_degrees`**`(double rad, int& deg, int& min, double& sec) noexcept`
* `constexpr double` **`radians_to_hours`**`(double rad) noexcept`
* `void` **`radians_to_hours`**`(double rad, int& hrs, double& min) noexcept`
* `void` **`radians_to_hours`**`(double rad, int& hrs, int& min, double& sec) noexcept`

Conversion of angles between radians, degrees (with optional minutes and
seconds), and hours of right ascension (with optional minutes and seconds).
Where a broken down angle is passed in two or three arguments, the arguments
are treated as being independently signed; if the angle is negative, all of
the components should be signed.

* `Ustring` **`format_angle`**`(double rad, Uview fmt = "dms")`

Format an angle as degrees (or hours), minutes, and seconds. The format string
contains the following elements (order and case sensitive):

* `d|dm|dms|h|hm|hms` = Breakdown format
* `+` = Always insert a sign prefix _(optional)_
* `<integer>` = Number of decimal places for the last element _(optional; default 0)_
* `[ips]` = Flags _(optional)_
* `,<string>,<string>,<string>` = Postfix symbols for degrees (or hours), minutes, and seconds _(optional)_

Flags:

* `i` = Insert the symbol attached to the last element before the decimal
point instead of after the fractional part (e.g. `12˚34'56".789`). This flag
has no effect if the precision is zero.
* `p` = Pad the leading element with zeros to three (degrees) or two (hours)
digits (other elements are always fixed width).
* `s` = Use the standard unit symbols (`˚'"` for degrees, `hms` for hours).
This may not be combined with explicit unit symbols at the end of the format
string.

If no unit symbols are given, the elements will be separated with spaces; to
get space delimiters with unit symbols, include the trailing space with the
symbols.

The function will throw `std::invalid_argument` if the format string is
invalid.

* `double` **`parse_angle`**`(const Ustring& str, Uview fmt = "")`

Parse a angle expressed in degrees (or hours), minutes, and seconds. Any of
the three elements may have a decimal part; the common astronomical convention
of placing the unit before the decimal point is recognised. Whitespace between
elements is ignored, apart from being recognised as a delimiter, as is leading
and trailing whitespace.

The only valid values for the format string are `"d[ms]"` (degrees), `"h[ms]"`
(hours), or an empty string (characters after the first are ignored and do not
imply a specific number of elements). If the format is empty, a unit is
expected to be attached to the leading element. A unit starting with `"D"`,
`"d"`, or `"˚"` is assumed to mean degrees; a unit starting with `"H"` or
`"h"` is assumed to mean hours; any other unit (including none) is an error.
If a specific format is supplied, it is an error if the unit on the leading
element disagrees with the format, but a missing or unrecognised unit will be
assumed to match the format.

The function will throw `std::invalid_argument` if the angle string cannot be
parsed, if the format string is not one of the legal values, or if the angle
string and format string have different units.

## Solar system data ##

* `struct` **`SunData`**
    * `double`  **`mass`**         `= 1.988e30`  _- mass                           (kg)_
    * `double`  **`luminosity`**   `= 3.828e26`  _- luminosity                     (W)_
    * `double`  **`radius`**       `= 6.957e8`   _- radius                         (m)_
    * `double`  **`temperature`**  `= 5772`      _- temperature                    (K)_
    * `double`  **`V`**            `= -26.75`    _- apparent visual magnitude      (mag)_
    * `double`  **`M_U`**          `= +5.66`     _- absolute U magnitude           (mag)_
    * `double`  **`M_B`**          `= +5.47`     _- absolute B magnitude           (mag)_
    * `double`  **`M_V`**          `= +4.82`     _- absolute V magnitude           (mag)_
    * `double`  **`M_R`**          `= +4.28`     _- absolute R magnitude           (mag)_
    * `double`  **`M_I`**          `= +3.94`     _- absolute I magnitude           (mag)_
    * `double`  **`M_bol`**        `= +4.74`     _- absolute bolometric magnitude  (mag)_
    * `double`  **`BC`**           `= -0.08`     _- bolometric correction          (mag)_
    * `double`  **`B_V`**          `= +0.65`     _- colour index                   (mag)_
* `struct` **`PlanetData`**
    * `Ustring`  **`name`**               _- name_                     <!-- -->
    * `double`   **`distance`**           _- semi-major axis           (m)_
    * `double`   **`period`**             _- orbital period            (s)_
    * `double`   **`eccentriciy`**        _- orbital eccentricity_     <!-- -->
    * `double`   **`inclination`**        _- orbital inclination       (rad)_
    * `double`   **`mass`**               _- mass                      (kg)_
    * `double`   **`radius`**             _- mean radius               (m)_
    * `double`   **`equatorial_radius`**  _- equatorial radius         (m)_
    * `double`   **`polar_radius`**       _- polar radius              (m)_
    * `double`   **`density`**            _- mean density              (kg m<sup>-3</sup>)_
    * `double`   **`rotation`**           _- sidereal rotation period  (s)_
    * `double`   **`axial_inclination`**  _- axial inclination         (rad)_
    * `int`      **`satellites`**         _- number of satellites_     <!-- -->
* `struct` **`DwarfPlanetData`**
    * `int`      **`number`**       _- minor planet number       (0 if not numbered)_
    * `Ustring`  **`name`**         _- name_                     <!-- -->
    * `double`   **`distance`**     _- semi-major axis           (m)_
    * `double`   **`period`**       _- orbital period            (s)_
    * `double`   **`eccentriciy`**  _- orbital eccentricity_     <!-- -->
    * `double`   **`inclination`**  _- orbital inclination       (rad)_
    * `double`   **`mass`**         _- mass                      (kg; may be null)_
    * `double`   **`radius`**       _- mean radius               (m)_
    * `double`   **`density`**      _- mean density              (kg m<sup>-3</sup>; may be null)_
    * `double`   **`rotation`**     _- sidereal rotation period  (s; may be null)_
    * `int`      **`satellites`**   _- satellites_               <!-- -->
* `struct` **`SatelliteData`**
    * `int`      **`planet`**     _- planet number          (1-based)_
    * `int`      **`satellite`**  _- satellite number       (1-based)_
    * `Ustring`  **`label`**      _- discovery label        (may be empty)_
    * `Ustring`  **`name`**       _- name_                  <!-- -->
    * `double`   **`distance`**   _- semi-major axis        (m)_
    * `double`   **`period`**     _- orbital period         (s)_
    * `double`   **`ecc`**        _- orbital eccentricity_  <!-- -->
    * `double`   **`incl`**       _- orbital inclination    (rad)_
    * `double`   **`mass`**       _- mass                   (kg)_
    * `double`   **`radius`**     _- mean radius            (m)_
    * `double`   **`density`**    _- mean density           (kg m<sup>-3</sup>)_
* `enum class` **`Planet`**
    * `Planet::`**`Mercury`**  `= 1`
    * `Planet::`**`Venus`**    `= 2`
    * `Planet::`**`Earth`**    `= 3`
    * `Planet::`**`Mars`**     `= 4`
    * `Planet::`**`Jupiter`**  `= 5`
    * `Planet::`**`Saturn`**   `= 6`
    * `Planet::`**`Uranus`**   `= 7`
    * `Planet::`**`Neptune`**  `= 8`
* `constexpr SunData` **`Sun`**
* `inline const std::vector<PlanetData>&` **`planets`**
* `inline const std::vector<DwarfPlanetData>&` **`dwarf_planets`**
* `inline const std::vector<SatelliteData>&` **`satellites`**

Reference data for solar system bodies. Null (unknown) values are represented
by `NaN`. The `planets` table has a dummy entry at index zero, so that
indexing reflects the conventional numbering of the planets. The
`dwarf_planets` table includes all bodies currently known or suspected to be
dwarf planets. Any specific values listed in the documentation above are
convenient approximations and do not necessarily match the more exact values
in the library.

## Stars ##

### Stellar property relations ###

* `double` **`magnitude_to_ratio`**`(double delta_mag) noexcept`
* `double` **`ratio_to_magnitude`**`(double ratio) noexcept`

Conversions between a magnitude difference and a ratio. A ratio greater than 1
corresponds to a negative magnitude difference, a ratio less than 1 to a
negative difference. For the second function, behaviour is undefined if the
ratio is less than or equal to zero.

* `double` **`magnitude_to_luminosity`**`(double M_bol) noexcept`
* `double` **`luminosity_to_magnitude`**`(double L) noexcept`
* `double` **`magnitude_to_visual_luminosity`**`(double M_V) noexcept`
* `double` **`visual_luminosity_to_magnitude`**`(double L_V) noexcept`

Conversions between an absolute magnitude and a luminosity relative to the
Sun. The first two functions work with bolometric luminosity and magnitude,
the last two with visual luminosity and magnitude. For the versions that take
a luminosity argument, behaviour is undefined if the argument is less than or
equal to zero.

* `double` **`luminosity_radius_to_temperature`**`(double lum, double rad) noexcept`
* `double` **`luminosity_temperature_to_radius`**`(double lum, double temp) noexcept`
* `double` **`radius_temperature_to_luminosity`**`(double rad, double temp) noexcept`
* `double` **`solar_luminosity_radius_to_temperature`**`(double lum, double rad) noexcept`
* `double` **`solar_luminosity_temperature_to_radius`**`(double lum, double temp) noexcept`
* `double` **`solar_radius_temperature_to_luminosity`**`(double rad, double temp) noexcept`

Conversions between bolometric luminosity, radius, and effective temperature.
The first three functions express luminosity and radius in SI units (watts and
metres, respectively); the last three express them in solar units. All
functions express temperature in kelvins. For all of these functions,
behaviour is undefined if any argument is less than or equal to zero.

### Spectral classification ###

* `enum class` **`LC`**`: uint8_t`
    * `LC::`**`Ia`**
    * `LC::`**`Ib`**
    * `LC::`**`II`**
    * `LC::`**`III`**
    * `LC::`**`IV`**
    * `LC::`**`V`**
    * `LC::`**`VI`**
    * `LC::`**`VII`**
    * `LC::`**`none`**
* `enum class` **`Sp`**`: uint8_t`
    * `Sp::`**`O`**`, Sp::`**`B`**`, Sp::`**`A`**`, Sp::`**`F`**`, Sp::`**`G`**`, Sp::`**`K`**`, Sp::`**`M`**         _- Main sequence_
    * `Sp::`**`L`**`, Sp::`**`T`**`, Sp::`**`Y`**                                                                     _- Brown dwarfs_
    * `Sp::`**`DA`**`, Sp::`**`DB`**`, Sp::`**`DC`**`, Sp::`**`DO`**`, Sp::`**`DQ`**`, Sp::`**`DX`**`, Sp::`**`DZ`**  _- White dwarfs_
    * `Sp::`**`WC`**`, Sp::`**`WN`**`, Sp::`**`WNh`**`, Sp::`**`WO`**                                                 _- Wolf-Rayet stars_
    * `Sp::`**`C`**`, Sp::`**`S`**                                                                                    _- Carbon stars_
    * `Sp::`**`none`**

Enumerations representing the standard luminosity classes and spectral classes.

* `constexpr bool` **`is_main_sequence`**`(Sp sp) noexcept` _- O through M_
* `constexpr bool` **`is_brown_dwarf`**`(Sp sp) noexcept` _- L through Y_
* `constexpr bool` **`is_white_dwarf`**`(Sp sp) noexcept` _-D A through DZ_
* `constexpr bool` **`is_wolf_rayet`**`(Sp sp) noexcept` _-W C through WO_
* `constexpr bool` **`is_carbon_star`**`(Sp sp) noexcept` _- C through S_

Broader classification of spectral classes. Note that `is_main_sequence()`
only checks that the spectral class is one of the classes on the standard
Hertzsprung-Russell diagram; it does not necessarily imply that the star is
actually on the main sequence.

* `struct` **`Spectrum`**
    * `LC Spectrum::`**`lc`** `= LC::none`
    * `Sp Spectrum::`**`sp`** `= Sp::none`
    * `uint8_t Spectrum::`**`sub`** `= 0`
    * `constexpr Spectrum::`**`Spectrum`**`() noexcept`
    * `constexpr Spectrum::`**`Spectrum`**`(LC l, Sp s, uint8_t n = 5) noexcept`
    * `constexpr Spectrum::`**`Spectrum`**`(LC l, int index) noexcept`
    * `constexpr Spectrum::`**`Spectrum`**`(Sp s, uint8_t n = 5) noexcept`
    * `explicit constexpr Spectrum::`**`Spectrum`**`(int index) noexcept`
    * `explicit Spectrum::`**`Spectrum`**`(Uview str)`
    * `constexpr int Spectrum::`**`index`**`() const noexcept`
    * `Ustring Spectrum::`**`str`**`() const`
    * `bool Spectrum::`**`try_parse`**`(Uview str) noexcept`
    * `size_t Spectrum::`**`hash`**`() const noexcept`
    * `constexpr void Spectrum::`**`update_lc`**`() noexcept`
* `constexpr bool` **`operator==`**`(const Spectrum& lhs, const Spectrum& rhs) noexcept`
* `constexpr bool` **`operator!=`**`(const Spectrum& lhs, const Spectrum& rhs) noexcept`
* `constexpr bool` **`operator<`**`(const Spectrum& lhs, const Spectrum& rhs) noexcept`
* `constexpr bool` **`operator>`**`(const Spectrum& lhs, const Spectrum& rhs) noexcept`
* `constexpr bool` **`operator<=`**`(const Spectrum& lhs, const Spectrum& rhs) noexcept`
* `constexpr bool` **`operator>=`**`(const Spectrum& lhs, const Spectrum& rhs) noexcept`
* `inline std::ostream&` **`operator<<`**`(std::ostream& out, const Spectrum& spc)`
* `inline Ustring` **`to_str`**`(const Spectrum& spc)`
* `template <> class` **`std::hash`**`<Spectrum>`

This structure represents a complete spectral class, consisting of a
luminosity class, a spectral class, and a numeric subclass.

The `index` value is an index into the standard main sequence and brown dwarf
spectral types; it is equal to 10 times the spectral class (converted to an
integer), plus the subclass, and ranges from 0 (class O0) to 99 (class Y9),
although the extreme values probably do not represent the types of real stars.
The index number is meaningless for star types outside the O-Y range.

Comparison of `Spectrum` values goes by luminosity class first, then spectral
class, then subclass.

Conversion of a `Spectrum` to a string follows the usual conventions. The
luminosity class is only included if the spectral class is O-M.

The constructor from a string, and the `try_parse()` function, attempt to
parse a spectral type in one of the standard formats. If the string is
invalid, `try_parse()` returns false, while the constructor throws
`std::invalid_argument`. For types O-M, luminosity class is assumed to be V if
it is not present; for all other types, luminosity class is ignored and always
set to VII for white dwarfs, otherwise to none. Luminosity class prefixes are
recognised (`c→Ia`, `g→III`, `sg→IV`, `d→V`, `sd→VI`), but the suffix takes
precedence if both are present. Decimal places in the subclass are ignored.
Ranges such as `"B5-7 V"` or `"G5 III-IV"` are not supported.

### Hertzsprung-Russell diagram ###

* `struct` **`ClassData`**
    * `Spectrum`  **`spectrum`**     _= spectral type_
    * `double`    **`mass`**         _= mass (Sun = 1)_
    * `double`    **`luminosity`**   _= luminosity (Sun = 1)_
    * `double`    **`radius`**       _= radius (Sun = 1)_
    * `double`    **`temperature`**  _= effective temperature (K)_
    * `double`    **`M_V`**          _= absolute visual magnitude_
    * `double`    **`M_bol`**        _= absolute bolometric magnitude_
    * `double`    **`B_V`**          _= colour index_
    * `double`    **`BC`**           _= bolometric correction_
    * `constexpr ClassData::`**`ClassData`**`() noexcept`
    * `explicit ClassData::`**`ClassData`**`(const Spectrum& spc)`

This class implements a table of the typical properties of stars for a given
spectral type. Behaviour is undefined if the spectral class is invalid. (Note
that `ClassData(Spectrum("G2 V"))` should not be expected to be an exact match
for the Sun.)

Sources:

* Arthur N. Cox, ed. (1999), _Astrophysical Quantities_ (4th edition)
* Neill Reid (2002), ["M dwarfs, L dwarfs, and T dwarfs"](http://www.stsci.edu/~inr/ldwarf3.html)
* Wikipedia

## Utilities ##

### Interpolation table ###

* `template <bool LogX = false, bool LogY = false> class` **`InterpolationTable`**
    * `InterpolationTable`**`InterpolationTable`**`()`
    * `InterpolationTable`**`InterpolationTable`**`(std::initializer_list<std::pair<double, double>> list)`
    * `void InterpolationTable`**`insert`**`(double x, double y)`
    * `double InterpolationTable`**`operator()`**`(double x) const`

This is an interpolating lookup table (similar to a simplified version of
[`ScaleMap`](scale-map.html)). Arguments between two data points will be
interpolated; arguments outside the range of data points will be extrapolated
from the nearest two points. If no points have been supplied, lookup will
always return zero; if only one point has been supplied, lookup will always
return that point's `y` value. The template arguments indicate whether the two
coordinates are intended to be interpolated linearly or logarithmically.
Behaviour is undefined if a value supplied for a logarithmic coordinate
(either while building the table or in the lookup operator) is less than or
equal to zero.
