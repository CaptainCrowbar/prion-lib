# English Language Specific Functions #

By Ross Smith

* `#include "rs-core/english.hpp"`

## Contents ##

[TOC]

## Case conversion functions ##

* `Ustring` **`extended_title_case`**`(Uview str)`
* `void` **`extended_title_case_in`**`(Ustring& str)`

Convert a string to title case, checking for common short English words that
are conventionally set in lower case.

* `template <typename T> Ustring` **`format_enum`**`(T t, Uview flags = "")`

Format an enumeration type originally defined using one of the
`RS_ENUM[_CLASS]()` macros. Type and namespace prefixes are removed. The flags
consist of an optional case conversion flag (defaulting to lower case),
followed by an optional delimiter to place between words (defaulting to a
space).

Case flags:

* `'l'` = lower case
* `'s'` = title case in first word, otherwise lower case
* `'t'` = title case
* `'x'` = extended title case (as above)

## List formatting functions ##

* `template <typename Range> Ustring` **`comma_list`**`(const Range& range, Uview conj = "")`

Format a list of strings as a comma delimited list, with an optional
conjunction between the last two items, following the usual English
conventions.

Examples:

    list = {"abc"};                comma_list(list) == "abc";
    list = {"abc", "def"};         comma_list(list) == "abc, def";
    list = {"abc", "def", "ghi"};  comma_list(list) == "abc, def, ghi";
    list = {"abc"};                comma_list(list, "and") == "abc";
    list = {"abc", "def"};         comma_list(list, "and") == "abc and def";
    list = {"abc", "def", "ghi"};  comma_list(list, "and") == "abc, def, and ghi";

## Number formatting functions ##

* `Ustring` **`cardinal`**`(size_t n, size_t threshold = npos)`
* `Ustring` **`ordinal`**`(size_t n, size_t threshold = npos)`

Generate the English name for a number, in cardinal (`"forty-two"`) or ordinal
(`"forty-second"`) form. Numbers greater than or equal to the threshold value
will be left in numeric form (with a suffix on the ordinal numbers).

* `Ustring` **`format_count`**`(double x, int prec)`

Formats a number to the specified number of significant digits. the number is
written directly, rounded to the nearest integer, if it is under
10<sup>4</sup>; followed by a name if it is under 10<sup>12</sup>; or in
scientific notation otherwise.

Examples:

    format_count(1.23, 3) == "1";
    format_count(12.3, 3) == "12";
    format_count(123.0, 3) == "123";
    format_count(1.23e3, 3) == "1230";
    format_count(1.23e4, 3) == "12.3 thousand";
    format_count(1.23e5, 3) == "123 thousand";
    format_count(1.23e6, 3) == "1.23 million";
    format_count(1.23e7, 3) == "12.3 million";
    format_count(1.23e8, 3) == "123 million";
    format_count(1.23e9, 3) == "1.23 billion";
    format_count(1.23e10, 3) == "12.3 billion";
    format_count(1.23e11, 3) == "123 billion";
    format_count(1.23e12, 3) == "1.23x10^12";
    format_count(1.23e13, 3) == "1.23x10^13";
    format_count(1.23e14, 3) == "1.23x10^14";
    format_count(1.23e15, 3) == "1.23x10^15";

* `Ustring` **`number_of`**`(size_t n, Uview name, Uview plural_name = "", size_t threshold = 21)`

Combines a cardinal number with the singular or plural of a noun. The plural
form may be supplied explicitly; if not, it will be obtained by calling the
`plural()` function described below. The `threshold` argument has the same
meaning as for the `cardinal()` function.

Examples:

    number_of(0, "thing") == "no things";
    number_of(1, "thing") == "one thing";
    number_of(2, "thing") == "two things";
    number_of(3, "thing") == "three things";
    number_of(10, "thing") == "ten things";
    number_of(20, "thing") == "twenty things";
    number_of(30, "thing") == "30 things";

## Pluralization functions ##

* `Ustring` **`plural`**`(Uview noun)`

Returns the plural form of a singular noun. This function is aware of most of
the irregular plurals in English.

Partly based on [Damian Conway's work](http://www.csse.monash.edu.au/~damian/papers/HTML/Plurals.html).
