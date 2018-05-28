#pragma once

#include "rs-core/random.hpp"
#include "rs-core/range-core.hpp"
#include <algorithm>
#include <functional>
#include <iterator>

namespace RS::Range {

    // before, until, from, after

    template <typename Predicate, bool Inclusive>
    struct BeforeObject:
    AlgorithmBase<BeforeObject<Predicate, Inclusive>> {
        Predicate pred;
        BeforeObject(const Predicate& p): pred(p) {}
    };

    template <typename Predicate, bool Inclusive>
    struct AfterObject:
    AlgorithmBase<AfterObject<Predicate, Inclusive>> {
        Predicate pred;
        AfterObject(const Predicate& p): pred(p) {}
    };

    template <typename Range, typename Predicate, bool Inclusive>
    class BeforeIterator:
    public ForwardIterator<BeforeIterator<Range, Predicate, Inclusive>, const Meta::RangeValue<Range>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Meta::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        BeforeIterator() = default;
        BeforeIterator(underlying_iterator i, underlying_iterator e, Predicate p): iter(i), end(e), pred(p)
            { if (! Inclusive && i != e && p(*i)) iter = e; }
        const auto& operator*() const noexcept { return *iter; }
        BeforeIterator& operator++() {
            if (Inclusive && pred(*iter)) {
                iter = end;
            } else {
                ++iter;
                if (! Inclusive && iter != end && pred(*iter))
                    iter = end;
            }
            return *this;
        }
        bool operator==(const BeforeIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        underlying_iterator end;
        predicate_type pred;
    };

    template <typename Range, typename Predicate, bool Inclusive>
    Irange<BeforeIterator<Range, Predicate, Inclusive>> operator>>(const Range& lhs, BeforeObject<Predicate, Inclusive> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.pred}, {e, e, {rhs.pred}}};
    }

    template <typename Range, typename Predicate, bool Inclusive>
    Irange<Meta::RangeIterator<const Range>> operator>>(const Range& lhs, AfterObject<Predicate, Inclusive> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        auto i = std::find_if(b, e, rhs.pred);
        if (! Inclusive && i != e)
            ++i;
        return {i, e};
    }

    template <typename Container, typename Predicate, bool Inclusive>
    Container& operator<<(Container& lhs, BeforeObject<Predicate, Inclusive> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        auto i = std::find_if(b, e, rhs.pred);
        if (Inclusive && i != e)
            ++i;
        Container temp;
        std::copy(b, i, append(temp));
        lhs = move(temp);
        return lhs;
    }

    template <typename Container, typename Predicate, bool Inclusive>
    Container& operator<<(Container& lhs, AfterObject<Predicate, Inclusive> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        auto i = std::find_if(b, e, rhs.pred);
        if (! Inclusive && i != e)
            ++i;
        Container temp;
        std::copy(i, e, append(temp));
        lhs = move(temp);
        return lhs;
    }

    template <typename Predicate>
    inline BeforeObject<Predicate, false> before_if(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline BeforeObject<Predicate, true> until_if(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline AfterObject<Predicate, true> from_if(Predicate p) {
        return p;
    }

    template <typename Predicate>
    inline AfterObject<Predicate, false> after_if(Predicate p) {
        return p;
    }

    template <typename T>
    inline auto before(T t) {
        return before_if([t] (const auto& x) { return x == t; });
    }

    template <typename T>
    inline auto until(T t) {
        return until_if([t] (const auto& x) { return x == t; });
    }

    template <typename T>
    inline auto from(T t) {
        return from_if([t] (const auto& x) { return x == t; });
    }

    template <typename T>
    inline auto after(T t) {
        return after_if([t] (const auto& x) { return x == t; });
    }

    // filter, filter_out

    template <typename Predicate>
    struct FilterObject:
    AlgorithmBase<FilterObject<Predicate>> {
        Predicate pred;
        bool sense;
        FilterObject(const Predicate& p, bool s): pred(p), sense(s) {}
    };

    template <typename Range, typename Predicate>
    class SelectIterator:
    public ForwardIterator<SelectIterator<Range, Predicate>, const Meta::RangeValue<Range>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Meta::RangeValue<Range>;
        using predicate_type = std::function<bool(const value_type&)>;
        SelectIterator() = default;
        SelectIterator(underlying_iterator i, underlying_iterator e, Predicate p, bool s): iter(i), end(e), pred(p), sense(s) { update(); }
        const auto& operator*() const noexcept { return *iter; }
        SelectIterator& operator++() { ++iter; update(); return *this; }
        bool operator==(const SelectIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        underlying_iterator end;
        predicate_type pred;
        bool sense;
        void update() { while (iter != end && pred(*iter) != sense) ++iter; }
    };

    template <typename Range, typename Predicate>
    Irange<SelectIterator<Range, Predicate>> operator>>(const Range& lhs, FilterObject<Predicate> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.pred, rhs.sense}, {e, e, rhs.pred, rhs.sense}};
    }

    template <typename Container, typename Predicate>
    Container& operator<<(Container& lhs, FilterObject<Predicate> rhs) {
        using std::begin;
        using std::end;
        Container temp;
        std::copy_if(begin(lhs), end(lhs), append(temp), [=] (auto& x) { return rhs.pred(x) == rhs.sense; });
        lhs = move(temp);
        return lhs;
    }

    template <typename Predicate>
    inline FilterObject<Predicate> filter(Predicate p) {
        return {p, true};
    }

    template <typename Predicate>
    inline FilterObject<Predicate> filter_out(Predicate p) {
        return {p, false};
    }

    // not_null

    struct NotNullObject:
    AlgorithmBase<NotNullObject> {};

    template <typename Range>
    class NotNullIterator:
    public ForwardIterator<NotNullIterator<Range>, const Meta::RangeValue<Range>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Meta::RangeValue<Range>;
        NotNullIterator() = default;
        NotNullIterator(underlying_iterator i, underlying_iterator e): iter(i), end(e) { update(); }
        const auto& operator*() const noexcept { return *iter; }
        NotNullIterator& operator++() { ++iter; update(); return *this; }
        bool operator==(const NotNullIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        underlying_iterator end;
        void update() { while (iter != end && ! *iter) ++iter; }
    };

    template <typename Range>
    Irange<NotNullIterator<Range>> operator>>(const Range& lhs, NotNullObject) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e}, {e, e}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, NotNullObject) {
        using std::begin;
        using std::end;
        Container temp;
        std::copy_if(begin(lhs), end(lhs), append(temp), [] (auto& t) { return !!t; });
        lhs = move(temp);
        return lhs;
    }

    constexpr NotNullObject not_null = {};

    // sample_k

    template <typename RandomEngine>
    struct SampleByNumberObject:
    AlgorithmBase<SampleByNumberObject<RandomEngine>> {
        size_t num;
        RandomEngine* rng;
        SampleByNumberObject(size_t k, RandomEngine* r): num(k), rng(r) {}
    };

    template <typename RandomAccessRange, typename RandomEngine>
    auto operator>>(const RandomAccessRange& lhs, SampleByNumberObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        size_t n = std::distance(b, e), m = std::min(rhs.num, n);
        auto result = make_shared_range<Meta::RangeValue<RandomAccessRange>>(b, b + m);
        for (size_t i = m; i < n; ++i) {
            auto j = random_integer<size_t>(*rhs.rng, i);
            if (j < m)
                result.first[j] = b[i];
        }
        return result;
    }

    template <typename Container, typename RandomEngine>
    Container& operator<<(Container& lhs, SampleByNumberObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        auto sample_k = lhs >> rhs;
        Container temp(begin(sample_k), end(sample_k));
        lhs = move(temp);
        return lhs;
    }

    template <typename RandomEngine>
    inline SampleByNumberObject<RandomEngine> sample_k(size_t k, RandomEngine& rng) {
        return {k, &rng};
    }

    // sample_p

    template <typename RandomEngine>
    struct SampleByProbObject:
    AlgorithmBase<SampleByProbObject<RandomEngine>> {
        double prob;
        RandomEngine* rng;
        SampleByProbObject(double p, RandomEngine* r): prob(p), rng(r) {}
    };

    template <typename Range, typename RandomEngine>
    class SampleByProbIterator:
    public ForwardIterator<SampleByProbIterator<Range, RandomEngine>, const Meta::RangeValue<Range>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Meta::RangeValue<Range>;
        SampleByProbIterator() = default;
        SampleByProbIterator(underlying_iterator i, underlying_iterator e, double p, RandomEngine* r): iter(i), end(e), prob(p), rng(r) { update(); }
        const auto& operator*() const noexcept { return *iter; }
        SampleByProbIterator& operator++() { ++iter; update(); return *this; }
        bool operator==(const SampleByProbIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        underlying_iterator end;
        double prob = 0;
        RandomEngine* rng = nullptr;
        void update() { while (iter != end && ! random_bool(*rng, prob)) ++iter; }
    };

    template <typename Range, typename RandomEngine>
    Irange<SampleByProbIterator<Range, RandomEngine>> operator>>(const Range& lhs, SampleByProbObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.prob, rhs.rng}, {e, e, 0, nullptr}};
    }

    template <typename Container, typename RandomEngine>
    Container& operator<<(Container& lhs, SampleByProbObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        Container temp;
        std::copy_if(begin(lhs), end(lhs), append(temp), [&] (auto&) { return random_bool(*rhs.rng, rhs.prob); });
        lhs = move(temp);
        return lhs;
    }

    template <typename RandomEngine>
    inline SampleByProbObject<RandomEngine> sample_p(double p, RandomEngine& rng) {
        return {p, &rng};
    }

    // sample_replace

    template <typename RandomEngine>
    struct SampleReplaceObject:
    AlgorithmBase<SampleReplaceObject<RandomEngine>> {
        size_t num;
        RandomEngine* rng;
        SampleReplaceObject(size_t k, RandomEngine* r): num(k), rng(r) {}
    };

    template <typename RandomAccessRange, typename RandomEngine>
    auto operator>>(const RandomAccessRange& lhs, SampleReplaceObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        size_t n = std::distance(b, e);
        auto result = make_shared_range<Meta::RangeValue<RandomAccessRange>>(rhs.num);
        for (auto& x: result)
            x = b[random_integer(*rhs.rng, n)];
        return result;
    }

    template <typename Container, typename RandomEngine>
    Container& operator<<(Container& lhs, SampleReplaceObject<RandomEngine> rhs) {
        using std::begin;
        using std::end;
        auto sample = lhs >> rhs;
        Container temp(begin(sample), end(sample));
        lhs = move(temp);
        return lhs;
    }

    template <typename RandomEngine>
    inline SampleReplaceObject<RandomEngine> sample_replace(size_t k, RandomEngine& rng) {
        return {k, &rng};
    }

    // skip, take

    struct SkipObject:
    AlgorithmBase<SkipObject> {
        size_t num;
    };

    struct TakeObject:
    AlgorithmBase<TakeObject> {
        size_t num;
    };

    template <typename Range>
    class TakeIterator:
    public ForwardIterator<TakeIterator<Range>, const Meta::RangeValue<Range>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Meta::RangeValue<Range>;
        TakeIterator() = default;
        TakeIterator(underlying_iterator i, underlying_iterator e, size_t n): iter(i), end(e), num(n) { if (n == 0) iter = end; }
        const auto& operator*() const noexcept { return *iter; }
        TakeIterator& operator++() { if (--num == 0) iter = end; else ++iter; return *this; }
        bool operator==(const TakeIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        underlying_iterator end;
        size_t num;
    };

    template <typename Range, bool RA = Meta::category_is_equal<Range, std::random_access_iterator_tag>>
    struct SkipToIndex;

    template <typename Range>
    struct SkipToIndex<Range, true> {
        Meta::RangeIterator<Range> operator()(Range& r, size_t n) const {
            using std::begin;
            using std::end;
            auto b = begin(r), e = end(r);
            n = std::min(n, size_t(std::distance(b, e)));
            std::advance(b, n);
            return b;
        }
    };

    template <typename Range>
    struct SkipToIndex<Range, false> {
        Meta::RangeIterator<Range> operator()(Range& r, size_t n) const {
            using std::begin;
            using std::end;
            auto b = begin(r), e = end(r);
            for (; n > 0 && b != e; ++b, --n) {}
            return b;
        }
    };

    template <typename Range>
    Irange<Meta::RangeIterator<const Range>> operator>>(const Range& lhs, SkipObject rhs) {
        using std::end;
        return {SkipToIndex<const Range>()(lhs, rhs.num), end(lhs)};
    }

    template <typename Range>
    Irange<TakeIterator<Range>> operator>>(const Range& lhs, TakeObject rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.num}, {e, e, 0}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, SkipObject rhs) {
        using std::begin;
        auto i = SkipToIndex<Container>()(lhs, rhs.num);
        lhs.erase(begin(lhs), i);
        return lhs;
    }

    template <typename Container>
    Container& operator<<(Container& lhs, TakeObject rhs) {
        using std::end;
        auto i = SkipToIndex<Container>()(lhs, rhs.num);
        lhs.erase(i, end(lhs));
        return lhs;
    }

    inline SkipObject skip(size_t n) {
        SkipObject o;
        o.num = n;
        return o;
    }

    inline TakeObject take(size_t n) {
        TakeObject o;
        o.num = n;
        return o;
    }

    // stride

    struct StrideObject:
    AlgorithmBase<StrideObject> {
        size_t skip;
        size_t step;
        StrideObject(size_t a, size_t b): skip(a), step(b) {}
    };

    template <typename Range>
    class StrideIterator:
    public ForwardIterator<StrideIterator<Range>, const Meta::RangeValue<Range>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Meta::RangeValue<Range>;
        StrideIterator() = default;
        StrideIterator(underlying_iterator i, underlying_iterator e, size_t skip, size_t step): iter(i), end(e), delta(step) { advance_by(iter, skip, end); }
        const auto& operator*() const noexcept { return *iter; }
        StrideIterator& operator++() { advance_by(iter, delta, end); return *this; }
        bool operator==(const StrideIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        underlying_iterator end;
        size_t delta;
    };

    template <typename Range>
    Irange<StrideIterator<Range>> operator>>(const Range& lhs, StrideObject rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.skip, rhs.step}, {e, e, 0, 0}};
    }

    template <typename Container>
    Container& operator<<(Container& lhs, StrideObject rhs) {
        using std::begin;
        using std::end;
        Container temp;
        lhs >> rhs >> append(temp);
        lhs = move(temp);
        return lhs;
    }

    inline StrideObject stride(size_t step) {
        return {step - 1, step};
    }

    inline StrideObject stride(size_t skip, size_t step) {
        return {skip, step};
    }

    // unique

    template <typename EqualityPredicate>
    struct UniqueObject:
    AlgorithmBase<UniqueObject<EqualityPredicate>> {
        EqualityPredicate eq;
        UniqueObject() = default;
        UniqueObject(const EqualityPredicate& p): eq(p) {}
        template <typename EP2> UniqueObject<EP2> operator()(EP2 p) const { return p; }
    };

    template <typename Range, typename EqualityPredicate>
    class UniqueIterator:
    public ForwardIterator<UniqueIterator<Range, EqualityPredicate>, const Meta::RangeValue<Range>> {
    public:
        using iterator_category = Meta::MinCategory<Range, std::forward_iterator_tag>;
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using value_type = Meta::RangeValue<Range>;
        using equality_type = std::function<bool(const value_type&, const value_type&)>;
        UniqueIterator() = default;
        UniqueIterator(underlying_iterator i, underlying_iterator e, EqualityPredicate p): iter(i), end(e), eq(p) {}
        auto& operator*() const noexcept { return *iter; }
        UniqueIterator& operator++() { auto j = iter; do ++iter; while (iter != end && eq(*j, *iter)); return *this; }
        bool operator==(const UniqueIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
        underlying_iterator end;
        equality_type eq;
    };

    template <typename Range, typename EqualityPredicate>
    Irange<UniqueIterator<Range, EqualityPredicate>> operator>>(const Range& lhs, UniqueObject<EqualityPredicate> rhs) {
        using std::begin;
        using std::end;
        auto b = begin(lhs), e = end(lhs);
        return {{b, e, rhs.eq}, {e, e, rhs.eq}};
    }

    template <typename Container, typename EqualityPredicate>
    Container& operator<<(Container& lhs, UniqueObject<EqualityPredicate> rhs) {
        using std::begin;
        using std::end;
        Container temp;
        std::unique_copy(begin(lhs), end(lhs), append(temp), rhs.eq);
        lhs = move(temp);
        return lhs;
    }

    constexpr UniqueObject<std::equal_to<>> unique = {};

}
