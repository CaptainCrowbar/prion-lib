# Bounded Array #

By Ross Smith

* `#include "rs-core/bounded-array.hpp"`

## Contents ##

[TOC]

## Bounded array ##

* `template <typename T, size_t N> class` **`BoundedArray`**

`BoundedArray` is a vector-like container with a fixed, statically known upper
limit on its size (supplied as a template parameter). Its contents are always
stored on the stack, and no dynamic allocation will be done (unless the
element type itself does any internally). Any attempt to grow the size beyond
the fixed limit will throw a `std::length_error` exception.

The element type `T` must be a complete, concrete type that is at least
destructible and either move or copy constructible and assignable. Some
operations place additional requirements on `T`. Copy construction and
assignment are required by any operation that may copy an object; default
construction is only required by operations that may create new elements
without being supplied with a value to give them. `BoundedArray`'s equality
comparison calls `T`'s equality operator; the ordered comparisons call `T`'s
less-than operator. `BoundedArray`'s hash function calls `std::hash<T>`.
Comparison, swap, move, and hash operations on `T` are assumed not to throw.

Most of the `BoundedArray` interface is the same as a standard vector.
Operations that are new to `BoundedArray` or differ in some significant way
from the corresponding operations on a vector are described in more detail.

* `class BoundedArray::`**`const_iterator`**
* `using BoundedArray::`**`const_reference`** `= const T&`
* `using BoundedArray::`**`difference_type`** `= ptrdiff_t`
* `class BoundedArray::`**`iterator`**
* `using BoundedArray::`**`reference`** `= T&`
* `using BoundedArray::`**`size_type`** `= size_t`
* `using BoundedArray::`**`value_type`** `= T`

Member types.

* `static constexpr size_t BoundedArray::`**`bound`** `= N`

Member constant.

* `BoundedArray::`**`BoundedArray`**`() noexcept`
* `explicit BoundedArray::`**`BoundedArray`**`(size_t n, const T& t = {})`
* `template <typename InputIterator> BoundedArray::`**`BoundedArray`**`(InputIterator i, InputIterator j, ArrayDetail::IteratorEnable<InputIterator, T>* = nullptr)`
* `BoundedArray::`**`BoundedArray`**`(std::initializer_list<T> list)`
* `BoundedArray::`**`~BoundedArray`**`() noexcept`
* `BoundedArray::`**`BoundedArray`**`(const BoundedArray& ca)`
* `BoundedArray::`**`BoundedArray`**`(BoundedArray&& ca) noexcept`
* `BoundedArray& BoundedArray::`**`operator=`**`(const BoundedArray& ca)`
* `BoundedArray& BoundedArray::`**`operator=`**`(BoundedArray&& ca) noexcept`
* `BoundedArray& BoundedArray::`**`operator=`**`(std::initializer_list<T> list)`

Life cycle functions.

* `T& BoundedArray::`**`operator[]`**`(size_t i) noexcept`
* `const T& BoundedArray::`**`operator[]`**`(size_t i) const noexcept`
* `T& BoundedArray::`**`at`**`(size_t i)`
* `const T& BoundedArray::`**`at`**`(size_t i) const`
* `BoundedArray::iterator BoundedArray::`**`begin`**`() noexcept`
* `BoundedArray::const_iterator BoundedArray::`**`begin`**`() const noexcept`
* `BoundedArray::const_iterator BoundedArray::`**`cbegin`**`() const noexcept`
* `BoundedArray::iterator BoundedArray::`**`end`**`() noexcept`
* `BoundedArray::const_iterator BoundedArray::`**`end`**`() const noexcept`
* `BoundedArray::const_iterator BoundedArray::`**`cend`**`() const noexcept`
* `T* BoundedArray::`**`data`**`() noexcept`
* `const T* BoundedArray::`**`data`**`() const noexcept`
* `const T* BoundedArray::`**`cdata`**`() const noexcept`

Element access functions.

* `size_t BoundedArray::`**`capacity`**`() const noexcept`
* `void BoundedArray::`**`clear`**`() noexcept`
* `bool BoundedArray::`**`empty`**`() const noexcept`
* `void BoundedArray::`**`resize`**`(size_t n, const T& t = {})`
* `size_t BoundedArray::`**`size`**`() const noexcept`

Capacity functions. The `capacity()` function always returns `N`. The other
functions have their usual semantics for sequential containers, except that
`resize()` will throw `std::length_error` if the new size exceeds the
capacity.

* `template <typename InputIterator> BoundedArray::iterator BoundedArray::`**`append`**`(InputIterator i, InputIterator j)`
* `template <typename InputRange> BoundedArray::iterator BoundedArray::`**`append`**`(const InputRange& r)`
* `template <typename InputRange> BoundedArray::iterator BoundedArray::`**`append`**`(InputRange&& r)`
* `template <typename... Args> void BoundedArray::`**`emplace_back`**`(Args&&... args)`
* `template <typename... Args> BoundedArray::iterator BoundedArray::`**`emplace`**`(const_iterator i, Args&&... args)`
* `BoundedArray::iterator BoundedArray::`**`insert`**`(const_iterator i, const T& t)`
* `BoundedArray::iterator BoundedArray::`**`insert`**`(const_iterator i, T&& t)`
* `template <typename InputIterator> BoundedArray::iterator BoundedArray::`**`insert`**`(const_iterator i, InputIterator j, InputIterator k)`
* `void BoundedArray::`**`push_back`**`(const T& t)`
* `void BoundedArray::`**`push_back`**`(T&& t)`

Insertion functions. The `append()` function is a convenience shorthand for
insertion at the end, which is usually more efficient than insertion anywhere
else; like `insert()`, it returns an iterator pointing to the first inserted
element. All of these will throw `std::length_error` if the insertion would
grow the size beyond the static capacity.

* `void BoundedArray::`**`erase`**`(const_iterator i) noexcept`
* `void BoundedArray::`**`erase`**`(const_iterator i, const_iterator j) noexcept`
* `void BoundedArray::`**`pop_back`**`() noexcept`

Erasure functions.

* `size_t BoundedArray::`**`hash`**`() const noexcept`
* `class std::`**`hash`**`<BoundedArray<T, N>>`

Hash functions.

* `void BoundedArray::`**`swap`**`(BoundedArray& ca) noexcept`
* `void` **`swap`**`(BoundedArray& a, BoundedArray& b) noexcept`

Swap functions.

* `bool` **`operator==`**`(const BoundedArray& lhs, const BoundedArray& rhs) noexcept`
* `bool` **`operator!=`**`(const BoundedArray& lhs, const BoundedArray& rhs) noexcept`
* `bool` **`operator<`**`(const BoundedArray& lhs, const BoundedArray& rhs) noexcept`
* `bool` **`operator>`**`(const BoundedArray& lhs, const BoundedArray& rhs) noexcept`
* `bool` **`operator<=`**`(const BoundedArray& lhs, const BoundedArray& rhs) noexcept`
* `bool` **`operator>=`**`(const BoundedArray& lhs, const BoundedArray& rhs) noexcept`

Comparison operators.
