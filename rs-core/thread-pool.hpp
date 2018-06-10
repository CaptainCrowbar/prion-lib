#pragma once

#include "rs-core/common.hpp"
#include "rs-core/time.hpp"
#include <atomic>
#include <chrono>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace RS {

    class ThreadPool {
    public:
        RS_NO_COPY_MOVE(ThreadPool);
        ThreadPool(): ThreadPool(0) {}
        explicit ThreadPool(size_t threads);
        ~ThreadPool() noexcept;
        size_t size() const noexcept { return workers.size(); }
        template <typename F> void insert(F&& f);
        size_t pending() const noexcept { return queued; }
        void clear() noexcept;
        void wait() { poll.wait(); }
        template <typename R, typename P> bool wait_for(std::chrono::duration<R, P> timeout) { return poll.wait_for(timeout); }
        template <typename C, typename D> bool wait_until(std::chrono::time_point<C, D> timeout) { return poll.wait_until(timeout); }
    private:
        using callback = std::function<void()>;
        struct worker {
            std::mutex mutex;
            std::deque<callback> queue;
            std::thread thread;
        };
        std::atomic<size_t> hold;
        std::atomic<size_t> index;
        std::atomic<size_t> queued;
        std::atomic<size_t> stop;
        std::vector<worker> workers;
        PollCondition poll;
        static size_t adjust_threads(size_t n) noexcept;
    };

        template <typename F>
        void ThreadPool::insert(F&& f) {
            if (hold)
                return;
            size_t i = index;
            index = (i + 1) % workers.size();
            auto& w = workers[i];
            auto lock = make_lock(w.mutex);
            callback c(std::forward<F>(f));
            ++queued;
            w.queue.push_back(std::move(c));
        }

}
