# Multiple Precision Integers #

By Ross Smith

* `#include "rs-core/mp-integer.hpp"`

The `Mpint` and `Mpuint` classes are simple arbitrary precision integer
implementations. Multiplication and division are less efficient than they
could be.

## Contents ##

* [TOC]

## Unsigned integers ##

* `class` **`Mpuint`**
    * `Mpuint::`**`Mpuint`**`()`
    * `Mpuint::`**`Mpuint`**`(uint64_t x)`
    * `explicit Mpuint::`**`Mpuint`**`(Uview s, int base = 0)`
    * `Mpuint::`**`~Mpuint`**`() noexcept`
    * `Mpuint::`**`Mpuint`**`(const Mpuint& n)`
    * `Mpuint::`**`Mpuint`**`(Mpuint&& n) noexcept`
    * `Mpuint& Mpuint::`**`operator=`**`(const Mpuint& n)`
    * `Mpuint& Mpuint::`**`operator=`**`(Mpuint&& n) noexcept`
    * `template <typename T> explicit Mpuint::`**`operator T`**`() const`
    * `explicit Mpuint::`**`operator bool`**`() const noexcept`
    * `bool Mpuint::`**`operator!`**`() const noexcept`
    * `Mpuint Mpuint::`**`operator+`**`() const`
    * `Mpuint& Mpuint::`**`operator++`**`()`
    * `Mpuint Mpuint::`**`operator++`**`(int)`
    * `Mpuint& Mpuint::`**`operator--`**`()`
    * `Mpuint Mpuint::`**`operator--`**`(int)`
    * `Mpuint& Mpuint::`**`operator+=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator-=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator*=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator/=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator%=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator&=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator|=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator^=`**`(const Mpuint& rhs)`
    * `Mpuint& Mpuint::`**`operator<<=`**`(ptrdiff_t rhs)`
    * `Mpuint& Mpuint::`**`operator>>=`**`(ptrdiff_t rhs)`
    * `size_t Mpuint::`**`bits`**`() const noexcept`
    * `size_t Mpuint::`**`bits_set`**`() const noexcept`
    * `size_t Mpuint::`**`bytes`**`() const noexcept`
    * `int Mpuint::`**`compare`**`(const Mpuint& rhs) const noexcept`
    * `bool Mpuint::`**`get_bit`**`(size_t i) const noexcept`
    * `uint8_t Mpuint::`**`get_byte`**`(size_t i) const noexcept`
    * `void Mpuint::`**`set_bit`**`(size_t i, bool b = true)`
    * `void Mpuint::`**`set_byte`**`(size_t i, uint8_t b)`
    * `void Mpuint::`**`flip_bit`**`(size_t i)`
    * `size_t Mpuint::`**`hash`**`() const noexcept`
    * `bool Mpuint::`**`is_even`**`() const noexcept`
    * `bool Mpuint::`**`is_odd`**`() const noexcept`
    * `Mpuint Mpuint::`**`pow`**`(const Mpuint& n) const`
    * `int Mpuint::`**`sign`**`() const noexcept`
    * `Ustring Mpuint::`**`str`**`(int base = 10, size_t digits = 1) const`
    * `void Mpuint::`**`write_be`**`(void* ptr, size_t n) const noexcept`
    * `void Mpuint::`**`write_le`**`(void* ptr, size_t n) const noexcept`
    * `static Mpuint Mpuint::`**`from_double`**`(double x)`
    * `template <typename RNG> static Mpuint Mpuint::`**`random`**`(RNG& rng, const Mpuint& n)`
    * `static Mpuint Mpuint::`**`read_be`**`(const void* ptr, size_t n)`
    * `static Mpuint Mpuint::`**`read_le`**`(const void* ptr, size_t n)`
* `Mpuint` **`operator+`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator-`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator*`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator/`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator%`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `pair<Mpuint, Mpuint>` **`divide`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`quo`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`rem`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator&`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator|`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator^`**`(const Mpuint& lhs, const Mpuint& rhs)`
* `Mpuint` **`operator<<`**`(const Mpuint& lhs, size_t rhs)`
* `Mpuint` **`operator>>`**`(const Mpuint& lhs, size_t rhs)`
* `bool` **`operator==`**`(const Mpuint& lhs, const Mpuint& rhs) noexcept`
* `bool` **`operator!=`**`(const Mpuint& lhs, const Mpuint& rhs) noexcept`
* `bool` **`operator<`**`(const Mpuint& lhs, const Mpuint& rhs) noexcept`
* `bool` **`operator>`**`(const Mpuint& lhs, const Mpuint& rhs) noexcept`
* `bool` **`operator<=`**`(const Mpuint& lhs, const Mpuint& rhs) noexcept`
* `bool` **`operator>=`**`(const Mpuint& lhs, const Mpuint& rhs) noexcept`
* `Mpuint` **`abs`**`(const Mpuint& x)`
* `int` **`sign_of`**`(const Mpuint& x) noexcept`
* `Ustring` **`bin`**`(const Mpuint& x, size_t digits = 1)`
* `Ustring` **`dec`**`(const Mpuint& x, size_t digits = 1)`
* `Ustring` **`hex`**`(const Mpuint& x, size_t digits = 1)`
* `Ustring` **`to_str`**`(const Mpuint& x)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Mpuint& x)`
* `struct std::`**`hash`**`<Mpuint>`
* `class std::`**`numeric_limits`**`<Mpuint>`

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
than 2 or greater than 36. The output operator and `to_str()` call `str(10)`.

The `read_be/le()` and `write_be/le()` functions read and write an integer
from a block of bytes, in big or little endian order. The write functions will
truncate the integer if it is too big to fit in the specified number of bytes.

The `random()` function returns a random integer from zero to `n-1`; behaviour
is undefined if `n<1`.

## Signed integers ##

* `class` **`Mpint`**
    * `Mpint::`**`Mpint`**`()`
    * `Mpint::`**`Mpint`**`(int64_t x)`
    * `Mpint::`**`Mpint`**`(const Mpuint& x)`
    * `explicit Mpint::`**`Mpint`**`(Uview s, int base = 0)`
    * `Mpint::`**`~Mpint`**`() noexcept`
    * `Mpint::`**`Mpint`**`(const Mpint& n)`
    * `Mpint::`**`Mpint`**`(Mpint&& n) noexcept`
    * `Mpint& Mpint::`**`operator=`**`(const Mpint& n)`
    * `Mpint& Mpint::`**`operator=`**`(Mpint&& n) noexcept`
    * `template <typename T> explicit Mpint::`**`operator T`**`() const`
    * `explicit Mpint::`**`operator Mpuint`**`() const`
    * `explicit Mpint::`**`operator bool`**`() const noexcept`
    * `bool Mpint::`**`operator!`**`() const noexcept`
    * `Mpint Mpint::`**`operator+`**`() const`
    * `Mpint Mpint::`**`operator-`**`() const`
    * `Mpint& Mpint::`**`operator++`**`()`
    * `Mpint Mpint::`**`operator++`**`(int)`
    * `Mpint& Mpint::`**`operator--`**`()`
    * `Mpint Mpint::`**`operator--`**`(int)`
    * `Mpint& Mpint::`**`operator+=`**`(const Mpint& rhs)`
    * `Mpint& Mpint::`**`operator-=`**`(const Mpint& rhs)`
    * `Mpint& Mpint::`**`operator*=`**`(const Mpint& rhs)`
    * `Mpint& Mpint::`**`operator/=`**`(const Mpint& rhs)`
    * `Mpint& Mpint::`**`operator%=`**`(const Mpint& rhs)`
    * `Mpuint Mpint::`**`abs`**`() const`
    * `int Mpint::`**`compare`**`(const Mpint& rhs) const noexcept`
    * `size_t Mpint::`**`hash`**`() const noexcept`
    * `bool Mpint::`**`is_even`**`() const noexcept`
    * `bool Mpint::`**`is_odd`**`() const noexcept`
    * `Mpint Mpint::`**`pow`**`(const Mpint& n) const`
    * `int Mpint::`**`sign`**`() const noexcept`
    * `Ustring Mpint::`**`str`**`(int base = 10, size_t digits = 1, bool sign = false) const`
    * `static Mpint Mpint::`**`from_double`**`(double x)`
    * `template <typename RNG> static Mpint Mpint::`**`random`**`(RNG& rng, const Mpint& n)`
* `Mpint` **`operator+`**`(const Mpint& lhs, const Mpint& rhs)`
* `Mpint` **`operator-`**`(const Mpint& lhs, const Mpint& rhs)`
* `Mpint` **`operator*`**`(const Mpint& lhs, const Mpint& rhs)`
* `Mpint` **`operator/`**`(const Mpint& lhs, const Mpint& rhs)`
* `Mpint` **`operator%`**`(const Mpint& lhs, const Mpint& rhs)`
* `pair<Mpint, Mpint>` **`divide`**`(const Mpint& lhs, const Mpint& rhs)`
* `Mpint` **`quo`**`(const Mpint& lhs, const Mpint& rhs)`
* `Mpint` **`rem`**`(const Mpint& lhs, const Mpint& rhs)`
* `bool` **`operator==`**`(const Mpint& lhs, const Mpint& rhs) noexcept`
* `bool` **`operator!=`**`(const Mpint& lhs, const Mpint& rhs) noexcept`
* `bool` **`operator<`**`(const Mpint& lhs, const Mpint& rhs) noexcept`
* `bool` **`operator>`**`(const Mpint& lhs, const Mpint& rhs) noexcept`
* `bool` **`operator<=`**`(const Mpint& lhs, const Mpint& rhs) noexcept`
* `bool` **`operator>=`**`(const Mpint& lhs, const Mpint& rhs) noexcept`
* `Mpint` **`abs`**`(const Mpint& x)`
* `int` **`sign_of`**`(const Mpint& x) noexcept`
* `Ustring` **`bin`**`(const Mpint& x, size_t digits = 1)`
* `Ustring` **`dec`**`(const Mpint& x, size_t digits = 1)`
* `Ustring` **`hex`**`(const Mpint& x, size_t digits = 1)`
* `Ustring` **`to_str`**`(const Mpint& x)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Mpint& x)`
* `struct std::`**`hash`**`<Mpint>`
* `class std::`**`numeric_limits`**`<Mpint>`

An arbitrary precision signed integer. Most of its operations do the same
thing as the corresponding functions on `Mpuint` or plain `int`.

The explicit conversion to `Mpuint` returns the absolute value of the number. The
`pow()` function will throw `std::domain_error` if the exponent is negative.

The signed division operators perform Euclidean division: if the division is
not exact, the remainder is always positive (regardless of the signs of the
arguments), and the quotient is the integer that satisfies `lhs=q*rhs+r`. The
`quo()` and `rem()` functions are redundant with the division operators, but
are included for consistency with the functions defined in the common module
for standard arithmetic types.

## Related types ##

* `using` **`Mpratio`** `= Rational<Mpint>`

Multiple precision rational numbers.

## Integer literals ##

* `namespace RS::Literals`
    * `Mpint operator""`**`_mpi`**`(const char* raw)`
    * `Mpuint operator""`**`_mpu`**`(const char* raw)`

These allow MP integer literals such as `12345_mpi` or `0xabcdef_mpu`. They
perform the same conversions as the constructors with base 0.

## Random distributions ##

* `class` **`RandomMpint`**
    * `using RandomMpint::`**`result_type`** `= Mpint`
    * `RandomMpint::`**`RandomMpint`**`()`
    * `RandomMpint::`**`RandomMpint`**`(Mpint a, Mpint b)`
    * `RandomMpint::`**`~RandomMpint`**`() noexcept`
    * `RandomMpint::`**`RandomMpint`**`(const RandomMpint& n)`
    * `RandomMpint::`**`RandomMpint`**`(RandomMpint&& n) noexcept`
    * `RandomMpint& RandomMpint::`**`operator=`**`(const RandomMpint& n)`
    * `RandomMpint& RandomMpint::`**`operator=`**`(RandomMpint&& n) noexcept`
    * `template <typename RNG> Mpint RandomMpint::`**`operator()`**`(RNG& rng)`
    * `Mpint RandomMpint::`**`min`**`() const`
    * `Mpint RandomMpint::`**`max`**`() const`
* `class` **`RandomMpuint`**
    * `using RandomMpuint::`**`result_type`** `= Mpuint`
    * `RandomMpuint::`**`RandomMpuint`**`()`
    * `RandomMpuint::`**`RandomMpuint`**`(Mpuint a, Mpuint b)`
    * `RandomMpuint::`**`~RandomMpuint`**`() noexcept`
    * `RandomMpuint::`**`RandomMpuint`**`(const RandomMpuint& n)`
    * `RandomMpuint::`**`RandomMpuint`**`(RandomMpuint&& n) noexcept`
    * `RandomMpuint& RandomMpuint::`**`operator=`**`(const RandomMpuint& n)`
    * `RandomMpuint& RandomMpuint::`**`operator=`**`(RandomMpuint&& n) noexcept`
    * `template <typename RNG> Mpuint RandomMpuint::`**`operator()`**`(RNG& rng)`
    * `Mpuint RandomMpuint::`**`min`**`() const`
    * `Mpuint RandomMpuint::`**`max`**`() const`

These generate random integers within the specified range (`a` to `b`,
inclusive). The bounds can be supplied in either order. The default
constructor creates a generator that always returns zero.
