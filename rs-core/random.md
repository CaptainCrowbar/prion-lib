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

* `class` **`RandomBoolean`**
    * `using RandomBoolean::`**`result_type`** `= bool`
    * `RandomBoolean::`**`RandomBoolean`**`() noexcept`
    * `explicit RandomBoolean::`**`RandomBoolean`**`(Rat p) noexcept`
    * `template <typename RNG> bool RandomBoolean::`**`operator()`**`(RNG& rng) const`
    * `Rat RandomBoolean::`**`prob`**`() const noexcept`
* `inline RandomBoolean` **`random_boolean`**`()`
* `inline RandomBoolean` **`random_boolean`**`(Rat p)`
* `inline RandomBoolean` **`random_boolean`**`(int a, int b)`
* `inline RandomBoolean` **`random_boolean`**`(double p)`

Random boolean (Bernoulli) distribution. The probability of success defaults
to 1/2; it can be supplied as a rational, a pair of integers (representing a
ratio), or a floating point number. Probabilities outside the 0-1 range are
clamped to the nearest end of the range.

* `template <typename T> class` **`RandomInteger`**
    * `using RandomInteger::`**`result_type`** `= T`
    * `RandomInteger::`**`RandomInteger`**`() noexcept`
    * `RandomInteger::`**`RandomInteger`**`(T a, T b) noexcept`
    * `template <typename RNG> T RandomInteger::`**`operator()`**`(RNG& rng) const`
    * `T RandomInteger::`**`min`**`() const noexcept`
    * `T RandomInteger::`**`max`**`() const noexcept`
* `template <typename T> RandomInteger<T>` **`random_integer`**`(T t)`
* `template <typename T> RandomInteger<T>` **`random_integer`**`(T a, T b)`

Uniform random integer distribution. This returns an integer from `a` to `b`
inclusive; the bounds can be supplied in either order. If a single argument is
supplied, it returns an integer from 0 to `t-1` inclusive (always 0 if `t<2`).
A default constructed distribution always returns zero.

* `template <typename T> class` **`RandomDice`**
    * `using RandomDice::`**`result_type`** `= T`
    * `RandomDice::`**`RandomDice`**`() noexcept`
    * `RandomDice::`**`RandomDice`**`(T n, T faces) noexcept`
    * `template <typename RNG> T RandomDice::`**`operator()`**`(RNG& rng) const`
    * `T RandomDice::`**`number`**`() const noexcept`
    * `T RandomDice::`**`faces`**`() const noexcept`
* `template <typename T> RandomDice<T>` **`random_dice`**`(T n = 1, T faces = 6)`

This generates the result of rolling `n` dice, each numbered from `1` to
`faces`. The default constructor sets `n=1` and `faces=6`. This will always
return zero if either argument is less than 1.

* `template <typename T> class` **`RandomTriangleInteger`**
    * `using RandomTriangleInteger::`**`result_type`** `= T`
    * `RandomTriangleInteger::`**`RandomTriangleInteger`**`() noexcept`
    * `RandomTriangleInteger::`**`RandomTriangleInteger`**`(T hi, T lo) noexcept`
    * `template <typename RNG> T RandomTriangleInteger::`**`operator()`**`(RNG& rng) const`
    * `T RandomTriangleInteger::`**`high_end`**`() const noexcept`
    * `T RandomTriangleInteger::`**`low_end`**`() const noexcept`
* `template <typename T> RandomTriangleInteger<T>` **`random_triangle_integer`**`(T hi, T lo)`

This returns an integer with a triangular distribution, with the highest
probability at `hi` and the lowest at `lo`.

* `template <typename T> class` **`RandomReal`**
    * `using RandomReal::`**`result_type`** `= T`
    * `RandomReal::`**`RandomReal`**`() noexcept`
    * `RandomReal::`**`RandomReal`**`(T a, T b) noexcept`
    * `template <typename RNG> T RandomReal::`**`operator()`**`(RNG& rng) const`
    * `T RandomReal::`**`min`**`() const noexcept`
    * `T RandomReal::`**`max`**`() const noexcept`
* `template <typename T> RandomReal<T>` **`random_real`**`()`
* `template <typename T> RandomReal<T>` **`random_real`**`(T max)`
* `template <typename T> RandomReal<T>` **`random_real`**`(T a, T b)`

Uniform random floating point distribution. This returns a number from `a` to
`b`; the bounds can be supplied in either order. If a single argument is
supplied, it returns a number from 0 to `t` ('t' may be negative). A default
constructed distribution return a value in the unit range.

* `template <typename T> class` **`RandomNormal`**
    * `using RandomNormal::`**`result_type`** `= T`
    * `RandomNormal::`**`RandomNormal`**`() noexcept`
    * `RandomNormal::`**`RandomNormal`**`(T mean, T sd) noexcept`
    * `template <typename RNG> T RandomNormal::`**`operator()`**`(RNG& rng) const`
    * `T RandomNormal::`**`mean`**`() const noexcept`
    * `T RandomNormal::`**`sd`**`() const noexcept`
* `template <typename T> RandomNormal<T>` **`random_normal`**`()`
* `template <typename T> RandomNormal<T>` **`random_normal`**`(T mean, T sd)`

Normal (Gaussian) distribution, with the given mean and standard deviation.
The absolute value of the standard deviation is used. The default constructor
sets `mean=0` and `sd=1`.

* `template <typename T> class` **`RandomChoice`**
    * `using RandomChoice::`**`result_type`** `= T`
    * `RandomChoice::`**`RandomChoice`**`() noexcept`
    * `template <typename InputIterator> RandomChoice::`**`RandomChoice(InputIterator i, InputIterator j)`
    * `template <typename InputRange> explicit RandomChoice::`**`RandomChoice(const InputRange& list)`
    * `RandomChoice::`**`RandomChoice`**`(std::initializer_list<T> list)`
    * `template <typename RNG> const T& RandomChoice::`**`operator()`**`(RNG& rng)`
    * `bool RandomChoice::`**`empty`**`() const noexcept`
    * `size_t RandomChoice::`**`size`**`() const noexcept`
* `template <typename InputIterator> RandomChoice<[value type]>` **`random_choice`**`(InputIterator i, InputIterator j)`
* `template <typename InputRange> RandomChoice<[value type]>` **`random_choice`**`(const InputRange& list)`
* `template <typename T> RandomChoice<T>` **`random_choice`**`(std::initializer_list<T> list)`
* `template <typename ForwardIterator, typename RNG> const [value type]&` **`random_choice_from`**`(ForwardIterator i, ForwardIterator j, RNG& rng)`
* `template <typename ForwardRange, typename RNG> const [value type]&` **`random_choice_from`**`(const ForwardRange& list, RNG& rng)`
* `template <typename T, typename RNG> const T&` **`random_choice_from`**`(std::initializer_list<T> list, RNG& rng)`

Selects an element at random from the input list. The `random_choice_from()`
functions are shortcuts that can be used to avoid the overhead of copying the
list in one-time calls.

### Random distribution properties ###

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

* `class` **`UniformIntegerProperties`**
    * `explicit UniformIntegerProperties::`**`UniformIntegerProperties`**`(int n) noexcept`
    * `explicit UniformIntegerProperties::`**`UniformIntegerProperties`**`(int a, int b) noexcept`
    * `int UniformIntegerProperties::`**`min`**`() const noexcept`
    * `int UniformIntegerProperties::`**`max`**`() const noexcept`
    * `double UniformIntegerProperties::`**`mean`**`() const noexcept`
    * `double UniformIntegerProperties::`**`sd`**`() const noexcept`
    * `double UniformIntegerProperties::`**`variance`**`() const noexcept`
    * `double UniformIntegerProperties::`**`pdf`**`(int x) const noexcept`
    * `double UniformIntegerProperties::`**`cdf`**`(int x) const noexcept`
    * `double UniformIntegerProperties::`**`ccdf`**`(int x) const noexcept`

Uniform discrete distribution over the range from `a` to `b` inclusive. The
bounds will be swapped if they are in the wrong order. The constructor from a
single integer is equivalent to `UniformIntegerProperties(0,n-1)`.

* `class` **`BinomialDistributionProperties`**
    * `explicit BinomialDistributionProperties::`**`BinomialDistributionProperties`**`(int t, double p) noexcept`
    * `int BinomialDistributionProperties::`**`t`**`() const noexcept`
    * `double BinomialDistributionProperties::`**`p`**`() const noexcept`
    * `int BinomialDistributionProperties::`**`min`**`() const  noexcept`
    * `int BinomialDistributionProperties::`**`max`**`() const  noexcept`
    * `double BinomialDistributionProperties::`**`mean`**`() const noexcept`
    * `double BinomialDistributionProperties::`**`sd`**`() const  noexcept`
    * `double BinomialDistributionProperties::`**`variance`**`() const noexcept`
    * `double BinomialDistributionProperties::`**`pdf`**`(int x) const noexcept`
    * `double BinomialDistributionProperties::`**`cdf`**`(int x) const noexcept`
    * `double BinomialDistributionProperties::`**`ccdf`**`(int x) const noexcept`

Binomial distribution (representing the distribution of the number of
successes after `t` tries with success probability `p` per try). Behaviour is
undefined if `t<0`, `p<0`, or `p>1`.

* `class` **`DiceProperties`**
    * `DiceProperties::`**`DiceProperties`**`() noexcept`
    * `explicit DiceProperties::`**`DiceProperties`**`(int number, int faces = 6) noexcept`
    * `int DiceProperties::`**`number`**`() const noexcept`
    * `int DiceProperties::`**`faces`**`() const noexcept`
    * `int DiceProperties::`**`min`**`() const noexcept`
    * `int DiceProperties::`**`max`**`() const noexcept`
    * `double DiceProperties::`**`mean`**`() const noexcept`
    * `double DiceProperties::`**`sd`**`() const noexcept`
    * `double DiceProperties::`**`variance`**`() const noexcept`
    * `double DiceProperties::`**`pdf`**`(int x) const noexcept`
    * `double DiceProperties::`**`cdf`**`(int x) const noexcept`
    * `double DiceProperties::`**`ccdf`**`(int x) const noexcept`

The distribution resulting from rolling `number` dice, each numbered from 1 to
`faces` (defaulting to `number=1` and `faces=6`). If either argument is less
than 1, the distribution has a fixed value of zero.

* `class` **`UniformRealProperties`**
    * `UniformRealProperties::`**`UniformRealProperties`**`() noexcept`
    * `explicit UniformRealProperties::`**`UniformRealProperties`**`(double a, double b = 0) noexcept`
    * `double UniformRealProperties::`**`min`**`() const noexcept`
    * `double UniformRealProperties::`**`max`**`() const noexcept`
    * `double UniformRealProperties::`**`mean`**`() const noexcept`
    * `double UniformRealProperties::`**`sd`**`() const noexcept`
    * `double UniformRealProperties::`**`variance`**`() const noexcept`
    * `double UniformRealProperties::`**`pdf`**`(double x) const noexcept`
    * `double UniformRealProperties::`**`cdf`**`(double x) const noexcept`
    * `double UniformRealProperties::`**`ccdf`**`(double x) const noexcept`
    * `double UniformRealProperties::`**`quantile`**`(double p) const noexcept`
    * `double UniformRealProperties::`**`cquantile`**`(double q) const noexcept`

Uniform continuous distribution over the range from `a` to `b` (0 to 1 by
default). The bounds will be swapped if they are in the wrong order.

* `class` **`NormalDistributionProperties`**
    * `NormalDistributionProperties::`**`NormalDistributionProperties`**`() noexcept`
    * `explicit NormalDistributionProperties::`**`NormalDistributionProperties`**`(double m, double s) noexcept`
    * `double NormalDistributionProperties::`**`min`**`() const noexcept`
    * `double NormalDistributionProperties::`**`max`**`() const noexcept`
    * `double NormalDistributionProperties::`**`mean`**`() const noexcept`
    * `double NormalDistributionProperties::`**`sd`**`() const noexcept`
    * `double NormalDistributionProperties::`**`variance`**`() const noexcept`
    * `double NormalDistributionProperties::`**`pdf`**`(double x) const noexcept`
    * `double NormalDistributionProperties::`**`cdf`**`(double x) const noexcept`
    * `double NormalDistributionProperties::`**`ccdf`**`(double x) const noexcept`
    * `double NormalDistributionProperties::`**`quantile`**`(double p) const noexcept`
    * `double NormalDistributionProperties::`**`cquantile`**`(double q) const noexcept`

Normal distribution with the given mean and standard deviation (0 and 1 by
default). The sign of the standard deviation is ignored. The results of the
statistical functions are unspecified if the standard deviation is zero. The
quantile functions use the Beasley-Springer approximation: for `|z|<3.75`,
absolute error is less than `1e-6`, relative error is less than `2.5e-7`; for
`|z|<7.5`, absolute error is less than `5e-4`, relative error is less than
`5e-5`.

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

* `template <typename T> struct` **`UniqueDistribution`**
    * `using UniqueDistribution::`**`distribution_type`** `= T`
    * `using UniqueDistribution::`**`result_type`** `= [result type of T]`
    * `explicit UniqueDistribution::`**`UniqueDistribution`**`(T& t)`
    * `template <typename RNG> result_type UniqueDistribution::`**`operator()`**`(RNG& rng)`
    * `void UniqueDistribution::`**`clear`**`() noexcept`
    * `bool UniqueDistribution::`**`empty`**`() const noexcept`
    * `result_type UniqueDistribution::`**`min`**`() const`
    * `result_type UniqueDistribution::`**`max`**`() const`
    * `size_t UniqueDistribution::`**`size`**`() const noexcept`
* `template <typename T> UniqueDistribution<T>` **`unique_distribution`**`(T& t)`

This is a wrapper for another random distribution. It keeps an internal cache
of all values generated to date, and will repeatedly call the underlying
distribution until a new value is generated. The `clear()` function resets the
cache.

The underlying distribution only needs to have a function call operator that
can take a reference to a random number engine, unless the `min()` or `max()`
functions are called, which require the corresponding functions on the
underlying distribution.

The `UniqueDistribution` object contains a reference to the underlying
distribution, which is expected to remain valid. Behaviour is undefined if the
underlying distribution is destroyed while a `UniqueDistribution` object still
has a reference to it, or if `UniqueDistribution::operator()` is called when
the cache already contains every possible value of the result type.

### Weighted choice distribution ###

* `template <typename T, typename F = double> class` **`WeightedChoice`**
    * `using WeightedChoice::`**`frequency_type`** `= F`
    * `using WeightedChoice::`**`result_type`** `= T`
    * `WeightedChoice::`**`WeightedChoice`**`() noexcept`
    * `WeightedChoice::`**`WeightedChoice`**`(initializer_list<pair<T, F>> pairs)`
    * `template <typename InputRange> explicit WeightedChoice::`**`WeightedChoice`**`(const InputRange& pairs)`
    * `template <typename RNG> T WeightedChoice::`**`operator()`**`(RNG& rng) const`
    * `void WeightedChoice::`**`add`**`(const T& t, F f)`
    * `void WeightedChoice::`**`append`**`(initializer_list<pair<T, F>> pairs)`
    * `template <typename InputRange> void WeightedChoice::`**`append`**`(const InputRange& pairs)`
    * `bool WeightedChoice::`**`empty`**`() const noexcept`

Selects items at random from a weighted list. The frequency type (`F`) must be
an arithmetic type; it can be an integer or floating point type. The default
constructor creates an empty list; the other constructors copy a list of
`(value,frequency)` pairs. The `add()` and `append()` functions can be used to
add `(value,frequency)` pairs, one at a time or in bulk. Entries with a
frequency less than or equal to zero are ignored. The function call operator
will return a default constructed `T` if the list is empty.

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
