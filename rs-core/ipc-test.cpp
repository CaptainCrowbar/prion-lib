#include "rs-core/ipc.hpp"
#include "rs-core/unit-test.hpp"
#include <chrono>
#include <memory>
#include <mutex>

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

constexpr const char* app_name = "com.captaincrowbar.test-core-ipc";

void test_core_ipc_named_mutex() {

    using lock_type = std::unique_lock<NamedMutex>;

    std::unique_ptr<NamedMutex> mutex;

    TRY(mutex = std::make_unique<NamedMutex>(app_name));
    REQUIRE(mutex);

    TRY(mutex->lock());
    TRY(mutex->unlock());
    TEST(mutex->try_lock());
    TRY(mutex->unlock());
    TEST(mutex->try_lock_for(100ms));
    TRY(mutex->unlock());
    TEST(mutex->try_lock_until(system_clock::now() + 100ms));
    TRY(mutex->unlock());
    TEST(mutex->try_lock_until(high_resolution_clock::now() + 100ms));
    TRY(mutex->unlock());

    {
        lock_type lock;
        TRY(lock = lock_type(*mutex));
    }

    {
        auto lock = make_lock(*mutex);
    }

}
