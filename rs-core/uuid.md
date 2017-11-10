# UUID #

By Ross Smith

* `#include "rs-core/uuid.hpp"`

## Contents ##

[TOC]

## UUID ##

* `class` **`Uuid`**
    * `Uuid::`**`Uuid`**`() noexcept`
    * `Uuid::`**`Uuid`**`(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h, uint8_t i, uint8_t j, uint8_t k, uint8_t l, uint8_t m, uint8_t n, uint8_t o, uint8_t p) noexcept`
    * `Uuid::`**`Uuid`**`(const void* ptr, size_t n) noexcept`
    * `explicit Uuid::`**`Uuid`**`(const U8string& s)`
    * `Uuid::`**`Uuid`**`(const Uuid& u) noexcept`
    * `Uuid::`**`Uuid`**`(Uuid&& u) noexcept`
    * `Uuid::`**`~Uuid`**`() noexcept`
    * `Uuid& Uuid::`**`operator=`**`(const Uuid& u) noexcept`
    * `Uuid& Uuid::`**`operator=`**`(Uuid&& u) noexcept`
    * `uint8_t& Uuid::`**`operator[](`**`size_t i) noexcept`
    * `const uint8_t& Uuid::`**`operator[]`**`(size_t i) const noexcept`
    * `uint8_t* Uuid::`**`begin`**`() noexcept`
    * `const uint8_t* Uuid::`**`begin`**`() const noexcept`
    * `uint8_t* Uuid::`**`end`**`() noexcept`
    * `const uint8_t* Uuid::`**`end`**`() const noexcept`
    * `size_t Uuid::`**`hash`**`() const noexcept`
    * `size_t Uuid::`**`size`**`() const noexcept`
    * `U8string Uuid::`**`str`**`() const`
* `bool` **`operator==`**`(const Uuid& lhs, const Uuid& rhs) noexcept`
* `bool` **`operator!=`**`(const Uuid& lhs, const Uuid& rhs) noexcept`
* `bool` **`operator<`**`(const Uuid& lhs, const Uuid& rhs) noexcept`
* `bool` **`operator>`**`(const Uuid& lhs, const Uuid& rhs) noexcept`
* `bool` **`operator<=`**`(const Uuid& lhs, const Uuid& rhs) noexcept`
* `bool` **`operator>=`**`(const Uuid& lhs, const Uuid& rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& o, const Uuid& u)`
* `class std::`**`hash`**`<Uuid>`

This class holds a standard 16 byte universally unique identifier (UUID).

The default constructor sets all bytes to zero. The second constructor accepts
an explicit list of 16 bytes. The third constructor copies bytes from the
given location (if `n<16`, the extra space is filled with zeros; if `n>16`,
the extra data is ignored; a null pointer will set all bytes to zero).

The fourth constructor parses the string representation of a UUID. It expects
the string to hold exactly 32 hex digits, in groups each containing an even
number of digits (optionally prefixed with `"0x"`), and will treat any
characters that are not ASCII alphanumerics as delimiters. It will throw
`std::invalid_argument` if the string is not a valid UUID.

The `begin()`, `end()`, and `operator[]` functions grant access to the byte
representation. Behaviour is undefined if the index to `operator[]` is greater
than 15. The `size()` function always returns 16.

A specialization of `std::hash` is provided to allow `Uuid` to be used as the
key in an unordered container.

The `str()` function, and the output operator, format the UUID in the standard
broken down hex representation, e.g. `"01234567-89ab-cdef-0123-456789abcdef"`.

The comparison operators perform the natural bytewise lexicographical
comparisons.

* `struct` **`RandomUuid`**
    * `using RandomUuid::`**`result_type`** `= RandomUuid::Uuid`
    * `template <typename RNG> Uuid RandomUuid::`**`operator()`**`(RNG& rng) const`

Generates a random version 4 UUID.
