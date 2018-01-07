# Multiple Precision Integers #

By Ross Smith

* `#include "rs-core/mp-integer.hpp"`

The `Nat` and `Int` classes are simple arbitrary precision integer
implementations. Multiplication and division are less efficient than they
could be.

## Contents ##

* [TOC]

## Unsigned integers ##

* `class` **`Nat`**
    * `Nat::`**`Nat`**`()`
    * `Nat::`**`Nat`**`(uint64_t x)`
    * `explicit Nat::`**`Nat`**`(Uview s, int base = 0)`
    * `Nat::`**`~Nat`**`() noexcept`
    * `Nat::`**`Nat`**`(const Nat& n)`
    * `Nat::`**`Nat`**`(Nat&& n) noexcept`
    * `Nat& Nat::`**`operator=`**`(const Nat& n)`
    * `Nat& Nat::`**`operator=`**`(Nat&& n) noexcept`
    * `template <typename T> explicit Nat::`**`operator T`**`() const`
    * `explicit Nat::`**`operator bool`**`() const noexcept`
    * `bool Nat::`**`operator!`**`() const noexcept`
    * `Nat Nat::`**`operator+`**`() const`
    * `Nat& Nat::`**`operator++`**`()`
    * `Nat Nat::`**`operator++`**`(int)`
    * `Nat& Nat::`**`operator--`**`()`
    * `Nat Nat::`**`operator--`**`(int)`
    * `Nat& Nat::`**`operator+=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator-=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator*=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator/=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator%=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator&=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator|=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator^=`**`(const Nat& rhs)`
    * `Nat& Nat::`**`operator<<=`**`(ptrdiff_t rhs)`
    * `Nat& Nat::`**`operator>>=`**`(ptrdiff_t rhs)`
    * `size_t Nat::`**`bits`**`() const noexcept`
    * `size_t Nat::`**`bits_set`**`() const noexcept`
    * `size_t Nat::`**`bytes`**`() const noexcept`
    * `int Nat::`**`compare`**`(const Nat& rhs) const noexcept`
    * `bool Nat::`**`get_bit`**`(size_t i) const noexcept`
    * `uint8_t Nat::`**`get_byte`**`(size_t i) const noexcept`
    * `void Nat::`**`set_bit`**`(size_t i, bool b = true)`
    * `void Nat::`**`set_byte`**`(size_t i, uint8_t b)`
    * `void Nat::`**`flip_bit`**`(size_t i)`
    * `bool Nat::`**`is_even`**`() const noexcept`
    * `bool Nat::`**`is_odd`**`() const noexcept`
    * `Nat Nat::`**`pow`**`(const Nat& n) const`
    * `int Nat::`**`sign`**`() const noexcept`
    * `Ustring Nat::`**`str`**`(int base = 10, size_t digits = 1) const`
    * `void Nat::`**`write_be`**`(void* ptr, size_t n) const noexcept`
    * `void Nat::`**`write_le`**`(void* ptr, size_t n) const noexcept`
    * `static Nat Nat::`**`from_double`**`(double x)`
    * `template <typename RNG> static Nat Nat::`**`random`**`(RNG& rng, const Nat& n)`
    * `static Nat Nat::`**`read_be`**`(const void* ptr, size_t n)`
    * `static Nat Nat::`**`read_le`**`(const void* ptr, size_t n)`
* `Nat` **`operator+`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator-`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator*`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator/`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator%`**`(const Nat& lhs, const Nat& rhs)`
* `pair<Nat, Nat>` **`divide`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`quo`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`rem`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator&`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator|`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator^`**`(const Nat& lhs, const Nat& rhs)`
* `Nat` **`operator<<`**`(const Nat& lhs, size_t rhs)`
* `Nat` **`operator>>`**`(const Nat& lhs, size_t rhs)`
* `bool` **`operator==`**`(const Nat& lhs, const Nat& rhs) noexcept`
* `bool` **`operator!=`**`(const Nat& lhs, const Nat& rhs) noexcept`
* `bool` **`operator<`**`(const Nat& lhs, const Nat& rhs) noexcept`
* `bool` **`operator>`**`(const Nat& lhs, const Nat& rhs) noexcept`
* `bool` **`operator<=`**`(const Nat& lhs, const Nat& rhs) noexcept`
* `bool` **`operator>=`**`(const Nat& lhs, const Nat& rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Nat& x)`
* `Nat` **`abs`**`(const Nat& x)`
* `int` **`sign_of`**`(const Nat& x) noexcept`
* `Ustring` **`bin`**`(const Nat& x, size_t digits = 1)`
* `Ustring` **`dec`**`(const Nat& x, size_t digits = 1)`
* `Ustring` **`hex`**`(const Nat& x, size_t digits = 1)`

An arbitrary precision unsigned integer (natural number). Most of its methods
are conventional arithmetic operations with their natural behaviour.

The constructor from a string accepts base 0, 2, 10, or 16; it will throw
`std::invalid_argument` if the base is not one of these. Base 0 will check for
a `"0b"` or `"0x"` prefix, assuming decimal otherwise. Apostrophe delimiters
are allowed.

The explicit cast template returns the value converted to `T`, if possible.
`T` must be a primitive integer or floating point arithmetic type. Results are
unspecified if the value is outside the return type's representable range.

Division operations will throw `std::domain_error` if the divisor is zero.

The `bits()` function returns the number of bits in the number's binary
representation, starting with the highest 1 bit; `bits_set()` returns the
number of 1 bits.

The `bytes()` function returns the number of bytes needed to hold the number.

The `compare()` function returns a three way comparison of `*this` and `rhs`,
returning -1 if `*this<rhs`, 0 if `*this=rhs`, and 1 if `*this>rhs`.

The `get_bit/byte()`, `set_bit/byte()`, and `flip_bit()` functions read or
write a specific bit or byte, selected by zero based index from the least
significant end. If the index is too big for the stored value,
`get_bit/byte()` will return zero or false, while `set_bit/byte()` and
`flip_bit()` will extend the value to include any bits newly set to 1.

The `str()` function will throw `std::invalid_argument` if the base is less
than 2 or greater than 36.

The `read_be/le()` and `write_be/le()` functions read and write an integer
from a block of bytes, in big or little endian order. The write functions will
truncate the integer if it is too big to fit in the specified number of bytes.

The `random()` function returns a random integer from zero to `n-1`; behaviour
is undefined if `n<1`.

## Signed integers ##

* `class` **`Int`**
    * `Int::`**`Int`**`()`
    * `Int::`**`Int`**`(int64_t x)`
    * `Int::`**`Int`**`(const Nat& x)`
    * `explicit Int::`**`Int`**`(Uview s, int base = 0)`
    * `Int::`**`~Int`**`() noexcept`
    * `Int::`**`Int`**`(const Int& n)`
    * `Int::`**`Int`**`(Int&& n) noexcept`
    * `Int& Int::`**`operator=`**`(const Int& n)`
    * `Int& Int::`**`operator=`**`(Int&& n) noexcept`
    * `template <typename T> explicit Int::`**`operator T`**`() const`
    * `explicit Int::`**`operator Nat`**`() const`
    * `explicit Int::`**`operator bool`**`() const noexcept`
    * `bool Int::`**`operator!`**`() const noexcept`
    * `Int Int::`**`operator+`**`() const`
    * `Int Int::`**`operator-`**`() const`
    * `Int& Int::`**`operator++`**`()`
    * `Int Int::`**`operator++`**`(int)`
    * `Int& Int::`**`operator--`**`()`
    * `Int Int::`**`operator--`**`(int)`
    * `Int& Int::`**`operator+=`**`(const Int& rhs)`
    * `Int& Int::`**`operator-=`**`(const Int& rhs)`
    * `Int& Int::`**`operator*=`**`(const Int& rhs)`
    * `Int& Int::`**`operator/=`**`(const Int& rhs)`
    * `Int& Int::`**`operator%=`**`(const Int& rhs)`
    * `Nat Int::`**`abs`**`() const`
    * `int Int::`**`compare`**`(const Int& rhs) const noexcept`
    * `bool Int::`**`is_even`**`() const noexcept`
    * `bool Int::`**`is_odd`**`() const noexcept`
    * `Int Int::`**`pow`**`(const Int& n) const`
    * `int Int::`**`sign`**`() const noexcept`
    * `Ustring Int::`**`str`**`(int base = 10, size_t digits = 1, bool sign = false) const`
    * `static Int Int::`**`from_double`**`(double x)`
    * `template <typename RNG> static Int Int::`**`random`**`(RNG& rng, const Int& n)`
* `Int` **`operator+`**`(const Int& lhs, const Int& rhs)`
* `Int` **`operator-`**`(const Int& lhs, const Int& rhs)`
* `Int` **`operator*`**`(const Int& lhs, const Int& rhs)`
* `Int` **`operator/`**`(const Int& lhs, const Int& rhs)`
* `Int` **`operator%`**`(const Int& lhs, const Int& rhs)`
* `pair<Int, Int>` **`divide`**`(const Int& lhs, const Int& rhs)`
* `Int` **`quo`**`(const Int& lhs, const Int& rhs)`
* `Int` **`rem`**`(const Int& lhs, const Int& rhs)`
* `bool` **`operator==`**`(const Int& lhs, const Int& rhs) noexcept`
* `bool` **`operator!=`**`(const Int& lhs, const Int& rhs) noexcept`
* `bool` **`operator<`**`(const Int& lhs, const Int& rhs) noexcept`
* `bool` **`operator>`**`(const Int& lhs, const Int& rhs) noexcept`
* `bool` **`operator<=`**`(const Int& lhs, const Int& rhs) noexcept`
* `bool` **`operator>=`**`(const Int& lhs, const Int& rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Int& x)`
* `Int` **`abs`**`(const Int& x)`
* `int` **`sign_of`**`(const Int& x) noexcept`
* `Ustring` **`bin`**`(const Int& x, size_t digits = 1)`
* `Ustring` **`dec`**`(const Int& x, size_t digits = 1)`
* `Ustring` **`hex`**`(const Int& x, size_t digits = 1)`

An arbitrary precision signed integer. Most of its operations do the same
thing as the corresponding functions on `Nat` or plain `int`.

The explicit conversion to `Nat` returns the absolute value of the number. The
`pow()` function will throw `std::domain_error` if the exponent is negative.

The signed division operators perform Euclidean division: if the division is
not exact, the remainder is always positive (regardless of the signs of the
arguments), and the quotient is the integer that satisfies `lhs=q*rhs+r`. The
`quo()` and `rem()` functions are redundant with the division operators, but
are included for consistency with the functions defined in the common module
for standard arithmetic types.

## Related types ##

* `using` **`Uratmp`** `= Rational<Nat>`
* `using` **`Ratmp`** `= Rational<Int>`

Multiple precision rational numbers.

## Integer literals ##

* `namespace RS::Literals`
    * `Nat operator""`**`_nat`**`(const char* raw)`
    * `Int operator""`**`_int`**`(const char* raw)`

These allow MP integer literals such as `12345_int` or `0xabcdef_nat`. They
perform the same conversions as the constructors with base 0.

## Random distributions ##

* `class` **`RandomNat`**
    * `using RandomNat::`**`result_type`** `= Nat`
    * `RandomNat::`**`RandomNat`**`()`
    * `RandomNat::`**`RandomNat`**`(Nat a, Nat b)`
    * `RandomNat::`**`~RandomNat`**`() noexcept`
    * `RandomNat::`**`RandomNat`**`(const RandomNat& n)`
    * `RandomNat::`**`RandomNat`**`(RandomNat&& n) noexcept`
    * `RandomNat& RandomNat::`**`operator=`**`(const RandomNat& n)`
    * `RandomNat& RandomNat::`**`operator=`**`(RandomNat&& n) noexcept`
    * `template <typename RNG> Nat RandomNat::`**`operator()`**`(RNG& rng)`
    * `Nat RandomNat::`**`min`**`() const`
    * `Nat RandomNat::`**`max`**`() const`
* `class` **`RandomInt`**
    * `using RandomInt::`**`result_type`** `= Int`
    * `RandomInt::`**`RandomInt`**`()`
    * `RandomInt::`**`RandomInt`**`(Int a, Int b)`
    * `RandomInt::`**`~RandomInt`**`() noexcept`
    * `RandomInt::`**`RandomInt`**`(const RandomInt& n)`
    * `RandomInt::`**`RandomInt`**`(RandomInt&& n) noexcept`
    * `RandomInt& RandomInt::`**`operator=`**`(const RandomInt& n)`
    * `RandomInt& RandomInt::`**`operator=`**`(RandomInt&& n) noexcept`
    * `template <typename RNG> Int RandomInt::`**`operator()`**`(RNG& rng)`
    * `Int RandomInt::`**`min`**`() const`
    * `Int RandomInt::`**`max`**`() const`

These generate random integers within the specified range (`a` to `b`,
inclusive). The bounds can be supplied in either order. The default
constructor creates a generator that always returns zero.
