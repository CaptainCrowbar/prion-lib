#pragma once

#include "rs-core/common.hpp"
#include <map>
#include <unordered_map>
#include <vector>

namespace RS {

    template <typename T>
    class PrimeGenerator {
    public:
        const T& get() const noexcept { return current; }
        void next() {
            if (current < T(3)) {
                ++current;
                return;
            }
            if (current < T(7)) {
                current += 2;
                return;
            }
            if (! recurse) {
                multiples.clear();
                recurse = make_copy_ptr<PrimeGenerator>();
                recurse->next();
                recurse->next();
                factor = recurse->get();
                square = factor * factor;
            }
            for (;;) {
                current += T(2);
                if (current == square) {
                    T delta = T(2) * factor;
                    T next = square + delta;
                    multiples.insert({next, delta});
                    recurse->next();
                    factor = recurse->get();
                    square = factor * factor;
                } else {
                    auto it = multiples.find(current);
                    if (it == multiples.end())
                        return;
                    T delta = it->second;
                    T next = it->first + delta;
                    while (multiples.count(next))
                        next += delta;
                    multiples.erase(it);
                    multiples.insert({next, delta});
                }
            }
        }
        T operator()() {
            next();
            return get();
        }
    private:
        std::unordered_map<T, T> multiples;
        CopyPtr<PrimeGenerator> recurse;
        T current = T(1);
        T factor = T(0);
        T square = T(0);
    };

    template <typename T>
    class PrimeIterator:
    public ForwardIterator<PrimeIterator<T>, const T> {
    public:
        PrimeIterator() { gen.next(); }
        explicit PrimeIterator(bool init) { if (init) gen.next(); }
        const T& operator*() const noexcept { return gen.get(); }
        PrimeIterator& operator++() { gen.next(); return *this; }
        bool operator==(const PrimeIterator& rhs) const noexcept { return gen.get() == rhs.gen.get(); }
    private:
        PrimeGenerator<T> gen;
    };

    template <typename T>
    auto prime_numbers() {
        using PI = PrimeIterator<T>;
        return irange(PI(true), PI(false));
    }

    template <typename T>
    bool is_prime(T n) {
        if (n < T(2))
            return false;
        for (PrimeIterator<T> it; *it * *it <= n; ++it)
            if (n % *it == T(0))
                return false;
        return true;
    }

    template <typename T>
    T next_prime(T n) {
        if (n <= T(2))
            return T(2);
        if (n % T(2) == T(0))
            ++n;
        while (! is_prime(n))
            n += T(2);
        return n;
    }

    template <typename T>
    T prev_prime(T n) {
        if (n < T(2))
            return T(0);
        if (n == T(2))
            return T(2);
        if (n % T(2) == T(0))
            --n;
        while (n >= T(2) && ! is_prime(n))
            n -= T(2);
        return n;
    }

    template <typename T>
    std::vector<T> prime_factors(T n) {
        std::vector<T> factors;
        PrimeIterator<T> it;
        while (n > T(1) && *it * *it <= n) {
            if (n % *it == T(0)) {
                n /= *it;
                factors.push_back(*it);
            } else {
                ++it;
            }
        }
        if (n > T(1))
            factors.push_back(n);
        return factors;
    }

    template <typename T>
    std::map<T, T> prime_factors_map(T n) {
        std::map<T, T> factors;
        PrimeIterator<T> it;
        while (n > T(1) && *it * *it <= n) {
            if (n % *it == T(0)) {
                n /= *it;
                ++factors[*it];
            } else {
                ++it;
            }
        }
        if (n > T(1))
            ++factors[n];
        return factors;
    }

    template <typename T>
    std::vector<T> prime_list(T m, T n) {
        std::vector<T> v;
        if (n < T(2)) {
            // do nothing
        } else if (m <= T(2)) {
            auto z = size_t(n);
            std::vector<bool> sieve(z + 1, true);
            sieve[0] = sieve[1] = false;
            sieve[2] = true;
            for (size_t i = 4; i <= z; i += 2)
                sieve[i] = false;
            for (size_t i = 3; 2 * i <= z; i += 2)
                if (sieve[i])
                    for (size_t j = 2 * i; j <= z; j += i)
                        sieve[j] = false;
            for (size_t i = 0; i <= z; ++i)
                if (sieve[i])
                    v.push_back(T(i));
        } else {
            for (T t = m; t <= n; ++t)
                if (is_prime(t))
                    v.push_back(t);
        }
        return v;
    }

    template <typename T>
    std::vector<T> prime_list(T n) {
        return prime_list(T(2), n);
    }

}
