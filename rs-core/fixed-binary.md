# Fixed Precision Integers #

By Ross Smith

* `#include "rs-core/fixed-binary.hpp"`

## Contents ##

[TOC]

## Binary integer classes ##

* `template <size_t N> class` **`SmallBinary`**
* `template <size_t N> class` **`LargeBinary`**
* `template <size_t N> using` **`Binary`** `= std::conditional_t<N <= 64, SmallBinary<N>, LargeBinary<N>>`

All of these classes represent unsigned integers with `N` bits. `SmallBinary`
stores its value in a single native integer of an appropriate size,
`LargeBinary` in a suitably sized array of 32-bit integers, stored in little
endian order. Normally you should just use the `Binary` alias; all of the
functions described below are common to both templates (except that, where a
function is documented as taking a `Binary` argument by value, usually the
`LargeBinary` version actually takes its argument by reference).

* `static constexpr size_t Binary::`**`bits`**` = N`
* `static constexpr size_t Binary::`**`bytes`**` = [total bytes in representation]`
* `static constexpr size_t Binary::`**`hex_digits`**` = [maximum number of hex digits in value]`

Member constants.

* `constexpr Binary::`**`Binary`**`() noexcept`
* `Binary::`**`Binary`**`(uint64_t x) noexcept`
* `template <size_t M> explicit Binary::`**`Binary`**`(Binary<M> x) noexcept`
* `explicit Binary::`**`Binary`**`(std::initializer_list<uint64_t> init) noexcept`
* `explicit Binary::`**`Binary`**`(const Ustring& str, int base = 10) noexcept`
* `Binary::`**`~Binary`**`() noexcept`
* `Binary::`**`Binary`**`(const Binary& x) noexcept`
* `Binary::`**`Binary`**`(Binary&& x) noexcept`
* `Binary& Binary::`**`operator=`**`(const Binary& x) noexcept`
* `Binary& Binary::`**`operator=`**`(Binary&& x) noexcept`

Life cycle operations. The default constructor sets the value to zero. A
`Binary` can be constructed from a native unsigned integer, another `Binary`
of any size, a list of 64-bit integers (in big endian order), or a string. In
all cases, if the actual value supplied is too large to fit in `N` bits, the
excess bits are discarded. The string constructor calls `parse()` and follows
the same rules.

* `Ustring Binary::`**`as_binary`**`() const`
* `Ustring Binary::`**`as_decimal`**`() const`
* `Ustring Binary::`**`as_hex`**`() const`
* `std::ostream&` **`operator<<`**`(std::ostream& out, Binary x)`
* `Ustring` **`to_str`**`(Binary x)`

String formatting functions. The output operator and `to_str()` function call
`as_decimal()`. The `as_binary()` and `as_hex()` functions always return the
same number of digits (equal to `bits` and `hex_digits` respectively);
`as_decimal()` writes only the number of digits needed, with no leading zeros
(unless the value is zero).

* `double Binary::`**`as_double`**`() const noexcept`
* `static Binary Binary::`**`from_double`**`(double x) noexcept`

Conversions between `Binary` and `double`. Conversions to a `double` may
produce an approximation if `N>53`. Conversion from a `double` yields the
equivalent of converting the significand exactly and then shifting by a number
of bits based on the exponent; behaviour is undefined if the argument is
negative.

* `void Binary::`**`clear`**`() noexcept`

Sets the value to zero.

* `int Binary::`**`compare`**`(Binary y) const noexcept`

Compares the current value (`x`) to `y`, returning `-1` if `x<y`, `0` if
`x==y`, or `1` if `x>y`.

* `constexpr uint8_t* Binary::`**`data`**`() noexcept`
* `constexpr const uint8_t* Binary::`**`data`**`() const noexcept`

Pointers to the internal representation, which will consist of a number of
bytes equal to the `bytes` constant, in little endian order.

* `template <typename T> bool Binary::`**`fits_in`**`() const noexcept`

True if the current value of the `Binary` will fit in a `T` without loss of
information. Behaviour is undefined if `T` is not a primitive arithmetic type.

* `size_t Binary::`**`hash`**`() const noexcept`
* `struct` **`std::hash`**`<Binary>`

Hash function.

* `size_t Binary::`**`parse`**`(const Ustring& str, int base = 10) noexcept`

Attempts to parse a string as an unsigned integer, writing the value into the
current object. This will always update the value, setting it to zero if the
string does not contain a valid number. It will stop parsing when the first
character that cannot be a digit in the given base is encountered, if this
comes before the end of the string, and returns the number of characters
successfully parsed. Behaviour is undefined if the base is less than 2 or
greater than 35.

* `size_t Binary::`**`significant_bits`**`() const noexcept`

Returns the number of significant bits in the value.

* `explicit Binary::`**`operator bool`**`() const noexcept`

Returns true if the value is not zero.

* `template <typename T> explicit Binary::`**`operator T`**`() const noexcept`

Converts a `Binary` into a standard integer type. The usual arithmetic
overflow rules apply if the value is out of range for the result type.
Behaviour is undefined if `T` is not a primitive arithmetic type, or if `T` is
signed and `fits_in<T>()` is false.

* `Binary Binary::`**`operator+`**`() const noexcept`
* `Binary Binary::`**`operator-`**`() const noexcept`
* `Binary& Binary::`**`operator++`**`() noexcept`
* `Binary Binary::`**`operator++`**`(int) noexcept`
* `Binary& Binary::`**`operator--`**`() noexcept`
* `Binary Binary::`**`operator--`**`(int) noexcept`
* `Binary& Binary::`**`operator+=`**`(Binary y) noexcept`
* `Binary& Binary::`**`operator-=`**`(Binary y) noexcept`
* `Binary& Binary::`**`operator*=`**`(Binary y) noexcept`
* `Binary& Binary::`**`operator/=`**`(Binary y) noexcept`
* `Binary& Binary::`**`operator%=`**`(Binary y) noexcept`
* `Binary` **`operator+`**`(Binary x, Binary y) noexcept`
* `Binary` **`operator-`**`(Binary x, Binary y) noexcept`
* `Binary` **`operator*`**`(Binary x, Binary y) noexcept`
* `Binary` **`operator/`**`(Binary x, Binary y) noexcept`
* `Binary` **`operator%`**`(Binary x, Binary y) noexcept`
* `static void Binary::`**`divide`**`(Binary x, Binary y, Binary& q, Binary& r) noexcept`

Arithmetic operations. Most of these just have the obvious semantics, with the
usual wraparound behaviour on overflow or underflow. For all of the division
and remainder operations (including `divide()`), behaviour is undefined if the
divisor is zero.

* `Binary Binary::`**`operator~`**`() const noexcept`
* `Binary& Binary::`**`operator&=`**`(Binary y) noexcept`
* `Binary& Binary::`**`operator|=`**`(Binary y) noexcept`
* `Binary& Binary::`**`operator^=`**`(Binary y) noexcept`
* `Binary& Binary::`**`operator<<=`**`(int y) noexcept`
* `Binary& Binary::`**`operator>>=`**`(int y) noexcept`
* `Binary` **`operator&`**`(Binary x, Binary y) noexcept`
* `Binary` **`operator|`**`(Binary x, Binary y) noexcept`
* `Binary` **`operator^`**`(Binary x, Binary y) noexcept`
* `Binary` **`operator<<`**`(Binary x, int y) noexcept`
* `Binary` **`operator>>`**`(Binary x, int y) noexcept`
* `Binary` **`rotl`**`(Binary x, int y) noexcept`
* `Binary` **`rotr`**`(Binary x, int y) noexcept`

Bitwise operations. These have their usual semantics. The bit count argument
to the shift and rotate operations can be any value, including out of range or
negative values; a negative shift or rotate is interpreted as a positive one
in the opposite direction.

* `bool` **`operator==`**`(Binary x, Binary y) noexcept`
* `bool` **`operator!=`**`(Binary x, Binary y) noexcept`
* `bool` **`operator<`**`(Binary x, Binary y) noexcept`
* `bool` **`operator>`**`(Binary x, Binary y) noexcept`
* `bool` **`operator<=`**`(Binary x, Binary y) noexcept`
* `bool` **`operator>=`**`(Binary x, Binary y) noexcept`

Comparison operators.

* `static Binary Binary::`**`max`**`() noexcept`

Returns the maximum value of the type (the complement of zero).

* `template <typename RNG> static Binary Binary::`**`random`**`(RNG& rng)`
* `template <typename RNG> static Binary Binary::`**`random`**`(RNG& rng, Binary x)`
* `template <typename RNG> static Binary Binary::`**`random`**`(RNG& rng, Binary x, Binary y)`

Generate a random integer. The first version generates a random value from the
whole range of the type; the second generates a value from zero to `x-1` (this
will always return zero if `x<2`); the third returns a value from `x` to `y`
inclusive (the bounds can be supplied in either order).
