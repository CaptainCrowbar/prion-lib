# Core Floating Point #

By Ross Smith

* `#include "rs-core/float.hpp"`

## Contents ##

[TOC]

## Arithmetic constants ##

<!-- The mixture of Markdown and HTML markup here is to generate a fixed width
layout without any syntax colouring errors in Sublime Text. -->

* `#define` **`RS_DEFINE_CONSTANT`**`(name, value) `
    * <code>constexpr long double              **name##_ld**    </code>`= value`
    * <code>constexpr double                   **name##_d**     </code>`= double(name##_ld)`
    * <code>constexpr double                   **name**         </code>`= name##_d`
    * <code>constexpr float                    **name##_f**     </code>`= float(name##_ld)`
    * <code>template &lt;typename T&gt; constexpr T  **name##_c**     </code>`= T(name##_ld)`
* `#define` **`RS_DEFINE_CONSTANT_2`**`(name, symbol, value) `
    * <code>constexpr long double              **name##_ld**    </code>`= value`
    * <code>constexpr long double              **symbol##_ld**  </code>`= name##_ld`
    * <code>constexpr double                   **name##_d**     </code>`= double(name##_ld)`
    * <code>constexpr double                   **symbol##_d**   </code>`= double(symbol##_ld)`
    * <code>constexpr double                   **name**         </code>`= name##_d`
    * <code>constexpr double                   **symbol**       </code>`= symbol##_d`
    * <code>constexpr float                    **name##_f**     </code>`= float(name##_ld)`
    * <code>constexpr float                    **symbol##_f**   </code>`= float(symbol##_ld)`
    * <code>template &lt;typename T&gt; constexpr T  **name##_c**     </code>`= T(name##_ld)`
    * <code>template &lt;typename T&gt; constexpr T  **symbol##_c**   </code>`= T(symbol##_ld)`

Define a floating point constant, in several forms. This generates a set of
three named constants (for the three standard floating point types) and a
variable template. The `value` argument must be a floating point `constexpr`
expression, normally of type `long double`.

The mathematical constants are the union of the Posix and
[P0631](https://wg21.link/p0631) lists; spelling is mostly taken from P0631.
However, this intentionally uses a different suffix convention to avoid
collisions if P0631 becomes part of the standard library.

The values quoted here for the following constants are approximate; the values
actually supplied are accurate to the precision of the type.

Constant                    | Definition                | Value
--------                    | ----------                | -----
**Mathematical constants**  |                           |
**`e`**                     | e                         | 2.7183
**`ln2`**                   | log<sub>e</sub> 2         | 0.6931
**`ln10`**                  | log<sub>e</sub> 10        | 2.3026
**`log2e`**                 | log<sub>2</sub> e         | 1.4427
**`log10e`**                | log<sub>10</sub> e        | 0.4343
**`pi`**                    | &pi;                      | 3.1416
**`pi_over_2`**             | &pi;/2                    | 1.5708
**`pi_over_4`**             | &pi;/4                    | 0.7854
**`inv_pi`**                | 1/&pi;                    | 0.3183
**`inv_sqrtpi`**            | 1/&radic;&pi;             | 0.5642
**`two_over_pi`**           | 2/&pi;                    | 0.6366
**`two_over_sqrtpi`**       | 2/&radic;&pi;             | 1.1284
**`sqrt2`**                 | &radic;2                  | 1.4142
**`sqrt3`**                 | &radic;3                  | 1.7321
**`inv_sqrt2`**             | 1/&radic;2                | 0.7071
**`inv_sqrt3`**             | 1/&radic;3                | 0.5774
**`egamma`**                | &gamma; (Euler constant)  | 0.5772
**`phi`**                   | &phi; (golden ratio)      | 1.6180
**Conversion factors**      |                           |
**`arcsec`**                |                           | 4.848e-6 rad
**`arcmin`**                |                           | 2.909e-4 rad
**`degree`**                |                           | 0.01745 rad
**`inch`**                  |                           | 0.0254 m
**`foot`**                  |                           | 0.3048 m
**`yard`**                  |                           | 0.9144 m
**`mile`**                  |                           | 1609 m
**`nautical_mile`**         |                           | 1852 m
**`ounce`**                 |                           | 0.02835 kg
**`pound`**                 |                           | 0.4536 kg
**`short_ton`**             |                           | 907.2 kg
**`long_ton`**              |                           | 1016 kg
**`pound_force`**           |                           | 4.448 N
**`erg`**                   |                           | 10<sup>-7</sup> J
**`foot_pound`**            |                           | 1.356 J
**`calorie`**               |                           | 4.184 J
**`kilocalorie`**           |                           | 4184 J
**`ton_tnt`**               |                           | 4.184&times;10<sup>9</sup> J
**`horsepower`**            |                           | 745.7 W
**`mmHg`**                  |                           | 133.3 Pa
**`atmosphere`**            |                           | 101300 Pa
**`zero_celsius`**          |                           | 273.15 K

## Arithmetic functions ##

* `template <typename T> constexpr T` **`c_pow`**`(T x, int y) noexcept`

Returns an integer power of a floating point value as a `constexpr`
expression. Behaviour is undefined if `x=0` and `y<0`, or if the true result
is outside the range of `T`.

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
