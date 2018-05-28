#pragma once

#include "rs-core/range-core.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // compare, compare_3way, is_equal, is_equivalent

    template <typename Range2, typename ComparisonPredicate>
    struct CompareObject:
    AlgorithmBase<CompareObject<Range2, ComparisonPredicate>> {
        const Range2* range;
        ComparisonPredicate comp;
        CompareObject(const Range2* rp, const ComparisonPredicate& cp): range(rp), comp(cp) {}
        template <typename CP2> CompareObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range2, typename ComparisonPredicate>
    struct Compare3WayObject:
    AlgorithmBase<Compare3WayObject<Range2, ComparisonPredicate>> {
        const Range2* range;
        ComparisonPredicate comp;
        Compare3WayObject(const Range2* rp, const ComparisonPredicate& cp): range(rp), comp(cp) {}
        template <typename CP2> Compare3WayObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range2, typename EqualityPredicate>
    struct IsEqualObject:
    AlgorithmBase<IsEqualObject<Range2, EqualityPredicate>> {
        const Range2* range;
        EqualityPredicate comp;
        IsEqualObject(const Range2* rp, const EqualityPredicate& cp): range(rp), comp(cp) {}
        template <typename CP2> IsEqualObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range2, typename ComparisonPredicate>
    struct IsEquivalentObject:
    AlgorithmBase<IsEquivalentObject<Range2, ComparisonPredicate>> {
        const Range2* range;
        ComparisonPredicate comp;
        IsEquivalentObject(const Range2* rp, const ComparisonPredicate& cp): range(rp), comp(cp) {}
        template <typename CP2> IsEquivalentObject<Range2, CP2> operator()(const CP2& cp) const { return {range, cp}; }
    };

    template <typename Range1, typename Range2, typename ComparisonPredicate>
    bool operator>>(const Range1& lhs, const CompareObject<Range2, ComparisonPredicate>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), ie = end(lhs), j = begin(*rhs.range), je = end(*rhs.range);
        for (; i != ie && j != je; ++i, ++j) {
            if (rhs.comp(*i, *j))
                return true;
            else if (rhs.comp(*j, *i))
                return false;
        }
        return j != je;
    }

    template <typename Range1, typename Range2, typename ComparisonPredicate>
    int operator>>(const Range1& lhs, const Compare3WayObject<Range2, ComparisonPredicate>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), ie = end(lhs), j = begin(*rhs.range), je = end(*rhs.range);
        for (; i != ie && j != je; ++i, ++j) {
            if (rhs.comp(*i, *j))
                return -1;
            else if (rhs.comp(*j, *i))
                return 1;
        }
        if (i != ie)
            return 1;
        else if (j != je)
            return -1;
        else
            return 0;
    }

    template <typename Range1, typename Range2, typename EqualityPredicate>
    bool operator>>(const Range1& lhs, const IsEqualObject<Range2, EqualityPredicate>& rhs) {
        using std::begin;
        using std::end;
        static constexpr bool ra1 = Meta::category_is_equal<const Range1, std::random_access_iterator_tag>;
        static constexpr bool ra2 = Meta::category_is_equal<const Range2, std::random_access_iterator_tag>;
        auto i = begin(lhs), ie = end(lhs), j = begin(*rhs.range), je = end(*rhs.range);
        if (ra1 && ra2 && std::distance(i, ie) != std::distance(j, je))
            return false;
        for (; i != ie && j != je; ++i, ++j)
            if (! rhs.comp(*i, *j))
                return false;
        return i == ie && j == je;
    }

    template <typename Range1, typename Range2, typename ComparisonPredicate>
    bool operator>>(const Range1& lhs, const IsEquivalentObject<Range2, ComparisonPredicate>& rhs) {
        using std::begin;
        using std::end;
        static constexpr bool ra1 = Meta::category_is_equal<const Range1, std::random_access_iterator_tag>;
        static constexpr bool ra2 = Meta::category_is_equal<const Range2, std::random_access_iterator_tag>;
        auto i = begin(lhs), ie = end(lhs), j = begin(*rhs.range), je = end(*rhs.range);
        if (ra1 && ra2 && std::distance(i, ie) != std::distance(j, je))
            return false;
        for (; i != ie && j != je; ++i, ++j)
            if (rhs.comp(*i, *j) || rhs.comp(*j, *i))
                return false;
        return i == ie && j == je;
    }

    template <typename Range2>
    inline CompareObject<Range2, std::less<>> compare(Range2& r) {
        return {&r, {}};
    }

    template <typename Range2>
    inline Compare3WayObject<Range2, std::less<>> compare_3way(Range2& r) {
        return {&r, {}};
    }

    template <typename Range2>
    inline IsEqualObject<Range2, std::equal_to<>> is_equal(Range2& r) {
        return {&r, {}};
    }

    template <typename Range2>
    inline IsEquivalentObject<Range2, std::less<>> is_equivalent(Range2& r) {
        return {&r, {}};
    }

    // concat, prefix

    template <typename Range2>
    struct ConcatObject:
    AlgorithmBase<ConcatObject<Range2>> {
        const Range2* range;
        ConcatObject(const Range2* r): range(r) {}
    };

    template <typename Range2>
    struct PrefixObject:
    AlgorithmBase<PrefixObject<Range2>> {
        const Range2* range;
        PrefixObject(const Range2* r): range(r) {}
    };

    template <typename Range1, typename Range2>
    class ConcatIterator:
    public ForwardIterator<ConcatIterator<Range1, Range2>, const Meta::RangeValue<Range1>> {
    public:
        static_assert(std::is_same<Meta::RangeValue<Range1>, Meta::RangeValue<Range2>>::value);
        using left_iterator = Meta::RangeIterator<const Range1>;
        using right_iterator = Meta::RangeIterator<const Range2>;
        using iterator_category = Meta::MinCategory<Range1, Range2, std::forward_iterator_tag>;
        ConcatIterator() = default;
        ConcatIterator(left_iterator b1, left_iterator e1, right_iterator b2): left(b1), left_end(e1), right(b2) {}
        const auto& operator*() const noexcept { return on_left() ? *left : *right; }
        ConcatIterator& operator++() { if (on_left()) ++left; else ++right; return *this; }
        bool operator==(const ConcatIterator& rhs) const noexcept { return left == rhs.left && right == rhs.right; }
    private:
        left_iterator left, left_end;
        right_iterator right;
        bool on_left() const noexcept { return left != left_end; }
    };

    template <typename Range1, typename Range2>
    Irange<ConcatIterator<Range1, Range2>> operator>>(const Range1& lhs, ConcatObject<Range2> rhs) {
        using std::begin;
        using std::end;
        auto b1 = begin(lhs), e1 = end(lhs);
        auto b2 = begin(*rhs.range), e2 = end(*rhs.range);
        return {{b1, e1, b2}, {e1, e1, e2}};
    }

    template <typename Range1, typename Range2>
    Irange<ConcatIterator<Range2, Range1>> operator>>(const Range1& lhs, PrefixObject<Range2> rhs) {
        using std::begin;
        using std::end;
        auto b1 = begin(lhs), e1 = end(lhs);
        auto b2 = begin(*rhs.range), e2 = end(*rhs.range);
        return {{b2, e2, b1}, {e2, e2, e1}};
    }

    template <typename Container, typename Range2>
    Container& operator<<(Container& lhs, ConcatObject<Range2> rhs) {
        using std::begin;
        using std::end;
        lhs.insert(lhs.end(), begin(*rhs.range), end(*rhs.range));
        return lhs;
    }

    template <typename Container, typename Range2>
    Container& operator<<(Container& lhs, PrefixObject<Range2> rhs) {
        using std::begin;
        using std::end;
        lhs.insert(lhs.begin(), begin(*rhs.range), end(*rhs.range));
        return lhs;
    }

    template <typename Range2>
    inline ConcatObject<Range2> concat(Range2& r) {
        return {&r};
    }

    template <typename Range2>
    inline PrefixObject<Range2> prefix(Range2& r) {
        return {&r};
    }

    // inner_product

    template <typename Range, typename T, typename BinaryFunction1, typename BinaryFunction2>
    struct InnerProductObject:
    AlgorithmBase<InnerProductObject<Range, T, BinaryFunction1, BinaryFunction2>> {
        const Range* range;
        T init;
        BinaryFunction1 mul;
        BinaryFunction2 add;
        InnerProductObject(const Range* r, const T& t): range(r), init(t), mul(), add() {}
        InnerProductObject(const Range* r, const T& t, const BinaryFunction1& m, const BinaryFunction2& a):
            range(r), init(t), mul(m), add(a) {}
    };

    template <typename Range1, typename Range2, typename T, typename BinaryFunction1, typename BinaryFunction2>
    auto operator>>(const Range1& lhs, const InnerProductObject<Range2, T, BinaryFunction1, BinaryFunction2>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), e1 = end(lhs);
        auto j = begin(*rhs.range), e2 = end(*rhs.range);
        T result = rhs.init;
        for (; i != e1 && j != e2; ++i, ++j)
            result = rhs.add(result, rhs.mul(*i, *j));
        return result;
    }

    template <typename Range>
    inline InnerProductObject<Range, Meta::RangeValue<Range>, std::multiplies<>, std::plus<>> inner_product(const Range& r) {
        return {&r, Meta::RangeValue<Range>()};
    }

    template <typename Range, typename T, typename BinaryFunction1, typename BinaryFunction2>
    inline InnerProductObject<Range, T, BinaryFunction1, BinaryFunction2>
    inner_product(const Range& r, const T& t, BinaryFunction1 f1, BinaryFunction2 f2) {
        return {&r, t, f1, f2};
    }

    // interleave

    template <typename Range2>
    struct InterleaveObject:
    AlgorithmBase<InterleaveObject<Range2>> {
        const Range2* range;
        InterleaveObject(const Range2* r): range(r) {}
    };

    template <typename Range1, typename Range2>
    class InterleaveIterator:
    public ForwardIterator<InterleaveIterator<Range1, Range2>, const Meta::RangeValue<Range1>> {
    public:
        static_assert(std::is_same<Meta::RangeValue<Range1>, Meta::RangeValue<Range2>>::value);
        using left_iterator = Meta::RangeIterator<const Range1>;
        using right_iterator = Meta::RangeIterator<const Range2>;
        using iterator_category = Meta::MinCategory<Range1, Range2, std::forward_iterator_tag>;
        InterleaveIterator() = default;
        InterleaveIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2):
            left(b1), left_end(e1), right(b2), right_end(e2), on_left(b1 != e1 || b2 == e2) {}
        const auto& operator*() const noexcept { return on_left ? *left : *right; }
        InterleaveIterator& operator++() {
            if (on_left) {
                if (left != left_end)
                    ++left;
                on_left = right == right_end;
            } else {
                if (right != right_end)
                    ++right;
                on_left = left != left_end || right == right_end;
            }
            return *this;
        }
        bool operator==(const InterleaveIterator& rhs) const noexcept
            { return left == rhs.left && right == rhs.right && on_left == rhs.on_left; }
    private:
        left_iterator left, left_end;
        right_iterator right, right_end;
        bool on_left;
    };

    template <typename Range1, typename Range2>
    Irange<InterleaveIterator<Range1, Range2>> operator>>(const Range1& lhs, InterleaveObject<Range2> rhs) {
        using std::begin;
        using std::end;
        auto b1 = begin(lhs), e1 = end(lhs);
        auto b2 = begin(*rhs.range), e2 = end(*rhs.range);
        return {{b1, e1, b2, e2}, {e1, e1, e2, e2}};
    }

    template <typename Container, typename Range2>
    Container& operator<<(Container& lhs, InterleaveObject<Range2> rhs) {
        using std::begin;
        using std::end;
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = move(temp);
        return lhs;
    }

    template <typename Range2>
    inline InterleaveObject<Range2> interleave(Range2& r) {
        return {&r};
    }

    // merge

    template <typename OrderedRange2, typename ComparisonPredicate>
    struct MergeObject:
    AlgorithmBase<MergeObject<OrderedRange2, ComparisonPredicate>> {
        const OrderedRange2* range;
        ComparisonPredicate comp;
        MergeObject(const OrderedRange2* r, const ComparisonPredicate& c): range(r), comp(c) {}
    };

    template <typename OrderedRange1, typename OrderedRange2, typename ComparisonPredicate>
    class MergeIterator:
    public ForwardIterator<MergeIterator<OrderedRange1, OrderedRange2, ComparisonPredicate>, const Meta::RangeValue<OrderedRange1>> {
    public:
        static_assert(std::is_same<Meta::RangeValue<OrderedRange1>, Meta::RangeValue<OrderedRange2>>::value);
        using left_iterator = Meta::RangeIterator<const OrderedRange1>;
        using right_iterator = Meta::RangeIterator<const OrderedRange2>;
        using iterator_category = Meta::MinCategory<OrderedRange1, OrderedRange2, std::forward_iterator_tag>;
        using left_value = Meta::RangeValue<OrderedRange1>;
        using right_value = Meta::RangeValue<OrderedRange2>;
        using predicate_type = std::function<bool(const left_value&, const right_value&)>;
        MergeIterator() = default;
        MergeIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, ComparisonPredicate p):
            it1(b1), end1(e1), it2(b2), end2(e2), comp(p), left() { update(); }
        const auto& operator*() const noexcept { return left ? *it1 : *it2; }
        MergeIterator& operator++() {
            if (it2 == end2)
                ++it1;
            else if (it1 == end1)
                ++it2;
            else if (left)
                ++it1;
            else
                ++it2;
            update();
            return *this;
        }
        bool operator==(const MergeIterator& rhs) const noexcept { return it1 == rhs.it1 && it2 == rhs.it2; }
    private:
        left_iterator it1, end1;
        right_iterator it2, end2;
        predicate_type comp;
        bool left;
        void update() { left = it1 != end1 && (it2 == end2 || comp(*it1, *it2)); }
    };

    template <typename OrderedRange1, typename OrderedRange2, typename ComparisonPredicate>
    Irange<MergeIterator<OrderedRange1, OrderedRange2, ComparisonPredicate>>
    operator>>(const OrderedRange1& lhs, MergeObject<OrderedRange2, ComparisonPredicate> rhs) {
        using std::begin;
        using std::end;
        auto b1 = begin(lhs), e1 = end(lhs);
        auto b2 = begin(*rhs.range), e2 = end(*rhs.range);
        return {{b1, e1, b2, e2, rhs.comp}, {e1, e1, e2, e2, rhs.comp}};
    }

    template <typename Container, typename OrderedRange2, typename ComparisonPredicate>
    Container& operator<<(Container& lhs, MergeObject<OrderedRange2, ComparisonPredicate> rhs) {
        using std::begin;
        using std::end;
        Container temp;
        std::merge(begin(lhs), end(lhs), begin(*rhs.range), end(*rhs.range), append(temp), rhs.comp);
        lhs = move(temp);
        return lhs;
    }

    template <typename OrderedRange2>
    inline MergeObject<OrderedRange2, std::less<>> merge(OrderedRange2& r) {
        return {&r, {}};
    }

    template <typename OrderedRange2, typename ComparisonPredicate>
    inline MergeObject<OrderedRange2, ComparisonPredicate> merge(OrderedRange2& r, ComparisonPredicate p) {
        return {&r, p};
    }

    // outer_product, self_cross

    template <typename ForwardRange, typename BinaryFunction>
    struct OuterProductObject:
    AlgorithmBase<OuterProductObject<ForwardRange, BinaryFunction>> {
        const ForwardRange* range;
        BinaryFunction fun;
        OuterProductObject(const ForwardRange* r, const BinaryFunction& f): range(r), fun(f) {}
        BinaryFunction get_fun() const { return fun; }
    };

    template <typename ForwardRange>
    struct OuterProductObject<ForwardRange, void>:
    AlgorithmBase<OuterProductObject<ForwardRange, void>> {
        const ForwardRange* range;
        OuterProductObject(const ForwardRange* r): range(r) {}
        auto get_fun() const { return [] (const auto& a, const auto& b) { return std::make_pair(a, b); }; }
    };

    template <>
    struct OuterProductObject<void, void>:
    AlgorithmBase<OuterProductObject<void, void>> {
        template <typename BinaryFunction> OuterProductObject<void, BinaryFunction>
            operator()(const BinaryFunction& f) const { return {nullptr, f}; }
        auto get_fun() const { return [] (const auto& a, const auto& b) { return std::make_pair(a, b); }; }
    };

    template <typename ForwardRange1, typename ForwardRange2, typename BinaryFunction>
    class OuterProductIterator:
    public ForwardIterator<OuterProductIterator<ForwardRange1, ForwardRange2, BinaryFunction>, const PairResult<ForwardRange1, ForwardRange2, BinaryFunction>> {
    public:
        static_assert(! Meta::category_is_less<ForwardRange1, std::forward_iterator_tag>);
        static_assert(! Meta::category_is_less<ForwardRange2, std::forward_iterator_tag>);
        using value_type = PairResult<ForwardRange1, ForwardRange2, BinaryFunction>;
        using left_iterator = Meta::RangeIterator<const ForwardRange1>;
        using right_iterator = Meta::RangeIterator<const ForwardRange2>;
        using function_type = std::function<value_type(const Meta::RangeValue<ForwardRange1>&, const Meta::RangeValue<ForwardRange2>&)>;
        using product_object = OuterProductObject<ForwardRange2, BinaryFunction>;
        OuterProductIterator() = default;
        template <typename BinaryFunction2>
            OuterProductIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, const BinaryFunction2& f):
            left(b1), left_end(e1), right(b2), right_begin(b2), right_end(e2), fun(f) { update(); }
        const value_type& operator*() const noexcept { return value; }
        OuterProductIterator& operator++() {
            if (++right == right_end) {
                right = right_begin;
                ++left;
            }
            update();
            return *this;
        }
        bool operator==(const OuterProductIterator& rhs) const noexcept { return left == rhs.left || right == rhs.right; }
    private:
        left_iterator left, left_end;
        right_iterator right, right_begin, right_end;
        function_type fun;
        value_type value;
        void update() { if (left != left_end && right != right_end) value = fun(*left, *right); }
    };

    template <typename ForwardRange1, typename ForwardRange2, typename BinaryFunction>
    Irange<OuterProductIterator<ForwardRange1, ForwardRange2, BinaryFunction>>
    operator>>(const ForwardRange1& lhs, OuterProductObject<ForwardRange2, BinaryFunction> rhs) {
        using std::begin;
        using std::end;
        auto b1 = begin(lhs), e1 = end(lhs);
        auto b2 = begin(*rhs.range), e2 = end(*rhs.range);
        return {{b1, e1, b2, e2, rhs.get_fun()}, {e1, e1, e2, e2, rhs.get_fun()}};
    }

    template <typename ForwardRange, typename BinaryFunction>
    Irange<OuterProductIterator<ForwardRange, ForwardRange, BinaryFunction>>
    operator>>(const ForwardRange& lhs, OuterProductObject<void, BinaryFunction> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, b, e, rhs.get_fun()}, {e, e, e, e, rhs.get_fun()}};
    }

    template <typename ForwardRange2>
    inline OuterProductObject<ForwardRange2, void> outer_product(const ForwardRange2& r) {
        return {&r};
    }

    template <typename ForwardRange2, typename BinaryFunction>
    inline OuterProductObject<ForwardRange2, BinaryFunction> outer_product(const ForwardRange2& r, BinaryFunction f) {
        return {&r, f};
    }

    constexpr OuterProductObject<void, void> self_cross = {};

    // set algorithms

    struct SetDifferenceTraits {
        static constexpr bool left = true;
        static constexpr bool right = false;
        static constexpr bool both = false;
    };

    struct SetDifferenceFromTraits {
        static constexpr bool left = false;
        static constexpr bool right = true;
        static constexpr bool both = false;
    };

    struct SetIntersectionTraits {
        static constexpr bool left = false;
        static constexpr bool right = false;
        static constexpr bool both = true;
    };

    struct SetSymmetricDifferenceTraits {
        static constexpr bool left = true;
        static constexpr bool right = true;
        static constexpr bool both = false;
    };

    struct SetUnionTraits {
        static constexpr bool left = true;
        static constexpr bool right = true;
        static constexpr bool both = true;
    };

    template <typename OrderedRange2, typename ComparisonPredicate, typename SetTraits>
    struct SetOperationObject:
    AlgorithmBase<SetOperationObject<OrderedRange2, ComparisonPredicate, SetTraits>> {
        const OrderedRange2* range;
        ComparisonPredicate comp;
        SetOperationObject(const OrderedRange2* r): range(r), comp() {}
        SetOperationObject(const OrderedRange2* r, const ComparisonPredicate& c): range(r), comp(c) {}
    };

    template <typename OrderedRange1, typename OrderedRange2, typename ComparisonPredicate, typename SetTraits>
    class SetOperationIterator:
    public ForwardIterator<SetOperationIterator<OrderedRange1, OrderedRange2, ComparisonPredicate, SetTraits>, const Meta::RangeValue<OrderedRange1>> {
    public:
        static_assert(std::is_same<Meta::RangeValue<OrderedRange1>, Meta::RangeValue<OrderedRange2>>::value);
        using left_iterator = Meta::RangeIterator<const OrderedRange1>;
        using right_iterator = Meta::RangeIterator<const OrderedRange2>;
        using iterator_category = Meta::MinCategory<OrderedRange1, OrderedRange2, std::forward_iterator_tag>;
        using left_value = Meta::RangeValue<OrderedRange1>;
        using right_value = Meta::RangeValue<OrderedRange2>;
        using predicate_type = std::function<bool(const left_value&, const right_value&)>;
        SetOperationIterator() = default;
        SetOperationIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, ComparisonPredicate p):
            it1(b1), end1(e1), it2(b2), end2(e2), comp(p) { if (! accept()) ++*this; }
        const auto& operator*() const noexcept {
            if (it1 == end1)            return *it2;
            else if (it2 == end2)       return *it1;
            else if (comp(*it2, *it1))  return *it2;
            else                        return *it1;
        }
        SetOperationIterator& operator++() {
            do {
                if (it1 == end1)            ++it2;
                else if (it2 == end2)       ++it1;
                else if (comp(*it1, *it2))  ++it1;
                else if (comp(*it2, *it1))  ++it2;
                else                        { ++it1; ++it2; }
            } while (! accept());
            return *this;
        }
        bool operator==(const SetOperationIterator& rhs) const noexcept { return it1 == rhs.it1 && it2 == rhs.it2; }
    private:
        left_iterator it1, end1;
        right_iterator it2, end2;
        predicate_type comp;
        bool accept() const {
            if (it1 == end1 && it2 == end2)  return true;
            else if (it1 == end1)            return SetTraits::right;
            else if (it2 == end2)            return SetTraits::left;
            else if (comp(*it1, *it2))       return SetTraits::left;
            else if (comp(*it2, *it1))       return SetTraits::right;
            else                             return SetTraits::both;
        }
    };

    template <typename OrderedRange1, typename OrderedRange2, typename ComparisonPredicate, typename SetTraits>
    Irange<SetOperationIterator<OrderedRange1, OrderedRange2, ComparisonPredicate, SetTraits>>
    operator>>(const OrderedRange1& lhs, SetOperationObject<OrderedRange2, ComparisonPredicate, SetTraits> rhs) {
        using std::begin;
        using std::end;
        auto b1 = begin(lhs), e1 = end(lhs);
        auto b2 = begin(*rhs.range), e2 = end(*rhs.range);
        return {{b1, e1, b2, e2, rhs.comp}, {e1, e1, e2, e2, rhs.comp}};
    }

    template <typename Container, typename OrderedRange2, typename ComparisonPredicate, typename SetTraits>
    Container& operator<<(Container& lhs, SetOperationObject<OrderedRange2, ComparisonPredicate, SetTraits> rhs) {
        using std::begin;
        using std::end;
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = move(temp);
        return lhs;
    }

    template <typename OrderedRange2>
    inline SetOperationObject<OrderedRange2, std::less<>, SetDifferenceTraits> set_difference(OrderedRange2& r) {
        return &r;
    }

    template <typename OrderedRange2, typename ComparisonPredicate>
    inline SetOperationObject<OrderedRange2, ComparisonPredicate, SetDifferenceTraits> set_difference(OrderedRange2& r, ComparisonPredicate p) {
        return {&r, p};
    }

    template <typename OrderedRange2>
    inline SetOperationObject<OrderedRange2, std::less<>, SetDifferenceFromTraits> set_difference_from(OrderedRange2& r) {
        return &r;
    }

    template <typename OrderedRange2, typename ComparisonPredicate>
    inline SetOperationObject<OrderedRange2, ComparisonPredicate, SetDifferenceFromTraits> set_difference_from(OrderedRange2& r, ComparisonPredicate p) {
        return {&r, p};
    }

    template <typename OrderedRange2>
    inline SetOperationObject<OrderedRange2, std::less<>, SetIntersectionTraits> set_intersection(OrderedRange2& r) {
        return &r;
    }

    template <typename OrderedRange2, typename ComparisonPredicate>
    inline SetOperationObject<OrderedRange2, ComparisonPredicate, SetIntersectionTraits> set_intersection(OrderedRange2& r, ComparisonPredicate p) {
        return {&r, p};
    }

    template <typename OrderedRange2>
    inline SetOperationObject<OrderedRange2, std::less<>, SetSymmetricDifferenceTraits> set_symmetric_difference(OrderedRange2& r) {
        return &r;
    }

    template <typename OrderedRange2, typename ComparisonPredicate>
    inline SetOperationObject<OrderedRange2, ComparisonPredicate, SetSymmetricDifferenceTraits> set_symmetric_difference(OrderedRange2& r, ComparisonPredicate p) {
        return {&r, p};
    }

    template <typename OrderedRange2>
    inline SetOperationObject<OrderedRange2, std::less<>, SetUnionTraits> set_union(OrderedRange2& r) {
        return &r;
    }

    template <typename OrderedRange2, typename ComparisonPredicate>
    inline SetOperationObject<OrderedRange2, ComparisonPredicate, SetUnionTraits> set_union(OrderedRange2& r, ComparisonPredicate p) {
        return {&r, p};
    }

    // zip

    template <typename Range2, typename BinaryFunction>
    struct ZipObject:
    AlgorithmBase<ZipObject<Range2, BinaryFunction>> {
        const Range2* range;
        BinaryFunction fun;
        ZipObject(const Range2* r, const BinaryFunction& f): range(r), fun(f) {}
        BinaryFunction get_fun() const { return fun; }
    };

    template <typename Range2>
    struct ZipObject<Range2, void>:
    AlgorithmBase<ZipObject<Range2, void>> {
        const Range2* range;
        ZipObject(const Range2* r): range(r) {}
        auto get_fun() const { return [] (const auto& a, const auto& b) { return std::make_pair(a, b); }; }
    };

    template <typename Range1, typename Range2, typename BinaryFunction>
    class ZipIterator:
    public ForwardIterator<ZipIterator<Range1, Range2, BinaryFunction>, const PairResult<Range1, Range2, BinaryFunction>> {
    public:
        using left_iterator = Meta::RangeIterator<const Range1>;
        using right_iterator = Meta::RangeIterator<const Range2>;
        using iterator_category = Meta::MinCategory<Range1, Range2, std::forward_iterator_tag>;
        using value_type = PairResult<Range1, Range2, BinaryFunction>;
        using function_type = std::function<value_type(const Meta::RangeValue<Range1>&, const Meta::RangeValue<Range2>&)>;
        using zip_object = ZipObject<Range2, BinaryFunction>;
        ZipIterator() = default;
        ZipIterator(left_iterator b1, left_iterator e1, right_iterator b2, right_iterator e2, zip_object z):
            left(b1), left_end(e1), right(b2), right_end(e2), fun(z.get_fun()) { update(); }
        const value_type& operator*() const noexcept { return value; }
        ZipIterator& operator++() { ++left; ++right; update(); return *this; }
        bool operator==(const ZipIterator& rhs) const noexcept { return left == rhs.left || right == rhs.right; }
    private:
        left_iterator left, left_end;
        right_iterator right, right_end;
        function_type fun;
        value_type value;
        void update() { if (left != left_end && right != right_end) value = fun(*left, *right); }
    };

    template <typename Range1, typename Range2, typename BinaryFunction>
    Irange<ZipIterator<Range1, Range2, BinaryFunction>> operator>>(const Range1& lhs, ZipObject<Range2, BinaryFunction> rhs) {
        using std::begin;
        using std::end;
        auto b1 = begin(lhs), e1 = end(lhs);
        auto b2 = begin(*rhs.range), e2 = end(*rhs.range);
        return {{b1, e1, b2, e2, rhs}, {e1, e1, e2, e2, rhs}};
    }

    template <typename Range2>
    inline ZipObject<Range2, void> zip(const Range2& r) {
        return {&r};
    }

    template <typename Range2, typename BinaryFunction>
    inline ZipObject<Range2, BinaryFunction> zip(const Range2& r, BinaryFunction f) {
        return {&r, f};
    }

}
