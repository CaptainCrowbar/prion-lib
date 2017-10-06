# Compact Array #

By Ross Smith

* `#include "rs-core/compact-array.hpp"`

## Contents ##

[TOC]

## Compact array ##

* `template <typename T, size_t N> class` **`CompactArray`**

`CompactArray` is a vector-like container optimized for small numbers of
objects. An extra template parameter specifies a size threshold beyond which
the array is seldom or never expected to grow. As long as the actual element
count stays less than or equal to the threshold, the entire array will be held
on the stack, and no dynamic allocation will be done (unless the element type
itself does any internally). If the size grows beyond the threshold, the
`CompactArray` will transparently switch to dynamic allocation. The user does
not need to know or care when the switch to dynamic allocation takes place.

The element type `T` must be a complete, concrete type that is at least
destructible and either move or copy constructible and assignable. Some
operations place additional requirements on `T`. Copy construction and
assignment are required by any operation that may copy an object; default
construction is only required by operations that may create new elements
without being supplied with a value to give them. `CompactArray`'s equality
comparison calls `T`'s equality operator; the ordered comparisons call `T`'s
less-than operator. `CompactArray`'s hash function calls `std::hash<T>`.
Comparison, swap, move, and hash operations on `T` are assumed not to throw.

The threshold size (`N`) should not be too large; the CompactArray allocates
`N*sizeof(T)` bytes on the stack, plus a few bytes for housekeeping. Setting
`N` to zero effectively turns `CompactArray` into a `std::vector`.

Most of the `CompactArray` interface is the same as a standard vector.
Operations that are new to `CompactArray` or differ in some significant way
from the corresponding operations on a vector are described in more detail.

* `class CompactArray::`**`const_iterator`**
* `using CompactArray::`**`const_reference`** `= const T&`
* `using CompactArray::`**`difference_type`** `= ptrdiff_t`
* `class CompactArray::`**`iterator`**
* `using CompactArray::`**`reference`** `= T&`
* `using CompactArray::`**`size_type`** `= size_t`
* `using CompactArray::`**`value_type`** `= T`

Member types.

* `static constexpr size_t CompactArray::`**`threshold`** `= N`

Member constant.

* `CompactArray::`**`CompactArray`**`() noexcept`
* `explicit CompactArray::`**`CompactArray`**`(size_t n, const T& t = {})`
* `template <typename InputIterator> CompactArray::`**`CompactArray`**`(InputIterator i, InputIterator j, ArrayDetail::IteratorEnable<InputIterator, T>* = nullptr)`
* `CompactArray::`**`CompactArray`**`(std::initializer_list<T> list)`
* `CompactArray::`**`~CompactArray`**`() noexcept`
* `CompactArray::`**`CompactArray`**`(const CompactArray& ca)`
* `CompactArray::`**`CompactArray`**`(CompactArray&& ca) noexcept`
* `CompactArray& CompactArray::`**`operator=`**`(const CompactArray& ca)`
* `CompactArray& CompactArray::`**`operator=`**`(CompactArray&& ca) noexcept`
* `CompactArray& CompactArray::`**`operator=`**`(std::initializer_list<T> list)`

Life cycle functions.

* `T& CompactArray::`**`operator[]`**`(size_t i) noexcept`
* `const T& CompactArray::`**`operator[]`**`(size_t i) const noexcept`
* `T& CompactArray::`**`at`**`(size_t i)`
* `const T& CompactArray::`**`at`**`(size_t i) const`
* `CompactArray::iterator CompactArray::`**`begin`**`() noexcept`
* `CompactArray::const_iterator CompactArray::`**`begin`**`() const noexcept`
* `CompactArray::const_iterator CompactArray::`**`cbegin`**`() const noexcept`
* `CompactArray::iterator CompactArray::`**`end`**`() noexcept`
* `CompactArray::const_iterator CompactArray::`**`end`**`() const noexcept`
* `CompactArray::const_iterator CompactArray::`**`cend`**`() const noexcept`
* `T* CompactArray::`**`data`**`() noexcept`
* `const T* CompactArray::`**`data`**`() const noexcept`
* `const T* CompactArray::`**`cdata`**`() const noexcept`

Element access functions.

* `size_t CompactArray::`**`capacity`**`() const noexcept`
* `void CompactArray::`**`clear`**`() noexcept`
* `bool CompactArray::`**`empty`**`() const noexcept`
* `bool CompactArray::`**`is_compact`**`() const noexcept`
* `void CompactArray::`**`reserve`**`(size_t n)`
* `void CompactArray::`**`resize`**`(size_t n, const T& t = {})`
* `void CompactArray::`**`shrink_to_fit`**`()`
* `size_t CompactArray::`**`size`**`() const noexcept`

Capacity functions. The capacity will never be less than `N`; if it grows
above `N`, the dynamically allocated capacity grows by factors of 2, in the
same way as most `std::vector` implementations. The `is_compact()` function
can be used to check whether the elements are currently statically or
dynamically allocated. Once allocation has switched to dynamic memory, the
capacity will not be reduced by operations that remove elements (except
`clear()`, which resets the container to static allocation). The user can
reduce the current allocation overhead by calling `shrink_to_fit()`, which
forces the container back to static allocation if the current element count is
small enough, or if not, reallocates the dynamic array to set the capacity
exactly equal to the current size.

* `template <typename InputIterator> CompactArray::iterator CompactArray::`**`append`**`(InputIterator i, InputIterator j)`
* `template <typename InputRange> CompactArray::iterator CompactArray::`**`append`**`(const InputRange& r)`
* `template <typename InputRange> CompactArray::iterator CompactArray::`**`append`**`(InputRange&& r)`
* `template <typename... Args> void CompactArray::`**`emplace_back`**`(Args&&... args)`
* `template <typename... Args> CompactArray::iterator CompactArray::`**`emplace`**`(const_iterator i, Args&&... args)`
* `CompactArray::iterator CompactArray::`**`insert`**`(const_iterator i, const T& t)`
* `CompactArray::iterator CompactArray::`**`insert`**`(const_iterator i, T&& t)`
* `template <typename InputIterator> CompactArray::iterator CompactArray::`**`insert`**`(const_iterator i, InputIterator j, InputIterator k)`
* `void CompactArray::`**`push_back`**`(const T& t)`
* `void CompactArray::`**`push_back`**`(T&& t)`

Insertion functions. The `append()` function is a convenience shorthand for
insertion at the end, which is usually more efficient than insertion anywhere
else. Like `insert()`, it returns an iterator pointing to the first inserted
element.

* `void CompactArray::`**`erase`**`(const_iterator i) noexcept`
* `void CompactArray::`**`erase`**`(const_iterator i, const_iterator j) noexcept`
* `void CompactArray::`**`pop_back`**`() noexcept`

Erasure functions.

* `size_t CompactArray::`**`hash`**`() const noexcept`
* `class std::`**`hash`**`<CompactArray<T, N>>`

Hash functions.

* `void CompactArray::`**`swap`**`(CompactArray& ca) noexcept`
* `void` **`swap`**`(CompactArray& a, CompactArray& b) noexcept`

Swap functions.

* `bool` **`operator==`**`(const CompactArray& lhs, const CompactArray& rhs) noexcept`
* `bool` **`operator!=`**`(const CompactArray& lhs, const CompactArray& rhs) noexcept`
* `bool` **`operator<`**`(const CompactArray& lhs, const CompactArray& rhs) noexcept`
* `bool` **`operator>`**`(const CompactArray& lhs, const CompactArray& rhs) noexcept`
* `bool` **`operator<=`**`(const CompactArray& lhs, const CompactArray& rhs) noexcept`
* `bool` **`operator>=`**`(const CompactArray& lhs, const CompactArray& rhs) noexcept`

Comparison operators.
