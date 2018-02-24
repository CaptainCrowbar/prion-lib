# Template Metaprogramming #

By Ross Smith

* `#include "rs-core/meta.hpp"`

## Contents ##

[TOC]

## Introduction ##

Everything in this header is in `namespace RS::Meta`. The `Meta::Nil` type is
exported to `namespace RS` because it is used as a placeholder type elsewhere
in the library.

Metafunctions are templates that operate on types; usually they take one or
more types as arguments and "return" another type, but non-type template
arguments and return types are also possible.

Metafunctions that return a type are implemented as alias templates,
evaluating directly to the result type, without the traditional `::type`
member type.

Metafunctions that return a boolean or integer value are implemented twice,
once as template types with the traditional `::value` member constant, and
again as template constants that yield the value directly. The two
implementations are supplied because the inline constant form is more
convenient if the value is going to be used directly, but these can't be used
as template template arguments in building other metafunctions, for which the
traditional member constant form is required.

## Logical metafunctions ##

* `template <typename P> struct` **`Not`**
* `template <typename... PS> struct` **`And`**
* `template <typename... PS> struct` **`Or`**
* `template <typename P1, typename P2> struct` **`Xor`**
* `template <typename P> constexpr bool` **`not_`**
* `template <typename... PS> constexpr bool` **`and_`**
* `template <typename... PS> constexpr bool` **`or_`**
* `template <typename P1, typename P2> constexpr bool` **`xor_`**

Predicate metafunctions that perform the standard logical operations on one or
more arguments, themselves expected to be predicate metafunctions with a
`::value` member.

## Type lists ##

A typelist is an instantiation of the `Typelist<...>` template. `Nil` is a
synonym for the empty typelist. An assortment of metafunctions are defined
here that operate on typelists.

Example:

    using SignedTypes = Typelist<short, int, long>;
    using UnsignedTypes = Typelist<unsigned short, unsigned int, unsigned long>;
    using IntegerTypes = Concat<SignedTypes, UnsignedTypes>;
    static constexpr int n_types = length_of<IntegerTypes>; // 6

In the documentation below, these abbreviations are used to indicate the
requirements on template parameters:

* `BF` -- Binary metafunction _(T1 &otimes; T2 &rarr; T3)_
* `BP` -- Binary predicate metafunction _(T1 &otimes; T2 &rarr; bool)_
* `CP` -- Comparison predicate metafunction _(T1 &otimes; T2 &rarr; bool)_
* `SL` -- Sorted typelist
* `T` -- General type
* `TL` -- Typelist
* `UF` -- Unary metafunction _(T1 &rarr; T2)_
* `UP` -- Unary predicate metafunction _(T &rarr; bool)_
* `N` -- `int`

Indexing into typelists is zero-based; functions that return an index will
return -1 to indicate failure. A comparison predicate is expected to have
less-than semantics, in the same way as the comparison predicates used for
ordering in the STL.

### Typelist primitives ###

* `template <typename... TS> struct` **`Typelist`**
* `using` **`Nil`** `= Typelist<>`

Comparison operators are defined for all typelists (two typelist objects of
the same type are always equal). An output operator is defined for `Nil`
(formatted as `"nil"`), but not for non-empty typelists.

### Typelist composition and decomposition metafunctions ###

| Name                 | Returns  | Description                                                 |
| ----                 | -------  | -----------                                                 |
| `Append<TL,T>`       | `TL`     | Insert `T` at the end of `TL`                               |
| `Concat<TL,...>`     | `TL`     | Concatenate typelists or types; argument list may be empty  |
| `Insert<TL,T,CP>`    | `TL`     | Insert `T` in a sorted list                                 |
| `InsertAt<TL,T,N>`   | `TL`     | Insert `T` at index `N` in `TL`                             |
| `Most<TL>`           | `TL`     | All but the last element in `TL`; `Nil` if `TL` is `Nil`    |
| `Prefix<T,TL>`       | `TL`     | Insert `T` at the beginning of `TL`                         |
| `RepList<TL,N>`      | `TL`     | Concatenate `N` copies of `TL`                              |
| `RepType<T,N>`       | `TL`     | Concatenate `N` copies of `T`                               |
| `Resize<TL,N,T>`     | `TL`     | Resize list to length `N`; use `T` for padding              |
| `Skip<TL,N>`         | `TL`     | Discard the first `N` elements from `TL`                    |
| `Sublist<TL,N1,N2>`  | `TL`     | Discard `N1` elements and return next `N2` elements         |
| `Tail<TL>`           | `TL`     | All but the first element in `TL`; `Nil` if `TL` is `Nil`   |
| `Take<TL,N>`         | `TL`     | First `N` elements in `TL`                                  |

### Type selection metafunctions ###

| Name              | Returns  | Description                                                        |
| ----              | -------  | -----------                                                        |
| `Head<TL>`        | `T`      | First element in `TL`; `Nil` if `TL` is `Nil`                      |
| `Last<TL>`        | `T`      | Last element in `TL`; `Nil` if `TL` is `Nil`                       |
| `MaxType<TL,CP>`  | `T`      | Maximum element in `TL` according to `CP`; `Nil` if `TL` is `Nil`  |
| `MinType<TL,CP>`  | `T`      | Minimum element in `TL` according to `CP`; `Nil` if `TL` is `Nil`  |
| `TypeAt<TL,N>`    | `T`      | `Nth` element in `TL`; `Nil` if `N` is out of bounds               |

### Typelist transformation metafunctions ###

| Name                       | Returns  | Description                                                                   |
| ----                       | -------  | -----------                                                                   |
| `FoldLeft<TL,BF,T>`        | `T`      | Left-to-right reduce, with `T` as initial value                               |
| `FoldRight<TL,BF,T>`       | `T`      | Right-to-left reduce, with `T` as initial value                               |
| `FullyUnique<TL>`          | `TL`     | Drop all duplicate elements                                                   |
| `MakeSet<TL,CP>`           | `TL`     | `Sort` followed by `Unique`                                                   |
| `Map<TL,UF>`               | `TL`     | Replace each element `T` in `TL` with `UF<T>`                                 |
| `PartialReduce<TL,BP,BF>`  | `TL`     | Reduce adjacent elements matched with `BP`                                    |
| `Reduce<TL,BF>`            | `T`      | Left-to-right reduce, with no initial value; `Nil` if `TL` is `Nil`           |
| `Remove<TL,T>`             | `TL`     | Remove all occurrences of `T` in `TL`                                         |
| `RemoveIf<TL,UP>`          | `TL`     | Remove all elements for which `UP<T>` is true                                 |
| `Reverse<TL>`              | `TL`     | Reverse the order of `TL`                                                     |
| `Select<TL,UP>`            | `TL`     | All elements in `TL` for which `UP<T>` is true                                |
| `Sort<TL,CP>`              | `TL`     | Sort `TL` according to `CP` (stable sort)                                     |
| `Unique<TL>`               | `TL`     | Drop consecutive duplicate elements                                           |
| `Zip<TL1,TL2,BF>`          | `TL`     | Map each pair of elements to `BF<T1,T2>` (output has length of shorter list)  |

### Other typelist operations ###

| Name                   | Returns  | Description                                  |
| ----                   | -------  | -----------                                  |
| `InheritTypelist<TL>`  | `type`   | Inherits from all unique types in the list   |
| `TypelistToTuple<TL>`  | `tuple`  | Tuple with the same types as the list        |
| `TupleToTypelist<T>`   | `TL`     | List with the same types as a tuple or pair  |

### Typelist property metafunctions ###

| Type                 | Constant               | Returns  | Description                                                       |
| ----                 | --------               | -------  | -----------                                                       |
| `AllOf<TL,UP>`       | `all_of<TL,UP>`        | `bool`   | True if all types match `UP`; true if `TL` is `Nil`               |
| `AnyOf<TL,UP>`       | `any_of<TL,UP>`        | `bool`   | True if at least one type matches `UP`; false if `TL` is `Nil`    |
| `CountIf<TL,UP>`     | `count_if<TL,UP>`      | `int`    | Number of elements in `TL` for which `UP<T>` is true              |
| `CountIfNot<TL,UP>`  | `count_if_not<TL,UP>`  | `int`    | Number of elements in `TL` for which `UP<T>` is false             |
| `CountType<TL,T>`    | `count_type<TL,T>`     | `int`    | Number of times `T` occurs in `TL`                                |
| `FindIf<TL,UP>`      | `find_if<TL,UP>`       | `int`    | Index of the first matching element in `TL`; -1 if not found      |
| `FindIfNot<TL,UP>`   | `find_if_not<TL,UP>`   | `int`    | Index of the first non-matching element in `TL`; -1 if not found  |
| `FindType<TL,T>`     | `find_type<TL,T>`      | `int`    | Index of the first `T` in `TL`; -1 if not found                   |
| `InList<TL,T>`       | `in_list<TL,T>`        | `bool`   | True if `T` is in `TL`                                            |
| `IsEmpty<TL>`        | `is_empty<TL>`         | `bool`   | True if `TL` is `Nil`                                             |
| `IsNotEmpty<TL>`     | `is_not_empty<TL>`     | `bool`   | True if `TL` is a typelist and is not `Nil`                       |
| `IsUnique<TL>`       | `is_unique<TL>`        | `bool`   | True if `TL` contains no duplicates                               |
| `LengthOf<TL>`       | `length_of<TL>`        | `int`    | Number of entries in `TL`                                         |
| `NoneOf<TL,UP>`      | `none_of<TL,UP>`       | `bool`   | True if no types match `UP`; true if `TL` is `Nil`                |

## Introspection ##

### Reflection tools ###

_(Based on Walter E. Brown,
[N4502 Proposing Standard Library Support for the C++ Detection Idiom V2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4502.pdf),
2015.)_

* `template <template <typename...> typename Archetype, typename... Args> using` **`IsDetected`**
* `template <template <typename...> typename Archetype, typename... Args> constexpr bool` **`is_detected`**

True if the archetype is a valid expression for the supplied argument types.

* `template <typename Default, template <typename...> typename Archetype, typename... Args> using` **`DetectedOr`**

Returns the type defined by the archetype if it is valid, otherwise the default type.

* `template <typename Result, template <typename...> typename Archetype, typename... Args> using` **`IsDetectedExact`**
* `template <typename Result, template <typename...> typename Archetype, typename... Args> constexpr bool` **`is_detected_exact`**

True if the archetype is valid and returns a specific type.

* `template <typename Result, template <typename...> typename Archetype, typename... Args> using` **`IsDetectedConvertible`**
* `template <typename Result, template <typename...> typename Archetype, typename... Args> constexpr bool` **`is_detected_convertible`**

True if the archetype is valid and returns a type convertible to the specified type.

### Operator detection ###

<!-- DEFN -->

| Metafunction type                            | Metafunction constant                            | Operator  |
| -----------------                            | ---------------------                            | --------  |
| **`HasPreIncrementOperator`**`<T>`           | **`has_pre_increment_operator`**`<T>`            | `++T`     |
| **`HasPreDecrementOperator`**`<T>`           | **`has_pre_decrement_operator`**`<T>`            | `--T`     |
| **`HasPostIncrementOperator`**`<T>`          | **`has_post_increment_operator`**`<T>`           | `T++`     |
| **`HasPostDecrementOperator`**`<T>`          | **`has_post_decrement_operator`**`<T>`           | `T--`     |
| **`HasUnaryPlusOperator`**`<T>`              | **`has_unary_plus_operator`**`<T>`               | `+T`      |
| **`HasUnaryMinusOperator`**`<T>`             | **`has_unary_minus_operator`**`<T>`              | `-T`      |
| **`HasDereferenceOperator`**`<T>`            | **`has_dereference_operator`**`<T>`              | `*T`      |
| **`HasLogicalNotOperator`**`<T>`             | **`has_logical_not_operator`**`<T>`              | `!T`      |
| **`HasBitwiseNotOperator`**`<T>`             | **`has_bitwise_not_operator`**`<T>`              | `~T`      |
| **`HasIndexOperator`**`<T,T2=T>`             | **`has_index_operator`**`<T,T2=T>`               | `T[T2]`   |
| **`HasPlusOperator`**`<T,T2=T>`              | **`has_plus_operator`**`<T,T2=T>`                | `T+T2`    |
| **`HasMinusOperator`**`<T,T2=T>`             | **`has_minus_operator`**`<T,T2=T>`               | `T-T2`    |
| **`HasMultiplyOperator`**`<T,T2=T>`          | **`has_multiply_operator`**`<T,T2=T>`            | `T*T2`    |
| **`HasDivideOperator`**`<T,T2=T>`            | **`has_divide_operator`**`<T,T2=T>`              | `T/T2`    |
| **`HasRemainderOperator`**`<T,T2=T>`         | **`has_remainder_operator`**`<T,T2=T>`           | `T%T2`    |
| **`HasBitwiseAndOperator`**`<T,T2=T>`        | **`has_bitwise_and_operator`**`<T,T2=T>`         | `T&T2`    |
| **`HasBitwiseOrOperator`**`<T,T2=T>`         | **`has_bitwise_or_operator`**`<T,T2=T>`          | `T|T2`    |
| **`HasBitwiseXorOperator`**`<T,T2=T>`        | **`has_bitwise_xor_operator`**`<T,T2=T>`         | `T^T2`    |
| **`HasLeftShiftOperator`**`<T,T2=T>`         | **`has_left_shift_operator`**`<T,T2=T>`          | `T<<T2`   |
| **`HasRightShiftOperator`**`<T,T2=T>`        | **`has_right_shift_operator`**`<T,T2=T>`         | `T>>T2`   |
| **`HasEqualOperator`**`<T,T2=T>`             | **`has_equal_operator`**`<T,T2=T>`               | `T==T2`   |
| **`HasNotEqualOperator`**`<T,T2=T>`          | **`has_not_equal_operator`**`<T,T2=T>`           | `T!=T2`   |
| **`HasLessThanOperator`**`<T,T2=T>`          | **`has_less_than_operator`**`<T,T2=T>`           | `T<T2`    |
| **`HasGreaterThanOperator`**`<T,T2=T>`       | **`has_greater_than_operator`**`<T,T2=T>`        | `T>T2`    |
| **`HasLessOrEqualOperator`**`<T,T2=T>`       | **`has_less_or_equal_operator`**`<T,T2=T>`       | `T<=T2`   |
| **`HasGreaterOrEqualOperator`**`<T,T2=T>`    | **`has_greater_or_equal_operator`**`<T,T2=T>`    | `T>=T2`   |
| **`HasLogicalAndOperator`**`<T,T2=T>`        | **`has_logical_and_operator`**`<T,T2=T>`         | `T&&T2`   |
| **`HasLogicalOrOperator`**`<T,T2=T>`         | **`has_logical_or_operator`**`<T,T2=T>`          | `T||T2`   |
| **`HasAssignOperator`**`<T,T2=T>`            | **`has_assign_operator`**`<T,T2=T>`              | `T=T2`    |
| **`HasPlusAssignOperator`**`<T,T2=T>`        | **`has_plus_assign_operator`**`<T,T2=T>`         | `T+=T2`   |
| **`HasMinusAssignOperator`**`<T,T2=T>`       | **`has_minus_assign_operator`**`<T,T2=T>`        | `T-=T2`   |
| **`HasMultiplyAssignOperator`**`<T,T2=T>`    | **`has_multiply_assign_operator`**`<T,T2=T>`     | `T*=T2`   |
| **`HasDivideAssignOperator`**`<T,T2=T>`      | **`has_divide_assign_operator`**`<T,T2=T>`       | `T/=T2`   |
| **`HasRemainderAssignOperator`**`<T,T2=T>`   | **`has_remainder_assign_operator`**`<T,T2=T>`    | `T%=T2`   |
| **`HasBitwiseAndAssignOperator`**`<T,T2=T>`  | **`has_bitwise_and_assign_operator`**`<T,T2=T>`  | `T&=T2`   |
| **`HasBitwiseOrAssignOperator`**`<T,T2=T>`   | **`has_bitwise_or_assign_operator`**`<T,T2=T>`   | `T|=T2`   |
| **`HasBitwiseXorAssignOperator`**`<T,T2=T>`  | **`has_bitwise_xor_assign_operator`**`<T,T2=T>`  | `T^=T2`   |
| **`HasLeftShiftAssignOperator`**`<T,T2=T>`   | **`has_left_shift_assign_operator`**`<T,T2=T>`   | `T<<=T2`  |
| **`HasRightShiftAssignOperator`**`<T,T2=T>`  | **`has_right_shift_assign_operator`**`<T,T2=T>`  | `T>>=T2`  |

Metafunction predicates to detect the existence of operators. For the binary
operators, the second argument type can be omitted, defaulting to the same
type for both arguments. An implicit reference qualification is applied to the
first argument of the increment, decrement, assignment, and fused assignment
operators.

| Metafunction type                           | Metafunction constant                         | Operators                                                          |
| -----------------                           | ---------------------                         | ---------                                                          |
| **`HasIncrementOperators`**`<T>`            | **`has_increment_operators`**`<T>`            | `++T T++`                                                          |
| **`HasStepOperators`**`<T>`                 | **`has_step_operators`**`<T>`                 | `++T T++ --T T--`                                                  |
| **`HasAdditiveOperators`**`<T,T2=T>`        | **`has_additive_operators`**`<T,T2=T>`        | `T+T2 T-T2 T+=T2 T-=T2`                                            |
| **`HasMultiplicativeOperators`**`<T,T2=T>`  | **`has_multiplicative_operators`**`<T,T2=T>`  | `T*T2 T/T2 T*=T2 T/=T2`                                            |
| **`HasArithmeticOperators`**`<T>`           | **`has_arithmetic_operators`**`<T>`           | `-T T+T T-T T*T T/T T+=T T-=T T*=T T/=T`                           |
| **`HasIntegerArithmeticOperators`**`<T>`    | **`has_integer_arithmetic_operators`**`<T>`   | `-T ++T --T T++ T-- T+T T-T T*T T/T T%T T+=T T-=T T*=T T/=T T%=T`  |
| **`HasLinearOperators`**`<T,T2=T>`          | **`has_linear_operators`**`<T,T2=T>`          | `++T --T T++ T-- T+T2 T2+T T-T T-T2 T+=T2 T-=T2`                   |
| **`HasScalingOperators`**`<T,T2=T>`         | **`has_scaling_operators`**`<T,T2=T>`         | `T*T2 T2*T T/T T/T2 T*=T2 T/=T2`                                   |
| **`HasBitwiseOperators`**`<T>`              | **`has_bitwise_operators`**`<T>`              | `~T T&T T|T T^T T<<int T>>int T&=T T|=T T^=T T<<=int T>>=int`      |
| **`HasEqualityOperators`**`<T,T2=T>`        | **`has_equality_operators`**`<T,T2=T>`        | `T==T2 T!=T2`                                                      |
| **`HasComparisonOperators`**`<T,T2=T>`      | **`has_comparison_operators`**`<T,T2=T>`      | `T==T2 T!=T2 T<T2 T>T2 T<=T2 T>=T2`                                |

Combined tests for groups of operators.

### Function detection ###

* `#define` **`RS_DETECT_FUNCTION`**`(function)`
    * `template <typename... Args> using` **`HasFunction_`**_`<function>`_
    * `template <typename... Args> constexpr bool` **`has_function_`**_`<function>`_
    * _true if `function(Args...)` exists_
* `#define` **`RS_DETECT_STD_FUNCTION`**`(function)`
    * `template <typename... Args> using` **`HasStdFunction_`**_`<function>`_
    * `template <typename... Args> constexpr bool` **`has_function_std_`**_`<function>`_
    * _true if `std::function(Args...)` exists_
* `#define` **`RS_DETECT_METHOD`**`(function)`
    * `template <typename T, typename... Args> using` **`HasMethod_`**_`<function>`_
    * `template <typename T, typename... Args> constexpr bool` **`has_method_`**_`<function>`_
    * _true if `T::function(Args...)` exists_
* `#define` **`RS_DETECT_MEMBER_TYPE`**`(type)`
    * `template <typename T> using` **`HasMemberType_`**_`<type>`_
    * `template <typename T> constexpr bool` **`has_member_type_`**_`<type>`_
    * _true if `T::type` exists_

Each of these macros defines a metafunction predicate (in the usual two forms)
that can be used to detect the existence of a function or type.

Example:

    RS_DETECT_FUNCTION(foo);
    int foo(int, string) { ... }
    assert(has_function_foo<int, string>);
    assert(! has_function_foo<string, int>);

### Type categories ###

<!-- DEFN -->

| Metafunction type                   | Metafunction constant                 |
| -----------------                   | ---------------------                 |
| **`IsHashable`**`<T>`               | **`is_hashable`**`<T>`                |
| **`IsIterator`**`<T>`               | **`is_iterator`**`<T>`                |
| **`IsForwardIterator`**`<T>`        | **`is_forward_iterator`**`<T>`        |
| **`IsBidirectionalIterator`**`<T>`  | **`is_bidirectional_iterator`**`<T>`  |
| **`IsRandomAccessIterator`**`<T>`   | **`is_random_access_iterator`**`<T>`  |
| **`IsMutableIterator`**`<T>`        | **`is_mutable_iterator`**`<T>`        |
| **`IsRange`**`<T>`                  | **`is_range`**`<T>`                   |
| **`IsMutableRange`**`<T>`           | **`is_mutable_range`**`<T>`           |
| **`IsContainer`**`<T>`              | **`is_container`**`<T>`               |
| **`IsInsertableContainer`**`<T>`    | **`is_insertable_container`**`<T>`    |
| **`IsSwappable`**`<T>`              | **`is_swappable`**`<T>`               |

Metafunction predicates that attempt to detect certain kinds of type. In most
cases we can only look for an expected set of valid operations on the type; we
have no way to check their semantics.

Notes on specific predicates:

* `is_hashable` is true if `std::hash<T>()(t)` is valid.
* Iterator category predicates are true if the iterator is at least that category.
* `is_[mutable_]range` is true if `begin()` and `end()` can be found either in `namespace std` or by ADL.
* `is_swappable` is true if `swap()` can be found either in `namespace std` or by ADL.

### Associated types ###

* `template <typename Iterator> using` **`IteratorValue`**
* `template <typename Range> using` **`RangeIterator`**
* `template <typename Range> using` **`RangeValue`**

These return the types associated with an iterator or range. They are
extracted by checking the return type of `begin()`, and do not require a
specialization of `iterator_traits` to exist.
