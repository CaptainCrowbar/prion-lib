#include "rs-core/thread-pool.hpp"
#include <algorithm>
#include <chrono>
#include <random>
#include <utility>

using namespace std::chrono;

namespace RS {

    // Class ThreadPool

    ThreadPool::ThreadPool(size_t threads):
    PollWait(), clear_count(0), next_worker(0), queued_jobs(0), shutting_down(false), workers(adjust_threads(threads)) {
        for (auto& work: workers)
            work.thread = std::thread([&] { thread_payload(work); });
    }

    ThreadPool::~ThreadPool() noexcept {
        clear();
        shutting_down = true;
        for (auto& work: workers)
            work.thread.join();
    }

    void ThreadPool::clear() noexcept {
        ++clear_count;
        for (auto& work: workers) {
            auto lock = make_lock(work.mutex);
            queued_jobs -= work.queue.size();
            work.queue.clear();
        }
        wait();
        --clear_count;
    }

    void ThreadPool::insert(const callback& call) {
        if (clear_count || ! call)
            return;
        size_t i = next_worker;
        next_worker = (i + 1) % workers.size();
        auto& work = workers[i];
        auto lock = make_lock(work.mutex);
        ++queued_jobs;
        work.queue.push_back(call);
    }

    void ThreadPool::insert(callback&& call) {
        if (clear_count || ! call)
            return;
        size_t i = next_worker;
        next_worker = (i + 1) % workers.size();
        auto& work = workers[i];
        auto lock = make_lock(work.mutex);
        ++queued_jobs;
        work.queue.push_back(std::move(call));
    }

    void ThreadPool::thread_payload(worker& work) noexcept {
        size_t threads = workers.size();
        auto seed = uint32_t(uintptr_t(&work));
        std::minstd_rand rng(seed);
        std::uniform_int_distribution<size_t> random_index(0, threads - 1);
        callback call;
        for (;;) {
            call = nullptr;
            {
                auto lock = make_lock(work.mutex);
                if (! work.queue.empty()) {
                    call = std::move(work.queue.back());
                    work.queue.pop_back();
                }
            }
            if (! call) {
                auto& victim = workers[random_index(rng)];
                auto lock = make_lock(victim.mutex);
                if (! victim.queue.empty()) {
                    call = std::move(victim.queue.front());
                    victim.queue.pop_front();
                }
            }
            if (call) {
                call();
                --queued_jobs;
            } else if (shutting_down) {
                break;
            } else {
                std::this_thread::sleep_for(1ms);
            }
        }
    }

    size_t ThreadPool::adjust_threads(size_t threads) noexcept {
        if (threads == 0)
            threads = std::thread::hardware_concurrency();
        return std::max(threads, size_t(1));
    }

}
