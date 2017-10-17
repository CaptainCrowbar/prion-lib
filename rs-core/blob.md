# Binary Large Object #

By Ross Smith

* `#include "rs-core/blob.hpp"`

## Contents ##

[TOC]

## Class Blob ##

* `class` **`Blob`**
    * `Blob::`**`Blob`**`()`
    * `explicit Blob::`**`Blob`**`(size_t n)`
    * `Blob::`**`Blob`**`(size_t n, uint8_t x)`
    * `Blob::`**`Blob`**`(void* p, size_t n)`
    * `template <typename F> Blob::`**`Blob`**`(void* p, size_t n, F f)`
    * `Blob::`**`~Blob`**`() noexcept`
    * `Blob::`**`Blob`**`(const Blob& b)`
    * `Blob::`**`Blob`**`(Blob&& b) noexcept: ptr(b.ptr), len(b.len)`
    * `Blob& Blob::`**`operator=`**`(const Blob& b)`
    * `Blob& Blob::`**`operator=`**`(Blob&& b) noexcept`
    * `void* Blob::`**`data`**`() noexcept`
    * `const void* Blob::`**`data`**`() const noexcept`
    * `uint8_t* Blob::`**`b_data`**`() noexcept`
    * `const uint8_t* Blob::`**`b_data`**`() const noexcept`
    * `char* Blob::`**`c_data`**`() noexcept`
    * `const char* Blob::`**`c_data`**`() const noexcept`
    * `Irange<uint8_t*> Blob::`**`bytes`**`() noexcept`
    * `Irange<const uint8_t*> Blob::`**`bytes`**`() const noexcept`
    * `Irange<char*> Blob::`**`chars`**`() noexcept`
    * `Irange<const char*> Blob::`**`chars`**`() const noexcept`
    * `void Blob::`**`clear`**`() noexcept`
    * `void Blob::`**`copy`**`(const void* p, size_t n)`
    * `bool Blob::`**`empty`**`() const noexcept`
    * `void Blob::`**`fill`**`(uint8_t x) noexcept`
    * `size_t Blob::`**`hash`**`() const noexcept`
    * `U8string Blob::`**`hex`**`(size_t block = 0) const`
    * `void Blob::`**`reset`**`(size_t n)`
    * `void Blob::`**`reset`**`(size_t n, uint8_t x)`
    * `void Blob::`**`reset`**`(void* p, size_t n)`
    * `template <typename F> void Blob::`**`reset`**`(void* p, size_t n, F f)`
    * `size_t Blob::`**`size`**`() const noexcept`
    * `string Blob::`**`str`**`() const`
    * `void Blob::`**`swap`**`(Blob& b) noexcept`
* `bool` **`operator==`**`(const Blob& lhs, const Blob& rhs) noexcept`
* `bool` **`operator!=`**`(const Blob& lhs, const Blob& rhs) noexcept`
* `bool` **`operator<`**`(const Blob& lhs, const Blob& rhs) noexcept`
* `bool` **`operator>`**`(const Blob& lhs, const Blob& rhs) noexcept`
* `bool` **`operator<=`**`(const Blob& lhs, const Blob& rhs) noexcept`
* `bool` **`operator>=`**`(const Blob& lhs, const Blob& rhs) noexcept`
* `void` **`swap`**`(Blob& sb1, Blob& sb2) noexcept`
* `class std::`**`hash`**`<Blob>`

A simple wrapper for a block of memory.

The constructor and `reset()` functions that take a length, and optionally a
byte value, allocate memory using `malloc()`. The versions that take only a
length do not initialize the newly allocated memory; the ones that take a
length and value will fill the entire blob with the value. The versions that
take a pointer, size, and optional deallocation function take ownership of the
referenced data, and will deallocate it when it is discarded. If no
deallocation function is supplied, the memory is assumed to have been acquired
with `malloc()`, and will be released with `free()`. When a blob is copied,
the memory for the new copy is always allocated using `malloc()`, regardless
of how the source blob was allocated.

Unlike a `std::vector`, a `Blob` always allocates exactly the required amount
of memory. Any operation that changes the blob's size will reallocate it and
invalidate all pointers into the old blob.

For all functions that take a pointer and length, if a null pointer is passed,
the length is ignored and the effect is the same as passing a valid pointer
and zero length.

The `*data()` functions return pointers to the beginning of the blob. The
`bytes()` and `chars()` functions return a pair of pointers marking the
beginning and end of the blob.

The `copy()` function reallocates the blob to the required size and copies the
referenced data.

The `hex()` function returns the blob's data in hex form; if a nonzero block
size is supplied, a line feed will be inserted after each block.

The `str()` function copies the entire blob into a string.

The comparison operators perform bytewise comparison by calling `memcmp()`.
