#pragma once

#include "rs-core/common.hpp"
#include "rs-core/digest.hpp"
#include "rs-core/string.hpp"
#include "rs-core/time.hpp"
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <ctime>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>

#ifdef _XOPEN_SOURCE
    #include <fcntl.h>
    #include <semaphore.h>
#endif

namespace RS {

    class NamedMutex {
    public:
        RS_NO_COPY_MOVE(NamedMutex);
        explicit NamedMutex(const Ustring& name);
        ~NamedMutex() noexcept;
        Ustring name() const;
        void lock();
        bool try_lock();
        template <typename R, typename P> bool try_lock_for(std::chrono::duration<R, P> rel_time);
        template <typename C, typename D> bool try_lock_until(std::chrono::time_point<C, D> abs_time);
        void unlock() noexcept;
    private:
        #ifdef _XOPEN_SOURCE
            Ustring path;
            sem_t* sem;
        #else
            std::wstring path;
            HANDLE han;
        #endif
        bool timed_lock(std::chrono::system_clock::time_point abs_time);
    };

        template <typename R, typename P>
        bool NamedMutex::try_lock_for(std::chrono::duration<R, P> rel_time) {
            using namespace std::chrono;
            if (rel_time <= duration<R, P>())
                return try_lock();
            else
                return timed_lock(system_clock::now() + rel_time);
        }

        template <typename C, typename D>
        bool NamedMutex::try_lock_until(std::chrono::time_point<C, D> abs_time) {
            using namespace std::chrono;
            system_clock::time_point sys_time;
            convert_time_point(abs_time, sys_time);
            return timed_lock(sys_time);
        }

        #ifdef _XOPEN_SOURCE

            inline NamedMutex::NamedMutex(const Ustring& name) {
                path = '/' + hex(digest<Sha256>(name));
                #ifdef __APPLE__
                    path.resize(30);
                #endif
                errno = 0;
                sem = sem_open(path.data(), O_CREAT, 0666, 1);
                int err = errno;
                if (sem == SEM_FAILED)
                    throw std::system_error(err, std::system_category(), path);
            }

            inline NamedMutex::~NamedMutex() noexcept {
                sem_close(sem);
            }

            inline Ustring NamedMutex::name() const {
                return path.substr(1, npos);
            }

            inline void NamedMutex::lock() {
                errno = 0;
                int rc = sem_wait(sem);
                int err = errno;
                if (rc == -1)
                    throw std::system_error(err, std::system_category(), path);
            }

            inline bool NamedMutex::try_lock() {
                errno = 0;
                int rc = sem_wait(sem);
                int err = errno;
                if (rc == 0)
                    return true;
                else if (rc == EAGAIN)
                    return false;
                else
                    throw std::system_error(err, std::system_category(), path);
            }

            inline void NamedMutex::unlock() noexcept {
                sem_post(sem);
            }

            #ifdef __APPLE__

                inline bool NamedMutex::timed_lock(std::chrono::system_clock::time_point abs_time) {
                    using namespace std::chrono;
                    static const auto max_delta = 250ms;
                    auto delta = 1ms;
                    for (;;) {
                        if (try_lock())
                            return true;
                        auto now = system_clock::now();
                        if (now >= abs_time)
                            return false;
                        std::this_thread::sleep_for(delta);
                        delta = std::min({2 * delta, max_delta, duration_cast<milliseconds>(abs_time - now)});
                    }
                }

            #else

                inline bool NamedMutex::timed_lock(std::chrono::system_clock::time_point abs_time) {
                    auto ts = timepoint_to_timespec(abs_time);
                    errno = 0;
                    int rc = sem_timedwait(sem, &ts);
                    int err = errno;
                    if (rc == 0)
                        return true;
                    else if (rc == ETIMEDOUT)
                        return false;
                    else
                        throw std::system_error(err, std::system_category(), path);
                }

            #endif

        #else

            inline NamedMutex::NamedMutex(const Ustring& name) {
                Ustring hash = hex(digest<Sha256>(name));
                path = L"Local\\" + uconv<std::wstring>(hash);
                han = CreateMutexW(nullptr, false, path.data());
                auto err = GetLastError();
                if (! han)
                    throw std::system_error(err, std::system_category(), uconv<Ustring>(path));
            }

            inline NamedMutex::~NamedMutex() noexcept {
                CloseHandle(han);
            }

            inline Ustring NamedMutex::name() const {
                return uconv<Ustring>(path.substr(6, npos));
            }

            inline void NamedMutex::lock() {
                auto rc = WaitForSingleObject(han, INFINITE);
                auto err = GetLastError();
                if (rc == WAIT_FAILED)
                    throw std::system_error(err, std::system_category(), uconv<Ustring>(path));
            }

            inline bool NamedMutex::try_lock() {
                auto rc = WaitForSingleObject(han, 0);
                auto err = GetLastError();
                if (rc == WAIT_FAILED)
                    throw std::system_error(err, std::system_category(), uconv<Ustring>(path));
                return rc != WAIT_TIMEOUT;
            }

            inline void NamedMutex::unlock() noexcept {
                ReleaseMutex(han);
            }

            inline bool NamedMutex::timed_lock(std::chrono::system_clock::time_point abs_time) {
                using namespace std::chrono;
                auto msec = duration_cast<milliseconds>(abs_time - system_clock::now()).count();
                uint32_t msec32 = msec > 0 ? uint32_t(msec) : 0;
                auto rc = WaitForSingleObject(han, msec32);
                auto err = GetLastError();
                if (rc == WAIT_FAILED)
                    throw std::system_error(err, std::system_category(), uconv<Ustring>(path));
                return rc != WAIT_TIMEOUT;
            }

        #endif

}
