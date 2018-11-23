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

* `constexpr bool` **`enum_is_valid`**`(EnumType t) noexcept`
* `std::vector<EnumType>` **`enum_values<EnumType>`**`()`
* `bool` **`str_to_enum`**`(std::string_view s, EnumType& t) noexcept`
* `std::string` **`to_str`**`(EnumType t)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, EnumType t)`

The `enum_is_valid()` function reports whether or not the argument is a named
value of the enumeration. The `enum_values<T>()` function returns a vector
containing a list of the enumeration's values. The `str_to_enum()` function
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
* `#define` **`RS_NO_INSTANCE`**`(T)`
    * `T() = delete;`
    * `T(const T&) = delete;`
    * `T(T&&) = delete;`
    * `~T() = delete;`
    * `T& operator=(const T&) = delete;`
    * `T& operator=(T&&) = delete;`

Convenience macros for defaulted or deleted life cycle operations.

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

Demangle a type name. The original mangled name can be supplied as an explicit
string, as a `std::type_info` or `std:type_index` object, as a type argument
to a template function (e.g. `type_name<int>()`), or as an object whose type
is to be named (e.g. `type_name(42)`). The last version will report the
dynamic type of the referenced object.

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

### Arithmetic constants ###

* `constexpr unsigned` **`KB`** `= 1024`
* `constexpr unsigned long` **`MB`** `= 1 048 576`
* `constexpr unsigned long` **`GB`** `= 1 073 741 824`
* `constexpr unsigned long long` **`TB`** `= 1 099 511 627 776`

Powers of 2<sup>10</sup>.

## Algorithms and ranges ##

### Generic algorithms ###

* `template <typename Iterator> void` **`advance_by`**`(Iterator& i, ptrdiff_t n, Iterator end)`

Advances an iterator by the given number of steps, or until it reaches `end`,
whichever comes first. This will take `O(1)` time if the iterator is random
access, otherwise `O(n)`.

* `template <typename T> constexpr Irange<T*>` **`array_range`**`(T* ptr, size_t len)`

Returns `irange(ptr,ptr+len)`.

* `template <typename T, size_t N> constexpr std::tuple<T,...>` **`array_to_tuple`**`(const T (&array)[N])`
* `template <typename T, size_t N> constexpr std::tuple<T,...>` **`array_to_tuple`**`(const std::array<T, N> &array)`

Convert a C-style array or a `std::array` to a tuple containing the same
values.

* `template <typename Range> [value type]` **`at_index`**`(const Range& r, size_t index)`
* `template <typename Range, typename T> [value type]` **`at_index`**`(const Range& r, size_t index, const T& def)`

Safe array access, returning the element at the given index, or the default
value (implicitly converted to the range's value type) if the index is out of
bounds. If no default value is supplied, a default constructed object of the
value type is returned. The array type can be any range with random access
iterators.

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

* `template <typename T> T` **`binomial`**`(T a, T b) noexcept`
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

* `template <typename T> T` **`int_power`**`(T x, T y) noexcept`

Calculates <code>x<sup>y</sup></code> for integer types.
<code>0<sup>0</sup></code> will return 1. Behaviour is undefined if `y` is
negative, or if `T` is signed and the true result would be out of range.

* `template <typename T> T` **`int_sqrt`**`(T t) noexcept`

Returns the integer square root of the argument (the true square root
truncated to an integer). Behaviour is undefined if the argument is negative.

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
