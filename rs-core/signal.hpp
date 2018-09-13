#pragma once

#include "rs-core/string.hpp"
#include "rs-core/channel.hpp"
#include "rs-core/time.hpp"
#include <atomic>
#include <condition_variable>
#include <csignal>
#include <deque>
#include <initializer_list>
#include <mutex>
#include <vector>

namespace RS {

    class PosixSignal:
    public MessageChannel<int> {
    public:
        RS_NO_COPY_MOVE(PosixSignal);
        using signal_list = std::vector<int>;
        PosixSignal(std::initializer_list<int> list);
        explicit PosixSignal(const signal_list& list);
        virtual ~PosixSignal() noexcept;
        virtual void close() noexcept;
        virtual bool is_async() const noexcept;
        virtual bool is_closed() const noexcept;
        virtual bool read(int& t);
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
            static constexpr int max_signals = 128;
            std::vector<int> signals;
            std::sig_atomic_t status = 0;
            PollCondition waiter{[&] { return available(); }};
            static std::sig_atomic_t signal_status[max_signals];
            bool available() const noexcept;
            void enable(int s);
            void reset() noexcept;
            static void signal_handler(int s);
        #endif
    };

}
