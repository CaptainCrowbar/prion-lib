#pragma once

#include "rs-core/range-core.hpp"
#include <algorithm>
#include <functional>
#include <iterator>

namespace RS::Range {

    // next_permutation, prev_permutation

    template <typename ComparisonPredicate, bool Next>
    struct PermutationObject:
    AlgorithmBase<PermutationObject<ComparisonPredicate, Next>> {
        ComparisonPredicate comp;
        PermutationObject() = default;
        PermutationObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> PermutationObject<CP2, Next> operator()(CP2 p) const { return p; }
        template <typename BidirectionalRange> void permute(BidirectionalRange& r) const {
            using std::begin;
            using std::end;
            if (Next)
                std::next_permutation(begin(r), end(r), comp);
            else
                std::prev_permutation(begin(r), end(r), comp);
        }
    };

    template <typename Range, typename ComparisonPredicate, bool Next>
    auto operator>>(const Range& lhs, PermutationObject<ComparisonPredicate, Next> rhs) {
        using std::begin;
        using std::end;
        auto r = lhs >> collect;
        rhs.permute(r);
        return r;
    }

    template <typename BidirectionalRange, typename ComparisonPredicate, bool Next>
    BidirectionalRange& operator<<(BidirectionalRange& lhs, PermutationObject<ComparisonPredicate, Next> rhs) {
        rhs.permute(lhs);
        return lhs;
    }

    constexpr PermutationObject<std::less<>, true> next_permutation = {};
    constexpr PermutationObject<std::less<>, false> prev_permutation = {};

    // reverse

    struct ReverseObject:
    AlgorithmBase<ReverseObject> {};

    template <typename BidirectionalRange>
    Irange<std::reverse_iterator<Meta::RangeIterator<const BidirectionalRange>>>
    operator>>(const BidirectionalRange& lhs, ReverseObject /*rhs*/) {
        static_assert(! Meta::category_is_less<const BidirectionalRange, std::bidirectional_iterator_tag>);
        using std::begin;
        using std::end;
        using reverse_iterator = std::reverse_iterator<Meta::RangeIterator<const BidirectionalRange>>;
        reverse_iterator rb(end(lhs)), re(begin(lhs));
        return {rb, re};
    }

    template <typename BidirectionalRange>
    BidirectionalRange& operator<<(BidirectionalRange& lhs, ReverseObject /*rhs*/) {
        static_assert(! Meta::category_is_less<BidirectionalRange, std::bidirectional_iterator_tag>);
        using std::begin;
        using std::end;
        std::reverse(begin(lhs), end(lhs));
        return lhs;
    }

    constexpr ReverseObject reverse = {};

    // shuffle

    template <typename RandomEngine>
    struct ShuffleObject:
    AlgorithmBase<ShuffleObject<RandomEngine>> {
        RandomEngine* rng;
        ShuffleObject(RandomEngine* r): rng(r) {}
    };

    template <typename Range, typename RandomEngine>
    auto operator>>(const Range& lhs, ShuffleObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        auto result = lhs >> collect;
        std::shuffle(result.begin(), result.end(), *rhs.rng);
        return result;
    }

    template <typename RandomAccessRange, typename RandomEngine>
    RandomAccessRange& operator<<(RandomAccessRange& lhs, ShuffleObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        std::shuffle(lhs.begin(), lhs.end(), *rhs.rng);
        return lhs;
    }

    template <typename RandomEngine>
    inline ShuffleObject<RandomEngine> shuffle(RandomEngine& rng) {
        return &rng;
    }

    // sort, stable_sort

    template <typename ComparisonPredicate, bool Stable>
    struct SortObject:
    AlgorithmBase<SortObject<ComparisonPredicate, Stable>> {
        ComparisonPredicate comp;
        SortObject() = default;
        SortObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> SortObject<CP2, Stable> operator()(CP2 p) const { return p; }
        template <typename RandomAccessRange> void sort(RandomAccessRange& r) const {
            using std::begin;
            using std::end;
            if (Stable)
                std::stable_sort(begin(r), end(r), comp);
            else
                std::sort(begin(r), end(r), comp);
        }
    };

    template <typename Range, typename ComparisonPredicate, bool Stable>
    auto operator>>(const Range& lhs, SortObject<ComparisonPredicate, Stable> rhs) {
        using std::begin;
        using std::end;
        auto r = lhs >> collect;
        rhs.sort(r);
        return r;
    }

    template <typename RandomAccessRange, typename ComparisonPredicate, bool Stable>
    RandomAccessRange& operator<<(RandomAccessRange& lhs, SortObject<ComparisonPredicate, Stable> rhs) {
        rhs.sort(lhs);
        return lhs;
    }

    constexpr SortObject<std::less<>, false> sort = {};
    constexpr SortObject<std::less<>, true> stable_sort = {};

}
