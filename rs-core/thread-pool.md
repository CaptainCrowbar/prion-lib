# Thread Pool #

By Ross Smith

* `#include "rs-core/thread-pool.hpp"`

## Contents ##

* [TOC]

## Class ThreadPool ##

* `class` **`ThreadPool`**`: public Wait`
    * `using ThreadPool::`**`callback`**` = std::function<void()>`
    * `ThreadPool::`**`ThreadPool`**`()`
    * `explicit ThreadPool::`**`ThreadPool`**`(size_t threads)`
    * `ThreadPool::`**`~ThreadPool`**`() noexcept`
    * `void ThreadPool::`**`clear`**`() noexcept`
    * `void ThreadPool::`**`insert`**`(const callback& f)`
    * `void ThreadPool::`**`insert`**`(callback&& f)`
    * `size_t ThreadPool::`**`pending`**`() const noexcept`
    * `size_t ThreadPool::`**`size`**`() const noexcept`

This class runs an internal thread pool, with the number of system threads
specified to the constructor. If no thread count is specified, or the count is
zero, `std::thread::hardware_concurrency()` will be used. The `size()`
function returns the thread count (always positive, and always constant for
the lifetime of the `ThreadPool` object).

The `insert()` function queues a job for execution. Behaviour is undefined if
a callback throws an exception. Jobs are processed by a work stealing
algorithm, and may not be executed in the order in which they were queued. The
`pending()` function returns the number of jobs currently queued or executing.

The `clear()` function discards any queued jobs that have not yet been
started, and waits for all currently executing jobs to finish before
returning. After it returns, `pending()` will be zero; new jobs can be queued
as usual afterwards. The destructor calls `clear()` and waits for it to
finish.

The `poll()` function (from `Wait`) returns true when there are no queued or
executing jobs left; the wait functions will block until this is the case or
the timeout expires.

All member functions, except the constructors and destructor, are async safe
and can be called from any thread. Functions other than `clear()` and the wait
functions can be called from inside an executing job. Although it is legal for
one thread to call `insert()` while another is calling `clear()`, the newly
inserted job will probably be discarded without being executed.
