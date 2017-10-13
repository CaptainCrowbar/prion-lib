#pragma once

#include "rs-core/common.hpp"
#include <atomic>
#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include <system_error>

namespace RS {

    // Thread class

    #ifdef _XOPEN_SOURCE

        class Thread {
        public:
            using callback = std::function<void()>;
            using id_type = pthread_t;
            Thread() = default;
            explicit Thread(callback f) {
                if (f) {
                    impl = std::make_unique<impl_type>();
                    impl->payload = f;
                    impl->state = thread_running;
                    int rc = pthread_create(&impl->thread, nullptr, thread_callback, impl.get());
                    if (rc)
                        throw std::system_error(rc, std::generic_category(), "pthread_create()");
                }
            }
            ~Thread() noexcept { if (impl && impl->state != thread_joined) pthread_join(impl->thread, nullptr); }
            Thread(Thread&&) = default;
            Thread& operator=(Thread&&) = default;
            id_type get_id() const noexcept { return impl ? impl->thread : id_type(); }
            bool poll() noexcept { return ! impl || impl->state != thread_running; }
            void wait() {
                if (! impl || impl->state == thread_joined)
                    return;
                int rc = pthread_join(impl->thread, nullptr);
                impl->state = thread_joined;
                if (rc)
                    throw std::system_error(rc, std::generic_category(), "pthread_join()");
                rethrow(impl->except);
            }
            static size_t cpu_threads() noexcept {
                size_t n = 0;
                #ifdef __APPLE__
                    mach_msg_type_number_t count = HOST_BASIC_INFO_COUNT;
                    host_basic_info_data_t info;
                    if (host_info(mach_host_self(), HOST_BASIC_INFO,
                            reinterpret_cast<host_info_t>(&info), &count) == 0)
                        n = info.avail_cpus;
                #else
                    char buf[256];
                    FILE* f = fopen("/proc/cpuinfo", "r");
                    while (fgets(buf, sizeof(buf), f))
                        if (memcmp(buf, "processor", 9) == 0)
                            ++n;
                    fclose(f);
                #endif
                if (n == 0)
                    n = 1;
                return n;
            }
            static id_type current() noexcept { return pthread_self(); }
            static void yield() noexcept { sched_yield(); }
        private:
            enum state_type {
                thread_running,
                thread_complete,
                thread_joined,
            };
            struct impl_type {
                std::exception_ptr except;
                Thread::callback payload;
                std::atomic<int> state;
                pthread_t thread;
            };
            std::unique_ptr<impl_type> impl;
            Thread(const Thread&) = delete;
            Thread& operator=(const Thread&) = delete;
            static void* thread_callback(void* ptr) noexcept {
                auto t = static_cast<impl_type*>(ptr);
                if (t->payload) {
                    try { t->payload(); }
                    catch (...) { t->except = std::current_exception(); }
                }
                t->state = thread_complete;
                return nullptr;
            }
        };

    #else

        class Thread {
        public:
            using callback = std::function<void()>;
            using id_type = uint32_t;
            Thread() = default;
            explicit Thread(callback f) {
                if (f) {
                    impl = std::make_unique<impl_type>();
                    impl->payload = f;
                    impl->state = thread_running;
                    impl->thread = CreateThread(nullptr, 0, thread_callback, impl.get(), 0, &impl->key);
                    if (! impl->thread)
                        throw std::system_error(GetLastError(), windows_category(), "CreateThread()");
                }
            }
            ~Thread() noexcept {
                if (impl) {
                    if (impl->state != thread_joined)
                        WaitForSingleObject(impl->thread, INFINITE);
                    CloseHandle(impl->thread);
                }
            }
            Thread(Thread&&) = default;
            Thread& operator=(Thread&&) = default;
            id_type get_id() const noexcept { return impl ? impl->key : 0; }
            bool poll() noexcept { return ! impl || impl->state != thread_running; }
            void wait() {
                if (! impl || impl->state == thread_joined)
                    return;
                auto rc = WaitForSingleObject(impl->thread, INFINITE);
                impl->state = thread_joined;
                if (rc == WAIT_FAILED)
                    throw std::system_error(GetLastError(), windows_category(), "WaitForSingleObject()");
                rethrow(impl->except);
            }
            static size_t cpu_threads() noexcept {
                SYSTEM_INFO sysinfo;
                memset(&sysinfo, 0, sizeof(sysinfo));
                GetSystemInfo(&sysinfo);
                return sysinfo.dwNumberOfProcessors;
            }
            static id_type current() noexcept { return GetCurrentThreadId(); }
            static void yield() noexcept { Sleep(0); }
        private:
            enum state_type {
                thread_running,
                thread_complete,
                thread_joined,
            };
            struct impl_type {
                std::exception_ptr except;
                Thread::callback payload;
                std::atomic<int> state;
                DWORD key;
                HANDLE thread;
            };
            std::unique_ptr<impl_type> impl;
            Thread(const Thread&) = delete;
            Thread& operator=(const Thread&) = delete;
            static DWORD WINAPI thread_callback(void* ptr) noexcept {
                auto t = static_cast<impl_type*>(ptr);
                if (t->payload) {
                    try { t->payload(); }
                    catch (...) { t->except = std::current_exception(); }
                }
                t->state = thread_complete;
                return 0;
            }
        };

    #endif

    // Synchronisation objects

    #ifdef _XOPEN_SOURCE

        class Mutex {
        public:
            Mutex() noexcept: pmutex(PTHREAD_MUTEX_INITIALIZER) {}
            ~Mutex() noexcept { pthread_mutex_destroy(&pmutex); }
            void lock() noexcept { pthread_mutex_lock(&pmutex); }
            bool try_lock() noexcept { return pthread_mutex_trylock(&pmutex) == 0; }
            void unlock() noexcept { pthread_mutex_unlock(&pmutex); }
        private:
            friend class ConditionVariable;
            pthread_mutex_t pmutex;
            RS_NO_COPY_MOVE(Mutex)
        };

    #else

        class Mutex {
        public:
            Mutex() noexcept { InitializeCriticalSection(&wcrit); }
            ~Mutex() noexcept { DeleteCriticalSection(&wcrit); }
            void lock() noexcept { EnterCriticalSection(&wcrit); }
            bool try_lock() noexcept { return TryEnterCriticalSection(&wcrit); }
            void unlock() noexcept { LeaveCriticalSection(&wcrit); }
        private:
            friend class ConditionVariable;
            CRITICAL_SECTION wcrit;
            RS_NO_COPY_MOVE(Mutex)
        };

    #endif

    class MutexLock {
    public:
        MutexLock() = default;
        explicit MutexLock(Mutex& m) noexcept: mx(&m) { mx->lock(); }
        ~MutexLock() noexcept { if (mx) mx->unlock(); }
        MutexLock(MutexLock&& lock) noexcept: mx(lock.mx) { lock.mx = nullptr; }
        MutexLock& operator=(MutexLock&& lock) noexcept { if (mx) mx->unlock(); mx = lock.mx; lock.mx = nullptr; return *this; }
    private:
        friend class ConditionVariable;
        Mutex* mx = nullptr;
    };

    inline auto make_lock(Mutex& m) noexcept { return MutexLock(m); }

    #ifndef __MINGW32__
        template <typename T> inline auto make_lock(T& t) { return std::unique_lock<T>(t); }
    #endif

    #ifdef _XOPEN_SOURCE

        // The Posix standard recommends using clock_gettime() with
        // CLOCK_REALTIME to find the current time for a CV wait. Darwin
        // doesn't have clock_gettime(), but the Apple man page for
        // pthread_cond_timedwait() recommends using gettimeofday() instead,
        // and manually converting timeval to timespec.

        class ConditionVariable {
        public:
            ConditionVariable() {
                int rc = pthread_cond_init(&pcond, nullptr);
                if (rc)
                    throw std::system_error(rc, std::generic_category(), "pthread_cond_init()");
            }
            ~ConditionVariable() noexcept { pthread_cond_destroy(&pcond); }
            void notify_all() noexcept { pthread_cond_broadcast(&pcond); }
            void notify_one() noexcept { pthread_cond_signal(&pcond); }
            void wait(MutexLock& ml) {
                int rc = pthread_cond_wait(&pcond, &ml.mx->pmutex);
                if (rc)
                    throw std::system_error(rc, std::generic_category(), "pthread_cond_wait()");
            }
            template <typename Pred> void wait(MutexLock& lock, Pred p) { while (! p()) wait(lock); }
            template <typename R, typename P, typename Pred> bool wait_for(MutexLock& lock, std::chrono::duration<R, P> t, Pred p) {
                using namespace std::chrono;
                return wait_impl(lock, duration_cast<nanoseconds>(t), predicate(p));
            }
            template <typename C, typename D, typename Pred> bool wait_until(MutexLock& lock, std::chrono::time_point<C, D> t, Pred p) {
                using namespace std::chrono;
                nanoseconds ns;
                auto now = C::now();
                if (now < t)
                    ns = duration_cast<nanoseconds>(t - now);
                return wait_impl(lock, ns, predicate(p));
            }
        private:
            using predicate = std::function<bool()>;
            pthread_cond_t pcond;
            RS_NO_COPY_MOVE(ConditionVariable)
            bool wait_impl(MutexLock& ml, std::chrono::nanoseconds ns, predicate p) {
                using namespace RS::Literals;
                using namespace std::chrono;
                static constexpr unsigned long G = 1'000'000'000ul;
                if (p())
                    return true;
                auto nsc = ns.count();
                if (nsc <= 0)
                    return false;
                timespec ts;
                #ifdef __APPLE__
                    timeval tv;
                    gettimeofday(&tv, nullptr);
                    ts = {tv.tv_sec, 1000 * tv.tv_usec};
                #else
                    clock_gettime(CLOCK_REALTIME, &ts);
                #endif
                ts.tv_nsec += nsc % G;
                ts.tv_sec += nsc / G + ts.tv_nsec / G;
                ts.tv_nsec %= G;
                for (;;) {
                    int rc = pthread_cond_timedwait(&pcond, &ml.mx->pmutex, &ts);
                    if (rc == ETIMEDOUT)
                        return p();
                    if (rc != 0)
                        throw std::system_error(rc, std::generic_category(), "pthread_cond_timedwait()");
                    if (p())
                        return true;
                }
            }
        };

    #else

        // Windows only has millisecond resolution in their API. The actual
        // resolution varies from 1ms to 15ms depending on the version of
        // Windows, what APIs have been called lately, and the phase of the
        // moon.

        class ConditionVariable {
        public:
            ConditionVariable() { InitializeConditionVariable(&wcond); }
            ~ConditionVariable() noexcept {}
            void notify_all() noexcept { WakeAllConditionVariable(&wcond); }
            void notify_one() noexcept { WakeConditionVariable(&wcond); }
            void wait(MutexLock& lock) {
                if (! SleepConditionVariableCS(&wcond, &lock.mx->wcrit, INFINITE))
                    throw std::system_error(GetLastError(), windows_category(), "SleepConditionVariableCS()");
            }
            template <typename Pred> void wait(MutexLock& lock, Pred p) { while (! p()) wait(lock); }
            template <typename R, typename P, typename Pred> bool wait_for(MutexLock& lock, std::chrono::duration<R, P> t, Pred p) {
                using namespace std::chrono;
                return wait_impl(lock, duration_cast<nanoseconds>(t), predicate(p));
            }
            template <typename C, typename D, typename Pred> bool wait_until(MutexLock& lock, std::chrono::time_point<C, D> t, Pred p) {
                using namespace std::chrono;
                nanoseconds ns;
                auto now = C::now();
                if (now < t)
                    ns = duration_cast<nanoseconds>(t - now);
                return wait_impl(lock, ns, predicate(p));
            }
        private:
            using predicate = std::function<bool()>;
            CONDITION_VARIABLE wcond;
            RS_NO_COPY_MOVE(ConditionVariable)
            bool wait_impl(MutexLock& lock, std::chrono::nanoseconds ns, predicate p) {
                using namespace std::chrono;
                if (p())
                    return true;
                if (ns <= nanoseconds())
                    return false;
                auto timeout = system_clock::now() + ns;
                for (;;) {
                    auto remain = timeout - system_clock::now();
                    auto ms = duration_cast<milliseconds>(remain).count();
                    if (ms < 0)
                        ms = 0;
                    else if (ms >= INFINITE)
                        ms = INFINITE - 1;
                    if (SleepConditionVariableCS(&wcond, &lock.mx->wcrit, uint32_t(ms))) {
                        if (p())
                            return true;
                    } else {
                        auto status = GetLastError();
                        if (status == ERROR_TIMEOUT)
                            return p();
                        else
                            throw std::system_error(status, windows_category(), "SleepConditionVariableCS()");
                    }
                }
            }
        };

    #endif

}
