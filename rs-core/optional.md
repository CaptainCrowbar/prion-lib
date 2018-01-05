# Optional Type #

By Ross Smith

* `#include "rs-core/optional.hpp"`

## Contents ##

[TOC]

## Exceptions ##

* `class` **`NullOption`**`: public std::runtime_error`
* `NullOption::`**`NullOption`**`()`

Thrown when the value of an empty `Optional` is accessed.

## Class Optional ##

* `template <typename T> class` **`Optional`**

A class that holds either a value of type `T` or nothing. `T` is only required
to be a concrete type, but in most cases any operation on `Optional<T>`
requires the same operation on `T` to be valid.

* `using Optional::`**`value_type`** `= T`

Member types.

* `Optional::`**`Optional`**`() noexcept`
* `Optional::`**`Optional`**`(std::nullptr_t) noexcept`
* `Optional::`**`Optional`**`(const Optional& opt)`
* `Optional::`**`Optional`**`(Optional&& opt) noexcept`
* `Optional::`**`Optional`**`(const T& t)`
* `Optional::`**`Optional`**`(T&& t) noexcept`
* `Optional::`**`~Optional`**`() noexcept`
* `Optional& Optional::`**`operator=`**`(const Optional& opt)`
* `Optional& Optional::`**`operator=`**`(Optional&& opt) noexcept`
* `Optional& Optional::`**`operator=`**`(const T& t)`
* `Optional& Optional::`**`operator=`**`(T&& t) noexcept`
* `Optional& Optional::`**`operator=`**`(std::nullptr_t) noexcept`

Life cycle operations. The default constructor and the constructor from a
`nullptr` create an empty `Optional` object; assignment from a `nullptr` makes
the object empty. Move construction or assignment from another `Optional`
leaves the RHS empty; copy or move construction or assignment from a `T`
performs the equivalent operation on `T`.

* `T* Optional::`**`begin`**`() noexcept`
* `const T* Optional::`**`begin`**`() const noexcept`
* `T* Optional::`**`end`**`() noexcept`
* `const T* Optional::`**`end`**`() const noexcept`

Begin and end iterators, treating the `Optional` as a container that may hold
zero or one element.

* `void Optional::`**`check`**`() const`

Throws `NullOption` if the object is empty; otherwise does nothing.

* `template <typename... Args> void Optional::`**`emplace`**`(Args&&... args)`

Constructs a new `T` value in place.

* `bool Optional::`**`has_value`**`() const noexcept`
* `explicit Optional::`**`operator bool`**`() const noexcept`

True if the object is not empty.

* `size_t Optional::`**`hash`**`() const noexcept`
* `template <typename T> struct std::`**`hash`**`<Optional<T>>`

Hash function. This will return zero if the object is empty, otherwise it will
call `std::hash<T>`.

* `void Optional::`**`reset`**`() noexcept`

Makes the object empty.

* `Ustring Optional::`**`str`**`() const`
* `template <typename T> std::ostream&` **`operator<<`**`(std::ostream& out, const Optional& opt)`
* `template <typename T> Ustring` **`to_str`**`(const Optional& opt)`

Convert the object to a string (all of these produce the same string). These
will return `"null"` if the object is empty, otherwise they will call
`to_str(T)`.

* `void Optional::`**`swap`**`(Optional& opt) noexcept`
* `template <typename T> void` **`swap`**`(Optional& lhs, Optional& rhs) noexcept`

Swap two objects. These will call `swap(T)` if neither object is empty; if
only one has a value, this is equivalent to move assignment.

* `T& Optional::`**`value`**`()`
* `const T& Optional::`**`value`**`() const`
* `T& Optional::`**`operator*`**`()`
* `const T& Optional::`**`operator*`**`() const`
* `T* Optional::`**`operator->`**`()`
* `const T* Optional::`**`operator->`**`() const`

Query the value contained in an `Optional` object. All of these will throw
`NullOption` if the object is empty.

* `const T& Optional::`**`value_or`**`(const T& defval = {}) const`

Query the value contained in an `Optional` object, returning a default value
instead of failing if the object is empty.

* `bool` **`operator==`**`(const Optional& lhs, const Optional& rhs) noexcept`
* `bool` **`operator==`**`(const Optional& lhs, const T& rhs) noexcept`
* `bool` **`operator==`**`(const T& lhs, const Optional& rhs) noexcept`
* `bool` **`operator==`**`(const Optional& lhs, std::nullptr_t rhs) noexcept`
* `bool` **`operator==`**`(std::nullptr_t lhs, const Optional& rhs) noexcept`
* `bool` **`operator!=`**`(const Optional& lhs, const Optional& rhs) noexcept`
* `bool` **`operator!=`**`(const Optional& lhs, const T& rhs) noexcept`
* `bool` **`operator!=`**`(const T& lhs, const Optional& rhs) noexcept`
* `bool` **`operator!=`**`(const Optional& lhs, std::nullptr_t rhs) noexcept`
* `bool` **`operator!=`**`(std::nullptr_t lhs, const Optional& rhs) noexcept`
* `bool` **`operator<`**`(const Optional& lhs, const Optional& rhs) noexcept`
* `bool` **`operator<`**`(const Optional& lhs, const T& rhs) noexcept`
* `bool` **`operator<`**`(const T& lhs, const Optional& rhs) noexcept`
* `bool` **`operator<`**`(const Optional& lhs, std::nullptr_t rhs) noexcept`
* `bool` **`operator<`**`(std::nullptr_t lhs, const Optional& rhs) noexcept`
* `bool` **`operator>`**`(const Optional& lhs, const Optional& rhs) noexcept`
* `bool` **`operator>`**`(const Optional& lhs, const T& rhs) noexcept`
* `bool` **`operator>`**`(const T& lhs, const Optional& rhs) noexcept`
* `bool` **`operator>`**`(const Optional& lhs, std::nullptr_t rhs) noexcept`
* `bool` **`operator>`**`(std::nullptr_t lhs, const Optional& rhs) noexcept`
* `bool` **`operator<=`**`(const Optional& lhs, const Optional& rhs) noexcept`
* `bool` **`operator<=`**`(const Optional& lhs, const T& rhs) noexcept`
* `bool` **`operator<=`**`(const T& lhs, const Optional& rhs) noexcept`
* `bool` **`operator<=`**`(const Optional& lhs, std::nullptr_t rhs) noexcept`
* `bool` **`operator<=`**`(std::nullptr_t lhs, const Optional& rhs) noexcept`
* `bool` **`operator>=`**`(const Optional& lhs, const Optional& rhs) noexcept`
* `bool` **`operator>=`**`(const Optional& lhs, const T& rhs) noexcept`
* `bool` **`operator>=`**`(const T& lhs, const Optional& rhs) noexcept`
* `bool` **`operator>=`**`(const Optional& lhs, std::nullptr_t rhs) noexcept`
* `bool` **`operator>=`**`(std::nullptr_t lhs, const Optional& rhs) noexcept`

Comparison operators. All empty objects compare equal; an empty object comes
before any non-empty object; if both arguments are non-empty, `T`'s comparison
operators are called.

* `Optional` **`operator&`**`(const Optional& lhs, const Optional& rhs)`
* `Optional` **`operator&`**`(const Optional& lhs, const T& rhs)`
* `Optional` **`operator&`**`(const T& lhs, const Optional& rhs)`
* `Optional` **`operator&`**`(const Optional& lhs, std::nullptr_t rhs)`
* `Optional` **`operator&`**`(std::nullptr_t lhs, const Optional& rhs)`
* `Optional` **`operator|`**`(const Optional& lhs, const Optional& rhs)`
* `Optional` **`operator|`**`(const Optional& lhs, const T& rhs)`
* `Optional` **`operator|`**`(const T& lhs, const Optional& rhs)`
* `Optional` **`operator|`**`(const Optional& lhs, std::nullptr_t rhs)`
* `Optional` **`operator|`**`(std::nullptr_t lhs, const Optional& rhs)`

Option coalescing operators. The `&` operator returns `rhs` if both arguments
have values, otherwise null; the `|` operator returns `lhs` if `lhs` has a
value, otherwise `rhs`.

* `template <typename T> Optional` **`make_optional`**`(T&& t)`

Constructs an `Optional<T>`.

* `template <typename F, typename T, typename... Args> [see below]` **`opt_call`**`(F f, Optional<T>& opt, Args&&... args)`
* `template <typename F, typename T, typename... Args> [see below]` **`opt_call`**`(F f, const Optional<T>& opt, Args&&... args)`
* `template <typename F, typename T1, typename T2, typename... Args> [see below]` **`opt_call`**`(F f, Optional<T1>& opt1, Optional<T2>& opt2, Args&&... args)`
* `template <typename F, typename T1, typename T2, typename... Args> [see below]` **`opt_call`**`(F f, const Optional<T1>& opt1, Optional<T2>& opt2, Args&&... args)`
* `template <typename F, typename T1, typename T2, typename... Args> [see below]` **`opt_call`**`(F f, Optional<T1>& opt1, const Optional<T2>& opt2, Args&&... args)`
* `template <typename F, typename T1, typename T2, typename... Args> [see below]` **`opt_call`**`(F f, const Optional<T1>& opt1, const Optional<T2>& opt2, Args&&... args)`

These call a function with one or two optional arguments, possibly followed by
any number of other arguments. The callback function `f()` is expected to take
a `T`, or a `T1` and a `T2`, as its first one or two arguments; these may be
constant or mutable, and the function may or may not return a value. If `opt`
is non-empty, or if `opt1` and `opt2` are both non-empty, the function is
called on their values, and an `Optional<R>`, where `R` is `f()`'s return
type, is wrapped around the result; otherwise, an empty `Optional<R>` is
returned. As a special case, if `f()` returns `void`, so does `opt_call()`.
