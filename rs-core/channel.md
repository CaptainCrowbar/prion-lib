# Message Dispatch #

By Ross Smith

* `#include "rs-core/channel.hpp"`

## Contents ##

[TOC]

## Class hierarchy ##

* `[abstract] class` **`Channel`**
    * `[abstract] class` **`EventChannel`**`: public Channel`
        * `class` **`TrueChannel`**`: public EventChannel`
        * `class` **`FalseChannel`**`: public EventChannel`
        * `class` **`TimerChannel`**`: public EventChannel`
        * `class` **`ThrottleChannel`**`: public EventChannel`
    * `[abstract] template <typename T> class` **`MessageChannel`**`: public Channel`
        * `template <typename T> class` **`GeneratorChannel`**`: public MessageChannel<T>`
        * `template <typename T> class` **`QueueChannel`**`: public MessageChannel<T>`
        * `template <typename T> class` **`ValueChannel`**`: public MessageChannel<T>`
    * `[abstract] class` **`StreamChannel`**`: public Channel`
        * `class` **`BufferChannel`**`: public StreamChannel`
* `class` **`Dispatch`**

## Channel base class ##

* `class` **`Channel`**`: public Wait`
    * `virtual Channel::`**`~Channel`**`() noexcept`
    * `Channel::`**`Channel`**`(Channel&& c) noexcept`
    * `Channel& Channel::`**`operator=`**`(Channel&& c) noexcept`
    * `virtual void Channel::`**`close`**`() noexcept = 0`
    * `virtual bool Channel::`**`is_closed`**`() const noexcept = 0`
    * `virtual bool Channel::`**`is_async`**`() const noexcept` _= true_

The base class for all readable message channels. All concrete channel classes
must derive from one of the three intermediate classes below, not directly
from Channel. Functions for handling global message dispatch are static
methods of this class.

Derived classes need to implement at least `close()`, `is_closed()`, and one
of the `do_wait_*()` functions from [`Wait`](time.html); `close()` must be
async safe and idempotent.

Any of the wait functions will return true if the channel is either ready for
reading or closed.

If `is_async()` is false, the channel can only be used in a synchronous
dispatch handler, usually because it calls an underlying native API that is
only intended to be used from the main thread.

If `is_shared()` (from `Wait`) is true, it is safe to call the wait functions
(and `read()` where relevant) from multiple threads at the same time, provided
it is acceptable for each message to be delivered to an unpredictable choice
of thread.

The channel base classes implement move operations in case a derived class
needs them, but derived channel types are usually not movable. Channel classes
are never copyable.

## Intermediate base classes ##

### Class EventChannel ###

* `class` **`EventChannel`**`: public Channel`
    * `using EventChannel::`**`callback`** `= std::function<void()>`
    * `protected EventChannel::`**`EventChannel`**`()`

Intermediate base class for event channels (channels that carry no information
beyond the fact that an event has happened).

### Class MessageChannel ###

* `template <typename T> class` **`MessageChannel`**`: public Channel`
    * `using MessageChannel::`**`callback`** `= std::function<void(const T&)>`
    * `using MessageChannel::`**`value_type`** `= T`
    * `virtual bool MessageChannel::`**`read`**`(T& t) = 0`
    * `Optional<T> MessageChannel::`**`read_opt`**`()`
    * `protected MessageChannel::`**`MessageChannel`**`()`

Intermediate base class for message channels (channels that send discrete
objects). When a message channel is added to a dispatch set, the callback
function will be applied to each incoming message object. The message type `T`
must be at least movable or copyable; if `read_opt()` is called it must also
be default constructible. Derived classes may impose additional restrictions.

A concrete message channel class must implement `read()` as well as the usual
`Channel` members. The `read()` function should return true on a successful
read, false if the channel is closed. It will normally be called only after a
successful wait; if no data is immediately available, `read()` may block or
return false. The state of the argument to `read()` if it returns false is
unspecified.

The `read_opt()` function calls `read()` and returns the message on success,
or an empty value on failure.

### Class StreamChannel ###

* `class` **`StreamChannel`**`: public Channel`
    * `using StreamChannel::`**`callback`** `= std::function<void(std::string&)>`
    * `static constexpr size_t StreamChannel::`**`default_buffer`** `= 16384`
    * `virtual size_t StreamChannel::`**`read`**`(void* dst, size_t maxlen) = 0`
    * `size_t StreamChannel::`**`buffer`**`() const noexcept`
    * `std::string StreamChannel::`**`read_all`**`()`
    * `std::string StreamChannel::`**`read_str`**`()`
    * `size_t StreamChannel::`**`read_to`**`(std::string& dst)`
    * `void StreamChannel::`**`set_buffer`**`(size_t n) noexcept`
    * `protected StreamChannel::`**`StreamChannel`**`()`

Intermediate base class for byte stream channels. When a stream channel is
added to a dispatch set, the callback function will be invoked whenever more
data arrives. The string argument to the callback is a buffer internal to the
dispatch manager; it starts out empty, with incoming data appended to it
before each callback. Data will never be removed from the buffer by the
dispatch manager; the callback function is responsible for removing any data
it has consumed.

Stream channels must implement `read()` as well as the usual `Channel`
members. The `read()` function should return the number of bytes read on
success, or zero if the channel is closed; if no data is available it may
block or return zero. If `dst` is null or `maxlen` is zero, `read()` should
return zero immediately.

The `read_all()`, `read_str()`, and `read_to()` functions are convenience
wrappers around `read()`. The `read_all()` function will read until the
channel is closed, blocking as necessary; `read_str()` calls `read()` once and
returns the new data as a string; `read_to()` calls `read()` and appends the
data to the string. The buffer size functions control the internal read limit
for `read_all/str/to()`.

## Concrete channel classes ##

Member functions inherited from the channel base classes are not documented
again here unless they have significantly different semantics.

### Trivial channels ###

* `class` **`TrueChannel`**`: public EventChannel`
    * `virtual bool TrueChannel::`**`is_shared`**`() const noexcept` _= true_
* `class` **`FalseChannel`**`: public EventChannel`
    * `virtual bool FalsChannel::`**`is_shared`**`() const noexcept` _= true_

Trivial event channels whose wait functions always succeed immediately
(`TrueChannel`) or always time out (`FalseChannel`).

### Class TimerChannel ###

* `class` **`TimerChannel`**`: public EventChannel`
    * `template <typename R, typename P> explicit TimerChannel::`**`TimerChannel`**`(std::chrono::duration<R, P> t) noexcept`
    * `virtual bool TimerChannel::`**`is_shared`**`() const noexcept` _= true_
    * `void TimerChannel::`**`flush`**`() noexcept`
    * `duration TimerChannel::`**`interval`**`() const noexcept`
    * `duration TimerChannel::`**`next`**`() const noexcept`

An event channel that delivers one tick every interval, starting at one
interval after the time of construction. Multiple ticks may be delivered at
once (represented by repeated calls to wait functions returning success
immediately) if multiple intervals have elapsed since the last check.

The `next()` function returns the time of the next tick (this may be in the
past if multiple ticks are pending); `flush()` discards any pending ticks.
These are async safe and can be called from any thread.

### Class ThrottleChannel ###

* `class` **`ThrottleChannel`**`: public EventChannel`
    * `template <typename R, typename P> explicit ThrottleChannel::`**`ThrottleChannel`**`(std::chrono::duration<R, P> t) noexcept`
    * `virtual bool ThrottleChannel::`**`is_shared`**`() const noexcept` _= true_
    * `duration ThrottleChannel::`**`interval`**`() const noexcept`

An event channel that throttles events to a maximum rate. The channel will
block if the minimum interval has not yet elapsed since the last event;
otherwise, or if this is the first event, it will succeed immediately. Unlike
a `TimerChannel`, this will not queue pending events; no extra events are
created if there is a long interval between calls. If the interval is zero or
negative, this is equivalent to `TrueChannel`.

### Class GeneratorChannel ###

* `template <typename T> class` **`GeneratorChannel`**`: public MessageChannel<T>`
    * `using GeneratorChannel::`**`generator`** `= std::function<T()>`
    * `template <typename F> explicit GeneratorChannel::`**`GeneratorChannel`**`(F f)`

A channel that calls a function to generate incoming messages. A default
constructed channel is always closed. Waiting on an open channel always
succeeds immediately, apart from any delay involved in the callback function
itself.

### Class QueueChannel ###

* `template <typename T> class` **`QueueChannel`**`: public MessageChannel<T>`
    * `QueueChannel::`**`QueueChannel`**`()`
    * `virtual bool QueueChannel::`**`is_shared`**`() const noexcept` _= true_
    * `void QueueChannel::`**`clear`**`() noexcept`
    * `bool QueueChannel::`**`write`**`(const T& t)`
    * `bool QueueChannel::`**`write`**`(T&& t)`

A last in, first out message queue.

### Class ValueChannel ###

* `template <typename T> class` **`ValueChannel`**`: public MessageChannel<T>`
    * `ValueChannel::`**`ValueChannel`**`()`
    * `explicit ValueChannel::`**`ValueChannel`**`(const T& t)`
    * `virtual bool ValueChannel::`**`is_shared`**`() const noexcept` _= true_
    * `void ValueChannel::`**`clear`**`() noexcept`
    * `bool ValueChannel::`**`write`**`(const T& t)`
    * `bool ValueChannel::`**`write`**`(T&& t)`

A single value buffer channel. Any value written into the channel overwrites
any other value that was written earlier but has not yet been read. The
channel only reports a new message when the value is changed; writing the same
value again will succeed but will not trigger a read. `T` must be equality
comparable.

### Class BufferChannel ###

* `class` **`BufferChannel`**`: public StreamChannel`
    * `BufferChannel::`**`BufferChannel`**`()`
    * `void BufferChannel::`**`clear`**`() noexcept`
    * `bool BufferChannel::`**`write`**`(std::string_view src)`
    * `bool BufferChannel::`**`write`**`(const void* src, size_t len)`

A byte stream buffer channel. Message boundaries are not preserved; calling
`read()` will extract all pending data up to the length limit, regardless of
whether it was written as a single block or multiple smaller blocks.

## Dispatch controller class ##

* `class` **`Dispatch`**
    * `enum class Dispatch::`**`mode`**
        * `Dispatch::mode::`**`async`**
        * `Dispatch::mode::`**`sync`**
    * `enum class Dispatch::`**`reason`**
        * `Dispatch::reason::`**`closed`**
        * `Dispatch::reason::`**`empty`**
        * `Dispatch::reason::`**`error`**
    * `struct Dispatch::`**`result`**
        * `Channel* result::`**`channel`**
        * `std::exception_ptr result::`**`error`**
        * `Dispatch::reason result::`**`why`**
    * `template <typename F> static void Dispatch::`**`add`**`(EventChannel& c, mode m, F f)`
    * `template <typename T, typename F> static void Dispatch::`**`add`**`(MessageChannel<T>& c, mode m, F f)`
    * `template <typename F> static void Dispatch::`**`add`**`(StreamChannel& c, mode m, F f)`
    * `static void Dispatch::`**`close`**`() noexcept`
    * `static bool Dispatch::`**`empty`**`() noexcept`
    * `static result Dispatch::`**`run`**`() noexcept`

The `Dispatch` class is not instantiable; all of its member functions are
static.

The `add()` functions take a channel, a dispatch mode flag, and a callback
function. The three intermediate channel base classes each define a `callback`
member type that indicates what signature the callback is expected to have
(the actual callback argument to `add()` does not have to be this exact type,
as long as it can be used to construct a function object of the appropriate
type). Add channels and their associated callback functions to the queue using
`add()`, then call `run()` to start the dispatch loop. Behaviour is undefined
if `run()` is called from multiple threads at the same time; normally it
should only be called from the main thread.

Dispatch functions must only be called synchronously (i.e. not from inside an
asynchronous callback function). Channels can safely be destroyed while in a
dispatch set (the dispatch queue will notice this and silently drop the
channel), provided this is done synchronously.

The `add()` functions start a synchronous or asynchronous task reading from
the channel. Asynchronous handlers are started immediately in a separate
thread; synchronous handlers will not be executed until `run()` is called. An
`add()` function will throw `std::bad_function_call` if the callback function
is null, or `std::invalid_argument` if any of these conditions is true:

* The mode flag is not one of the `Dispatch::mode` enumeration values.
* The channel is synchronous (`Channel::is_async()` is false), but the mode is `Dispatch::mode::async`.
* The same channel is added more than once, and the channel is not shareable (`Channel::is_shared()` is false).

The `close()` function closes all channels and waits for any currently running
event handlers to finish. This is called by the destructor if the dispatch
queue is not empty.

The `empty()` function returns true if the dispatch list is currently empty.

The `run()` function runs until a channel is closed or a callback function
throws an exception; it returns immediately if the dispatch set is empty.
Asynchronous message handlers will continue to run in their own threads
regardless of whether the dispatch thread is currently calling `run()`.

The `result` structure returned from `run()` indicates why the
dispatch loop was stopped:

`result::why`           | Description                                         | `result::channel`     | `result::error`
-------------           | -----------                                         | -----------------     | ---------------
`reason::`**`closed`**  | A channel was closed                                | The closed channel    | Null
`reason::`**`empty`**   | The dispatch set was empty when `run()` was called  | Null                  | Null
`reason::`**`error`**   | A message handler callback threw an exception       | The throwing channel  | The exception thrown

Behaviour is undefined if `run()` is called in a thread other than the
application's main thread when the dispatch thread contains any synchronous
channels (`Channel::is_async()` is false).
