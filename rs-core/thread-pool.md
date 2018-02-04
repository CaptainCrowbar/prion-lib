# Thread Pool #

By Ross Smith

* `#include "rs-core/thread-pool.hpp"`

## Contents ##

* [TOC]

## Class ThreadPool ##

* `class` **`ThreadPool`**
    * `ThreadPool::`**`ThreadPool`**`()`
    * `explicit ThreadPool::`**`ThreadPool`**`(size_t threads)`
    * `ThreadPool::`**`~ThreadPool`**`() noexcept`
    * `size_t ThreadPool::`**`size`**`() const noexcept`
    * `template <typename F> void ThreadPool::`**`insert`**`(F&& f)`
    * `size_t ThreadPool::`**`pending`**`() const noexcept`
    * `void ThreadPool::`**`clear`**`() noexcept`
    * `void ThreadPool::`**`wait`**`()`
    * `template <typename R, typename P> bool ThreadPool::`**`wait_for`**`(std::chrono::duration<R, P> timeout)`
    * `template <typename C, typename D> bool ThreadPool::`**`wait_until`**`(std::chrono::time_point<C, D> timeout)`

This class runs an internal thread pool, with the number of system threads
specified to the constructor. If no thread count is specified, or the count is
zero, `std::thread::hardware_concurrency()` will be used. The `size()`
function returns the thread count (always positive, and always constant for
the lifetime of the `ThreadPool` object).

The `insert()` function queues a job for execution. The argument is expected
to be a function object that can be called with no arguments. Any return value
or exception will be silently discarded when the job is called. Jobs are
handled by a work stealing algorithm, and may not be executed in the order in
which they were queued. The `pending()` function returns the number of jobs
currently queued or executing.

The `clear()` function discards any queued jobs that have not yet been
started, and waits for all currently executing jobs to finish before
returning. After it returns, `pending()` will be zero; new jobs can be queued
as usual afterwards. The destructor calls `clear()`.

The wait functions will block until there are no queued or executing jobs
left, or the timeout expires. The timed wait functions return true on success
(no jobs left), false on timeout.

All member functions, except the constructors and destructor, are async safe
and can be called from any thread. Functions other than `clear()` and
`wait*()` can be called from inside an executing job. Although it is legal for
one thread to call `insert()` while another is calling `clear()`, the newly
inserted job will probably be discarded without being executed.
