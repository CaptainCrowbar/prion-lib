# Core Floating Point #

By Ross Smith

* `#include "rs-core/float.hpp"`

## Contents ##

[TOC]

## Arithmetic constants ##

* `#define` **`RS_DEFINE_CONSTANT`**`(name, value)`
    * `static constexpr float` **`name##_f`** `= value##f`
    * `static constexpr double` **`name##_d`** `= value`
    * `static constexpr long double` **`name##_ld`** `= value##l`
    * `template <typename T> static constexpr T` **`name##_c`** `= T(name##_ld)`

Defines a floating point constant, in several forms. This generates a set of
three named constants (for the three standard floating point types) and a
variable template. The `value` argument must be a floating point literal with
no type suffix.

(This intentionally uses a different naming convention to
[P0631](https://wg21.link/p0631), to avoid collisions if P0631 becomes part of
the standard library.)

The values quoted for the following constants are approximate; the values
actually supplied are accurate to at least `long double` precision where an
exact value is defined, or to the best known accuracy for the empirical
constants.

* _Mathematical constants from Posix/P0631_
    * **`e`** _= 2.7183_
    * **`log2e`** _= log<sub>2</sub> e = 1.4427_
    * **`log10e`** _= log<sub>10</sub> e = 0.4343_
    * **`ln2`** _= log<sub>e</sub> 2 = 0.6931_
    * **`ln10`** _= log<sub>e</sub> 10 = 2.3026_
    * **`pi`** _= &pi; = 3.1416_
    * **`pi_2`** _= &pi;/2 = 1.5708_
    * **`pi_4`** _= &pi;/4 = 0.7854_
    * **`one_pi`** _= 1/&pi; = 0.3183_
    * **`two_pi`** _= 2/&pi; = 0.6366_
    * **`two_sqrtpi`** _= 2/&radic;&pi; = 1.1284_
    * **`sqrt2`** _= &radic;2 = 1.4142_
    * **`one_sqrt2`** _= 1/&radic;2 = 0.7071_
* _Other mathematical constants_
    * **`sqrt3`** _= &radic;3 = 1.7321_
    * **`sqrt5`** _= &radic;5 = 2.2361_
    * **`sqrtpi`** _= &radic;&pi; = 1.7725_
    * **`sqrt2pi`** _= &radic;(2&pi;) = 2.5066_
* _Conversion factors_
    * **`inch`** _= 0.0254 m_
    * **`foot`** _= 0.3048 m_
    * **`yard`** _= 0.9144 m_
    * **`mile`** _= 1609 m_
    * **`nautical_mile`** _= 1852 m_
    * **`ounce`** _= 0.02835 kg_
    * **`pound`** _= 0.4536 kg_
    * **`short_ton`** _= 907.2 kg_
    * **`long_ton`** _= 1016 kg_
    * **`pound_force`** _= 4.448 N_
    * **`erg`** _= 10<sup>-7</sup> J_
    * **`foot_pound`** _= 1.356 J_
    * **`calorie`** _= 4.184 J_
    * **`kilocalorie`** _= 4184 J_
    * **`ton_tnt`** _= 4.184&times;10<sup>9</sup> J_
    * **`horsepower`** _= 745.7 W_
    * **`mmHg`** _= 133.3 Pa_
    * **`atmosphere`** _= 101300 Pa_
    * **`zero_celsius`** _= 273.15 K_
* _Physical constants_
    * **`atomic_mass_unit`** _= 1.661&times;10<sup>-27</sup> kg_
    * **`avogadro_constant`** _= 6.022&times;10<sup>23</sup> mol<sup>-1</sup>_
    * **`boltzmann_constant`** _= 1.381&times;10<sup>-23</sup> J K<sup>-1</sup>_
    * **`elementary_charge`** _= 1.602&times;10<sup>-19</sup> C_
    * **`gas_constant`** _= 8.314 J mol<sup>-1</sup> K<sup>-1</sup>_
    * **`gravitational_constant`** _= 6.674&times;10<sup>-11</sup> m<sup>3</sup> kg<sup>-1</sup> s<sup>-2</sup>_
    * **`planck_constant`** _= 6.626&times;10<sup>-34</sup> J s_
    * **`speed_of_light`** _= 2.998&times;10<sup>8</sup> m s<sup>-1</sup>_
    * **`stefan_boltzmann_constant`** _= 5.670&times;10<sup>-8</sup> W m<sup>-2</sup> K<sup>-4</sup>_
* _Astronomical constants_
    * **`earth_mass`** _= 5.972&times;10<sup>24</sup> kg_
    * **`earth_radius`** _= 6.371&times;10<sup>6</sup> m_
    * **`earth_gravity`** _= 9.807 m s<sup>-2</sup>_
    * **`jupiter_mass`** _= 1.899&times;10<sup>27</sup> kg_
    * **`jupiter_radius`** _= 6.991&times;10<sup>7</sup> m_
    * **`solar_mass`** _= 1.989&times;10<sup>30</sup> kg_
    * **`solar_radius`** _= 6.963&times;10<sup>8</sup> m_
    * **`solar_luminosity`** _= 3.846&times;10<sup>26</sup> W_
    * **`solar_temperature`** _= 5778 K_
    * **`astronomical_unit`** _= 1.496&times;10<sup>11</sup> m_
    * **`light_year`** _= 9.461&times;10<sup>15</sup> m_
    * **`parsec`** _= 3.086&times;10<sup>16</sup> m_
    * **`julian_day`** _= 86400 s_
    * **`julian_year`** _= 31557600 s_
    * **`sidereal_year`** _= 31558150 s_
    * **`tropical_year`** _= 31556925 s_

## Arithmetic functions ##

* `template <typename T> constexpr T` **`degrees`**`(T rad) noexcept`
* `template <typename T> constexpr T` **`radians`**`(T deg) noexcept`

Convert between degrees and radians.

* `template <typename T1, typename T2> constexpr T2` **`interpolate`**`(T1 x1, T2 y1, T1 x2, T2 y2, T1 x) noexcept`

Returns the value of `y` corresponding to `x`, by interpolating or
extrapolating the line between `(x1,y1)` and `(x2,y2)`. If `x1=x2` it will
return the average of `y1` and `y2`. The usual arithmetic rules apply if the
result is not representable by the return type.

* `template <typename T2, typename T1> T2` **`iceil`**`(T1 value) noexcept`
* `template <typename T2, typename T1> T2` **`ifloor`**`(T1 value) noexcept`
* `template <typename T2, typename T1> T2` **`iround`**`(T1 value) noexcept`

Rounds the argument toward positive infinity (`iceil()`), toward negative
infinity (`ifloor()`), or to the nearest integer (`iround()`; halves round
up), converting the result to the specified return type. The usual arithmetic
rules apply if the result is not representable by the return type.

## Arithmetic literals ##

* `namespace RS::`**`Literals`**
    * `constexpr float` **`operator""_degf`**`(long double x) noexcept`
    * `constexpr float` **`operator""_degf`**`(unsigned long long x) noexcept`
    * `constexpr double` **`operator""_deg(`**`long double x) noexcept`
    * `constexpr double` **`operator""_deg(`**`unsigned long long x) noexcept`
    * `constexpr long double` **`operator""_degl`**`(long double x) noexcept`
    * `constexpr long double` **`operator""_degl`**`(unsigned long long x) noexcept`

Angle literals, converting degrees to radians.

## Numerical algorithms ##

### Numerical integration ###

* `template <typename T, typename F> T` **`line_integral`**`(T x1, T x2, size_t k, F f)`

Computes the integral of `f(x)` over the interval `[x1,x2]` by the trapezoid
algorithm, using `k` subdivisions. This has complexity `O(k)`. Behaviour is
undefined if `k=0` or the function has a pole within the interval.

* `template <typename T, size_t N, typename F> T` **`volume_integral`**`(Vector<T, N> x1, Vector<T, N> x2, size_t k, F f)`

Computes the volume integral of `f(x)` over the rectangular prism whose
opposite corners are `x1` and `x2`, dividing each side into `k` subdivisions.
This has complexity <code>O(k<sup>N</sup>)</code> Behaviour is undefined if
`k=0` or the function has a pole within the volume.

### Precision sum ###

* `template <typename T> class` **`PrecisionSum`**
    * `using PrecisionSum::`**`value_type`** `= T`
    * `PrecisionSum::`**`PrecisionSum`**`()`
    * `void PrecisionSum::`**`operator()`**`(T t)`
    * `PrecisionSum::`**`operator T`**`() const`
    * `void PrecisionSum::`**`clear`**`() noexcept`
* `template <typename SinglePassRange> [value type]` **`precision_sum`**`(const SinglePassRange& range)`

Calculate the sum of a sequence of numbers using the high precision algorithm from
[Shewchuk](http://www-2.cs.cmu.edu/afs/cs/project/quake/public/papers/robust-arithmetic.ps)
and [Hettinger](http://code.activestate.com/recipes/393090/).
This can be called as either an accumulator to which values can be added one
at a time, or a range based function that calculates the sum in one call. The
range's value type must be a floating point arithmetic type. This is always
much more accurate than simple addition, and is guaranteed to give the correct
answer (the exact sum correctly rounded) if the value type implements IEEE
arithmetic (on GCC this requires the `-ffloat-store` option).

### Root finding ###

For all of these algorithms, `T` must be a floating point arithmetic type, `F`
(and `DF` if it is used) must be a function from `T` to `T`.

The structure members specify the tolerance and the maximum number of
iterations; iteration will stop when the absolute value of `f()` is less than
or equal to `epsilon`, or the maximum number of iterations is reached. The
`count` and `error` members are filled with the actual iteration count and
absolute error. If the function has multiple roots, the one closest to the
initial values will typically be found, but the nature of the algorithms means
that this cannot be promised.

The default epsilon is arbitrarily set to 10<sup>-4</sup> for `float`,
10<sup>-8</sup> for `double` and `long double`. Behaviour is undefined if
`epsilon` is not positive, if the function has no root, or for the algorithms
that take two initial values, if `f(a)` and `f(b)` have the same sign.

* `template <typename T> struct` **`Bisection`**
    * `T Bisection::`**`epsilon`** `= [see above]`
    * `size_t Bisection::`**`limit`** `= 100`
    * `size_t Bisection::`**`count`**
    * `T Bisection::`**`error`**
    * `template <typename F> T Bisection::`**`operator()`**`(F f, T a, T b)`

Use the bisection algorithm to find a root of a function. The function call
operator takes the function to be solved and two initial values, which the
caller is expected to ensure are on opposite sides of the root.

* `template <typename T> struct` **`FalsePosition`**
    * `T FalsePosition::`**`epsilon`** `= [see above]`
    * `size_t FalsePosition::`**`limit`** `= 100`
    * `size_t FalsePosition::`**`count`**
    * `T FalsePosition::`**`error`**
    * `template <typename F> T FalsePosition::`**`operator()`**`(F f, T a, T b)`

Use the [false position](http://en.wikipedia.org/wiki/False_position_method)
algorithm to find a root of a function. The function call operator takes the
function to be solved and two initial values, which the caller is expected to
ensure are on opposite sides of the root.

* `template <typename T> struct` **`NewtonRaphson`**
    * `T NewtonRaphson::`**`epsilon`** `= [see above]`
    * `size_t NewtonRaphson::`**`limit`** `= 100`
    * `size_t NewtonRaphson::`**`count`**
    * `T NewtonRaphson::`**`error`**
    * `template <typename F, typename DF> T NewtonRaphson::`**`operator()`**`(F f, DF df, T a = 0)`

Use the [Newton-Raphson](http://en.wikipedia.org/wiki/Newton%27s_method)
algorithm to find a root of a function. The function call operator takes the
function to be solved, the derivative of this function, and an initial value.
Results are unpredictable if `df()` is not a good approximation to the
derivative of `f()`.

* `template <typename T> struct` **`PseudoNewtonRaphson`**
    * `T PseudoNewtonRaphson::`**`epsilon`** `= [see above]`
    * `T PseudoNewtonRaphson::`**`delta`** `= [same as default epsilon]`
    * `size_t PseudoNewtonRaphson::`**`limit`** `= 100`
    * `size_t PseudoNewtonRaphson::`**`count`**
    * `T PseudoNewtonRaphson::`**`error`**
    * `template <typename F> T PseudoNewtonRaphson::`**`operator()`**`(F f, T a = 0)`

This uses the Newton-Raphson algorithm, but calculates an approximation to the
derivative by evaluating `f()` at a nearby point(`f(x+delta)`), instead of
taking the derivative as a separate function. Results are unpredictable if the
function exhibits significant irregularities on a scale smaller than `delta`.
Behaviour is undefined if `delta` is zero.
