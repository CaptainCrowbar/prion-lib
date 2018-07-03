#pragma once

#include "rs-core/common.hpp"
#include <functional>
#include <initializer_list>
#include <set>
#include <tuple>
#include <utility>

namespace RS {

    template <typename K1, typename K2, typename C1 = std::less<K1>, typename C2 = std::less<K2>>
    class MirrorMap {
    public:
        using left_key = K1;
        using right_key = K2;
        using left_compare = C1;
        using right_compare = C2;
        using value_type = std::pair<K1, K2>;
        class left_iterator;
        class right_iterator;
        struct insert_result {
            left_iterator left;
            right_iterator right;
            bool inserted = false;
        };
        using left_range = Irange<left_iterator>;
        using right_range = Irange<right_iterator>;
        MirrorMap() = default;
        explicit MirrorMap(C1 c1, C2 c2 = {});
        MirrorMap(std::initializer_list<value_type> list);
        template <typename Iterator> MirrorMap(Iterator i, Iterator j, C1 c1 = {}, C2 c2 = {});
        void clear() noexcept { right_set.clear(); left_set.clear(); }
        size_t count_left(const K1& key) const { return left_set.count(key); }
        size_t count_right(const K2& key) const { return right_set.count(key); }
        bool empty() const noexcept { return left_set.empty(); }
        left_range equal_left(const K1& key) const;
        right_range equal_right(const K2& key) const;
        void erase(left_iterator i) noexcept;
        void erase(right_iterator i) noexcept;
        bool erase(const value_type& pair) noexcept;
        size_t erase_left(const K1& key) noexcept;
        size_t erase_right(const K2& key) noexcept;
        left_iterator find_left(const K1& key) const;
        left_iterator find_left(const value_type& pair) const;
        right_iterator find_right(const K2& key) const;
        right_iterator find_right(const value_type& pair) const;
        insert_result insert(const value_type& pair);
        template <typename Iterator> void insert(Iterator i, Iterator j);
        left_range left() const { return {begin_left(), end_left()}; }
        left_iterator begin_left() const;
        left_iterator end_left() const;
        right_range right() const { return {begin_right(), end_right()}; }
        right_iterator begin_right() const;
        right_iterator end_right() const;
        C1 left_comp() const { return left_set.key_comp().cmp1; }
        C2 right_comp() const { return left_set.key_comp().cmp2; }
        left_iterator mirror(right_iterator i) const;
        right_iterator mirror(left_iterator i) const;
        size_t size() const noexcept { return left_set.size(); }
        void swap(MirrorMap& mm) noexcept;
        friend void swap(MirrorMap& mm1, MirrorMap& mm2) noexcept { mm1.swap(mm2); }
    private:
        struct left_order;
        using left_set_type = std::set<value_type, left_order>;
        using left_position = typename left_set_type::const_iterator;
        struct right_order;
        using right_set_type = std::set<left_position, right_order>;
        using right_position = typename right_set_type::const_iterator;
        left_set_type left_set;
        right_set_type right_set;
        void build_right_set();
    };

        template <typename K1, typename K2, typename C1, typename C2>
        struct MirrorMap<K1, K2, C1, C2>::left_order {
            using is_transparent = void;
            C1 cmp1;
            C2 cmp2;
            bool operator()(const value_type& lhs, const value_type& rhs) const {
                if (cmp1(lhs.first, rhs.first))
                    return true;
                else if (cmp1(rhs.first, lhs.first))
                    return false;
                else
                    return cmp2(lhs.second, rhs.second);
            }
            bool operator()(const value_type& lhs, const K1& rhs) const { return cmp1(lhs.first, rhs); }
            bool operator()(const K1& lhs, const value_type& rhs) const { return cmp1(lhs, rhs.first); }
        };

        template <typename K1, typename K2, typename C1, typename C2>
        struct MirrorMap<K1, K2, C1, C2>::right_order {
            using is_transparent = void;
            C1 cmp1;
            C2 cmp2;
            bool operator()(const value_type& lhs, const value_type& rhs) const {
                if (cmp2(lhs.second, rhs.second))
                    return true;
                else if (cmp2(rhs.second, lhs.second))
                    return false;
                else
                    return cmp1(lhs.first, rhs.first);
            }
            bool operator()(const left_position& lhs, const left_position& rhs) const { return (*this)(*lhs, *rhs); }
            bool operator()(const left_position& lhs, const value_type& rhs) const { return (*this)(*lhs, rhs); }
            bool operator()(const value_type& lhs, const left_position& rhs) const { return (*this)(lhs, *rhs); }
            bool operator()(const left_position& lhs, const K2& rhs) const { return cmp2(lhs->second, rhs); }
            bool operator()(const K2& lhs, const left_position& rhs) const { return cmp2(lhs, rhs->second); }
        };

        template <typename K1, typename K2, typename C1, typename C2>
        class MirrorMap<K1, K2, C1, C2>::left_iterator:
        public BidirectionalIterator<left_iterator, const value_type> {
        public:
            const value_type& operator*() const noexcept { return *iter; }
            left_iterator& operator++() { ++iter; return *this; }
            left_iterator& operator--() { --iter; return *this; }
            bool operator==(const left_iterator& rhs) const noexcept { return iter == rhs.iter; }
        private:
            friend class MirrorMap;
            left_position iter;
        };

        template <typename K1, typename K2, typename C1, typename C2>
        class MirrorMap<K1, K2, C1, C2>::right_iterator:
        public BidirectionalIterator<right_iterator, const value_type> {
        public:
            const value_type& operator*() const noexcept { return **iter; }
            right_iterator& operator++() { ++iter; return *this; }
            right_iterator& operator--() { --iter; return *this; }
            bool operator==(const right_iterator& rhs) const noexcept { return iter == rhs.iter; }
        private:
            friend class MirrorMap;
            right_position iter;
        };

        template <typename K1, typename K2, typename C1, typename C2>
        MirrorMap<K1, K2, C1, C2>::MirrorMap(C1 c1, C2 c2):
        left_set(left_order{c1, c2}),
        right_set(right_order{c1, c2}) {}

        template <typename K1, typename K2, typename C1, typename C2>
        MirrorMap<K1, K2, C1, C2>::MirrorMap(std::initializer_list<value_type> list):
        left_set{list},
        right_set() {
            build_right_set();
        }

        template <typename K1, typename K2, typename C1, typename C2>
        template <typename Iterator>
        MirrorMap<K1, K2, C1, C2>::MirrorMap(Iterator i, Iterator j, C1 c1, C2 c2):
        left_set(i, j, left_order{c1, c2}), right_set(right_order{c1, c2}) {
            build_right_set();
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::left_range MirrorMap<K1, K2, C1, C2>::equal_left(const K1& key) const {
            auto eqr = left_set.equal_range(key);
            left_range range;
            range.first.iter = eqr.first;
            range.second.iter = eqr.second;
            return range;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::right_range MirrorMap<K1, K2, C1, C2>::equal_right(const K2& key) const {
            auto eqr = right_set.equal_range(key);
            right_range range;
            range.first.iter = eqr.first;
            range.second.iter = eqr.second;
            return range;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        void MirrorMap<K1, K2, C1, C2>::erase(left_iterator i) noexcept {
            right_set.erase(i.iter);
            left_set.erase(i.iter);
        }

        template <typename K1, typename K2, typename C1, typename C2>
        void MirrorMap<K1, K2, C1, C2>::erase(right_iterator i) noexcept {
            auto j = *i.iter;
            right_set.erase(i.iter);
            left_set.erase(j);
        }

        template <typename K1, typename K2, typename C1, typename C2>
        bool MirrorMap<K1, K2, C1, C2>::erase(const value_type& pair) noexcept {
            auto i = left_set.find(pair);
            if (i == left_set.end())
                return false;
            right_set.erase(i);
            left_set.erase(i);
            return true;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        size_t MirrorMap<K1, K2, C1, C2>::erase_left(const K1& key) noexcept {
            auto eqr = left_set.equal_range(key);
            size_t n = 0;
            for (auto i = eqr.first; i != eqr.second; ++i, ++n)
                right_set.erase(i);
            left_set.erase(eqr.first, eqr.second);
            return n;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        size_t MirrorMap<K1, K2, C1, C2>::erase_right(const K2& key) noexcept {
            auto eqr = right_set.equal_range(key);
            size_t n = 0;
            for (auto i = eqr.first; i != eqr.second; ++i, ++n)
                left_set.erase(*i);
            right_set.erase(eqr.first, eqr.second);
            return n;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::left_iterator MirrorMap<K1, K2, C1, C2>::find_left(const K1& key) const {
            left_iterator i;
            i.iter = left_set.find(key);
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::left_iterator MirrorMap<K1, K2, C1, C2>::find_left(const value_type& pair) const {
            left_iterator i;
            i.iter = left_set.find(pair);
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::right_iterator MirrorMap<K1, K2, C1, C2>::find_right(const K2& key) const {
            right_iterator i;
            i.iter = right_set.find(key);
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::right_iterator MirrorMap<K1, K2, C1, C2>::find_right(const value_type& pair) const {
            right_iterator i;
            i.iter = right_set.find(pair);
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::insert_result MirrorMap<K1, K2, C1, C2>::insert(const value_type& pair) {
            insert_result res;
            std::tie(res.left.iter, res.inserted) = left_set.insert(pair);
            res.right.iter = right_set.insert(res.left.iter).first;
            return res;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        template <typename Iterator>
        void MirrorMap<K1, K2, C1, C2>::insert(Iterator i, Iterator j) {
            for (; i != j; ++i)
                insert(*i);
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::left_iterator MirrorMap<K1, K2, C1, C2>::begin_left() const {
            left_iterator i;
            i.iter = left_set.begin();
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::left_iterator MirrorMap<K1, K2, C1, C2>::end_left() const {
            left_iterator i;
            i.iter = left_set.end();
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::right_iterator MirrorMap<K1, K2, C1, C2>::begin_right() const {
            right_iterator i;
            i.iter = right_set.begin();
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::right_iterator MirrorMap<K1, K2, C1, C2>::end_right() const {
            right_iterator i;
            i.iter = right_set.end();
            return i;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::left_iterator MirrorMap<K1, K2, C1, C2>::mirror(right_iterator i) const {
            left_iterator j;
            j.iter = i.iter == right_set.end() ? left_set.end() : *i.iter;
            return j;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        typename MirrorMap<K1, K2, C1, C2>::right_iterator MirrorMap<K1, K2, C1, C2>::mirror(left_iterator i) const {
            right_iterator j;
            j.iter = i.iter == left_set.end() ? right_set.end() : right_set.find(i.iter);
            return j;
        }

        template <typename K1, typename K2, typename C1, typename C2>
        void MirrorMap<K1, K2, C1, C2>::swap(MirrorMap& mm) noexcept {
            left_set.swap(mm.left_set);
            right_set.swap(mm.right_set);
        }

        template <typename K1, typename K2, typename C1, typename C2>
        void MirrorMap<K1, K2, C1, C2>::build_right_set() {
            for (auto i = left_set.begin(); i != left_set.end(); ++i)
                right_set.insert(i);
        }

}
