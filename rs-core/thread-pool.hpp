#pragma once

#include "rs-core/common.hpp"
#include "rs-core/time.hpp"
#include <algorithm>
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

        inline ThreadPool::ThreadPool(size_t threads):
        hold(0), index(0), queued(0), stop(0), workers(adjust_threads(threads)), poll([this] { return ! queued; }) {
            for (size_t i = 0, n = workers.size(); i < n; ++i)
                workers[i].thread = std::thread([i,n,this] {
                    auto delta = poll.min_interval();
                    while (! stop) {
                        callback call;
                        {
                            auto& w = workers[i];
                            auto lock = make_lock(w.mutex);
                            if (! w.queue.empty()) {
                                call = std::move(w.queue.back());
                                w.queue.pop_back();
                            }
                        }
                        for (size_t j = 1; j < n && ! call && ! stop; ++j) {
                            auto& w = workers[(i + j) % n];
                            auto lock = make_lock(w.mutex);
                            if (! w.queue.empty()) {
                                call = std::move(w.queue.front());
                                w.queue.pop_front();
                            }
                        }
                        if (stop)
                            break;
                        if (call) {
                            try { call(); } catch (...) {}
                            --queued;
                            delta = poll.min_interval();
                        } else {
                            std::this_thread::sleep_for(delta);
                            delta = std::min(2 * delta, poll.max_interval());
                        }
                    }
                });
        }

        inline ThreadPool::~ThreadPool() noexcept {
            clear();
            stop = true;
            for (auto& w: workers)
                w.thread.join();
        }

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

        inline void ThreadPool::clear() noexcept {
            ++hold;
            for (auto& w: workers) {
                auto lock = make_lock(w.mutex);
                w.queue.clear();
            }
            wait();
            --hold;
        }

        inline size_t ThreadPool::adjust_threads(size_t n) noexcept {
            if (n == 0)
                n = std::thread::hardware_concurrency();
            return n ? n : 1;
        }

}
