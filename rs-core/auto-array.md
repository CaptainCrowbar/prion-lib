# Automatically Resized Arrays #

By Ross Smith

* `#include "rs-core/auto-array.hpp"`

## Contents ##

[TOC]

## AutoVector and AutoDeque classes ##

* `template <typename T> class` **`AutoVector`**
* `template <typename T> class` **`AutoDeque`**

These classes are vector-like or deque-like containers that automatically
resize themselves when elements are inserted at a new index. The constructor
takes a default value, to be used when uninitialized elements are referenced.
Calling the non-`const` version of `operator[]` extends the array, if
necessary, to make the index valid. The `const` version of `operator[]` will
also work for any index, but it may simply return a reference to the stored
default value if the index is outside the current bounds.

The `AutoDeque` container uses signed indexing, with the zero index remaining
attached to the original beginning of the array. Elements before this point
can be created or referenced by calling `set()`, `get()`, or `operator[]` with
a negative index. The `realign()` function resets the zero index to the
current beginning of the array.

Most operations are common to both classes; these are indicated below by using
`[Class]` instead of one of the specific class names.

* `class [Class]::`**`const_iterator`**
* `class [Class]::`**`iterator`**
* `using [Class]::`**`value_type`** `= T`
* `using AutoVector::`**`index_type`** `= size_t`
* `using AutoDeque::`**`index_type`** `= ptrdiff_t`

Member types. The iterators have the same properties as those of the
corresponding standard containers.

* `[Class]::`**`[Class]`**`()`
* `explicit [Class]::`**`[Class]`**`(const T& def)`
* `explicit [Class]::`**`[Class]`**`(size_t n, const T& t, const T& def = T())`
* `template <typename InIter> [Class]::`**`[Class]`**`(InIter i1, InIter i2, const T& def = T())`
* `[Class]::`**`[Class]`**`(std::initializer_list<T> list)`

Constructors. These differ from those of the standard containers in having an
optional default value argument, defaulting to the element type's default
value. The element type does not have to be default constructible if a default
value is explicitly supplied.

* `[Class]::`**`~[Class]`**`()`
* `[Class]::`**`[Class]`**`(const [Class]& c)`
* `[Class]::`**`[Class]`**`([Class]&& c)`
* `[Class]& [Class]::`**`operator=`**`(const [Class]& c)`
* `[Class]& [Class]::`**`operator=`**`([Class]&& c)`
* `[Class]& [Class]::`**`operator=`**`(std::initializer_list<T> list)`

Other life cycle operations.

* `T& [Class]::`**`operator[]`**`(index_type i) noexcept`
* `const T& [Class]::`**`operator[]`**`(index_type i) const noexcept`
* `T& [Class]::`**`ref`**`(index_type i)`
* `const T& [Class]::`**`get`**`(index_type i) const noexcept`

Indexing functions. The `set()` and `get()` functions are equivalent to the
two overloads of `operator[]`. The non-`const` functions will return a
reference to the stored default value if the index is outside the current
range of the array; the `const` versions will extend the array if necessary to
make the index valid, filling any newly created elements with the default
value.

* `const T& [Class]::`**`get_default`**`() const noexcept`
* `void [Class]::`**`set_default`**`(const T& def)`

Query or change the default value. Changing the default value does not affect
any existing elements that have already been created with this value.

* `ptrdiff_t AutoDeque::`**`min_index`**`() const noexcept`
* `ptrdiff_t AutoDeque::`**`max_index`**`() const noexcept`

The minimum and maximum index of the array, as it is currently stored. If the
array is empty, these will return 0 and -1 respectively.

* `void AutoDeque::`**`realign`**`() noexcept`

If the array is not empty, this resets the zero index to the current beginning
of the array (the non-default element with the lowest index).

* `void [Class]::`**`trim`**`() noexcept`

Removes all trailing elements that are equal to the default value. For
`AutoDeque`, this also removes any leading default elements with negative
indices.

* `std::ostream&` **`operator<<`**`(std::ostream& out, const [Class]& obj)`

Arrays are output in the same format as
[`to_str()`](string.html#string-formatting-functions).

* `iterator [Class]::`**`begin`**`() noexcept`
* `const_iterator [Class]::`**`begin`**`() const noexcept`
* `const_iterator [Class]::`**`cbegin`**`() const noexcept`
* `iterator [Class]::`**`end`**`() noexcept`
* `const_iterator [Class]::`**`end`**`() const noexcept`
* `const_iterator [Class]::`**`cend`**`() const noexcept`
* `bool [Class]::`**`empty`**`() const noexcept`
* `size_t [Class]::`**`size`**`() const noexcept`
* `size_t AutoVector::`**`capacity`**`() const noexcept`
* `void [Class]::`**`clear`**`() noexcept`
* `void [Class]::`**`clear`**`(const T& def)`
* `void AutoVector::`**`reserve`**`(size_t n)`
* `void AutoVector::`**`shrink_to_fit`**`() noexcept`
* `iterator [Class]::`**`insert`**`(const_iterator i, const T& t)`
* `iterator [Class]::`**`insert`**`(const_iterator i, size_t n, const T& t)`
* `template <typename InIter> iterator [Class]::`**`insert`**`(const_iterator i, InIter j1, InIter j2)`
* `iterator [Class]::`**`insert`**`(const_iterator i, std::initializer_list<T> list)`
* `template <typename... Args> iterator [Class]::`**`emplace`**`(const_iterator i, Args&&... args)`
* `iterator [Class]::`**`erase`**`(const_iterator i) noexcept`
* `iterator [Class]::`**`erase`**`(const_iterator i1, const_iterator i2) noexcept`
* `void [Class]::`**`swap`**`([Class]& obj) noexcept`
* `void` **`swap`**`([Class]& lhs, [Class]& rhs) noexcept`
* `bool` **`operator==`**`(const [Class]& rhs) const noexcept`
* `bool` **`operator!=`**`(const [Class]& rhs) const noexcept`
* `bool` **`operator<`**`(const [Class]& rhs) const noexcept`
* `bool` **`operator>`**`(const [Class]& rhs) const noexcept`
* `bool` **`operator<=`**`(const [Class]& rhs) const noexcept`
* `bool` **`operator>=`**`(const [Class]& rhs) const noexcept`

These all have the same behaviour as the corresponding operations on the
standard containers.
