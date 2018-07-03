# Order Preserving Map #

By Ross Smith

* `#include "rs-core/ordered-map.hpp"`

## Contents ##

[TOC]

## Class OrderedMap ##

* `template <typename K, typename T> class` **`OrderedMap`**
    * `typename OrderedMap::`**`const_iterator`**
    * `typename OrderedMap::`**`iterator`**
    * `using OrderedMap::`**`key_type`** `= K`
    * `using OrderedMap::`**`mapped_type`** `= T`
    * `using OrderedMap::`**`value_type`** `= std::pair<const K, T>`
    * `T& OrderedMap::`**`operator[]`**`(const K& k)`
    * `iterator OrderedMap::`**`begin`**`()`
    * `const_iterator OrderedMap::`**`begin`**`() const`
    * `const_iterator OrderedMap::`**`cbegin`**`() const`
    * `iterator OrderedMap::`**`end`**`()`
    * `const_iterator OrderedMap::`**`end`**`() const`
    * `const_iterator OrderedMap::`**`cend`**`() const`
    * `void OrderedMap::`**`clear`**`() noexcept`
    * `bool OrderedMap::`**`empty`**`() const noexcept`
    * `bool OrderedMap::`**`erase`**`(const K& k) noexcept`
    * `void OrderedMap::`**`erase`**`(const_iterator i) noexcept`
    * `iterator OrderedMap::`**`find`**`(const K& k)`
    * `const_iterator OrderedMap::`**`find`**`(const K& k) const`
    * `bool OrderedMap::`**`has`**`(const K& k) const`
    * `std::pair<iterator, bool> OrderedMap::`**`insert`**`(const K& k, const T& t)`
    * `std::pair<iterator, bool> OrderedMap::`**`insert`**`(const value_type& v)`
    * `iterator OrderedMap::`**`set`**`(const K& k, const T& t)`
    * `iterator OrderedMap::`**`set`**`(const value_type& v)`
    * `size_t OrderedMap::`**`size`**`() const noexcept`
    * `void OrderedMap::`**`swap`**`(OrderedMap& om) noexcept`
    * `void` **`swap`**`(OrderedMap& om1, OrderedMap& om2) noexcept`

An `OrderedMap` behaves the same as a `std::map` in most respects, except that
iteration over the map returns the elements in insertion order instead of the
key type's natural order.

The `has()` function returns true if a matching key is present in the map. The
`set()` functions perform an insert or update, creating a new value if the key
is not already present, or replacing the mapped value if it is not, returning
an iterator pointing to the new or modified element.
