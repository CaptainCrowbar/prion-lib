# Intervals and Interval Based Containers #

By Ross Smith

* `#include "rs-core/interval.hpp"`

## Contents ##

[TOC]

## Supporting types ##

* `enum class` **`IntervalBound`**
    * `IntervalBound::`**`empty`** `= 0`    _[the interval is empty]_
    * `IntervalBound::`**`open`** `= 1`     _[the interval does not include the boundary value]_
    * `IntervalBound::`**`closed`** `= 2`   _[the interval includes the boundary value]_
    * `IntervalBound::`**`unbound`** `= 3`  _[the interval is unbounded in this direction]_

Indicates what kind of bound the interval has. An `Interval` object contains
two of these values, one for each bound. Some combinations are impossible; if
one bound is `empty`, the other must also be `empty`.

* `enum class` **`IntervalCategory`**
    * `IntervalCategory::`**`none`** `= 0`        _[not usable in an interval]_
    * `IntervalCategory::`**`ordered`** `= 1`     _[ordered but not an arithmetic type, e.g. string]_
    * `IntervalCategory::`**`integral`** `= 2`    _[incrementable and not continuous, e.g. integer]_
    * `IntervalCategory::`**`continuous`** `= 3`  _[models a continuous arithmetic type, e.g. floating point]_
* `template <typename T> struct` **`IntervalTraits`**
    * `static constexpr IntervalCategory IntervalTraits::`**`category`**
* `template <typename T> constexpr IntervalCategory` **`interval_category`** `= IntervalTraits<T>::category`

Indicates the behaviour of a particular underlying type with respect to
intervals. To be usable as the template parameter to `Interval` and related
types, a type must at the very least be default constructible, copyable, and
equality and less-than comparable. Additional type properties enable
additional interval properties.

The `IntervalTraits` or `interval_category` templates determine the category
of a given type, according to the following algorithm:

<!-- TEXT -->
* _if `T` is not default constructible, copy constructible, equality comparable, and less-than comparable_
    * _category is `none`_
* _else if `T` is a primitive integer type other than `bool`_
    * _category is `integral`_
* _else if `T` is a primitive floating point type_
    * _category is `continuous`_
* _else if `T` has binary `+ - * /` operators but not `%`_
    * _category is `continuous`_
* _else if `T` has unary `++` and `--` operators_
    * _category is `integral`_
* _else_
    * _category is `ordered`_

The interval category for a user defined type can be controlled by
specializing `IntervalCategory` for that type, if the default algorithm does
not give the appropriate category.

* `enum class` **`IntervalMatch`**
    * `IntervalMatch::`**`empty`** `= -2`  _[the interval is empty]_
    * `IntervalMatch::`**`low`** `= -1`    _[the value is less than the interval's lower bound]_
    * `IntervalMatch::`**`ok`** `= 0`      _[the value is an element of the interval]_
    * `IntervalMatch::`**`high`** `= 1`    _[the value is greater than the interval's upper bound]_

The result of the `Interval::match(t)` method, indicating the relationship of
a specific value to the interval.

* `enum class` **`IntervalOrder`**
    * _[see below for the list of values]_

The result of the `Interval::order(interval)` method, indicating the
relationship of one interval to another. In the table below, the values in the
first column are the possible results of calling `A.order(B)`, where `A` and
`B` are intervals. In the relationship diagrams, `A` is the white interval,
`B` is the black interval, with the underlying type running horizontally from
left to right. A negative value indicates that `A` will compare
lexicographically less than `B`; a positive value indicates that `A` will
compare greater than `B`.

Name                                      | Value  | Picture                                   | Description
----                                      | -----  | -------                                   | -----------
`IntervalOrder::`**`b_only`**             | -7     | ![bbb](../graphics/interval-7.png)        | A is empty, B is not
`IntervalOrder::`**`a_below_b`**          | -6     | ![aaa...bbb](../graphics/interval-6.png)  | A's upper bound is less than B's lower bound, with a gap
`IntervalOrder::`**`a_touches_b`**        | -5     | ![aaabbb](../graphics/interval-5.png)     | A's upper bound is less than B's lower bound, with no gap
`IntervalOrder::`**`a_overlaps_b`**       | -4     | ![aaa***bbb](../graphics/interval-4.png)  | A's upper bound overlaps B's lower bound
`IntervalOrder::`**`a_extends_below_b`**  | -3     | ![aaa***](../graphics/interval-3.png)     | B is a subset of A, with the same upper bound
`IntervalOrder::`**`a_encloses_b`**       | -2     | ![aaa***aaa](../graphics/interval-2.png)  | B is a subset of A, matching neither bound
`IntervalOrder::`**`b_extends_above_a`**  | -1     | ![***bbb](../graphics/interval-1.png)     | A is a subset of B, with the same lower bound
`IntervalOrder::`**`equal`**              | 0      | ![***](../graphics/interval+0.png)        | A and B are the same (this includes the case where both are empty)
`IntervalOrder::`**`a_extends_above_b`**  | 1      | ![***aaa](../graphics/interval+1.png)     | B is a subset of A, with the same lower bound
`IntervalOrder::`**`b_encloses_a`**       | 2      | ![bbb***bbb](../graphics/interval+2.png)  | A is a subset of B, matching neither bound
`IntervalOrder::`**`b_extends_below_a`**  | 3      | ![bbb***](../graphics/interval+3.png)     | A is a subset of B, with the same upper bound
`IntervalOrder::`**`b_overlaps_a`**       | 4      | ![bbb***aaa](../graphics/interval+4.png)  | B's upper bound overlaps A's lower bound
`IntervalOrder::`**`b_touches_a`**        | 5      | ![bbbaaa](../graphics/interval+5.png)     | B's upper bound is less than A's lower bound, with no gap
`IntervalOrder::`**`b_below_a`**          | 6      | ![bbb...aaa](../graphics/interval+6.png)  | B's upper bound is less than A's lower bound, with a gap
`IntervalOrder::`**`a_only`**             | 7      | ![aaa](../graphics/interval+7.png)        | B is empty, A is not

## Interval class ##

* `template <typename T> class` **`Interval`**

A class representing an interval over the underlying type `T`. This template
is only defined for underlying types for which the interval category is not
`none`.

The interval may be a finite interval with upper and lower bounds, bounded on
one side but unbounded on the other, an empty interval (containing no values),
or a universal interval (containing all values of the underlying type).
Depending on the properties of the underlying type, the interval class may
also distinguish between open, closed, and half-open intervals, indicating
whether or not the interval includes neither, both, or one of its bounds. For
integer-like types the open-closed distinction is not made, since an interval
specified using open bounds can be trivially turned into a closed interval by
incrementing or decrementing the bounds.

* `using Interval::`**`value_type`** `= T`

The underlying domain type.

* `class Interval::`**`iterator`**

An iterator over all values in the interval. This is defined only for
integer-like underlying types (i.e. types where the `interval_category` is
`integral`). This is a `const` iterator, and is random access if the
underlying type has addition and subtraction operators, otherwise
bidirectional.

* `static constexpr IntervalCategory Interval::`**`category`** `= interval_category<T>`

The interval category of the underlying type.

* `Interval::`**`Interval`**`()`
* `Interval::`**`Interval`**`(const T& t)`
* `Interval::`**`Interval`**`(const T& t, IntervalBound l, IntervalBound r)`
* `Interval::`**`Interval`**`(const T& min, const T& max, IntervalBound lr = IntervalBound::closed)`
* `Interval::`**`Interval`**`(const T& min, const T& max, IntervalBound l, IntervalBound r)`
* `Interval::`**`Interval`**`(const T& min, const T& max, Uview mode)`

Constructors. The default constructor creates an empty interval. The second
constructor creates an interval containing exactly one value, and provides an
implicit conversion from `T` to `Interval<T>`.

The next three constructors take the two boundary values and the two boundary
types; either can be reduced to a single value for both bounds. These will
automatically adjust the interval properties to produce a consistent result:
for integral types, open bounds will be turned into closed ones, and for all
types, the interval will be turned into an empty interval if the upper bound
is less than the lower bound (less than or equal if one or both is an open
bound). If one or both bounds is `empty` or `unbound`, the corresponding `min`
or `max` value is ignored. These constructors will throw
`std::invalid_argument` if one boundary type is `empty` but the other is not.

The sixth constructor also takes explicit boundary value and boundary type
arguments, but the boundary types are specified using a string as shorthand:

Mode    | Left bound  | Right bound  | Description
----    | ----------  | -----------  | -----------
`"()"`  | `open`      | `open`       | Open interval
`"(]"`  | `open`      | `closed`     | Half-open interval, closed on the right
`"[)"`  | `closed`    | `open`       | Half-open interval, closed on the left
`"[]"`  | `closed`    | `closed`     | Closed interval
`"<"`   | `unbound`   | `open`       | Open interval, bounded above (first argument is ignored)
`"<="`  | `unbound`   | `closed`     | Closed interval, bounded above (first argument is ignored)
`">"`   | `open`      | `unbound`    | Open interval, bounded below (second argument is ignored)
`">="`  | `closed`    | `unbound`    | Closed interval, bounded below (second argument is ignored)

This constructor will throw `std::invalid_argument` if the mode string is not
one of these.

A universal interval (containing all possible values of `T`) can be
constructed using the `all()` static member function (see below).

* `Interval::`**`Interval`**`(const Interval& in)`
* `Interval::`**`Interval`**`(Interval&& in) noexcept`
* `Interval::`**`~Interval`**`() noexcept`
* `Interval& Interval::`**`operator=`**`(const Interval& in)`
* `Interval& Interval::`**`operator=`**`(Interval&& in) noexcept`

Other life cycle functions.

* `explicit Interval::`**`operator bool`**`() const noexcept`

Explicit conversion to `bool`; true if the interval is not empty.

* `const T& Interval::`**`min`**`() const noexcept`
* `const T& Interval::`**`max`**`() const noexcept`
* `IntervalBound Interval::`**`left`**`() const noexcept`
* `IntervalBound Interval::`**`right`**`() const noexcept`

These return the boundary values and boundary types. If either end of the
interval is unbounded (boundary type `empty` or `unbound`), the value of the
corresponding `min()` or `max()` function is unspecified.

* `bool Interval::`**`is_empty`**`() const noexcept` _[true if the interval is empty]_
* `bool Interval::`**`is_single`**`() const noexcept` _[true if the interval contains exactly one value ]_
* `bool Interval::`**`is_finite`**`() const noexcept` _[true if the interval is non-empty and bounded on both sides]_
* `bool Interval::`**`is_infinite`**`() const noexcept` _[true if the interval is non-empty and unbound on at least one side]_
* `bool Interval::`**`is_universal`**`() const noexcept` _[true if the interval contains all values of the underlying type]_
* `bool Interval::`**`is_left_bounded`**`() const noexcept` _[true if the interval is non-empty and bounded below]_
* `bool Interval::`**`is_left_closed`**`() const noexcept` _[true if the interval is non-empty and includes its lower bound]_
* `bool Interval::`**`is_left_open`**`() const noexcept` _[true if the interval is non-empty and does not include its lower bound]_
* `bool Interval::`**`is_right_bounded`**`() const noexcept` _[true if the interval is non-empty and bounded above]_
* `bool Interval::`**`is_right_closed`**`() const noexcept` _[true if the interval is non-empty and includes its upper bound]_
* `bool Interval::`**`is_right_open`**`() const noexcept` _[true if the interval is non-empty and does not include its upper bound]_

Interval properties.

* `Interval::iterator Interval::`**`begin`**`() const`
* `Interval::iterator Interval::`**`end`**`() const`

Iterators over the values in an interval. These are defined only for
integer-like underlying types (interval category `integral`). If either of the
bounds does not exist (bound type `empty` or `unbound`), behaviour is
undefined if the corresponding `begin ()` or `end()` function is called.

* `[see below] Interval::`**`size`**`() const`

Returns the length of the interval. For `integral` types, the return type is
`size_t`, and `size()` returns the number of values in the interval, or `npos`
if one or both bounds is `unbound`.

For `continuous` types, the return type is `T`, and `size()` returns the
difference between the upper and lower bounds, without regard to whether they
are open or closed bounds. Behaviour is undefined if one or both of the bounds
is `unbound`.

For `ordered` types, the `size()` function is not defined.

* `bool Interval::`**`contains`**`(const T& t) const`
* `IntervalMatch Interval::`**`match`**`(const T& t) const`
* `bool Interval::`**`operator()`**`(const T& t) const`

These determine the relationship between a specific value and an interval. The
`match()` function returns a value of the `IntervalMatch` enumeration
indicating the precise relationship, while `contains()` simply indicates
whether the value is an element of the interval. The function call operator is
equivalent to `contains()`

* `IntervalSet<T> Interval::`**`inverse`**`() const`
* `IntervalSet<T> Interval::`**`operator~`**`() const`

These return the inverse interval (the set of all values that are not in the
interval). The inverse may in general consist of more than one interval, so
the return type is `IntervalSet` (described below), not `Interval`.

* `IntervalOrder Interval::`**`order`**`(const Interval& b) const`

Determines the relationship between two intervals. See the description of the
`IntervalOrder` enumeration above for the interpretation of the result.

* `int Interval::`**`compare`**`(const Interval& b) const noexcept`

Performs a three-way lexicographical ordering of two intervals, according to
`T`'s less-than operator. The return value is a positive integer if `*this>b`,
a negative integer if `*this<b`, or zero if `*this==b`. An empty interval
compares less than any non-empty interval.

* `bool Interval::`**`includes`**`(const Interval& b) const` _[true if `b` is a subset of `*this`]_
* `bool Interval::`**`overlaps`**`(const Interval& b) const` _[true if `*this` and `b` have any values in common]_
* `bool Interval::`**`touches`**`(const Interval& b) const` _[true if there are no values between the two intervals]_

Query specific relationships between two intervals.

* `Interval Interval::`**`envelope`**`(const Interval& b) const`

Returns the smallest interval that contains both argument intervals (`*this`
and `b`). This is similar to `set_union()`, except that if the intervals are
both non-empty and are disjoint, this will also contain the interval between
them.

* `Interval Interval::`**`set_intersection`**`(const Interval& b) const`
* `IntervalSet<T> Interval::`**`set_union`**`(const Interval& b) const`
* `IntervalSet<T> Interval::`**`set_difference`**`(const Interval& b) const`
* `IntervalSet<T> Interval::`**`set_symmetric_difference`**`(const Interval& b) const`
* `Interval& Interval::`**`operator&=`**`(const Interval& b)` _[set intersection]_
* `Interval` **`operator&`**`(const Interval& a, const Interval& b)` _[set intersection]_
* `IntervalSet<T>` **`operator|`**`(const Interval& a, const Interval& b)` _[set union]_
* `IntervalSet<T>` **`operator-`**`(const Interval& a, const Interval& b)` _[set difference]_
* `IntervalSet<T>` **`operator^`**`(const Interval& a, const Interval& b)` _[set symmetric difference]_

Set operations on two intervals. In general the result of a set operation may
consist of more than one interval, so most of these return an `IntervalSet`
rather than an `Interval`.

* `bool Interval::`**`parse`**`(Uview view) noexcept`
* `Ustring Interval::`**`str`**`() const`
* `bool` **`from_str`**`(Uview view, Interval& in)`
* `Ustring` **`to_str`**`(const Interval& in)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Interval& in)`

Conversions between intervals and strings. These are defined only if the
corresponding `from_str()` or `to_str()` function is defined for `T`. They do
not attempt to cover every possible serialization format; if `T`'s string
format contains characters that may be used as punctuation in the formats
listed below, parsing a string may not work, while generating one may produce
a string that cannot be parsed.

In the table below, the format in the first column is the format generated by
`to_str()` and related functions; the formats in the alternate column will
also be recognized by `from_str()` and related functions, but will not be
generated. `A` and `B` represent formatted values of `T`. If `T`'s
`from_str()` or `to_str()` fail, `parse()` will return false, while the other
functions will propagate the exceptions thrown by the corresponding functions
on `T`.

Format   | Alternate            | Description
------   | ---------            | -----------
`{}`     | _empty string_       | Empty interval
`*`      |                      | Universal interval (contains every value)
`A`      | `=A`                 | Single value
`(A,B)`  |                      | Open interval
`(A,B]`  |                      | Half-open interval, closed on the right
`[A,B)`  | `A..<B`              | Half-open interval, closed on the left
`[A,B]`  | `A..B` _or_ `A...B`  | Closed interval
`<A`     |                      | Open interval, bounded above
`<=A`    | `A-`                 | Closed interval, bounded above
`>A`     |                      | Open interval, bounded below
`>=A`    | `A+`                 | Closed interval, bounded below

* `void Interval::`**`convert_from_json`**`(const json& j)`
* `json Interval::`**`convert_to_json`**`() const`
* `void` **`from_json`**`(const json& j, Interval& in)`
* `void` **`to_json`**`(json& j, const Interval& in)`

Conversions between `Interval<T>` and `nlohmann::json`. These are only defined
if the corresponding functions are defined for `T`.

* `size_t Interval::`**`hash`**`() const noexcept`
* `class` **`std::hash<Interval<T>>`**

Hash function for an interval.

* `void Interval::`**`swap`**`(Interval& in) noexcept`
* `void` **`swap`**`(Interval& a, Interval& b)`

Swap two intervals.

* `static Interval Interval::`**`all`**`() noexcept`

Constructs a universal interval, i.e. an unbounded interval containing all
possible values of `T`.

* `bool` **`operator==`**`(const Interval& a, const Interval& b) noexcept`
* `bool` **`operator!=`**`(const Interval& a, const Interval& b) noexcept`
* `bool` **`operator<`**`(const Interval& a, const Interval& b) noexcept`
* `bool` **`operator>`**`(const Interval& a, const Interval& b) noexcept`
* `bool` **`operator<=`**`(const Interval& a, const Interval& b) noexcept`
* `bool` **`operator>=`**`(const Interval& a, const Interval& b) noexcept`

Lexicographical comparison operators. These call `T`'s equality and less-than
operators.

## Interval set ##

* `template <typename T> class` **`IntervalSet`**

This class represents a subset of `T`'s domain as a set of intervals.

* `using IntervalSet::`**`value_type`** `= T`
* `using IntervalSet::`**`interval_type`** `= Interval<T>`
* `class IntervalSet::`**`iterator`** _[bidirectional const iterator]_

Member types.

* `static constexpr IntervalCategory IntervalSet::`**`category`** `= interval_category<T>`

The underlying value type's interval category.

* `IntervalSet::`**`IntervalSet`**`()`
* `IntervalSet::`**`IntervalSet`**`(const T& t)`
* `IntervalSet::`**`IntervalSet`**`(const Interval<T>& in)`
* `IntervalSet::`**`IntervalSet`**`(std::initializer_list<Interval<T>> list)`
* `IntervalSet::`**`IntervalSet`**`(const IntervalSet& set)`
* `IntervalSet::`**`IntervalSet`**`(IntervalSet&& set) noexcept`
* `IntervalSet::`**`~IntervalSet`**`() noexcept`
* `IntervalSet& IntervalSet::`**`operator=`**`(const IntervalSet& set)`
* `IntervalSet& IntervalSet::`**`operator=`**`(IntervalSet&& set) noexcept`

Life cycle functions. When a set is constructed from a list of intervals or
values, the intervals are ordered lexicographically, and adjacent intervals
are merged when they touch or overlap.

* `IntervalSet::iterator IntervalSet::`**`begin`**`() const noexcept`
* `IntervalSet::iterator IntervalSet::`**`end`**`() const noexcept`

Iterators over the intervals in the set.

* `bool IntervalSet::`**`empty`**`() const noexcept`

True if the set is empty.

* `size_t IntervalSet::`**`size`**`() const noexcept`

Returns the number of intervals in the set.

* `bool IntervalSet::`**`contains`**`(const T& t) const`
* `bool IntervalSet::`**`operator()`**`(const T& t) const`

True if the value is an element of any of the intervals in the set.

* `void IntervalSet::`**`clear`**`() noexcept`

Clears all intervals from the set, leaving it empty.

* `void IntervalSet::`**`insert`**`(const Interval<T>& in)`

Adds a new interval to the set. Adjacent intervals are merged when they touch
or overlap.

* `void IntervalSet::`**`erase`**`(const Interval<T>& in)`

Removes an interval from the set. Intervals in the set that overlap this
interval will be modified or removed as necessary. This will have no effect if
this interval does not overlap any existing interval in the set.

* `IntervalSet IntervalSet::`**`inverse`**`() const`
* `IntervalSet IntervalSet::`**`operator~`**`() const`

Returns the complement of the set, i.e. a new set whose member intervals
contain every value of `T` that is not in this set.

* `IntervalSet IntervalSet::`**`set_intersection`**`(const IntervalSet& b) const`
* `IntervalSet IntervalSet::`**`set_union`**`(const IntervalSet& b) const`
* `IntervalSet IntervalSet::`**`set_difference`**`(const IntervalSet& b) const`
* `IntervalSet IntervalSet::`**`set_symmetric_difference`**`(const IntervalSet& b) const`
* `IntervalSet& IntervalSet::`**`operator&=`**`(const IntervalSet& b)` _[set intersection]_
* `IntervalSet& IntervalSet::`**`operator|=`**`(const IntervalSet& b)` _[set union]_
* `IntervalSet& IntervalSet::`**`operator-=`**`(const IntervalSet& b)` _[set difference]_
* `IntervalSet& IntervalSet::`**`operator^=`**`(const IntervalSet& b)` _[set symmetric difference]_
* `IntervalSet` **`operator&`**`(const IntervalSet& a, const IntervalSet& b)` _[set intersection]_
* `IntervalSet` **`operator|`**`(const IntervalSet& a, const IntervalSet& b)` _[set union]_
* `IntervalSet` **`operator-`**`(const IntervalSet& a, const IntervalSet& b)` _[set difference]_
* `IntervalSet` **`operator^`**`(const IntervalSet& a, const IntervalSet& b)` _[set symmetric difference]_

Set operations.

* `bool IntervalSet::`**`parse`**`(Uview view) noexcept`
* `Ustring IntervalSet::`**`str`**`() const`
* `bool` **`from_str`**`(Uview view, IntervalSet& set)`
* `Ustring` **`to_str`**`(const IntervalSet& set)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const IntervalSet& set)`

Interval set parsing and formatting functions. An interval set is expected to
be in the format `"{A,B,C,...}"`, where `A`, `B`, `C`, etc are intervals or
values of `T`. A single value of `T` or `Interval<T>` will also be recognized,
as will an empty set represented by an empty string or `"{}"`. These call the
corresponding functions on `Interval<T>`, which in turn call the corresponding
functions on `T`, and will propagate any exceptions (or false return values)
from those. As with the string functions for `Interval`, results may be
inconsistent, or not round-trippable, if the format of a serialized `T`
contains any characters that may be mistaken for punctuation in the set
format.

* `size_t IntervalSet::`**`hash`**`() const noexcept`
* `class` **`std::hash<IntervalSet>`**

Hash function for an interval set.

* `void IntervalSet::`**`swap`**`(IntervalSet& set) noexcept`
* `void` **`swap`**`(IntervalSet& a, IntervalSet& b)`

Swap two interval sets.

* `bool` **`operator==`**`(const IntervalSet& a, const IntervalSet& b) noexcept`
* `bool` **`operator!=`**`(const IntervalSet& a, const IntervalSet& b) noexcept`
* `bool` **`operator<`**`(const IntervalSet& a, const IntervalSet& b) noexcept`
* `bool` **`operator>`**`(const IntervalSet& a, const IntervalSet& b) noexcept`
* `bool` **`operator<=`**`(const IntervalSet& a, const IntervalSet& b) noexcept`
* `bool` **`operator>=`**`(const IntervalSet& a, const IntervalSet& b) noexcept`

Lexicographical comparison operators. These call `T`'s equality and less-than
operators.

## Interval map ##

* `template <typename K, typename T> class` **`IntervalMap`**

A map from a set of intervals over `K` to values of `T`. The `IntervalMap`
object also contains a default value of `T` that will be returned when a key
is not a member of any of the intervals in the map.

* `using IntervalMap::`**`key_type`** `= K`
* `using IntervalMap::`**`interval_type`** `= Interval<K>`
* `using IntervalMap::`**`mapped_type`** `= T`
* `using IntervalMap::`**`value_type`** `= std::pair<const Interval<K>, T>`
* `class IntervalMap::`**`iterator`** _[bidirectional const iterator]_

Member types.

* `static constexpr IntervalCategory IntervalMap::`**`category`** `= interval_category<K>`

The underlying value type's interval category.

* `IntervalMap::`**`IntervalMap`**`()`
* `explicit IntervalMap::`**`IntervalMap`**`(const T& defval)`
* `IntervalMap::`**`IntervalMap`**`(std::initializer_list<value_type> list)`
* `IntervalMap::`**`IntervalMap`**`(const IntervalMap& map)`
* `IntervalMap::`**`IntervalMap`**`(IntervalMap&& map) noexcept`
* `IntervalMap::`**`~IntervalMap`**`() noexcept`
* `IntervalMap& IntervalMap::`**`operator=`**`(const IntervalMap& map)`
* `IntervalMap& IntervalMap::`**`operator=`**`(IntervalMap&& map) noexcept`

Life cycle functions. An optional default value can be provided; if none is
provided, `T`'s default constructor is used. When a map is constructed from a
list of `(interval,value)` pairs, the intervals are ordered lexicographically,
and adjacent intervals are merged when they touch or overlap and have the same
mapped value. When two intervals overlap but do not have the same mapped
value, later entries in the initializer list will overwrite earlier ones.

* `const T& IntervalMap::`**`operator[]`**`(const K& key) const`

Returns the mapped value corresponding to the interval containing the given
key, or the default value if no interval contains the key.

* `IntervalMap::iterator IntervalMap::`**`begin`**`() const noexcept`
* `IntervalMap::iterator IntervalMap::`**`end`**`() const noexcept`

Iterators over the map's list of `(interval,value)` pairs.

* `bool IntervalMap::`**`empty`**`() const noexcept`

True if the map is empty.

* `size_t IntervalMap::`**`size`**`() const noexcept`

Returns the number of intervals in the map.

* `const T& IntervalMap::`**`default_value`**`() const noexcept`
* `void IntervalMap::`**`default_value`**`(const T& defval)`

Query or set the default value.

* `bool IntervalMap::`**`contains`**`(const K& key) const`

True if one of the intervals in the map contains the key.

* `IntervalMap::iterator IntervalMap::`**`find`**`(const K& key) const`

Returns an iterator pointing to the interval containing the given key, or
`end()` if no such interval exists.

* `IntervalMap::iterator IntervalMap::`**`lower_bound`**`(const K& key) const`
* `IntervalMap::iterator IntervalMap::`**`upper_bound`**`(const K& key) const`

If the key is contained in one of the intervals in the map, `lower_bound()`
returns an iterator pointing to that interval, and `upper_bound()` returns the
next iterator. If not, both functions return the iterator pointing to the
first interval after the given key, or `end()` if no such interval exists.

* `void IntervalMap::`**`clear`**`() noexcept`
* `void IntervalMap::`**`reset`**`(const T& defval = {})`

These clear all `(interval,value)` pairs from the map. The `clear()` function
does not change the default value, while `reset()` also changes it.

* `void IntervalMap::`**`insert`**`(const Interval<K>& in, const T& t)`
* `void IntervalMap::`**`insert`**`(const value_type& v)`

Adds a new `(interval,value)` pair to the map. Intervals in the map that
overlap this interval will be modified or removed as necessary.

* `void IntervalMap::`**`erase`**`(const Interval<K>& in)`

Removes an interval from the map. Intervals in the map that overlap this
interval will be modified or removed as necessary. This will have no effect if
this interval does not overlap any existing interval in the map.

* `bool IntervalMap::`**`parse`**`(Uview view) noexcept`
* `Ustring IntervalMap::`**`str`**`() const`
* `bool` **`from_str`**`(Uview view, IntervalMap& set)`
* `Ustring` **`to_str`**`(const IntervalMap& set)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const IntervalMap& set)`

Interval map parsing and formatting functions. An interval map is expected to
be in the format `"{A:X,B:Y,C:Z,...}"`, where `A`, `B`, `C`, etc are intervals
or values of `K`, and `X`, `Y`, `Z`, etc are values of `T`. An empty map can
be represented by an empty string or `"{}"`. The default value does not
participate in serialization; a map constructed from a string will always have
its default value set to `T()`.

These call the corresponding functions on `Interval<K>` (which in turn call
the corresponding functions on `K`) and `T`, and will propagate any exceptions
(or false return values) from those. As with the string functions for
`Interval` and `IntervalSet`, results may be inconsistent, or not
round-trippable, if the format of a serialized `K` or `T` contains any
characters that may be mistaken for punctuation in the map format.

* `size_t IntervalMap::`**`hash`**`() const noexcept`
* `class` **`std::hash<IntervalMap>`**

Hash function for an interval map.

* `void IntervalMap::`**`swap`**`(IntervalMap& map) noexcept`
* `void` **`swap`**`(IntervalMap& a, IntervalMap& b)`

Swap two interval maps.

* `bool` **`operator==`**`(const IntervalMap& a, const IntervalMap& b) noexcept`
* `bool` **`operator!=`**`(const IntervalMap& a, const IntervalMap& b) noexcept`
* `bool` **`operator<`**`(const IntervalMap& a, const IntervalMap& b) noexcept`
* `bool` **`operator>`**`(const IntervalMap& a, const IntervalMap& b) noexcept`
* `bool` **`operator<=`**`(const IntervalMap& a, const IntervalMap& b) noexcept`
* `bool` **`operator>=`**`(const IntervalMap& a, const IntervalMap& b) noexcept`

Lexicographical comparison operators. These call `T`'s equality and less-than
operators.
