# Variant Type #

By Ross Smith

* `#include "rs-core/variant.hpp"`

## Contents ##

[TOC]

## Exceptions ##

* `class` **`VariantError`**`: public std::runtime_error`
    * `VariantError::`**`VariantError`**`()`

Thrown when a variant is accessed through a type that does not match its
current branch.

## Class Variant ##

* `template <typename... TS> class` **`Variant`**

A variant that can take values of any of its branch types. Requirements on the
branch types are:

* The type list must not be empty.
* The same type must not appear more than once.
* If `Nil` is a branch type, it must be the first branch.
* All types must be default constructible and movable.

These requirements are checked by `static_assert`. If the variant's copy
constructor, copy assignment operator, hash function, output operator, or any
comparison operator is called, the corresponding operation must be defined for
all branch types. (All of these operations are defined for `Nil`.)

* `using Variant::`**`typelist`** `= Meta::Typelist<TS...>`
* `using Variant::`**`head`** `= Meta::Head<typelist>`

Member types.

* `static constexpr bool Variant::`**`is_nullable`**

True if the first type is `Nil`.

* `Variant::`**`Variant`**`()`
* `template <typename T> Variant::`**`Variant`**`(const T& t)`
* `template <typename T> Variant::`**`Variant`**`(T&& t) noexcept`
* `Variant::`**`~Variant`**`() noexcept`
* `Variant::`**`Variant`**`(const Variant& v)`
* `Variant::`**`Variant`**`(Variant&& v) noexcept`
* `Variant& Variant::`**`operator=`**`(const Variant& v)`
* `Variant& Variant::`**`operator=`**`(Variant&& v) noexcept`

Life cycle functions. The default constructor sets the variant to a default
constructed value of the first branch type. If the variant is nullable, the
source object of move construction or move assignment is reset to `Nil`;
otherwise, its current branch is left in its normal moved-from state.

* `explicit Variant::`**`operator bool`**`() const`

This calls the corresponding operator on the current branch; it will return
false if the current branch is `Nil`. This will fail to compile if any branch
type is not implicitly or explicitly convertible to `bool`.

* `template <typename T> T& Variant::`**`as`**`()`
* `template <typename T> const T& Variant::`**`as`**`() const`
* `template <int Index> [type at index]& Variant::`**`at`**`()`
* `template <int Index> const [type at index]& Variant::`**`at`**`() const`
* `template <typename T> T Variant::`**`get_as`**`(const T& def = {}) const`
* `template <int Index> [type at index] Variant::`**`get_at`**`(const [type at index]& def = {}) const`
* `template <typename T> T& Variant::`**`ref_as`**`() noexcept`
* `template <typename T> const T& Variant::`**`ref_as`**`() const noexcept`
* `template <int Index> [type at index]& Variant::`**`ref_at`**`() noexcept`
* `template <int Index> const [type at index]& Variant::`**`ref_at`**`() const noexcept`

Return a reference to the variant's current value. These will fail to compile
if `T` is not a branch of the variant, or the index is out of bounds. The
three sets of functions differ in how they behave when the requested type or
index does not match the current branch: the `as()/at()` versions will throw
`VariantError`; the `get_as()/get_at()` versions return the default value; the
`ref_as()/ref_at()` versions do not check the current branch type, and
behaviour is undefined if it does not match.

* `template <typename T, typename F> void Variant::`**`call_if`**`(F f)`
* `template <typename T, typename F> void Variant::`**`call_if`**`(F f) const`
* `template <typename T, typename F> [return type of F] Variant::`**`call_or`**`(F f) const`
* `template <typename T, typename F, typename RT> RT Variant::`**`call_or`**`(F f, const RT& def) const`

Call a function on the variant's current value, if the current branch matches
the specified type. All of these expect `F` to be a unary function that takes
a `T&` (first version) or `const T&` (other versions). In the first version of
`call_or()`, the return type of `F` must be default constructible; in the
second version, it must be implicitly convertible to `RT`.

* `bool Variant::`**`empty`**`() const noexcept`

True if the current branch is `Nil`. If `is_nullable` is false, `empty()` will
always be false.

* `size_t Variant::`**`hash`**`() const noexcept`
* `template <typename... TS> struct std::`**`hash`**`<Variant<TS...>>`

Hash function. This will call the hash function of the current branch; it will
fail to compile if any branch type does not have an instantiation of
`std::hash` (one is supplied for `Nil`).

* `int Variant::`**`index`**`() const noexcept`

Returns the zero-based index of the current branch. The return value will
always be greater than or equal to zero and less than the number of types in
the `Variant` template argument list.

* `template <typename T> bool Variant::`**`is`**`() const noexcept`

True if `T` is the current branch type. This will fail to compile if `T` is
not one of the variant's branches.

* `bool Variant::`**`is_equal`**`(const Variant& v) const noexcept`
* `bool Variant::`**`is_greater`**`(const Variant& v) const noexcept`
* `bool Variant::`**`is_less`**`(const Variant& v) const noexcept`
* `template <typename T> bool Variant::`**`is_equal`**`(const T& t) const noexcept`
* `template <typename T> bool Variant::`**`is_greater`**`(const T& t) const noexcept`
* `template <typename T> bool Variant::`**`is_less`**`(const T& t) const noexcept`
* `bool` **`operator==`**`(const Variant& lhs, const Variant& rhs)`
* `bool` **`operator!=`**`(const Variant& lhs, const Variant& rhs)`
* `bool` **`operator<`**`(const Variant& lhs, const Variant& rhs)`
* `bool` **`operator>`**`(const Variant& lhs, const Variant& rhs)`
* `bool` **`operator<=`**`(const Variant& lhs, const Variant& rhs)`
* `bool` **`operator>=`**`(const Variant& lhs, const Variant& rhs)`
* `template <typename T> bool` **`operator==`**`(const Variant& lhs, const T& rhs)`
* `template <typename T> bool` **`operator!=`**`(const Variant& lhs, const T& rhs)`
* `template <typename T> bool` **`operator<`**`(const Variant& lhs, const T& rhs)`
* `template <typename T> bool` **`operator>`**`(const Variant& lhs, const T& rhs)`
* `template <typename T> bool` **`operator<=`**`(const Variant& lhs, const T& rhs)`
* `template <typename T> bool` **`operator>=`**`(const Variant& lhs, const T& rhs)`
* `template <typename T> bool` **`operator==`**`(const T& lhs, const Variant& rhs)`
* `template <typename T> bool` **`operator!=`**`(const T& lhs, const Variant& rhs)`
* `template <typename T> bool` **`operator<`**`(const T& lhs, const Variant& rhs)`
* `template <typename T> bool` **`operator>`**`(const T& lhs, const Variant& rhs)`
* `template <typename T> bool` **`operator<=`**`(const T& lhs, const Variant& rhs)`
* `template <typename T> bool` **`operator>=`**`(const T& lhs, const Variant& rhs)`

Comparison functions. If both arguments are the same type, that type's
equality operator or `std::less` will be called. Otherwise, the side with the
lower branch index is considered less than the other. The versions that take
an argument of a different type will fail to compile if `T` is not a branch of
the variant.

* `void Variant::`**`reset`**`()`

Resets the variant to a default constructed instance of its first branch type.

* `std::string Variant::`**`str`**`() const`
* `std::string` **`to_str`**`(const Variant& v)`
* `void Variant::`**`write`**`(std::ostream& out) const`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Variant& v)`

Formatting functions. These call `to_str()` or the output operator on the
current branch type, and will fail to compile if this is not defined for all
branches. An empty variant (one whose current branch is `Nil`) will be written
as `"nil"`.


* `template <typename V, int Index> using` **`VariantTypeAt`** _[type at the given index, or `Nil` if out of bounds]_
* `template <typename V> struct` **`IsVariant`** _[true if `V` is a variant]_
* `template <typename V> constexpr bool` **`is_variant`**
* `template <typename V, typename T> struct` **`VariantHasType`** _[true if `T` is one of the variant's types]_
* `template <typename V, typename T> constexpr bool` **`variant_has_type`**
* `template <typename V, typename T> struct` **`VariantIndexOf`** _[index of `T` in the variant, or -1 if not found]_
* `template <typename V, typename T> constexpr int` **`variant_index_of`**
* `template <typename V> struct` **`VariantLength`** _[number of types in the variant]_
* `template <typename V> constexpr int` **`variant_length`**


Variant metafunctions.

* `template <typename V, typename T> constexpr void` **`static_check_variant_type`**`() noexcept`
* `template <typename V, int Index> constexpr void` **`static_check_variant_index`**`() noexcept`

These are wrappers around `static_assert` checks that `V` is a variant type
and that the branch type or index is valid. These only perform static checks
that the arguments are legal as far as the type system is concerned; they do
not check the actual status of a variant object.
