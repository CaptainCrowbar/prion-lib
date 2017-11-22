#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <utility>
#include <vector>

namespace RS {

    namespace RS_Detail {

        template <typename K, typename T>
        struct ArrayMapTraits {
            using value_type = std::pair<K, T>;
            using mapped_ref = T&;
            using mapped_cref = const T&;
            static const K& key(const value_type& v) noexcept { return v.first; }
            static bool match(const K& k, const value_type& v) noexcept { return k == v.first; }
        };

        template <typename K>
        struct ArrayMapTraits<K, void> {
            using value_type = K;
            using mapped_ref = void*;
            using mapped_cref = const void*;
            static const K& key(const K& v) noexcept { return v; }
            static bool match(const K& k, const K& v) noexcept { return k == v; }
        };

    }

    template <typename K, typename T>
    class ArrayMap:
    public LessThanComparable<ArrayMap<K, T>> {
    private:
        using traits = RS_Detail::ArrayMapTraits<K, T>;
        using array_type = std::vector<typename traits::value_type>;
        array_type array;
    public:
        using const_iterator = typename array_type::const_iterator;
        using iterator = typename array_type::iterator;
        using key_type = K;
        using mapped_type = T;
        using value_type = typename traits::value_type;
        ArrayMap() = default;
        ArrayMap(std::initializer_list<value_type> list): array(list) {}
        typename traits::mapped_ref operator[](const K& k); // map only
        iterator begin() noexcept { return array.begin(); }
        const_iterator begin() const noexcept { return array.begin(); }
        iterator end() noexcept { return array.end(); }
        const_iterator end() const noexcept { return array.end(); }
        size_t capacity() const noexcept { return array.capacity(); }
        void clear() noexcept { array.clear(); }
        bool empty() const noexcept { return array.empty(); }
        bool erase(const K& k) noexcept;
        void erase(const_iterator i) noexcept { array.erase(i); }
        iterator find(const K& k) noexcept { return std::find_if(begin(), end(), [&] (const value_type& v) { return traits::match(k, v); }); }
        const_iterator find(const K& k) const noexcept { return std::find_if(begin(), end(), [&] (const value_type& v) { return traits::match(k, v); }); }
        bool has(const K& k) const noexcept { return find(k) != end(); }
        std::pair<iterator, bool> insert(const value_type& v);
        void reserve(size_t n) { array.reserve(n); }
        size_t size() const noexcept { return array.size(); }
        friend bool operator==(const ArrayMap& lhs, const ArrayMap& rhs) noexcept { return lhs.array == rhs.array; }
        friend bool operator<(const ArrayMap& lhs, const ArrayMap& rhs) noexcept { return lhs.array < rhs.array; }
    };

    template <typename K> using ArraySet = ArrayMap<K, void>;

    template <typename K, typename T>
    typename ArrayMap<K, T>::traits::mapped_ref ArrayMap<K, T>::operator[](const K& k) {
        auto i = find(k);
        if (i == end()) {
            array.push_back({k, {}});
            i = std::prev(end());
        }
        return i->second;
    }

    template <typename K, typename T>
    bool ArrayMap<K, T>::erase(const K& k) noexcept {
        auto i = find(k);
        if (i == end())
            return false;
        array.erase(i);
        return true;
    }

    template <typename K, typename T>
    std::pair<typename ArrayMap<K, T>::iterator, bool> ArrayMap<K, T>::insert(const value_type& v) {
        auto i = find(traits::key(v));
        if (i != end())
            return {i, false};
        array.push_back(v);
        return {std::prev(end()), true};
    }

}
