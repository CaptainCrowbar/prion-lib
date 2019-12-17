# Topological Order #

By Ross Smith

* `#include "rs-core/topological-order.hpp"`

## Contents ##

[TOC]

## Exceptions ##

* `class` **`TopologicalOrderError`**`: public std::runtime_error`
    * `class` **`TopologicalOrderCycle`**`: public TopologicalOrderError`
    * `class` **`TopologicalOrderEmpty`**`: public TopologicalOrderError`

Exceptions reporting errors in topological order operations.

## Class TopologicalOrder ##

* `template <typename T, typename Compare = std::less<T>> class` **`TopologicalOrder`**

A container that sorts its elements according to a defined topological order,
given a set of _(before,after)_ relations. The comparison object is used only
to sort elements within a subset; it does not participate in the topological
relation.

* `using TopologicalOrder::`**`compare_type`** `= Compare`
* `using TopologicalOrder::`**`value_type`** `= T`

Member types.

* `TopologicalOrder::`**`TopologicalOrder`**`()`
* `explicit TopologicalOrder::`**`TopologicalOrder`**`(Compare c)`
* `TopologicalOrder::`**`~TopologicalOrder`**`() noexcept`
* `TopologicalOrder::`**`TopologicalOrder`**`(const TopologicalOrder& t)`
* `TopologicalOrder::`**`TopologicalOrder`**`(TopologicalOrder&& t) noexcept`
* `TopologicalOrder& TopologicalOrder::`**`operator=`**`(const TopologicalOrder& t)`
* `TopologicalOrder& TopologicalOrder::`**`operator=`**`(TopologicalOrder&& t) noexcept`

Life cycle functions.

* `void TopologicalOrder::`**`clear`**`() noexcept`

Remove all elements from the container.

* `Compare TopologicalOrder::`**`comp`**`() const`

Returns a copy of the comparison predicate.

* `bool TopologicalOrder::`**`empty`**`() const noexcept`

True if the container is empty.

* `bool TopologicalOrder::`**`erase`**`(const T& t)`

Erase a specific element from the container. This will do nothing if the
element was not present.

* `T TopologicalOrder::`**`front`**`() const`
* `std::vector<T> TopologicalOrder::`**`front_set`**`() const`
* `T TopologicalOrder::`**`back`**`() const`
* `std::vector<T> TopologicalOrder::`**`back_set`**`() const`

Query the current front set (elements that never appear on the right of an
ordering relation) or back set (elements that never appear on the left), or
the first element in each set. The elements in each returned list will be
ordered according to the container's comparison predicate. An element that has
no ordering relations with any other element will appear in both sets. The
`front()` and `back()` functions will throw `TopologicalOrderEmpty` if the
container is empty, or `TopologicalOrderCycle` if a cycle is detected; the set
functions will throw if a cycle is detected but will simply return an empty
set if the container is empty. Not all cycles will be detected by these
functions (a cycle that involves only interior elements, while still leaving
well defined front and back sets, will not be detected here).

* `bool TopologicalOrder::`**`has`**`(const T& t) const`

True if the element is in the container.

* `template <typename... Args> void TopologicalOrder::`**`insert`**`(const T& t, const Args&... args)`
* `template <typename R> void TopologicalOrder::`**`insert_n`**`(const R& r)`

Insert one or more elements. If only one element is inserted, it has no
implied ordering relations with any other elements. If multiple elements are
listed, an ordering relation is implied between each element and all elements
after it in the list. In all insert functions, implied relations between an
element and itself are ignored.

* `template <typename R> void TopologicalOrder::`**`insert_1n`**`(const T& t1, const R& r2)`
* `void TopologicalOrder::`**`insert_1n`**`(const T& t1, std::initializer_list<T> r2)`
* `template <typename R> void TopologicalOrder::`**`insert_n1`**`(const R& r1, const T& t2)`
* `void TopologicalOrder::`**`insert_n1`**`(std::initializer_list<T> r1, const T& t2)`
* `template <typename R1, typename R2> void TopologicalOrder::`**`insert_mn`**`(const R1& r1, const R2& r2)`
* `void TopologicalOrder::`**`insert_mn`**`(std::initializer_list<T> r1, std::initializer_list<T> r2)`

Insert one or more pairs of elements, with ordering relations between the left
and right arguments. The `insert_1n()` function creates an ordering relation
between the first argument and every element of the second range;
`insert_n1()` creates an ordering relation between every element of the first
range and the second element; `insert_mn()` creates an ordering relation
between every pair in the cross product of the two ranges.

* `bool TopologicalOrder::`**`is_front`**`(const T& t) const`
* `bool TopologicalOrder::`**`is_back`**`(const T& t) const`

True if the give element is part of the current front or back set.

* `T TopologicalOrder::`**`pop_front`**`()`
* `std::vector<T> TopologicalOrder::`**`pop_front_set`**`()`
* `T TopologicalOrder::`**`pop_back`**`()`
* `std::vector<T> TopologicalOrder::`**`pop_back_set`**`()`

Remove one or all elements of the current front or back set. These will throw
`TopologicalOrderCycle` or `TopologicalOrderEmpty` under the same
circumstances as the corresponding front or back element or set functions.

* `size_t TopologicalOrder::`**`size`**`() const noexcept`

Returns the number of elements in the container.
