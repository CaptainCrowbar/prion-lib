#pragma once

#include "rs-core/range-core.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // const_ptr, dynamic_ptr, reinterpret_ptr, static_ptr

    template <typename T>
    struct ConstPtrObject:
    AlgorithmBase<ConstPtrObject<T>> {};

    template <typename T>
    struct DynamicPtrObject:
    AlgorithmBase<DynamicPtrObject<T>> {};

    template <typename T>
    struct ReinterpretPtrObject:
    AlgorithmBase<ReinterpretPtrObject<T>> {};

    template <typename T>
    struct StaticPtrObject:
    AlgorithmBase<StaticPtrObject<T>> {};

    template <typename Range, typename T>
    class ConstPtrIterator:
    public FlexibleRandomAccessIterator<ConstPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = decltype(generic_const_cast<T>(*std::declval<underlying_iterator>()));
        ConstPtrIterator() = default;
        ConstPtrIterator(underlying_iterator i): iter(i), value{} {}
        const value_type& operator*() const noexcept {
            if (! value)
                value = generic_const_cast<T>(*iter);
            return value;
        }
        ConstPtrIterator& operator++() { ++iter; value = {}; return *this; }
        ConstPtrIterator& operator--() { --iter; value = {}; return *this; }
        ConstPtrIterator& operator+=(ptrdiff_t rhs) { iter += rhs; value = {}; return *this; }
        ptrdiff_t operator-(const ConstPtrIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const ConstPtrIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable value_type value;
    };

    template <typename Range, typename T>
    class DynamicPtrIterator:
    public FlexibleRandomAccessIterator<DynamicPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = decltype(generic_dynamic_cast<T>(*std::declval<underlying_iterator>()));
        DynamicPtrIterator() = default;
        DynamicPtrIterator(underlying_iterator i): iter(i), value{} {}
        const value_type& operator*() const noexcept {
            if (! value)
                value = generic_dynamic_cast<T>(*iter);
            return value;
        }
        DynamicPtrIterator& operator++() { ++iter; value = {}; return *this; }
        DynamicPtrIterator& operator--() { --iter; value = {}; return *this; }
        DynamicPtrIterator& operator+=(ptrdiff_t rhs) { iter += rhs; value = {}; return *this; }
        ptrdiff_t operator-(const DynamicPtrIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const DynamicPtrIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable value_type value;
    };

    template <typename Range, typename T>
    class ReinterpretPtrIterator:
    public FlexibleRandomAccessIterator<ReinterpretPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = T*;
        ReinterpretPtrIterator() = default;
        ReinterpretPtrIterator(underlying_iterator i): iter(i), value{} {}
        const value_type& operator*() const noexcept {
            if (! value)
                value = reinterpret_cast<T*>(*iter);
            return value;
        }
        ReinterpretPtrIterator& operator++() { ++iter; value = {}; return *this; }
        ReinterpretPtrIterator& operator--() { --iter; value = {}; return *this; }
        ReinterpretPtrIterator& operator+=(ptrdiff_t rhs) { iter += rhs; value = {}; return *this; }
        ptrdiff_t operator-(const ReinterpretPtrIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const ReinterpretPtrIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable value_type value;
    };

    template <typename Range, typename T>
    class StaticPtrIterator:
    public FlexibleRandomAccessIterator<StaticPtrIterator<Range, T>, const T> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = decltype(generic_static_cast<T>(*std::declval<underlying_iterator>()));
        StaticPtrIterator() = default;
        StaticPtrIterator(underlying_iterator i): iter(i), value{} {}
        const value_type& operator*() const noexcept {
            if (! value)
                value = generic_static_cast<T>(*iter);
            return value;
        }
        StaticPtrIterator& operator++() { ++iter; value = {}; return *this; }
        StaticPtrIterator& operator--() { --iter; value = {}; return *this; }
        StaticPtrIterator& operator+=(ptrdiff_t rhs) { iter += rhs; value = {}; return *this; }
        ptrdiff_t operator-(const StaticPtrIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const StaticPtrIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable value_type value;
    };

    template <typename Range, typename T>
    Irange<ConstPtrIterator<Range, T>> operator>>(const Range& lhs, ConstPtrObject<T> /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename Range, typename T>
    Irange<DynamicPtrIterator<Range, T>> operator>>(const Range& lhs, DynamicPtrObject<T> /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename Range, typename T>
    Irange<ReinterpretPtrIterator<Range, T>> operator>>(const Range& lhs, ReinterpretPtrObject<T> /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename Range, typename T>
    Irange<StaticPtrIterator<Range, T>> operator>>(const Range& lhs, StaticPtrObject<T> /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ConstPtrObject<T>) {
        using std::begin;
        using std::end;
        std::transform(begin(lhs), end(lhs), begin(lhs), generic_const_cast<T>);
        return lhs;
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, DynamicPtrObject<T>) {
        using std::begin;
        using std::end;
        std::transform(begin(lhs), end(lhs), begin(lhs), generic_dynamic_cast<T>);
        return lhs;
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ReinterpretPtrObject<T>) {
        using std::begin;
        using std::end;
        std::transform(begin(lhs), end(lhs), begin(lhs), [] (auto* p) { return reinterpret_cast<T*>(p); });
        return lhs;
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, StaticPtrObject<T>) {
        using std::begin;
        using std::end;
        std::transform(begin(lhs), end(lhs), begin(lhs), generic_static_cast<T>);
        return lhs;
    }

    template <typename T>
    inline ConstPtrObject<T> const_ptr() noexcept {
        return {};
    };

    template <typename T>
    inline DynamicPtrObject<T> dynamic_ptr() noexcept {
        return {};
    };

    template <typename T>
    inline ReinterpretPtrObject<T> reinterpret_ptr() noexcept {
        return {};
    };

    template <typename T>
    inline StaticPtrObject<T> static_ptr() noexcept {
        return {};
    };

    // construct, initialize

    template <typename T, bool Init>
    struct ConstructObject;

    template <typename T>
    struct ConstructObject<T, false>:
    AlgorithmBase<ConstructObject<T, false>> {
        template <typename... Args> T operator()(const Args&... args) const { return T(args...); }
    };

    template <typename T>
    struct ConstructObject<T, true>:
    AlgorithmBase<ConstructObject<T, true>> {
        template <typename... Args> T operator()(const Args&... args) const { return T{args...}; }
    };

    template <typename Range, typename T, bool Init>
    class ConstructIterator:
    public FlexibleRandomAccessIterator<ConstructIterator<Range, T, Init>, const T> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        ConstructIterator() = default;
        ConstructIterator(underlying_iterator i): iter(i), value(), ok(false) {}
        const T& operator*() const noexcept {
            if (! ok) {
                value = tuple_invoke(ConstructObject<T, Init>(), *iter);
                ok = true;
            }
            return value;
        }
        ConstructIterator& operator++() { ++iter; ok = false; return *this; }
        ConstructIterator& operator--() { --iter; ok = false; return *this; }
        ConstructIterator& operator+=(ptrdiff_t rhs) { iter += rhs; ok = false; return *this; }
        ptrdiff_t operator-(const ConstructIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const ConstructIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable T value;
        mutable bool ok;
    };

    template <typename Range, typename T, bool Init>
    Irange<ConstructIterator<Range, T, Init>> operator>>(const Range& lhs, ConstructObject<T, Init> /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename T>
    inline ConstructObject<T, false> construct() noexcept {
        return {};
    };

    template <typename T>
    inline ConstructObject<T, true> initialize() noexcept {
        return {};
    };

    // convert

    template <typename T>
    struct ConvertObject:
    AlgorithmBase<ConvertObject<T>> {};

    template <typename Range, typename T>
    class ConvertIterator:
    public FlexibleRandomAccessIterator<ConvertIterator<Range, T>, const T> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        ConvertIterator() = default;
        ConvertIterator(underlying_iterator i): iter(i), value(), ok(false) {}
        const T& operator*() const noexcept {
            if (! ok) {
                value = static_cast<T>(*iter);
                ok = true;
            }
            return value;
        }
        ConvertIterator& operator++() { ++iter; ok = false; return *this; }
        ConvertIterator& operator--() { --iter; ok = false; return *this; }
        ConvertIterator& operator+=(ptrdiff_t rhs) { iter += rhs; ok = false; return *this; }
        ptrdiff_t operator-(const ConvertIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const ConvertIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable T value;
        mutable bool ok;
    };

    template <typename Range, typename T>
    Irange<ConvertIterator<Range, T>> operator>>(const Range& lhs, ConvertObject<T> /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename ForwardRange, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ConvertObject<T>) {
        using std::begin;
        using std::end;
        std::transform(begin(lhs), end(lhs), begin(lhs), [] (auto& t) { return static_cast<T>(t); });
        return lhs;
    }

    template <typename T>
    inline ConvertObject<T> convert() noexcept {
        return {};
    };

    // dereference

    struct DereferenceObject:
    AlgorithmBase<DereferenceObject> {};

    template <typename Range>
    class DereferenceIterator:
    public FlexibleRandomAccessIterator<DereferenceIterator<Range>, const std::decay_t<decltype(*std::declval<Meta::RangeValue<Range>>())>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        DereferenceIterator() = default;
        DereferenceIterator(underlying_iterator i): iter(i) {}
        const auto& operator*() const noexcept { return **iter; }
        DereferenceIterator& operator++() { ++iter; return *this; }
        DereferenceIterator& operator--() { --iter; return *this; }
        DereferenceIterator& operator+=(ptrdiff_t rhs) { iter += rhs; return *this; }
        ptrdiff_t operator-(const DereferenceIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const DereferenceIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
    };

    template <typename Range>
    Irange<DereferenceIterator<Range>> operator>>(const Range& lhs, DereferenceObject /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    constexpr DereferenceObject dereference = {};

    // indexed

    template <typename Integer>
    struct IndexedObject:
    AlgorithmBase<IndexedObject<Integer>> {
        Integer init = 0, delta = 1;
        template <typename I2> IndexedObject<I2> operator()(I2 i, I2 d = I2(1)) const {
            IndexedObject<I2> o;
            o.init = i;
            o.delta = d;
            return o;
        }
    };

    template <typename Range, typename Integer>
    class IndexedIterator:
    public ForwardIterator<IndexedIterator<Range, Integer>, const std::pair<Integer, Meta::RangeValue<Range>>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = std::pair<Integer, Meta::RangeValue<Range>>;
        IndexedIterator() = default;
        IndexedIterator(underlying_iterator i, Integer n, Integer d): iter(i), key(n), delta(d), value(), ok(false) {}
        const value_type& operator*() const noexcept {
            if (! ok) {
                value = std::make_pair(key, *iter);
                ok = true;
            }
            return value;
        }
        IndexedIterator& operator++() { ++iter; key += delta; ok = false; return *this; }
        bool operator==(const IndexedIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        Integer key, delta;
        mutable value_type value;
        mutable bool ok;
    };

    template <typename Range, typename Integer>
    Irange<IndexedIterator<Range, Integer>> operator>>(const Range& lhs, IndexedObject<Integer> rhs) {
        using std::begin;
        using std::end;
        return {{begin(lhs), rhs.init, rhs.delta}, {end(lhs), 0, 0}};
    }

    constexpr IndexedObject<int> indexed = {};

    // iterators

    struct IteratorsObject:
    AlgorithmBase<IteratorsObject> {};

    template <typename Range>
    class IteratorsIterator:
    public FlexibleRandomAccessIterator<IteratorsIterator<Range>, const Meta::RangeIterator<Range>> {
    public:
        using underlying_iterator = Meta::RangeIterator<Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        IteratorsIterator() = default;
        IteratorsIterator(underlying_iterator i): iter(i) {}
        const auto& operator*() const noexcept { return iter; }
        IteratorsIterator& operator++() { ++iter; return *this; }
        IteratorsIterator& operator--() { --iter; return *this; }
        IteratorsIterator& operator+=(ptrdiff_t rhs) { iter += rhs; return *this; }
        ptrdiff_t operator-(const IteratorsIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const IteratorsIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
    };

    template <typename Range>
    Irange<IteratorsIterator<Range>> operator>>(Range& lhs, IteratorsObject /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename Range>
    Irange<IteratorsIterator<const Range>> operator>>(const Range& lhs, IteratorsObject /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    constexpr IteratorsObject iterators = {};

    // keys, values

    template <bool Keys>
    struct KeysValuesObject:
    AlgorithmBase<KeysValuesObject<Keys>> {};

    template <typename Range, bool Keys>
    struct KeyValueType;

    template <typename Range>
    struct KeyValueType<Range, true> {
        using type = typename Meta::RangeValue<Range>::first_type;
        static const type& get(const Meta::RangeValue<Range>& rv) { return rv.first; }
    };

    template <typename Range>
    struct KeyValueType<Range, false> {
        using type = typename Meta::RangeValue<Range>::second_type;
        static const type& get(const Meta::RangeValue<Range>& rv) { return rv.second; }
    };

    template <typename Range, bool Keys>
    class KeyValueIterator:
    public FlexibleRandomAccessIterator<KeyValueIterator<Range, Keys>, const typename KeyValueType<Range, Keys>::type> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        KeyValueIterator() = default;
        KeyValueIterator(underlying_iterator i): iter(i) {}
        const auto& operator*() const noexcept { return KeyValueType<Range, Keys>::get(*iter); }
        KeyValueIterator& operator++() { ++iter; return *this; }
        KeyValueIterator& operator--() { --iter; return *this; }
        KeyValueIterator& operator+=(ptrdiff_t rhs) { iter += rhs; return *this; }
        ptrdiff_t operator-(const KeyValueIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const KeyValueIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
    };

    template <typename Range, bool Keys>
    Irange<KeyValueIterator<Range, Keys>> operator>>(const Range& lhs, KeysValuesObject<Keys> /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    constexpr KeysValuesObject<true> keys = {};
    constexpr KeysValuesObject<false> values = {};

    // map, map_pairs

    template <typename UnaryFunction>
    struct MapObject:
    AlgorithmBase<MapObject<UnaryFunction>> {
        UnaryFunction fun;
        MapObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename BinaryFunction>
    struct MapPairsObject:
    AlgorithmBase<MapPairsObject<BinaryFunction>> {
        BinaryFunction fun;
        MapPairsObject(const BinaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    class MapIterator:
    public FlexibleRandomAccessIterator<MapIterator<Range, UnaryFunction>, const InvokeResult<UnaryFunction, Meta::RangeValue<Range>>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = InvokeResult<UnaryFunction, Meta::RangeValue<Range>>;
        using function_type = std::function<value_type(const Meta::RangeValue<Range>&)>;
        MapIterator() = default;
        MapIterator(underlying_iterator i, UnaryFunction f): iter(i), fun(f), value(), ok(false) {}
        const value_type& operator*() const { if (! ok) { value = fun(*iter); ok = true; } return value; }
        MapIterator& operator++() { ++iter; ok = false; return *this; }
        MapIterator& operator--() { --iter; ok = false; return *this; }
        MapIterator& operator+=(ptrdiff_t rhs) { iter += rhs; ok = false; return *this; }
        ptrdiff_t operator-(const MapIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const MapIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        function_type fun;
        mutable value_type value;
        mutable bool ok;
    };

    template <typename Range, typename BinaryFunction>
    class MapPairsIterator:
    public FlexibleRandomAccessIterator<MapPairsIterator<Range, BinaryFunction>,
        const InvokeResult<BinaryFunction, typename Meta::RangeValue<Range>::first_type, typename Meta::RangeValue<Range>::second_type>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using first_type = typename Meta::RangeValue<Range>::first_type;
        using second_type = typename Meta::RangeValue<Range>::second_type;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = InvokeResult<BinaryFunction, first_type, second_type>;
        using function_type = std::function<value_type(const first_type&, const second_type&)>;
        MapPairsIterator() = default;
        MapPairsIterator(underlying_iterator i, BinaryFunction f): iter(i), fun(f), value(), ok(false) {}
        const value_type& operator*() const { if (! ok) { value = fun(iter->first, iter->second); ok = true; } return value; }
        MapPairsIterator& operator++() { ++iter; ok = false; return *this; }
        MapPairsIterator& operator--() { --iter; ok = false; return *this; }
        MapPairsIterator& operator+=(ptrdiff_t rhs) { iter += rhs; ok = false; return *this; }
        ptrdiff_t operator-(const MapPairsIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const MapPairsIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        function_type fun;
        mutable value_type value;
        mutable bool ok;
    };

    template <typename Range, typename UnaryFunction>
    Irange<MapIterator<Range, UnaryFunction>> operator>>(const Range& lhs, MapObject<UnaryFunction> rhs) {
        using std::begin;
        using std::end;
        return {{begin(lhs), rhs.fun}, {end(lhs), rhs.fun}};
    }

    template <typename Range, typename BinaryFunction>
    Irange<MapPairsIterator<Range, BinaryFunction>> operator>>(const Range& lhs, MapPairsObject<BinaryFunction> rhs) {
        using std::begin;
        using std::end;
        return {{begin(lhs), rhs.fun}, {end(lhs), rhs.fun}};
    }

    template <typename ForwardRange, typename UnaryFunction>
    ForwardRange& operator<<(ForwardRange& lhs, MapObject<UnaryFunction> rhs) {
        using std::begin;
        using std::end;
        std::transform(begin(lhs), end(lhs), begin(lhs), rhs.fun);
        return lhs;
    }

    template <typename UnaryFunction>
    inline MapObject<UnaryFunction> map(UnaryFunction f) {
        return f;
    }

    template <typename BinaryFunction>
    inline MapPairsObject<BinaryFunction> map_pairs(BinaryFunction f) {
        return f;
    }

    // replace, replace_if

    template <typename Predicate, typename T>
    struct ReplaceObject:
    AlgorithmBase<ReplaceObject<Predicate, T>> {
        Predicate pred;
        T subst;
        ReplaceObject(const Predicate& p, const T& t): pred(p), subst(t) {}
    };

    template <typename Range, typename Predicate>
    class ReplaceIterator:
    public FlexibleRandomAccessIterator<ReplaceIterator<Range, Predicate>, const Meta::RangeValue<Range>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = Meta::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        ReplaceIterator() = default;
        template <typename T> ReplaceIterator(underlying_iterator i, Predicate p, const T& t): iter(i), pred(p), subst(t) {}
        const value_type& operator*() const { if (pred(*iter)) return subst; else return *iter; }
        ReplaceIterator& operator++() { ++iter; return *this; }
        ReplaceIterator& operator--() { --iter; return *this; }
        ReplaceIterator& operator+=(ptrdiff_t rhs) { iter += rhs; return *this; }
        ptrdiff_t operator-(const ReplaceIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const ReplaceIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        predicate_type pred;
        value_type subst;
    };

    template <typename Range, typename Predicate, typename T>
    Irange<ReplaceIterator<Range, Predicate>> operator>>(const Range& lhs, const ReplaceObject<Predicate, T>& rhs) {
        using std::begin;
        using std::end;
        return {{begin(lhs), rhs.pred, rhs.subst}, {end(lhs), rhs.pred, rhs.subst}};
    }

    template <typename ForwardRange, typename Predicate, typename T>
    ForwardRange& operator<<(ForwardRange& lhs, ReplaceObject<Predicate, T> rhs) {
        using std::begin;
        using std::end;
        Meta::RangeValue<ForwardRange> s = rhs.subst;
        std::replace_if(begin(lhs), end(lhs), rhs.pred, s);
        return lhs;
    }

    template <typename Predicate, typename T>
    inline ReplaceObject<Predicate, T> replace_if(Predicate p, const T& t2) {
        return {p, t2};
    }

    template <typename T1, typename T2>
    inline auto replace(const T1& t1, const T2& t2) {
        return replace_if([t1] (auto& x ) { return x == t1; }, t2);
    }

    // stringify

    struct StringifyObject:
    AlgorithmBase<StringifyObject> {};

    template <typename Range>
    class StringifyIterator:
    public FlexibleRandomAccessIterator<StringifyIterator<Range>, const std::string> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        StringifyIterator() = default;
        StringifyIterator(underlying_iterator i): iter(i), value(), ok(false) {}
        const std::string& operator*() const noexcept {
            using RS::to_str;
            if (! ok) {
                value = to_str(*iter);
                ok = true;
            }
            return value;
        }
        StringifyIterator& operator++() { ++iter; ok = false; return *this; }
        StringifyIterator& operator--() { --iter; ok = false; return *this; }
        StringifyIterator& operator+=(ptrdiff_t rhs) { iter += rhs; ok = false; return *this; }
        ptrdiff_t operator-(const StringifyIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const StringifyIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable std::string value;
        mutable bool ok;
    };

    template <typename Range>
    Irange<StringifyIterator<Range>> operator>>(const Range& lhs, StringifyObject /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    constexpr StringifyObject stringify = {};

    // swap_pairs

    struct SwapPairsObject:
    AlgorithmBase<SwapPairsObject> {};

    template <typename PairRange>
    struct SwapPairsType {
        using type1 = std::decay_t<typename Meta::RangeValue<PairRange>::first_type>;
        using type2 = std::decay_t<typename Meta::RangeValue<PairRange>::second_type>;
        using value_type = std::pair<type2, type1>;
    };

    template <typename PairRange>
    class SwapPairsIterator:
    public FlexibleRandomAccessIterator<SwapPairsIterator<PairRange>, const typename SwapPairsType<PairRange>::value_type> {
    public:
        using underlying_iterator = Meta::RangeIterator<const PairRange>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        using value_type = typename SwapPairsType<PairRange>::value_type;
        SwapPairsIterator() = default;
        SwapPairsIterator(underlying_iterator i): iter(i), value(), ok(false) {}
        const value_type& operator*() const noexcept {
            if (! ok) {
                value = std::make_pair(iter->second, iter->first);
                ok = true;
            }
            return value;
        }
        SwapPairsIterator& operator++() { ++iter; ok = false; return *this; }
        SwapPairsIterator& operator--() { --iter; ok = false; return *this; }
        SwapPairsIterator& operator+=(ptrdiff_t rhs) { iter += rhs; ok = false; return *this; }
        ptrdiff_t operator-(const SwapPairsIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const SwapPairsIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        mutable value_type value;
        mutable bool ok;
    };

    template <typename PairRange>
    Irange<SwapPairsIterator<PairRange>> operator>>(const PairRange& lhs, SwapPairsObject /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename PairContainer>
    PairContainer& operator<<(PairContainer& lhs, SwapPairsObject /*rhs*/) {
        using std::begin;
        using std::end;
        PairContainer temp;
        lhs >> SwapPairsObject() >> append(temp);
        lhs = move(temp);
        return lhs;
    }

    constexpr SwapPairsObject swap_pairs = {};

}
