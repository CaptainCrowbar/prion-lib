# RPG-Style Dice #

By Ross Smith

* `#include "rs-core/rpg-dice.hpp"`

## Contents ##

[TOC]

## Class RpgDice ##

* `template <typename T, typename S = double> class` **`RpgDice`**
* `using` **`IntDice`** `= RpgDice<int>`

The `RpgDice` class is a random number distribution that emulates rolling
multiple dice of different kinds, and applying some limited arithmetic to the
results. The first template argument type `T` must be an integer type; the
scalar type `S` must be a floating point type. A set of dice can be specified
either by building it up from instances of the `RpgDice` class, or by parsing
a string in a conventional format.

Examples:

    // These both produce the same distribution
    auto dice1 = IntDice(2, 10) * 5 + IntDice(3, 6) + 10;
    auto dice2 = IntDice("2d10x5 + 3d6 + 10");

* `using RpgDice::`**`integer_type`** `= T`
* `using RpgDice::`**`rational_type`** `= Rational<T>`
* `using RpgDice::`**`scalar_type`** `= S`

Types used in the class. `rational_type` is the output type of the generator.

* `RpgDice::`**`RpgDice`**`() noexcept`

Creates a null dice roller, which always yields zero.

* `explicit RpgDice::`**`RpgDice`**`(T n, T faces = 6, const Rational<T>& factor = 1)`

Creates an object that rolls `n` dice, each numbered from 1 to `faces`,
optionally multiplying the result by `factor`. This will produce a null
(always zero) dice roller if any of the arguments is zero; it will throw
`std::invalid_argument` if `n` or `faces` is negative.

* `explicit RpgDice::`**`RpgDice`**`(Uview str)`

Creates a dice roller by parsing the string representation.

A dice representation consists of any number of dice groups or numeric
modifiers. A dice group can consist of an optional number of dice (1 by
default), the letter `"D"` (case insensitive), and an optional number of faces
(6 by default). For example, `"3d6"` means "roll three six-sided dice and add
the results" (and could also be written `"3D"`).

A group can be preceded or followed by an integer multiplier, delimited by
either a star or the letter `"X"` (case insensitive), and followed by a
divisor, delimited by a slash. For example, `"3d6x2/3"` means "roll 3d6 and
multiply the result by two thirds". The returned value always keeps fractions
intact, it does not round to an integer.

The string can also add or subtract constant integers or fractions. For
example, `"3d6+10"` means "roll 3d6 and add 10" (the modifier does not have to
be at the end; `"10+3d6"` is equally valid).

White space is not significant. More complicated arithmetic, such as anything
that would require parentheses, is not supported. This constructor will throw
`std::invalid_argument` if the expression is not a valid dice specification
according to the above rules, or `std::domain_error` if it requires division
by zero. Behaviour is undefined if any of the numbers in the expression, or
any intermediate value in the arithmetic required to evaluate it, is out of
range for `T`.

* `RpgDice::`**`RpgDice`**`(const RpgDice& d)`
* `RpgDice::`**`RpgDice`**`(RpgDice&& d) noexcept`
* `RpgDice::`**`~RpgDice`**`() noexcept`
* `RpgDice& RpgDice::`**`operator=`**`(const RpgDice& d)`
* `RpgDice& RpgDice::`**`operator=`**`(RpgDice&& d) noexcept`

Other life cycle functions.

* `template <typename RNG> Rational<T> RpgDice::`**`operator()`**`(RNG& rng)`

The main generator function. The `RNG` class can be any standard conforming
random number engine.

* `RpgDice RpgDice::`**`operator+`**`() const`
* `RpgDice RpgDice::`**`operator-`**`() const`
* `RpgDice& RpgDice::`**`operator+=`**`(const RpgDice& rhs)`
* `RpgDice& RpgDice::`**`operator+=`**`(const Rational<T>& rhs)`
* `RpgDice& RpgDice::`**`operator+=`**`(T rhs)`
* `RpgDice& RpgDice::`**`operator-=`**`(const RpgDice& rhs)`
* `RpgDice& RpgDice::`**`operator-=`**`(const Rational<T>& rhs)`
* `RpgDice& RpgDice::`**`operator-=`**`(T rhs)`
* `RpgDice& RpgDice::`**`operator*=`**`(const Rational<T>& rhs)`
* `RpgDice& RpgDice::`**`operator*=`**`(T rhs)`
* `RpgDice& RpgDice::`**`operator/=`**`(const Rational<T>& rhs)`
* `RpgDice& RpgDice::`**`operator/=`**`(T rhs)`
* `RpgDice` **`operator+`**`(const RpgDice& lhs, const RpgDice& rhs)`
* `RpgDice` **`operator+`**`(const RpgDice& lhs, const Rational<T>& rhs)`
* `RpgDice` **`operator+`**`(const RpgDice& lhs, T rhs)`
* `RpgDice` **`operator+`**`(const Rational<T>& lhs, const RpgDice& rhs)`
* `RpgDice` **`operator+`**`(T lhs, const RpgDice& rhs)`
* `RpgDice` **`operator-`**`(const RpgDice& lhs, const RpgDice& rhs)`
* `RpgDice` **`operator-`**`(const RpgDice& lhs, const Rational<T>& rhs)`
* `RpgDice` **`operator-`**`(const RpgDice& lhs, T rhs)`
* `RpgDice` **`operator-`**`(const Rational<T>& lhs, const RpgDice& rhs)`
* `RpgDice` **`operator-`**`(T lhs, const RpgDice& rhs)`
* `RpgDice` **`operator*`**`(const RpgDice& lhs, const Rational<T>& rhs)`
* `RpgDice` **`operator*`**`(const RpgDice& lhs, T rhs)`
* `RpgDice` **`operator*`**`(const Rational<T>& lhs, const RpgDice& rhs)`
* `RpgDice` **`operator*`**`(T lhs, const RpgDice& rhs)`
* `RpgDice` **`operator/`**`(const RpgDice& lhs, const Rational<T>& rhs)`
* `RpgDice` **`operator/`**`(const RpgDice& lhs, T rhs)`

Operations that modify or combine two sets of dice, or a set of dice and a
numeric factor. Addition and subtraction simply combine groups of dice
together, in the same way as the plus and minus operators in the string
format. Multiplication or division by a rational number multiplies or divides
the result of future rolls by that number.

The division operators will throw `std::domain_error` if the RHS is zero.

* `Rational<T> RpgDice::`**`mean`**`() const noexcept`
* `Rational<T> RpgDice::`**`variance`**`() const noexcept`
* `S RpgDice::`**`sd`**`() const noexcept`
* `Rational<T> RpgDice::`**`min`**`() const noexcept`
* `Rational<T> RpgDice::`**`max`**`() const noexcept`

These return statistical properties of the dice roll results.

* `Ustring RpgDice::`**`str`**`() const`
* `Ustring` **`to_str`**`(const RpgDice& d)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const RpgDice& d)`

These format the collection of dice stored in the object, in the same format
as described for the constructor. Because the string is being reconstructed
from the stored properties of the distribution, the result may not exactly
match the original string supplied to the constructor, but will be
functionally equivalent.

* `IntDice` **`operator""_d4`**`(unsigned long long n)`
* `IntDice` **`operator""_d6`**`(unsigned long long n)`
* `IntDice` **`operator""_d8`**`(unsigned long long n)`
* `IntDice` **`operator""_d10`**`(unsigned long long n)`
* `IntDice` **`operator""_d12`**`(unsigned long long n)`
* `IntDice` **`operator""_d20`**`(unsigned long long n)`
* `IntDice` **`operator""_d100`**`(unsigned long long n)`

Literals for some commonly used dice. For example, `3_d6` is equivalent to
`IntDice(3,6)` or `IntDice("3d6")`.

* `IntDice` **`operator""_dice`**`(const char* p, size_t n)`

Literal equivalent of the constructor from a string. For example, `"3d6"_dice`
is equivalent to `IntDice(3,6)` or `IntDice("3d6")`.
