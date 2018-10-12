#pragma once

#include "rs-core/common.hpp"
#include "rs-core/time.hpp"
#include <atomic>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace RS {

    class ThreadPool:
    public PollWait {
    public:
        RS_NO_COPY_MOVE(ThreadPool);
        using callback = std::function<void()>;
        ThreadPool(): ThreadPool(0) {}
        explicit ThreadPool(size_t threads);
        ~ThreadPool() noexcept;
        virtual bool poll() { return ! queued_jobs; }
        void clear() noexcept;
        void insert(const callback& call);
        void insert(callback&& call);
        size_t pending() const noexcept { return queued_jobs; }
        size_t size() const noexcept { return workers.size(); }
    private:
        struct worker {
            std::mutex mutex;
            std::deque<callback> queue;
            std::thread thread;
        };
        std::atomic<size_t> clear_count;
        std::atomic<size_t> next_worker;
        std::atomic<size_t> queued_jobs;
        std::atomic<bool> shutting_down;
        std::vector<worker> workers;
        void thread_payload(worker& work) noexcept;
        static size_t adjust_threads(size_t threads) noexcept;
    };

}
