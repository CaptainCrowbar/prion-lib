#pragma once

#include "rs-core/common.hpp"
#include <functional>
#include <iterator>
#include <list>
#include <map>
#include <memory>
#include <stdexcept>
#include <unordered_map>

namespace RS {

    RS_ENUM_CLASS(IndexMode, int, 1, unique, duplicate);

    template <typename T> class IndexTable;
    template <typename K, typename T, IndexMode M = IndexMode::unique> class Index;

    class IndexCollision:
    public std::runtime_error {
    public:
        IndexCollision(): std::runtime_error("Index collision") {}
    };

    namespace RS_Detail {

        template <typename K, typename T, IndexMode M> struct IndexMapTraits;

        template <typename K, typename T>
        struct IndexMapTraits<K, T, IndexMode::unique> {
            using compare_function = std::function<bool(const K&, const K&)>;
            using map_type = std::map<K, T, compare_function>;
            using value_type = typename map_type::value_type;
            static void insert(const K& k, const T& value, map_type& map) {
                if (! map.insert(value_type(k, value)).second)
                    throw IndexCollision();
            }
        };

        template <typename K, typename T>
        struct IndexMapTraits<K, T, IndexMode::duplicate> {
            using compare_function = std::function<bool(const K&, const K&)>;
            using map_type = std::multimap<K, T, compare_function>;
            using value_type = typename map_type::value_type;
            static void insert(const K& k, const T& value, map_type& map) {
                map.insert(value_type(k, value));
            }
        };

        template <typename T>
        class IndexRefBase {
        public:
            RS_NO_COPY_MOVE(IndexRefBase);
            using list_type = std::list<T>;
            using list_iterator = typename list_type::iterator;
            using value_type = T;
            virtual ~IndexRefBase() = default;
            virtual void clear() noexcept = 0;
            virtual void erase(list_iterator i) = 0;
            virtual void insert(list_iterator i) = 0;
        protected:
            IndexRefBase() = default;
        };

        template <typename T> class IndexRef;

        template <typename K, typename T, IndexMode M>
        class IndexRef<Index<K, T, M>>:
        public IndexRefBase<T> {
        public:
            RS_NO_COPY_MOVE(IndexRef);
            using index_type = Index<K, T, M>;
            using list_type = std::list<T>;
            using list_iterator = typename list_type::iterator;
            using map_traits = typename index_type::map_traits;
            explicit IndexRef(index_type& index) noexcept: ind(index) {}
            virtual ~IndexRef() noexcept { clear(); }
            virtual void clear() noexcept { ind.map.clear(); }
            virtual void erase(list_iterator i) { ind.map.erase(ind.ext(*i)); }
            virtual void insert(list_iterator i) { map_traits::insert(ind.ext(*i), i, ind.map); }
        private:
            index_type& ind;
        };

    }

    template <typename T>
    class IndexTable {
    private:
        using list_type = std::list<T>;
    public:
        RS_NO_COPY_MOVE(IndexTable);
        using const_iterator = typename list_type::const_iterator;
        using const_reference = const T&;
        using difference_type = ptrdiff_t;
        using iterator = typename list_type::iterator;
        using reference = T&;
        using size_type = size_t;
        using value_type = T;
        IndexTable() = default;
        template <typename InputRange> explicit IndexTable(const InputRange& src);
        template <typename Iterator> IndexTable(Iterator i1, Iterator i2);
        ~IndexTable() = default;
        iterator begin() { return list.begin(); }
        const_iterator begin() const { return list.begin(); }
        void clear();
        bool empty() const noexcept { return list.empty(); }
        iterator end() { return list.end(); }
        const_iterator end() const { return list.end(); }
        void erase(iterator i);
        void erase(iterator i1, iterator i2);
        void insert(const T& t);
        template <typename InputRange> void insert(const InputRange& src) { for (auto& t: src) insert(t); }
        template <typename Iterator> void insert(Iterator i1, Iterator i2) { for (; i1 != i2; ++i1) insert(*i1); }
        void push_back(const T& t) { insert(t); }
        size_t size() const noexcept { return list.size(); }
    private:
        template <typename K, typename T2, IndexMode M> friend class Index;
        using ref_base = RS_Detail::IndexRefBase<T>;
        using ref_ptr = std::shared_ptr<ref_base>;
        using ref_map = std::unordered_map<void*, ref_ptr>;
        using ref_pair = typename ref_map::value_type;
        ref_map indices;
        list_type list;
    };

    template <typename T>
    template <typename InputRange>
    IndexTable<T>::IndexTable(const InputRange& src):
    indices(), list(begin(src), end(src)) {}

    template <typename T>
    template <typename Iterator>
    IndexTable<T>::IndexTable(Iterator i1, Iterator i2):
    indices(), list(i1, i2) {}

    template <typename T>
    void IndexTable<T>::clear() {
        list.clear();
        for (auto& pair: indices)
            pair.second->clear();
    }

    template <typename T>
    void IndexTable<T>::erase(iterator i) {
        for (auto& pair: indices)
            pair.second->erase(i);
        list.erase(i);
    }

    template <typename T>
    void IndexTable<T>::erase(iterator i1, iterator i2) {
        for (iterator i = i1; i != i2; ++i)
            for (auto& pair: indices)
                pair.second->erase(i);
        list.erase(i1, i2);
    }

    template <typename T>
    void IndexTable<T>::insert(const T& t) {
        ScopedTransaction txn;
        txn([&] { list.push_back(t); }, [&] { list.pop_back(); });
        iterator i = std::prev(end());
        for (auto& pair: indices)
            txn([&] { pair.second->insert(i); }, [&] { pair.second->erase(i); });
        txn.commit();
    }

    template <typename K, typename T, IndexMode M>
    class Index {
    private:
        RS_NO_COPY_MOVE(Index);
        using list_type = std::list<T>;
        using list_iterator = typename list_type::iterator;
        using map_traits = RS_Detail::IndexMapTraits<K, list_iterator, M>;
        using map_type = typename map_traits::map_type;
        using map_const_iterator = typename map_type::const_iterator;
        using map_iterator = typename map_type::iterator;
        template <typename CT, typename MapIter> class basic_iterator:
        public BidirectionalIterator<basic_iterator<CT, MapIter>, CT> {
        public:
            basic_iterator() = default;
            basic_iterator(const basic_iterator<T, map_iterator>& i): iter(i.iter) {}
            CT& operator*() const noexcept { return *iter->second; }
            basic_iterator& operator++() { ++iter; return *this; }
            basic_iterator& operator--() { --iter; return *this; }
            bool operator==(const basic_iterator& rhs) const noexcept { return iter == rhs.iter; }
            const K& key() const noexcept { return iter->first; }
        private:
            friend class Index;
            MapIter iter;
            basic_iterator(MapIter i): iter(i) {}
        };
    public:
        using compare_function = std::function<bool(const K&, const K&)>;
        using const_iterator = basic_iterator<const T, map_const_iterator>;
        using const_reference = const T&;
        using difference_type = ptrdiff_t;
        using extract_function = std::function<K(const T&)>;
        using iterator = basic_iterator<T, map_iterator>;
        using key_type = K;
        using reference = T&;
        using size_type = size_t;
        using table_type = IndexTable<T>;
        using value_type = T;
        static constexpr IndexMode mode = M;
        Index() = default;
        explicit Index(table_type& table);
        Index(table_type& table, extract_function extract): tab(&table), ext(extract), map(std::less<K>()) { init(); }
        Index(table_type& table, extract_function extract, compare_function compare): tab(&table), ext(extract), map(compare) { init(); }
        ~Index() noexcept { if (tab) tab->indices.erase(this); }
        iterator begin() { return map.begin(); }
        const_iterator begin() const { return map.begin(); }
        compare_function compare() const { return map.key_comp(); }
        size_t count(const K& k) const noexcept { return map.count(k); }
        bool empty() const noexcept { return map.empty(); }
        iterator end() { return map.end(); }
        const_iterator end() const { return map.end(); }
        Irange<iterator> equal_range(const K& k);
        Irange<const_iterator> equal_range(const K& k) const;
        void erase(iterator i);
        void erase(iterator i1, iterator i2);
        void erase(const K& k);
        extract_function extract() const { return ext; }
        iterator find(const K& k) { return map.find(k); }
        const_iterator find(const K& k) const { return map.find(k); }
        iterator lower_bound(const K& k) { return map.lower_bound(k); }
        const_iterator lower_bound(const K& k) const { return map.lower_bound(k); }
        size_t size() const noexcept { return map.size(); }
        table_type& table() noexcept { return *tab; }
        const table_type& table() const noexcept { return *tab; }
        iterator upper_bound(const K& k) { return map.upper_bound(k); }
        const_iterator upper_bound(const K& k) const { return map.upper_bound(k); }
    private:
        friend class IndexTable<T>;
        friend class RS_Detail::IndexRef<Index>;
        using table_iterator = typename table_type::iterator;
        using ref_type = RS_Detail::IndexRef<Index>;
        table_type* tab = nullptr;
        extract_function ext;
        map_type map;
        void init();
    };

    template <typename K, typename T, IndexMode M>
    Index<K, T, M>::Index(table_type& table):
    tab(&table), ext(), map(std::less<K>()) {
        ext = [] (const T& t) { return static_cast<K>(t); };
        init();
    }

    template <typename K, typename T, IndexMode M>
    Irange<typename Index<K, T, M>::iterator> Index<K, T, M>::equal_range(const K& k) {
        auto pair = map.equal_range(k);
        return {pair.first, pair.second};
    }

    template <typename K, typename T, IndexMode M>
    Irange<typename Index<K, T, M>::const_iterator> Index<K, T, M>::equal_range(const K& k) const {
        auto pair = map.equal_range(k);
        return {pair.first, pair.second};
    }

    template <typename K, typename T, IndexMode M>
    void Index<K, T, M>::erase(iterator i) {
        auto m = i.iter;
        auto t = m->second;
        for(auto& pair: tab->indices)
            if (pair.first != this)
                pair.second->erase(t);
        tab->list.erase(t);
        map.erase(m);
    }

    template <typename K, typename T, IndexMode M>
    void Index<K, T, M>::erase(iterator i1, iterator i2) {
        auto next = i1;
        while (i1 != i2) {
            ++next;
            erase(i1);
            i1 = next;
        }
    }

    template <typename K, typename T, IndexMode M>
    void Index<K, T, M>::erase(const K& k) {
        auto m = map.find(k);
        if (m != map.end()) {
            auto t = m->second;
            for(auto& pair: tab->indices)
                if (pair.first != this)
                    pair.second->erase(t);
            tab->list.erase(t);
            map.erase(m);
        }
    }

    template <typename K, typename T, IndexMode M>
    void Index<K, T, M>::init() {
        for (auto t = tab->begin(), e = tab->end(); t != e; ++t)
            map_traits::insert(ext(*t), t, map);
        tab->indices[this] = std::make_shared<ref_type>(*this);
    }

}
