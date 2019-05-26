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

### Sip hash ###

* `template <typename T, size_t C = 2, size_t D = 4> class` **`SipHash`**
    * `using SipHash::`**`key_type`** `= [uint32_t if T is 32-bit, otherwise uint64_t]`
    * `using SipHash::`**`result_type`** `= T`
    * `static constexpr size_t SipHash::`**`key_size`** `= 2 * sizeof(key_type)`
    * `static constexpr size_t SipHash::`**`result_size`** `= sizeof(T)`
    * `SipHash::`**`SipHash`**`() noexcept`
    * `SipHash::`**`SipHash`**`(key_type k0, key_type k1) noexcept`
    * `explicit SipHash::`**`SipHash`**`(const uint8_t* keyptr) noexcept`
    * `void SipHash::`**`add`**`(const void* ptr, size_t len) noexcept`
    * `void SipHash::`**`clear`**`() noexcept`
    * `void SipHash::`**`clear`**`(key_type k0, key_type k1) noexcept`
    * `void SipHash::`**`clear`**`(const uint8_t* keyptr) noexcept`
    * `result_type SipHash::`**`get`**`() noexcept`
* `using` **`SipHash32`** `= SipHash<uint32_t>`
* `using` **`SipHash64`** `= SipHash<uint64_t>`
* `using` **`SipHash128`** `= SipHash<Uint128>`

SipHash is widely used as a hash table keying function to avoid hash flooding
attacks. The result type `T` must be a 32, 64, or 128-bit unsigned integer
(the [`Uint128`](int128.html) class can be used). The alternative
"half-sip-hash" algorithm is used for the 32-bit hash.

The key can be supplied either as a pair of integers (defaulting to `{0,0}`),
or as a block of memory containing `key_size` bytes. The first version of
`clear()` clears the hash state but keeps the previous seed; the other
versions re-initialize the state from a new seed.

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
