#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <functional>
#include <iterator>
#include <utility>
#include <vector>

namespace RS {

    class HashMapCommonBase {
    protected:
        static constexpr size_t lcg_hash(size_t n) noexcept {
            // Good LCG transformations for 32 and 64 bit integers
            // from Pierre L'Ecuyer (1999), "Tables of Linear Congruential Generators of Different Sizes and Good Lattice Structure"
            // http://www.ams.org/journals/mcom/1999-68-225/S0025-5718-99-00996-5/S0025-5718-99-00996-5.pdf
            if (sizeof(size_t) <= 4)
                return size_t(32'310'901ul * n + 850'757'001ul);
            else
                return size_t(3'935'559'000'370'003'845ull * n + 8'831'144'850'135'198'739ull);
        }
        static size_t next_size(size_t n) noexcept {
            // List of good hash table prime number sizes
            // https://planetmath.org/goodhashtableprimes
            static constexpr size_t primes[] = {
                23ull, 53ull, 97ull,
                193ull, 389ull, 769ull,
                1'543ull, 3'079ull, 6'151ull,
                12'289ull, 24'593ull, 49'157ull, 98'317ull,
                196'613ull, 393'241ull, 786'433ull,
                1'572'869ull, 3'145'739ull, 6'291'469ull,
                12'582'917ull, 25'165'843ull, 50'331'653ull,
                100'663'319ull, 201'326'611ull, 402'653'189ull, 805'306'457ull,
                1'610'612'741ull
            };
            auto p = std::upper_bound(std::begin(primes), std::end(primes), n);
            if (p == std::end(primes))
                return 2 * n - 1;
            else
                return *p;
        }
    };

    template <typename Map, typename K, typename T>
    class HashMapTypeBase {
    public:
        T& operator[](K key);
    protected:
        using value_type = std::pair<const K, T>;
        static K get_key(const value_type& value) { return value.first; }
        static const T& get_mapped(const value_type& value) { return value.second; }
    };

    template <typename Map, typename K, typename T>
    T& HashMapTypeBase<Map, K, T>::operator[](K key) {
        auto& self = *static_cast<Map>(this);
        size_t index = self.find_index(key);
        if (self.table_[index].state == Map::status::live)
            return self.table_[index].value().second;
        else
            return self.insert({key, {}}).first->second;
    }

    template <typename Map, typename K>
    class HashMapTypeBase<Map, K, void> {
    protected:
        using value_type = K;
        static K get_key(const value_type& value) { return value; }
        static constexpr auto get_mapped(const value_type& /*value*/) { return nullptr; }
    };

    template <typename K, typename T, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
    class HashMap:
    public HashMapCommonBase,
    public HashMapTypeBase<HashMap<K, T, Hash, Equal>, K, T> {

    private:

        using generic_map = HashMapTypeBase<HashMap<K, T, Hash, Equal>, K, T>;
        friend generic_map;

    public:

        class iterator;
        class const_iterator;
        using key_type = K;
        using mapped_type = T;
        using value_type = typename generic_map::value_type;
        using hasher = Hash;
        using key_equal = Equal;

        HashMap() noexcept {}
        explicit HashMap(Hash h, Equal e = {}): hash_(h), equal_(e) {}
        HashMap(const HashMap& map);
        HashMap(HashMap&& map) noexcept;
        ~HashMap() noexcept { clear(); }
        HashMap& operator=(const HashMap& map);
        HashMap& operator=(HashMap&& map) noexcept;

        iterator begin() noexcept;
        const_iterator begin() const noexcept { return cbegin(); }
        const_iterator cbegin() const noexcept;
        iterator end() noexcept;
        const_iterator end() const noexcept { return cend(); }
        const_iterator cend() const noexcept;
        bool empty() const noexcept { return size_ == 0; }
        bool contains(K key) const noexcept;
        size_t count(K key) const noexcept { return size_t(contains(key)); }
        iterator find(K key) noexcept;
        const_iterator find(K key) const noexcept;
        Hash hash_function() const { return hash_; }
        Equal key_eq() const { return equal_; }
        size_t size() const noexcept { return size_; }
        size_t table_size() const noexcept { return table_.size(); }

        void clear() noexcept;
        iterator erase(const_iterator i) noexcept;
        size_t erase(K key) noexcept;
        std::pair<iterator, bool> insert(const value_type& value);
        std::pair<iterator, bool> insert(value_type&& value);
        void rehash() { do_rehash(table_.size()); }
        void reserve(size_t n) { do_rehash(n + (n - 1) / 3 + 1); }
        void swap(HashMap& map) noexcept;
        friend void swap(HashMap& a, HashMap& b) noexcept { a.swap(b); }

        friend bool operator==(const HashMap& lhs, const HashMap& rhs) noexcept { return lhs.is_equal(rhs); }
        friend bool operator!=(const HashMap& lhs, const HashMap& rhs) noexcept { return ! (lhs == rhs); }

    private:

        enum class status: uint8_t { empty, live, dead };

        struct node_type {
            alignas (value_type) char mem[sizeof(value_type)];
            status state = status::empty;
            void construct(const value_type& value) { new (mem) value_type(value); state = status::live; }
            void construct(value_type&& value) noexcept { new (mem) value_type(std::move(value)); state = status::live; }
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

        void do_rehash(size_t min_size);
        size_t find_begin() const noexcept;
        size_t find_index(K key) const noexcept;
        std::pair<iterator, bool> insert_helper(K key);
        bool is_equal(const HashMap& rhs) const noexcept;

    };

    template <typename K, typename Hash = std::hash<K>, typename Equal = std::equal_to<K>>
    using HashSet = HashMap<K, void, Hash, Equal>;

    template <typename K, typename T, typename Hash, typename Equal>
    class HashMap<K, T, Hash, Equal>::iterator:
    public ForwardIterator<iterator, value_type> {
    public:
        value_type& operator*() const noexcept { return ptr_->value(); }
        iterator& operator++() { do ++ptr_; while (ptr_ != end_ && ptr_->state != status::live); return *this; }
        bool operator==(const iterator& i) const noexcept { return ptr_ == i.ptr_; }
    private:
        friend class HashMap;
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
        const node_type* ptr_ = nullptr;
        const node_type* end_ = nullptr;
    };

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>::HashMap(const HashMap& map):
    table_(map.table_.size()),
    hash_(map.hash_),
    equal_(map.equal_),
    size_(map.size_),
    tombstones_(map.tombstones_),
    threshold_(map.threshold_) {
        for (size_t i = 0, n = map.table_.size(); i != n; ++i)
            if (map.table_[i].state == status::live)
                table_[i].construct(map.table_[i].value());
    }

    template <typename K, typename T, typename Hash, typename Equal>
    HashMap<K, T, Hash, Equal>::HashMap(HashMap&& map) noexcept:
    table_(std::move(map.table_)),
    hash_(std::move(map.hash_)),
    equal_(std::move(map.equal_)),
    size_(std::exchange(map.size_, 0)),
    tombstones_(std::exchange(map.tombstones_, 0)),
    threshold_(std::exchange(map.threshold_, 0)) {}

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
    typename HashMap<K, T, Hash, Equal>::iterator HashMap<K, T, Hash, Equal>::erase(const_iterator i) noexcept {
        iterator j = end();
        j.ptr_ = table_.data() + (i.ptr_ - table_.data());
        j.ptr_->destroy();
        --size_;
        ++tombstones_;
        return ++j;
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
        auto pair = insert_helper(generic_map::get_key(value));
        if (pair.second)
            pair.first.ptr_->construct(value);
        return pair;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    std::pair<typename HashMap<K, T, Hash, Equal>::iterator, bool> HashMap<K, T, Hash, Equal>::insert(value_type&& value) {
        auto pair = insert_helper(generic_map::get_key(value));
        if (pair.second)
            pair.first.ptr_->construct(std::move(value));
        return pair;
    }

    template <typename K, typename T, typename Hash, typename Equal>
    void HashMap<K, T, Hash, Equal>::swap(HashMap& map) noexcept {
        std::swap(table_, map.table_);
        std::swap(hash_, map.hash_);
        std::swap(equal_, map.equal_);
        std::swap(size_, map.size_);
        std::swap(tombstones_, map.tombstones_);
        std::swap(threshold_, map.threshold_);
    }

    template <typename K, typename T, typename Hash, typename Equal>
    void HashMap<K, T, Hash, Equal>::do_rehash(size_t min_size) {
        size_t new_size = std::max(table_.size(), next_size(min_size));
        std::vector<node_type> temp_table(new_size);
        table_.swap(temp_table);
        size_ = tombstones_ = 0;
        threshold_ = new_size - new_size / 4;
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
        // Scramble the hash a bit because std::hash is usually crap
        size_t index = lcg_hash(hash_(key)) % table_.size();
        while (table_[index].state == status::dead
                || (table_[index].state == status::live && ! equal_(generic_map::get_key(table_[index].value()), key))) {
            ++index;
            if (index == table_.size())
                index = 0;
        }
        return index;
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
            auto rc = rhs.find(generic_map::get_key(value));
            if (! rc.second || generic_map::get_mapped(*rc.first) != generic_map::get_mapped(value))
                return false;
        }
        return true;
    }

}
