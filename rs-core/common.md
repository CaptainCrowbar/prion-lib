# Common Utilities #

By Ross Smith

* `#include "rs-core/common.hpp"`

## Contents ##

[TOC]

## Introduction ##

The `"unicorn/utility.hpp"` header from the Unicorn library is included in
this header. For convenient reference, all of the relevant contents of that
header are documented again here, marked with `[unicorn]`.

## Preprocessor macros ##

### Compilation environment properties ###

* `#define` **`RS_NATIVE_WCHAR`** `1` _- defined if the system API uses wide characters_
* `#define` **`RS_WCHAR_UTF16`** `1` _- defined if wchar_t and wstring are UTF-16_
* `#define` **`RS_WCHAR_UTF32`** `1` _- defined if wchar_t and wstring are UTF-32_

`[unicorn]` These are defined to reflect the encoding represented by `wchar_t`
and `std::wstring`. Systems where wide strings are neither UTF-16 nor UTF-32
are not supported.

### Type generation macros ###

* `#define` **`RS_BITMASK_OPERATORS`**`(EnumType)`

`[unicorn]` Defines bit manipulation and related operators for an `enum class`
(unary `!`, `~`; binary `&`, `&=`, `|`, `|=`, `^`, `^=`). The type can be
defined the conventional way or through the `RS_ENUM_CLASS()` macro.

* `#define` **`RS_ENUM`**`(EnumType, IntType, first_value, first_name, ...)`
* `#define` **`RS_ENUM_CLASS`**`(EnumType, IntType, first_value, first_name, ...)`

`[unicorn]` These define an enumeration, given the name of the enumeration
type, the underlying integer type, the integer value of the first entry, and a
list of value names. They will also define the following functions:

* `constexpr bool` **`enum_is_valid`**`(EnumType t) noexcept`
* `std::vector<EnumType>` **`enum_values<EnumType>`**`()`
* `bool` **`str_to_enum`**`(std::string_view s, EnumType& t) noexcept`
* `std::string` **`to_str`**`(EnumType t)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, EnumType t)`

The `enum_is_valid()` function reports whether or not the argument is a named
value of the enumeration. The `enum_values<T>()` function returns a vector
containing all of the enumeration values. The `str_to_enum()` function
converts the name of an enumeration value (case sensitive; possibly qualified
with the class name) to the corresponding value; if the string does not match
any value, it leaves the reference argument unchanged and returns false. The
`to_str()` function and the output operator print the name of an enumeration
constant (qualified with the class name if this is an `enum class`), or the
integer value if the argument is not a named value.

Example:

    RS_ENUM(Foo, 1, alpha, bravo, charlie)
    RS_ENUM_CLASS(Bar, 1, delta, echo, foxtrot)

Equivalent code:

    enum Foo { alpha = 1, bravo, charlie };
    constexpr bool enum_is_valid(Foo t) noexcept { ... }
    std::string to_str(Foo t) { ... }
    std::ostream& operator<<(std::ostream& out, Foo t) { ... }

    enum class Bar { delta = 1, echo, foxtrot };
    constexpr bool enum_is_valid(Bar t) noexcept { ... }
    std::string to_str(Bar t) { ... }
    std::ostream& operator<<(std::ostream& out, Bar t) { ... }

The macros can be used in any namespace, and the functions that take an enum
value as an argument will be in that namespace, but `enum_values()` is a
single function template in `namespace RS`.

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
* `#define` **`RS_NO_INSTANCE`**`(T)`
    * `T() = delete;`
    * `T(const T&) = delete;`
    * `T(T&&) = delete;`
    * `~T() = delete;`
    * `T& operator=(const T&) = delete;`
    * `T& operator=(T&&) = delete;`

`[unicorn]` Convenience macros for defaulted or deleted life cycle operations.

* `#define` **`RS_OVERLOAD`**`(f) [] (auto&&... args) { return f(std::forward<decltype(args)>(args)...); }`

`[unicorn]` Creates a function object wrapping a set of overloaded functions,
that can be passed to a context expecting a function (such as an STL
algorithm) without having to explicitly resolve the overload at the call site.
(From an idea by Arthur O'Dwyer on the C++ standard proposals mailing list, 14
Sep 2015.)

## Types ##

### Basic types ###

* `using` **`Ustring`** `= std::string`
* `using` **`Uview`** `= std::string_view`

`[unicorn]` Use `Ustring` or `Uview` for strings that are expected to be in
UTF-8 (or ASCII, since any ASCII string is also valid UTF-8), while plain
`std::string` or `std::string_view` is used where the string is expected to be
in some other Unicode encoding, or where the string is being used simply as an
array of bytes rather than encoded text.

* `using` **`Strings`** `= std::vector<std::string>`

`[unicorn]` Commonly used type defined for convenience.

* `using` **`NativeCharacter`** `= [char on Unix, wchar_t on Windows]`
* `using` **`NativeString`** `= [std::string on Unix, std::wstring on Windows]`

`[unicorn]` These are defined to reflect the character types used in the
operating system's native API.

* `using` **`WcharEquivalent`** `= [char16_t or char32_t]`
* `using` **`WstringEquivalent`** `= [std::u16string or std::u32string]`

`[unicorn]` These are defined to reflect the encoding represented by `wchar_t`
and `std::wstring`. Systems where wide strings are neither UTF-16 nor UTF-32
are not supported.

* `template <auto> class` **`IncompleteTemplate`**
* `class` **`IncompleteType`**
* `template <auto> class` **`CompleteTemplate`**
* `class` **`CompleteType`**

`[unicorn]` Dummy types used in metaprogramming. The `Complete` types are
complete but cannot be instantiated.

### Integer types ###

* `enum` **`ByteOrder`**
    * **`big_endian`**
    * **`little_endian`**
    * **`native_endian`** `= [big_endian or little_endian]`
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

### Exceptions ###

* `void` **`rethrow`**`(std::exception_ptr p)`

Rethrows the exception, if one is present. This is the same as
`std::rethrow_exception()`, except that it does nothing if the exception
pointer is null.

### Mixins ###

<!-- DEFN --> `[unicorn]` These are convenience base classes that define
members and operators that would normally just be repetitive boilerplate
(similar to the ones in Boost). They all use the CRTP idiom; a class `T`
should derive from `Mixin<T>` to automatically generate the desired
boilerplate code. The table below shows which operations the user is required
to define, and which ones the mixin will automatically define. (Here, `t` and
`u` are objects of type `T`, `v` is an object of `T`'s value type, and `n` is
an integer.)

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

* `template <typename T> class` **`NumericLimitsBase`**
    * _Universal properties_
        * `static constexpr bool is_specialized = true                                   // Always true`
        * `static constexpr bool is_bounded = false                                      // Finite set of values`
        * `static constexpr bool is_exact = false                                        // Exact representation`
        * `static constexpr bool is_integer = false                                      // Integer type`
        * `static constexpr bool is_modulo = false                                       // Modulo arithmetic`
        * `static constexpr bool is_signed = false                                       // Signed type`
        * `static constexpr bool traps = false                                           // Trap values exist`
        * `static constexpr int radix = 0                                                // Basis of representation`
    * _Bounded type properties_
        * `static constexpr int digits = 0                                               // Radix digits in significand`
        * `static constexpr int digits10 = 0                                             // Decimals represented without loss`
        * `static constexpr T lowest() noexcept { return T(); }                          // Minimum finite value`
        * `static constexpr T max() noexcept { return T(); }                             // Maximum finite value`
        * `static constexpr T min() noexcept { return T(); }                             // Minimum finite integer or positive float`
    * _Floating point properties_
        * `static constexpr bool has_denorm_loss = false                                 // Loss of accuracy as denormalization`
        * `static constexpr bool has_infinity = false                                    // Has positive infinity`
        * `static constexpr bool has_quiet_NaN = false                                   // Has quiet NaN`
        * `static constexpr bool has_signaling_NaN = false                               // Has signalling NaN`
        * `static constexpr bool is_iec559 = false                                       // IEC 559 standard`
        * `static constexpr bool tinyness_before = false                                 // Tinyness is detected before rounding`
        * `static constexpr int max_digits10 = 0                                         // Decimals required for different values`
        * `static constexpr int max_exponent = 0                                         // Maximum exponent`
        * `static constexpr int max_exponent10 = 0                                       // Maximum decimal exponent`
        * `static constexpr int min_exponent = 0                                         // Minimum exponent`
        * `static constexpr int min_exponent10 = 0                                       // Minimum decimal exponent`
        * `static constexpr std::float_denorm_style has_denorm = std::denorm_absent      // Has denormalized values`
        * `static constexpr std::float_round_style round_style = std::round_toward_zero  // Rounding style`
        * `static constexpr T denorm_min() noexcept { return T(); }                      // Minimum positive subnormal value`
        * `static constexpr T epsilon() noexcept { return T(); }                         // Difference between 1 and next value`
        * `static constexpr T infinity() noexcept { return T(); }                        // Positive infinity`
        * `static constexpr T quiet_NaN() noexcept { return T(); }                       // Quiet NaN`
        * `static constexpr T round_error() noexcept { return T(); }                     // Maximum rounding error`
        * `static constexpr T signaling_NaN() noexcept { return T(); }                   // Signalling NaN`
    * _Helper functions_
        * `protected static constexpr int digits2_to_10(size_t bits) noexcept            // Convert bit count to decimal digits`

A convenience class to help define specializations of `std::numeric_limits`
for new arithmetic types.

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

`[unicorn]` A wrapper for a pair of iterators, usable as a range in standard
algorithms.

* `template <typename InputRange> size_t` **`range_count`**`(const InputRange& r)`
* `template <typename InputRange> bool` **`range_empty`**`(const InputRange& r)`

`[unicorn]` Return the length of a range. The `range_count()` function is just
shorthand for `std::distance(begin(r),end(r))`, and `range_empty()` has the
obvious meaning.

### Smart pointers ###

* `template <typename T> class` **`CopyPtr`**
    * `using CopyPtr::`**`element_type`** `= T`
    * `CopyPtr::`**`CopyPtr`**`() noexcept`
    * `CopyPtr::`**`CopyPtr`**`(T* p) noexcept`
    * `CopyPtr::`**`CopyPtr`**`(std::nullptr_t) noexcept`
    * `CopyPtr::`**`CopyPtr`**`(const CopyPtr& cp)`
    * `CopyPtr::`**`CopyPtr`**`(CopyPtr&& cp) noexcept`
    * `CopyPtr::`**`~CopyPtr`**`() noexcept`
    * `CopyPtr& CopyPtr::`**`operator=`**`(T* p) noexcept`
    * `CopyPtr& CopyPtr::`**`operator=`**`(std::nullptr_t) noexcept`
    * `CopyPtr& CopyPtr::`**`operator=`**`(const CopyPtr& cp)`
    * `CopyPtr& CopyPtr::`**`operator=`**`(CopyPtr&& cp) noexcept`
    * `T& CopyPtr::`**`operator*`**`() const noexcept`
    * `T* CopyPtr::`**`operator->`**`() const noexcept`
    * `explicit CopyPtr::`**`operator bool`**`() const noexcept`
    * `T* CopyPtr::`**`get`**`() const noexcept`
    * `T* CopyPtr::`**`release`**`() noexcept`
    * `void CopyPtr::`**`reset`**`(T* p = nullptr) noexcept`
* `void` **`swap`**`(CopyPtr<T>& a, CopyPtr<T>& b) noexcept`
* `template <typename T, typename... Args> CopyPtr<T>` **`make_copy_ptr`**`(Args&&... args)`
* `template <typename T> class` **`ClonePtr`**
    * `using ClonePtr::`**`element_type`** `= T`
    * `ClonePtr::`**`ClonePtr`**`() noexcept`
    * `ClonePtr::`**`ClonePtr`**`(T* p) noexcept`
    * `ClonePtr::`**`ClonePtr`**`(std::nullptr_t) noexcept`
    * `ClonePtr::`**`ClonePtr`**`(const ClonePtr& cp)`
    * `ClonePtr::`**`ClonePtr`**`(ClonePtr&& cp) noexcept`
    * `ClonePtr::`**`~ClonePtr`**`() noexcept`
    * `ClonePtr& ClonePtr::`**`operator=`**`(T* p) noexcept`
    * `ClonePtr& ClonePtr::`**`operator=`**`(std::nullptr_t) noexcept`
    * `ClonePtr& ClonePtr::`**`operator=`**`(const ClonePtr& cp)`
    * `ClonePtr& ClonePtr::`**`operator=`**`(ClonePtr&& cp) noexcept`
    * `T& ClonePtr::`**`operator*`**`() const noexcept`
    * `T* ClonePtr::`**`operator->`**`() const noexcept`
    * `explicit ClonePtr::`**`operator bool`**`() const noexcept`
    * `T* ClonePtr::`**`get`**`() const noexcept`
    * `T* ClonePtr::`**`release`**`() noexcept`
    * `void ClonePtr::`**`reset`**`(T* p = nullptr) noexcept`
* `void` **`swap`**`(ClonePtr<T>& a, ClonePtr<T>& b) noexcept`
* `template <typename T, typename... Args> ClonePtr<T>` **`make_clone_ptr`**`(Args&&... args)`

Pointers that automatically copy their target object when the pointer is
copied, if the pointer is not null. `CopyPtr` calls `T`'s copy constructor,
while `ClonePtr` expects `T` to have a `clone()` member function that returns
a raw pointer to a newly created object. Moving a `CopyPtr` or `ClonePtr`
always leaves the moved-from pointer null.

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

### Type names ###

* `std::string` **`demangle`**`(const std::string& name)`
* `std::string` **`type_name`**`(const std::type_info& t)`
* `std::string` **`type_name`**`(const std::type_index& t)`
* `template <typename T> std::string` **`type_name`**`()`
* `template <typename T> std::string` **`type_name`**`(const T& t)`

`[unicorn]` Demangle a type name. The original mangled name can be supplied as
an explicit string, as a `std::type_info` or `std:type_index` object, as a
type argument to a template function (e.g. `type_name<int>()`), or as an
object whose type is to be named (e.g. `type_name(42)`). The last version will
report the dynamic type of the referenced object.

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

### Type traits ###

* `template <typename T> using` **`BinaryType`** `= [unsigned integer type]`

Yields an unsigned integer type the same size as `T`. This will fail to
compile if no such type exists.

* `template <typename T1, typename T2> using` **`CopyConst`** `= ...`

Yields a type created by transferring the `const` qualification (or lack of
it) from `T1` to the unqualified type of `T2`. For example, `CopyConst<int,
const string>` yields `string`, while `CopyConst<const int, string>` yields
`const string`.

* `template <typename... TS> using` **`LargerType`** `= [largest type]`
* `template <typename... TS> using` **`SmallerType`** `= [smallest type]`

The largest or smallest type (by `sizeof`) in the argument list. In the event
of a tie, the first qualifying type in the list is used.

* `template <size_t Bits> using` **`SignedInteger`** `= [signed integer type]`
* `template <size_t Bits> using` **`UnsignedInteger`** `= [unsigned integer type]`

Signed and unsigned integer types with the specified number of bits (the same
types as `int8_t`, `int16_t`, etc). These will fail to compile if `Bits` is
not a power of 2 in the supported range (8-64).

## Constants and literals ##

### General constants ###

* `constexpr const char*` **`ascii_whitespace`** `= "\t\n\v\f\r "`

`[unicorn]` ASCII whitespace characters.

* `constexpr bool` **`big_endian_target`**
* `constexpr bool` **`little_endian_target`**

`[unicorn]` One of these will be true and the other false, reflecting the
target system's byte order.

* `constexpr size_t` **`npos`** `= std::string::npos`

`[unicorn]` Defined for convenience. Following the conventions established by
the standard library, this value is often used as a function argument to mean
"as large as possible" or "no limit", or as a return value to mean "not
found".

* `template <typename T> constexpr bool` **`dependent_false`** ` = false`

`[unicorn]` Conventional workaround for an always-false `static_assert()` in
the final `else` clause of an `if constexpr` statement.

### Arithmetic constants ###

* `constexpr [unsigned integer]` **`KB`** `= 1024`
* `constexpr [unsigned integer]` **`MB`** `= 1 048 576`
* `constexpr [unsigned integer]` **`GB`** `= 1 073 741 824`
* `constexpr [unsigned integer]` **`TB`** `= 1 099 511 627 776`

Powers of 2<sup>10</sup>.

* `template <int N> constexpr [unsigned integer]` **`setbit`** `= 1 << N`

`[unicorn]` An integer constant with bit `N` set. The return type is the
smallest unsigned integer (not counting `bool`) that will hold the value.
Behaviour is undefined if `N<0` or `N>63`.

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

## Algorithms and ranges ##

### Generic algorithms ###

* `template <typename Iterator> void` **`advance_by`**`(Iterator& i, ptrdiff_t n, Iterator end)`

Advances an iterator by the given number of steps, or until it reaches `end`,
whichever comes first. This will take `O(1)` time if the iterator is random
access, otherwise `O(n)`.

* `template <typename Container> [output iterator]` **`append`**`(Container& con)`
* `template <typename Container> [output iterator]` **`overwrite`**`(Container& con)`
* `template <typename Range, typename Container> const Range&` **`operator>>`**`(const Range& lhs, [append iterator] rhs)`

`[unicorn]` The `append()` and `overwrite()` functions create output iterators
that will append elements to a standard container (see `append_to()` below).
The `append()` function is similar to `std::back_inserter()` (but supports
containers without `push_back()`), while `overwrite()` will first clear the
container and then return the append iterator. There is also an operator that
can be used to copy any range into a container (e.g. `range >> append(con)`).

* `template <typename Container, typename T> void` **`append_to`**`(Container& con, const T& t)`

`[unicorn]` Appends an item to a container; used by `append()` and
`overwrite()`. The generic version calls `con.insert(con.end(), t)`; overloads
(found by argument dependent lookup) can be used for container-like types that
do not have a suitable `insert()` method.

* `template <typename T> constexpr Irange<T*>` **`array_range`**`(T* ptr, size_t len)`

Returns `irange(ptr,ptr+len)`.

* `template <typename T, size_t N> constexpr std::tuple<T,...>` **`array_to_tuple`**`(const T (&array)[N])`
* `template <typename T, size_t N> constexpr std::tuple<T,...>` **`array_to_tuple`**`(const std::array<T, N> &array)`
* `template <typename... TS> std::array<CT, N>` **`tuple_to_array`**`(const std::tuple<TS...>& t)`
* `template <typename T1, typename T2> std::array<CT, 2>` **`tuple_to_array`**`(const std::pair<T1, T2>& t)`

The `array_to_tuple()` functions convert a C-style array or a `std::array` to
a tuple containing the same values. The `tuple_to_array()` functions convert a
tuple or pair to an array containing the same values. The element type of the
resulting array is the common type of the tuple elements; `tuple_to_array()`
will fail to compile if this type does not exist.

* `template <typename Range> [value type]` **`at_index`**`(const Range& r, size_t index)`
* `template <typename Range, typename T> [value type]` **`at_index`**`(const Range& r, size_t index, const T& def)`

Safe array access, returning the element at the given index, or the default
value (implicitly converted to the range's value type) if the index is out of
bounds. If no default value is supplied, a default constructed object of the
value type is returned. The array type can be any range with random access
iterators.

* `template <typename Range1, typename Range2> int` **`compare_3way`**`(const Range1& r1, const Range2& r2)`
* `template <typename Range1, typename Range2, typename Compare> int` **`compare_3way`**`(const Range1& r1, const Range2& r2, Compare cmp)`

`[unicorn]` Compare two ranges, returning -1 if the first range is less than
the second, zero if they are equal, and +1 if the first range is greater.

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
* `template <typename Container, typename T> void` **`con_trim`**`(Container& con, const T& t)`
* `template <typename Container, typename Predicate> void` **`con_trim_if`**`(Container& con, Predicate p)`
* `template <typename Container, typename T> void` **`con_trim_left`**`(Container& con, const T& t)`
* `template <typename Container, typename Predicate> void` **`con_trim_left_if`**`(Container& con, Predicate p)`
* `template <typename Container, typename T> void` **`con_trim_right`**`(Container& con, const T& t)`
* `template <typename Container, typename Predicate> void` **`con_trim_right_if`**`(Container& con, Predicate p)`

These carry out the same algorithms as the similarly named STL functions,
except that unwanted elements are removed from the container rather than
shuffled to the end. The `con_sort_unique()` functions perform a sort followed
by removing equivalent elements from the container; like `std::sort()`, its
predicate has less-than semantics (whereas that of `con_unique()`, like that
of `std::unique()`, has equality semantics). The `con_trim_*()` functions
remove matching elements from either or both ends of the container.

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

* `template <typename M, typename K> const M::mapped_type&` **`find_in_map`**`(const M& map, const K& key)`
* `template <typename M, typename K> const M::mapped_type&` **`find_in_map`**`(const M& map, const K& key, const M::mapped_type& def)`

Find a key in a map, returning the corresponding value, or the supplied
default value (or a default constructed `mapped_type`) if the key is not
found.

* `template <typename M, typename K> M::iterator` **`find_key_range`**`(M& map, const K& key)`
* `template <typename M, typename K> M::const_iterator` **`find_key_range`**`(const M& map, const K& key)`

Find the map entry matching a key, assuming the key represents the minimum
value of a range. This will return the exact matching key if there is one,
otherwise it will return the last key that is less than the target key, or
`map.end()` if there is no such key.

* `template <typename BidirectionalIterator, typename T> BidirectionalIterator` **`find_last`**`(BidirectionalIterator i, BidirectionalIterator j, const T& t)`
    * _Find the last element equal to the given value_
* `template <typename BidirectionalIterator, typename T> BidirectionalIterator` **`find_last_not`**`(BidirectionalIterator i, BidirectionalIterator j, const T& t)`
    * _Find the last element not equal to the given value_
* `template <typename BidirectionalIterator, typename UnaryPredicate> BidirectionalIterator` **`find_last_if`**`(BidirectionalIterator i, BidirectionalIterator j, UnaryPredicate p)`
    * _Find the last element that matches the predicate_
* `template <typename BidirectionalIterator, typename UnaryPredicate> BidirectionalIterator` **`find_last_if_not`**`(BidirectionalIterator i, BidirectionalIterator j, UnaryPredicate p)`
    * _Find the last element that does not match the predicate_
* `template <typename InputIterator, typename T> InputIterator` **`find_not`**`(InputIterator i, InputIterator j, const T& t)`
    * _Find the first element not equal to the given value_

Additional find algorithms. All of these return the end iterator if the search
fails.

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

* `template <typename T> T` **`read_be`**`(const void* src) noexcept`
* `template <typename T> void` **`read_be`**`(const void* src, T& dst) noexcept`
* `template <typename T> T` **`read_le`**`(const void* src) noexcept`
* `template <typename T> void` **`read_le`**`(const void* src, T& dst) noexcept`
* `template <typename T> void` **`write_be`**`(T src, void* dst) noexcept`
* `template <typename T> void` **`write_le`**`(T src, void* dst) noexcept`

Read or write integers to memory, in big or little endian order. The number of
bytes read or written is equal to `sizeof(T)`. It is up to the caller to
ensure that `T` is a type for which bytewise copying and the concept of byte
order make sense.

## Arithmetic functions ##

### Generic arithmetic functions ###

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

For the bit manipulation functions (`ifloor2()`, `iceil2()`, `ilog2p1()`,
`ispow2()`, `rotl()`, `popcount()`, and `rotr()`), behaviour is undefined if
`T` is not an integer, or if `T` is signed and the argument is negative.

* `template <typename T> constexpr std::make_signed_t<T>` **`as_signed`**`(T t) noexcept`
* `template <typename T> constexpr std::make_unsigned_t<T>` **`as_unsigned`**`(T t) noexcept`

`[unicorn]` These return their argument converted to a signed or unsigned
value of the same size (the argument is returned unchanged if `T` already had
the desired signedness). Behaviour is undefined if `T` is not an integer or
enumeration type.

* `template <typename T> constexpr T` **`binomial`**`(T a, T b) noexcept`
* `double` **`xbinomial`**`(int a, int b) noexcept`

These return the binomial coefficient (`a!/b!(a-b)!` if `0<=b<=a`, otherwise
zero). `T` must be an integer type. Behaviour is undefined if the correct
result would be out of range for the return type.

* `template <typename T> constexpr T` **`gcd`**`(T a, T b) noexcept`
* `template <typename T> constexpr T` **`lcm`**`(T a, T b) noexcept`

Return the greatest common divisor or lowest common multiple of two numbers.
These give the same results as `std::gcd/lcm()`, but without checking that `T`
is a primitive integral type, so these will work with user defined integer
types.

* `template <typename T> constexpr T` **`ifloor2`**`(T t) noexcept`
* `template <typename T> constexpr T` **`iceil2`**`(T t) noexcept`

`[unicorn]` Return the argument rounded down or up to a power of 2. For
`iceil2()`, behaviour is undefined if the argument is large enough that the
correct answer is not representable.

* `template <typename T> constexpr int` **`ilog2p1`**`(T t) noexcept`

`[unicorn]` Returns `floor(log2(t))+1`, equal to the number of significant
bits in `t`, or zero if `t` is zero.

* `template <typename T1, typename T2> constexpr T1` **`int_power`**`(T1 x, T2 y) noexcept`

Calculates <code>x<sup>y</sup></code> for integer types
(<code>0<sup>0</sup></code> will return 1). Behaviour is undefined if `y` is
negative, if the true result would be out of range for `T1`, or if either type
is not an integer.

* `template <typename T> constexpr T` **`int_sqrt`**`(T t) noexcept`

Returns the integer square root of the argument (the true square root
truncated to an integer). Behaviour is undefined if the argument is negative.

* `template <typename T> constexpr bool` **`ispow2`**`(T t) noexcept`

`[unicorn]` True if the argument is an exact power of 2.

* `constexpr uint64_t` **`letter_to_mask`**`(char c) noexcept`

`[unicorn]` Converts a letter to a mask with bit 0-51 set (corresponding to
`[A-Za-z]`). Returns zero if the argument is not an ASCII letter.

* `template <typename T> constexpr int` **`popcount`**`(T t) noexcept`

`[unicorn]` Returns the number of 1 bits in the argument.

* `template <typename T> constexpr T` **`rotl`**`(T t, int n) noexcept`
* `template <typename T> constexpr T` **`rotr`**`(T t, int n) noexcept`

`[unicorn]` Bitwise rotate left or right. The bit count is reduced modulo the
number of bits in `T`; a negative shift in one direction is treated as a
positive shift in the other.

## Date and time functions ##

### Constants ###

* `constexpr uint32_t` **`utc_zone`**
* `constexpr uint32_t` **`local_zone`**

`[unicorn]` Used to indicate whether a date is expressed in UTC or the local
time zone.

### Conversion functions ###

* `template <typename R, typename P> void` **`from_seconds`**`(double s, duration<R, P>& d) noexcept`
* `template <typename R, typename P> double` **`to_seconds`**`(const duration<R, P>& d) noexcept`

`[unicorn]` Convenience functions to convert between a `duration` and a
floating point number of seconds.

* `system_clock::time_point` **`make_date`**`(int year, int month, int day, int hour = 0, int min = 0, double sec = 0, uint32_t flags = utc_zone)`

`[unicorn]` Converts a broken down date into a time point. Behaviour if any of
the date arguments are invalid follows the same rules as `mktime()`. This will
throw `std::invalid_argument` if an invalid flag is passed.

### Formatting functions ###

* `Ustring` **`format_date`**`(system_clock::time_point tp, int prec = 0, uint32_t flags = utc_zone)`
* `Ustring` **`format_date`**`(system_clock::time_point tp, Uview format, uint32_t flags = utc_zone)`

`[unicorn]` These convert a time point into a broken down date and format it.
The first version writes the date in ISO 8601 format (`"yyyy-mm-dd
hh:mm:ss"`). If `prec` is greater than zero, the specified number of decimal
places will be added to the seconds field.

The second version writes the date using the conventions of `strftime()`. This
will return an empty string if anything goes wrong (there is no way to
distinguish between a conversion error and a legitimately empty result; this
is a limitation of `strftime()`).

Both of these will throw `std::invalid_argument` if an invalid flag is passed.

For reference, the portable subset of the `strftime()` formatting codes are:

| Code  | Description                          | Code    | Description                          |
| ----  | -----------                          | ----    | -----------                          |
|       **Date elements**                      | |       **Weekday elements**                   | |
| `%Y`  | Year number                          | `%a`    | Local weekday abbreviation           |
| `%y`  | Last 2 digits of the year (`00-99`)  | `%w`    | Sunday-based weekday number (`0-6`)  |
| `%m`  | Month number (`00-12`)               | `%A`    | Local weekday name                   |
| `%B`  | Local month name                     | `%U`    | Sunday-based week number (`00-53`)   |
| `%b`  | Local month abbreviation             | `%W`    | Monday-based week number (`00-53`)   |
| `%d`  | Day of the month (`01-31`)           |         **Other elements**                     | |
|       **Time of day elements**               | | `%c`  | Local standard date/time format      |
| `%H`  | Hour on 24-hour clock (`00-23`)      | `%x`    | Local standard date format           |
| `%I`  | Hour on 12-hour clock (`01-12`)      | `%X`    | Local standard time format           |
| `%p`  | Local equivalent of a.m./p.m.        | `%j`    | Day of the year (`001-366`)          |
| `%M`  | Minute (`00-59`)                     | `%Z`    | Time zone name                       |
| `%S`  | Second (`00-60`)                     | `%z`    | Time zone offset                     |

* `template <typename R, typename P> Ustring` **`format_time`**`(const duration<R, P>& time, int prec = 0)`

`[unicorn]` Formats a time duration in days, hours, minutes, seconds, and (if
`prec>0`) fractions of a second.

## Error handling ##

### Assertion functions ###

* `void` **`runtime_assert`**`(bool condition, std::string_view message) noexcept`

`[unicorn]` A portable, non-blockable version of `assert()`. If the condition
is false, this will print the message to standard error (followed by a line
break), and then call `abort()`.

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

* `template <typename Signature> class` **`CallRef`**
    * `using CallRef::`**`function_type`** `= std::function<Signature>`
    * `using CallRef::`**`signature_type`** `= Signature`
    * `CallRef::`**`CallRef`**`() noexcept`
    * `CallRef::`**`CallRef`**`(std::nullptr_t) noexcept`
    * `template <typename F> CallRef::`**`CallRef`**`(F f)`
    * `CallRef::`**`~CallRef`**`() noexcept`
    * `CallRef::`**`CallRef`**`(const CallRef& cr)`
    * `CallRef::`**`CallRef`**`(CallRef&& cr) noexcept`
    * `CallRef& CallRef::`**`operator=`**`(const CallRef& cr)`
    * `CallRef& CallRef::`**`operator=`**`(CallRef&& cr) noexcept`
    * `explicit CallRef::`**`operator bool`**`() const noexcept`
    * `template <typename... Args> [return type] CallRef::`**`operator()`**`(Args... args) const`

A function wrapper with reference semantics (a thin wrapper around
`std::shared_ptr<std::function<Signature>>`), intended to make it easier to
use heavyweight function objects where copying them around would impose too
much overhead. Like `std::function`, invoking a null `CallRef` will throw
`std::bad_function_call`.

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

* `struct` **`RangeEqual`**
    * `template <typename Range> bool RangeEqual::`**`operator()`**`(const Range& r1, const Range& r2) const`
* `struct` **`RangeCompare`**
    * `template <typename Range> bool RangeCompare::`**`operator()`**`(const Range& r1, const Range& r2) const`
* `constexpr RangeEqual` **`range_equal`**
* `constexpr RangeCompare` **`range_compare`**

Range comparison objects. These call `std::equal()` and
`std::lexicographical_compare()` respectively.

### Hash functions ###

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

* `template <typename T> size_t` **`std_hash`**`(const T& t) noexcept`

This simply calls `std::hash<T>::operator()` on its argument.

* `template <typename... Args> struct` **`TupleHash`**
    * `size_t` **`operator()`**`(const tuple<Args...>& t) const`

Hash function for a tuple.

### Scope guards ###

* `template <typename T> std::unique_lock<T>` **`make_lock`**`(T& t)`
* `template <typename T> std::shared_lock<T>` **`make_shared_lock`**`(T& t)`

`[unicorn]` Simple wrapper functions to create a mutex lock.

* `enum class` **`Scope`**
    * `Scope::`**`exit`**
    * `Scope::`**`fail`**
    * `Scope::`**`success`**
* `template <typename F, Scope S> class` **`BasicScopeGuard`**
    * `BasicScopeGuard::`**`BasicScopeGuard`**`() noexcept`
    * `BasicScopeGuard::`**`BasicScopeGuard`**`(F&& f)`
    * `BasicScopeGuard::`**`BasicScopeGuard`**`(BasicScopeGuard&& sg) noexcept`
    * `BasicScopeGuard::`**`~BasicScopeGuard`**`() noexcept`
    * `BasicScopeGuard& BasicScopeGuard::`**`operator=`**`(F&& f)`
    * `BasicScopeGuard& BasicScopeGuard::`**`operator=`**`(BasicScopeGuard&& sg) noexcept`
    * `void BasicScopeGuard::`**`release`**`() noexcept`
* `using` **`ScopeExit`** `= BasicScopeGuard<std::function<void()>, Scope::exit>`
* `using` **`ScopeFail`** `= BasicScopeGuard<std::function<void()>, Scope::fail>`
* `using` **`ScopeSuccess`** `= BasicScopeGuard<std::function<void()>, Scope::success>`
* `template <typename F> BasicScopeGuard<F, Scope::exit>` **`scope_exit`**`(F&& f)`
* `template <typename F> BasicScopeGuard<F, Scope::fail>` **`scope_fail`**`(F&& f)`
* `template <typename F> BasicScopeGuard<F, Scope::success>` **`scope_success`**`(F&& f)`

`[unicorn]` The scope guard class stores a function object, to be called when
the guard is destroyed. The three functions create scope guards with different
execution conditions. The three named specializations are conveniences for
when a scope guard needs to be stored as a movable object.

A **scope exit** guard calls the function unconditionally; **scope success**
calls it only on normal exit, but not when unwinding due to an exception;
**scope fail** calls it only when an exception causes stack unwinding, but not
on normal exit. If the constructor or creation function throws an exception
(this is only possible if the function object's move constructor or assignment
operator throws), **scope exit** and **scope fail** will call the function
before propagating the exception, while **scope success** will not. Any
exceptions thrown by the function call in the scope guard's destructor are
silently ignored (normally the function should be written so as not to throw
anything).

The `release()` function discards the saved function; after it is called, the
scope guard object will do nothing on destruction.

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

## Keyword arguments ##

### Keyword arguments ###

* `template <typename T, int ID = 0> struct` **`Kwarg`**
    * `constexpr ... Kwarg::`**`operator=`**`(const T& t) const noexcept`
* `template <typename T, int ID, typename... Args> constexpr bool` **`kwtest`**`(Kwarg<T, ID> key, Args... args)`
* `template <typename T, int ID, typename... Args> T` **`kwget`**`(Kwarg<T, ID> key, const T& def, Args... args)`

`[unicorn]` This provides a simple implementation of variadic keyword
arguments for C++ functions.

Define a `Kwarg<T[,ID]>` object for each keyword argument, where `T` is the
argument type. The `ID` parameter is only needed to distinguish between
keywords with the same argument type. Functions that will take keyword
arguments should be declared with a variadic argument pack, possibly preceded
by ordinary positional arguments.

When calling the function, the keyword arguments should be supplied in the
form `key=value`, where `key` is a `Kwarg` object, and `value` is the argument
value. The value type must be convertible to `T`. If `T` is `bool`, the
keyword alone can be passed as an argument, with the value defaulting to
`true`.

In the function body, call `kwget()` or `kwtest()` for each possible keyword
argument, with the corresponding `Kwarg` object as the key, a default value
(for `kwget()`), and the variadic arguments from the enclosing function. The
`kwget()` function returns the value attached to the keyword, or the default
value if the keyword was not found in the argument list; `kwtest()` returns
whether or not the keyword was present. If the same keyword appears more than
once in the actual argument list, the first one found will be returned.

Example:

    class Window {
    public:
        static constexpr Kwarg<int, 1> width = {};
        static constexpr Kwarg<int, 2> height = {};
        static constexpr Kwarg<std::string> title = {};
        static constexpr Kwarg<bool> visible = {};
        template <typename... Args> explicit Window(Args... args) {
            int win_width = kwget(width, 640, args...);
            int win_height = kwget(height, 480, args...);
            string title_text = kwget(title, "New Window"s, args...);
            bool is_visible = kwget(visible, false, args...);
            // ...
        }
    };

    Window app_window(Window::title="Hello World", Window::width=1000, Window::height=750, Window::visible);

## Multithreading ##

### Thread class ###

* `class` **`Thread`**`: public std::thread`
    * `Thread::`**`Thread`**`() noexcept`
    * `template <typename F, typename... Args> explicit Thread::`**`Thread`**`(F&& f, Args&&... args)`
    * `Thread::`**`Thread`**`(Thread&& t) noexcept`
    * `Thread::`**`~Thread`**`() noexcept`
    * `Thread& Thread::`**`operator=`**`(Thread&& t) noexcept`
    * `void Thread::`**`detach`**`()`
    * `void Thread::`**`join`**`()`
    * `id Thread::`**`get_id`**`() const noexcept`
    * `bool Thread::`**`joinable`**`() const noexcept`
    * `native_handle_type Thread::`**`native_handle`**`() noexcept`
    * `void Thread::`**`swap`**`(Thread& t) noexcept`

A simple wrapper for `std::thread`. This differs from `std::thread` only in
that it will automatically join on destruction, or when used on the LHS of an
assignment.

## String functions ##

### General string functions ###

* `template <typename C> basic_string<C>` **`cstr`**`(const C* ptr)`
* `template <typename C> basic_string<C>` **`cstr`**`(const C* ptr, size_t n)`

`[unicorn]` These construct a string from a pointer to a null-terminated
character sequence, or a pointer and a length. They differ from the
corresponding string constructors in that passing a null pointer will yield an
empty string, or a string of `n` null characters, instead of undefined
behaviour.

* `template <typename C> size_t` **`cstr_size`**`(const C* ptr)`

`[unicorn]` Returns the length of a null-terminated string (a generalized
version of `strlen()`). This will return zero if the pointer is null.

* `Ustring` **`dent`**`(size_t depth)`

`[unicorn]` Returns a string containing `4*depth` spaces, for indentation.

* `template <typename S> [string view]` **`make_view`**`(const S& s, size_t pos = 0, size_t len = npos) noexcept`

`[unicorn]` Returns a string view over the given string. The string argument
may be an instantiation of `std:basic_string` or `std::basic_string_view`, or
a pointer to a null terminated character array. The substring bounds are range
checked and clamped to the actual size of the string.

* `std::string` **`quote`**`(std::string_view str)`
* `Ustring` **`bquote`**`(std::string_view str)`

`[unicorn]` Return a quoted string; internal quotes, backslashes, and control
characters are escaped. The `quote()` function passes non-ASCII bytes through
unchanged, while `bquote()` escapes them.

* `Ustring` **`unqualify`**`(Uview str, Uview delims = ".:")`

`[unicorn]` Strips off any prefix ending in one of the delimiter characters
(e.g. `unqualify("foo::bar::zap()")` returns `"zap()"`). This will return the
original string unchanged if the delimiter string is empty or none of its
characters are found.

### Case conversion functions ###

* `constexpr char` **`ascii_tolower`**`(char c) noexcept`
* `constexpr char` **`ascii_toupper`**`(char c) noexcept`
* `std::string` **`ascii_lowercase`**`(std::string_view s)`
* `std::string` **`ascii_uppercase`**`(std::string_view s)`
* `std::string` **`ascii_titlecase`**`(std::string_view s)`
* `std::string` **`ascii_sentencecase`**`(std::string_view s)`

`[unicorn]` Simple ASCII-only case conversion functions. All non-ASCII
characters are left unchanged. The sentence case function capitalizes the
first letter of every sentence (delimited by a full stop or two consecutive
line breaks), leaving everything else alone.

### Character functions ###

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
* `constexpr bool` **`is_ascii`**`(char c) noexcept`

`[unicorn]` These are simple ASCII-only versions of the standard character
type functions. All of them will always return false for bytes outside the
ASCII range (0-127).

* `constexpr bool` **`ascii_isalnum_w`**`(char c) noexcept`
* `constexpr bool` **`ascii_isalpha_w`**`(char c) noexcept`
* `constexpr bool` **`ascii_ispunct_w`**`(char c) noexcept`

`[unicorn]` These behave the same as the corresponding functions without the
`"_w"` suffix, except that the underscore character is counted as a letter
instead of a punctuation mark. (The suffix is intended to suggest the `"\w"`
regex element, which does much the same thing.)

### String conversion functions ###

* `template <typename T> Ustring` **`bin`**`(T x, size_t digits = 8 * sizeof(T))`
* `template <typename T> Ustring` **`dec`**`(T x, size_t digits = 1)`
* `template <typename T> Ustring` **`hex`**`(T x, size_t digits = 2 * sizeof(T))`

`[unicorn]` Simple number formatting functions. These convert an integer to a
binary, decimal, or hexadecimal string, generating at least the specified
number of digits.

* `unsigned long long` **`binnum`**`(std::string_view str) noexcept`
* `long long` **`decnum`**`(std::string_view str) noexcept`
* `unsigned long long` **`hexnum`**`(std::string_view str) noexcept`
* `double` **`fpnum`**`(std::string_view str) noexcept`

`[unicorn]` The `binnum()`, `decnum()`, and `hexnum()` functions convert a
binary, decimal, or hexadecimal string to a number; `fpnum()` converts a
string to a floating point number. These will ignore any trailing characters
that are not part of a number, and will return zero if the string is empty or
does not contain a valid number. Results that are out of range will be clamped
to the nearest end of the return type's range, or for `fpnum()`, to positive
or negative infinity.

* `template <typename Range> Ustring` **`format_list`**`(const Range& r)`
* `template <typename Range> Ustring` **`format_list`**`(const Range& r, std::string_view prefix, std::string_view delimiter, std::string_view suffix)`
* `template <typename Range> Ustring` **`format_map`**`(const Range& r)`
* `template <typename Range> Ustring` **`format_map`**`(const Range& r, std::string_view prefix, std::string_view infix, std::string_view delimiter, std::string_view suffix)`

`[unicorn]` Format a range as a delimited list. The `format_list()` function
writes the elements in sequence, with `prefix` and `suffix` at the beginning
and end, and with a `delimiter` between each pair of elements; individual
elements are formatted using `to_str()` (see below). The `format_map()`
function expects the range's value type to be a pair (or something with
`first` and `second` members); the elements of each pair are separated with
the `infix` string, and the range is otherwise formatted in the same way as
`format_list()`. The default formats are based on JSON syntax:

<!-- TEXT -->
* `format_list(r) = format_list(r, "[", ",", "]")`
* `format_map(r) = format_map(r, "{", ":", ",", "}")`

* `template <typename T> Ustring` **`fp_format`**`(T t, char mode = 'g', int prec = 6)`
* `template <typename T> Ustring` **`opt_fp_format`**`(T t, char mode = 'g', int prec = 6)`

`[unicorn]` The `fp_format()` function performs simple floating point
formatting, by calling `snprintf()`. `T` must be an arithmetic type; it will
be converted to `long double` internally. The additional format `'Z/z'` is the
same as `'G/g'` except that trailing zeros are not stripped. The
`opt_fp_format()` function calls `fp_format()` for floating point types;
otherwise it calls `to_str(t)` and ignores the other two arguments. These will
throw `std::invalid_argument` if the mode is not one of `[EFGZefgz]` (and is
not ignored); they may throw `std::system_error` under implementation defined
circumstances.

* `Ustring` **`roman`**`(int n)`

`[unicorn]` Formats a number as a Roman numeral. Numbers greater than 1000
will be written with an arbitrarily long sequence of `"M"`. This will return
an empty string if the argument is less than 1.

* `int64_t` **`si_to_int`**`(Uview str)`
* `double` **`si_to_float`**`(Uview str)`

`[unicorn]` These parse a number from a string representation tagged with an
SI multiplier abbreviation (e.g. `"123k"`). For the integer version, only tags
representing positive powers of 1000 (starting with`"k"`) are recognised, and
are case insensitive. For the floating point version, all tags representing
powers of 100 are recognised (`"u"` is used for "micro"), and are case
sensitive, except that `"K"` is equivalent to `"k"`. For both versions, a
space is allowed between the number and the tag, and any additional text after
the number or tag is ignored. These will throw `std::invalid_argument` if the
string does not start with a valid number, or `std::range_error` if the result
is too big for the return type.

* `template <typename T> bool` **`from_str`**`(std::string_view view, T& t) noexcept`
* `template <typename T> T` **`from_str`**`(std::string_view view)`
* `template <typename T> std::string` **`to_str`**`(const T& t)`
* `template <typename T> struct` **`FromStr`**
    * `T FromStr::`**`operator()`**`(std::string_view s) const`
* `struct` **`ToStr`**
    * `template <typename T> std::string ToStr::`**`operator()`**`(const T& t) const`

`[unicorn]` Generic utility functions for converting arbitrary types to or
from a string. The conversion rules are described below; `to_str()` and the
first version of `from_str()` can also be overloaded for new types. The
`FromStr` and `ToStr` function objects call the corresponding functions and do
not need to be separately overloaded.

The first version of `from_str()` writes the converted object into its second
argument and returns true on success; otherwise, it returns false and leaves
the referenced object unchanged. The second version calls the first version,
throwing `invalid_argument` on failure, and returning the converted object on
success; for this version, `T` must be default constructible.

The `from_str()` functions follow these rules, using the first conversion rule
that matches the type:

* `static_cast` from `std::string_view` to `T`
* `static_cast` from `std::string` to `T`
* `static_cast` from `const char*` to `T`
* Read a `T` from a `std::istringstream` using `operator>>`
* Otherwise fail

The `to_str()` functions follow these rules, using the first conversion rule
that matches the type:

* If `T` is `bool`, return `"true"` or `"false"`
* If `T` is `char`, return a one-character string
* If `T` is `std::string` or `std::string_view`, simply copy the string
* If `T` is `[const] char*`, copy the string, or return an empty string if the pointer is null
* If `T` is `std::array<uint8_t,N>` or `std::vector<uint8_t>`, format each byte in hexadecimal
* If `T` is an integer type, call `std::to_string(t)`
* If `T` is a floating point type, call `fp_format(t)`
* Call `t.str()`, `to_string(t)`, or `std::to_string(t)`
* `static_cast` from `T` to `std::string`, `std::string_view`, or `const char*`
* If `T` is derived from `std::exception`, call `t.what()`
* If `T` is a `std::optional`, `std::shared_ptr`, or `std::unique_ptr`, return `to_str(*t)` or `"null"`
* If `T` is a `std::pair` or `std::tuple`, call `to_str()` on each element and return `"(e1,e2,...)"`
* If `T` is a range whose elements are pairs, call `to_str()` on each member of each pair and return `"{k1:v1,k2:v2,...}"`
* If `T` is a range whose elements are not pairs, call `to_str()` on each element and return `"[e1,e2,...]"`
* Write a `T` into a `std::ostringstream` using `operator<<`
* If all else fails, just return the demangled type name
