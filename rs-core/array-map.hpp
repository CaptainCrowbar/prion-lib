#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
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
            static bool match(const value_type& v1, const value_type& v2) noexcept { return v1.first == v2.first; }
        };

        template <typename K>
        struct ArrayMapTraits<K, void> {
            using value_type = K;
            using mapped_ref = void*;
            using mapped_cref = const void*;
            static const K& key(const K& v) noexcept { return v; }
            static bool match(const K& k1, const K& k2) noexcept { return k1 == k2; }
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
        const_iterator begin() const noexcept { return array.cbegin(); }
        const_iterator cbegin() const noexcept { return array.cbegin(); }
        iterator end() noexcept { return array.end(); }
        const_iterator end() const noexcept { return array.cend(); }
        const_iterator cend() const noexcept { return array.cend(); }
        size_t capacity() const noexcept { return array.capacity(); }
        void clear() noexcept { array.clear(); }
        bool empty() const noexcept { return array.empty(); }
        bool erase(const K& k) noexcept;
        void erase(const_iterator i) noexcept { array.erase(i); }
        iterator find(const K& k) noexcept { return std::find_if(begin(), end(), [&] (const value_type& v) { return traits::match(k, v); }); }
        const_iterator find(const K& k) const noexcept { return std::find_if(begin(), end(), [&] (const value_type& v) { return traits::match(k, v); }); }
        bool has(const K& k) const noexcept { return find(k) != end(); }
        std::pair<iterator, bool> insert(const value_type& v);
        std::pair<iterator, bool> insert(const_iterator i, const value_type& v);
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

        template <typename K, typename T>
        std::pair<typename ArrayMap<K, T>::iterator, bool> ArrayMap<K, T>::insert(const_iterator i, const value_type& v) {
            if (i != end() && traits::match(*i, v))
                return {begin() + (i - cbegin()), false};
            else
                return insert(v);
        }

    namespace RS_Detail {

        template <typename K, bool PreferOrdered> constexpr char associative_mode =
            Meta::is_hashable<K> && Meta::has_less_than_operator<K> ? (PreferOrdered ? 'O' : 'U') :
            Meta::is_hashable<K> ? 'U' :
            Meta::has_less_than_operator<K> ? 'O' :
            Meta::has_equal_operator<K> ? 'A' : 'X';

        template <typename K, typename T, bool PO, char AM = associative_mode<K, PO>> struct SelectAssociative;
        template <typename K, typename T, bool PO> struct SelectAssociative<K, T, PO, 'U'> { using type = std::unordered_map<K, T>; };
        template <typename K, typename T, bool PO> struct SelectAssociative<K, T, PO, 'O'> { using type = std::map<K, T>; };
        template <typename K, typename T, bool PO> struct SelectAssociative<K, T, PO, 'A'> { using type = ArrayMap<K, T>; };
        template <typename K, bool PO> struct SelectAssociative<K, void, PO, 'U'> { using type = std::unordered_set<K>; };
        template <typename K, bool PO> struct SelectAssociative<K, void, PO, 'O'> { using type = std::set<K>; };
        template <typename K, bool PO> struct SelectAssociative<K, void, PO, 'A'> { using type = ArraySet<K>; };

    }

    template <typename K, typename T, bool PreferOrdered = false> using BasicMap = typename RS_Detail::SelectAssociative<K, T, PreferOrdered>::type;
    template <typename K, bool PreferOrdered = false> using BasicSet = typename RS_Detail::SelectAssociative<K, void, PreferOrdered>::type;

}
