# Rational Numbers #

By Ross Smith

* `#include "rs-core/rational.hpp"`

## Contents ##

* [TOC]

## Rational numbers ##

* `template <typename T> class` **`Rational`**

The `Rational` template implements rational numbers over an integer type.

Constructors and arithmetic operators will always reduce the result to its
lowest terms; the denominator will always be positive. If the underlying type
is a fixed precision integer, results are unspecified if a calculation would
overflow the integer type's range.

* `using` **`Rat`** `= Rational<int>`
* `using` **`Urat`** `= Rational<unsigned>`
* `using` **`Rat16`** `= Rational<int16_t>`
* `using` **`Rat32`** `= Rational<int32_t>`
* `using` **`Rat64`** `= Rational<int64_t>`
* `using` **`Urat16`** `= Rational<uint16_t>`
* `using` **`Urat32`** `= Rational<uint32_t>`
* `using` **`Urat64`** `= Rational<uint64_t>`

Named instantiations.

* `using Rational::`**`integer_type`** `= T`

Member types.

* `Rational::`**`Rational`**`()`
* `template <typename T2> Rational::`**`Rational`**`(T2 t)`
* `template <typename T2, typename T3> Rational::`**`Rational`**`(T2 n, T3 d)`
* `extional::`**`~Rational`**`() noexcept`
* `Rational::`**`Rational`**`(const Rational& r)`
* `Rational::`**`Rational`**`(Rational&& r) noexcept`
* `Rational& Rational::`**`operator=`**`(const Rational& r)`
* `Rational& Rational::`**`operator=`**`(Rational&& r) noexcept`
* `template <typename T2> Rational& Rational::`**`operator=`**`(T2 t)`

Life cycle functions. The first constructor sets the number to zero; the
second sets it to an integer; the third to the ratio of two integers. `T2` and
`T3` must be implicitly convertible to `T`. The constructor that takes a
numerator and denominator will throw `std::domain_error` if the denominator is
zero.

* `explicit Rational::`**`operator bool`**`() const noexcept`
* `bool Rational::`**`operator!`**`() const noexcept`
* `template <typename T2> explicit Rational::`**`operator T2`**`() const`

Conversion operators. `T2` may be any type to which `T` can be explicitly
converted and that has a division operator.

* `Rational Rational::`**`operator+`**`() const`
* `Rational Rational::`**`operator-`**`() const`
* `Rational& Rational::`**`operator+=`**`(const Rational& rhs)`
* `Rational& Rational::`**`operator-=`**`(const Rational& rhs)`
* `Rational& Rational::`**`operator*=`**`(const Rational& rhs)`
* `Rational& Rational::`**`operator/=`**`(const Rational& rhs)`
* `Rational` **`operator+`**`(const Rational& lhs, const Rational& rhs)`
* `Rational` **`operator-`**`(const Rational& lhs, const Rational& rhs)`
* `Rational` **`operator*`**`(const Rational& lhs, const Rational& rhs)`
* `Rational` **`operator/`**`(const Rational& lhs, const Rational& rhs)`

Arithmetic operators. Division by zero will throw `std::domain_error`.

* `bool` **`operator==`**`(const Rational& lhs, const Rational& rhs) noexcept`
* `bool` **`operator!=`**`(const Rational& lhs, const Rational& rhs) noexcept`
* `bool` **`operator<`**`(const Rational& lhs, const Rational& rhs) noexcept`
* `bool` **`operator>`**`(const Rational& lhs, const Rational& rhs) noexcept`
* `bool` **`operator<=`**`(const Rational& lhs, const Rational& rhs) noexcept`
* `bool` **`operator>=`**`(const Rational& lhs, const Rational& rhs) noexcept`

Comparison operators. Mixed mode comparisons between rationals and integers
are also defined.

* `T Rational::`**`num`**`() const`
* `T Rational::`**`den`**`() const`

Query the numerator and denominator.

* `Rational Rational::`**`abs`**`() const`
* `Rational` **`abs`**`(const Rational& r)`

Return the absolute value of a rational number.

* `T Rational::`**`floor`**`() const`
* `T Rational::`**`ceil`**`() const`
* `T Rational::`**`round`**`() const`

Convert a rational to an integer. These follow the rules implied by the names
of the functions; `round()` rounds halves toward positive infinity.

* `bool Rational::`**`is_integer`**`() const noexcept`

True if the rational is an exact integer (equivalent to `den()==1`).

* `Rational Rational::`**`reciprocal`**`() const`

Returns the reciprocal of the rational number. This will throw
`std::domain_error` if the original value is zero.

* `int Rational::`**`sign`**`() const noexcept`
* `int` **`sign_of`**`(const Rational& r) noexcept`

Return the sign of a rational number (-1 if negative, 0 if zero, 1 if
positive).

* `bool Rational::`**`try_parse`**`(Uview s)`
* `static Rational Rational::`**`parse`**`(Uview s)`

Parse a string into a rational number. The string can be in integer, simple,
or mixed form (`"123"`, `"123/456"`, or `"123 456/789"`), with an optional
leading sign; non-significant whitespace is ignored. The `try_parse()`
function modifies the current `Rational` object in place, while `parse()`
returns a new object. If the argument is not a valid rational, `try_parse()`
will return false and leave the current object unchanged, while `parse()` will
throw `std::invalid_argument`.

* `T Rational::`**`whole`**`() const`
* `Rational Rational::`**`frac`**`() const`

Return the whole and fractional parts of a rational number. The whole part is
truncated toward zero; the fractional part will have the same sign as the
original number if it is not an exact integer.

* `Ustring Rational::`**`str`**`() const`
* `Ustring Rational::`**`mixed`**`() const`
* `Ustring Rational::`**`simple`**`() const`
* `Ustring` **`to_str`**`(const Rational& r)`
* `std::ostream&` **`operator<<`**`(std::ostream& o, const Rational& r)`

Convert a rational number to a string. The `mixed()` format expresses the
rational as an integer and a fraction (e.g. `"2 3/5"`); the `simple()` format
expresses it as a pure ratio (e.g. `"13/5"`). The `str()` format and the
output operator will write the rational as a simple integer if it has no
fractional part, otherwise they use the `simple()` format. All of these call
`to_str(T)`.
