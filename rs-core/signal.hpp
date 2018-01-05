#pragma once

#include "rs-core/string.hpp"
#include "rs-core/channel.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <csignal>
#include <deque>
#include <initializer_list>
#include <map>
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
        virtual bool read(int& t);
        virtual bool is_async() const noexcept { return false; }
        static Ustring name(int s);
    protected:
        virtual state do_wait_for(time_unit t);
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

    #ifdef _XOPEN_SOURCE

        inline PosixSignal::PosixSignal(const signal_list& list):
        MessageChannel<int>(), signals(list), newmask(), oldmask(), queue(), open(true) {
            signals.push_back(SIGUSR1);
            sigemptyset(&newmask);
            sigemptyset(&oldmask);
            for (auto s: signals)
                sigaddset(&newmask, s);
            pthread_sigmask(SIG_BLOCK, &newmask, &oldmask);
        }

        inline PosixSignal::~PosixSignal() noexcept {
            sigset_t pending;
            sigemptyset(&pending);
            sigpending(&pending);
            int s = 0;
            for (auto t: signals)
                if (sigismember(&pending, t))
                    sigwait(&pending, &s);
            pthread_sigmask(SIG_SETMASK, &oldmask, nullptr);
        }

        inline void PosixSignal::close() noexcept {
            if (open) {
                open = false;
                raise(SIGUSR1);
            }
        }

        inline bool PosixSignal::read(int& t) {
            if (! open || queue.empty())
                return false;
            t = queue.front();
            queue.pop_front();
            return true;
        }

        #if defined(__CYGWIN__) || defined(__APPLE__)

            inline Channel::state PosixSignal::do_wait_for(time_unit t) {
                using namespace std::chrono;
                static const time_unit delta = milliseconds(10);
                sigset_t pending;
                for (;;) {
                    if (! open)
                        return state::closed;
                    else if (! queue.empty())
                        return state::ready;
                    sigemptyset(&pending);
                    sigpending(&pending);
                    auto i = std::find_if(signals.begin(), signals.end(),
                        [&] (int s) { return sigismember(&pending, s); });
                    if (i != signals.end()) {
                        sigset_t mask;
                        sigemptyset(&mask);
                        sigaddset(&mask, *i);
                        int s = 0;
                        sigwait(&mask, &s);
                        queue.push_back(s);
                        return state::ready;
                    }
                    if (t <= time_unit()) {
                        return state::waiting;
                    } else if (t < delta) {
                        std::this_thread::sleep_for(t);
                        t = time_unit();
                    } else {
                        std::this_thread::sleep_for(delta);
                        t -= delta;
                    }
                }
            }

        #else

            inline Channel::state PosixSignal::do_wait_for(time_unit t) {
                if (! open)
                    return state::closed;
                else if (! queue.empty())
                    return state::ready;
                t = std::max(t, time_unit());
                auto ts = duration_to_timespec(t);
                int s = sigtimedwait(&newmask, nullptr, &ts);
                if (! open)
                    return state::closed;
                else if (s == -1)
                    return state::waiting;
                queue.push_back(s);
                return state::ready;
            }

        #endif

    #else

        inline PosixSignal::PosixSignal(const signal_list& /*list*/):
        MessageChannel<int>(), mutex(), cv(), open(true) {}

        inline PosixSignal::~PosixSignal() noexcept {}

        inline void PosixSignal::close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
            cv.notify_all();
        }

        inline bool PosixSignal::read(int& /*t*/) {
            return false;
        }

        inline Channel::state PosixSignal::do_wait_for(time_unit t) {
            auto lock = make_lock(mutex);
            if (t > time_unit())
                cv.wait_for(lock, t, [&] { return ! open; });
            return open ? state::waiting : state::closed;
        }

    #endif

    #define RS_IMPL_SIGNAL_NAME(s) {s, #s},

    inline Ustring PosixSignal::name(int s) {
        static const std::map<int, Ustring> map = {
            RS_IMPL_SIGNAL_NAME(SIGABRT)
            RS_IMPL_SIGNAL_NAME(SIGFPE)
            RS_IMPL_SIGNAL_NAME(SIGILL)
            RS_IMPL_SIGNAL_NAME(SIGINT)
            RS_IMPL_SIGNAL_NAME(SIGSEGV)
            RS_IMPL_SIGNAL_NAME(SIGTERM)
            #ifdef _XOPEN_SOURCE
                RS_IMPL_SIGNAL_NAME(SIGALRM)
                RS_IMPL_SIGNAL_NAME(SIGBUS)
                RS_IMPL_SIGNAL_NAME(SIGCHLD)
                RS_IMPL_SIGNAL_NAME(SIGCONT)
                RS_IMPL_SIGNAL_NAME(SIGHUP)
                RS_IMPL_SIGNAL_NAME(SIGKILL)
                RS_IMPL_SIGNAL_NAME(SIGPIPE)
                RS_IMPL_SIGNAL_NAME(SIGPROF)
                RS_IMPL_SIGNAL_NAME(SIGQUIT)
                RS_IMPL_SIGNAL_NAME(SIGSTOP)
                RS_IMPL_SIGNAL_NAME(SIGSYS)
                RS_IMPL_SIGNAL_NAME(SIGTRAP)
                RS_IMPL_SIGNAL_NAME(SIGTSTP)
                RS_IMPL_SIGNAL_NAME(SIGTTIN)
                RS_IMPL_SIGNAL_NAME(SIGTTOU)
                RS_IMPL_SIGNAL_NAME(SIGURG)
                RS_IMPL_SIGNAL_NAME(SIGUSR1)
                RS_IMPL_SIGNAL_NAME(SIGUSR2)
                RS_IMPL_SIGNAL_NAME(SIGVTALRM)
                RS_IMPL_SIGNAL_NAME(SIGXCPU)
                RS_IMPL_SIGNAL_NAME(SIGXFSZ)
                #ifdef SIGEMT
                    RS_IMPL_SIGNAL_NAME(SIGEMT)
                #endif
                #ifdef SIGINFO
                    RS_IMPL_SIGNAL_NAME(SIGINFO)
                #endif
                #ifdef SIGIO
                    RS_IMPL_SIGNAL_NAME(SIGIO)
                #endif
                #ifdef SIGLOST
                    RS_IMPL_SIGNAL_NAME(SIGLOST)
                #endif
                #ifdef SIGPOLL
                    RS_IMPL_SIGNAL_NAME(SIGPOLL)
                #endif
                #ifdef SIGPWR
                    RS_IMPL_SIGNAL_NAME(SIGPWR)
                #endif
                #ifdef SIGRTMIN
                    RS_IMPL_SIGNAL_NAME(SIGRTMIN)
                    RS_IMPL_SIGNAL_NAME(SIGRTMAX)
                #endif
                #ifdef SIGSTKFLT
                    RS_IMPL_SIGNAL_NAME(SIGSTKFLT)
                #endif
                #ifdef SIGTHR
                    RS_IMPL_SIGNAL_NAME(SIGTHR)
                #endif
                #ifdef SIGWINCH
                    RS_IMPL_SIGNAL_NAME(SIGWINCH)
                #endif
            #endif
        };
        auto it = map.find(s);
        if (it != map.end())
            return it->second;
        #ifdef SIGRTMIN
            if (s > SIGRTMIN && s < SIGRTMAX)
                return "SIGRTMIN+" + dec(s - SIGRTMIN);
        #endif
        return dec(s);
    }

    #undef RS_IMPL_SIGNAL_NAME

}
