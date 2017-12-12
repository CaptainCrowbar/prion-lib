#pragma once

#include "rs-core/common.hpp"
#include <deque>
#include <functional>
#include <iterator>
#include <tuple>
#include <unordered_map>

namespace RS {

    template <typename> class Cache;

    template <typename Result, typename... Args>
    class Cache<Result(Args...)> {
    public:
        using argument_tuple = std::tuple<Args...>;
        using function_type = std::function<Result(Args...)>;
        using result_type = Result;
        Cache() = default;
        template <typename F> explicit Cache(F f, size_t n = npos): log(), map(), fun(f), cap(n) {}
        Result operator()(Args... args);
        void clear() noexcept { log.clear(); map.clear(); }
    private:
        using cache_log = std::deque<const argument_tuple*>;
        struct cache_entry {
            Result result;
            typename cache_log::iterator log_iter;
        };
        using cache_map = std::unordered_map<argument_tuple, cache_entry, TupleHash<argument_tuple>>;
        cache_log log;
        cache_map map;
        function_type fun;
        size_t cap = 0;
    };

        template <typename Result, typename... Args>
        Result Cache<Result(Args...)>::operator()(Args... args) {
            argument_tuple tup{args...};
            auto map_iter = map.find(tup);
            bool insert = map_iter == map.end();
            log.push_back(nullptr);
            auto guard = scope_fail([&] { log.pop_back(); });
            if (insert)
                map_iter = map.insert({tup, {tuple_invoke(fun, tup), {}}}).first;
            log.back() = &map_iter->first;
            if (! insert)
                log.erase(map_iter->second.log_iter);
            map_iter->second.log_iter = std::prev(log.end());
            if (insert && map.size() > cap) {
                map.erase(*log.front());
                log.pop_front();
            }
            return map_iter->second.result;
        }

    template <typename F>
    auto memoize(F f, size_t n = npos) {
        return Cache<FunctionSignature<F>>(f, n);
    }

}
