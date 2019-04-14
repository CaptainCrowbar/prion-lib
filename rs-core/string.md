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

The `""_doc` literal removes a leading blank line if present, strips all
trailing whitespace, converts tab indentation to 4 spaces, and strips leading
indentation from all text lines, using the indentation of the trailing line to
indicate where the left margin is intended to be. Results are unspecified if
the last line of the original string contains anything other than whitespace,
or if its indentation contains a mixture of spaces and tabs.

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

## Case conversion functions ##

* `class` **`CaseWords`**

This class breaks down a name (such as a programming language variable name)
into its component parts. Name elements are assumed to consist of letters
(Unicode general category `{Ll,Lo,Lt,Lu}`), possibly followed by marks
(`{Lm,M}`), or entirely of digits (`{N}`); any other character (`{C,P,S,Z}`)
is treated as a delimiter and discarded. Words can also be delimited by
casing: names in mixed case will be broken down in the natural way; a
subsequence of two or more capital letters will be assumed to be a single
word, except that if it is followed by any lower case letters, the last
capital letter is assumed to start a new word (this means that a mixed case
name like `"HTTPResponse"` will be correctly parsed as `"HTTP Response"`).
Numbers are parsed as separate name elements.

* `using CaseWords::`**`iterator`** `= [random access const iterator]`
* `using CaseWords::`**`value_type`** `= Ustring`

Member types.

* `CaseWords::`**`CaseWords`**`()`
* `CaseWords::`**`CaseWords`**`(Uview text)`
* `CaseWords::`**`CaseWords`**`(const Ustring& text)`
* `CaseWords::`**`CaseWords`**`(const char* text)`
* `CaseWords::`**`CaseWords`**`(const CaseWords& c)`
* `CaseWords::`**`CaseWords`**`(CaseWords&& c) noexcept`
* `CaseWords::`**`~CaseWords`**`() noexcept`
* `CaseWords& CaseWords::`**`operator=`**`(const CaseWords& c)`
* `CaseWords& CaseWords::`**`operator=`**`(CaseWords&& c) noexcept`

Life cycle functions. The constructors that take a string break it down as
described above.

* `const Ustring& CaseWords::`**`operator[]`**`(size_t i) const noexcept`

Returns the word element (as described above) at the given index, in case
folded form. Behaviour is undefined if `i>=size()`.

* `CaseWords& CaseWords::`**`operator+=`**`(const CaseWords& c)`
* `CaseWords` **`operator+`**`(const CaseWords& a, const CaseWords& b)`

Concatenate two word lists.

* `iterator CaseWords::`**`begin`**`() const noexcept`
* `iterator CaseWords::`**`end`**`() const noexcept`

Iterators over the internal list of words, in case folded form.

* `bool CaseWords::`**`empty`**`() const noexcept`

True if the word list is empty.

* `size_t CaseWords::`**`hash`**`() const noexcept`
* `class` **`std::hash<CaseWords>`**

Hash function.

* `void CaseWords::`**`push_front`**`(Uview text)`
* `void CaseWords::`**`push_back`**`(Uview text)`

Insert a new substring (broken down in the same way as the constructors) at
the beginning or end of the list.

* `size_t CaseWords::`**`size`**`() const noexcept`

Returns the number of word elements in the list.

* `Ustring CaseWords::`**`str`**`(Uview format = "f ") const`

Returns the words converted to a specified casing pattern. The first letter of
the format string selects the case conversion mode; the rest of the format
string is used as a delimiter between word elements (in initials mode, the
delimiter is also added after the last initial). The selector letters are
(case insensitive):

* `"c"` _= Camel case (first word in lower case, the rest in mixed case)_
* `"f"` _= Fold case_
* `"i"` _= Initials_
* `"l"` _= Lower case_
* `"s"` _= Sentence case_
* `"t"` _= Title case_
* `"u"` _= Upper case_

* `Ustring CaseWords::`**`camel_case`**`() const { return str("c"); }` _- e.g. `"quickBrownFox"`_
* `Ustring CaseWords::`**`fold_case`**`() const { return str("f "); }` _- e.g. `"quick brown fox"`_
* `Ustring CaseWords::`**`initials`**`() const { return str("i"); }` _- e.g. `"QBF"`_
* `Ustring CaseWords::`**`kebab_case`**`() const { return str("l-"); }` _- e.g. `"quick-brown-fox"`_
* `Ustring CaseWords::`**`lower_case`**`() const { return str("l "); }` _- e.g. `"quick brown fox"`_
* `Ustring CaseWords::`**`macro_case`**`() const { return str("u_"); }` _- e.g. `"QUICK_BROWN_FOX"`_
* `Ustring CaseWords::`**`pascal_case`**`() const { return str("t"); }` _- e.g. `"QuickBrownFox"`_
* `Ustring CaseWords::`**`sentence_case`**`() const { return str("s "); }` _- e.g. `"Quick brown fox"`_
* `Ustring CaseWords::`**`snake_case`**`() const { return str("l_"); }` _- e.g. `"quick_brown_fox"`_
* `Ustring CaseWords::`**`title_case`**`() const { return str("t "); }` _- e.g. `"Quick Brown Fox"`_
* `Ustring CaseWords::`**`upper_case`**`() const { return str("u "); }` _- e.g. `"QUICK BROWN FOX"`_

Common special cases.

* `void CaseWords::`**`swap`**`(CaseWords& c) noexcept`
* `void` **`swap`**`(CaseWords& a, CaseWords& b) noexcept`

Swap two objects.

* `bool` **`operator==`**`(const CaseWords& a, const CaseWords& b) noexcept`
* `bool` **`operator!=`**`(const CaseWords& a, const CaseWords& b) noexcept`
* `bool` **`operator<`**`(const CaseWords& a, const CaseWords& b) noexcept`
* `bool` **`operator>`**`(const CaseWords& a, const CaseWords& b) noexcept`
* `bool` **`operator<=`**`(const CaseWords& a, const CaseWords& b) noexcept`
* `bool` **`operator>=`**`(const CaseWords& a, const CaseWords& b) noexcept`

Comparison operators. These work on a word by word basis, comparing their case
folded forms.

* `std::ostream&` **`operator<<`**`(std::ostream& out, const CaseWords& c)`

Write the words to the output stream, in the default format of `str()` or
`fold_case()`.

## Character functions ##

* `template <typename T> constexpr T` **`char_to`**`(char c) noexcept`

A simple conversion function that casts a `char` to a larger integer type by
first passing it through `unsigned char`, to ensure that characters with the
high bit set end up as integers in the 128-255 range, and not as negative or
extremely high values.

## String property functions ##

* `bool` **`ascii_icase_equal`**`(std::string_view lhs, std::string_view rhs) noexcept`
* `bool` **`ascii_icase_less`**`(std::string_view lhs, std::string_view rhs) noexcept`

Comparison functions that treat upper and lower case ASCII letters as
equivalent (comparison between letters and non-letters uses the upper case
code points).

* `bool` **`starts_with`**`(std::string_view str, std::string_view prefix) noexcept`
* `bool` **`ends_with`**`(std::string_view str, std::string_view suffix) noexcept`

True if the string starts or ends with the specified substring.

* `bool` **`string_is_ascii`**`(std::string_view str) noexcept`

True if the string contains no 8-bit bytes.

## String manipulation functions ##

* `std::string` **`add_prefix`**`(std::string_view s, std::string_view prefix)`
* `std::string` **`add_suffix`**`(std::string_view s, std::string_view suffix)`
* `std::string` **`drop_prefix`**`(std::string_view s, std::string_view prefix)`
* `std::string` **`drop_suffix`**`(std::string_view s, std::string_view suffix)`

The `add_prefix/suffix()` functions add a prefix or suffix to the string if it
was not already there, or return the string unchanged if it was. The
`drop_prefix/suffix()` functions remove a prefix or suffix if it was present,
otherwise return the string unchanged.

* `template <typename... Args> std::string` **`catstr`**`(Args... args)`

Generalized string concatenation. The arguments can be any type that can be
implicitly or explicitly converted to `std::string`, `char`, or any other
arithmetic type that can be converted using `std::to_string()`.

* `Ustring` **`indent`**`(Uview str, size_t depth)`

Inserts `4*depth` spaces of indentation on every non-empty line.

* `template <typename InputRange> std::string` **`join`**`(const InputRange& range, std::string_view delim = "", bool term = false)`

Join strings into a single string, using the given delimiter. The value type
of the input range must be a valid argument to `make_view()`. If the `term`
argument is set, an extra delimiter will be added after the last element
(useful when joining lines to form a text that would be expected to end with a
line break).

* `std::string` **`linearize`**`(std::string_view str)`

Replaces all whitespace in a string with a single space, and trims leading and
trailing whitespace. All non-ASCII bytes are treated as non-whitespace
characters.

* `template <typename C> void` **`null_term`**`(std::basic_string<C>& str) noexcept`

Cut off a string at the first null character (useful after the string has been
used as an output buffer by some C APIs).

* `std::string` **`pad_left`**`(std::string_view str, size_t len, char pad = ' ')`
* `std::string` **`pad_right`**`(std::string_view str, size_t len, char pad = ' ')`

Pad a string on the right or left, using the supplied character, to the given
length (in bytes). The string will be returned unchanged if it is already at
least `len` bytes long.

* `std::pair<std::string_view, std::string_view>` **`partition_at`**`(std::string_view str, std::string_view delim)`
* `std::pair<std::string_view, std::string_view>` **`partition_by`**`(std::string_view str, std::string_view delims = ascii_whitespace)`

These break a string into two parts at the first delimiter, returning the
parts before and after the delimiter. If the delimiter is not found, or is
empty, the returned pair contains the complete original string and an empty
string. The `partition_at()` function breaks the string at the first
occurrence of the delimiter substring; `partition_by()` breaks it at the first
contiguous group of one or more bytes from the delimiter list.

* `std::string` **`repeat`**`(std::string_view s, size_t n, std::string_view delim = "")`

Returns a string containing `n` copies of `s`. If a delimiter is given, it
will be placed between each pair of elements.

* `std::string` **`replace`**`(std::string_view s, std::string_view target, std::string_view subst, size_t n = npos)`

Replaces the first `n` occurrences (all of them by default) of `target` in `s`
with `subst`. This will return the string unchanged if `target` is empty or
`n=0`.

* `template <typename OutputIterator> void` **`split`**`(std::string_view src, OutputIterator dst, std::string_view delim = ascii_whitespace)`
* `Strings` **`splitv`**`(std::string_view src, std::string_view delim = ascii_whitespace)`

Split a string into substrings, discarding any delimiter characters. Any
contiguous subsequence of delimiter characters will be treated as a break
point. If the input string is empty, the output list will always be empty;
otherwise, if the delimiter list is empty, the output will consist of a single
string matching the input. The value type of the output iterator in `split()`
must be assignment compatible with `Ustring`; the `splitv()` version returns a
vector of strings instead of writing to an existing receiver.

* `template <typename OutputIterator> void` **`split_lines`**`(std::string_view src, OutputIterator dst)`
* `Strings` **`splitv_lines`**`(std::string_view src)`

Split a string into lines. A line ends with any LF or CRLF; these are not
copied into the output.

* `std::string` **`trim`**`(std::string_view str, std::string_view chars = ascii_whitespace)`
* `std::string` **`trim_left`**`(std::string_view str, std::string_view chars = ascii_whitespace)`
* `std::string` **`trim_right`**`(std::string_view str, std::string_view chars = ascii_whitespace)`

Trim unwanted bytes from the ends of a string.

## String formatting functions ##

* `template <typename T> Ustring` **`expand_integer`**`(T t, char delim = '\'')`
* `template <typename T> Ustring` **`expand_hex`**`(T t, char delim = '\'')`

Write a integer with delimiters between groups of 3 decimal or 4 hex digits.
The hex version will pad with leading zeros to match the native size of the
integer, and insert a `"0x"` prefix.

* `template <typename... Args> Ustring` **`fmt`**`(Uview pattern, const Args&... args)`

This performs string interpolation, inserting the variadic arguments
(formatted with `to_str()`, below) in place of each occurrence of `"$n"` or
`"${n}"` in the pattern string, where `n` is a decimal integer interpreted as
a 1-based index into the variadic argument list. An index out of bounds will
be replaced with an empty string. If a dollar sign is not followed by a bare
or braced number, the dollar sign is discarded and the next character is
copied unchanged (so `"$$"` will produce a literal dollar sign).

* `template <size_t N> Ustring` **`hex`**`(const std::array<uint8_t, N>& bytes)`
* `Ustring` **`hexdump`**`(const void* ptr, size_t n, size_t block = 0)`
* `Ustring` **`hexdump`**`(std::string_view str, size_t block = 0)`

Convert a block of raw data into hexadecimal bytes. If `block` is not zero, a
line feed is inserted after each block.

* `Ustring` **`tf`**`(bool b)`
* `Ustring` **`yn`**`(bool b)`

Convert a boolean to `"true/false"` or `"yes/no"`.

* `template <typename Range> Strings` **`to_strings`**`(const Range& r)`

Converts each element of the range to a string, using `to_str()`, and returns
them as a vector.

## HTML/XML tags ##

* `class` **`Tag`**
    * `Tag::`**`Tag`**`()`
    * `Tag::`**`Tag`**`(std::ostream& out, std::string_view element)`
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

* `template <typename... Args> void` **`tagged`**`(std::ostream& out, std::string_view element, const Args&... args)`
* `template <typename T> void` **`tagged`**`(std::ostream& out, std::string_view element, const T& t)`

This function can be used to write a piece of literal text enclosed in one or
more tags. The arguments are the output stream, a list of tags (using the same
format as the `Tag` class), and an object that will be written to the output
stream enclosed by the tags.

Example:

    tagged(std::cout, "p\n", "code", "Hello world");

Output:

    <p><code>Hello world</code></p>\n
