#pragma once

#include "rs-core/range-core.hpp"
#include <functional>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // adjacent_difference

    template <typename BinaryFunction>
    struct AdjacentDifferenceObject:
    AlgorithmBase<AdjacentDifferenceObject<BinaryFunction>> {
        BinaryFunction fun;
        AdjacentDifferenceObject() = default;
        AdjacentDifferenceObject(const BinaryFunction& f): fun(f) {}
        template <typename BF2> AdjacentDifferenceObject<BF2> operator()(BF2 f) const { return f; }
    };

    template <typename Range, typename BinaryFunction>
    class AdjacentDifferenceIterator:
    public ForwardIterator<AdjacentDifferenceIterator<Range, BinaryFunction>,
        InvokeResult<BinaryFunction, Meta::RangeValue<Range>, Meta::RangeValue<Range>>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = InvokeResult<BinaryFunction, Meta::RangeValue<Range>, Meta::RangeValue<Range>>;
        using function_type = std::function<value_type(const Meta::RangeValue<Range>&, const Meta::RangeValue<Range>&)>;
        AdjacentDifferenceIterator() = default;
        AdjacentDifferenceIterator(underlying_iterator b, underlying_iterator e, BinaryFunction f):
            iter1(b), iter2(b), fun(f), value(), ok(false) { if (b != e) ++iter2; }
        const value_type& operator*() const noexcept { if (! ok) { value = fun(*iter2, *iter1); ok = true; } return value; };
        AdjacentDifferenceIterator& operator++() { iter1 = iter2; ++iter2; ok = false; return *this; }
        bool operator==(const AdjacentDifferenceIterator& rhs) const noexcept { return iter2 == rhs.iter2; }
    private:
        underlying_iterator iter1, iter2;
        function_type fun;
        mutable value_type value;
        mutable bool ok;
    };

    template <typename Range, typename BinaryFunction>
    Irange<AdjacentDifferenceIterator<Range, BinaryFunction>>
    operator>>(const Range& lhs, AdjacentDifferenceObject<BinaryFunction> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename Container, typename BinaryFunction>
    Container& operator<<(Container& lhs, AdjacentDifferenceObject<BinaryFunction> rhs) {
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = move(temp);
        return lhs;
    }

    constexpr AdjacentDifferenceObject<std::minus<>> adjacent_difference = {};

    // census

    template <typename EqualityPredicate>
    struct CensusObject:
    AlgorithmBase<CensusObject<EqualityPredicate>> {
        EqualityPredicate eq;
        CensusObject() = default;
        CensusObject(const EqualityPredicate& p): eq(p) {}
        template <typename EP2> CensusObject<EP2> operator()(EP2 p) const { return p; }
    };

    template <typename Range, typename EqualityPredicate>
    class CensusIterator:
    public ForwardIterator<CensusIterator<Range, EqualityPredicate>, std::pair<Meta::RangeValue<Range>, size_t>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = std::pair<Meta::RangeValue<Range>, size_t>;
        using equality_predicate = std::function<bool(const Meta::RangeValue<Range>&, const Meta::RangeValue<Range>&)>;
        CensusIterator() = default;
        CensusIterator(underlying_iterator b, underlying_iterator e, EqualityPredicate p):
            iter(b), end(e), eq(p), value() { next(); }
        const value_type& operator*() const noexcept { return value; }
        CensusIterator& operator++() { next(); return *this; }
        bool operator==(const CensusIterator& rhs) const noexcept { return iter == rhs.iter && value.second == rhs.value.second; }
    private:
        underlying_iterator iter, end;
        equality_predicate eq;
        value_type value;
        void next() {
            value.second = 0;
            if (iter == end)
                return;
            value.first = *iter;
            do {
                ++iter;
                ++value.second;
            } while (iter != end && eq(*iter, value.first));
        }
    };

    template <typename Range, typename EqualityPredicate>
    Irange<CensusIterator<Range, EqualityPredicate>> operator>>(const Range& lhs, CensusObject<EqualityPredicate> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.eq}, {e, e, rhs.eq}};
    }

    template <typename Container, typename EqualityPredicate>
    Container& operator<<(Container& lhs, CensusObject<EqualityPredicate> rhs) {
        using std::begin;
        using std::end;
        std::partial_sum(begin(lhs), end(lhs), begin(lhs), rhs.eq);
        return lhs;
    }

    constexpr CensusObject<std::equal_to<>> census = {};

    // group

    template <typename EqualityPredicate>
    struct GroupObject:
    AlgorithmBase<GroupObject<EqualityPredicate>> {
        EqualityPredicate pred;
        GroupObject() = default;
        GroupObject(const EqualityPredicate& p): pred(p) {}
        template <typename EP2> GroupObject<EP2> operator()(EP2 p) const { return p; }
    };

    template <typename Range, typename EqualityPredicate>
    class GroupIterator:
    public ForwardIterator<GroupIterator<Range, EqualityPredicate>, std::pair<Meta::RangeValue<Range>, size_t>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Irange<underlying_iterator>;
        using predicate_type = std::function<bool(const Meta::RangeValue<Range>&, const Meta::RangeValue<Range>&)>;
        GroupIterator() = default;
        GroupIterator(underlying_iterator b, underlying_iterator e, EqualityPredicate p):
            value{b, b}, end(e), pred(p) { next(); }
        const value_type& operator*() const noexcept { return value; }
        GroupIterator& operator++() { next(); return *this; }
        bool operator==(const GroupIterator& rhs) const noexcept { return value.first == rhs.value.first; }
    private:
        value_type value;
        underlying_iterator end;
        predicate_type pred;
        void next() {
            value.first = value.second;
            if (value.first == end)
                return;
            do ++value.second;
                while (value.second != end && pred(*value.first, *value.second));
        }
    };

    template <typename Range, typename EqualityPredicate>
    Irange<GroupIterator<Range, EqualityPredicate>> operator>>(const Range& lhs, GroupObject<EqualityPredicate> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.pred}, {e, e, rhs.pred}};
    }

    constexpr GroupObject<std::equal_to<>> group = {};

    // group_by

    template <typename UnaryFunction>
    struct GroupByObject:
    AlgorithmBase<GroupByObject<UnaryFunction>> {
        UnaryFunction fun;
        GroupByObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    class GroupByIterator:
    public ForwardIterator<GroupByIterator<Range, UnaryFunction>, std::pair<Meta::RangeValue<Range>, size_t>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Irange<underlying_iterator>;
        using function_result = InvokeResult<UnaryFunction, Meta::RangeValue<Range>>;
        using function_type = std::function<function_result(const Meta::RangeValue<Range>&)>;
        GroupByIterator() = default;
        GroupByIterator(underlying_iterator b, underlying_iterator e, UnaryFunction f):
            value{b, b}, end(e), fun(f), current() {
                if (b != e)
                    current = f(*b);
                next();
            }
        const value_type& operator*() const noexcept { return value; }
        GroupByIterator& operator++() { next(); return *this; }
        bool operator==(const GroupByIterator& rhs) const noexcept { return value.first == rhs.value.first; }
    private:
        value_type value;
        underlying_iterator end;
        function_type fun;
        function_result current;
        void next() {
            value.first = value.second;
            if (value.first == end)
                return;
            auto prev = current;
            do {
                ++value.second;
                if (value.second == end)
                    break;
                current = fun(*value.second);
            } while (current == prev);
        }
    };

    template <typename Range, typename UnaryFunction>
    Irange<GroupByIterator<Range, UnaryFunction>> operator>>(const Range& lhs, GroupByObject<UnaryFunction> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename UnaryFunction>
    inline GroupByObject<UnaryFunction> group_by(UnaryFunction f) {
        return f;
    }

    // group_k

    struct GroupKObject:
    AlgorithmBase<GroupKObject> {
        size_t num;
        GroupKObject(size_t k): num(k) {}
    };

    template <typename Range>
    class GroupNIterator:
    public ForwardIterator<GroupNIterator<Range>, std::pair<Meta::RangeValue<Range>, size_t>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Irange<underlying_iterator>;
        GroupNIterator() = default;
        GroupNIterator(underlying_iterator b, underlying_iterator e, size_t k):
            value{b, b}, end(e), num(k) { next(); }
        const value_type& operator*() const noexcept { return value; }
        GroupNIterator& operator++() { next(); return *this; }
        bool operator==(const GroupNIterator& rhs) const noexcept { return value.first == rhs.value.first; }
    private:
        value_type value;
        underlying_iterator end;
        size_t num;
        void next() {
            value.first = value.second;
            if (value.first != end)
                for (size_t i = 0; i < num && value.second != end; ++i, ++value.second) {}
        }
    };

    template <typename Range>
    Irange<GroupNIterator<Range>> operator>>(const Range& lhs, GroupKObject rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.num}, {e, e, rhs.num}};
    }

    inline GroupKObject group_k(size_t k) {
        return {k == 0 ? 1 : k};
    }

    // partial_sum

    template <typename BinaryFunction>
    struct PartialSumObject:
    AlgorithmBase<PartialSumObject<BinaryFunction>> {
        BinaryFunction fun;
        PartialSumObject() = default;
        PartialSumObject(const BinaryFunction& f): fun(f) {}
        template <typename BF2> PartialSumObject<BF2> operator()(BF2 f) const { return f; }
    };

    template <typename Range, typename BinaryFunction>
    class PartialSumIterator:
    public ForwardIterator<PartialSumIterator<Range, BinaryFunction>,
        InvokeResult<BinaryFunction, Meta::RangeValue<Range>, Meta::RangeValue<Range>>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = InvokeResult<BinaryFunction, Meta::RangeValue<Range>, Meta::RangeValue<Range>>;
        using function_type = std::function<value_type(const Meta::RangeValue<Range>&, const Meta::RangeValue<Range>&)>;
        PartialSumIterator() = default;
        PartialSumIterator(underlying_iterator b, underlying_iterator e, BinaryFunction f):
            iter(b), fun(f), value(), ok(true) { if (b != e) value = *b; }
        const value_type& operator*() const noexcept { if (! ok) { value = fun(value, *iter); ok = true; } return value; };
        PartialSumIterator& operator++() { ++iter; ok = false; return *this; }
        bool operator==(const PartialSumIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        function_type fun;
        mutable value_type value;
        mutable bool ok;
    };

    template <typename Range, typename BinaryFunction>
    Irange<PartialSumIterator<Range, BinaryFunction>> operator>>(const Range& lhs, PartialSumObject<BinaryFunction> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename Container, typename BinaryFunction>
    Container& operator<<(Container& lhs, PartialSumObject<BinaryFunction> rhs) {
        using std::begin;
        using std::end;
        std::partial_sum(begin(lhs), end(lhs), begin(lhs), rhs.fun);
        return lhs;
    }

    const PartialSumObject<std::plus<>> partial_sum = {};

}
