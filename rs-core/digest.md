# Message Digest Algorithms #

By Ross Smith

* `#include "rs-core/digest.hpp"`

## Contents ##

[TOC]

## Hash class concept ##

* **`Hash`** `concept`
    * `typename Hash::`**`result_type`**
    * `void Hash::`**`add`**`(const void* ptr, size_t len)`
    * `void Hash::`**`clear`**`()`
    * `result_type Hash::`**`get`**`()`

The result type must be either an unsigned integer type or a `std::array` of
`unsigned char`. Blocks of data are added to the hash using `add()`; the hash
must give the same result regardless of how the data is broken up into blocks.
Calling `clear()` resets the hash to its initial state. The hash value is
returned from `get()`; repeated calls to `get()`, with no intervening calls to
`add()` or `clear()`, must yield the same value.

## Helper functions ##

* `template <typename Hash> [result type]` **`digest`**`(const void* ptr, size_t len)`
* `template <typename Hash> [result type]` **`digest`**`(std::string_view view)`

Wrapper functions that apply the hash function to a single block of data and
return the resulting hash value.

## Hash classes ##

### Multiplicative hash classes ###

* `template <uint32_t Seed, uint32_t Mod> class` **`Multiplicative`**
    * `using Multiplicative::`**`result_type`** `= uint32_t`
    * `void Multiplicative::`**`add`**`(const void* ptr, size_t len) noexcept`
    * `void Multiplicative::`**`clear`**`() noexcept`
    * `uint32_t Multiplicative::`**`get`**`() noexcept`
* `using` **`Bernstein`** `= Multiplicative<5381, 33>`
* `using` **`Kernighan`** `= Multiplicative<0, 31>`

Simple multiplicative hash functions.

### Murmur hash ###

* `class` **`Murmur3_32`**
    * `using Murmur3_32::`**`result_type`** `= uint32_t`
    * `Murmur3_32::`**`Murmur3_32`**`() noexcept`
    * `explicit Murmur3_32::`**`Murmur3_32`**`(uint32_t seed) noexcept`
    * `void Murmur3_32::`**`add`**`(const void* ptr, size_t len) noexcept`
    * `void Murmur3_32::`**`clear`**`() noexcept`
    * `void Murmur3_32::`**`clear`**`(uint32_t seed) noexcept`
    * `uint32_t Murmur3_32::`**`get`**`() noexcept`

This is a seedable hash, giving a different result depending on the seed. If
no seed is supplied, the seed defaults to zero and the behaviour is the same
as the other hash classes. The first version of `clear()` clears the hash
state but keeps the previous seed; the second re-initializes the state from a
new seed.

### Other general purpose hash functions ###

<!-- DEFN -->

Class            | Bits      | Result type
-----            | ----      | -----------
**`Crc32`**      | 32        | `uint32_t`
**`Djb2a`**      | 32        | `uint32_t`
**`Fnv1a_32`**   | 32        | `uint32_t`
**`Fnv1a_64`**   | 64        | `uint64_t`
**`Fnv1a_std`**  | 32 or 64  | `uint32_t` or `uint64_t`

The `Fnv1a_std` class is an alias for either `Fnv1a_32` or `Fnv1a_64`,
whichever matches the size of a `size_t` (the result type of `std::hash`).

### Cryptographic hash functions ###

<!-- DEFN -->

Class         | Bits  | Result type
-----         | ----  | -----------
**`Md5`**     | 128   | `std::array<uint8_t, 16>`
**`Sha1`**    | 160   | `std::array<uint8_t, 20>`
**`Sha256`**  | 256   | `std::array<uint8_t, 32>`
**`Sha512`**  | 512   | `std::array<uint8_t, 64>`

Common cryptographic hash functions.
