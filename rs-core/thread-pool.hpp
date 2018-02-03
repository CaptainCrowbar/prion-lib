#pragma once

#include "rs-core/common.hpp"
#include "rs-core/time.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
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
        ~ThreadPool() noexcept { clear(); }
        size_t size() const noexcept { return workers.size(); }
        template <typename F> void insert(F&& f);
        size_t pending() const noexcept { return queued; }
        void clear() noexcept;
        void wait();
        template <typename R, typename P> bool wait_for(std::chrono::duration<R, P> timeout);
        template <typename C, typename D> bool wait_until(std::chrono::time_point<C, D> timeout);
    private:
        using callback = std::function<void()>;
        struct worker {
            std::mutex mutex;
            std::vector<callback> queue;
            std::thread thread;
        };
        std::atomic<bool> alive;
        std::atomic<size_t> index;
        std::atomic<size_t> queued;
        std::vector<worker> workers;
        static size_t adjust_threads(size_t n) noexcept;
        static const Backoff& backoff() noexcept;
    };

        inline ThreadPool::ThreadPool(size_t threads):
        alive(true), index(0), queued(0), workers(adjust_threads(threads)) {
            for (size_t i = 0, n = workers.size(); i < n; ++i)
                workers[i].thread = std::thread([this,i,n] {
                    auto delta = backoff().min();
                    while (alive) {
                        callback call;
                        for (size_t j = 0; j < n && alive && ! call; ++j) {
                            auto& w = workers[(i + j) % n];
                            auto lock = make_lock(w.mutex);
                            if (! w.queue.empty()) {
                                call = std::move(w.queue.back());
                                w.queue.pop_back();
                            }
                        }
                        if (! alive)
                            break;
                        if (call) {
                            try { call(); } catch (...) {}
                            --queued;
                            delta = backoff().min();
                        } else {
                            std::this_thread::sleep_for(delta);
                            delta = std::min(2 * delta, backoff().max());
                        }
                    }
                });
        }

        template <typename F>
        void ThreadPool::insert(F&& f) {
            if (! alive)
                return;
            ++queued;
            size_t i = index;
            index = (i + 1) % workers.size();
            auto& w = workers[i];
            auto lock = make_lock(w.mutex);
            callback c(std::forward<F>(f));
            w.queue.push_back(std::move(c));
        }

        inline void ThreadPool::clear() noexcept {
            alive = false;
            for (auto& w: workers) {
                auto lock = make_lock(w.mutex);
                w.queue.clear();
            }
            for (auto& w: workers)
                w.thread.join();
            alive = true;
        }

        inline void ThreadPool::wait() {
            backoff().wait([this] { return ! pending(); });
        }

        template <typename R, typename P>
        bool ThreadPool::wait_for(std::chrono::duration<R, P> timeout) {
            return backoff().wait_for([this] { return ! pending(); }, timeout);
        }

        template <typename C, typename D>
        bool ThreadPool::wait_until(std::chrono::time_point<C, D> timeout) {
            return backoff().wait_until([this] { return ! pending(); }, timeout);
        }

        inline size_t ThreadPool::adjust_threads(size_t n) noexcept {
            if (n == 0)
                n = std::thread::hardware_concurrency();
            return n ? n : 1;
        }

        inline const Backoff& ThreadPool::backoff() noexcept {
            static const Backoff b;
            return b;
        }

}
