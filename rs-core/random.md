# Random Numbers #

By Ross Smith

* `#include "rs-core/random.hpp"`

## Contents ##

[TOC]

## Random number sources ##

### LCG functions ###

* `constexpr uint32_t` **`lcg32`**`(uint32_t x) noexcept { return 32310901ul * x + 850757001ul; }`
* `constexpr uint64_t` **`lcg64`**`(uint64_t x) noexcept { return 3935559000370003845ull * x + 8831144850135198739ull; }`

Good LCG transformations for 32 and 64 bit integers,
from Pierre L'Ecuyer (1999),
["Tables of Linear Congruential Generators of Different Sizes and Good Lattice Structure"](http://www.ams.org/journals/mcom/1999-68-225/S0025-5718-99-00996-5/S0025-5718-99-00996-5.pdf).

* `class` **`Lcg32`**
    * `using Lcg32::`**`result_type`** `= uint32_t`
    * `constexpr Lcg32::`**`Lcg32`**`() noexcept`
    * `explicit constexpr Lcg32::`**`Lcg32`**`(uint32_t s) noexcept`
    * `constexpr uint32_t Lcg32::`**`operator()`**`() noexcept`
    * `constexpr bool Lcg32::`**`operator==`**`(const Lcg32& rhs) const noexcept`
    * `constexpr bool Lcg32::`**`operator!=`**`(const Lcg32& rhs) const noexcept`
    * `constexpr void Lcg32::`**`seed`**`(uint32_t s) noexcept`
    * `static constexpr uint32_t Lcg32::`**`min`**`() noexcept` _= 0_
    * `static constexpr uint32_t Lcg32::`**`max`**`() noexcept` _= 2<sup>32</sup>-1_
* `class` **`Lcg64`**
    * `using Lcg64::`**`result_type`** `= uint64_t`
    * `constexpr Lcg64::`**`Lcg64`**`() noexcept`
    * `explicit constexpr Lcg64::`**`Lcg64`**`(uint64_t s) noexcept`
    * `constexpr uint64_t Lcg64::`**`operator()`**`() noexcept`
    * `constexpr bool Lcg64::`**`operator==`**`(const Lcg64& rhs) const noexcept`
    * `constexpr bool Lcg64::`**`operator!=`**`(const Lcg64& rhs) const noexcept`
    * `constexpr void Lcg64::`**`seed`**`(uint64_t s) noexcept`
    * `static constexpr uint64_t Lcg64::`**`min`**`() noexcept` _= 0_
    * `static constexpr uint64_t Lcg64::`**`max`**`() noexcept` _= 2<sup>64</sup>-1_

Random number engine classes based on the above functions.

### ISAAC generators ###

* `class` **`Isaac32`**
    * `using Isaac32::`**`result_type`** `= uint32_t`
    * `Isaac32::`**`Isaac32`**`() noexcept`
    * `explicit Isaac32::`**`Isaac32`**`(uint32_t s) noexcept`
    * `Isaac32::`**`Isaac32`**`(const uint32_t* sptr, size_t len) noexcept`
    * `Isaac32::`**`Isaac32`**`(std::initializer_list<uint32_t> s) noexcept`
    * `uint32_t Isaac32::`**`operator()`**`() noexcept`
    * `void Isaac32::`**`seed`**`() noexcept`
    * `void Isaac32::`**`seed`**`(uint32_t s) noexcept`
    * `void Isaac32::`**`seed`**`(const uint32_t* sptr, size_t len) noexcept`
    * `void Isaac32::`**`seed`**`(std::initializer_list<uint32_t> s) noexcept`
    * `static constexpr uint32_t Isaac32::`**`min`**`() noexcept` _= 0_
    * `static constexpr uint32_t Isaac32::`**`max`**`() noexcept` _= 2<sup>32</sup>-1_
* `class` **`Isaac64`**
    * `using Isaac64::`**`result_type`** `= uint64_t`
    * `Isaac64::`**`Isaac64`**`() noexcept`
    * `explicit Isaac64::`**`Isaac64`**`(uint64_t s) noexcept`
    * `Isaac64::`**`Isaac64`**`(const uint64_t* sptr, size_t len) noexcept`
    * `Isaac64::`**`Isaac64`**`(std::initializer_list<uint64_t> s) noexcept`
    * `uint64_t Isaac64::`**`operator()`**`() noexcept`
    * `void Isaac64::`**`seed`**`() noexcept`
    * `void Isaac64::`**`seed`**`(uint64_t s) noexcept`
    * `void Isaac64::`**`seed`**`(const uint64_t* sptr, size_t len) noexcept`
    * `void Isaac64::`**`seed`**`(std::initializer_list<uint64_t> s) noexcept`
    * `static constexpr uint64_t Isaac64::`**`min`**`() noexcept` _= 0_
    * `static constexpr uint64_t Isaac64::`**`max`**`() noexcept` _= 2<sup>64</sup>-1_

ISAAC cryptographic quality generator by [Bob Jenkins](http://burtleburtle.net/bob/rand/isaacafa.html).

### PCG generators ###

* `class` **`Pcg32`**
    * `using Pcg32::`**`result_type`** `= uint32_t`
    * `constexpr Pcg32::`**`Pcg32`**`() noexcept`
    * `constexpr explicit Pcg32::`**`Pcg32`**`(uint64_t s) noexcept`
    * `constexpr uint32_t Pcg32::`**`operator()`**`() noexcept`
    * `constexpr void Pcg32::`**`advance`**`(int64_t offset) noexcept`
    * `constexpr void Pcg32::`**`seed`**`(uint64_t s) noexcept`
    * `static constexpr uint32_t Pcg32::`**`min`**`() noexcept` _= 0_
    * `static constexpr uint32_t Pcg32::`**`max`**`() noexcept` _= 2<sup>32</sup>-1_
* `class` **`Pcg64`**
    * `using Pcg64::`**`result_type`** `= uint64_t`
    * `constexpr Pcg64::`**`Pcg64`**`() noexcept`
    * `constexpr explicit Pcg64::`**`Pcg64`**`(Uint128 s) noexcept`
    * `constexpr explicit Pcg64::`**`Pcg64`**`(uint64_t hi, uint64_t lo) noexcept`
    * `constexpr uint64_t Pcg64::`**`operator()`**`() noexcept`
    * `constexpr void Pcg64::`**`advance`**`(int64_t offset) noexcept`
    * `constexpr void Pcg64::`**`seed`**`(Uint128 s) noexcept`
    * `constexpr void Pcg64::`**`seed`**`(uint64_t hi, uint64_t lo) noexcept`
    * `static constexpr uint64_t Pcg64::`**`min`**`() noexcept` _= 0_
    * `static constexpr uint64_t Pcg64::`**`max`**`() noexcept` _= 2<sup>64</sup>-1_

PCG generators by [Melissa O'Neill](http://www.pcg-random.org/).

### Xoshiro and related generators ###

* `class` **`SplitMix64`**
    * `using SplitMix64::`**`result_type`** `= uint64_t`
    * `constexpr SplitMix64::`**`SplitMix64`**`() noexcept: x(0)`
    * `constexpr explicit SplitMix64::`**`SplitMix64`**`(uint64_t s) noexcept: x (s)`
    * `constexpr uint64_t SplitMix64::`**`operator()`**`() noexcept`
    * `constexpr void SplitMix64::`**`seed`**`(uint64_t s = 0) noexcept`
    * `static constexpr uint64_t SplitMix64::`**`min`**`() noexcept`
    * `static constexpr uint64_t SplitMix64::`**`max`**`() noexcept`
* `class` **`Xoroshiro64s`**
* `class` **`Xoroshiro64ss`**
* `class` **`Xoroshiro128p`**
* `class` **`Xoroshiro128ss`**
* `class` **`Xoshiro128p`**
* `class` **`Xoshiro128ss`**
* `class` **`Xoshiro256p`**
* `class` **`Xoshiro256ss`**
    * `using [Xoroshiro64,Xoshiro128]::`**`result_type`** `= uint32_t`
    * `using [Xoroshiro128,Xoshiro256]::`**`result_type`** `= uint64_t`
    * `constexpr [Class]::`**`[Class]`**`() noexcept`
    * `constexpr explicit [Class]::`**`[Class]`**`(result_type s) noexcept`
    * `constexpr [Xoroshiro]::`**`[Class]`**`(result_type s, result_type t) noexcept`
    * `constexpr [Xoshiro]::`**`[Class]`**`(result_type s, result_type t, result_type u, result_type v) noexcept`
    * `constexpr result_type [Class]::`**`operator()`**`() noexcept`
    * `constexpr void [Class]::`**`seed`**`(result_type s = 0) noexcept`
    * `constexpr void [Class]::`**`seed`**`(result_type s, result_type t) noexcept`
    * `constexpr void [Xoshiro]::`**`seed`**`(result_type s, result_type t, result_type u, result_type v) noexcept`
    * `static constexpr result_type [Class]::`**`min`**`() noexcept`
    * `static constexpr result_type [Class]::`**`max`**`() noexcept`
* `using` **`Xoshiro`** `= Xoshiro256ss`

Xoshiro and related algorithms by [David Blackman and Sebastiano Vigna](http://xoshiro.di.unimi.it/).

Algorithm         | Result      | State     | Seeds                  | Recommendation
---------         | ------      | -----     | -----                  | --------------
`SplitMix64`      | `uint64_t`  | 8 bytes   | 1 `uint64_t`           | Used internally in `Xoshiro/Xoroshiro`; not recommended for general purpose
`Xoroshiro64s`    | `uint32_t`  | 8 bytes   | 1 or 2 `uint32_t`      | Substitute for `Xoshiro128p` when small state is important
`Xoroshiro64ss`   | `uint32_t`  | 8 bytes   | 1 or 2 `uint32_t`      | Substitute for `Xoshiro128ss` when small state is important
`Xoroshiro128p`   | `uint64_t`  | 16 bytes  | 1 or 2 `uint64_t`      | Substitute for `Xoshiro256p` when small state is important
`Xoroshiro128ss`  | `uint64_t`  | 16 bytes  | 1 or 2 `uint64_t`      | Substitute for `Xoshiro256ss` when small state is important
`Xoshiro128p`     | `uint32_t`  | 16 bytes  | 1, 2, or 4 `uint32_t`  | Faster than `Xoshiro128ss`, but low bits are poor
`Xoshiro128ss`    | `uint32_t`  | 16 bytes  | 1, 2, or 4 `uint32_t`  | Best general purpose 32-bit
`Xoshiro256p`     | `uint64_t`  | 32 bytes  | 1, 2, or 4 `uint64_t`  | Faster than `Xoshiro256ss`, but low bits are poor
`Xoshiro256ss`    | `uint64_t`  | 32 bytes  | 1, 2, or 4 `uint64_t`  | Best general purpose
`Xoshiro`         | `uint64_t`  | 32 bytes  | 1, 2, or 4 `uint64_t`  | Synonym for `Xoshiro256ss`

## Random distributions ##

### Basic random distributions ###

These are similar to the standard distribution classes, but provide portable
deterministic behaviour: given the same pseudo-random number engine (the `RNG`
type), these can be relied on to return the same values on different systems
and compilers (apart from small differences due to rounding errors in the
floating point distributions).

Each distribution (apart from the boolean distribution) is provided both as a
class template, which requires the result type to be supplied explicitly, and
as one or more functions that deduce the result type and return an instance of
the class.

Most of these also provide statistical functions that return properties of the
distribution:

The `pdf()`, `cdf()`, and `ccdf()` functions represent the probability
distribution function, cumulative distribution function, and complementary
cumulative distribution function. For discrete distributions, `pdf(x)` is the
probably of a result exactly equal to `x`, `cdf(x)` is the probability of a
result less than or equal to `x`, and `ccdf(x)` is the probability of a result
greater than or equal to `x` (`cdf+ccdf-pdf=1`). For continuous distributions,
`pdf(x)` is the probably density at `x`, `cdf(x)` is the probability of a
result less than `x`, and `ccdf(x)` is the probability of a result greater
than `x` (`cdf+ccdf=1`).

The `quantile()` and `cquantile()` (complementary quantile) functions are only
defined for continuous distributions, and are the inverse of `cdf()` and
`ccdf()` respectively. Behaviour is undefined if the argument to one of the
quantile functions is less than 0 or greater than 1; it may or may not be
defined for exactly 0 or 1, depending on how the distribution is bounded.

* `template <typename T> class` **`UniformInteger`**
    * `using UniformInteger::`**`result_type`** `= T`
    * `UniformInteger::`**`UniformInteger`**`() noexcept`
    * `UniformInteger::`**`UniformInteger`**`(T a, T b) noexcept`
    * `template <typename RNG> T UniformInteger::`**`operator()`**`(RNG& rng) const`
    * `T UniformInteger::`**`min`**`() const noexcept`
    * `T UniformInteger::`**`max`**`() const noexcept`
    * `Rational<T> UniformInteger::`**`mean`**`() const noexcept`
    * `Rational<T> UniformInteger::`**`variance`**`() const noexcept`
    * `double UniformInteger::`**`sd`**`() const noexcept`
    * `double UniformInteger::`**`pdf`**`(T x) const noexcept`
    * `double UniformInteger::`**`cdf`**`(T x) const noexcept`
    * `double UniformInteger::`**`ccdf`**`(T x) const noexcept`
* `template <typename T> UniformInteger<T>` **`random_integer`**`(T t)`
* `template <typename T> UniformInteger<T>` **`random_integer`**`(T a, T b)`

Uniform random integer distribution. This returns an integer from `a` to `b`
inclusive; the bounds can be supplied in either order. If a single argument is
supplied, it returns an integer from 0 to `t-1` inclusive (always 0 if `t<2`).
A default constructed distribution always returns zero.

* `class` **`Bernoulli`**
    * `using Bernoulli::`**`result_type`** `= bool`
    * `Bernoulli::`**`Bernoulli`**`() noexcept`
    * `explicit Bernoulli::`**`Bernoulli`**`(Rat p) noexcept`
    * `template <typename RNG> bool Bernoulli::`**`operator()`**`(RNG& rng) const`
    * `Rat Bernoulli::`**`prob`**`() const noexcept`
* `Bernoulli` **`random_boolean`**`()`
* `Bernoulli` **`random_boolean`**`(Rat p)`
* `Bernoulli` **`random_boolean`**`(int a, int b)`
* `Bernoulli` **`random_boolean`**`(double p)`

Random boolean (Bernoulli) distribution. The probability of success defaults
to 1/2; it can be supplied as a rational, a pair of integers (representing a
ratio), or a floating point number. Probabilities outside the 0-1 range are
clamped to the nearest end of the range.

* `template <typename T> class` **`Binomial`**
    * `using Binomial::`**`result_type`** `= T`
    * `Binomial::`**`Binomial`**`() noexcept`
    * `Binomial::`**`Binomial`**`(const Rational<T>& p, T n) noexcept`
    * `template <typename RNG> bool Binomial::`**`operator()`**`(RNG& rng) const`
    * `Rational<T> Binomial::`**`prob`**`() const noexcept`
    * `T Binomial::`**`number`**`() const noexcept`
    * `T Binomial::`**`min`**`() const  noexcept`
    * `T Binomial::`**`max`**`() const  noexcept`
    * `Rational<T> Binomial::`**`mean`**`() const noexcept`
    * `Rational<T> Binomial::`**`variance`**`() const noexcept`
    * `double Binomial::`**`sd`**`() const  noexcept`
    * `double Binomial::`**`pdf`**`(T x) const noexcept`
    * `double Binomial::`**`cdf`**`(T x) const noexcept`
    * `double Binomial::`**`ccdf`**`(T x) const noexcept`
* `template <typename T> Binomial` **`random_boolean`**`(const Rational<T>& p, T n) noexcept`

Binomial distribution, reporting the number of successes after `n` tests each
with probability `p`. The argument `p` is clamped to the 0-1 range; the result
is always zero if `n<1`.

* `template <typename T> class` **`Dice`**
    * `using Dice::`**`result_type`** `= T`
    * `Dice::`**`Dice`**`() noexcept`
    * `Dice::`**`Dice`**`(T n, T faces) noexcept`
    * `template <typename RNG> T Dice::`**`operator()`**`(RNG& rng) const`
    * `T Dice::`**`number`**`() const noexcept`
    * `T Dice::`**`faces`**`() const noexcept`
    * `T Dice::`**`min`**`() const noexcept`
    * `T Dice::`**`max`**`() const noexcept`
    * `Rational<T> Dice::`**`mean`**`() const noexcept`
    * `Rational<T> Dice::`**`variance`**`() const noexcept`
    * `double Dice::`**`sd`**`() const noexcept`
    * `double Dice::`**`pdf`**`(T x) const noexcept`
    * `double Dice::`**`cdf`**`(T x) const noexcept`
    * `double Dice::`**`ccdf`**`(T x) const noexcept`
* `template <typename T> Dice<T>` **`random_dice`**`(T n = 1, T faces = 6)`

This generates the result of rolling `n` dice, each numbered from `1` to
`faces`. The default constructor sets `n=1` and `faces=6`. This will always
return zero if either argument is less than 1.

* `template <typename T> class` **`TriangleInteger`**
    * `using TriangleInteger::`**`result_type`** `= T`
    * `TriangleInteger::`**`TriangleInteger`**`() noexcept`
    * `TriangleInteger::`**`TriangleInteger`**`(T hi, T lo) noexcept`
    * `template <typename RNG> T TriangleInteger::`**`operator()`**`(RNG& rng) const`
    * `T TriangleInteger::`**`high_end`**`() const noexcept`
    * `T TriangleInteger::`**`low_end`**`() const noexcept`
* `template <typename T> TriangleInteger<T>` **`random_triangle_integer`**`(T hi, T lo)`

This returns an integer with a triangular distribution, with the highest
probability at `hi` and the lowest at `lo`.

* `template <typename T> class` **`UniformReal`**
    * `using UniformReal::`**`result_type`** `= T`
    * `UniformReal::`**`UniformReal`**`() noexcept`
    * `UniformReal::`**`UniformReal`**`(T a, T b) noexcept`
    * `template <typename RNG> T UniformReal::`**`operator()`**`(RNG& rng) const`
    * `T UniformReal::`**`min`**`() const noexcept`
    * `T UniformReal::`**`max`**`() const noexcept`
    * `T UniformReal::`**`mean`**`() const noexcept`
    * `T UniformReal::`**`sd`**`() const noexcept`
    * `T UniformReal::`**`variance`**`() const noexcept`
    * `T UniformReal::`**`pdf`**`(T x) const noexcept`
    * `T UniformReal::`**`cdf`**`(T x) const noexcept`
    * `T UniformReal::`**`ccdf`**`(T x) const noexcept`
    * `T UniformReal::`**`quantile`**`(T p) const noexcept`
    * `T UniformReal::`**`cquantile`**`(T q) const noexcept`
* `template <typename T> UniformReal<T>` **`random_real`**`()`
* `template <typename T> UniformReal<T>` **`random_real`**`(T max)`
* `template <typename T> UniformReal<T>` **`random_real`**`(T a, T b)`

Uniform random floating point distribution. This returns a number from `a` to
`b`; the bounds can be supplied in either order. If a single argument is
supplied, it returns a number from 0 to `t` ('t' may be negative). A default
constructed distribution return a value in the unit range.

* `template <typename T> class` **`Normal`**
    * `using Normal::`**`result_type`** `= T`
    * `Normal::`**`Normal`**`() noexcept`
    * `Normal::`**`Normal`**`(T mean, T sd) noexcept`
    * `template <typename RNG> T Normal::`**`operator()`**`(RNG& rng) const`
    * `T Normal::`**`mean`**`() const noexcept`
    * `T Normal::`**`sd`**`() const noexcept`
    * `T Normal::`**`variance`**`() const noexcept`
    * `T Normal::`**`pdf`**`(T x) const noexcept`
    * `T Normal::`**`cdf`**`(T x) const noexcept`
    * `T Normal::`**`ccdf`**`(T x) const noexcept`
    * `T Normal::`**`quantile`**`(T p) const noexcept`
    * `T Normal::`**`cquantile`**`(T q) const noexcept`
* `template <typename T> Normal<T>` **`random_normal`**`()`
* `template <typename T> Normal<T>` **`random_normal`**`(T mean, T sd)`

Normal (Gaussian) distribution, with the given mean and standard deviation.
The absolute value of the standard deviation is used. The default constructor
sets `mean=0` and `sd=1`. The quantile functions use the Beasley-Springer
approximation: for `|z|<3.75`, absolute error is less than `1e-6`, relative
error is less than `2.5e-7`; for `|z|<7.5`, absolute error is less than
`5e-4`, relative error is less than `5e-5`. Behaviour is undefined if the
distribution property functions are called when the standard deviation is
zero.

* `template <typename T> class` **`DiscreteNormal`**
    * `using DiscreteNormal::`**`result_type`** `= T`
    * `DiscreteNormal::`**`DiscreteNormal`**`()`
    * `DiscreteNormal::`**`DiscreteNormal`**`(const Rational<T>& mean, const Rational<T>& sd)`
    * `template <typename RNG> T DiscreteNormal::`**`operator()`**`(RNG& rng) const`
    * `Rational<T> DiscreteNormal::`**`mean`**`() const noexcept`
    * `Rational<T> DiscreteNormal::`**`sd`**`() const noexcept`
* `template <typename T> DiscreteNormal<T>` **`random_discrete_normal`**`(const Rational<T>& mean, const Rational<T>& sd)`

Generates an integer that follows a good deterministic approximation to the
result of generating a normally distributed real value and then rounding it to
an integer.

* `template <typename T, typename S = double> class` **`Poisson`**
    * `using Poisson::`**`result_type`** `= T`
    * `using Poisson::`**`scalar_type`** `= S`
    * `Poisson::`**`Poisson`**`()`
    * `explicit Poisson::`**`Poisson`**`(S lambda) noexcept`
    * `template <typename RNG> T Poisson::`**`operator()`**`(RNG& rng) const`
    * `S Poisson::`**`lambda`**`() const noexcept`
    * `S Poisson::`**`mean`**`() const noexcept`
    * `S Poisson::`**`variance`**`() const noexcept`
    * `S Poisson::`**`sd`**`() const noexcept`
    * `S Poisson::`**`pdf`**`(T x) const noexcept`
    * `S Poisson::`**`cdf`**`(T x) const noexcept`
    * `S Poisson::`**`ccdf`**`(T x) const noexcept`

Poisson distribution. `T` and `S` must be integer and floating point types
respectively. The default constructor sets `lambda=1`. Behaviour is undefined
if `lambda<=0`.

* `template <typename T> class` **`Beta`**
    * `using Beta::`**`result_type`** `= T`
    * `Beta::`**`Beta`**`() noexcept`
    * `Beta::`**`Beta`**`(T a, T b) noexcept`
    * `template <typename RNG> T Beta::`**`operator()`**`(RNG& rng) const`
    * `T Beta::`**`alpha`**`() const noexcept`
    * `T Beta::`**`beta`**`() const noexcept`
    * `T Beta::`**`mean`**`() const noexcept`
    * `T Beta::`**`variance`**`() const noexcept`
    * `T Beta::`**`sd`**`() const noexcept`
    * `T Beta::`**`pdf`**`(T x) const noexcept`
    * `T Beta::`**`cdf`**`(T x) const noexcept`
    * `T Beta::`**`ccdf`**`(T x) const noexcept`
    * `T Beta::`**`quantile`**`(T p) const noexcept`
    * `T Beta::`**`cquantile`**`(T q) const noexcept`

Beta distribution. The default constructor sets `a=b=1`, which produces a
uniform distribution. Behaviour is undefined if `a<=0` or `b<=0`.

### Random choice distributions ###

* `template <typename T> class` **`Choice`**
    * `using Choice::`**`result_type`** `= T`
    * `using Choice::`**`value_type`** `= T`
    * `Choice::`**`Choice`**`() noexcept`
    * `template <typename InputIterator> Choice::`**`Choice`**`(InputIterator i, InputIterator j)`
    * `template <typename InputRange> explicit Choice::`**`Choice`**`(const InputRange& list)`
    * `Choice::`**`Choice`**`(std::initializer_list<T> list)`
    * `template <typename RNG> const T& Choice::`**`operator()`**`(RNG& rng) const`
    * `void Choice::`**`add`**`(const T& t)`
    * `void Choice::`**`push_back`**`(const T& t)`
    * `void Choice::`**`append`**`(initializer_list<T> list)`
    * `template <typename InputRange> void Choice::`**`append`**`(const InputRange& list)`
    * `void Choice::`**`clear`**`() noexcept`
    * `bool Choice::`**`empty`**`() const noexcept`
    * `size_t Choice::`**`size`**`() const noexcept`
* `template <typename InputIterator> Choice<[value type]>` **`random_choice`**`(InputIterator i, InputIterator j)`
* `template <typename InputRange> Choice<[value type]>` **`random_choice`**`(const InputRange& list)`
* `template <typename T> Choice<T>` **`random_choice`**`(std::initializer_list<T> list)`
* `template <typename ForwardIterator, typename RNG> const [value type]&` **`random_choice_from`**`(ForwardIterator i, ForwardIterator j, RNG& rng)`
* `template <typename ForwardRange, typename RNG> const [value type]&` **`random_choice_from`**`(const ForwardRange& list, RNG& rng)`
* `template <typename T, typename RNG> const T&` **`random_choice_from`**`(std::initializer_list<T> list, RNG& rng)`

Selects an element at random from the input list. The `add()` and
`push_back()` functions are synonyms. The `random_choice_from()` functions are
shortcuts that can be used to avoid the overhead of copying the list in
one-time calls. Behaviour is undefined if the list is empty.

* `template <typename T, typename F = double> class` **`WeightedChoice`**
    * `using WeightedChoice::`**`frequency_type`** `= F`
    * `using WeightedChoice::`**`result_type`** `= T`
    * `using WeightedChoice::`**`value_type`** `= std::pair<T, F>`
    * `WeightedChoice::`**`WeightedChoice`**`() noexcept`
    * `WeightedChoice::`**`WeightedChoice`**`(initializer_list<std::pair<T, F>> pairs)`
    * `template <typename InputRange> explicit WeightedChoice::`**`WeightedChoice`**`(const InputRange& pairs)`
    * `template <typename RNG> const T& WeightedChoice::`**`operator()`**`(RNG& rng) const`
    * `void WeightedChoice::`**`add`**`(const T& t, F f)`
    * `void WeightedChoice::`**`add`**`(const value_type& pair)`
    * `void WeightedChoice::`**`push_back`**`(const value_type& pair)`
    * `void WeightedChoice::`**`append`**`(initializer_list<std::pair<T, F>> pairs)`
    * `template <typename InputRange> void WeightedChoice::`**`append`**`(const InputRange& pairs)`
    * `void WeightedChoice::`**`clear`**`() noexcept`
    * `bool WeightedChoice::`**`empty`**`() const noexcept`

Selects items at random from a weighted list. The frequency type (`F`) must be
an arithmetic type; it can be an integer or floating point type. The default
constructor creates an empty list; the other constructors copy a list of
`(value,frequency)` pairs. The `add()` and `append()` functions can be used to
add `(value,frequency)` pairs, one at a time or in bulk (`push_back()` is a
synonym for `add()`). Entries with a frequency less than or equal to zero are
ignored. Behaviour is undefined if the function call operator is called on an
empty set.


### Random samples ###

* `template <typename ForwardRange, typename RNG> vector<[value type]>` **`random_sample_from`**`(const ForwardRange& range, size_t k, RNG& rng)`

Returns a random sample of `k` elements from the input range. This will throw
`std::length_error` if `k` is larger than the size of the range.

### Spatial distributions ###

* `template <typename T, size_t N> class` **`RandomVector`**
    * `using RandomVector::`**`result_type`** `= Vector<T, N>`
    * `using RandomVector::`**`scalar_type`** `= T`
    * `static constexpr size_t RandomVector::`**`dim`** `= N`
    * `RandomVector::`**`RandomVector`**`()`
    * `explicit RandomVector::`**`RandomVector`**`(T t)`
    * `explicit RandomVector::`**`RandomVector`**`(const Vector<T, N>& v)`
    * `template <typename RNG> Vector<T, N> RandomVector::`**`operator()`**`(RNG& rng) const`
    * `Vector<T, N> RandomVector::`**`scale`**`() const`
* `template <typename T, size_t N> class` **`SymmetricRandomVector`**
    * `using SymmetricRandomVector::`**`result_type`** `= Vector<T, N>`
    * `using SymmetricRandomVector::`**`scalar_type`** `= T`
    * `static constexpr size_t SymmetricRandomVector::`**`dim`** `= N`
    * `SymmetricRandomVector::`**`SymmetricRandomVector`**`()`
    * `explicit SymmetricRandomVector::`**`SymmetricRandomVector`**`(T t)`
    * `explicit SymmetricRandomVector::`**`SymmetricRandomVector`**`(const Vector<T, N>& v)`
    * `template <typename RNG> Vector<T, N> SymmetricRandomVector::`**`operator()`**`(RNG& rng) const`
    * `Vector<T, N> SymmetricRandomVector::`**`scale`**`() const`

`RandomVector` generates a random vector in the rectilinear volume between the
origin and the vector supplied to the constructor (the unit cube by default).
`SymmetricRandomVector` generates a vector in a volume symmetrical about the
origin, between `v` and `-v`. The `scale()` function returns the vector
supplied to the constructor.

* `template <typename T, size_t N> class` **`RandomInSphere`**
    * `using RandomInSphere::`**`result_type`** `= Vector<T, N>`
    * `using RandomInSphere::`**`scalar_type`** `= T`
    * `static constexpr size_t RandomInSphere::`**`dim`** `= N`
    * `RandomInSphere::`**`RandomInSphere`**`()`
    * `explicit RandomInSphere::`**`RandomInSphere`**`(T r)`
    * `template <typename RNG> Vector<T, N> RandomInSphere::`**`operator()`**`(RNG& rng) const`
    * `T RandomInSphere::`**`radius`**`() const noexcept`
* `template <typename T, size_t N> class` **`RandomOnSphere`**
    * `using RandomOnSphere::`**`result_type`** `= Vector<T, N>`
    * `using RandomOnSphere::`**`scalar_type`** `= T`
    * `static constexpr size_t RandomOnSphere::`**`dim`** `= N`
    * `RandomOnSphere::`**`RandomOnSphere`**`()`
    * `explicit RandomOnSphere::`**`RandomOnSphere`**`(T r)`
    * `template <typename RNG> Vector<T, N> RandomOnSphere::`**`operator()`**`(RNG& rng) const`
    * `T RandomOnSphere::`**`radius`**`() const noexcept`

These generate a random point inside or on the surface of a sphere in
`N`-dimensional space. `T` must be a floating point arithmetic type; `N` must
be a positive integer. The radius of the sphere defaults to 1; the absolute
value of the radius is used if it is negative. Note that, because of the
limitations of floating point arithmetic, the point generated by
`RandomOnSphere` cannot be guaranteed to be exactly on the surface.

### Unique distribution ###

* `template <typename T> struct` **`UniqueGenerator`**
    * `using UniqueGenerator::`**`distribution_type`** `= T`
    * `using UniqueGenerator::`**`result_type`** `= [result type of T]`
    * `explicit UniqueGenerator::`**`UniqueGenerator`**`(T& t)`
    * `template <typename RNG> result_type UniqueGenerator::`**`operator()`**`(RNG& rng)`
    * `void UniqueGenerator::`**`clear`**`() noexcept`
    * `bool UniqueGenerator::`**`empty`**`() const noexcept`
    * `result_type UniqueGenerator::`**`min`**`() const`
    * `result_type UniqueGenerator::`**`max`**`() const`
    * `size_t UniqueGenerator::`**`size`**`() const noexcept`
* `template <typename T> UniqueGenerator<T>` **`unique_distribution`**`(T& t)`

This is a wrapper for another random distribution. It keeps an internal cache
of all values generated to date, and will repeatedly call the underlying
distribution until a new value is generated. The `clear()` function resets the
cache.

The underlying distribution only needs to have a function call operator that
can take a reference to a random number engine, unless the `min()` or `max()`
functions are called, which require the corresponding functions on the
underlying distribution.

The `UniqueGenerator` object contains a reference to the underlying
distribution, which is expected to remain valid. Behaviour is undefined if the
underlying distribution is destroyed while a `UniqueGenerator` object still
has a reference to it, or if `UniqueGenerator::operator()` is called when the
cache already contains every possible value of the result type.

## Other random algorithms ##

* `template <typename RNG> void` **`random_bytes`**`(RNG& rng, void* ptr, size_t n)`

Fills a block of memory with random bits generated by the RNG. This will do
nothing if `ptr` is null or `n=0`.

* `template <typename RNG1, typename RNG2> void` **`seed_from`**`(RNG1& src, RNG2& dst)`

Seed one standard random engine from another, by constructing an intermediate
`seed_seq`.

* `template <typename RNG, typename RandomAccessRange> void` **`shuffle`**`(RNG& rng, RandomAccessRange& range)`
* `template <typename RNG, typename RandomAccessIterator> void` **`shuffle`**`(RNG& rng, RandomAccessIterator i, RandomAccessIterator j)`

Shuffle the elements of a random access range into random order. This is
functionally the same as `std::shuffle()`, duplicated here to provide a
version with reproducible behaviour on all systems.

## Text generators ##

* `Ustring` **`lorem_ipsum`**`(uint64_t seed, size_t bytes, bool paras = true)`

Generates random _Lorem Ipsum_ text with approximately the specified number of
bytes (the length will be between `bytes` and `bytes+15` inclusive). Normally
it will be broken into paragraphs, delimited by two line feeds; if `paras` is
false, there will be no paragraph breaks, and no trailing LF.
