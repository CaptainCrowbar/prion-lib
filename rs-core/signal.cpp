#include "rs-core/signal.hpp"
#include <algorithm>
#include <chrono>
#include <map>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>

using namespace std::chrono;

namespace RS {

    // Class PosixSignal

    #ifdef _XOPEN_SOURCE

        PosixSignal::PosixSignal(std::initializer_list<int> list):
        PosixSignal(signal_list{list}) {}

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

        bool PosixSignal::is_async() const noexcept {
            return false;
        }

        bool PosixSignal::is_closed() const noexcept {
            return ! open;
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

        std::sig_atomic_t PosixSignal::signal_status[max_signals];

        PosixSignal::PosixSignal(std::initializer_list<int> list) {
            for (int s: list)
                enable(s);
        }

        PosixSignal::PosixSignal(const signal_list& list) {
            for (int s: list)
                enable(s);
        }

        PosixSignal::~PosixSignal() noexcept {
            reset();
        }

        void PosixSignal::close() noexcept {
            status = 0;
        }

        bool PosixSignal::is_async() const noexcept {
            return false;
        }

        bool PosixSignal::is_closed() const noexcept {
            return status == 0;
        }

        bool PosixSignal::read(int& t) {
            if (status == 0)
                return false;
            for (int s: signals) {
                int count = signal_status[s];
                if (count > 0) {
                    signal_status[s] = count - 1;
                    t = s;
                    return true;
                }
            }
            return false;
        }

        bool PosixSignal::do_wait_for(duration t) {
            return waiter.wait_for(t);
        }

        bool PosixSignal::available() const noexcept {
            if (status == 0)
                return true;
            for (int s: signals)
                if (signal_status[s] > 0)
                    return true;
            return false;
        }

        void PosixSignal::enable(int s) {
            if (s == 0 || s >= max_signals)
                throw std::invalid_argument("Invalid signal: " + std::to_string(s));
            signal_status[s] = 0;
            auto rc = std::signal(s, signal_handler);
            int error = errno;
            if (rc == SIG_ERR)
                throw std::system_error(error, std::generic_category());
            signals.push_back(s);
            status = 1;
        }

        void PosixSignal::reset() noexcept {
            for (int s: signals)
                std::signal(s, SIG_DFL);
            signals.clear();
            status = 0;
        }

        void PosixSignal::signal_handler(int s) {
            ++signal_status[s];
            std::signal(s, signal_handler);
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
