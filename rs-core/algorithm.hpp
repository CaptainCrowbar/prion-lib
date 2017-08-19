#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <vector>

namespace RS {

    // Difference algorithm
    // Implementation of the Myers algorithm
    // http://xmailserver.org/diff2.pdf

    template <typename RandomAccessRange>
    struct DiffEntry {
        using iterator = RangeIterator<const RandomAccessRange>;
        using subrange = Irange<iterator>;
        subrange del, ins;
    };

    template <typename RandomAccessRange> using DiffList = std::vector<DiffEntry<RandomAccessRange>>;

    namespace RS_Detail {

        struct DiffHelper {
            using match_function = std::function<bool(ptrdiff_t, ptrdiff_t)>;
            using insert_function = std::function<void(size_t, size_t, size_t, size_t)>;
            using offset_pair = std::pair<ptrdiff_t, ptrdiff_t>;
            using offset_list = std::vector<ptrdiff_t>;
            using offset_iterator = offset_list::iterator;
            offset_list fwdpath, revpath;
            offset_iterator fwd, rev;
            std::vector<bool> lchanges, rchanges;
            size_t lsize, rsize;
            match_function match;
            insert_function insert;
            DiffHelper(size_t lcount, size_t rcount):
                fwdpath(2 * (lcount + rcount + 1)), revpath(2 * (lcount + rcount + 1)),
                fwd(fwdpath.begin() + lcount + rcount + 1), rev(revpath.begin() + lcount + rcount + 1),
                lchanges(lcount, false), rchanges(rcount, false),
                lsize(lcount), rsize(rcount),
                match(), insert() {}
            void collect_diffs() {
                size_t lpos = 0, rpos = 0;
                for (; lpos != lsize && rpos != rsize; ++lpos, ++rpos) {
                    size_t lstart = lpos, rstart = rpos;
                    for (; lpos != lsize && lchanges[lpos]; ++lpos) {}
                    for (; rpos != rsize && rchanges[rpos]; ++rpos) {}
                    if (lstart != lpos || rstart != rpos)
                        insert(lstart, lpos, rstart, rpos);
                }
                if (lpos != lsize || rpos != rsize)
                    insert(lpos, lsize, rpos, rsize);
            }
            void longest_common(ptrdiff_t l1, ptrdiff_t l2, ptrdiff_t r1, ptrdiff_t r2) {
                for (; l1 < l2 && r1 < r2 && match(l1, r1); ++l1, ++r1) {}
                for (; l1 < l2 && r1 < r2 && match(l2 - 1, r2 - 1); --l2, --r2) {}
                if (l1 == l2) {
                    std::fill(rchanges.begin() + r1, rchanges.begin() + r2, true);
                    return;
                }
                if (r1 == r2) {
                    std::fill(lchanges.begin() + l1, lchanges.begin() + l2, true);
                    return;
                }
                fwd[1] = l1;
                rev[-1] = l2;
                ptrdiff_t d1 = l1 - r1, d2 = l2 - r2, delta = d2 - d1;
                for (ptrdiff_t d = 0;; ++d) {
                    for (ptrdiff_t k = - d; k <= d; k = k + 2) {
                        ptrdiff_t i = k == - d ? fwd[k + 1] : fwd[k - 1] + 1;
                        if (k < d)
                            i = std::max(i, fwd[k + 1]);
                        for (ptrdiff_t j = i - k - d1; i < l2 && j < r2 && match(i, j); ++i, ++j) {}
                        fwd[k] = i;
                        if ((delta & 1) && k > delta - d && k < delta + d && rev[k - delta] <= fwd[k]) {
                            longest_common(l1, fwd[k], r1, fwd[k] - k - d1);
                            longest_common(fwd[k], l2, fwd[k] - k - d1, r2);
                            return;
                        }
                    }
                    for (ptrdiff_t k = - d; k <= d; k = k + 2) {
                        ptrdiff_t i = k == d ? rev[k - 1] : rev[k + 1] - 1;
                        if (k > - d)
                            i = std::min(i, rev[k - 1]);
                        for (ptrdiff_t j = i - k - d2; i > l1 && j > r1 && match(i - 1, j - 1); --i, --j) {}
                        rev[k] = i;
                        if (! (delta & 1) && k >= - d - delta && k <= d - delta && rev[k] <= fwd[k + delta]) {
                            longest_common(l1, fwd[k + delta], r1, fwd[k + delta] - k - d2);
                            longest_common(fwd[k + delta], l2, fwd[k + delta] - k - d2, r2);
                            return;
                        }
                    }
                }
            }
        };

    }

    template <typename RandomAccessRange, typename EqualityPredicate>
    DiffList<RandomAccessRange> diff(const RandomAccessRange& lhs, const RandomAccessRange& rhs, EqualityPredicate eq) {
        using std::begin;
        using std::end;
        auto lbegin = begin(lhs), rbegin = begin(rhs);
        DiffList<RandomAccessRange> diffs;
        RS_Detail::DiffHelper d(range_count(lhs), range_count(rhs));
        d.match = [=] (ptrdiff_t i, ptrdiff_t j) { return eq(lbegin[i], rbegin[j]); };
        d.insert = [=,&diffs] (size_t lpos1, size_t lpos2, size_t rpos1, size_t rpos2) { diffs.push_back({{lbegin + lpos1, lbegin + lpos2}, {rbegin + rpos1, rbegin + rpos2}}); };
        d.longest_common(0, d.lsize, 0, d.rsize);
        d.collect_diffs();
        return diffs;
    }

    template <typename RandomAccessRange>
    DiffList<RandomAccessRange> diff(const RandomAccessRange& lhs, const RandomAccessRange& rhs) {
        return diff(lhs, rhs, std::equal_to<RangeValue<RandomAccessRange>>());
    }

    // Edit distance (Levenshtein distance)
    // Based on code by JuniorCompressor
    // http://stackoverflow.com/questions/29017600

    template <typename ForwardRange1, typename ForwardRange2, typename T>
    T edit_distance(const ForwardRange1& range1, const ForwardRange2& range2, T ins, T del, T sub) {
        static_assert(std::is_arithmetic<T>::value);
        using std::begin;
        const size_t n1 = range_count(range1), n2 = range_count(range2);
        std::vector<T> buf1(n2 + 1), buf2(n2 + 1);
        for (size_t j = 1; j <= n2; ++j)
            buf1[j] = buf1[j - 1] + ins;
        auto p = begin(range1);
        for (size_t i = 1; i <= n1; ++i, ++p) {
            buf2[0] = del * static_cast<T>(i);
            auto q = begin(range2);
            for (size_t j = 1; j <= n2; ++j, ++q)
                buf2[j] = std::min({buf2[j - 1] + ins, buf1[j] + del, buf1[j - 1] + (*p == *q ? T() : sub)});
            buf1.swap(buf2);
        }
        return buf1[n2];
    }

    template <typename ForwardRange1, typename ForwardRange2>
    size_t edit_distance(const ForwardRange1& range1, const ForwardRange2& range2) {
        return edit_distance(range1, range2, size_t(1), size_t(1), size_t(1));
    }

    // Find optimum

    template <typename ForwardRange, typename UnaryFunction, typename Compare>
    RangeIterator<ForwardRange> find_optimum(ForwardRange& range, UnaryFunction f, Compare c) {
        using std::begin;
        using std::end;
        auto i = begin(range), i_end = end(range);
        if (i == i_end)
            return i_end;
        auto opt = i;
        auto opt_value = f(*i);
        for (++i; i != i_end; ++i) {
            auto test_value = f(*i);
            if (c(test_value, opt_value)) {
                opt = i;
                opt_value = test_value;
            }
        }
        return opt;
    }

    template <typename ForwardRange, typename UnaryFunction>
    RangeIterator<ForwardRange> find_optimum(ForwardRange& range, UnaryFunction f) {
        return find_optimum(range, f, std::less<>());
    }

    // Paired for each

    template <typename InputRange1, typename InputRange2, typename BinaryFunction>
    void paired_for_each(InputRange1& range1, InputRange2& range2, BinaryFunction f) {
        using std::begin;
        using std::end;
        auto i = begin(range1), i_end = end(range1);
        auto j = begin(range2), j_end = end(range2);
        for (; i != i_end && j != j_end; ++i, ++j)
            f(*i, *j);
    }

    // Paired sort

    namespace RS_Detail {

        template <bool Stable, typename RandomAccessRange1, typename RandomAccessRange2, typename Compare>
        void paired_sort(RandomAccessRange1& range1, RandomAccessRange2& range2, Compare comp) {
            using std::begin;
            using T1 = RangeValue<RandomAccessRange1>;
            using T2 = RangeValue<RandomAccessRange2>;
            auto p = begin(range1);
            auto q = begin(range2);
            size_t size = std::min(range_count(range1), range_count(range2));
            std::vector<std::pair<T1, T2>> pairs;
            pairs.reserve(size);
            for (size_t i = 0; i < size; ++i)
                pairs.push_back({p[i], q[i]});
            auto pair_comp = [comp] (const auto& a, const auto& b) { return comp(a.first, b.first); };
            if (Stable)
                std::stable_sort(pairs.begin(), pairs.end(), pair_comp);
            else
                std::sort(pairs.begin(), pairs.end(), pair_comp);
            for (size_t i = 0; i < size; ++i) {
                p[i] = pairs[i].first;
                q[i] = pairs[i].second;
            }
        }

    }

    template <typename RandomAccessRange1, typename RandomAccessRange2>
    void paired_sort(RandomAccessRange1& range1, RandomAccessRange2& range2) {
        RS_Detail::paired_sort<false>(range1, range2, std::less<>());
    }

    template <typename RandomAccessRange1, typename RandomAccessRange2, typename Compare>
    void paired_sort(RandomAccessRange1& range1, RandomAccessRange2& range2, Compare comp) {
        RS_Detail::paired_sort<false>(range1, range2, comp);
    }

    template <typename RandomAccessRange1, typename RandomAccessRange2>
    void paired_stable_sort(RandomAccessRange1& range1, RandomAccessRange2& range2) {
        RS_Detail::paired_sort<true>(range1, range2, std::less<>());
    }

    template <typename RandomAccessRange1, typename RandomAccessRange2, typename Compare>
    void paired_stable_sort(RandomAccessRange1& range1, RandomAccessRange2& range2, Compare comp) {
        RS_Detail::paired_sort<true>(range1, range2, comp);
    }

    // Paired transform

    template <typename InputRange1, typename InputRange2, typename OutputIterator, typename BinaryFunction>
    void paired_transform(const InputRange1& range1, const InputRange2& range2, OutputIterator out, BinaryFunction f) {
        using std::begin;
        using std::end;
        auto i = begin(range1), i_end = end(range1);
        auto j = begin(range2), j_end = end(range2);
        for (; i != i_end && j != j_end; ++i, ++j, ++out)
            *out = f(*i, *j);
    }

    // Topological order

    class TopologicalOrderError: public std::runtime_error { protected: explicit TopologicalOrderError(const U8string& msg): std::runtime_error("Topological order error: " + msg) {} };
        class TopologicalOrderCycle: public TopologicalOrderError { public: TopologicalOrderCycle(): TopologicalOrderError("Dependency cycle") {} };
        class TopologicalOrderEmpty: public TopologicalOrderError { public: TopologicalOrderEmpty(): TopologicalOrderError("Empty graph") {} };

    template <typename T, typename Compare = std::less<T>>
    class TopologicalOrder {
    public:
        using compare_type = Compare;
        using value_type = T;
        TopologicalOrder() = default;
        explicit TopologicalOrder(Compare c): graph(c) {}
        void clear() noexcept { graph.clear(); }
        Compare comp() const { return graph.key_comp(); }
        bool empty() const noexcept { return graph.empty(); }
        bool erase(const T& t);
        T front() const;
        std::vector<T> front_set() const;
        T back() const;
        std::vector<T> back_set() const;
        bool has(const T& t) const { return graph.count(t); }
        void insert(const T& t) { ensure_key(t); }
        template <typename... Args> void insert(const T& t1, const T& t2, const Args&... args);
        template <typename Range> void insert_1n(const T& t1, const Range& r2) { insert_ranges(array_range(&t1, 1), r2); }
        void insert_1n(const T& t1, std::initializer_list<T> r2) { insert_ranges(array_range(&t1, 1), r2); }
        template <typename Range> void insert_n1(const Range& r1, const T& t2) { insert_ranges(r1, array_range(&t2, 1)); }
        void insert_n1(std::initializer_list<T> r1, const T& t2) { insert_ranges(r1, array_range(&t2, 1)); }
        template <typename Range1, typename Range2> void insert_mn(const Range1& r1, const Range2& r2) { insert_ranges(r1, r2); }
        void insert_mn(std::initializer_list<T> r1, std::initializer_list<T> r2) { insert_ranges(r1, r2); }
        bool is_front(const T& t) const;
        bool is_back(const T& t) const;
        T pop_front();
        std::vector<T> pop_front_set();
        T pop_back();
        std::vector<T> pop_back_set();
        size_t size() const noexcept { return graph.size(); }
        U8string format_by_node() const; // For testing
        U8string format_by_set() const; // For testing
    private:
        using set_type = std::set<T, Compare>;
        struct link_sets { set_type left, right; };
        using map_type = std::map<T, link_sets, Compare>;
        using map_iterator = typename map_type::iterator;
        map_type graph;
        map_iterator ensure_key(const T& t);
        template <typename Range1, typename Range2> void insert_ranges(const Range1& r1, const Range2& r2);
        link_sets make_link_sets() const { return {set_type(graph.key_comp()), set_type(graph.key_comp())}; }
    };

    template <typename T, typename Compare>
    bool TopologicalOrder<T, Compare>::erase(const T& t) {
        auto i = graph.find(t);
        if (i == graph.end())
            return false;
        graph.erase(i);
        for (auto& node: graph) {
            node.second.left.erase(t);
            node.second.right.erase(t);
        }
        return true;
    }

    template <typename T, typename Compare>
    T TopologicalOrder<T, Compare>::front() const {
        if (graph.empty())
            throw TopologicalOrderEmpty();
        for (auto& node: graph)
            if (node.second.left.empty())
                return node.first;
        throw TopologicalOrderCycle();
    }

    template <typename T, typename Compare>
    std::vector<T> TopologicalOrder<T, Compare>::front_set() const {
        std::vector<T> v;
        if (graph.empty())
            return v;
        for (auto& node: graph)
            if (node.second.left.empty())
                v.push_back(node.first);
        if (v.empty())
            throw TopologicalOrderCycle();
        return v;
    }

    template <typename T, typename Compare>
    T TopologicalOrder<T, Compare>::back() const {
        if (graph.empty())
            throw TopologicalOrderEmpty();
        for (auto& node: graph)
            if (node.second.right.empty())
                return node.first;
        throw TopologicalOrderCycle();
    }

    template <typename T, typename Compare>
    std::vector<T> TopologicalOrder<T, Compare>::back_set() const {
        std::vector<T> v;
        if (graph.empty())
            return v;
        for (auto& node: graph)
            if (node.second.right.empty())
                v.push_back(node.first);
        if (v.empty())
            throw TopologicalOrderCycle();
        return v;
    }

    template <typename T, typename Compare>
    template <typename... Args>
    void TopologicalOrder<T, Compare>::insert(const T& t1, const T& t2, const Args&... args) {
        size_t n = sizeof...(args) + 2;
        std::vector<T> v{t1, t2, args...};
        std::vector<map_iterator> its(n);
        for (size_t i = 0; i < n; ++i)
            its[i] = ensure_key(v[i]);
        for (size_t i = 0; i < n - 1; ++i) {
            for (size_t j = i + 1; j < n; ++j) {
                if (its[i] != its[j]) {
                    its[i]->second.right.insert(v[j]);
                    its[j]->second.left.insert(v[i]);
                }
            }
        }
    }

    template <typename T, typename Compare>
    bool TopologicalOrder<T, Compare>::is_front(const T& t) const {
        auto i = graph.find(t);
        return i != graph.end() && i->second.left.empty();
    }

    template <typename T, typename Compare>
    bool TopologicalOrder<T, Compare>::is_back(const T& t) const {
        auto i = graph.find(t);
        return i != graph.end() && i->second.right.empty();
    }

    template <typename T, typename Compare>
    T TopologicalOrder<T, Compare>::pop_front() {
        if (graph.empty())
            throw TopologicalOrderEmpty();
        auto i = std::find_if(graph.begin(), graph.end(), [] (auto& node) { return node.second.left.empty(); });
        if (i == graph.end())
            throw TopologicalOrderCycle();
        T t = i->first;
        graph.erase(i);
        for (auto& node: graph) {
            node.second.left.erase(t);
            node.second.right.erase(t);
        }
        return t;
    }

    template <typename T, typename Compare>
    std::vector<T> TopologicalOrder<T, Compare>::pop_front_set() {
        std::vector<T> v;
        if (graph.empty())
            return v;
        auto i = graph.begin(), g_end = graph.end();
        while (i != g_end) {
            if (i->second.left.empty()) {
                v.push_back(i->first);
                graph.erase(i++);
            } else {
                ++i;
            }
        }
        if (v.empty())
            throw TopologicalOrderCycle();
        for (auto& node: graph) {
            auto remains = make_link_sets();
            std::set_difference(node.second.left.begin(), node.second.left.end(), v.begin(), v.end(), append(remains.left), graph.key_comp());
            std::set_difference(node.second.right.begin(), node.second.right.end(), v.begin(), v.end(), append(remains.right), graph.key_comp());
            node.second = std::move(remains);
        }
        return v;
    }

    template <typename T, typename Compare>
    T TopologicalOrder<T, Compare>::pop_back() {
        if (graph.empty())
            throw TopologicalOrderEmpty();
        auto i = std::find_if(graph.begin(), graph.end(), [] (auto& node) { return node.second.right.empty(); });
        if (i == graph.end())
            throw TopologicalOrderCycle();
        T t = i->first;
        graph.erase(i);
        for (auto& node: graph) {
            node.second.left.erase(t);
            node.second.right.erase(t);
        }
        return t;
    }

    template <typename T, typename Compare>
    std::vector<T> TopologicalOrder<T, Compare>::pop_back_set() {
        std::vector<T> v;
        if (graph.empty())
            return v;
        auto i = graph.begin(), g_end = graph.end();
        while (i != g_end) {
            if (i->second.right.empty()) {
                v.push_back(i->first);
                graph.erase(i++);
            } else {
                ++i;
            }
        }
        if (v.empty())
            throw TopologicalOrderCycle();
        for (auto& node: graph) {
            auto remains = make_link_sets();
            std::set_difference(node.second.left.begin(), node.second.left.end(), v.begin(), v.end(), append(remains.left), graph.key_comp());
            std::set_difference(node.second.right.begin(), node.second.right.end(), v.begin(), v.end(), append(remains.right), graph.key_comp());
            node.second = std::move(remains);
        }
        return v;
    }

    template <typename T, typename Compare>
    U8string TopologicalOrder<T, Compare>::format_by_node() const {
        U8string text;
        for (auto& node: graph) {
            text += to_str(node.first);
            if (! node.second.right.empty())
                text += " => " + to_str(node.second.right);
            text += "\n";
        }
        return text;
    }

    template <typename T, typename Compare>
    U8string TopologicalOrder<T, Compare>::format_by_set() const {
        U8string text;
        auto work = *this;
        for (int i = 1; ! work.empty(); ++i)
            text += to_str(work.pop_front_set()) + "\n";
        return text;
    }

    template <typename T, typename Compare>
    typename TopologicalOrder<T, Compare>::map_iterator TopologicalOrder<T, Compare>::ensure_key(const T& t) {
        auto i = graph.find(t);
        if (i == graph.end())
            i = graph.insert({t, make_link_sets()}).first;
        return i;
    }

    template <typename T, typename Compare>
    template <typename Range1, typename Range2>
    void TopologicalOrder<T, Compare>::insert_ranges(const Range1& r1, const Range2& r2) {
        using std::begin;
        using std::end;
        for (auto& t1: r1) {
            auto i = ensure_key(t1);
            std::copy(begin(r2), end(r2), append(i->second.right));
            i->second.right.erase(t1);
        }
        for (auto& t2: r2) {
            auto j = ensure_key(t2);
            std::copy(begin(r1), end(r1), append(j->second.left));
            j->second.left.erase(t2);
        }
    }

}
