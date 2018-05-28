#pragma once

#include "rs-core/range-core.hpp"
#include "rs-core/range-permutation.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // all_of, any_of, none_of

    template <typename Predicate>
    struct AllOfObject:
    AlgorithmBase<AllOfObject<Predicate>> {
        Predicate pred;
        AllOfObject(const Predicate& p): pred(p) {}
    };

    template <typename Predicate>
    struct AnyOfObject:
    AlgorithmBase<AnyOfObject<Predicate>> {
        Predicate pred;
        AnyOfObject(const Predicate& p): pred(p) {}
    };

    template <typename Predicate>
    struct NoneOfObject:
    AlgorithmBase<NoneOfObject<Predicate>> {
        Predicate pred;
        NoneOfObject(const Predicate& p): pred(p) {}
    };

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const AllOfObject<Predicate>& rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return std::find_if_not(b, e, rhs.pred) == e;
    }

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const AnyOfObject<Predicate>& rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return std::find_if(b, e, rhs.pred) != e;
    }

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const NoneOfObject<Predicate>& rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return std::find_if(b, e, rhs.pred) == e;
    }

    template <typename Predicate>
    inline AllOfObject<Predicate> all_of(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline AnyOfObject<Predicate> any_of(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline NoneOfObject<Predicate> none_of(Predicate p) {
        return p;
    }

    // count, count_if

    template <typename T>
    struct CountObject:
    AlgorithmBase<CountObject<T>> {
        T value;
        CountObject(const T& t): value(t) {}
    };

    template <>
    struct CountObject<void>:
    AlgorithmBase<CountObject<void>> {
        template <typename T> CountObject<T> operator()(const T& t) const { return t; }
    };

    template <typename Predicate>
    struct CountIfObject:
    AlgorithmBase<CountIfObject<Predicate>> {
        Predicate pred;
        CountIfObject(const Predicate& p): pred(p) {}
    };

    template <typename Range>
    size_t operator>>(const Range& lhs, CountObject<void> /*rhs*/) {
        using std::begin;
        using std::end;
        return std::distance(begin(lhs), end(lhs));
    }

    template <typename Range, typename T>
    size_t operator>>(const Range& lhs, const CountObject<T>& rhs) {
        using std::begin;
        using std::end;
        return std::count(begin(lhs), end(lhs), rhs.value);
    }

    template <typename Range, typename Predicate>
    size_t operator>>(const Range& lhs, const CountIfObject<Predicate>& rhs) {
        using std::begin;
        using std::end;
        return std::count_if(begin(lhs), end(lhs), rhs.pred);
    }

    constexpr CountObject<void> count = {};

    template <typename Predicate>
    inline CountIfObject<Predicate> count_if(Predicate p) {
        return p;
    }

    // fold_left, fold_right

    template <typename T, typename BinaryFunction>
    struct FoldLeftObject:
    AlgorithmBase<FoldLeftObject<T, BinaryFunction>> {
        T init;
        BinaryFunction fun;
        FoldLeftObject(const T& t, const BinaryFunction& f): init(t), fun(f) {}
    };

    template <typename T, typename BinaryFunction>
    struct FoldRightObject:
    AlgorithmBase<FoldRightObject<T, BinaryFunction>> {
        T init;
        BinaryFunction fun;
        FoldRightObject(const T& t, const BinaryFunction& f): init(t), fun(f) {}
    };

    template <typename Range, typename T, typename BinaryFunction>
    T operator>>(const Range& lhs, const FoldLeftObject<T, BinaryFunction>& rhs) {
        using std::begin;
        using std::end;
        return std::accumulate(begin(lhs), end(lhs), rhs.init, rhs.fun);
    }

    template <typename BidirectionalRange, typename T, typename BinaryFunction>
    T operator>>(const BidirectionalRange& lhs, const FoldRightObject<T, BinaryFunction>& rhs) {
        static_assert(! Meta::category_is_less<BidirectionalRange, std::bidirectional_iterator_tag>);
        using std::begin;
        using std::end;
        auto rev = lhs >> reverse;
        auto f = rhs.fun;
        auto fyx = [f] (const auto& x, const auto& y) { return f(y, x); };
        return std::accumulate(begin(rev), end(rev), rhs.init, fyx);
    }

    template <typename T, typename BinaryFunction>
    inline FoldLeftObject<T, BinaryFunction> fold_left(const T& t, BinaryFunction f) {
        return {t, f};
    }

    template <typename T, typename BinaryFunction>
    inline FoldRightObject<T, BinaryFunction> fold_right(const T& t, BinaryFunction f) {
        return {t, f};
    }

    // is_empty, is_nonempty

    template <bool Sense>
    struct IsEmptyObject:
    AlgorithmBase<IsEmptyObject<Sense>> {};

    template <typename Range, bool Sense>
    bool operator>>(const Range& lhs, IsEmptyObject<Sense>) {
        using std::begin;
        using std::end;
        return (begin(lhs) == end(lhs)) == Sense;
    }

    constexpr IsEmptyObject<true> is_empty = {};
    constexpr IsEmptyObject<false> is_nonempty = {};

    // is_sorted

    template <typename ComparisonPredicate>
    struct IsSortedObject:
    AlgorithmBase<IsSortedObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        IsSortedObject() = default;
        IsSortedObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> IsSortedObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename Range, typename ComparisonPredicate>
    bool operator>>(const Range& lhs, const IsSortedObject<ComparisonPredicate>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), j = i, k = end(lhs);
        if (i == k || ++j == k)
            return true;
        for (; j != k; ++i, ++j)
            if (rhs.comp(*j, *i))
                return false;
        return true;
    }

    constexpr IsSortedObject<std::less<>> is_sorted = {};

    // min, max, min_max

    template <typename ComparisonPredicate>
    struct MinObject:
    AlgorithmBase<MinObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        MinObject() = default;
        MinObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> MinObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename ComparisonPredicate>
    struct MaxObject:
    AlgorithmBase<MaxObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        MaxObject() = default;
        MaxObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> MaxObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename ComparisonPredicate>
    struct MinMaxObject:
    AlgorithmBase<MinMaxObject<ComparisonPredicate>> {
        ComparisonPredicate comp;
        MinMaxObject() = default;
        MinMaxObject(const ComparisonPredicate& p): comp(p) {}
        template <typename CP2> MinMaxObject<CP2> operator()(CP2 p) const { return {p}; }
    };

    template <typename Range, typename ComparisonPredicate>
    Meta::RangeValue<Range> operator>>(const Range& lhs, const MinObject<ComparisonPredicate>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), j = end(lhs);
        if (i == j)
            return {};
        else
            return *std::min_element(i, j, rhs.comp);
    }

    template <typename Range, typename ComparisonPredicate>
    Meta::RangeValue<Range> operator>>(const Range& lhs, const MaxObject<ComparisonPredicate>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), j = end(lhs);
        if (i == j)
            return {};
        else
            return *std::max_element(i, j, rhs.comp);
    }

    template <typename Range, typename ComparisonPredicate>
    std::pair<Meta::RangeValue<Range>, Meta::RangeValue<Range>> operator>>(const Range& lhs, const MinMaxObject<ComparisonPredicate>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), j = end(lhs);
        if (i == j)
            return {};
        auto ipair = std::minmax_element(i, j, rhs.comp);
        return {*ipair.first, *ipair.second};
    }

    constexpr MinObject<std::less<>> min = {};
    constexpr MaxObject<std::less<>> max = {};
    constexpr MinMaxObject<std::less<>> min_max = {};

    // reduce

    template <typename BinaryFunction>
    struct ReduceObject:
    AlgorithmBase<ReduceObject<BinaryFunction>> {
        BinaryFunction fun;
        ReduceObject(const BinaryFunction& f): fun(f) {}
    };

    template <typename Range, typename BinaryFunction>
    Meta::RangeValue<Range> operator>>(const Range& lhs, const ReduceObject<BinaryFunction>& rhs) {
        using std::begin;
        using std::end;
        auto i = begin(lhs), j = end(lhs);
        if (i == j)
            return {};
        auto t = *i++;
        return std::accumulate(i, j, t, rhs.fun);
    }

    template <typename BinaryFunction>
    inline ReduceObject<BinaryFunction> reduce(BinaryFunction f) {
        return {f};
    }

    // statistics, pair_statistics

    template <typename T>
    class Statistics:
    EqualityComparable<Statistics<T>> {
    public:
        using value_type = T;
        void add(T x) noexcept {
            ++count;
            sumx += x;
            sumx2 += x * x;
            if (count == 1)
                minx = maxx = x;
            else if (x < minx)
                minx = x;
            else if (x > maxx)
                maxx = x;
        }
        size_t n() const noexcept { return count; }
        T sum() const noexcept { return sumx; }
        T sum_squares() const noexcept { return sumx2; }
        T min() const noexcept { return minx; }
        T max() const noexcept { return maxx; }
        T mean() const noexcept { return sumx / T(count); }
        T stdev() const { return compute_sd(count, sumx, sumx2); }
        T pop_stdev() const { return compute_psd(count, sumx, sumx2); }
        bool operator==(const Statistics<T>& rhs) const noexcept {
            return count == rhs.count && sumx == rhs.sumx && sumx2 == rhs.sumx2
                && minx == rhs.minx && maxx == rhs.maxx;
        }
    private:
        friend class Statistics<std::pair<T, T>>;
        size_t count = 0;
        T sumx = T(0);
        T sumx2 = T(0);
        T minx = T(0);
        T maxx = T(0);
        static T compute_sd(size_t n, T sx, T sx2) {
            using std::sqrt;
            if (n < 1)
                return T(0);
            T tn = T(n), m = sx / tn;
            return sqrt(sx2 / tn - m * m);
        }
        static T compute_psd(size_t n, T sx, T sx2) {
            using std::sqrt;
            if (n < 2)
                return T(0);
            T tn = T(n), m = sx / tn;
            return sqrt((sx2 - tn * m * m) / (tn - T(1)));
        }
    };

    template <typename T>
    class Statistics<std::pair<T, T>>:
    EqualityComparable<Statistics<std::pair<T, T>>> {
    public:
        using value_type = T;
        using pair_type = std::pair<T, T>;
        void add(T x, T y) noexcept {
            ++count;
            sumx += x;
            sumx2 += x * x;
            sumy += y;
            sumy2 += y * y;
            sumxy += x * y;
            if (count == 1) {
                minx = maxx = x;
                miny = maxy = y;
            } else {
                if (x < minx)
                    minx = x;
                else if (x > maxx)
                    maxx = x;
                if (y < miny)
                    miny = y;
                else if (y > maxy)
                    maxy = y;
            }
        }
        void add(const pair_type& xy) noexcept {
            add(xy.first, xy.second);
        }
        size_t n() const noexcept { return count; }
        T sum_x() const noexcept { return sumx; }
        T sum_x2() const noexcept { return sumx2; }
        T sum_y() const noexcept { return sumy; }
        T sum_y2() const noexcept { return sumy2; }
        T sum_xy() const noexcept { return sumxy; }
        T min_x() const noexcept { return minx; }
        T min_y() const noexcept { return miny; }
        T max_x() const noexcept { return maxx; }
        T max_y() const noexcept { return maxy; }
        T mean_x() const noexcept { return sumx / T(count); }
        T mean_y() const noexcept { return sumy / T(count); }
        T stdev_x() const { return single::compute_sd(count, sumx, sumx2); }
        T stdev_y() const { return single::compute_sd(count, sumy, sumy2); }
        T pop_stdev_x() const { return single::compute_psd(count, sumx, sumx2); }
        T pop_stdev_y() const { return single::compute_psd(count, sumy, sumy2); }
        T r() const { return compute_r(count, sumx, sumx2, sumy, sumy2, sumxy); }
        std::pair<T, T> linear_xy() const { return compute_linear(count, sumx, sumx2, sumy, sumxy); }
        std::pair<T, T> linear_yx() const { return compute_linear(count, sumy, sumy2, sumx, sumxy); }
        bool operator==(const Statistics<std::pair<T, T>>& rhs) const noexcept {
            return count == rhs.count && sumx == rhs.sumx && sumx2 == rhs.sumx2
                && sumy == rhs.sumy && sumy2 == rhs.sumy2 && sumxy == rhs.sumxy
                && minx == rhs.minx && maxx == rhs.maxx && miny == rhs.miny && maxy == rhs.maxy;
        }
    private:
        using single = Statistics<T>;
        size_t count = 0;
        T sumx = T(0);
        T sumx2 = T(0);
        T sumy = T(0);
        T sumy2 = T(0);
        T sumxy = T(0);
        T minx = T(0);
        T miny = T(0);
        T maxx = T(0);
        T maxy = T(0);
        static T compute_r(size_t n, T sx, T sx2, T sy, T sy2, T sxy) {
            using std::sqrt;
            if (n < 2)
                return T(0);
            T tn = T(n);
            return (tn * sxy - sx * sy) / sqrt((tn * sx2 - sx * sx) * (tn * sy2 - sy * sy));
        }
        static std::pair<T, T> compute_linear(size_t n, T sx, T sx2, T sy, T sxy) {
            if (n == 0)
                return {T(1), T(0)};
            if (n == 1)
                return {T(1), sy - sx};
            T tn = T(n);
            T a = (tn * sxy - sx * sy) / (tn * sx2 - sx * sx);
            T b = sy / tn - a * sx / tn;
            return {a, b};
        }
    };

    struct StatisticsObject:
    AlgorithmBase<StatisticsObject> {};

    template <typename T>
    struct StatisticsType {
        using base_type = std::decay_t<T>;
        using data_type = std::conditional_t<std::is_floating_point<base_type>::value, base_type, double>;
        using result_type = Statistics<data_type>;
        static void tally(T x, result_type& s) noexcept { s.add(data_type(x)); }
    };

    template <typename T1, typename T2>
    struct StatisticsType<std::pair<T1, T2>> {
        using base_type1 = std::decay_t<T1>;
        using base_type2 = std::decay_t<T2>;
        using data_type1 = std::conditional_t<std::is_floating_point<base_type1>::value, base_type1, double>;
        using data_type2 = std::conditional_t<std::is_floating_point<base_type2>::value, base_type2, double>;
        using data_type = std::conditional_t<(sizeof(data_type2) > sizeof(data_type1)), data_type2, data_type1>;
        using result_type = Statistics<std::pair<data_type, data_type>>;
        static void tally(std::pair<T1, T2> xy, result_type& s) noexcept { s.add(xy); }
    };

    template <typename Range>
    typename StatisticsType<Meta::RangeValue<Range>>::result_type operator>>(const Range& lhs, StatisticsObject /*rhs*/) {
        typename StatisticsType<Meta::RangeValue<Range>>::result_type s;
        for (auto& x: lhs)
            StatisticsType<Meta::RangeValue<Range>>::tally(x, s);
        return s;
    }

    constexpr StatisticsObject statistics = {};

    // sum, product

    template <typename T>
    struct SumObject:
    AlgorithmBase<SumObject<T>> {
        T init;
        SumObject(const T& t): init(t) {}
    };

    template <>
    struct SumObject<void>:
    AlgorithmBase<SumObject<void>> {
        template <typename T> SumObject<T> operator()(const T& t) const { return {t}; }
    };

    template <typename T>
    struct ProductObject:
    AlgorithmBase<ProductObject<T>> {
        T init;
        ProductObject(const T& t): init(t) {}
    };

    template <>
    struct ProductObject<void>:
    AlgorithmBase<ProductObject<void>> {
        template <typename T> ProductObject<T> operator()(const T& t) const { return {t}; }
    };

    template <typename Range, typename T>
    T operator>>(const Range& lhs, const SumObject<T>& rhs) {
        using std::begin;
        using std::end;
        return std::accumulate(begin(lhs), end(lhs), rhs.init);
    }

    template <typename Range>
    Meta::RangeValue<Range> operator>>(const Range& lhs, const SumObject<void>& /*rhs*/) {
        using std::begin;
        using std::end;
        return std::accumulate(begin(lhs), end(lhs), Meta::RangeValue<Range>());
    }

    template <typename Range, typename T>
    T operator>>(const Range& lhs, const ProductObject<T>& rhs) {
        using std::begin;
        using std::end;
        return std::accumulate(begin(lhs), end(lhs), rhs.init, std::multiplies<>());
    }

    template <typename Range>
    Meta::RangeValue<Range> operator>>(const Range& lhs, const ProductObject<void>& /*rhs*/) {
        using std::begin;
        using std::end;
        return std::accumulate(begin(lhs), end(lhs), Meta::RangeValue<Range>(1), std::multiplies<>());
    }

    constexpr SumObject<void> sum = {};
    constexpr ProductObject<void> product = {};

}
