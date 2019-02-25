#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <iterator>
#include <list>
#include <unordered_map>
#include <utility>

namespace RS {

    template <typename K, typename T>
    class OrderedMap:
    public LessThanComparable<OrderedMap<K, T>> {
    private:
        using pair_list = std::list<std::pair<const K&, T>>;
        using key_index = std::unordered_map<K, typename pair_list::iterator>;
    public:
        using const_iterator = typename pair_list::const_iterator;
        using iterator = typename pair_list::iterator;
        using key_type = K;
        using mapped_type = T;
        using value_type = std::pair<const K, T>;
        OrderedMap() = default;
        OrderedMap(const OrderedMap& map);
        OrderedMap(OrderedMap&& map) = default;
        ~OrderedMap() = default;
        OrderedMap& operator=(const OrderedMap& map);
        OrderedMap& operator=(OrderedMap&& map) = default;
        T& operator[](const K& k);
        bool operator==(const OrderedMap& rhs) const noexcept;
        bool operator<(const OrderedMap& rhs) const noexcept;
        iterator begin() { return pairs_.begin(); }
        const_iterator begin() const { return pairs_.cbegin(); }
        const_iterator cbegin() const { return pairs_.cbegin(); }
        iterator end() { return pairs_.end(); }
        const_iterator end() const { return pairs_.cend(); }
        const_iterator cend() const { return pairs_.cend(); }
        void clear() noexcept { pairs_.clear(); keys_.clear(); }
        bool empty() const noexcept { return pairs_.empty(); }
        bool erase(const K& k) noexcept;
        void erase(const_iterator i) noexcept;
        iterator find(const K& k);
        const_iterator find(const K& k) const;
        bool has(const K& k) const { return keys_.count(k) != 0; }
        std::pair<iterator, bool> insert(const K& k, const T& t);
        std::pair<iterator, bool> insert(const value_type& v) { return insert(v.first, v.second); }
        iterator set(const K& k, const T& t);
        iterator set(const value_type& v) { return set(v.first, v.second); }
        size_t size() const noexcept { return pairs_.size(); }
        void swap(OrderedMap& rhs) noexcept { keys_.swap(rhs.keys_); pairs_.swap(rhs.pairs_); }
        friend void swap(OrderedMap& lhs, OrderedMap& rhs) noexcept { lhs.swap(rhs); }
    private:
        key_index keys_;
        pair_list pairs_;
    };

        template <typename K, typename T>
        OrderedMap<K, T>::OrderedMap(const OrderedMap& map) {
            for (auto& [k,v]: map)
                insert(k, v);
        }

        template <typename K, typename T>
        OrderedMap<K, T>& OrderedMap<K, T>::operator=(const OrderedMap& map) {
            OrderedMap temp(map);
            swap(temp);
            return *this;
        }

        template <typename K, typename T>
        T& OrderedMap<K, T>::operator[](const K& k) {
            auto [ki, kflag] = keys_.insert({k, {}});
            if (kflag) {
                pairs_.push_back({ki->first, {}});
                ki->second = std::prev(pairs_.end());
            }
            return ki->second->second;
        }

        template <typename K, typename T>
        bool OrderedMap<K, T>::operator==(const OrderedMap& rhs) const noexcept {
            if (keys_.size() != rhs.keys_.size())
                return false;
            else
                return std::equal(pairs_.begin(), pairs_.end(), rhs.pairs_.begin());
        }

        template <typename K, typename T>
        bool OrderedMap<K, T>::operator<(const OrderedMap& rhs) const noexcept {
            return std::lexicographical_compare(pairs_.begin(), pairs_.end(), rhs.pairs_.begin(), rhs.pairs_.end());
        }

        template <typename K, typename T>
        bool OrderedMap<K, T>::erase(const K& k) noexcept {
            auto ki = keys_.find(k);
            if (ki == keys_.end())
                return false;
            pairs_.erase(ki->second);
            keys_.erase(ki);
            return true;
        }

        template <typename K, typename T>
        void OrderedMap<K, T>::erase(const_iterator i) noexcept {
            auto ki = keys_.find(i->first);
            pairs_.erase(i);
            keys_.erase(ki);
        }

        template <typename K, typename T>
        typename OrderedMap<K, T>::iterator OrderedMap<K, T>::find(const K& k) {
            auto ki = keys_.find(k);
            if (ki == keys_.end())
                return end();
            else
                return ki->second;
        }

        template <typename K, typename T>
        typename OrderedMap<K, T>::const_iterator OrderedMap<K, T>::find(const K& k) const {
            auto ki = keys_.find(k);
            if (ki == keys_.end())
                return end();
            else
                return ki->second;
        }

        template <typename K, typename T>
        std::pair<typename OrderedMap<K, T>::iterator, bool> OrderedMap<K, T>::insert(const K& k, const T& t) {
            auto [ki, kflag] = keys_.insert({k, {}});
            if (kflag) {
                pairs_.push_back({ki->first, t});
                ki->second = std::prev(pairs_.end());
            }
            return {ki->second, kflag};
        }

        template <typename K, typename T>
        typename OrderedMap<K, T>::iterator OrderedMap<K, T>::set(const K& k, const T& t) {
            auto [ki, kflag] = keys_.insert({k, {}});
            if (kflag) {
                pairs_.push_back({ki->first, t});
                ki->second = std::prev(pairs_.end());
            } else {
                ki->second->second = t;
            }
            return ki->second;
        }

}
