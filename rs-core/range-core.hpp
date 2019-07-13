#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS::Range {

    // Internal utilities

    template <typename T2> struct GenericConstCast {
        template <typename T1> T2* operator()(T1* p) const { return const_cast<T2*>(p); }
        template <typename T1> std::shared_ptr<T2> operator()(const std::shared_ptr<T1>& p) const { return std::const_pointer_cast<T2>(p); }
    };
    template <typename T2> struct GenericDynamicCast {
        template <typename T1> T2* operator()(T1* p) const { return dynamic_cast<T2*>(p); }
        template <typename T1> std::shared_ptr<T2> operator()(const std::shared_ptr<T1>& p) const { return std::dynamic_pointer_cast<T2>(p); }
    };
    template <typename T2> struct GenericStaticCast {
        template <typename T1> T2* operator()(T1* p) const { return static_cast<T2*>(p); }
        template <typename T1> std::shared_ptr<T2> operator()(const std::shared_ptr<T1>& p) const { return std::static_pointer_cast<T2>(p); }
    };
    template <typename T2> constexpr GenericConstCast<T2> generic_const_cast = {};
    template <typename T2> constexpr GenericDynamicCast<T2> generic_dynamic_cast = {};
    template <typename T2> constexpr GenericStaticCast<T2> generic_static_cast = {};

    template <typename Range1, typename Range2, typename BinaryFunction> struct PairResultType
        { using type = std::invoke_result_t<BinaryFunction, Meta::RangeValue<Range1>, Meta::RangeValue<Range2>>; };
    template <typename Range1, typename Range2> struct PairResultType<Range1, Range2, void> { using type = std::pair<Meta::RangeValue<Range1>, Meta::RangeValue<Range2>>; };
    template <typename Range1, typename Range2, typename BinaryFunction> using PairResult = typename PairResultType<Range1, Range2, BinaryFunction>::type;

    template <typename T>
    struct SharedIterator:
    public RandomAccessIterator<SharedIterator<T>, T> {
        std::shared_ptr<std::vector<T>> share;
        typename std::vector<T>::iterator iter;
        T& operator*() const noexcept { return *iter; }
        SharedIterator& operator+=(ptrdiff_t rhs) noexcept { iter += rhs; return *this; }
        ptrdiff_t operator-(const SharedIterator& rhs) const noexcept { return iter - rhs.iter; }
    };

    template <typename T> using SharedRange = Irange<SharedIterator<T>>;

    template <typename T, typename... Args>
    SharedRange<T> make_shared_range(Args&&... args) {
        SharedIterator<T> i, j;
        i.share = j.share = std::make_shared<std::vector<T>>(std::forward<Args>(args)...);
        i.iter = i.share->begin();
        j.iter = j.share->end();
        return {i, j};
    }

    // Algorithm combinators

    template <typename T>
    struct AlgorithmBase {};

    template <typename T1, typename T2>
    struct AlgorithmCombinator:
    AlgorithmBase<AlgorithmCombinator<T1, T2>> {
        T1 first;
        T2 second;
        AlgorithmCombinator(const T1& t1, const T2& t2): first(t1), second(t2) {}
    };

    template <typename T1, typename T2>
    AlgorithmCombinator<T1, T2> operator*(const AlgorithmBase<T1>& lhs, const AlgorithmBase<T2>& rhs) {
        return {*static_cast<const T1*>(&lhs), *static_cast<const T2*>(&rhs)};
    }

    template <typename Range, typename T1, typename T2>
    auto operator>>(const Range& lhs, const AlgorithmCombinator<T1, T2>& rhs) {
        return lhs >> rhs.first >> rhs.second;
    }

    template <typename Range, typename T1, typename T2>
    Range& operator<<(Range& lhs, const AlgorithmCombinator<T1, T2>& rhs) {
        return lhs << rhs.first << rhs.second;
    }

    // Output iterator wrapper

    template <typename OutputIterator>
    struct OutputObject:
    AlgorithmBase<OutputObject<OutputIterator>> {
        OutputIterator iter;
        OutputObject(OutputIterator i): iter(i) {}
    };

    template <typename Range, typename OutputIterator>
    void operator>>(const Range& lhs, OutputObject<OutputIterator> rhs) {
        using std::begin;
        using std::end;
        std::copy(begin(lhs), end(lhs), rhs.iter);
    }

    template <typename OutputIterator>
    inline OutputObject<OutputIterator> output(OutputIterator i) {
        return i;
    }

    // Trivial passthrough filter

    struct PassthroughObject:
    AlgorithmBase<PassthroughObject> {};

    template <typename Range>
    class PassthroughIterator:
    public FlexibleRandomAccessIterator<PassthroughIterator<Range>, const Meta::RangeValue<Range>> {
    public:
        using underlying_iterator = Meta::RangeIterator<const Range>;
        using iterator_category = typename std::iterator_traits<underlying_iterator>::iterator_category;
        PassthroughIterator() = default;
        PassthroughIterator(underlying_iterator i): iter(i) {}
        const auto& operator*() const noexcept { return *iter; }
        PassthroughIterator& operator++() { ++iter; return *this; }
        PassthroughIterator& operator--() { --iter; return *this; }
        PassthroughIterator& operator+=(ptrdiff_t rhs) { iter += rhs; return *this; }
        ptrdiff_t operator-(const PassthroughIterator& rhs) const { return iter - rhs.iter; }
        bool operator==(const PassthroughIterator& rhs) const noexcept { return iter == rhs.iter; }
    private:
        underlying_iterator iter;
    };

    template <typename Range>
    Irange<PassthroughIterator<Range>> operator>>(const Range& lhs, PassthroughObject /*rhs*/) {
        using std::begin;
        using std::end;
        return {{begin(lhs)}, {end(lhs)}};
    }

    template <typename Range>
    Range& operator<<(Range& lhs, PassthroughObject /*rhs*/) {
        return lhs;
    }

    constexpr PassthroughObject passthrough = {};

    // Collect a range as a container

    struct CollectObject:
    AlgorithmBase<CollectObject> {};

    template <typename Range>
    auto operator>>(const Range& lhs, CollectObject /*rhs*/) {
        using std::begin;
        using std::end;
        return make_shared_range<Meta::RangeValue<Range>>(begin(lhs), end(lhs));
    }

    constexpr CollectObject collect = {};

    template <typename Container>
    struct CollectAsObject:
    AlgorithmBase<CollectAsObject<Container>> {};

    template <typename Range, typename Container>
    auto operator>>(const Range& lhs, CollectAsObject<Container> /*rhs*/) {
        using std::begin;
        using std::end;
        return Container(begin(lhs), end(lhs));
    }

    template <typename Container>
    inline CollectAsObject<Container> collect_as() {
        return {};
    }

    // Simple iteration

    template <typename UnaryFunction>
    struct EachObject:
    AlgorithmBase<EachObject<UnaryFunction>> {
        UnaryFunction fun;
        EachObject(const UnaryFunction& f): fun(f) {}
    };

    template <typename Range, typename UnaryFunction>
    const Range& operator>>(const Range& lhs, EachObject<UnaryFunction> rhs) {
        using std::begin;
        using std::end;
        std::for_each(begin(lhs), end(lhs), rhs.fun);
        return lhs;
    }

    template <typename Range, typename UnaryFunction>
    Range& operator<<(Range& lhs, EachObject<UnaryFunction> rhs) {
        using std::begin;
        using std::end;
        std::for_each(begin(lhs), end(lhs), rhs.fun);
        return lhs;
    }

    template <typename UnaryFunction>
    inline EachObject<UnaryFunction> each(UnaryFunction f) {
        return f;
    }

    template <typename BinaryFunction>
    inline auto each_pair(BinaryFunction f) {
        return each([f] (auto& pair) { return f(pair.first, pair.second); });
    }

}
