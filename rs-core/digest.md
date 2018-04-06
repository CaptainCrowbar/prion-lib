# Message Digest Algorithms #

By Ross Smith

* `#include "rs-core/digest.hpp"`

## Contents ##

[TOC]

## Message digest concepts ##

* `class` **`MessageDigest`**
    * `using MessageDigest::`**`result_type`** `= [result type]`
    * `static constexpr size_t MessageDigest::`**`result_size`** `= [size of result in bytes]`
    * `MessageDigest::`**`MessageDigest`**`()`
    * `MessageDigest::`**`~MessageDigest`**`()`
    * `MessageDigest& MessageDigest::`**`operator()`**`(const void* ptr, size_t n)`
    * `MessageDigest::`**`operator result_type`**`()`

This is the pattern expected to be followed by a message digest class. The
result type must be either an unsigned integer type or
`std::array<uint8_t,result_size>`. A message digest class is not required to
be copyable or movable.

A message digest class may or may not be default constructible, depending on
whether or not the algorithm requires any initial parameters. The internal
state is initialized by the constructor, updated by any number of calls to
`operator()`, and finalized to generate the actual hash value by calling
`operator result_type`. Behaviour is undefined if `operator()` is passed a
null pointer, or if any function other than the destructor is called after
`operator result_type`.

Documentation for individual classes below does not bother to list the
standard members where they simply match the concept's standard signature and
behaviour.

## Common hash functions ##

### Adler32 ###

* `class` **`Adler32`**
    * `using Adler32::`**`result_type`** `= uint32_t`
    * `static constexpr size_t Adler32::`**`result_size`** `= 4`

### CRC32 ###

* `class` **`Crc32`**
    * `using Crc32::`**`result_type`** `= uint32_t`
    * `static constexpr size_t Crc32::`**`result_size`** `= 4`

### SipHash ###

* `class` **`SipHash24`**
    * `using SipHash::`**`result_type`** `= uint64_t`
    * `static constexpr size_t SipHash::`**`key_size`**`= 16`
    * `static constexpr size_t SipHash::`**`result_size`**`= 8`
    * `explicit SipHash::`**`SipHash`**`(const uint8_t* key) noexcept`

Behaviour is undefined if a null pointer is passed to any function.

TODO - Not fully implemented yet, multiple calls to operator() will not work

## Cryptographic message digests ##

### MD5 hash ###

* `class` **`Md5`**
    * `using Md5::`**`result_type`** `= std::array<uint8_t, 16>`
    * `static constexpr size_t Md5::`**`result_size`** `= 16`

### SHA1 hash ###

* `class` **`Sha1`**
    * `using Sha1::`**`result_type`** `= std::array<uint8_t, 20>`
    * `static constexpr size_t Sha1::`**`result_size`** `= 20`

### SHA256 hash ###

* `class` **`Sha256`**
    * `using Sha256::`**`result_type`** `= std::array<uint8_t, 32>`
    * `static constexpr size_t Sha256::`**`result_size`** `= 32`

### SHA512 hash ###

* `class` **`Sha512`**
    * `using Sha512::`**`result_type`** `= std::array<uint8_t, 64>`
    * `static constexpr size_t Sha512::`**`result_size`** `= 64`

## Utility functions ##

* `template <typename MD> MD::result_type` **`digest`**`(std::string_view s)`
* `template <typename MD> MD::result_type` **`digest`**`(const void* ptr, size_t n)`

Convenience wrappers for message digest classes.
