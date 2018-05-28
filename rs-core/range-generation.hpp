#pragma once

#include "rs-core/range-core.hpp"
#include <functional>
#include <type_traits>
#include <utility>

namespace RS::Range {

    // epsilon

    template <typename T>
    struct EpsilonObject {
        const T* begin() const { return nullptr; }
        const T* end() const { return nullptr; }
    };

    template <typename T>
    inline EpsilonObject<T> epsilon() { return {}; }

    // fill, single

    template <typename T>
    class FillIterator:
    public FlexibleRandomAccessIterator<FillIterator<T>, const T> {
    public:
        FillIterator() = default;
        FillIterator(const T& t, size_t n): value(t), num(n) {}
        const T& operator*() const noexcept { return value; }
        FillIterator& operator++() noexcept { --num; return *this; }
        FillIterator& operator--() noexcept { ++num; return *this; }
        FillIterator& operator+=(ptrdiff_t rhs) noexcept { if (rhs < 0) num += size_t(- rhs); else num -= size_t(rhs); return *this; }
        ptrdiff_t operator-(const FillIterator& rhs) const noexcept { return num <= rhs.num ? ptrdiff_t(rhs.num - num) : - ptrdiff_t(num - rhs.num); }
        bool operator==(const FillIterator& rhs) const noexcept { return num == rhs.num; }
    private:
        T value;
        size_t num = 0;
    };

    template <typename Range, typename T>
    Range& operator<<(Range& lhs, const Irange<FillIterator<T>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename T>
    inline Irange<FillIterator<T>> fill(const T& t, size_t n = npos) {
        return {{t, n}, {}};
    }

    template <typename T>
    inline Irange<FillIterator<T>> single(const T& t) {
        return {{t, 1}, {}};
    }

    // generate

    template <typename Function>
    class GenerateIterator:
    public InputIterator<GenerateIterator<Function>, const std::decay_t<decltype(std::declval<Function>()())>> {
    public:
        using value_type = std::decay_t<decltype(std::declval<Function>()())>;
        using function_type = std::function<value_type()>;
        GenerateIterator() = default;
        GenerateIterator(Function f, size_t n): fun(f), num(n), ok(false) {}
        const value_type& operator*() const noexcept { if (! ok) { value = fun(); ok = true; } return value; }
        GenerateIterator& operator++() { --num; ok = false; return *this; }
        bool operator==(const GenerateIterator& rhs) const noexcept { return num == rhs.num; }
    private:
        function_type fun;
        size_t num = 0;
        mutable value_type value;
        mutable bool ok = false;
    };

    template <typename Range, typename Function>
    Range& operator<<(Range& lhs, const Irange<GenerateIterator<Function>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename Function>
    inline Irange<GenerateIterator<Function>> generate(Function f, size_t n = npos) {
        return {{f, n}, {}};
    }

    // iota

    template <typename T>
    class IotaIterator:
    public ForwardIterator<IotaIterator<T>, const T> {
    public:
        using function_type = std::function<T(const T&)>;
        IotaIterator() = default;
        template <typename UnaryFunction> IotaIterator(const T& t, UnaryFunction f, size_t n): value(t), fun(f), num(n) {}
        const T& operator*() const noexcept { return value; }
        IotaIterator& operator++() { value = fun(value); --num; return *this; }
        bool operator==(const IotaIterator& rhs) const noexcept { return num == rhs.num; }
    private:
        T value;
        function_type fun;
        size_t num = 0;
    };

    template <typename Range, typename T>
    Range& operator<<(Range& lhs, const Irange<IotaIterator<T>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename T, typename UnaryFunction>
    inline Irange<IotaIterator<T>> iota(const T& init, UnaryFunction f, size_t n = npos) {
        return {{init, f, n}, {}};
    }

    template <typename T>
    inline auto iota(const T& init) {
        return iota(init, [] (T t) { return T(++t); });
    }

    template <typename T>
    inline auto iota(const T& init, const T& delta, size_t n = npos) {
        return iota(init, [delta] (const T& t) { return T(t + delta); }, n);
    }

    // random

    template <typename RandomDistributon, typename RandomEngine>
    class RandomIterator:
    public InputIterator<RandomIterator<RandomDistributon, RandomEngine>, const typename RandomDistributon::result_type> {
    public:
        using value_type = typename RandomDistributon::result_type;
        RandomIterator() = default;
        RandomIterator(RandomDistributon d, RandomEngine& r, size_t n): dist(d), rng(&r), num(n), value(dist(r)) {}
        const value_type& operator*() const noexcept { return value; }
        RandomIterator& operator++() { --num; value = dist(*rng); return *this; }
        bool operator==(const RandomIterator& rhs) const noexcept { return num == rhs.num; }
    private:
        RandomDistributon dist;
        RandomEngine* rng;
        size_t num = 0;
        value_type value;
    };

    template <typename Range, typename RandomDistributon, typename RandomEngine>
    Range& operator<<(Range& lhs, const Irange<RandomIterator<RandomDistributon, RandomEngine>>& rhs) {
        auto i = rhs.begin();
        for (auto& x: lhs)
            x = *i++;
        return lhs;
    }

    template <typename RandomDistributon, typename RandomEngine>
    inline Irange<RandomIterator<RandomDistributon, RandomEngine>> random(RandomDistributon d, RandomEngine& r, size_t n = npos) {
        return {{d, r, n}, {}};
    }

}
