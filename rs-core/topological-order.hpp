#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <utility>
#include <vector>

namespace RS {

    class TopologicalOrderError: public std::runtime_error { protected: explicit TopologicalOrderError(const Ustring& msg): std::runtime_error("Topological order error: " + msg) {} };
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
        Ustring format_by_node() const; // For testing
        Ustring format_by_set() const; // For testing
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
    Ustring TopologicalOrder<T, Compare>::format_by_node() const {
        Ustring text;
        for (auto& node: graph) {
            text += to_str(node.first);
            if (! node.second.right.empty())
                text += " => " + to_str(node.second.right);
            text += "\n";
        }
        return text;
    }

    template <typename T, typename Compare>
    Ustring TopologicalOrder<T, Compare>::format_by_set() const {
        Ustring text;
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
