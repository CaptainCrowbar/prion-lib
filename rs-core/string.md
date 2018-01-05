# Core String Algorithms #

By Ross Smith

* `#include "rs-core/string.hpp"`

## Contents ##

[TOC]

## String literals ##

All of these are in `namespace RS::Literals`.

* `Strings` **`operator""_csv`**`(const char* p, size_t n)`
* `Strings` **`operator""_qw`**`(const char* p, size_t n)`

These split a string into parts. The `""_csv` literal splits the string at
each comma, trimming whitespace from each element; the `""_qw` literal splits
the string into words delimited by whitespace.

Examples:

    "abc, def, ghi"_csv => {"abc", "def", "ghi"}
    " jkl mno pqr "_qw => {"jkl", "mno", "pqr"}

* `Ustring` **`operator""_doc`**`(const char* p, size_t n)`

The `""_doc` literal removes a leading line feed if present, adds a trailing
one if not present, and removes any common leading indentation from all non-
empty lines. Tab indentation is converted to 4 spaces.

Example:

    R"(
        Hello world.
            Hello again.
        Goodbye.
    )"_doc

Result:

    "Hello world.\n"
    "    Hello again.\n"
    "Goodbye.\n"

## Character functions ##

* `constexpr bool` **`ascii_isalnum`**`(char c) noexcept`
* `constexpr bool` **`ascii_isalpha`**`(char c) noexcept`
* `constexpr bool` **`ascii_iscntrl`**`(char c) noexcept`
* `constexpr bool` **`ascii_isdigit`**`(char c) noexcept`
* `constexpr bool` **`ascii_isgraph`**`(char c) noexcept`
* `constexpr bool` **`ascii_islower`**`(char c) noexcept`
* `constexpr bool` **`ascii_isprint`**`(char c) noexcept`
* `constexpr bool` **`ascii_ispunct`**`(char c) noexcept`
* `constexpr bool` **`ascii_isspace`**`(char c) noexcept`
* `constexpr bool` **`ascii_isupper`**`(char c) noexcept`
* `constexpr bool` **`ascii_isxdigit`**`(char c) noexcept`

These are simple ASCII-only versions of the standard character type functions.
All of them will always return false for bytes outside the ASCII range
(0-127).

* `constexpr bool` **`ascii_isalnum_w`**`(char c) noexcept`
* `constexpr bool` **`ascii_isalpha_w`**`(char c) noexcept`
* `constexpr bool` **`ascii_ispunct_w`**`(char c) noexcept`

These behave the same as the corresponding functions without the `"_w"`
suffix, except that the underscore character is counted as a letter instead of
a punctuation mark. (The suffix is intended to suggest the `"\w"` regex
element, which does much the same thing.)

* `constexpr char` **`ascii_tolower`**`(char c) noexcept`
* `constexpr char` **`ascii_toupper`**`(char c) noexcept`

Simple ASCII-only case conversion functions. All non-ASCII characters are left
unchanged.

* `template <typename T> constexpr T` **`char_to`**`(char c) noexcept`

A simple conversion function that casts a `char` to a larger integer type by
first passing it through `unsigned char`, to ensure that characters with the
high bit set end up as integers in the 128-255 range, and not as negative or
extremely high values.

## String property functions ##

* `bool` **`ascii_icase_equal`**`(const std::string& lhs, const std::string& rhs) noexcept`
* `bool` **`ascii_icase_less`**`(const std::string& lhs, const std::string& rhs) noexcept`

Comparison functions that treat upper and lower case ASCII letters as
equivalent (comparison between letters and non-letters uses the upper case
code points).

* `bool` **`starts_with`**`(const std::string& str, const std::string& prefix) noexcept`
* `bool` **`ends_with`**`(const std::string& str, const std::string& suffix) noexcept`

True if the string starts or ends with the specified substring.

* `bool` **`string_is_ascii`**`(const std::string& str) noexcept`

True if the string contains no 8-bit bytes.

## String manipulation functions ##

* `std::string` **`add_prefix`**`(const std::string& s, const std::string& prefix)`
* `std::string` **`add_suffix`**`(const std::string& s, const std::string& suffix)`
* `std::string` **`drop_prefix`**`(const std::string& s, const std::string& prefix)`
* `std::string` **`drop_suffix`**`(const std::string& s, const std::string& suffix)`

The `add_prefix/suffix()` functions add a prefix or suffix to the string if it
was not already there, or return the string unchanged if it was. The
`drop_prefix/suffix()` functions remove a prefix or suffix if it was present,
otherwise return the string unchanged.

* `std::string` **`ascii_lowercase`**`(const std::string& s)`
* `std::string` **`ascii_uppercase`**`(const std::string& s)`
* `std::string` **`ascii_titlecase`**`(const std::string& s)`
* `std::string` **`ascii_sentencecase`**`(const std::string& s)`

Simple ASCII-only case conversion functions. All non-ASCII characters are left
unchanged. The sentence case function capitalizes the first letter of every
sentence (delimited by a full stop or two consecutive line breaks), leaving
everything else alone.

* `Ustring` **`dent`**`(size_t depth)`

Returns a string containing `4*depth` spaces, for indentation.

* `Ustring` **`indent`**`(const Ustring& str, size_t depth)`

Inserts `4*depth` spaces of indentation on every non-empty line.

* `template <typename InputRange> std::string` **`join`**`(const InputRange& range, const std::string& delim = "", bool term = false)`

Join strings into a single string, using the given delimiter. The value type
of the input range must be assignment compatible with `Ustring`. If the
`term` argument is set, an extra delimiter will be added after the last
element (useful when joining lines to form a text that would be expected to
end with a line break).

* `std::string` **`linearize`**`(const std::string& str)`

Replaces all whitespace in a string with a single space, and trims leading and
trailing whitespace. All non-ASCII bytes are treated as non-whitespace
characters.

* `template <typename C> void` **`null_term`**`(std::basic_string<C>& str) noexcept`
* `template <typename C> std::basic_string<C>` **`null_term_str`**`(const std::basic_string<C>& str)`

Cut off a string at the first null character (useful after the string has been
used as an output buffer by some C APIs).

* `std::string` **`pad_left`**`(const std::string& str, size_t len, char pad = ' ')`
* `std::string` **`pad_right`**`(const std::string& str, size_t len, char pad = ' ')`

Pad a string on the right or left, using the supplied character, to the given
length (in bytes). The string will be returned unchanged if it is already at
least `len` bytes long.

* `std::pair<std::string, std::string>` **`partition_at`**`(const std::string& str, const std::string& delim)`
* `std::pair<std::string, std::string>` **`partition_by`**`(const std::string& str, const std::string& delims = ascii_whitespace)`

These break a string into two parts at the first delimiter, returning the
parts before and after the delimiter. If the delimiter is not found, or is
empty, the returned pair contains the complete original string and an empty
string. The `partition_at()` function breaks the string at the first
occurrence of the delimiter substring; `partition_by()` breaks it at the first
contiguous group of one or more bytes from the delimiter list.

* `Ustring` **`quote`**`(const std::string& str)`
* `Ustring` **`bquote`**`(const std::string& str)`

Return a quoted string; internal quotes, backslashes, and control characters
are escaped. The `bquote()` function always escapes all non-ASCII bytes;
`quote()` passes valid UTF-8 unchanged, but will switch to `bquote()` mode if
the string is not valid UTF-8.

* `std::string` **`repeat`**`(const std::string& s, size_t n, const std::string& delim = "")`

Returns a string containing `n` copies of `s`. If a delimiter is given, it
will be placed between each pair of elements.

* `std::string` **`replace`**`(const std::string& s, const std::string& target, const std::string& subst, size_t n = npos)`

Replaces the first `n` occurrences (all of them by default) of `target` in `s`
with `subst`. This will return the string unchanged if `target` is empty or
`n=0`.

* `template <typename OutputIterator> void` **`split`**`(const std::string& src, OutputIterator dst, const std::string& delim = ascii_whitespace)`
* `Strings` **`splitv`**`(const std::string& src, const std::string& delim = ascii_whitespace)`

Split a string into substrings, discarding any delimiter characters. Any
contiguous subsequence of delimiter characters will be treated as a break
point. If the input string is empty, the output list will always be empty;
otherwise, if the delimiter list is empty, the output will consist of a single
string matching the input. The value type of the output iterator in `split()`
must be assignment compatible with `Ustring`; the `splitv()` version returns
a vector of strings instead of writing to an existing receiver.

* `template <typename OutputIterator> void` **`split_lines`**`(const std::string& src, OutputIterator dst)`
* `Strings` **`splitv_lines`**`(const std::string& src)`

Split a string into lines. A line ends with any LF or CRLF; these are not
copied into the output.

* `std::string` **`trim`**`(const std::string& str, const std::string& chars = ascii_whitespace)`
* `std::string` **`trim_left`**`(const std::string& str, const std::string& chars = ascii_whitespace)`
* `std::string` **`trim_right`**`(const std::string& str, const std::string& chars = ascii_whitespace)`

Trim unwanted bytes from the ends of a string.

* `std::string` **`unqualify`**`(const std::string& str, const std::string& delims = ".:")`

Strips off any prefix ending in one of the delimiter characters (e.g.
`unqualify("RS::unqualify()")` returns `"unqualify()"`). This will return the
original string unchanged if the delimiter string is empty or none of its
characters are found.

## String formatting functions ##

* `template <typename T> Ustring` **`bin`**`(T x, size_t digits = 8 * sizeof(T))`
* `template <typename T> Ustring` **`dec`**`(T x, size_t digits = 1)`
* `template <typename T> Ustring` **`hex`**`(T x, size_t digits = 2 * sizeof(T))`

Simple number formatting functions. These convert an integer to a binary,
decimal, or hexadecimal string, generating at least the specified number of
digits.

* `template <typename... Args> Ustring` **`fmt`**`(const Ustring& pattern, const Args&... args)`

This performs string interpolation, inserting the variadic arguments
(formatted with `to_str()`, below) in place of each occurrence of `"$n"` or
`"${n}"` in the pattern string, where `n` is a decimal integer interpreted as
a 1-based index into the variadic argument list. An index out of bounds will
be replaced with an empty string. If a dollar sign is not followed by a bare
or braced number, the dollar sign is discarded and the next character is
copied unchanged (so `"$$"` will produce a literal dollar sign).

* `template <typename T> Ustring` **`fp_format`**`(T t, char mode = 'g', int prec = 6)`

Simple floating point formatting, by calling `snprintf()`. `T` must be an
arithmetic type; it will be converted to `double` internally. The additional
format `'Z/z'` is the same as `'G/g'` except that trailing zeros are not
stripped. This will throw `std::invalid_argument` if the mode is not one of
`[EFGZefgz]`; it may throw `std::system_error` under implementation defined
circumstances.

* `Ustring` **`roman`**`(int n)`

Formats a number as a Roman numeral. Numbers greater than 1000 will be written
with an arbitrarily long sequence of `"M"`. This will return an empty string
if the argument is less than 1.

* `Ustring` **`hexdump`**`(const void* ptr, size_t n, size_t block = 0)`
* `Ustring` **`hexdump`**`(const std::string& str, size_t block = 0)`

Converts a block of raw data into hexadecimal bytes. If `block` is not zero, a
line feed is inserted after each block.

* `Ustring` **`tf`**`(bool b)`
* `Ustring` **`yn`**`(bool b)`

Convert a boolean to `"true/false"` or `"yes/no"`.

* `template <typename T> Ustring` **`to_str`**`(const T& t)`

Formats an object as a string. This uses the following rules for formatting
various types:

* `std::string`, `char`, character pointers, and anything with an implicit
conversion to `std::string` - The string content is simply copied directly
without using an output stream; a null character pointer is treated as an
empty string.
* Unicode string and character types - Converted to UTF-8 using `uconv()`.
* Integer types - Formatted using `dec()`.
* Floating point types - Formatted using `fp_format()`.
* `bool` - Written as `"true"` or `"false"`.
* Ranges (other than strings) - Serialized in a format similar to a JSON array
(e.g. `"[1,2,3]"`), or an object (e.g. `"{1:a,2:b,3:c}"`) if the range's value
type is a pair; `to_str()` is called recursively on each range element.
* Otherwise, the type's output operator will be called.

## String parsing functions ##

* `unsigned long long` **`binnum`**`(const std::string& str) noexcept`
* `long long` **`decnum`**`(const std::string& str) noexcept`
* `unsigned long long` **`hexnum`**`(const std::string& str) noexcept`
* `double` **`fpnum`**`(const std::string& str) noexcept`

The `binnum()`, `decnum()`, and `hexnum()` functions convert a binary,
decimal, or hexadecimal string to a number; `fpnum()` converts a string to a
floating point number. These will ignore any trailing characters that are not
part of a number, and will return zero if the string is empty or does not
contain a valid number. Results that are out of range will be clamped to the
nearest end of the return type's range (for `fpnum()` this will normally be
positive or negative infinity).

* `int64_t` **`si_to_int`**`(const Ustring& s)`
* `double` **`si_to_float`**`(const Ustring& s)`

These parse a number from a string representation tagged with an SI multiplier
abbreviation (e.g. `"123k"`). For the integer version, only tags representing
positive powers of 1000 (starting with`"k"`) are recognised, and are case
insensitive. For the floating point version, all tags representing powers of
100 are recognised (`"u"` is used for "micro"), and are case sensitive, except
that `"K"` is equivalent to `"k"`. For both versions, a space is allowed
between the number and the tag, and any additional text after the number or
tag is ignored.

These will throw `std::invalid_argument` if the string does not start with a
valid number, or `std::range_error` if the result is too big for the return
type.

## HTML/XML tags ##

* `class` **`Tag`**
    * `Tag::`**`Tag`**`()`
    * `Tag::`**`Tag`**`(std::ostream& out, const std::string& element)`
    * `Tag::`**`~Tag`**`() noexcept`
    * `Tag::`**`Tag`**`(Tag&& t) noexcept`
    * `Tag& Tag::`**`operator=`**`(Tag&& t) noexcept`

This class writes an HTML/XML tag in its constructor, then writes the
corresponding closing tag in its destructor. If the supplied string ends with
one line feed, a line feed will be written after the closing tag, but not the
opening one; if it ends with two line feeds, one will be written after both
tags.

The opening tag can be supplied with or without enclosing angle brackets. The
constructor does not attempt any validation of the tag's format (except that
an empty tag will cause the class to do nothing); no promises are made about
the output if the `element` argument is not a valid HTML/XML tag.

If the opening tag is standalone, the text will simply be written as is, and
no closing tag will be written. Standalone tags are identified by a closing
slash; the class is not aware of HTML's list of automatic self closing tags.

* `template <typename... Args> void` **`tagged`**`(std::ostream& out, const std::string& element, const Args&... args)`
* `template <typename T> void` **`tagged`**`(std::ostream& out, const std::string& element, const T& t)`

This function can be used to write a piece of literal text enclosed in one or
more tags. The arguments are the output stream, a list of tags (using the same
format as the `Tag` class), and an object that will be written to the output
stream enclosed by the tags.

Example:

    tagged(std::cout, "p\n", "code", "Hello world");

Output:

    <p><code>Hello world</code></p>\n

## Type names ##

* `std::string` **`demangle`**`(const std::string& name)`
* `std::string` **`type_name`**`(const std::type_info& t)`
* `std::string` **`type_name`**`(const std::type_index& t)`
* `template <typename T> std::string` **`type_name`**`()`
* `template <typename T> std::string` **`type_name`**`(const T& t)`

Demangle a type name. The original mangled name can be supplied as an explicit
string, as a `std::type_info` or `std:type_index` object, as a type argument
to a template function (e.g. `type_name<int>()`), or as an object whose type
is to be named (e.g. `type_name(42)`). The last version will report the
dynamic type of the referenced object.

## Unicode functions ##

* `template <typename S2, typename S1> S2` **`uconv`**`(const S1& s)`

Converts between UTF representations. The input and output types (`S1` and
`S2`) must be instantiations of `std::basic_string` with 8, 16, or 32 bit
character types. If the character types are the same size, the input string is
copied to the output without any validity checking; otherwise, invalid UTF in
the input is replaced with the standard Unicode replacement character
(`U+FFFD`) in the output.

* `template <typename C> size_t` **`ulength`**`(const std::basic_string<C>& s) noexcept`

Returns the number of characters (Unicode scalar values) in a string. The UTF
encoding is chosen based on `sizeof(C)`; the result is unspecified if the
string is not valid UTF.

* `template <typename C> bool` **`uvalid`**`(const std::basic_string<C>& s) noexcept`
* `template <typename C> bool` **`uvalid`**`(const std::basic_string<C>& s, size_t& n) noexcept`

Check a string for a valid UTF encoding; the encoding is deduced from the size
of the character type. The second version reports the number of code units
before an invalid code sequence was encountered (i.e. the size of a valid UTF
prefix; this will be equal to `s.size()` if the function returns true). These
will always succeed for an empty string.
