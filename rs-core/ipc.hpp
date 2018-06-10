#pragma once

#include "rs-core/common.hpp"
#include "rs-core/time.hpp"
#include <chrono>
#include <string>

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

}
