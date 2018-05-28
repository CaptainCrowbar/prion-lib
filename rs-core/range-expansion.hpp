#pragma once

#include "rs-core/range-core.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <limits>
#include <memory>
#include <type_traits>

namespace RS::Range {

    // combinations

    struct CombinationsObject:
    AlgorithmBase<CombinationsObject> {
        size_t num;
        CombinationsObject(size_t k): num(k) {}
    };

    template <typename ForwardRange>
    class CombinationsIterator:
    public InputIterator<CombinationsIterator<ForwardRange>, const SharedRange<Meta::RangeValue<ForwardRange>>> {
    public:
        using range_iterator = Meta::RangeIterator<const ForwardRange>;
        using value_type = Meta::RangeValue<ForwardRange>;
        CombinationsIterator() = default;
        CombinationsIterator(range_iterator i, range_iterator j, size_t k):
            its(make_shared_range<range_iterator>(i == j ? 0 : k, i)),
            val(make_shared_range<value_type>(its.size(), *i)),
            src{i, j},
            count(i == j ? npos : 0) {}
        const auto& operator*() const noexcept { return val; }
        CombinationsIterator& operator++() {
            if (count == npos)
                return *this;
            auto& ivec = *its.first.share;
            auto& vvec = *val.first.share;
            size_t i = ivec.size();
            for (; i > 0; --i) {
                auto& j = ivec[i - 1];
                auto& v = vvec[i - 1];
                ++j;
                if (j != src.second) {
                    v = *j;
                    break;
                }
                j = src.first;
                v = *j;
            }
            if (i == 0)
                count = npos;
            else
                ++count;
            return *this;
        }
        bool operator==(const CombinationsIterator& rhs) const noexcept { return count == rhs.count; }
    private:
        SharedRange<range_iterator> its;
        SharedRange<value_type> val;
        Irange<range_iterator> src;
        size_t count = npos;
    };

    template <typename ForwardRange>
    Irange<CombinationsIterator<ForwardRange>>
    operator>>(const ForwardRange& lhs, CombinationsObject rhs) {
        using std::begin;
        using std::end;
        return {{begin(lhs), end(lhs), rhs.num}, {}};
    }

    inline CombinationsObject combinations(size_t k) { return {k}; }

    // flat_map

    template <typename UnaryFunction>
    struct FlatMapObject:
    AlgorithmBase<FlatMapObject<UnaryFunction>> {
        UnaryFunction fun;
        FlatMapObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    class FlatMapIterator:
    public ForwardIterator<FlatMapIterator<Range, UnaryFunction>, const Meta::RangeValue<InvokeResult<UnaryFunction, Meta::RangeValue<Range>>>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using level_1_iterator = Meta::RangeIterator<const Range>;
        using level_2_range = InvokeResult<UnaryFunction, Meta::RangeValue<Range>>;
        using level_2_iterator = Meta::RangeIterator<level_2_range>;
        using function_type = std::function<level_2_range(const Meta::RangeValue<Range>&)>;
        FlatMapIterator() = default;
        FlatMapIterator(level_1_iterator b, level_1_iterator e, UnaryFunction f):
            iter1(b), end1(e), range2(), iter2(), fun(f) { update(); }
        const auto& operator*() const noexcept { return *iter2; }
        FlatMapIterator& operator++() {
            using std::end;
            ++iter2;
            if (iter2 == end(*range2)) {
                ++iter1;
                range2.reset();
                update();
            }
            return *this;
        }
        bool operator==(const FlatMapIterator& rhs) const noexcept {
            if (iter1 != rhs.iter1)
                return false;
            else if (range2 && rhs.range2)
                return iter2 == rhs.iter2;
            else
                return ! range2 && ! rhs.range2;
        }
    private:
        level_1_iterator iter1, end1;
        std::shared_ptr<level_2_range> range2;
        level_2_iterator iter2;
        function_type fun;
        void update() {
            using std::begin;
            if (! range2 && iter1 != end1) {
                range2 = std::make_shared<level_2_range>(fun(*iter1));
                iter2 = begin(*range2);
            }
        }
    };

    template <typename Range, typename UnaryFunction>
    Irange<FlatMapIterator<Range, UnaryFunction>> operator>>(const Range& lhs, FlatMapObject<UnaryFunction> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.fun}, {e, e, rhs.fun}};
    }

    template <typename Container, typename UnaryFunction>
    Container& operator<<(Container& lhs, FlatMapObject<UnaryFunction> rhs) {
        using std::begin;
        using std::end;
        Container temp;
        for (auto& x: lhs) {
            auto range = rhs.fun(x);
            std::copy(begin(range), end(range), append(temp));
        }
        lhs = move(temp);
        return lhs;
    }

    template <typename UnaryFunction>
    inline FlatMapObject<UnaryFunction> flat_map(UnaryFunction f) {
        return f;
    }

    // flatten

    struct FlattenObject:
    AlgorithmBase<FlattenObject> {};

    template <typename NestedRange>
    class FlattenIterator:
    public ForwardIterator<FlattenIterator<NestedRange>, const Meta::RangeValue<const Meta::RangeValue<NestedRange>>> {
    public:
        using iterator_category = Meta::MinCategory<NestedRange, std::forward_iterator_tag>;
        using level_1_iterator = Meta::RangeIterator<const NestedRange>;
        using level_2_range = Meta::RangeValue<NestedRange>;
        using level_2_iterator = Meta::RangeIterator<const level_2_range>;
        FlattenIterator() = default;
        FlattenIterator(level_1_iterator b, level_1_iterator e):
            iter1(b), end1(e), iter2(), end2() { init2(); }
        const auto& operator*() const noexcept { return *iter2; }
        FlattenIterator& operator++() {
            ++iter2;
            if (iter2 == end2) {
                ++iter1;
                init2();
            }
            return *this;
        }
        bool operator==(const FlattenIterator& rhs) const noexcept {
            if (iter1 != rhs.iter1)
                return false;
            else if (iter1 == end1 || rhs.iter1 == rhs.end1)
                return iter1 == end1 && rhs.iter1 == rhs.end1;
            else
                return iter2 == rhs.iter2;
        }
    private:
        level_1_iterator iter1, end1;
        level_2_iterator iter2, end2;
        void init2() {
            using std::begin;
            using std::end;
            if (iter1 == end1) {
                iter2 = end2 = level_2_iterator();
            } else {
                iter2 = begin(*iter1);
                end2 = end(*iter1);
            }
        }
    };

    template <typename NestedRange>
    Irange<FlattenIterator<NestedRange>> operator>>(const NestedRange& lhs, FlattenObject /*rhs*/) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e}, {e, e}};
    }

    constexpr FlattenObject flatten = {};

    // insert_before, insert_after, insert_between, insert_around

    template <typename T>
    struct InsertBeforeObject:
    AlgorithmBase<InsertBeforeObject<T>> {
        T value;
        InsertBeforeObject(const T& t): value(t) {}
    };

    template <typename T>
    struct InsertAfterObject:
    AlgorithmBase<InsertAfterObject<T>> {
        T value;
        InsertAfterObject(const T& t): value(t) {}
    };

    template <typename T>
    struct InsertBetweenObject:
    AlgorithmBase<InsertBetweenObject<T>> {
        T value;
        InsertBetweenObject(const T& t): value(t) {}
    };

    template <typename T>
    struct InsertAroundObject:
    AlgorithmBase<InsertAroundObject<T>> {
        T before, after;
        InsertAroundObject(const T& t1, const T& t2): before(t1), after(t2) {}
    };

    template <typename Range, typename T>
    class InsertBeforeIterator:
    public ForwardIterator<InsertBeforeIterator<Range, T>, const T> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using range_iterator = Meta::RangeIterator<const Range>;
        InsertBeforeIterator() = default;
        InsertBeforeIterator(range_iterator b, range_iterator e, const T& t): iter(b), end(e), value(t), step(0) {}
        const auto& operator*() const noexcept { return step == 0 ? value : *iter; }
        InsertBeforeIterator& operator++() {
            if (step == 0) {
                step = 1;
            } else {
                ++iter;
                step = 0;
            }
            return *this;
        }
        bool operator==(const InsertBeforeIterator& rhs) const noexcept { return iter == rhs.iter && step == rhs.step; }
    private:
        range_iterator iter, end;
        T value;
        uint8_t step = 0;
    };

    template <typename Range, typename T>
    class InsertAfterIterator:
    public ForwardIterator<InsertAfterIterator<Range, T>, const T> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using range_iterator = Meta::RangeIterator<const Range>;
        InsertAfterIterator() = default;
        InsertAfterIterator(range_iterator b, range_iterator e, const T& t): iter(b), end(e), value(t), step(0) {}
        const auto& operator*() const noexcept { return step == 0 ? *iter : value; }
        InsertAfterIterator& operator++() {
            if (step == 0) {
                step = 1;
            } else {
                ++iter;
                step = 0;
            }
            return *this;
        }
        bool operator==(const InsertAfterIterator& rhs) const noexcept { return iter == rhs.iter && step == rhs.step; }
    private:
        range_iterator iter, end;
        T value;
        uint8_t step = 0;
    };

    template <typename Range, typename T>
    class InsertBetweenIterator:
    public ForwardIterator<InsertBetweenIterator<Range, T>, const T> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using range_iterator = Meta::RangeIterator<const Range>;
        InsertBetweenIterator() = default;
        InsertBetweenIterator(range_iterator b, range_iterator e, const T& t): iter(b), end(e), value(t), step(b != e) {}
        const auto& operator*() const noexcept { return step == 0 ? value : *iter; }
        InsertBetweenIterator& operator++() {
            if (step == 0) {
                step = 1;
            } else {
                ++iter;
                step = 0;
            }
            return *this;
        }
        bool operator==(const InsertBetweenIterator& rhs) const noexcept { return iter == rhs.iter && step == rhs.step; }
    private:
        range_iterator iter, end;
        T value;
        uint8_t step = 0;
    };

    template <typename Range, typename T>
    class InsertAroundIterator:
    public ForwardIterator<InsertAroundIterator<Range, T>, const T> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using range_iterator = Meta::RangeIterator<const Range>;
        InsertAroundIterator() = default;
        InsertAroundIterator(range_iterator b, range_iterator e, const T& t1, const T& t2):
            iter(b), end(e), before(t1), after(t2), step(0) {}
        const auto& operator*() const noexcept {
            switch (step) {
                case 0:   return before;
                case 1:   return *iter;
                default:  return after;
            }
        }
        InsertAroundIterator& operator++() {
            ++step;
            if (step == 3) {
                step = 0;
                ++iter;
            }
            return *this;
        }
        bool operator==(const InsertAroundIterator& rhs) const noexcept { return iter == rhs.iter && step == rhs.step; }
    private:
        range_iterator iter, end;
        T before, after;
        uint8_t step = 0;
    };

    template <typename Range, typename T>
    Irange<InsertBeforeIterator<Range, T>> operator>>(const Range& lhs, InsertBeforeObject<T> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.value}, {e, e, rhs.value}};
    }

    template <typename Range, typename T>
    Irange<InsertAfterIterator<Range, T>> operator>>(const Range& lhs, InsertAfterObject<T> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.value}, {e, e, rhs.value}};
    }

    template <typename Range, typename T>
    Irange<InsertBetweenIterator<Range, T>> operator>>(const Range& lhs, InsertBetweenObject<T> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.value}, {e, e, rhs.value}};
    }

    template <typename Range, typename T>
    Irange<InsertAroundIterator<Range, T>> operator>>(const Range& lhs, InsertAroundObject<T> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.before, rhs.after}, {e, e, rhs.before, rhs.after}};
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertBeforeObject<T>& rhs) {
        using std::begin;
        using std::end;
        Container temp;
        for (auto& x: lhs) {
            append_to(temp, rhs.value);
            append_to(temp, x);
        }
        lhs = move(temp);
        return lhs;
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertAfterObject<T>& rhs) {
        using std::begin;
        using std::end;
        Container temp;
        for (auto& x: lhs) {
            append_to(temp, x);
            append_to(temp, rhs.value);
        }
        lhs = move(temp);
        return lhs;
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertBetweenObject<T>& rhs) {
        using std::begin;
        using std::end;
        Container temp;
        auto i = begin(lhs), e = end(lhs);
        if (i != e)
            append_to(temp, *i++);
        for (; i != e; ++i) {
            append_to(temp, rhs.value);
            append_to(temp, *i);
        }
        lhs = move(temp);
        return lhs;
    }

    template <typename Container, typename T>
    Container& operator<<(Container& lhs, const InsertAroundObject<T>& rhs) {
        using std::begin;
        using std::end;
        Container temp;
        for (auto& x: lhs) {
            append_to(temp, rhs.before);
            append_to(temp, x);
            append_to(temp, rhs.after);
        }
        lhs = move(temp);
        return lhs;
    }

    template <typename T>
    inline InsertBeforeObject<T> insert_before(const T& t) {
        return t;
    }

    template <typename T>
    inline InsertAfterObject<T> insert_after(const T& t) {
        return t;
    }

    template <typename T>
    inline InsertBetweenObject<T> insert_between(const T& t) {
        return t;
    }

    template <typename T>
    inline InsertAroundObject<T> insert_around(const T& t1, const T& t2) {
        return {t1, t2};
    }

    // permutations

    template <typename ComparisonPredicate>
    struct PermutationsObject:
    AlgorithmBase<PermutationsObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        PermutationsObject() = default;
        PermutationsObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> PermutationsObject<CP2> operator()(CP2 p) const { return p; }
    };

    template <typename Range, typename ComparisonPredicate>
    class PermutationsIterator:
    public InputIterator<PermutationsIterator<Range, ComparisonPredicate>, const SharedRange<Meta::RangeValue<Range>>> {
    public:
        using range_iterator = Meta::RangeIterator<const Range>;
        PermutationsIterator() = default;
        PermutationsIterator(range_iterator i, range_iterator j, ComparisonPredicate p):
            val(make_shared_range<Meta::RangeValue<Range>>(i, j)), comp(p), count(i == j ? npos : 0) {}
        const auto& operator*() const noexcept { return val; }
        PermutationsIterator& operator++() {
            if (std::next_permutation(val.begin(), val.end(), comp))
                ++count;
            else
                count = npos;
            return *this;
        }
        bool operator==(const PermutationsIterator& rhs) const noexcept { return count == rhs.count; }
    private:
        SharedRange<Meta::RangeValue<Range>> val;
        ComparisonPredicate comp;
        size_t count = npos;
    };

    template <typename Range, typename ComparisonPredicate>
    Irange<PermutationsIterator<Range, ComparisonPredicate>>
    operator>>(const Range& lhs, PermutationsObject<ComparisonPredicate> rhs) {
        using std::begin;
        using std::end;
        return {{begin(lhs), end(lhs), rhs.comp}, {}};
    }

    constexpr PermutationsObject<std::less<>> permutations = {};

    // repeat

    struct RepeatObject:
    AlgorithmBase<RepeatObject> {
        ptrdiff_t num = std::numeric_limits<ptrdiff_t>::max();
        RepeatObject operator()(size_t k) const noexcept {
            RepeatObject o;
            if (k < size_t(o.num))
                o.num = k;
            return o;
        }
    };

    template <typename FR>
    class RepeatIterator:
    public FlexibleRandomAccessIterator<RepeatIterator<FR>, const Meta::RangeValue<FR>> {
    public:
        static_assert(! Meta::category_is_less<FR, std::forward_iterator_tag>);
        using underlying_iterator = Meta::RangeIterator<const FR>;
        using iterator_category = Meta::MinCategory<FR, std::bidirectional_iterator_tag>;
        RepeatIterator() = default;
        RepeatIterator(underlying_iterator b, underlying_iterator e, ptrdiff_t c): begin(b), end(e), iter(b), cycle(c) {}
        const auto& operator*() const noexcept { return *iter; }
        RepeatIterator& operator++() {
            ++iter;
            if (iter == end) {
                iter = begin;
                ++cycle;
            }
            return *this;
        }
        RepeatIterator& operator--() {
            if (iter == begin) {
                iter = end;
                --cycle;
            }
            --iter;
            return *this;
        }
        RepeatIterator& operator+=(ptrdiff_t rhs) {
            ptrdiff_t offset = std::distance(begin, iter) + rhs;
            auto qr = divide(offset, std::distance(begin, end));
            cycle += qr.first;
            iter = begin;
            std::advance(iter, qr.second);
            return *this;
        }
        ptrdiff_t operator-(const RepeatIterator& rhs) const {
            return (cycle - rhs.cycle) * std::distance(begin, end) + std::distance(rhs.iter, iter);
        }
        bool operator==(const RepeatIterator& rhs) const noexcept { return cycle == rhs.cycle && iter == rhs.iter; }
    private:
        underlying_iterator begin, end, iter;
        ptrdiff_t cycle = 0;
    };

    template <typename FR>
    Irange<RepeatIterator<FR>> operator>>(const FR& lhs, RepeatObject rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, 0}, {b, e, rhs.num}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, RepeatObject rhs) {
        using std::begin;
        using std::end;
        if (rhs.num == 0) {
            lhs.clear();
        } else if (rhs.num > 1) {
            Container temp = lhs;
            for (ptrdiff_t i = 1; i < rhs.num; ++i)
                std::copy(begin(lhs), end(lhs), append(temp));
            lhs = move(temp);
        }
        return lhs;
    }

    constexpr RepeatObject repeat = {};

    // subsets

    struct SubsetsObject:
    AlgorithmBase<SubsetsObject> {
        size_t num;
        SubsetsObject(size_t k): num(k) {}
    };

    template <typename ForwardRange>
    class SubsetsIterator:
    public InputIterator<SubsetsIterator<ForwardRange>, const SharedRange<Meta::RangeValue<ForwardRange>>> {
    public:
        using range_iterator = Meta::RangeIterator<const ForwardRange>;
        using value_type = Meta::RangeValue<ForwardRange>;
        SubsetsIterator() = default;
        SubsetsIterator(range_iterator i, range_iterator j, size_t k):
            its(make_shared_range<range_iterator>(std::min(k, size_t(std::distance(i, j))))),
            val(make_shared_range<value_type>(its.size())),
            src{i, j},
            count(0) {
                auto& ivec = *its.first.share;
                auto& vvec = *val.first.share;
                for (size_t m = 0, n = ivec.size(); m < n; ++i, ++m) {
                    ivec[m] = i;
                    vvec[m] = *i;
                }
            }
        const auto& operator*() const noexcept { return val; }
        SubsetsIterator& operator++() {
            if (count == npos)
                return *this;
            auto& ivec = *its.first.share;
            auto& vvec = *val.first.share;
            size_t i = ivec.size(), j = 0;
            for (; i > 0; --i) {
                auto k = ivec[i - 1];
                for (j = i - 1; j < ivec.size(); ++j) {
                    ivec[j] = ++k;
                    if (k == src.second)
                        break;
                    vvec[j] = *k;
                }
                if (j == ivec.size())
                    break;
            }
            if (i == 0)
                count = npos;
            else
                ++count;
            return *this;
        }
        bool operator==(const SubsetsIterator& rhs) const noexcept { return count == rhs.count; }
    private:
        SharedRange<range_iterator> its;
        SharedRange<value_type> val;
        Irange<range_iterator> src;
        size_t count = npos;
    };

    template <typename ForwardRange>
    Irange<SubsetsIterator<ForwardRange>>
    operator>>(const ForwardRange& lhs, SubsetsObject rhs) {
        using std::begin;
        using std::end;
        return {{begin(lhs), end(lhs), rhs.num}, {}};
    }

    inline SubsetsObject subsets(size_t k) { return {k}; }

}
