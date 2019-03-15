# Algorithms #

By Ross Smith

* `#include "rs-core/algorithm.hpp"`

## Contents ##

[TOC]

## General algorithms ##

### Difference algorithm ###

* `template <typename RandomAccessRange> struct` **`DiffEntry`**
    * `using DiffEntry::`**`iterator`** `= RangeIterator<const RandomAccessRange>`
    * `using DiffEntry::`**`subrange`** `= Irange<iterator>`
    * `DiffEntry::subrange DiffEntry::`**`del`**
    * `DiffEntry::subrange DiffEntry::`**`ins`**
* `template <typename RandomAccessRange> using` **`DiffList`** `= std::vector<DiffEntry<RandomAccessRange>>`
* `template <typename RandomAccessRange> DiffList<RandomAccessRange>` **`diff`**`(const RandomAccessRange& lhs, const RandomAccessRange& rhs)`
* `template <typename RandomAccessRange, typename EqualityPredicate> DiffList<RandomAccessRange>` **`diff`**`(const RandomAccessRange& lhs, const RandomAccessRange& rhs, EqualityPredicate eq)`

This is an implementation of the algorithm described in [Eugene Myers' 1986
paper](http://xmailserver.org/diff2.pdf). The return value is a list of diffs,
each consisting of two pairs of iterators: the `del` member is a subrange of
`lhs` indicating which elements have been removed, and the `ins` member is a
subrange of `rhs` indicating which elements have been inserted in the same
location; at least one subrange in each diff entry will be non-empty.

Complexity: `O((n1+n2)d)`, where `n1` and `n2` are the lengths of the input
ranges and `d` is the number of differences.

### Edit distance ###

* `template <typename ForwardRange1, typename ForwardRange2> size_t` **`edit_distance`**`(const ForwardRange1& range1, const ForwardRange2& range2)`
* `template <typename ForwardRange1, typename ForwardRange2, typename T> T` **`edit_distance`**`(const ForwardRange1& range1, const ForwardRange2& range2, T ins, T del, T sub)`

These return the edit distance (Levenshtein distance) between two ranges,
based on the number of insertions, deletions, and substitutions required to
transform one range into the other. By default, each operation is given a
weight of 1; optionally, explicit weights can be given to each operation. The
weight type (`T`) must be an arithmetic type. Behaviour is undefined if any of
the weights are negative.

Complexity: Quadratic.

### Find optimum ###

* `template <typename ForwardRange, typename UnaryFunction> [range iterator]` **`find_optimum`**`(ForwardRange& range, UnaryFunction f)`
* `template <typename ForwardRange, typename UnaryFunction, typename Compare> [range iterator]` **`find_optimum`**`(ForwardRange& range, UnaryFunction f, Compare c)`

These return an iterator identifying the range element for which `f(x)` has
the minimum value, according to the given comparison function. The comparison
function is expected to have less-than semantics, and defaults to
`std::less<>()`; use `std::greater<>()` to get the maximum value of `f(x)`
instead of the minimum. If there is more than one optimum element, the first
one will be returned. These will return an end iterator only if the range is
empty.

Complexity: Linear.

### Order by index ###

* `template <typename RandomAccessRange1, typename RandomAccessRange2> void` **`order_by_index`**`(RandomAccessRange1& range, RandomAccessRange2& index)`

Rearrange the elements of the first range according to the list of index
values in the second, simultaneously ordering the index range. The value type
of the index range must be an integer type. Behaviour is undefined if the two
ranges have different lengths, or if the contents of the index range are not a
permutation of the integers from 0 to `n-1` (where `n` is the length of the
first range).

## Paired range operations ##

For all of these algorithms, if the two ranges supplied have different
lengths, the length of the shorter range is used; the excess elements in the
longer range are ignored.

### Paired for each ###

* `template <typename InputRange1, typename InputRange2, typename BinaryFunction> void` **`paired_for_each`**`(InputRange1& range1, InputRange2& range2, BinaryFunction f)`

Calls `f(x,y)` for each pair of corresponding elements in the two ranges.

### Paired sort ###

* `template <typename RandomAccessRange1, typename RandomAccessRange2> void` **`paired_sort`**`(RandomAccessRange1& range1, RandomAccessRange2& range2)`
* `template <typename RandomAccessRange1, typename RandomAccessRange2, typename Compare> void` **`paired_sort`**`(RandomAccessRange1& range1, RandomAccessRange2& range2, Compare comp)`
* `template <typename RandomAccessRange1, typename RandomAccessRange2> void` **`paired_stable_sort`**`(RandomAccessRange1& range1, RandomAccessRange2& range2)`
* `template <typename RandomAccessRange1, typename RandomAccessRange2, typename Compare> void` **`paired_stable_sort`**`(RandomAccessRange1& range1, RandomAccessRange2& range2, Compare comp)`

Sort `range1` according to the comparison function (with the usual `std::less`
default), also reordering `range2` in the same way (that is, each element in
`range2` goes into the same position as the corresponding element of
`range1`).

### Paired transform ###

* `template <typename InputRange1, typename InputRange2, typename OutputIterator, typename BinaryFunction> void` **`paired_transform`**`(const InputRange1& range1, const InputRange2& range2, OutputIterator out, BinaryFunction f)`

For each pair of corresponding elements in the two ranges, this calls `f(x,y)`
and appends the result to the output range.
