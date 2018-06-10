#include "rs-core/signal.hpp"
#include <algorithm>
#include <chrono>
#include <map>
#include <thread>

using namespace std::chrono;

namespace RS {

    // Class PosixSignal

    #ifdef _XOPEN_SOURCE

        PosixSignal::PosixSignal(const signal_list& list):
        MessageChannel<int>(), signals(list), newmask(), oldmask(), queue(), open(true) {
            signals.push_back(SIGUSR1);
            sigemptyset(&newmask);
            sigemptyset(&oldmask);
            for (auto s: signals)
                sigaddset(&newmask, s);
            pthread_sigmask(SIG_BLOCK, &newmask, &oldmask);
        }

        PosixSignal::~PosixSignal() noexcept {
            sigset_t pending;
            sigemptyset(&pending);
            sigpending(&pending);
            int s = 0;
            for (auto t: signals)
                if (sigismember(&pending, t))
                    sigwait(&pending, &s);
            pthread_sigmask(SIG_SETMASK, &oldmask, nullptr);
        }

        void PosixSignal::close() noexcept {
            if (open) {
                open = false;
                raise(SIGUSR1);
            }
        }

        bool PosixSignal::read(int& t) {
            if (! open || queue.empty())
                return false;
            t = queue.front();
            queue.pop_front();
            return true;
        }

        #if defined(__APPLE__) || defined(__CYGWIN__)

            bool PosixSignal::do_wait_for(duration t) {
                static const duration delta = milliseconds(10);
                sigset_t pending;
                for (;;) {
                    if (! open || ! queue.empty())
                        return true;
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
                        return true;
                    } else if (t <= duration()) {
                        return false;
                    } else if (t < delta) {
                        std::this_thread::sleep_for(t);
                        t = duration();
                    } else {
                        std::this_thread::sleep_for(delta);
                        t -= delta;
                    }
                }
            }

        #else

            bool PosixSignal::do_wait_for(duration t) {
                if (! open || ! queue.empty())
                    return true;
                t = std::max(t, duration());
                auto ts = duration_to_timespec(t);
                int s = sigtimedwait(&newmask, nullptr, &ts);
                if (! open)
                    return true;
                else if (s == -1)
                    return false;
                queue.push_back(s);
                return true;
            }

        #endif

    #else

        PosixSignal::PosixSignal(const signal_list& /*list*/):
        MessageChannel<int>(), mutex(), cv(), open(true) {}

        PosixSignal::~PosixSignal() noexcept {}

        void PosixSignal::close() noexcept {
            auto lock = make_lock(mutex);
            open = false;
            cv.notify_all();
        }

        bool PosixSignal::read(int& /*t*/) {
            return false;
        }

        bool PosixSignal::do_wait_for(duration t) {
            auto lock = make_lock(mutex);
            if (t > duration())
                cv.wait_for(lock, t, [&] { return ! open; });
            return ! open;
        }

    #endif

    #define SIGNAL_NAME(s) {s, #s},

    Ustring PosixSignal::name(int s) {
        static const std::map<int, Ustring> map = {
            SIGNAL_NAME(SIGABRT)
            SIGNAL_NAME(SIGFPE)
            SIGNAL_NAME(SIGILL)
            SIGNAL_NAME(SIGINT)
            SIGNAL_NAME(SIGSEGV)
            SIGNAL_NAME(SIGTERM)
            #ifdef _XOPEN_SOURCE
                SIGNAL_NAME(SIGALRM)
                SIGNAL_NAME(SIGBUS)
                SIGNAL_NAME(SIGCHLD)
                SIGNAL_NAME(SIGCONT)
                SIGNAL_NAME(SIGHUP)
                SIGNAL_NAME(SIGKILL)
                SIGNAL_NAME(SIGPIPE)
                SIGNAL_NAME(SIGPROF)
                SIGNAL_NAME(SIGQUIT)
                SIGNAL_NAME(SIGSTOP)
                SIGNAL_NAME(SIGSYS)
                SIGNAL_NAME(SIGTRAP)
                SIGNAL_NAME(SIGTSTP)
                SIGNAL_NAME(SIGTTIN)
                SIGNAL_NAME(SIGTTOU)
                SIGNAL_NAME(SIGURG)
                SIGNAL_NAME(SIGUSR1)
                SIGNAL_NAME(SIGUSR2)
                SIGNAL_NAME(SIGVTALRM)
                SIGNAL_NAME(SIGXCPU)
                SIGNAL_NAME(SIGXFSZ)
                #ifdef SIGEMT
                    SIGNAL_NAME(SIGEMT)
                #endif
                #ifdef SIGINFO
                    SIGNAL_NAME(SIGINFO)
                #endif
                #ifdef SIGIO
                    SIGNAL_NAME(SIGIO)
                #endif
                #ifdef SIGLOST
                    SIGNAL_NAME(SIGLOST)
                #endif
                #ifdef SIGPOLL
                    SIGNAL_NAME(SIGPOLL)
                #endif
                #ifdef SIGPWR
                    SIGNAL_NAME(SIGPWR)
                #endif
                #ifdef SIGRTMIN
                    SIGNAL_NAME(SIGRTMIN)
                    SIGNAL_NAME(SIGRTMAX)
                #endif
                #ifdef SIGSTKFLT
                    SIGNAL_NAME(SIGSTKFLT)
                #endif
                #ifdef SIGTHR
                    SIGNAL_NAME(SIGTHR)
                #endif
                #ifdef SIGWINCH
                    SIGNAL_NAME(SIGWINCH)
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

}
