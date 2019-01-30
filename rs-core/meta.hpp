#pragma once

#include "unicorn/utility.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <ostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace RS::Meta {

    // Logical metafunctions

    template <typename P> struct Not { static constexpr bool value = ! P::value; };

    template <typename... PS> struct And: std::true_type {};
    template <typename P, typename... PS> struct And<P, PS...> { static constexpr bool value = P::value && And<PS...>::value; };
    template <> struct And<>: std::true_type {};

    template <typename... PS> struct Or: std::true_type {};
    template <typename P, typename... PS> struct Or<P, PS...> { static constexpr bool value = P::value || Or<PS...>::value; };
    template <> struct Or<>: std::false_type {};

    template <typename P, typename Q> struct Xor { static constexpr bool value = bool(P::value) != bool(Q::value); };

    template <typename P> constexpr bool not_ = Not<P>::value;
    template <typename... PS> constexpr bool and_ = And<PS...>::value;
    template <typename... PS> constexpr bool or_ = Or<PS...>::value;
    template <typename P1, typename P2> constexpr bool xor_ = Xor<P1, P2>::value;

    // Typelist primitives

    template <typename... TS> struct Typelist {};
    using Nil = Typelist<>;

    template <typename... TS> constexpr bool operator==(Typelist<TS...>, Typelist<TS...>) noexcept { return true; }
    template <typename... TS> constexpr bool operator!=(Typelist<TS...>, Typelist<TS...>) noexcept { return false; }
    template <typename... TS> constexpr bool operator<(Typelist<TS...>, Typelist<TS...>) noexcept { return false; }
    template <typename... TS> constexpr bool operator>(Typelist<TS...>, Typelist<TS...>) noexcept { return false; }
    template <typename... TS> constexpr bool operator<=(Typelist<TS...>, Typelist<TS...>) noexcept { return true; }
    template <typename... TS> constexpr bool operator>=(Typelist<TS...>, Typelist<TS...>) noexcept { return true; }
    inline std::ostream& operator<<(std::ostream& out, Nil) { return out << "nil"; }
    inline std::string to_str(Nil) { return "nil"; }

    // Utilities

    template <bool B> using BoolT = std::integral_constant<bool, B>;
    template <int N> using IntT = std::integral_constant<int, N>;
    template <typename T> struct ReturnT { using type = T; };

    namespace MetaDetail {

        // Typelist composition and decomposition metafunctions

        template <typename TL, typename T> struct Append;
        template <typename... TS, typename T> struct Append<Typelist<TS...>, T>:
            ReturnT<Typelist<TS..., T>> {};
        template <typename... TS> struct Append<Typelist<TS...>, Nil>:
            ReturnT<Typelist<TS...>> {};

        template <typename T, typename TL> struct Prefix;
        template <typename T, typename... TS> struct Prefix<T, Typelist<TS...>>:
            ReturnT<Typelist<T, TS...>> {};

        template <typename... TLS> struct Concat;
        template <typename TL, typename... TLS> struct Concat<TL, TLS...>:
            Concat<TL, typename Concat<TLS...>::type> {};
        template <typename T1, typename T2> struct Concat<T1, T2>:
            ReturnT<Typelist<T1, T2>> {};
        template <typename T, typename... TS> struct Concat<T, Typelist<TS...>>:
            ReturnT<Typelist<T, TS...>> {};
        template <typename... TS, typename T> struct Concat<Typelist<TS...>, T>:
            ReturnT<Typelist<TS..., T>> {};
        template <typename... Types1, typename... Types2> struct Concat<Typelist<Types1...>, Typelist<Types2...>>:
            ReturnT<Typelist<Types1..., Types2...>> {};
        template <typename T> struct Concat<T>:
            ReturnT<Typelist<T>> {};
        template <typename... TS> struct Concat<Typelist<TS...>>:
            ReturnT<Typelist<TS...>> {};
        template <> struct Concat<Nil>:
            ReturnT<Nil> {};
        template <> struct Concat<>:
            ReturnT<Nil> {};

        template <typename TL, typename T, template <typename, typename> class CP> struct Insert;
        template <typename T1, typename... TS, typename T2, template <typename, typename> class CP> struct Insert<Typelist<T1, TS...>, T2, CP>:
            std::conditional<CP<T1, T2>::value, typename Prefix<T1, typename Insert<Typelist<TS...>, T2, CP>::type>::type, Typelist<T2, T1, TS...>> {};
        template <typename T, template <typename, typename> class CP> struct Insert<Nil, T, CP>:
            ReturnT<Typelist<T>> {};

        template <typename TL> struct Most;
        template <typename T, typename... TS> struct Most<Typelist<T, TS...>>:
            Prefix<T, typename Most<Typelist<TS...>>::type> {};
        template <typename T> struct Most<Typelist<T>>:
            ReturnT<Nil> {};
        template <> struct Most<Nil>:
            ReturnT<Nil> {};

        template <typename TL, int N> struct RepList:
            Concat<TL, typename RepList<TL, N - 1>::type> {};
        template <typename TL> struct RepList<TL, 0>:
            ReturnT<Nil> {};

        template <typename T, int N> struct RepType:
            Prefix<T, typename RepType<T, N - 1>::type> {};
        template <typename T> struct RepType<T, 0>:
            ReturnT<Nil> {};

        template <typename TL, int N, typename T> struct Resize;
        template <typename T1, typename... TS, int N, typename T2> struct Resize<Typelist<T1, TS...>, N, T2>:
            Concat<T1, typename Resize<Typelist<TS...>, N - 1, T2>::type> {};
        template <typename T1, typename... TS, typename T2> struct Resize<Typelist<T1, TS...>, 0, T2>:
            ReturnT<Nil> {};
        template <int N, typename T> struct Resize<Nil, N, T>:
            RepType<T, N> {};
        template <typename T> struct Resize<Nil, 0, T>:
            ReturnT<Nil> {};

        template <typename TL, int N> struct Skip;
        template <typename T, typename... TS, int N> struct Skip<Typelist<T, TS...>, N>:
            Skip<Typelist<TS...>, N - 1> {};
        template <typename T, typename... TS> struct Skip<Typelist<T, TS...>, 0>:
            ReturnT<Typelist<T, TS...>> {};
        template <int N> struct Skip<Nil, N>:
            ReturnT<Nil> {};
        template <> struct Skip<Nil, 0>:
            ReturnT<Nil> {};

        template <typename TL> struct Tail;
        template <typename T, typename... TS> struct Tail<Typelist<T, TS...>>:
            ReturnT<Typelist<TS...>> {};
        template <> struct Tail<Nil>:
            ReturnT<Nil> {};

        template <typename TL, int N> struct Take;
        template <typename T, typename... TS, int N> struct Take<Typelist<T, TS...>, N>:
            Concat<T, typename Take<Typelist<TS...>, N - 1>::type> {};
        template <typename T, typename... TS> struct Take<Typelist<T, TS...>, 0>:
            ReturnT<Nil> {};
        template <int N> struct Take<Nil, N>:
            ReturnT<Nil> {};
        template <> struct Take<Nil, 0>:
            ReturnT<Nil> {};

        template <typename TL, typename T, int N> struct InsertAt:
            Concat<typename Take<TL, (N < 1 ? 0 : N)>::type, T, typename Skip<TL, (N < 1 ? 0 : N)>::type> {};

        template <typename TL, int N1, int N2> struct Sublist:
            Take<typename Skip<TL, N1>::type, N2> {};

        // Type selection metafunctions

        template <typename TL> struct Head;
        template <typename T, typename... TS> struct Head<Typelist<T, TS...>>:
            ReturnT<T> {};
        template <> struct Head<Nil>:
            ReturnT<Nil> {};

        template <typename TL> struct Last;
        template <typename T, typename... TS> struct Last<Typelist<T, TS...>>:
            Last<Typelist<TS...>> {};
        template <typename T> struct Last<Typelist<T>>:
            ReturnT<T> {};
        template <> struct Last<Nil>:
            ReturnT<Nil> {};

        template <typename TL, template <typename, typename> class CP> struct MaxType;
        template <typename T, typename... TS, template <typename, typename> class CP> struct MaxType<Typelist<T, TS...>, CP>:
            MaxType<Typelist<T, typename MaxType<Typelist<TS...>, CP>::type>, CP> {};
        template <typename T1, typename T2, template <typename, typename> class CP> struct MaxType<Typelist<T1, T2>, CP>:
            std::conditional<CP<T1, T2>::value, T2, T1> {};
        template <typename T, template <typename, typename> class CP> struct MaxType<Typelist<T>, CP>:
            ReturnT<T> {};
        template <template <typename, typename> class CP> struct MaxType<Nil, CP>:
            ReturnT<Nil> {};

        template <typename TL, template <typename, typename> class CP> struct MinType;
        template <typename T, typename... TS, template <typename, typename> class CP> struct MinType<Typelist<T, TS...>, CP>:
            MinType<Typelist<T, typename MinType<Typelist<TS...>, CP>::type>, CP> {};
        template <typename T1, typename T2, template <typename, typename> class CP> struct MinType<Typelist<T1, T2>, CP>:
            std::conditional<CP<T1, T2>::value, T1, T2> {};
        template <typename T, template <typename, typename> class CP> struct MinType<Typelist<T>, CP>:
            ReturnT<T> {};
        template <template <typename, typename> class CP> struct MinType<Nil, CP>:
            ReturnT<Nil> {};

        template <typename TL, int N> struct TypeAt;
        template <typename T, typename... TS, int N> struct TypeAt<Typelist<T, TS...>, N>:
            TypeAt<Typelist<TS...>, (N > 0 ? N - 1 : -1)> {};
        template <typename T, typename... TS> struct TypeAt<Typelist<T, TS...>, 0>:
            ReturnT<T> {};
        template <typename T, typename... TS> struct TypeAt<Typelist<T, TS...>, -1>:
            ReturnT<Nil> {};
        template <int N> struct TypeAt<Nil, N>:
            ReturnT<Nil> {};

        // Typelist transformation metafunctions

        template <typename TL, template <typename, typename> class BF, typename T> struct FoldLeft;
        template <typename T1, typename... TS, template <typename, typename> class BF, typename T2> struct FoldLeft<Typelist<T1, TS...>, BF, T2>:
            FoldLeft<Typelist<TS...>, BF, BF<T2, T1>> {};
        template <template <typename, typename> class BF, typename T> struct FoldLeft<Nil, BF, T>:
            ReturnT<T> {};

        template <typename TL, template <typename, typename> class BF, typename T> struct FoldRight;
        template <typename T1, typename... TS, template <typename, typename> class BF, typename T2> struct FoldRight<Typelist<T1, TS...>, BF, T2>:
            FoldRight<typename Most<Typelist<T1, TS...>>::type, BF, BF<typename Last<Typelist<T1, TS...>>::type, T2>> {};
        template <template <typename, typename> class BF, typename T> struct FoldRight<Nil, BF, T>:
            ReturnT<T> {};

        template <typename TL, template <typename> class UF> struct Map;
        template <typename T, typename... TS, template <typename> class UF> struct Map<Typelist<T, TS...>, UF>:
            Prefix<UF<T>, typename Map<Typelist<TS...>, UF>::type> {};
        template <template <typename> class UF> struct Map<Nil, UF>:
            ReturnT<Nil> {};

        template <typename TL, template <typename, typename> class BP, template <typename, typename> class BF> struct PartialReduce;
        template <typename T1, typename T2, template <typename, typename> class BP, template <typename, typename> class BF, bool Match, typename... TS>
            struct PartialReduceHelper:
            PartialReduce<Typelist<BF<T1, T2>, TS...>, BP, BF> {};
        template <typename T1, typename T2, template <typename, typename> class BP, template <typename, typename> class BF, typename... TS>
            struct PartialReduceHelper<T1, T2, BP, BF, false, TS...>:
            Prefix<T1, typename PartialReduce<Typelist<T2, TS...>, BP, BF>::type> {};
        template <typename T1, typename T2, typename... TS, template <typename, typename> class BP, template <typename, typename> class BF>
            struct PartialReduce<Typelist<T1, T2, TS...>, BP, BF>:
            PartialReduceHelper<T1, T2, BP, BF, BP<T1, T2>::value, TS...> {};
        template <typename T, template <typename, typename> class BP, template <typename, typename> class BF> struct PartialReduce<Typelist<T>, BP, BF>:
            ReturnT<Typelist<T>> {};
        template <template <typename, typename> class BP, template <typename, typename> class BF> struct PartialReduce<Nil, BP, BF>:
            ReturnT<Nil> {};

        template <typename TL, template <typename, typename> class BF> struct Reduce;
        template <typename T1, typename T2, typename... TS, template <typename, typename> class BF> struct Reduce<Typelist<T1, T2, TS...>, BF>:
            Reduce<Typelist<BF<T1, T2>, TS...>, BF> {};
        template <typename T, template <typename, typename> class BF> struct Reduce<Typelist<T>, BF>:
            ReturnT<T> {};
        template <template <typename, typename> class BF> struct Reduce<Nil, BF>:
            ReturnT<Nil> {};

        template <typename TL, typename T> struct Remove;
        template <typename T1, typename... TS, typename T2> struct Remove<Typelist<T1, TS...>, T2>:
            Prefix<T1, typename Remove<Typelist<TS...>, T2>::type> {};
        template <typename T, typename... TS> struct Remove<Typelist<T, TS...>, T>:
            Remove<Typelist<TS...>, T> {};
        template <typename T> struct Remove<Nil, T>:
            ReturnT<Nil> {};

        template <typename TL, template <typename> class UP> struct RemoveIf;
        template <typename T, typename... TS, template <typename> class UP> struct RemoveIf<Typelist<T, TS...>, UP>:
            std::conditional<UP<T>::value, typename RemoveIf<Typelist<TS...>, UP>::type, typename Prefix<T, typename RemoveIf<Typelist<TS...>, UP>::type>::type> {};
        template <template <typename> class UP> struct RemoveIf<Nil, UP>:
            ReturnT<Nil> {};

        template <typename TL> struct Reverse;
        template <typename T, typename... TS> struct Reverse<Typelist<T, TS...>>:
            Append<typename Reverse<Typelist<TS...>>::type, T> {};
        template <typename T> struct Reverse<Typelist<T, Nil>>:
            ReturnT<Typelist<T, Nil>> {};
        template <> struct Reverse<Nil>:
            ReturnT<Nil> {};

        template <typename TL, template <typename> class UP> struct Select;
        template <typename T, typename... TS, template <typename> class UP> struct Select<Typelist<T, TS...>, UP>:
            std::conditional<UP<T>::value, typename Prefix<T, typename Select<Typelist<TS...>, UP>::type>::type, typename Select<Typelist<TS...>, UP>::type> {};
        template <template <typename> class UP> struct Select<Nil, UP>:
            ReturnT<Nil> {};

        template <typename TL, template <typename, typename> class CP> struct Sort;
        template <typename T, typename... TS, template <typename, typename> class CP> struct Sort<Typelist<T, TS...>, CP>:
            Insert<typename Sort<Typelist<TS...>, CP>::type, T, CP> {};
        template <template <typename, typename> class CP> struct Sort<Nil, CP>:
            ReturnT<Nil> {};

        template <typename TL> struct Unique;
        template <typename T, typename... TS> struct Unique<Typelist<T, T, TS...>>:
            Unique<Typelist<T, TS...>> {};
        template <typename T, typename... TS> struct Unique<Typelist<T, TS...>>:
            Prefix<T, typename Unique<Typelist<TS...>>::type> {};
        template <> struct Unique<Nil>:
            ReturnT<Nil> {};

        template <typename TL> struct FullyUnique;
        template <typename T, typename... TS> struct FullyUnique<Typelist<T, TS...>>:
            ReturnT<typename Prefix<T, typename FullyUnique<typename Remove<Typelist<TS...>, T>::type>::type>::type> {};
        template <> struct FullyUnique<Nil>:
            ReturnT<Nil> {};

        template <typename TL1, typename TL2, template <typename, typename> class BF> struct Zip;
        template <typename T1, typename... Types1, typename T2, typename... Types2, template <typename, typename> class BF>
            struct Zip<Typelist<T1, Types1...>, Typelist<T2, Types2...>, BF>:
            Prefix<BF<T1, T2>, typename Zip<Typelist<Types1...>, Typelist<Types2...>, BF>::type> {};
        template <typename TL, template <typename, typename> class BF> struct Zip<TL, Nil, BF>:
            ReturnT<Nil> {};
        template <typename TL, template <typename, typename> class BF> struct Zip<Nil, TL, BF>:
            ReturnT<Nil> {};
        template <template <typename, typename> class BF> struct Zip<Nil, Nil, BF>:
            ReturnT<Nil> {};

        template <typename TL, template <typename, typename> class CP> struct MakeSet:
            Unique<typename Sort<TL, CP>::type> {};

        // Other typelist operations

        template <typename TL> struct InheritTypelistHelper;
        template <typename T, typename... TS> struct InheritTypelistHelper<Typelist<T, TS...>>:
            T, InheritTypelistHelper<Typelist<TS...>> {};
        template <> struct InheritTypelistHelper<Nil> {};

        template <typename TL> struct TypelistToTuple;
        template <typename... TS> struct TypelistToTuple<Typelist<TS...>>:
            ReturnT<std::tuple<TS...>> {};

        template <typename T> struct TupleToTypelist;
        template <typename... TS> struct TupleToTypelist<std::tuple<TS...>>:
            ReturnT<Typelist<TS...>> {};
        template <typename T1, typename T2> struct TupleToTypelist<std::pair<T1, T2>>:
            ReturnT<Typelist<T1, T2>> {};

        // Typelist function call operations

        template <typename TL> struct CallForEach;
        template <typename T, typename... TS> struct CallForEach<Typelist<T, TS...>> {
            template <typename... Args> void operator()(Args&&... args) const {
                T t(std::forward<Args>(args)...);
                t();
                CallForEach<Typelist<TS...>>()(std::forward<Args>(args)...);
            }
        };
        template <> struct CallForEach<Nil> {
            template <typename... Args> void operator()(Args&&...) const {}
        };

        template <typename TL> struct CallForEachWith;
        template <typename T, typename... TS> struct CallForEachWith<Typelist<T, TS...>> {
            template <typename... Args> void operator()(Args&&... args) const {
                T t;
                t(std::forward<Args>(args)...);
                CallForEachWith<Typelist<TS...>>()(std::forward<Args>(args)...);
            }
        };
        template <> struct CallForEachWith<Nil> {
            template <typename... Args> void operator()(Args&&...) const {}
        };

        template <typename TL> struct FunctionForEach;
        template <typename T, typename... TS> struct FunctionForEach<Typelist<T, TS...>> {
            template <typename F, typename... Args> void operator()(F&& f, Args&&... args) const {
                T t(std::forward<Args>(args)...);
                f(t);
                FunctionForEach<Typelist<TS...>>()(std::forward<F>(f), std::forward<Args>(args)...);
            }
        };
        template <> struct FunctionForEach<Nil> {
            template <typename F, typename... Args> void operator()(F&&, Args&&...) const {}
        };

        template <typename TL> struct FunctionForEachWith;
        template <typename T, typename... TS> struct FunctionForEachWith<Typelist<T, TS...>> {
            template <typename F, typename... Args> void operator()(F&& f, Args&&... args) const {
                T t;
                f(t, std::forward<Args>(args)...);
                FunctionForEachWith<Typelist<TS...>>()(std::forward<F>(f), std::forward<Args>(args)...);
            }
        };
        template <> struct FunctionForEachWith<Nil> {
            template <typename F, typename... Args> void operator()(F&&, Args&&...) const {}
        };

    }

    // Typelist property metafunctions

    template <typename TL, template <typename> class UP> struct AllOf;
    template <typename T, typename... TS, template <typename> class UP> struct AllOf<Typelist<T, TS...>, UP>:
        BoolT<UP<T>::value && AllOf<Typelist<TS...>, UP>::value> {};
    template <template <typename> class UP> struct AllOf<Nil, UP>:
        std::true_type {};

    template <typename TL, template <typename> class UP> struct AnyOf;
    template <typename T, typename... TS, template <typename> class UP> struct AnyOf<Typelist<T, TS...>, UP>:
        BoolT<UP<T>::value || AnyOf<Typelist<TS...>, UP>::value> {};
    template <template <typename> class UP> struct AnyOf<Nil, UP>:
        std::false_type {};

    template <typename TL, template <typename> class UP> struct CountIf;
    template <typename T, typename... TS, template <typename> class UP> struct CountIf<Typelist<T, TS...>, UP>:
        IntT<UP<T>::value + CountIf<Typelist<TS...>, UP>::value> {};
    template <template <typename> class UP> struct CountIf<Nil, UP>:
        IntT<0> {};

    template <typename TL, template <typename> class UP> struct CountIfNot;
    template <typename T, typename... TS, template <typename> class UP> struct CountIfNot<Typelist<T, TS...>, UP>:
        IntT<! UP<T>::value + CountIfNot<Typelist<TS...>, UP>::value> {};
    template <template <typename> class UP> struct CountIfNot<Nil, UP>:
        IntT<0> {};

    template <typename TL, typename T> struct CountType;
    template <typename T1, typename... TS, typename T2> struct CountType<Typelist<T1, TS...>, T2>:
        IntT<std::is_same<T1, T2>::value + CountType<Typelist<TS...>, T2>::value> {};
    template <typename T> struct CountType<Nil, T>:
        IntT<0> {};

    template <typename TL, template <typename> class UP> struct FindIf;
    template <typename T, typename... TS, template <typename> class UP> struct FindIf<Typelist<T, TS...>, UP>:
        IntT<UP<T>::value ? 0 : (FindIf<Typelist<TS...>, UP>::value >= 0) ? FindIf<Typelist<TS...>, UP>::value + 1 : -1> {};
    template <template <typename> class UP> struct FindIf<Nil, UP>:
        IntT<-1> {};

    template <typename TL, template <typename> class UP> struct FindIfNot;
    template <typename T, typename... TS, template <typename> class UP> struct FindIfNot<Typelist<T, TS...>, UP>:
        IntT<! UP<T>::value ? 0 : (FindIfNot<Typelist<TS...>, UP>::value >= 0) ? FindIfNot<Typelist<TS...>, UP>::value + 1 : -1> {};
    template <template <typename> class UP> struct FindIfNot<Nil, UP>:
        IntT<-1> {};

    template <typename TL, typename T> struct FindType;
    template <typename T1, typename... TS, typename T2> struct FindType<Typelist<T1, TS...>, T2>:
        IntT<(FindType<Typelist<TS...>, T2>::value >= 0) ? FindType<Typelist<TS...>, T2>::value + 1 : -1> {};
    template <typename T, typename... TS> struct FindType<Typelist<T, TS...>, T>:
        IntT<0> {};
    template <typename T> struct FindType<Nil, T>:
        IntT<-1> {};

    template <typename TL, typename T> struct InList;
    template <typename T1, typename... TS, typename T2> struct InList<Typelist<T1, TS...>, T2>:
        InList<Typelist<TS...>, T2> {};
    template <typename T, typename... TS> struct InList<Typelist<T, TS...>, T>:
        std::true_type {};
    template <typename T> struct InList<Nil, T>:
        std::false_type {};

    template <typename TL> struct IsEmpty;
    template <typename... TS> struct IsEmpty<Typelist<TS...>>:
        BoolT<sizeof...(TS) == 0> {};

    template <typename TL> struct IsNotEmpty:
        std::false_type {};
    template <typename... TS> struct IsNotEmpty<Typelist<TS...>>:
        std::true_type {};
    template <> struct IsNotEmpty<Nil>:
        std::false_type {};

    template <typename TL> struct IsUnique:
        std::true_type {};
    template <typename T, typename... TS> struct IsUnique<Typelist<T, TS...>>:
        BoolT<! CountType<Typelist<TS...>, T>::value && IsUnique<Typelist<TS...>>::value> {};

    template <typename TL> struct LengthOf;
    template <typename... TS> struct LengthOf<Typelist<TS...>>:
        IntT<sizeof...(TS)> {};

    template <typename TL, template <typename> class UP> struct NoneOf;
    template <typename T, typename... TS, template <typename> class UP> struct NoneOf<Typelist<T, TS...>, UP>:
        BoolT<! UP<T>::value && NoneOf<Typelist<TS...>, UP>::value> {};
    template <template <typename> class UP> struct NoneOf<Nil, UP>:
        std::true_type {};

    // Typelist composition and decomposition metafunctions

    template <typename TL, typename T> using Append                                          = typename MetaDetail::Append<TL, T>::type;
    template <typename T, typename TL> using Prefix                                          = typename MetaDetail::Prefix<T, TL>::type;
    template <typename... TLS> using Concat                                                  = typename MetaDetail::Concat<TLS...>::type;
    template <typename TL, typename T, template <typename, typename> class CP> using Insert  = typename MetaDetail::Insert<TL, T, CP>::type;
    template <typename TL, typename T, int N> using InsertAt                                 = typename MetaDetail::InsertAt<TL, T, N>::type;
    template <typename TL> using Most                                                        = typename MetaDetail::Most<TL>::type;
    template <typename TL, int N> using RepList                                              = typename MetaDetail::RepList<TL, N>::type;
    template <typename T, int N> using RepType                                               = typename MetaDetail::RepType<T, N>::type;
    template <typename TL, int N, typename T> using Resize                                   = typename MetaDetail::Resize<TL, N, T>::type;
    template <typename TL, int N> using Skip                                                 = typename MetaDetail::Skip<TL, N>::type;
    template <typename TL, int N1, int N2> using Sublist                                     = typename MetaDetail::Sublist<TL, N1, N2>::type;
    template <typename TL> using Tail                                                        = typename MetaDetail::Tail<TL>::type;
    template <typename TL, int N> using Take                                                 = typename MetaDetail::Take<TL, N>::type;

    // Type selection metafunctions

    template <typename TL> using Head                                             = typename MetaDetail::Head<TL>::type;
    template <typename TL> using Last                                             = typename MetaDetail::Last<TL>::type;
    template <typename TL, template <typename, typename> class CP> using MaxType  = typename MetaDetail::MaxType<TL, CP>::type;
    template <typename TL, template <typename, typename> class CP> using MinType  = typename MetaDetail::MinType<TL, CP>::type;
    template <typename TL, int N> using TypeAt                                    = typename MetaDetail::TypeAt<TL, N>::type;

    // Typelist transformation metafunctions

    template <typename TL, template <typename, typename> class BF, typename T> using FoldLeft                                   = typename MetaDetail::FoldLeft<TL, BF, T>::type;
    template <typename TL, template <typename, typename> class BF, typename T> using FoldRight                                  = typename MetaDetail::FoldRight<TL, BF, T>::type;
    template <typename TL> using FullyUnique                                                                                    = typename MetaDetail::FullyUnique<TL>::type;
    template <typename TL, template <typename, typename> class CP> using MakeSet                                                = typename MetaDetail::MakeSet<TL, CP>::type;
    template <typename TL, template <typename> class UF> using Map                                                              = typename MetaDetail::Map<TL, UF>::type;
    template <typename TL, template <typename, typename> class BP, template <typename, typename> class BF> using PartialReduce  = typename MetaDetail::PartialReduce<TL, BP, BF>::type;
    template <typename TL, template <typename, typename> class BF> using Reduce                                                 = typename MetaDetail::Reduce<TL, BF>::type;
    template <typename TL, typename T> using Remove                                                                             = typename MetaDetail::Remove<TL, T>::type;
    template <typename TL, template <typename> class UP> using RemoveIf                                                         = typename MetaDetail::RemoveIf<TL, UP>::type;
    template <typename TL> using Reverse                                                                                        = typename MetaDetail::Reverse<TL>::type;
    template <typename TL, template <typename> class UP> using Select                                                           = typename MetaDetail::Select<TL, UP>::type;
    template <typename TL, template <typename, typename> class CP> using Sort                                                   = typename MetaDetail::Sort<TL, CP>::type;
    template <typename TL> using Unique                                                                                         = typename MetaDetail::Unique<TL>::type;
    template <typename TL1, typename TL2, template <typename, typename> class BF> using Zip                                     = typename MetaDetail::Zip<TL1, TL2, BF>::type;

    // Other typelist operations

    template <typename TL> using InheritTypelist  = typename MetaDetail::InheritTypelistHelper<FullyUnique<TL>>;
    template <typename TL> using TypelistToTuple  = typename MetaDetail::TypelistToTuple<TL>::type;
    template <typename T> using TupleToTypelist   = typename MetaDetail::TupleToTypelist<T>::type;

    // Typelist property metafunctions

    template <typename TL, template <typename> class UP> constexpr bool all_of       = AllOf<TL, UP>::value;
    template <typename TL, template <typename> class UP> constexpr bool any_of       = AnyOf<TL, UP>::value;
    template <typename TL, template <typename> class UP> constexpr int count_if      = CountIf<TL, UP>::value;
    template <typename TL, template <typename> class UP> constexpr int count_if_not  = CountIfNot<TL, UP>::value;
    template <typename TL, typename T> constexpr int count_type                      = CountType<TL, T>::value;
    template <typename TL, template <typename> class UP> constexpr int find_if       = FindIf<TL, UP>::value;
    template <typename TL, template <typename> class UP> constexpr int find_if_not   = FindIfNot<TL, UP>::value;
    template <typename TL, typename T> constexpr int find_type                       = FindType<TL, T>::value;
    template <typename TL, typename T> constexpr bool in_list                        = InList<TL, T>::value;
    template <typename TL> constexpr bool is_empty                                   = IsEmpty<TL>::value;
    template <typename TL> constexpr bool is_not_empty                               = IsNotEmpty<TL>::value;
    template <typename TL> constexpr bool is_unique                                  = IsUnique<TL>::value;
    template <typename TL> constexpr int length_of                                   = LengthOf<TL>::value;
    template <typename TL, template <typename> class UP> constexpr bool none_of      = NoneOf<TL, UP>::value;

    // Typelist function call operations

    template <typename TL, typename... Args> void call_for_each(Args&&... args) {
        MetaDetail::CallForEach<TL>()(std::forward<Args>(args)...);
    }

    template <typename TL, typename... Args> void call_for_each_with(Args&&... args) {
        MetaDetail::CallForEachWith<TL>()(std::forward<Args>(args)...);
    }

    template <typename TL, typename F, typename... Args> void function_for_each(F&& f, Args&&... args) {
        MetaDetail::FunctionForEach<TL>()(f, std::forward<Args>(args)...);
    }

    template <typename TL, typename F, typename... Args> void function_for_each_with(F&& f, Args&&... args) {
        MetaDetail::FunctionForEachWith<TL>()(f, std::forward<Args>(args)...);
    }

    // Operator detection

    #define RS_DETECT_PREFIX_OPERATOR(op, tname, cname) \
        template <typename T> using Has##tname##OperatorArchetype = decltype(op std::declval<T>()); \
        template <typename T> using Has##tname##Operator = ::RS::Meta::IsDetected<Has##tname##OperatorArchetype, T>; \
        template <typename T> constexpr bool has_##cname##_operator = Has##tname##Operator<T>::value;

    #define RS_DETECT_PREFIX_OPERATOR_REF(op, tname, cname) \
        template <typename T> using Has##tname##OperatorArchetype = decltype(op std::declval<T&>()); \
        template <typename T> using Has##tname##Operator = ::RS::Meta::IsDetected<Has##tname##OperatorArchetype, T>; \
        template <typename T> constexpr bool has_##cname##_operator = Has##tname##Operator<T>::value;

    #define RS_DETECT_POSTFIX_OPERATOR(op, tname, cname) \
        template <typename T> using Has##tname##OperatorArchetype = decltype(std::declval<T>() op); \
        template <typename T> using Has##tname##Operator = ::RS::Meta::IsDetected<Has##tname##OperatorArchetype, T>; \
        template <typename T> constexpr bool has_##cname##_operator = Has##tname##Operator<T>::value;

    #define RS_DETECT_POSTFIX_OPERATOR_REF(op, tname, cname) \
        template <typename T> using Has##tname##OperatorArchetype = decltype(std::declval<T&>() op); \
        template <typename T> using Has##tname##Operator = ::RS::Meta::IsDetected<Has##tname##OperatorArchetype, T>; \
        template <typename T> constexpr bool has_##cname##_operator = Has##tname##Operator<T>::value;

    #define RS_DETECT_BINARY_OPERATOR(op, tname, cname) \
        template <typename T, typename T2 = T> using Has##tname##OperatorArchetype = decltype(std::declval<T>() op std::declval<T2>()); \
        template <typename T, typename T2 = T> using Has##tname##Operator = ::RS::Meta::IsDetected<Has##tname##OperatorArchetype, T, T2>; \
        template <typename T, typename T2 = T> constexpr bool has_##cname##_operator = Has##tname##Operator<T, T2>::value;

    #define RS_DETECT_BINARY_OPERATOR_REF(op, tname, cname) \
        template <typename T, typename T2 = T> using Has##tname##OperatorArchetype = decltype(std::declval<T&>() op std::declval<T2>()); \
        template <typename T, typename T2 = T> using Has##tname##Operator = ::RS::Meta::IsDetected<Has##tname##OperatorArchetype, T, T2>; \
        template <typename T, typename T2 = T> constexpr bool has_##cname##_operator = Has##tname##Operator<T, T2>::value;

    RS_DETECT_PREFIX_OPERATOR_REF   (++,   PreIncrement,      pre_increment);
    RS_DETECT_PREFIX_OPERATOR_REF   (--,   PreDecrement,      pre_decrement);
    RS_DETECT_POSTFIX_OPERATOR_REF  (++,   PostIncrement,     post_increment);
    RS_DETECT_POSTFIX_OPERATOR_REF  (--,   PostDecrement,     post_decrement);
    RS_DETECT_PREFIX_OPERATOR       (+,    UnaryPlus,         unary_plus);
    RS_DETECT_PREFIX_OPERATOR       (-,    UnaryMinus,        unary_minus);
    RS_DETECT_PREFIX_OPERATOR       (*,    Dereference,       dereference);
    RS_DETECT_PREFIX_OPERATOR       (!,    LogicalNot,        logical_not);
    RS_DETECT_PREFIX_OPERATOR       (~,    BitwiseNot,        bitwise_not);
    RS_DETECT_BINARY_OPERATOR       (+,    Plus,              plus);
    RS_DETECT_BINARY_OPERATOR       (-,    Minus,             minus);
    RS_DETECT_BINARY_OPERATOR       (*,    Multiply,          multiply);
    RS_DETECT_BINARY_OPERATOR       (/,    Divide,            divide);
    RS_DETECT_BINARY_OPERATOR       (%,    Remainder,         remainder);
    RS_DETECT_BINARY_OPERATOR       (&,    BitwiseAnd,        bitwise_and);
    RS_DETECT_BINARY_OPERATOR       (|,    BitwiseOr,         bitwise_or);
    RS_DETECT_BINARY_OPERATOR       (^,    BitwiseXor,        bitwise_xor);
    RS_DETECT_BINARY_OPERATOR       (<<,   LeftShift,         left_shift);
    RS_DETECT_BINARY_OPERATOR       (>>,   RightShift,        right_shift);
    RS_DETECT_BINARY_OPERATOR       (==,   Equal,             equal);
    RS_DETECT_BINARY_OPERATOR       (!=,   NotEqual,          not_equal);
    RS_DETECT_BINARY_OPERATOR       (<,    LessThan,          less_than);
    RS_DETECT_BINARY_OPERATOR       (>,    GreaterThan,       greater_than);
    RS_DETECT_BINARY_OPERATOR       (<=,   LessOrEqual,       less_or_equal);
    RS_DETECT_BINARY_OPERATOR       (>=,   GreaterOrEqual,    greater_or_equal);
    RS_DETECT_BINARY_OPERATOR       (&&,   LogicalAnd,        logical_and);
    RS_DETECT_BINARY_OPERATOR       (||,   LogicalOr,         logical_or);
    RS_DETECT_BINARY_OPERATOR_REF   (=,    Assign,            assign);
    RS_DETECT_BINARY_OPERATOR_REF   (+=,   PlusAssign,        plus_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (-=,   MinusAssign,       minus_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (*=,   MultiplyAssign,    multiply_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (/=,   DivideAssign,      divide_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (%=,   RemainderAssign,   remainder_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (&=,   BitwiseAndAssign,  bitwise_and_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (|=,   BitwiseOrAssign,   bitwise_or_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (^=,   BitwiseXorAssign,  bitwise_xor_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (<<=,  LeftShiftAssign,   left_shift_assign);
    RS_DETECT_BINARY_OPERATOR_REF   (>>=,  RightShiftAssign,  right_shift_assign);

    template <typename T, typename T2> using HasIndexOperatorArchetype = decltype(std::declval<T&>()[std::declval<T2>()]);
    template <typename T, typename T2> using HasIndexOperator = IsDetected<HasIndexOperatorArchetype, T, T2>;
    template <typename T, typename T2> constexpr bool has_index_operator = HasIndexOperator<T, T2>::value;

    template <typename T> using HasIncrementOperators = And<
        HasPreIncrementOperator<T>,
        HasPostIncrementOperator<T>
    >;

    template <typename T> using HasStepOperators = And<
        HasIncrementOperators<T>,
        HasPreDecrementOperator<T>,
        HasPostDecrementOperator<T>
    >;

    template <typename T, typename T2 = T> using HasAdditiveOperators = And<
        HasPlusOperator<T, T2>,
        HasMinusOperator<T, T2>,
        HasPlusAssignOperator<T, T2>,
        HasMinusAssignOperator<T, T2>
    >;

    template <typename T, typename T2 = T> using HasMultiplicativeOperators = And<
        HasMultiplyOperator<T, T2>,
        HasDivideOperator<T, T2>,
        HasMultiplyAssignOperator<T, T2>,
        HasDivideAssignOperator<T, T2>
    >;

    template <typename T> using HasArithmeticOperators = And<
        HasAdditiveOperators<T>,
        HasUnaryMinusOperator<T>,
        HasMultiplyOperator<T>,
        HasDivideOperator<T>,
        HasMultiplyAssignOperator<T>,
        HasDivideAssignOperator<T>
    >;

    template <typename T> using HasIntegerArithmeticOperators = And<
        HasArithmeticOperators<T>,
        HasStepOperators<T>,
        HasRemainderOperator<T>,
        HasRemainderAssignOperator<T>
    >;

    template <typename T, typename T2 = T> using HasLinearOperators = And<
        HasAdditiveOperators<T, T2>,
        HasStepOperators<T>,
        HasPlusOperator<T2, T>,
        HasMinusOperator<T>
    >;

    template <typename T, typename T2 = T> using HasScalingOperators = And<
        HasMultiplicativeOperators<T, T2>,
        HasMultiplyOperator<T2, T>,
        HasDivideOperator<T>
    >;

    template <typename T> using HasBitwiseOperators = And<
        HasBitwiseNotOperator<T>,
        HasBitwiseAndOperator<T>,
        HasBitwiseXorOperator<T>,
        HasBitwiseOrOperator<T>,
        HasLeftShiftOperator<T, int>,
        HasRightShiftOperator<T, int>,
        HasBitwiseAndAssignOperator<T>,
        HasBitwiseXorAssignOperator<T>,
        HasBitwiseOrAssignOperator<T>,
        HasRightShiftAssignOperator<T, int>,
        HasLeftShiftAssignOperator<T, int>
    >;

    template <typename T, typename T2 = T> using HasEqualityOperators = And<
        HasEqualOperator<T, T2>,
        HasNotEqualOperator<T, T2>
    >;

    template <typename T, typename T2 = T> using HasComparisonOperators = And<
        HasEqualityOperators<T, T2>,
        HasLessThanOperator<T, T2>,
        HasGreaterThanOperator<T, T2>,
        HasLessOrEqualOperator<T, T2>,
        HasGreaterOrEqualOperator<T, T2>
    >;

    template <typename T> constexpr bool has_increment_operators = HasIncrementOperators<T>::value;
    template <typename T> constexpr bool has_step_operators = HasStepOperators<T>::value;
    template <typename T, typename T2 = T> constexpr bool has_additive_operators = HasAdditiveOperators<T, T2>::value;
    template <typename T, typename T2 = T> constexpr bool has_multiplicative_operators = HasMultiplicativeOperators<T, T2>::value;
    template <typename T> constexpr bool has_arithmetic_operators = HasArithmeticOperators<T>::value;
    template <typename T> constexpr bool has_integer_arithmetic_operators = HasIntegerArithmeticOperators<T>::value;
    template <typename T, typename T2 = T> constexpr bool has_linear_operators = HasLinearOperators<T, T2>::value;
    template <typename T, typename T2 = T> constexpr bool has_scaling_operators = HasScalingOperators<T, T2>::value;
    template <typename T> constexpr bool has_bitwise_operators = HasBitwiseOperators<T>::value;
    template <typename T, typename T2 = T> constexpr bool has_equality_operators = HasEqualityOperators<T, T2>::value;
    template <typename T, typename T2 = T> constexpr bool has_comparison_operators = HasComparisonOperators<T, T2>::value;

    // Function detection

    #define RS_DETECT_FUNCTION(func) \
        template <typename... Args> using HasFunctionArchetype_##func = decltype(func(std::declval<Args>()...)); \
        template <typename... Args> using HasFunction_##func = ::RS::Meta::IsDetected<HasFunctionArchetype_##func, Args...>; \
        template <typename... Args> constexpr bool has_function_##func = HasFunction_##func<Args...>::value;

    #define RS_DETECT_STD_FUNCTION(func) \
        template <typename... Args> using HasStdFunctionArchetype_##func = decltype(std::func(std::declval<Args>()...)); \
        template <typename... Args> using HasStdFunction_##func = ::RS::Meta::IsDetected<HasStdFunctionArchetype_##func, Args...>; \
        template <typename... Args> constexpr bool has_function_std_##func = HasStdFunction_##func<Args...>::value;

    #define RS_DETECT_METHOD(func) \
        template <typename T, typename... Args> using HasMethodArchetype_##func = decltype(std::declval<T>().func(std::declval<Args>()...)); \
        template <typename T, typename... Args> using HasMethod_##func = ::RS::Meta::IsDetected<HasMethodArchetype_##func, T, Args...>; \
        template <typename T, typename... Args> constexpr bool has_method_##func = HasMethod_##func<T, Args...>::value;

    #define RS_DETECT_MEMBER(name) \
        template <typename T> using HasMemberArchetype_##name = decltype(std::declval<T>().name); \
        template <typename T> using HasMember_##name = ::RS::Meta::IsDetected<HasMemberArchetype_##name, T>; \
        template <typename T> constexpr bool has_member_##name = HasMember_##name<T>::value;

    #define RS_DETECT_MEMBER_TYPE(type) \
        template <typename T> using HasMemberTypeArchetype_##type = typename T::type; \
        template <typename T> using HasMemberType_##type = ::RS::Meta::IsDetected<HasMemberTypeArchetype_##type, T>; \
        template <typename T> constexpr bool has_member_type_##type = HasMemberType_##type<T>::value;

    // Type categories

    namespace MetaDetail {

        template <typename T> using IsHashableArchetype = decltype(std::hash<T>()(std::declval<const T&>()));

        template <typename T, bool Iter = IsDetected<IteratorCategory, T>::value> struct ExtendedIteratorCategory: ReturnT<void> {};
        template <typename T> struct ExtendedIteratorCategory<T, true>: ReturnT<IteratorCategory<T>> {};

        template <typename T, typename Cat = typename ExtendedIteratorCategory<T>::type> struct IteratorLevelType: IntT<0> {};
        template <typename T> struct IteratorLevelType<T, std::input_iterator_tag>: IntT<1> {};
        template <typename T> struct IteratorLevelType<T, std::forward_iterator_tag>: IntT<2> {};
        template <typename T> struct IteratorLevelType<T, std::bidirectional_iterator_tag>: IntT<3> {};
        template <typename T> struct IteratorLevelType<T, std::random_access_iterator_tag>: IntT<4> {};

        template <typename T> using HasStdSwapArchetype = decltype(std::swap(std::declval<T&>(), std::declval<T&>()));
        template <typename T> using HasAdlSwapArchetype = decltype(swap(std::declval<T&>(), std::declval<T&>()));

        template <typename T> using HasFirstFieldArchetype = decltype(std::declval<T&>().first);
        template <typename T> using HasSecondFieldArchetype = decltype(std::declval<T&>().second);

        template <typename T> using HasBeginMethodArchetype = decltype(std::declval<T>().begin());
        template <typename T> using HasEndMethodArchetype = decltype(std::declval<T>().end());
        template <typename T> using HasIteratorArchetype = typename T::iterator;
        template <typename T> using HasValueTypeArchetype = typename T::value_type;
        template <typename T, typename... Args> using HasInsertMethodArchetype = decltype(std::declval<T>().insert(std::declval<Args>()...));

    }

    template <typename T> using IsHashable = IsDetected<MetaDetail::IsHashableArchetype, T>;

    template <typename T> struct IsForwardIterator: BoolT<(MetaDetail::IteratorLevelType<T>::value >= 2)> {};
    template <typename T> struct IsBidirectionalIterator: BoolT<(MetaDetail::IteratorLevelType<T>::value >= 3)> {};
    template <typename T> struct IsRandomAccessIterator: BoolT<(MetaDetail::IteratorLevelType<T>::value >= 4)> {};

    template <typename T, bool = IsIterator<T>::value> struct IsMutableIterator: std::false_type {};
    template <typename T> struct IsMutableIterator<T, true>: Not<std::is_const<typename std::remove_pointer_t<decltype(&*std::declval<T>())>>> {};

    template <typename T> struct IsMutableRange: IsMutableIterator<typename MetaDetail::RangeIteratorType<T>::type> {};

    template <typename T> using IsContainer = And<IsDetected<MetaDetail::HasBeginMethodArchetype, T>, IsDetected<MetaDetail::HasEndMethodArchetype, T>,
        IsDetected<MetaDetail::HasIteratorArchetype, T>, IsDetected<MetaDetail::HasValueTypeArchetype, T>>;

    template <typename T, bool = IsContainer<T>::value> struct IsInsertableContainer: std::false_type {};
    template <typename T> struct IsInsertableContainer<T, true>: IsDetected<MetaDetail::HasInsertMethodArchetype, T&, typename T::iterator, typename T::value_type> {};

    template <typename T> using IsPair = And<IsDetected<MetaDetail::HasFirstFieldArchetype, T>, IsDetected<MetaDetail::HasSecondFieldArchetype, T>>;

    template <typename T> using IsSwappable = Or<IsDetected<MetaDetail::HasStdSwapArchetype, T>, IsDetected<MetaDetail::HasAdlSwapArchetype, T>>;

    template <typename T> constexpr bool is_hashable = IsHashable<T>::value;
    template <typename T> constexpr bool is_forward_iterator = IsForwardIterator<T>::value;
    template <typename T> constexpr bool is_bidirectional_iterator = IsBidirectionalIterator<T>::value;
    template <typename T> constexpr bool is_random_access_iterator = IsRandomAccessIterator<T>::value;
    template <typename T> constexpr bool is_mutable_iterator = IsMutableIterator<T>::value;
    template <typename T> constexpr bool is_mutable_range = IsMutableRange<T>::value;
    template <typename T> constexpr bool is_container = IsContainer<T>::value;
    template <typename T> constexpr bool is_insertable_container = IsInsertableContainer<T>::value;
    template <typename T> constexpr bool is_pair = IsPair<T>::value;
    template <typename T> constexpr bool is_swappable = IsSwappable<T>::value;

    // Iterator utilities

    using IteratorCategoryList = Typelist<
        std::input_iterator_tag,
        std::output_iterator_tag,
        std::forward_iterator_tag,
        std::bidirectional_iterator_tag,
        std::random_access_iterator_tag
    >;

    template <typename T> struct IsIteratorCategory: std::false_type {};
    template <> struct IsIteratorCategory<std::input_iterator_tag>: std::true_type {};
    template <> struct IsIteratorCategory<std::output_iterator_tag>: std::true_type {};
    template <> struct IsIteratorCategory<std::forward_iterator_tag>: std::true_type {};
    template <> struct IsIteratorCategory<std::bidirectional_iterator_tag>: std::true_type {};
    template <> struct IsIteratorCategory<std::random_access_iterator_tag>: std::true_type {};
    template <typename T> constexpr bool is_iterator_category = IsIteratorCategory<T>::value;

    template <typename T, char = is_iterator<T> ? 'I' : is_range<T> ? 'R' : is_iterator_category<T> ? 'C' : 'X'> struct GetIteratorCategoryType;
    template <typename IRC> struct GetIteratorCategoryType<IRC, 'C'> { using type = IRC; };
    template <typename IRC> struct GetIteratorCategoryType<IRC, 'I'> { using type = typename std::iterator_traits<IRC>::iterator_category; };
    template <typename IRC> struct GetIteratorCategoryType<IRC, 'R'> { using type = typename std::iterator_traits<RangeIterator<IRC>>::iterator_category; };
    template <typename IRC> using GetIteratorCategory = typename GetIteratorCategoryType<IRC>::type;

    template <typename IRC1, typename IRC2> struct CategoryIsEqual: std::is_same<GetIteratorCategory<IRC1>, GetIteratorCategory<IRC2>> {};
    template <typename IRC1, typename IRC2> struct CategoryIsLess {
        static constexpr int index1 = find_type<IteratorCategoryList, GetIteratorCategory<IRC1>>;
        static constexpr int index2 = find_type<IteratorCategoryList, GetIteratorCategory<IRC2>>;
        static constexpr bool value = index1 + index2 != 1 && (index1 < index2);
    };
    template <typename IRC1, typename IRC2> constexpr bool category_is_equal = CategoryIsEqual<IRC1, IRC2>::value;
    template <typename IRC1, typename IRC2> constexpr bool category_is_less = CategoryIsLess<IRC1, IRC2>::value;

    template <typename... IRCs> struct MinCategoryType { using type = GetIteratorCategory<MinType<Typelist<IRCs...>, CategoryIsLess>>; };
    template <typename... IRCs> using MinCategory = typename MinCategoryType<IRCs...>::type;

}

namespace RS {

    using Meta::Nil;

}

namespace std {

    template <> struct hash<RS::Meta::Nil> {
        using argument_type = RS::Meta::Nil;
        using result_type = size_t;
        size_t operator()(RS::Meta::Nil) const noexcept { return 0; }
    };

}
