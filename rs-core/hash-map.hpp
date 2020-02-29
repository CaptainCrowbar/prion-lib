#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS {

    template <typename K, typename T, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
        class HashMap;
    template <typename K, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
        using HashSet = HashMap<K, void, Hash, Equal>;

    namespace detail {

        template <typename Map, typename K, typename T>
        class HashMapBase {
        public:
            T& operator[](K key) {
                auto& self = *static_cast<Map*>(this);
                return self.emplace(key).first->second;
            }
            T& at(K key) {
                auto& self = *static_cast<Map*>(this);
                size_t index = self.find_known(key);
                if (index == npos)
                    throw std::out_of_range("Key not found in hash map");
                return self.table_[index].value().second;
            }
            const T& at(K key) const {
                auto& self = *static_cast<const Map*>(this);
                size_t index = self.find_known(key);
                if (index == npos)
                    throw std::out_of_range("Key not found in hash map");
                return self.table_[index].value().second;
            }
            template <typename K1, typename... TS> auto emplace(K1&& k, TS&&... ts) {
                auto& self = *static_cast<Map*>(this);
                K key{std::forward<K1>(k)};
                auto pair = self.insert_helper(key);
                if (pair.second)
                    pair.first.ptr_->emplace(key, std::forward<TS>(ts)...);
                return pair;
            }
        protected:
            using value_type = std::pair<const K, T>;
            static K get_key(const value_type& value) { return value.first; }
            static const T& get_mapped(const value_type& value) { return value.second; }
        };

        template <typename Map, typename K>
        class HashMapBase<Map, K, void> {
        public:
            template <typename... TS> auto emplace(TS&&... ts) {
                auto& self = *static_cast<Map*>(this);
                K key(std::forward<TS>(ts)...);
                return self.insert(key);
            }
        protected:
            using value_type = K;
            static K get_key(const value_type& value) { return value; }
            static auto get_mapped(const value_type& /*value*/) { return nullptr; }
        };

    }

    template <typename K, typename T, typename Hash, typename Equal>
    class HashMap:
    public detail::HashMapBase<HashMap<K, T, Hash, Equal>, K, T> {

    private:

        using base = detail::HashMapBase<HashMap<K, T, Hash, Equal>, K, T>;
        friend base;

    public:

        class iterator;
        class const_iterator;

        using hasher = Hash;
        using key_equal = Equal;
        using key_type = K;
        using mapped_type = T;
        using value_type = typename base::value_type;

        HashMap() noexcept {}
        explicit HashMap(Hash h, Equal e = {}): hash_(h), equal_(e) {}
        template <typename Iterator> HashMap(Iterator i, Iterator j, Hash h = {}, Equal e = {});
        HashMap(std::initializer_list<value_type> init);
        HashMap(const HashMap& map);
        HashMap(HashMap&& map) noexcept;
        ~HashMap() noexcept { clear(); }
        HashMap& operator=(const HashMap& map);
        HashMap& operator=(HashMap&& map) noexcept;
        HashMap& operator=(std::initializer_list<value_type> init);

        // HashMapBase supplies operator[], at(), and emplace()
        // (because their implementation depends on whether this is a map or a set)

        iterator begin() noexcept;
        const_iterator begin() const noexcept { return cbegin(); }
        const_iterator cbegin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept { return cend(); }
        const_iterator cend() const noexcept;
        bool contains(K key) const noexcept;
        size_t count(K key) const noexcept { return size_t(contains(key)); }
        bool empty() const noexcept { return size_ == 0; }
        iterator find(K key) noexcept;
        const_iterator find(K key) const noexcept;
        size_t free_space() const noexcept { return threshold_ - size_ - tombstones_; }
        Hash hash_function() const { return hash_; }
        Equal key_eq() const { return equal_; }
        size_t size() const noexcept { return size_; }
        size_t table_size() const noexcept { return table_.size(); }

        void clear() noexcept;
        void erase(const_iterator i) noexcept;
        size_t erase(K key) noexcept;
        std::pair<iterator, bool> insert(const value_type& value);
        std::pair<iterator, bool> insert(value_type&& value);
        template <typename Iterator> void insert(Iterator i, Iterator j);
        void rehash() { do_rehash(table_.size()); }
        void reserve(size_t n) { do_rehash(n + (n - 1) / 3 + 1); }
        void swap(HashMap& map) noexcept;
        friend void swap(HashMap& a, HashMap& b) noexcept { a.swap(b); }

        friend bool operator==(const HashMap& lhs, const HashMap& rhs) noexcept { return lhs.is_equal(rhs); }
        friend bool operator!=(const HashMap& lhs, const HashMap& rhs) noexcept { return ! (lhs == rhs); }

    private:

        static constexpr bool is_set = std::is_same_v<T, void>;

        enum class status: uint8_t { empty, live, dead };

        struct node_type {
            alignas (value_type) char mem[sizeof(value_type)];
            status state = status::empty;
            void copy(const value_type& value) { new (mem) value_type(value); state = status::live; }
            void move(value_type&& value) noexcept { new (mem) value_type(std::move(value)); state = status::live; }
            template <typename... Args> void emplace(K key, Args&&... args) {
                new (mem) value_type(std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(args...));
                state = status::live;
            }
            void destroy() noexcept { value().~value_type(); state = status::dead; }
            value_type& value() noexcept { return *reinterpret_cast<value_type*>(mem); }
            const value_type& value() const noexcept { return *reinterpret_cast<const value_type*>(mem); }
        };

        std::vector<node_type> table_;  // Array of value+state cells
        Hash hash_;                     // Key hash function
        Equal equal_;                   // Key equality predicate
        size_t size_ = 0;               // Number of live elements
        size_t tombstones_ = 0;         // Number of tombstones
        size_t threshold_ = 0;          // Size threshold before we enlarge the table
        size_t mask_ = 0;               // Bitmask for hash table size

        void do_rehash(size_t min_size);
        size_t find_begin() const noexcept;
        size_t find_index(K key) const noexcept;
        size_t find_known(K key) const noexcept;
        std::pair<iterator, bool> insert_helper(K key);
        bool is_equal(const HashMap& rhs) const noexcept;

    };

    template <typename K, typename T, typename Hash, typename Equal>
    class HashMap<K, T, Hash, Equal>::iterator:
    public ForwardIterator<iterator, value_type> {
    public:
        value_type& operator*() const noexcept { return ptr_->value(); }
        iterator& operator++() { do ++ptr_; while (ptr_ != end_ && ptr_->state != status::live); return *this; }
        bool operator==(const iterator& i) const noexcept { return ptr_ == i.ptr_; }
    private:
        friend class HashMap;
        friend HashMap::base;
        node_type* ptr_ = nullptr;
        node_type* end_ = nullptr;
    };

    template <typename K, typename T, typename Hash, typename Equal>
    class HashMap<K, T, Hash, Equal>::const_iterator:
    public ForwardIterator<const_iterator, const value_type> {
    public:
        const_iterator() noexcept {}
        const_iterator(iterator i) noexcept: ptr_(i.ptr_), end_(i.end_) {}
        const value_type& operator*() const noexcept { return ptr_->value(); }
        const_iterator& operator++() { do ++ptr_; while (ptr_ != end_ && ptr_->state != status::live); return *this; }
        bool operator==(const const_iterator& i) const noexcept { return ptr_ == i.ptr_; }
    private:
        friend class HashMap;
        friend HashMap::base;
        const node_type* ptr_ = nullptr;
        const node_type* end_ = nullptr;
    };

    template <typename K, typename T, typename Hash, typename Equal>
    template <typename Iterator>
    HashMap<K, T, Hash, Equal>::HashMap(Iterator i, Iterator j, Hash h, Equal e):
        HashMap(h, e) {
        insert(i, j);
    }

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>::HashMap(std::initializer_list<value_type> init) {
        for (auto& v: init)
            insert(v);
    }

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>::HashMap(const HashMap& map):
        table_(map.table_.size()),
        hash_(map.hash_),
        equal_(map.equal_),
        size_(map.size_),
        tombstones_(map.tombstones_),
        threshold_(map.threshold_),
        mask_(map.mask_) {
        for (size_t i = 0, n = map.table_.size(); i != n; ++i)
            if (map.table_[i].state == status::live)
                table_[i].copy(map.table_[i].value());
    }

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>::HashMap(HashMap&& map) noexcept:
        table_(std::move(map.table_)),
        hash_(std::move(map.hash_)),
        equal_(std::move(map.equal_)),
        size_(std::exchange(map.size_, 0)),
        tombstones_(std::exchange(map.tombstones_, 0)),
        threshold_(std::exchange(map.threshold_, 0)),
        mask_(std::exchange(map.mask_, 0)) {}

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>& HashMap<K, T, Hash, Equal>::operator=(const HashMap& map) {
        HashMap temp(map);
        swap(temp);
        return *this;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>& HashMap<K, T, Hash, Equal>::operator=(HashMap&& map) noexcept {
        HashMap temp(std::move(map));
        swap(temp);
        return *this;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>& HashMap<K, T, Hash, Equal>::operator=(std::initializer_list<value_type> init) {
        HashMap temp{init};
        swap(temp);
        return *this;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    typename HashMap<K, T, Hash, Equal>::iterator HashMap<K, T, Hash, Equal>::begin() noexcept {
        iterator i;
        i.ptr_ = table_.data() + find_begin();
        i.end_ = table_.data() + table_.size();
        return i;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    typename HashMap<K, T, Hash, Equal>::const_iterator HashMap<K, T, Hash, Equal>::cbegin() const noexcept {
        const_iterator i;
        i.ptr_ = table_.data() + find_begin();
        i.end_ = table_.data() + table_.size();
        return i;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    typename HashMap<K, T, Hash, Equal>::iterator HashMap<K, T, Hash, Equal>::end() noexcept {
        iterator i;
        i.ptr_ = i.end_ = table_.data() + table_.size();
        return i;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    typename HashMap<K, T, Hash, Equal>::const_iterator HashMap<K, T, Hash, Equal>::cend() const noexcept {
        const_iterator i;
        i.ptr_ = i.end_ = table_.data() + table_.size();
        return i;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    bool HashMap<K, T, Hash, Equal>::contains(K key) const noexcept {
        return ! table_.empty() && table_[find_index(key)].state == status::live;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    typename HashMap<K, T, Hash, Equal>::iterator HashMap<K, T, Hash, Equal>::find(K key) noexcept {
        iterator i = end();
        if (! table_.empty()) {
            auto& node = table_[find_index(key)];
            if (node.state == status::live)
                i.ptr_ = &node;
        }
        return i;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    typename HashMap<K, T, Hash, Equal>::const_iterator HashMap<K, T, Hash, Equal>::find(K key) const noexcept {
        const_iterator i = end();
        if (! table_.empty()) {
            auto& node = table_[find_index(key)];
            if (node.state == status::live)
                i.ptr_ = &node;
        }
        return i;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    void HashMap<K, T, Hash, Equal>::clear() noexcept {
        for (auto& node: table_) {
            if (node.state == status::live)
                node.destroy();
            node.state = status::empty;
        }
        size_ = tombstones_ = 0;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    void HashMap<K, T, Hash, Equal>::erase(const_iterator i) noexcept {
        iterator j = end();
        j.ptr_ = table_.data() + (i.ptr_ - table_.data());
        j.ptr_->destroy();
        --size_;
        ++tombstones_;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    size_t HashMap<K, T, Hash, Equal>::erase(K key) noexcept {
        auto i = find(key);
        if (i == end())
            return 0;
        erase(i);
        return 1;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    std::pair<typename HashMap<K, T, Hash, Equal>::iterator, bool> HashMap<K, T, Hash, Equal>::insert(const value_type& value) {
        auto pair = insert_helper(base::get_key(value));
        if (pair.second)
            pair.first.ptr_->copy(value);
        return pair;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    std::pair<typename HashMap<K, T, Hash, Equal>::iterator, bool> HashMap<K, T, Hash, Equal>::insert(value_type&& value) {
        auto pair = insert_helper(base::get_key(value));
        if (pair.second)
            pair.first.ptr_->move(std::move(value));
        return pair;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    template <typename Iterator>
    void HashMap<K, T, Hash, Equal>::insert(Iterator i, Iterator j) {
        if constexpr (! std::is_same_v<typename std::iterator_traits<Iterator>::iterator_category, std::input_iterator_tag>) {
            size_t n = std::distance(i, j);
            if (n > free_space())
                rehash(size_ + n);
        }
        for (; i != j; ++i)
            insert(*i);
    }

    template <typename K, typename T, typename Hash, typename Equal>
    void HashMap<K, T, Hash, Equal>::swap(HashMap& map) noexcept {
        std::swap(table_, map.table_);
        std::swap(hash_, map.hash_);
        std::swap(equal_, map.equal_);
        std::swap(size_, map.size_);
        std::swap(tombstones_, map.tombstones_);
        std::swap(threshold_, map.threshold_);
        std::swap(mask_, map.mask_);
    }

    template <typename K, typename T, typename Hash, typename Equal>
    void HashMap<K, T, Hash, Equal>::do_rehash(size_t min_size) {
        int bits = std::max(ilog2p1(std::max(min_size, table_.size())), 4);
        size_t new_size = size_t(1) << bits;
        std::vector<node_type> temp_table(new_size);
        table_.swap(temp_table);
        size_ = tombstones_ = 0;
        threshold_ = new_size * 3 / 4;
        mask_ = new_size - 1;
        for (auto& node: temp_table) {
            if (node.state == status::live) {
                insert(std::move(node.value()));
                node.destroy();
            }
        }
    }

    template <typename K, typename T, typename Hash, typename Equal>
    size_t HashMap<K, T, Hash, Equal>::find_begin() const noexcept {
        size_t i = 0;
        while (i < table_.size() && table_[i].state != status::live)
            ++i;
        return i;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    size_t HashMap<K, T, Hash, Equal>::find_index(K key) const noexcept {
        // Returns the position of the key, or the empty slot where it belongs
        for (size_t index = hash_(key);; ++index) {
            index &= mask_;
            if (table_[index].state == status::empty
                    || (table_[index].state == status::live && equal_(base::get_key(table_[index].value()), key)))
                return index;
        }
    }

    template <typename K, typename T, typename Hash, typename Equal>
    size_t HashMap<K, T, Hash, Equal>::find_known(K key) const noexcept {
        // Returns the position of the key, or npos if it isn't there
        for (size_t index = hash_(key);; ++index) {
            index &= mask_;
            if (table_[index].state == status::empty)
                return npos;
            if (table_[index].state == status::live) {
                if (equal_(base::get_key(table_[index].value()), key))
                    return index;
                else
                    return npos;
            }
        }
    }

    template <typename K, typename T, typename Hash, typename Equal>
    std::pair<typename HashMap<K, T, Hash, Equal>::iterator, bool> HashMap<K, T, Hash, Equal>::insert_helper(K key) {
        iterator i = end();
        if (! table_.empty()) {
            i.ptr_ = table_.data() + find_index(key);
            if (i.ptr_->state == status::live)
                return {i, false};
        }
        if (size_ + tombstones_ >= threshold_) {
            do_rehash(2 * size_);
            i.ptr_ = table_.data() + find_index(key);
        }
        ++size_;
        return {i, true};
    }

    template <typename K, typename T, typename Hash, typename Equal>
    bool HashMap<K, T, Hash, Equal>::is_equal(const HashMap& rhs) const noexcept {
        if (size_ != rhs.size_)
            return false;
        for (auto& value: *this) {
            auto it = rhs.find(base::get_key(value));
            if (it == rhs.end() || base::get_mapped(*it) != base::get_mapped(value))
                return false;
        }
        return true;
    }

}
