# Message Digest Algorithms #

By Ross Smith

* `#include "rs-core/digest.hpp"`

## Contents ##

[TOC]

## Hash function base class ##

* `template <size_t Bits> class` **`HashFunction`**
    * `static constexpr size_t HashFunction::`**`bits`** `= Bits`
    * `static constexpr size_t HashFunction::`**`bytes`** `= Bits / 8`
    * `using HashFunction::`**`result_type`** `= [see below]`
    * `virtual HashFunction::`**`~HashFunction`**`() noexcept`
    * `virtual result_type HashFunction::`**`hash`**`(const void* ptr, size_t len) const noexcept = 0`
    * `result_type HashFunction::`**`operator()`**`(const void* ptr, size_t len) const noexcept`
    * `result_type HashFunction::`**`operator()`**`(const std::string& str) const noexcept`

Common base class for hash functions of a given output size. A static assert
ensures that the bit count is positive and a multiple of 8. If the bit count
is less than or equal to 64, the result type is the smallest standard fixed
size unsigned integer (`uint8_t` through `uint64_t`) that will hold the
result; otherwise, the result type is `std::array<uint8_t,bytes>`. The
function call operators call the virtual `hash()` function. Behaviour is
undefined if a null pointer is passed to any function.

Members of individual hash function classes are described below only when they
are not present in the base class or have significant semantic differences.

## General purpose hash functions ##

* `class` **`Adler32`**`: public HashFunction<32>`
* `class` **`Crc32`**`: public HashFunction<32>`
* `class` **`Djb2a`**`: public HashFunction<32>`
* `class` **`Fnv1a_32`**`: public HashFunction<32>`
* `class` **`Fnv1a_64`**`: public HashFunction<64>`
* `using` **`Fnv1a_std`** `= Fnv1a_32 or Fnv1a_64 [see below]`
* `class` **`Murmur3_32`**`: public HashFunction<32>`
    * `Murmur3_32::`**`Murmur3_32`**`() noexcept`
    * `explicit Murmur3_32::`**`Murmur3_32`**`(uint32_t seed) noexcept`
* `class` **`SipHash_32`**`: public HashFunction<32>`
    * `static constexpr size_t SipHash_32::`**`key_bits`** `= 128`
    * `static constexpr size_t SipHash_32::`**`key_bytes`** `= 16`
    * `SipHash_32::`**`SipHash_32`**`() noexcept`
    * `SipHash_32::`**`SipHash_32`**`(const void* key, size_t keylen) noexcept`
* `class` **`SipHash_64`**`: public HashFunction<64>`
    * `static constexpr size_t SipHash_64::`**`key_bits`** `= 128`
    * `static constexpr size_t SipHash_64::`**`key_bytes`** `= 16`
    * `SipHash_64::`**`SipHash_64`**`() noexcept`
    * `SipHash_64::`**`SipHash_64`**`(const void* key, size_t keylen) noexcept`
* `using` **`SipHash_std`** `= SipHash_32 or SipHash_64 [see below]`

Commonly used general purpose hash functions. The Murmur3 and SipHash hashes
can optionally take an initialization key; if the length of the key does not
match the standard length, the key will be zero padded or truncated on the
right. `SipHash_64` is the standard 64-bit SipHash; `SipHash_32` is generated
by XORing the two halves of a `SipHash_64`.

The classes whose names end in `_std` are aliases for the corresponding 32-bit
or 64-bit implementations, selected to match the size of a `size_t` (the
result type of `std::hash`).

## Cryptographic hash functions ##

* `class` **`Md5`**`: public HashFunction<128>`
* `class` **`Sha1`**`: public HashFunction<160>`
* `class` **`Sha256`**`: public HashFunction<256>`
* `class` **`Sha512`**`: public HashFunction<512>`

Commonly used cryptographic hash functions.
