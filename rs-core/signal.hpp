#pragma once

#include "rs-core/string.hpp"
#include "rs-core/channel.hpp"
#include <atomic>
#include <condition_variable>
#include <csignal>
#include <deque>
#include <initializer_list>
#include <mutex>
#include <vector>

namespace RS {

    // Ctrl-C => SIGINT
    // kill => SIGTERM by default

    class PosixSignal:
    public MessageChannel<int> {
    public:
        RS_NO_COPY_MOVE(PosixSignal);
        using signal_list = std::vector<int>;
        PosixSignal(std::initializer_list<int> list): PosixSignal(signal_list{list}) {}
        explicit PosixSignal(const signal_list& list);
        virtual ~PosixSignal() noexcept;
        virtual void close() noexcept;
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool read(int& t);
        virtual bool is_async() const noexcept { return false; }
        static Ustring name(int s);
    protected:
        virtual bool do_wait_for(duration t);
    private:
        #ifdef _XOPEN_SOURCE
            signal_list signals;
            sigset_t newmask;
            sigset_t oldmask;
            std::deque<int> queue;
            std::atomic<bool> open;
        #else
            std::mutex mutex;
            std::condition_variable cv;
            bool open;
        #endif
    };

}
