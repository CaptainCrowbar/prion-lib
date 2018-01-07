# Zlib Compression #

By Ross Smith

* `#include "rs-core/zlib.hpp"`

This header provides wrappers around some of the compression functions in
[Zlib](https://www.zlib.net). Including this header will also import the
contents of `<zlib.h>`.

## Contents ##

[TOC]

## Error handling ##

* `class` **`ZlibCategory`**`: public std::error_category`
* `const std::error_category&` **`zlib_category`**`() noexcept`

A system error category for Zlib error codes.

## Compression functions ##

* `struct` **`Zlib`**
    * `static std::string` **`compress`**`(std::string_view src, int level = Z_DEFAULT_COMPRESSION)`
    * `static std::string` **`uncompress`**`(std::string_view src)`

Basic compression and uncompression functions. Either of these may throw
`std::system_error` or `std::bad_alloc` if anything goes wrong. For
compression, `std::bad_alloc` is normally the only possible exception, unless
there is an internal error in Zlib; for uncompression, a corrupt archive will
trigger a `std::system_error` with a `ZlibCategory` error category.

## I/O class ##

* `class` **`Gzio`**`: public IO`
    * `using Gzio::`**`handle_type`** `= gzFile`
    * `Gzio::`**`Gzio`**`()`
    * `explicit Gzio::`**`Gzio`**`(const File& f, IO::mode m = IO::mode::read_only)`
    * `Gzio::`**`Gzio`**`(const File& f, Uview iomode)`
    * `virtual Gzio::`**`~Gzio`**`()`
    * `Gzio::`**`Gzio`**`(Gzio&& io)`
    * `Gzio& Gzio::`**`operator=`**`(Gzio&& io)`
    * `gzFile Gzio::`**`get`**`() const noexcept`
    * `void Gzio::`**`ungetc`**`(char c)`

An I/O class that reads and writes Gzip files. A compressed file can't be
opened for reading and writing at the same time, so the `IO::mode` argument to
the second constructor can only be `read_only`, `write_only`, or `append`;
anything else will fail. The mode argument to the third constructor follows
the same format as the corresponding argument to `gzopen()` in Zlib.
