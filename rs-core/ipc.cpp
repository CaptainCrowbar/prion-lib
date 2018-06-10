#include "rs-core/ipc.hpp"
#include "rs-core/digest.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cerrno>
#include <ctime>
#include <system_error>
#include <thread>

using namespace std::chrono;

namespace RS {

    // Class NamedMutex

    #ifdef _XOPEN_SOURCE

        NamedMutex::NamedMutex(const Ustring& name) {
            auto hash = Sha256()(name);
            path = '/' + hex(hash);
            #ifdef __APPLE__
                path.resize(30);
            #endif
            errno = 0;
            sem = sem_open(path.data(), O_CREAT, 0666, 1);
            int err = errno;
            if (sem == SEM_FAILED)
                throw std::system_error(err, std::system_category(), path);
        }

        NamedMutex::~NamedMutex() noexcept {
            sem_close(sem);
        }

        Ustring NamedMutex::name() const {
            return path.substr(1, npos);
        }

        void NamedMutex::lock() {
            errno = 0;
            int rc = sem_wait(sem);
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::system_category(), path);
        }

        bool NamedMutex::try_lock() {
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

        void NamedMutex::unlock() noexcept {
            sem_post(sem);
        }

        #ifdef __APPLE__

            bool NamedMutex::timed_lock(system_clock::time_point abs_time) {
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

            bool NamedMutex::timed_lock(system_clock::time_point abs_time) {
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

        NamedMutex::NamedMutex(const Ustring& name) {
            auto hash = Sha256()(name);
            Ustring xhash = hex(hash);
            path = L"Local\\" + uconv<std::wstring>(xhash);
            han = CreateMutexW(nullptr, false, path.data());
            auto err = GetLastError();
            if (! han)
                throw std::system_error(err, std::system_category(), uconv<Ustring>(path));
        }

        NamedMutex::~NamedMutex() noexcept {
            CloseHandle(han);
        }

        Ustring NamedMutex::name() const {
            return uconv<Ustring>(path.substr(6, npos));
        }

        void NamedMutex::lock() {
            auto rc = WaitForSingleObject(han, INFINITE);
            auto err = GetLastError();
            if (rc == WAIT_FAILED)
                throw std::system_error(err, std::system_category(), uconv<Ustring>(path));
        }

        bool NamedMutex::try_lock() {
            auto rc = WaitForSingleObject(han, 0);
            auto err = GetLastError();
            if (rc == WAIT_FAILED)
                throw std::system_error(err, std::system_category(), uconv<Ustring>(path));
            return rc != WAIT_TIMEOUT;
        }

        void NamedMutex::unlock() noexcept {
            ReleaseMutex(han);
        }

        bool NamedMutex::timed_lock(system_clock::time_point abs_time) {
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
