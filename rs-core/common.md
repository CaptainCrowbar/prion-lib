# Common Utilities #

By Ross Smith

* `#include "rs-core/common.hpp"`

## Contents ##

[TOC]

## Preprocessor macros ##

* `#define` **`RS_BITMASK_OPERATORS`**`(EC)`

Defines bit manipulation and related operators for an `enum class` (unary `!`,
`~`; binary `&`, `&=`, `|`, `|=`, `^`, `^=`).

* `#define` **`RS_ENUM`**`(EnumType, IntType, first_value, first_name, ...)`
* `#define` **`RS_ENUM_CLASS`**`(EnumType, IntType, first_value, first_name, ...)`

These define an enumeration, given the name of the enumeration type, the
underlying integer type, the integer value of the first entry, and a list of
value names. They will also define the following functions:

* `std::ostream&` **`operator<<`**`(std::ostream& out, EnumType t)`
* `constexpr bool` **`enum_is_valid`**`(EnumType t) noexcept`
* `std::vector<EnumType>` **`enum_values<EnumType>`**`()`

The output operator prints the name of an enumeration constant (qualified with
the class name if this is an `enum class`), or the integer value if the
argument is not a named value. The `enum_is_valid()` function reports whether
or not the argument is a named value of the enumeration. The
`enum_values<T>()` function returns a `vector` containing a list of the
enumeration's values.

Example:

    RS_ENUM(Foo, 1, alpha, bravo, charlie)
    RS_ENUM_CLASS(Bar, 1, delta, echo, foxtrot)

Equivalent code:

    enum Foo { alpha = 1, bravo, charlie };
    std::ostream& operator<<(std::ostream& out, Foo f) { ... }
    enum class Bar { delta = 1, echo, foxtrot };
    std::ostream& operator<<(std::ostream& out, Bar b) { ... }

* `#define` **`RS_LDLIB`**`([tag:] lib ...)`

This instructs the makefile to link with one or more static libraries. Specify
library names without the `-l` prefix (e.g. `RS_LDLIB(foo)` will link with
`-lfoo`). If link order is important for a particular set of libraries, supply
them in a space delimited list in a single `RS_LDLIB()` line.

Libraries that are needed only on specific targets can be prefixed with one of
the target identifiers listed below (e.g. `RS_LDLIB(apple:foo)` will link
with `-lfoo` for Apple targets only). Only one target can be specified per
invocation; if the same libraries are needed on multiple targets, but not on
all targets, you will need a separate `RS_LDLIB()` line for each target.

<!-- TEXT -->
* `apple:`
* `cygwin:`
* `linux:`
* `msvc:`

`RS_LDLIB()` lines are picked up at the `"make dep"` stage; if you change a
link library, the change will not be detected until dependencies are rebuilt.

* `#define` **`RS_MOVE_ONLY`**`(T)`
    * `T(const T&) = delete;`
    * `T(T&&) = default;`
    * `T& operator=(const T&) = delete;`
    * `T& operator=(T&&) = default;`
* `#define` **`RS_NO_COPY_MOVE`**`(T)`
    * `T(const T&) = delete;`
    * `T(T&&) = delete;`
    * `T& operator=(const T&) = delete;`
    * `T& operator=(T&&) = delete;`

Convenience macros for defaulted or deleted copy and move operations.

* `#define` **`RS_OVERLOAD`**`(f) [] (auto&&... args) { return f(std::forward<decltype(args)>(args)...); }`

Creates a function object wrapping a set of overloaded functions, that can be
passed to a context expecting a function (such as an STL algorithm) without
having to explicitly resolve the overload at the call site. (From an idea by
Arthur O'Dwyer on the C++ standard proposals mailing list, 14 Sep 2015.)

## Types ##

### Integer types ###

* `enum` **`ByteOrder`**
    * **`big_endian`**
    * **`little_endian`**
* `template <typename T, ByteOrder B> class` **`Endian`**
    * `using Endian::`**`value_type`** `= T`
    * `static constexpr ByteOrder Endian::`**`byte_order`** `= B`
    * `constexpr Endian::`**`Endian`**`() noexcept`
    * `constexpr Endian::`**`~Endian`**`() noexcept`
    * `constexpr Endian::`**`Endian`**`(const Endian& e) noexcept`
    * `constexpr Endian::`**`Endian`**`(Endian&& e) noexcept`
    * `constexpr Endian& Endian::`**`operator=`**`(const Endian& e) noexcept`
    * `constexpr Endian& Endian::`**`operator=`**`(Endian&& e) noexcept`
    * `constexpr Endian::`**`Endian`**`(T t) noexcept`
    * `explicit Endian::`**`Endian`**`(const void* p) noexcept`
    * `constexpr Endian::`**`operator T`**`() const noexcept`
    * `constexpr T Endian::`**`get`**`() const noexcept`
    * `constexpr const T* Endian::`**`ptr`**`() const noexcept`
    * `T* Endian::`**`ptr`**`() noexcept`
    * `constexpr T Endian::`**`rep`**`() const noexcept`
    * `T& Endian::`**`rep`**`() noexcept`
* `template <typename T> using` **`BigEndian`** `= Endian<T, big_endian>`
* `template <typename T> using` **`LittleEndian`** `= Endian<T, little_endian>`
* `template <typename T, ByteOrder B> std::ostream&` **`operator<<`**`(std::ostream& out, Endian<T, B> t)`
* `template <typename T, ByteOrder B> class std::`**`hash`**`<Endian<T, B>>`
* _comparison operators between all combinations of `Endian` and `T`_

An `Endian` object holds an integer in a defined byte order. This is a literal
type and can be used in `constexpr` expressions.

Assignment to or from an endian integer performs any necessary reordering
transparently. The default constructor sets the value to zero; the constructor
from a pointer copies `sizeof(T)` bytes into the object. The `ptr()` and
`rep()` functions give access to the internal, byte ordered form. The hash
function gives the same result as the underlying integer type's hash.

### String and character types ###

* `using` **`Ustring`** `= std::string`
* `using` **`Uview`** `= std::string_view`

Use `Ustring` or `Uview` for strings that are expected to be in UTF-8 (or
ASCII, since any ASCII string is also valid UTF-8), while plain `std::string`
or `std::string_view` is used where the string is expected to be in some non-
Unicode encoding, or where the string is being used simply as an array of
bytes rather than encoded text.

* `using` **`Strings`** `= std::vector<std::string>`

Commonly used type defined for convenience.

* `#define` **`RS_NATIVE_WCHAR`** `1` _- defined if the system API uses wide characters_
* `using` **`NativeCharacter`** `= [char on Unix, wchar_t on Windows]`
* `using` **`NativeString`** `= [std::string on Unix, std::wstring on Windows]`

These are defined to reflect the character types used in the operating
system's native API.

* `#define` **`RS_WCHAR_UTF16`** `1` _- defined if wchar_t and wstring are UTF-16_
* `#define` **`RS_WCHAR_UTF32`** `1` _- defined if wchar_t and wstring are UTF-32_
* `using` **`WcharEquivalent`** `= [char16_t or char32_t]`
* `using` **`WstringEquivalent`** `= [std::u16string or std::u32string]`

These are defined to reflect the encoding represented by `wchar_t` and
`std::wstring`. Systems where wide strings are neither UTF-16 nor UTF-32 are
not supported.

* `template <typename C> basic_string<C>` **`cstr`**`(const C* ptr)`
* `template <typename C> basic_string<C>` **`cstr`**`(const C* ptr, size_t n)`

These construct a string from a pointer to a null-terminated character
sequence, or a pointer and a length. They differ from the corresponding string
constructors in that passing a null pointer will yield an empty string, or a
string of `n` null characters, instead of undefined behaviour.

* `template <typename C> size_t` **`cstr_size`**`(const C* ptr)`

Returns the length of a null-terminated string (a generalized version of
`strlen()`). This will return zero if the pointer is null.

### Exceptions ###

* `void` **`rethrow`**`(std::exception_ptr p)`

Rethrows the exception, if one is present. This is the same as
`std::rethrow_exception()`, except that it does nothing if the exception
pointer is null.

### Metaprogramming and type traits ###

* `template <typename T> using` **`BinaryType`** `= [unsigned integer type]`

Yields an unsigned integer type the same size as `T`. This will fail to
compile if no such type exists.

* `template <typename T1, typename T2> using` **`CopyConst`** `= ...`

Yields a type created by transferring the `const` qualification (or lack of
it) from `T1` to the unqualified type of `T2`. For example, `CopyConst<int,
const string>` yields `string`, while `CopyConst<const int, string>` yields
`const string`.

* `template <size_t Bits> using` **`SignedInteger`** `= [signed integer type]`
* `template <size_t Bits> using` **`UnsignedInteger`** `= [unsigned integer type]`

Signed and unsigned integer types with the specified number of bits (the same
types as `int8_t`, `int16_t`, etc). These will fail to compile if `Bits` is
not a power of 2 in the supported range (8-64).

### Mixins ###

<!-- DEFN --> These are convenience base classes that define members and
operators that would normally just be repetitive boilerplate (similar to the
ones in Boost). They all use the CRTP idiom; a class `T` should derive from
`Mixin<T>` to automatically generate the desired boilerplate code. The table
below shows which operations the user is required to define, and which ones
the mixin will automatically define. (Here, `t` and `u` are objects of type
`T`, `v` is an object of `T`'s value type, and `n` is an integer.)

Mixin                                       | Requires                         | Defines
-----                                       | --------                         | -------
**`EqualityComparable`**`<T>`               | `t==u`                           | `t!=u`
**`LessThanComparable`**`<T>`               | `t==u, t<u`                      | `t!=u, t>u, t<=u, t>=u`
**`InputIterator`**`<T,CV>`                 | `*t, ++t, t==u`                  | `t->, t++, t!=u`
**`OutputIterator`**`<T>`                   | `t=v`                            | `*t, ++t, t++`
**`ForwardIterator`**`<T,CV>`               | `*t, ++t, t==u`                  | `t->, t++, t!=u`
**`BidirectionalIterator`**`<T,CV>`         | `*t, ++t, --t, t==u`             | `t->, t++, t--, t!=u`
**`RandomAccessIterator`**`<T,CV>`          | `*t, t+=n, t-u`                  | `t->, t[n], ++t, t++, --t, t--, t-=n, t+n, n+t, t-n,`<br>`t==u, t!=u, t<u, t>u, t<=u, t>=u`
**`FlexibleRandomAccessIterator`**`<T,CV>`  | `*t, ++t, --t, t+=n, t-u, t==u`  | `t->, t[n], t++, t--, t-=n, t+n, n+t, t-n,`<br>`t!=u, t<u, t>u, t<=u, t>=u`

In the iterator mixins, `CV` is either `V` or `const V`, where `V` is the
iterator's value type, depending on whether a mutable or const iterator is
required.

The first version of `RandomAccessIterator` uses the minimal set of user
supplied operations to generate all of those required;
`FlexibleRandomAccessIterator` requires more user supplied operations, but
will decay safely to one of the simpler iterator types if an underlying type
does not supply all of the corresponding operations.

In addition to the operators listed in the table above, all iterator mixins
supply the standard member types:

* `using` **`difference_type`** `= ptrdiff_t`
* `using` **`iterator_category`** `= [standard iterator tag type]`
* `using` **`pointer`** `= CV*`
* `using` **`reference`** `= CV&`
* `using` **`value_type`** `= std::remove_const_t<CV>`

### Type adapters ###

* `template <typename T, int Def = 0> struct` **`AutoMove`**
    * `using AutoMove::`**`value_type`** `= T`
    * `T AutoMove::`**`value`**
    * `AutoMove::`**`AutoMove`**`()`
    * `AutoMove::`**`AutoMove`**`(const T& t)`
    * `AutoMove::`**`AutoMove`**`(T&& t)`
    * `AutoMove::`**`~AutoMove`**`()`
    * `AutoMove::`**`AutoMove`**`(const AutoMove& x)`
    * `AutoMove::`**`AutoMove`**`(AutoMove&& x)`
    * `AutoMove& AutoMove::`**`operator=`**`(const AutoMove& x)`
    * `AutoMove& AutoMove::`**`operator=`**`(AutoMove&& x)`

This wraps a `T` object and provides reset-on-move behaviour: `AutoMove<T>`'s
move constructor and move assignment operator set the value in the moved-from
object to `T`'s default value. If `T` is explicitly convertible from an `int`,
a default value other than zero can be provided. `AutoMove<T>` will be
copyable if `T` is copyable. (This is basically a simplified version of the
`Resource` scope guard template.)

* `template <typename T, int Def = 0> struct` **`NoTransfer`**
    * `using NoTransfer::`**`value_type`** `= T`
    * `T NoTransfer::`**`value`**
    * `NoTransfer::`**`NoTransfer`**`()`
    * `NoTransfer::`**`NoTransfer`**`(const T& t)`
    * `NoTransfer::`**`NoTransfer`**`(T&& t)`
    * `NoTransfer::`**`~NoTransfer`**`()`
    * `NoTransfer::`**`NoTransfer`**`(const NoTransfer& x)`
    * `NoTransfer::`**`NoTransfer`**`(NoTransfer&& x)`
    * `NoTransfer& NoTransfer::`**`operator=`**`(const NoTransfer& x)`
    * `NoTransfer& NoTransfer::`**`operator=`**`(NoTransfer&& x)`

This is more or less the reverse of `AutoMove`: the embedded value always
stays with its original `NoTransfer` object and is not moved or copied;
instead, moving or copying a `NoTransfer` sets the value of the destination
object to its default value. This is intended for data members that need to
remain associated with object identity rather than object value.

### Type related functions ###

* `template <typename T2, typename T1> bool` **`is`**`(const T1& ref) noexcept`
* `template <typename T2, typename T1> bool` **`is`**`(const T1* ptr) noexcept`
* `template <typename T2, typename T1> bool` **`is`**`(const unique_ptr<T1>& ptr) noexcept`
* `template <typename T2, typename T1> bool` **`is`**`(const shared_ptr<T1>& ptr) noexcept`
* `template <typename T2, typename T1> T2&` **`as`**`(T1& ref)`
* `template <typename T2, typename T1> const T2&` **`as`**`(const T1& ref)`
* `template <typename T2, typename T1> T2&` **`as`**`(T1* ptr)`
* `template <typename T2, typename T1> const T2&` **`as`**`(const T1* ptr)`
* `template <typename T2, typename T1> T2&` **`as`**`(unique_ptr<T1>& ptr)`
* `template <typename T2, typename T1> T2&` **`as`**`(const unique_ptr<T1>& ptr)`
* `template <typename T2, typename T1> T2&` **`as`**`(shared_ptr<T1>& ptr)`
* `template <typename T2, typename T1> T2&` **`as`**`(const shared_ptr<T1>& ptr)`

These are simple wrappers around `dynamic_cast`. The `is()` function returns
true if the pointer or reference's target has the requested dynamic type;
`as()` returns a reference to the object converted to the requested dynamic
type, or throws `std::bad_cast` if the `dynamic_cast` fails.

* `template <typename T2, typename T1> T2` **`binary_cast`**`(const T1& t) noexcept`
* `template <typename T2, typename T1> T2` **`implicit_cast`**`(const T1& t)`

Type conversions. The `binary_cast()` operation does a simple bitwise copy
from one type to another; it will fail to compile if the two types have
different sizes, but does no other safety checks. The `implicit_cast()`
operation performs the conversion only if it would be allowed as an implicit
conversion.

### Version number ###

* `class` **`Version`**
    * `using Version::`**`value_type`** `= unsigned`
    * `Version::`**`Version`**`() noexcept`
    * `template <typename... Args> Version::`**`Version`**`(unsigned n, Args... args)`
    * `explicit Version::`**`Version`**`(const Ustring& s)`
    * `Version::`**`Version`**`(const Version& v)`
    * `Version::`**`Version`**`(Version&& v) noexcept`
    * `Version::`**`~Version`**`() noexcept`
    * `Version& Version::`**`operator=`**`(const Version& v)`
    * `Version& Version::`**`operator=`**`(Version&& v) noexcept`
    * `unsigned Version::`**`operator[]`**`(size_t i) const noexcept`
    * `const unsigned* Version::`**`begin`**`() const noexcept`
    * `const unsigned* Version::`**`end`**`() const noexcept`
    * `unsigned Version::`**`major`**`() const noexcept`
    * `unsigned Version::`**`minor`**`() const noexcept`
    * `unsigned Version::`**`patch`**`() const noexcept`
    * `size_t Version::`**`size`**`() const noexcept`
    * `string Version::`**`str`**`(size_t min_elements = 2) const`
    * `string Version::`**`suffix`**`() const`
    * `uint32_t Version::`**`to32`**`() const noexcept`
    * `static Version Version::`**`from32`**`(uint32_t n) noexcept`
* `bool` **`operator==`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator!=`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator<`**`(const Version& lhs, const Version& rhs) noexcept;`
* `bool` **`operator>`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator<=`**`(const Version& lhs, const Version& rhs) noexcept`
* `bool` **`operator>=`**`(const Version& lhs, const Version& rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& o, const Version& v)`

A version number, represented as an array of integers optionally followed by a
trailing string (e.g. `1.2.3beta`). The default constructor sets the version
number to zero; the second constructor expects one or more integers followed
by a string; the third constructor parses a string (a string that does not
start with an integer is assumed to start with an implicit zero). Results are
unspecified if a version number element is too big to fit in an `unsigned
int`.

The indexing operator returns the requested element; it will return zero if
the index is out of range for the stored array. The `major()`, `minor()`, and
`patch()` functions return elements 0, 1, and 2. The `suffix()` function
returns the trailing string element.

The `size()` function returns the number of numeric elements in the version
number. This will always return at least 1.

The `str()` function (and the output operator) formats the version number in
the conventional form; a minimum number of elements can be requested. The
`to32()` and `from32()` functions pack or unpack the version into a 32 bit
integer, with one byte per element (e.g. version `1.2.3` becomes
`0x01020300`); `to32()` truncates elements higher than 255 and ignores the
string element.

## Constants and literals ##

### Arithmetic literals ###

* `namespace RS::`**`Literals`**
    * `constexpr int8_t` **`operator""_s8`**`(unsigned long long n) noexcept`
    * `constexpr uint8_t` **`operator""_u8`**`(unsigned long long n) noexcept`
    * `constexpr int16_t` **`operator""_s16`**`(unsigned long long n) noexcept`
    * `constexpr uint16_t` **`operator""_u16`**`(unsigned long long n) noexcept`
    * `constexpr int32_t` **`operator""_s32`**`(unsigned long long n) noexcept`
    * `constexpr uint32_t` **`operator""_u32`**`(unsigned long long n) noexcept`
    * `constexpr int64_t` **`operator""_s64`**`(unsigned long long n) noexcept`
    * `constexpr uint64_t` **`operator""_u64`**`(unsigned long long n) noexcept`
    * `constexpr ptrdiff_t` **`operator""_pt`**`(unsigned long long n) noexcept`
    * `constexpr size_t` **`operator""_sz`**`(unsigned long long n) noexcept`

Integer literals.

### String related constants ###

* `constexpr const char*` **`ascii_whitespace`** `= "\t\n\v\f\r "`

ASCII whitespace characters.

* `constexpr size_t` **`npos`** `= string::npos`

Defined for convenience. Following the conventions established by the standard
library, this value is often used as a function argument to mean "as large as
possible" or "no limit", or as a return value to mean "not found".

### Other constants ###

* `constexpr bool` **`big_endian_target`**
* `constexpr bool` **`little_endian_target`**

One of these will be true and the other false, reflecting the target system's
byte order.

* `constexpr unsigned` **`KB`** `= 1024`
* `constexpr unsigned long` **`MB`** `= 1 048 576`
* `constexpr unsigned long` **`GB`** `= 1 073 741 824`
* `constexpr unsigned long long` **`TB`** `= 1 099 511 627 776`

Powers of 2<sup>10</sup>.

## Algorithms and ranges ##

### Generic algorithms ###

* `template <typename Container> [output iterator]` **`append`**`(Container& con)`
* `template <typename Container> [output iterator]` **`overwrite`**`(Container& con)`

These create output iterators that will append elements to a standard
container (see `append_to()` below). The `append()` function is similar to
`std::back_inserter()` (but supports containers without `push_back()`), while
`overwrite()` will first clear the container and then return the append
iterator.

* `template <typename Container, typename T> void` **`append_to`**`(Container& con, const T& t)`

Appends an item to a container; used by `append()` and `overwrite()`. The
generic version calls `con.insert(con.end(), t)`; overloads (found by argument
dependent lookup) can be used for container-like types that do not have a
suitable `insert()` method.

* `template <typename Range1, typename Range2> int` **`compare_3way`**`(const Range1& r1, const Range2& r2)`
* `template <typename Range1, typename Range2, typename Compare> int` **`compare_3way`**`(const Range1& r1, const Range2& r2, Compare cmp)`

Compare two ranges, returning -1 if the first range is less than the second,
zero if they are equal, and +1 if the first range is greater.

* `template <typename R, typename Container> void` **`con_append`**`(const R& src, Container& dst)`
* `template <typename R, typename Container> void` **`con_overwrite`**`(const R& src, Container& dst)`

These are just shorthand for a `std::copy()` from a range to an append or
overwrite iterator.

* `template <typename Container, typename T> void` **`con_remove`**`(Container& con, const T& t)`
* `template <typename Container, typename Predicate> void` **`con_remove_if`**`(Container& con, Predicate p)`
* `template <typename Container, typename Predicate> void` **`con_remove_if_not`**`(Container& con, Predicate p)`
* `template <typename Container> void` **`con_unique`**`(Container& con)`
* `template <typename Container, typename BinaryPredicate> void` **`con_unique`**`(Container& con, BinaryPredicate p)`
* `template <typename Container> void` **`con_sort_unique`**`(Container& con)`
* `template <typename Container, typename Compare> void` **`con_sort_unique`**`(Container& con, Compare cmp)`

These carry out the same algorithms as the similarly named STL functions,
except that unwanted elements are removed from the container rather than
shuffled to the end. The `con_sort_unique()` functions perform a sort followed
by removing equivalent elements from the container; like `std::sort()`, its
predicate has less-than semantics (whereas that of `con_unique()`, like that
of `std::unique()`, has equality semantics).

* `template <typename InputRange, typename... Ranges> std::vector<[value type]>` **`concatenate`**`(const InputRange& range, const Ranges&... ranges)`

Returns a vector composed by concatenating all of the argument ranges. The
value type is that of the first argument range. If any subsequent argument has
a different value type, it must be assignment compatible with the first.

* `template <typename T, typename F> void` **`do_n`**`(T n, F f)`
* `template <typename T, typename F> void` **`for_n`**`(T n, F f)`

These simply call the function `n` times. `T` must be an integer. The `do_n()`
loop expects a function that takes no arguments, while `for_n()` passes the
iteration count (zero to `n-1`) as an argument.

* `template <typename Container> void` **`ensure_min`**`(Container& con, size_t n)`
* `template <typename Container, typename T> void` **`ensure_min`**`(Container& con, size_t n, const T& t)`
* `template <typename Container> void` **`ensure_max`**`(Container& con, size_t n)`

If `con.size()<n`, `ensure_min()` calls `con.resize(n[,t])`; otherwise, it
does nothing. If `con.size()>n`, `ensure_max()` calls `con.resize(n)`;
otherwise, it does nothing.

* `template <typename M, typename K> M::mapped_type` **`find_in_map`**`(const M& map, const K& key)`
* `template <typename M, typename K, typename T> M::mapped_type` **`find_in_map`**`(const M& map, const K& key, const T& def)`

Find a key in a map, returning the corresponding value, or the supplied
default value (or a default constructed `mapped_type`) if the key is not
found.

* `template <typename Range1, typename Range2> bool` **`sets_intersect`**`(const Range1& r1, const Range2& r2)`
* `template <typename Range1, typename Range2, typename Compare> bool` **`sets_intersect`**`(const Range1& r1, const Range2& r2, Compare c)`

Test whether two ordered ranges (sets) have any elements in common. The ranges
are assumed to be ordered in accordance with the comparison predicate.

* `template <typename... Args> void` **`sort_list`**`(Args&... args)`
* `template <typename Compare, typename... Args> void` **`sort_list_by`**`(Compare c, Args&... args)`

Sort an explicit list of variables.

* `template <typename InputRange> [value type]` **`sum_of`**`(const InputRange& r)`
* `template <typename InputRange> [value type]` **`product_of`**`(const InputRange& r)`

Simple range sum and product functions, to save the trouble of calling
`std::accumulate()` in trivial cases. Calling `sum_of()` on an empty range
will return a default constructed value. The value type of `product_of()` must
be constructible by a `static_cast` from `int`; an empty range will return 1.

### Integer sequences ###

* `template <typename T> Irange<[random access iterator]>` **`iseq`**`(T stop) noexcept`
* `template <typename T> Irange<[random access iterator]>` **`iseq`**`(T init, T stop) noexcept`
* `template <typename T> Irange<[random access iterator]>` **`iseq`**`(T init, T stop, T delta) noexcept`
* `template <typename T> Irange<[random access iterator]>` **`xseq`**`(T stop) noexcept`
* `template <typename T> Irange<[random access iterator]>` **`xseq`**`(T init, T stop) noexcept`
* `template <typename T> Irange<[random access iterator]>` **`xseq`**`(T init, T stop, T delta) noexcept`

Linear sequences of integers (`T` must be an integer type). The generated
range starts with `init` (defaulting to zero) and runs to `stop`, in steps of
`delta` (defaulting to 1). The `iseq()` function produces an inclusive
(closed) sequence that includes `stop`, while `xseq()` is an exclusive (half
open) sequence that excludes it (if the exact value `stop` would never be
generated, because `stop-init` is not a multiple of `delta`, there is no
difference between the two).

If `stop=init`, or if `delta=0`, the `iseq()` sequence contains a single
value, while the `xseq()` sequence is empty. Both sequences will be empty if
`stop-init` and `delta` have opposite signs.

The iterators must be able to generate a value beyond the end of the sequence
in order to check for an end iterator; for `iseq()`, this means that
`stop+delta` must be in range for `T`.

### Memory algorithms ###

* `int` **`mem_compare`**`(const void* lhs, size_t n1, const void* rhs, size_t n2) noexcept`

This returns -1 if the first block is less than the second (by a
lexicographical bytewise comparison), 0 if they are equal, 1 if the first is
greater than the second. If the two blocks are equal up to the length of the
shorter one, the shorter is considered less than the longer. This is similar
to `memcmp()`, except that the memory blocks being compared may have different
lengths. A null pointer is treated as less than any non-null pointer; the
length accompanying a null pointer is ignored.

* `size_t` **`mem_match`**`(const void* lhs, const void* rhs, size_t n) noexcept`

Returns the number of leading bytes the two blocks have in common. This will
return zero if either pointer is null.

* `void` **`mem_swap`**`(void* ptr1, void* ptr2, size_t n) noexcept`

Swap two blocks of memory. This will work if the two ranges overlap or are the
same, but behaviour is undefined if either pointer is null.

### Range traits ###

* `template <typename Iterator> using` **`IteratorValueType`**
* `template <typename Range> using` **`RangeIteratorType`**
* `template <typename Range> using` **`RangeValueType`**

These return the types associated with an iterator or range. They are
extracted by checking the return type of `begin()`, and do not require a
specialization of `iterator_traits` to exist.

* `template <typename T, size_t N> constexpr size_t` **`array_count`**`(T[N]) noexcept`
* `template <typename InputRange> size_t` **`range_count`**`(const InputRange& r)`
* `template <typename InputRange> bool` **`range_empty`**`(const InputRange& r)`

Return the length of a range. The `range_count()` function is just shorthand
for `std::distance(begin(r),end(r))`, and `range_empty()` has the obvious
meaning. The `array_count()` version returns the same value, but only works on
C-style arrays and is `constexpr`.

### Range types ###

* `template <typename Iterator> struct` **`Irange`**
    * `using Irange::`**`iterator`** `= Iterator`
    * `using Irange::`**`value_type`** `= [Iterator's value type]`
    * `Iterator Irange::`**`first`**
    * `Iterator Irange::`**`second`**
    * `constexpr Iterator Irange::`**`begin`**`() const { return first; }`
    * `constexpr Iterator Irange::`**`end`**`() const { return second; }`
    * `constexpr bool Irange::`**`empty`**`() const { return first == second; }`
    * `constexpr size_t Irange::`**`size`**`() const { return std::distance(first, second); }`
* `template <typename Iterator> constexpr Irange<Iterator>` **`irange`**`(const Iterator& i, const Iterator& j)`
* `template <typename Iterator> constexpr Irange<Iterator>` **`irange`**`(const pair<Iterator, Iterator>& p)`

A wrapper for a pair of iterators, usable as a range in standard algorithms.

* `template <typename T> constexpr Irange<T*>` **`array_range`**`(T* ptr, size_t len)`

Returns `irange(ptr,ptr+len)`.

* `template <typename T, size_t N> constexpr std::tuple<T,...>` **`array_to_tuple`**`(const T (&array)[N])`
* `template <typename T, size_t N> constexpr std::tuple<T,...>` **`array_to_tuple`**`(const std::array<T, N> &array)`

Convert a C-style array or a `std::array` to a tuple containing the same
values.

## Arithmetic functions ##

### Generic arithmetic functions ###

* `template <typename T, typename T2, typename T3> constexpr T` **`clamp`**`(const T& x, const T2& min, const T3& max) noexcept`

Clamps a value to a fixed range. This returns `min` if `t<min`, `max` if
`t>max`, otherwise `t`. `T2` and `T3` must be implicitly convertible t `T`.

* `template <typename T> pair<T, T>` **`divide`**`(T x, T y) noexcept`
* `template <typename T> T` **`quo`**`(T x, T y) noexcept`
* `template <typename T> T` **`rem`**`(T x, T y) noexcept`

These perform Euclidean division: if the division is not exact, the remainder
is always positive (regardless of the signs of the arguments), and the
quotient is the integer that satisfies `x=q*y+r` (apart from rounding errors).
The pair returned by `divide()` contains the quotient and remainder. For all
three functions, behaviour is undefined if `y=0` or the quotient is out of
`T`'s representable range.

* `template <typename T> std::pair<T, T>` **`symmetric_divide`**`(T x, T y) noexcept`
* `template <typename T> T` **`symmetric_quotient`**`(T x, T y) noexcept`
* `template <typename T> T` **`symmetric_remainder`**`(T x, T y) noexcept`

These perform the same operations as the division functions above, except that
the remainder is constrained to `(-|y|/2,|y|/2]` instead of `[0,|y|)`.
Behaviour is undefined if `y=0` or the quotient is out of `T`'s representable
range. These functions do not make sense (and will not compile) if `T` is
unsigned.

* `template <typename T> T` **`shift_left`**`(T t, int n) noexcept`
* `template <typename T> T` **`shift_right`**`(T t, int n) noexcept`

Multiply or divide `t` by <code>2<sup>n</sup></code>. `T` may be an integer or
floating point type. Results are unspecified if the correct result would be
out of `T`'s representable range.

* `template <typename T> int` **`sign_of`**`(T t) noexcept`

Returns 1 if the argument is positive, 0 if zero, and -1 if negative.

### Integer arithmetic functions ###

* `template <typename T> constexpr std::make_signed_t<T>` **`as_signed`**`(T t) noexcept`
* `template <typename T> constexpr std::make_unsigned_t<T>` **`as_unsigned`**`(T t) noexcept`

These return their argument converted to a signed or unsigned value of the
same size (the argument is returned unchanged if `T` already had the desired
signedness). Behaviour is undefined if `T` is not an integer or enumeration
type.

* `template <typename T> T` **`binomial`**`(T a, T b) noexcept`
* `double` **`xbinomial`**`(int a, int b) noexcept`

These return the binomial coefficient (`a!/b!(a-b)!` if `0<=b<=a`, otherwise
zero). `T` must be an integer type. Behaviour is undefined if the correct
result would be out of range for the return type.

* `template <typename T> constexpr T` **`gcd`**`(T a, T b) noexcept`
* `template <typename T> constexpr T` **`lcm`**`(T a, T b) noexcept`

Return the greatest common divisor or lowest common multiple of two numbers.
The argument type must be an integer. If one argument is zero, `gcd()` will
return the other one, and `lcm()` will return zero. The return value is always
positive (if not zero), regardless of the signs of the arguments. If the
correct result for `lcm()` would be outside the range of `T`, behaviour is
undefined if `T` is signed, unspecified if `T` is unsigned.

* `template <typename T> T` **`int_power`**`(T x, T y) noexcept`

Calculates <code>x<sup>y</sup></code> for integer types.
<code>0<sup>0</sup></code> will return 1. Behaviour is undefined if `y` is
negative, or if `T` is signed and the true result would be out of range.

* `template <typename T> T` **`int_sqrt`**`(T t) noexcept`

Returns the integer square root of the argument (the true square root
truncated to an integer). Behaviour is undefined if the argument is negative.

### Bitwise operations ###

For all functions in this section that are templated on the argument type, the
function will fail to compile if `T` is not an integer, and behaviour is
undefined if `T` is signed and the argument is negative.

* `template <typename T> constexpr int` **`ibits`**`(T t) noexcept`

Returns the number of 1 bits in the argument.

* `template <typename T> constexpr T` **`ifloor2`**`(T t) noexcept`
* `template <typename T> constexpr T` **`iceil2`**`(T t) noexcept`

Return the argument rounded down or up to a power of 2. For `iceil2()`,
behaviour is undefined if the argument is large enough that the correct answer
is not representable.

* `template <typename T> constexpr int` **`ilog2p1`**`(T t) noexcept`

Returns `floor(log2(t))+1`, equal to the number of significant bits in `t`,
or zero if `t` is zero.

* `template <typename T> constexpr bool` **`ispow2`**`(T t) noexcept`

True if the argument is an exact power of 2.

* `constexpr uint64_t` **`letter_to_mask`**`(char c) noexcept`

Converts a letter to a mask with bit 0-51 set (corresponding to `[A-Za-z]`.
Returns zero if the argument is not an ASCII letter.

* `template <typename T> constexpr T` **`rotl`**`(T t, int n) noexcept`
* `template <typename T> constexpr T` **`rotr`**`(T t, int n) noexcept`

Bitwise rotate left or right. The bit count is reduced modulo the number of
bits in `T`; a negative shift in one direction is treated as a positive shift
in the other.

## Functional utilities ##

### Function traits ###

* `template <typename Function> struct` **`Arity`**
    * `static constexpr size_t Arity::`**`value`**
* `template <typename Function> using` **`ArgumentTuple`** `= [tuple type]`
* `template <typename Function, size_t Index> using` **`ArgumentType`** `= [type of given argument]`
* `template <typename Function> using` **`ReturnType`** `= [return type]`
* `template <typename Function> using` **`FunctionSignature`** `= [result(args...)]`
* `template <typename Function> using` **`StdFunction`** `= function<FunctionSignature<Function>>`

Properties of a function type. This will work with ordinary functions (if not
overloaded), function pointers, function objects, lambda expressions, and
`std::function` instantiations.

### Function operations ###

* `template <typename Function> StdFunction<Function>` **`stdfun`**`(Function& f)`

Wraps a function in the appropriate standard function type.

* `template<typename Function, typename Tuple> decltype(auto)` **`tuple_invoke`**`(Function&& f, Tuple&& t)`

Calls a function, passing a tuple as the argument list.

### Generic function objects ###

* `struct` **`DoNothing`**
    * `void` **`operator()`**`() const noexcept {}`
    * `template <typename T> void` **`operator()`**`(T&) const noexcept {}`
    * `template <typename T> void` **`operator()`**`(const T&) const noexcept {}`
* `constexpr DoNothing` **`do_nothing`**
* `struct` **`Identity`**
    * `template <typename T> T&` **`operator()`**`(T& t) const noexcept { return t; }`
    * `template <typename T> const T&` **`operator()`**`(const T& t) const noexcept { return t; }`
* `constexpr Identity` **`identity`**

Trivial function objects.

### Hash functions ###

* `class` **`Djb2a`**
    * `Djb2a&` **`Djb2a`**`::operator()(const void* ptr, size_t n) noexcept`
    * `Djb2a&` **`Djb2a`**`::operator()(const string& s) noexcept`
    * `Djb2a::`**`operator uint32_t`**`() const noexcept`
* `uint32_t` **`djb2a`**`(const void* ptr, size_t n) noexcept`

A simple, efficient hash algorithm for a string of bytes.

* `size_t` **`hash_mix`**`(size_t h1, size_t h2) noexcept`

Combine two hash values.

* `template <typename... Args> size_t` **`hash_value`**`(const Args&... args) noexcept`
* `template <typename... Args> void` **`hash_combine`**`(size_t& hash, const Args&... args) noexcept`
* `template <typename InputRange> size_t` **`hash_range`**`(const InputRange& range) noexcept`
* `template <typename InputRange> void` **`hash_range`**`(size_t& hash, const InputRange& range) noexcept`

Functions for combining hashes incrementally, or for generating the hash of a
number of objects in one call, for use in implementing hash functions for
compound types. The first version of each pair of functions (`hash_value()`
and `hash_combine()` have different names to avoid overload resolution
problems) calculates the hash of the supplied data; the second version takes
an existing hash value and mixes it with additional data. All of these call
the element type's `std::hash`.

* `template <typename... Args> struct` **`TupleHash`**
    * `size_t` **`operator()`**`(const tuple<Args...>& t) const`

Hash function for a tuple.

### Scope guards ###

* `template <typename T, int Def = 0> class` **`Resource`**
    * `using Resource::`**`delete_function`** `= std::function<void(T&)>`
    * `using Resource::`**`resource_type`** `= T`
    * `using Resource::`**`value_type`** `= [dereferenced T]` _[only if T is a pointer]_
    * `Resource::`**`Resource`**`() noexcept`
    * `explicit Resource::`**`Resource`**`(T t) noexcept`
    * `template <typename Del> Resource::`**`Resource`**`(T t, Del d)`
    * `Resource::`**`Resource`**`(Resource&& r) noexcept`
    * `Resource::`**`~Resource`**`() noexcept`
    * `Resource& Resource::`**`operator=`**`(Resource&& r) noexcept`
    * `explicit Resource::`**`operator bool`**`() const noexcept`
    * `value_type& Resource::`**`operator*`**`() noexcept` _[only if T is a non-void pointer]_
    * `const value_type& Resource::`**`operator*`**`() const noexcept` _[only if T is a non-void pointer]_
    * `T Resource::`**`operator->`**`() const noexcept` _[only if T is a non-void pointer]_
    * `T& Resource::`**`get`**`() noexcept`
    * `T Resource::`**`get`**`() const noexcept`
    * `T Resource::`**`release`**`() noexcept`
    * `void Resource::`**`reset`**`() noexcept`
    * `void Resource::`**`reset`**`(T t) noexcept`
    * `static T Resource::`**`def`**`() noexcept`
* `template <typename T, typename Del> Resource<T>` **`make_resource`**`(T t, Del d)`

This holds a resource of some kind, a deleter function that will be called on
destruction (similar to a `unique_ptr`), and optionally a default value (only
usable if `T` is constructible from an `int`). `T` is assumed to be copyable
and movable without throwing; normally it will be a primitive object such as a
pointer or integer.

The deleter function passed to the constructor is expected to take a single
argument of type `T`; it defaults to a null function. The destructor will call
`d(t)`, unless the resource value is equal to the default or the deleter is
null. The constructor will call `d(t)` if anything goes wrong, with the same
constraints (this can only happen if copying `D` throws).

The `def()` function returns the default value of `T`, which is `T(Def)` if
`T` is constructible from an `int`, otherwise `T()`. The boolean conversion
operator returns true if the stored value is not equal to the default (note
that this may give a different result from `bool(T)`).

* `class` **`[scope guard]`**
    * `[scope guard]::`**`[scope guard]`**`(F&& f)`
    * `[scope guard]::`**`[scope guard]`**`([scope guard]&&) noexcept`
    * `[scope guard]::`**`~[scope guard]`**`() noexcept`
    * `void [scope guard]::`**`release`**`() noexcept`
* `template <typename F> inline [scope guard]` **`scope_exit`**`(F&& f)`
* `template <typename F> inline [scope guard]` **`scope_fail`**`(F&& f)`
* `template <typename F> inline [scope guard]` **`scope_success`**`(F&& f)`

The anonymous scope guard class stores a function object, to be called when
the guard is destroyed. The three functions create scope guards with different
execution conditions.

The `scope_exit()` guard calls the function unconditionally; `scope_success()`
calls it only on normal exit (not when unwinding due to an exception);
`scope_fail()` calls it only when an exception causes stack unwinding (not on
normal exit). If the creation function throws an exception (this is only
possible if the function object's copy or move constructor or assignment
operator throws), `scope_exit()` and `scope_fail()` will call the function
before propagating the exception, while `scope_success()` will not. Any
exceptions thrown by the function call in the scope guard's destructor are
silently ignored (normally the function should be written so as not to throw
anything).

The `release()` function discards the saved function; after it is called, the
scope guard object will do nothing on destruction.

* `class` **`ScopedTransaction`**
    * `using ScopedTransaction::`**`callback`** `= function<void()>`
    * `ScopedTransaction::`**`ScopedTransaction`**`() noexcept`
    * `ScopedTransaction::`**`~ScopedTransaction`**`() noexcept`
    * `void ScopedTransaction::`**`operator()`**`(callback func, callback undo)`
    * `void ScopedTransaction::`**`commit`**`() noexcept`
    * `void ScopedTransaction::`**`rollback`**`() noexcept`

This holds a stack of "undo" operations, to be carried out if anything goes
wrong during a sequence of operations. The function call operator accepts two
function objects; `func()` is called immediately, while `undo()` is saved on
the stack. If `func()` throws an exception, its `undo()` is not called, but
any pre-existing undo stack is invoked. Either function can be a null pointer
if no action is required.

Calling `commit()` discards the saved undo functions; `rollback()` calls them
in reverse order of insertion, silently ignoring any exceptions (normally the
undo functions should be written so as not to throw anything). The destructor
will perform a rollback if neither `commit()` nor `rollback()` have been
called since the last new call.

A single `ScopedTransaction` object can be used for multiple transactions.
Once `commit()` or `rollback()` is called, the undo stack is discarded, and
any newly added function pairs become part of a new cycle, equivalent to a
newly constructed `ScopedTransaction`.

* `template <typename T> std::unique_lock<T>` **`make_lock`**`(T& t)`

Simple wrapper function to create a mutex lock.

## I/O utilities ##

### Logging ###

* `void` **`logx`**`(const Ustring& msg) noexcept`
* `void` **`logx`**`(const char* msg) noexcept`
* `template <typename... Args> void` **`logx`**`(Args... args) noexcept`

These write a message to standard output, followed by a line feed and an
output flush. If multiple arguments are supplied, they are delimited with a
space.

This is intended for use in multithreaded code that needs coherent logging
output. A private mutex is used to ensure that messages from different threads
are not interleaved. Output from different threads will be given different
colours, chosen at random based on a hash of the thread ID. Because these are
intended only for debugging the code around them, any exceptions thrown by
their internal workings are silently ignored.
