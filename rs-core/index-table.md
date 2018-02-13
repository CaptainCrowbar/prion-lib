# Multi-Index Map #

By Ross Smith

* `#include "rs-core/index-map.hpp"`

## Contents ##

[TOC]

## Introduction ##

The `IndexTable` and `Index` classes provide a collection over which an
arbitrary set of associative indexes can be created.

`IndexTable` is the actual data collection. This does not in itself provide
any associative semantics; it simply acts as a collection over which an index
can be built. The second class, `Index`, maintains an index over a table. The
index is generated when the `Index` object is constructed, and automatically
kept up to date as elements are inserted or erased from the underlying table.

An index may be unique keyed (no two keys may be equivalent, according to the
comparison function) or duplicate keyed (equivalent keys are allowed). This
choice is controlled by a template argument to the `Index` class. An attempt
to create a duplicate key in a unique index will throw an exception. A single
`IndexTable` can have separate unique and non-unique indexes at the same time.

The only requirement on the data type (`T`) and the key type (`K`) is that
they must be copyable. Functions must be provided to extract a key from a data
value, and to compare keys; these default to `static_cast<K>(T)` and
`std::less<K>` respectively.

All `IndexTable` and `Index` iterators are bidirectional iterators, with `T`
as their value type. Inserting an element does not invalidate any iterators;
erasing an element only invalidates any iterators that were pointing to that
element. Although the non-`const` iterators over an `IndexTable` or `Index`
provide a reference through which data elements may be updated, behaviour is
undefined if any operation on an element (other than insertion or deletion)
would change the key associated with it in any currently existing index.

In the complexity specifications, `n` is the total number of elements in the
table, and `k` is the number involved in a single operation (e.g. the number
inserted or erased).

## Supporting definitions ##

* `class` **`IndexCollision`**`: public std::runtime_error`

An `IndexCollision` exception is thrown if a duplicate key is inserted into a
unique index.

* `enum class` **`IndexMode`**
    * `IndexMode::`**`unique`**
    * `IndexMode::`**`duplicate`**

Flags used in the `Index` class template to indicate whether duplicates are
allowed.

## Class IndexTable ##

* `template <typename T> class` **`IndexTable`**

The basic data table class.

* `using IndexTable::`**`const_iterator`** `= [bidirectional iterator]`
* `using IndexTable::`**`iterator`** `= [bidirectional iterator]`
* `using IndexTable::`**`const_reference`** `= const T&`
* `using IndexTable::`**`difference_type`** `= ptrdiff_t`
* `using IndexTable::`**`reference`** `= T&`
* `using IndexTable::`**`size_type`** `= size_t`
* `using IndexTable::`**`value_type`** `= T`

Member types.

* `IndexTable::`**`IndexTable`**`()`
* `template <typename InputRange> explicit IndexTable::`**`IndexTable`**`(const InputRange& src)`
* `template <typename Iterator> IndexTable::`**`IndexTable`**`(Iterator i1, Iterator i2)`
* `IndexTable::`**`~IndexTable`**`() noexcept`

Life cycle functions. The default constructor creates an empty table; the
second and third constructors copy elements from the range into the table.
_(Complexity: Amortised constant for the default constructor, otherwise
`O(n)`.)_

* `iterator IndexTable::`**`begin`**`()`
* `const_iterator IndexTable::`**`begin`**`() const`
* `iterator IndexTable::`**`end`**`()`
* `const_iterator IndexTable::`**`end`**`() const`

Iterators over the elements of the table, in the order in which they were
inserted. _(Complexity: Constant.)_

* `void IndexTable::`**`clear`**`()`

Deletes all elements. _(Complexity: `O(n)`.)_

* `bool IndexTable::`**`empty`**`() const noexcept`

True if the table is empty. _(Complexity: Constant.)_

* `void IndexTable::`**`erase`**`(iterator i)`
* `void IndexTable::`**`erase`**`(iterator i1, iterator i2)`

Remove elements from the table. _(Complexity: Amortised constant for the first
version, `O(k)` for the second.)_

* `void IndexTable::`**`insert`**`(const T& t)`
* `template <typename InputRange> void IndexTable::`**`insert`**`(const InputRange& src)`
* `template <typename Iterator> void IndexTable::`**`insert`**`(Iterator i1, Iterator i2)`
* `void IndexTable::`**`push_back`**`(const T& t)`

These insert items into the table. The first version of `insert()` is a
synonym for `push_back()`. All of these will throw `IndexCollision` if it
would create a duplicate key in an index that does not allow duplicates.
_(Complexity: `O(k)`.)_

* `size_t IndexTable::`**`size`**`() const noexcept`

Returns the number of items in the table. _(Complexity: Amortised constant.)_

## Class Index ##

* `template <typename K, typename T, IndexMode M = IndexMode::unique> class` **`Index`**

The index type. The template arguments are the key type, the data type, and a
flag indicating whether duplicate keys will be allowed.

* `using Index::`**`iterator`** `= [bidirectional iterator]`
    * `const K& iterator::`**`key`**`() const noexcept`
* `using Index::`**`const_iterator`** `= [bidirectional iterator]`
    * `const K& const_iterator::`**`key`**`() const noexcept`
* `using Index::`**`compare_function`** `= function<bool(const K&, const K&)>`
* `using Index::`**`const_reference`** `= const T&`
* `using Index::`**`difference_type`** `= ptrdiff_t`
* `using Index::`**`extract_function`** `= function<K(const T&)>`
* `using Index::`**`key_type`** `= K`
* `using Index::`**`reference`** `= T&`
* `using Index::`**`size_type`** `= size_t`
* `using Index::`**`table_type`** `= IndexTable<T>`
* `using Index::`**`value_type`** `= T`

Member types. In addition to the normal bidirectional iterator operations, the
iterators have a `key()` member function that returns the element's key
(behaviour is undefined if this is called on a non-dereferenceable iterator).

* `static constexpr IndexMode Index::`**`mode`** `= M`

Member constants.

* `Index::`**`Index`**`()`
* `explicit Index::`**`Index`**`(table_type& table)`
* `Index::`**`Index`**`(table_type& table, extract_function extract)`
* `Index::`**`Index`**`(table_type& table, extract_function extract, compare_function compare)`
* `Index::`**`~Index`**`() noexcept`

Life cycle functions. The constructors take a reference to the table to be
indexed, and optionally a key extraction function (used to obtain the key from
a data value) and a comparison function (used to compare keys). The default
extraction function simply performs a `static_cast<K>(T)`; the default
comparison function is `std::less<K>`. A default constructed index is not
associated with any table and is always empty.

The constructors will build an index over the existing elements in the table;
they will throw `IndexCollision` if the table contains values with duplicate
keys but the index does not allow duplicates. An index may be destroyed before
or after its associated table. If the table is destroyed first, the index will
become empty. _(Complexity: Constant for the default constructor, `O(n)` for
the other constructors, amortised constant for the destructor.)_

* `iterator Index::`**`begin`**`()`
* `const_iterator Index::`**`begin`**`() const`
* `iterator Index::`**`end`**`()`
* `const_iterator Index::`**`end`**`() const`

Iterators over the table, in the order defined by the keys. If duplicates are
allowed, elements with duplicate keys are visited in order of insertion.
_(Complexity: Constant.)_

* `compare_function Index::`**`compare`**`() const`
* `extract_function Index::`**`extract`**`() const`

These return copies of the key comparison and extraction functions supplied to
the constructor. _(Complexity: Constant.)_

* `size_t Index::`**`count`**`(const K& k) const noexcept`

Returns the number of items in the table with the specified key. _(Complexity:
`O(log n)+O(k)`.)_

* `bool Index::`**`empty`**`() const noexcept`

True if the table is empty. This is equivalent to `table().empty()`, except
that it will return true if the table has been destroyed. _(Complexity:
Constant.)_

* `Irange<iterator> Index::`**`equal_range`**`(const K& k)`
* `Irange<const_iterator> Index::`**`equal_range`**`(const K& k) const`
* `iterator Index::`**`find`**`(const K& k)` _(unique keys only)_
* `const_iterator Index::`**`find`**`(const K& k) const` _(unique keys only)_
* `iterator Index::`**`lower_bound`**`(const K& k)`
* `const_iterator Index::`**`lower_bound`**`(const K& k) const`
* `iterator Index::`**`upper_bound`**`(const K& k)`
* `const_iterator Index::`**`upper_bound`**`(const K& k) const`

These find elements or subranges matching the specified key (following the
usual associative container semantics). The `find()` functions are not defined
for indices that allow duplicate keys. _(Complexity: `O(log n)`.)_

* `void Index::`**`erase`**`(iterator i)`
* `void Index::`**`erase`**`(iterator i1, iterator i2)`
* `void Index::`**`erase`**`(const K& k)`

These erase elements from the table, identified either by iterators or a key.
Erasing a nonexistent key is harmless. If the index allows duplicates, the
third version of `erase()` will erase all matching elements. _(Complexity:
Amortised constant for the first version; `O(log n)+O(k)` for the others.)_

* `size_t Index::`**`size`**`() const noexcept`

Returns the number of items in the table. This is equivalent to
`table().size()`, except that it will return zero if the table has been
destroyed. _(Complexity: Amortised constant.)_

* `table_type& Index::`**`table`**`() noexcept`
* `const table_type& Index::`**`table`**`() const noexcept`

These return references to the associated table. Behaviour is undefined if
the table has been destroyed or the index was default constructed.
_(Complexity: Constant.)_
