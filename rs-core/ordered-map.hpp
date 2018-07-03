#pragma once

#include "rs-core/common.hpp"
#include <iterator>
#include <list>
#include <unordered_map>
#include <utility>

namespace RS {

    template <typename K, typename T>
    class OrderedMap {
    private:
        using pair_list = std::list<std::pair<const K&, T>>;
        using key_index = std::unordered_map<K, typename pair_list::iterator>;
        key_index keys;
        pair_list pairs;
    public:
        using const_iterator = typename pair_list::const_iterator;
        using iterator = typename pair_list::iterator;
        using key_type = K;
        using mapped_type = T;
        using value_type = std::pair<const K, T>;
        T& operator[](const K& k);
        iterator begin() { return pairs.begin(); }
        const_iterator begin() const { return pairs.cbegin(); }
        const_iterator cbegin() const { return pairs.cbegin(); }
        iterator end() { return pairs.end(); }
        const_iterator end() const { return pairs.cend(); }
        const_iterator cend() const { return pairs.cend(); }
        void clear() noexcept { pairs.clear(); keys.clear(); }
        bool empty() const noexcept { return pairs.empty(); }
        bool erase(const K& k) noexcept;
        void erase(const_iterator i) noexcept;
        iterator find(const K& k);
        const_iterator find(const K& k) const;
        bool has(const K& k) const { return keys.count(k) != 0; }
        std::pair<iterator, bool> insert(const K& k, const T& t);
        std::pair<iterator, bool> insert(const value_type& v) { insert(v.first, v.second); }
        iterator set(const K& k, const T& t);
        iterator set(const value_type& v) { set(v.first, v.second); }
        size_t size() const noexcept { return pairs.size(); }
        void swap(OrderedMap& om) noexcept { keys.swap(om.keys); pairs.swap(om.pairs); }
        friend void swap(OrderedMap& om1, OrderedMap& om2) noexcept { om1.swap(om2); }
    };

        template <typename K, typename T>
        T& OrderedMap<K, T>::operator[](const K& k) {
            auto [ki, kflag] = keys.insert({k, {}});
            if (kflag) {
                pairs.push_back({ki->first, {}});
                ki->second = std::prev(pairs.end());
            }
            return ki->second->second;
        }

        template <typename K, typename T>
        bool OrderedMap<K, T>::erase(const K& k) noexcept {
            auto ki = keys.find(k);
            if (ki == keys.end())
                return false;
            pairs.erase(ki->second);
            keys.erase(ki);
            return true;
        }

        template <typename K, typename T>
        void OrderedMap<K, T>::erase(const_iterator i) noexcept {
            auto ki = keys.find(i->first);
            pairs.erase(i);
            keys.erase(ki);
        }

        template <typename K, typename T>
        typename OrderedMap<K, T>::iterator OrderedMap<K, T>::find(const K& k) {
            auto ki = keys.find(k);
            if (ki == keys.end())
                return end();
            else
                return ki->second;
        }

        template <typename K, typename T>
        typename OrderedMap<K, T>::const_iterator OrderedMap<K, T>::find(const K& k) const {
            auto ki = keys.find(k);
            if (ki == keys.end())
                return end();
            else
                return ki->second;
        }

        template <typename K, typename T>
        std::pair<typename OrderedMap<K, T>::iterator, bool> OrderedMap<K, T>::insert(const K& k, const T& t) {
            auto [ki, kflag] = keys.insert({k, {}});
            if (kflag) {
                pairs.push_back({ki->first, t});
                ki->second = std::prev(pairs.end());
            }
            return {ki->second, kflag};
        }

        template <typename K, typename T>
        typename OrderedMap<K, T>::iterator OrderedMap<K, T>::set(const K& k, const T& t) {
            auto [ki, kflag] = keys.insert({k, {}});
            if (kflag) {
                pairs.push_back({ki->first, t});
                ki->second = std::prev(pairs.end());
            } else {
                ki->second->second = t;
            }
            return ki->second;
        }

}
