#include "rs-core/thread-pool.hpp"
#include <algorithm>

namespace RS {

    // Class ThreadPool

    ThreadPool::ThreadPool(size_t threads):
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

    ThreadPool::~ThreadPool() noexcept {
        clear();
        stop = true;
        for (auto& w: workers)
            w.thread.join();
    }

    void ThreadPool::clear() noexcept {
        ++hold;
        for (auto& w: workers) {
            auto lock = make_lock(w.mutex);
            w.queue.clear();
        }
        wait();
        --hold;
    }

    size_t ThreadPool::adjust_threads(size_t n) noexcept {
        if (n == 0)
            n = std::thread::hardware_concurrency();
        return n ? n : 1;
    }

}
