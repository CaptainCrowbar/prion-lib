# Hash Map and Set #

By Ross Smith

* `#include "rs-core/hash-map.hpp"`

## Contents ##

[TOC]

## HashMap and HashSet classes ##

<!-- DEFN -->
In the table below, `Hash*` is used to indicate members that are present in
both `HashMap` and `HashSet`. Where an entry refers explicitly to one of the
two class templates, it applies only to that one.

<!-- TEXT -->
* `template <typename K, typename T, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>> class` **`HashMap`**
* `template <typename K, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>> using` **`HashSet`** `= HashMap<K, void, Hash, Equal>`
    * `class Hash*::`**`iterator`**
    * `class Hash*::`**`const_iterator`**
    * `using Hash*::`**`hasher`** `= Hash`
    * `using Hash*::`**`key_equal`** `= Equal`
    * `using Hash*::`**`key_type`** `= K`
    * `using Hash*::`**`mapped_type`** `= T`
    * `using HashMap::`**`value_type`** `= std::pair<const K, T>`
    * `using HashSet::`**`value_type`** `= K`
    * `Hash*::`**`Hash*`**`() noexcept`
    * `explicit Hash*::`**`Hash*`**`(Hash h, Equal e = {})`
    * `Hash*::`**`Hash*`**`(const Hash*& map)`
    * `Hash*::`**`Hash*`**`(Hash*&& map) noexcept`
    * `Hash*::`**`~Hash*`**`() noexcept`
    * `Hash*& Hash*::`**`operator=`**`(const Hash*& map)`
    * `Hash*& Hash*::`**`operator=`**`(Hash*&& map) noexcept`
    * `T& HashMap::`**`operator[]`**`(K key)`
    * `T& HashMap::`**`at`**`(K key)`
    * `const T& HashMap::`**`at`**`(K key) const`
    * `template <typename K1, typename... TS> std::pair<HashMap::iterator, bool> HashMap::`**`emplace`**`(K1&& k, TS&&... ts)`
    * `template <typename... TS> std::pair<HashSet::iterator, bool> HashSet::`**`emplace`**`(TS&&... ts)`
    * `Hash*::iterator Hash*::`**`begin`**`() noexcept`
    * `Hash*::const_iterator Hash*::`**`begin`**`() const noexcept`
    * `Hash*::const_iterator Hash*::`**`cbegin`**`() const noexcept`
    * `Hash*::iterator Hash*::`**`end`**`() noexcept`
    * `Hash*::const_iterator Hash*::`**`end`**`() const noexcept`
    * `Hash*::const_iterator Hash*::`**`cend`**`() const noexcept`
    * `bool Hash*::`**`contains`**`(K key) const noexcept`
    * `size_t Hash*::`**`count`**`(K key) const noexcept`
    * `bool Hash*::`**`empty`**`() const noexcept`
    * `Hash*::iterator Hash*::`**`find`**`(K key) noexcept`
    * `Hash*::const_iterator Hash*::`**`find`**`(K key) const noexcept`
    * `size_t Hash*::`**`free_space`**`() const noexcept`
    * `Hash Hash*::`**`hash_function`**`() const`
    * `Equal Hash*::`**`key_eq`**`() const`
    * `size_t Hash*::`**`size`**`() const noexcept`
    * `size_t Hash*::`**`table_size`**`() const noexcept`
    * `void Hash*::`**`clear`**`() noexcept`
    * `Hash*::iterator Hash*::`**`erase`**`(const_iterator i) noexcept`
    * `size_t Hash*::`**`erase`**`(K key) noexcept`
    * `std::pair<Hash*::iterator, bool> Hash*::`**`insert`**`(const value_type& value)`
    * `std::pair<Hash*::iterator, bool> Hash*::`**`insert`**`(value_type&& value)`
    * `void Hash*::`**`rehash`**`()`
    * `void Hash*::`**`reserve`**`(size_t n)`
    * `void Hash*::`**`swap`**`(Hash*& map) noexcept`
    * `void` **`swap`**`(Hash*& a, Hash*& b) noexcept`
    * `bool` **`operator==`**`(const Hash*& lhs, const Hash*& rhs) noexcept`
    * `bool` **`operator!=`**`(const Hash*& lhs, const Hash*& rhs) noexcept`

This is an open-addressed hash table, which is usually more time-efficient and
cache-friendly, but less space-efficient, than the chained hash tables in the
standard library. Most of its member functions have their usual semantics for
a standard-conforming container.

The `emplace()` function works differently for `HashMap` compared to standard
containers. Because insertion requires the key to be known in advance, the
first argument is used as the key, and the remaining arguments are used to
construct the mapped value. This does not apply to `HashSet`, where
`emplace()` works normally.

The `reserve()` function enlarges (if necessary) and rehashes the table to
ensure that at least `n` entries (the current `size()` plus future insertions)
can be reached without forcing a rehash. The `free_space()` function indicates
how many insertions can be made before a rehash is forced.

The `table_size()` function returns the current total size of the internal
array. An automatic rehash will happen when the number of occupied entries
(including tombstones) reaches 3/4 of this.
