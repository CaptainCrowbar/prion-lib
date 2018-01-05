# Binary and Text Encodings #

By Ross Smith

* `#include "rs-core/encoding.hpp"`

## Contents ##

[TOC]

## Encoding base class ##

* `class` **`Encoding`**
    * `virtual Encoding::`**`~Encoding`**`() noexcept`
    * `virtual void Encoding::`**`encode_bytes`**`(const void* src, size_t len, Ustring& dst, size_t width = npos) const = 0`
    * `virtual size_t Encoding::`**`decode_bytes`**`(const char* src, size_t len, string& dst) const = 0`
    * `void Encoding::`**`encode`**`(const string& src, Ustring& dst, size_t width = npos) const`
    * `Ustring Encoding::`**`encode`**`(const string& src, size_t width = npos) const`
    * `size_t Encoding::`**`decode`**`(const Ustring& src, string& dst) const`
    * `string Encoding::`**`decode`**`(const Ustring& src) const`

Common abstract base class for binary-to-text encodings. The `encode_bytes()`
and `decode_bytes()` virtual functions contain the actual transformation code;
the `encode()` and `decode()` functions call them, providing a more convenient
interface for some purposes.

The `encode_bytes()` function reads `len` bytes from `src`, appending the
encoded form to the `dst` string (existing content in `dst` will not be
erased). If the `width` argument is set, line feeds will be inserted every
`width` bytes (or as close to it as the output format allows).

The `decode_bytes()` function reads `len` bytes from `src`, appending the
decoded form to the `dst` string (existing content in `dst` will not be
erased). Bytes in the input that cannot be part of an encoded string are
skipped; output is unspecified if the input string is invalid in some other
way. The return value is the number of bytes read from the input string; this
may be less than `len` if the encoded format includes a termination marker.

Behaviour of `encode_bytes()` and `decode_bytes()` is undefined if `src` is
null.

## Character escape encodings ##

* `class` **`EscapeEncoding`**`: public Encoding`
* `class` **`QuoteEncoding`**`: public Encoding`

Bytes outside the printable ASCII range (32-126) are represented by escape
codes; those inside the range are copied unchanged, apart from the backslash.
`QuoteEncoding` gives the same encoding as `EscapeEncoding`, except that the
output is enclosed in quote marks, and embedded quotes are also escaped. The
quote decoder will do nothing if the input does not start with a quote;
otherwise, decoding will stop with the next unescaped quote.

## Hex encoding ##

* `class` **`HexEncoding`**`: public Encoding`

Each byte is encoded as two hexadecimal digits.

## Base 32 encoding ##

* `class` **`Base32Encoding`**`: public Encoding`

Each group of 5 bytes is encoded as 8 alphanumeric characters from `[0-9a-v]`;
encoding always generates lower case, but decoding is case insensitive. No end
marker is necessary because the length of the converted string is always
unambiguous in both directions, but multiple encoded strings cannot be
concatenated unless the length of each string is recorded separately.

## Base 64 encoding ##

* `class` **`Base64Encoding`**`: public Encoding`

[Base 64 encoding](https://en.wikipedia.org/wiki/Base64). Encoding uses the
`'+'` and `'/'` characters for index 62 and 63, and will always append padding
characters. Decoding will accept either `'+'` or `'-'` for 62, `'/'` or `'_'`
for 63, and will work with or without padding characters at the end; embedded
padding characters, resulting from concatenation of multiple encoded strings,
will be handled correctly, and will not be treated as a terminator.

## Base 85 encodings ##

* `class` **`Ascii85Encoding`**`: public Encoding`
* `class` **`Z85Encoding`**`: public Encoding`

The Adobe and ZeroMQ variants of [base 85
encoding](https://en.wikipedia.org/wiki/Ascii85). Leading `"<~"` and trailing
`"~>"` will be generated when encoding, and recognized but not required when
decoding.
