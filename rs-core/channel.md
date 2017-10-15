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
        * `class` **`TimerChannel`**`: public EventChannel, public Interval`
    * `[abstract] template <typename T> class` **`MessageChannel`**`: public Channel`
        * `template <typename T> class` **`GeneratorChannel`**`: public MessageChannel<T>`
        * `template <typename T> class` **`QueueChannel`**`: public MessageChannel<T>`
        * `template <typename T> class` **`ValueChannel`**`: public MessageChannel<T>`
    * `[abstract] class` **`StreamChannel`**`: public Channel`
        * `class` **`BufferChannel`**`: public StreamChannel`
* `[abstract] class` **`Interval`**
    * `class` **`Dispatch`**`: public Interval`
    * `[abstract] class` **`Polled`**`: public Interval`

## Support classes ##

### Class Interval ###

* `class` **`Interval`**
    * `using Interval::`**`time`** `= std::chrono::microseconds`
    * `time Interval::`**`interval`**`() const noexcept`
    * `template <typename R, typename P> void Interval::`**`set_interval`**`(std::chrono::duration<R, P> t) noexcept`

This is a general purpose mixin class for anything that uses a time interval.
It also provides the definition of the time unit used internally by all of the
message dispatch classes. The `set_interval()` function clamps the argument to
a minimum of zero.

### Class Polled ###

* `class` **`Polled`**`: public Interval`
    * `static constexpr auto Polled::`**`default_interval`** `= 10ms`
    * `virtual Polled::`**`~Polled`**`()`
    * `virtual Channel::state Polled::`**`poll`**`() = 0`

Mixin class for channels for which no easy timed wait operation exists,
requiring a polled implementation. Polled channel classes should derive from
both `Channel` and `Polled`, implementing `close()`, `poll()`, and
`do_wait()`. The `poll()` function should immediately return `ready`,
`waiting`, or `closed`; the `do_wait()` function should just call
`polled_wait()`.

## Channel base classes ##

### Class Channel ###

* `class` **`Channel`**
    * `enum class Channel::`**`state`**
        * `Channel::state::`**`ready`**
        * `Channel::state::`**`waiting`**
        * `Channel::state::`**`closed`**
    * `virtual Channel::`**`~Channel`**`() noexcept`
    * `Channel::`**`Channel`**`(Channel&& c) noexcept`
    * `Channel& Channel::`**`operator=`**`(Channel&& c) noexcept`
    * `virtual void Channel::`**`close`**`() noexcept = 0`
    * `virtual bool Channel::`**`multiplex`**`() const noexcept` _= false_
    * `virtual state Channel::`**`poll`**`()`
    * `virtual bool Channel::`**`sync`**`() const noexcept` _= false_
    * `template <typename R, typename P> Channel::state Channel::`**`wait`**`(std::chrono::duration<R, P> t)`
    * `protected virtual Channel::state Channel::`**`do_wait`**`(Interval::time t) = 0`

The base class for all readable message channels. All concrete channel classes
must derive from one of the three intermediate classes below, not directly
from Channel. Derived classes need to implement at least `close()` and
`do_wait()`, and may optionally implement `poll()` if a more efficient
implementation than the default call to `do_wait(0)` is available. The
`close()` function must be async safe and idempotent. The `wait()` function
simply converts the timeout to the internal duration type and calls
`do_wait()`.

If `multiplex()` is true, it is safe to call `wait()` (and `read()` where
relevant) from multiple threads at the same time, provided it is acceptable
for each message to be delivered to an unpredictable choice of thread.

If `sync()` is true, the channel can only be used in a synchronous dispatch
handler (see `Dispatch` below), usually because it calls an underlying native
API that is only intended to be used from the main thread.

The channel base classes implement move operations in case a derived class
needs them, but derived channel types are usually not movable. Channel classes
are never copyable.

### Class EventChannel ###

* `class` **`EventChannel`**`: public Channel`
    * `using EventChannel::`**`callback`** `= std::function<void()>`
    * `protected EventChannel::`**`EventChannel`**`()`

Intermediate base class for event channels (channels that carry no information
beyond the fact that an event has happened). The `callback` function type is
the callback type expected to be passed to `Dispatch::add()` (see below) when
an event channel is added to the dispatch set.

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

A message channel type should implement `read()` as well as the usual
`Channel` members. The `read()` function should return true on a successful
read, false if the channel is closed. It will normally be called only after a
successful `wait()`; if no data is immediately available, `read()` may block
or return false. The state of the argument to `read()` if it returns false is
unspecified.

The `read_opt()` function calls `read()` and returns the message on success,
or an empty value on failure.

### Class StreamChannel ###

* `class` **`StreamChannel`**`: public Channel`
    * `using StreamChannel::`**`callback`** `= std::function<void(std::string&)>`
    * `static constexpr size_t StreamChannel::`**`default_buffer`** `= 16384`
    * `virtual size_t StreamChannel::`**`read`**`(void* dst, size_t maxlen) = 0`
    * `size_t StreamChannel::`**`buffer`**`() const noexcept`
    * `void StreamChannel::`**`set_buffer`**`(size_t n) noexcept`
    * `std::string StreamChannel::`**`read_all`**`()`
    * `std::string StreamChannel::`**`read_str`**`()`
    * `size_t StreamChannel::`**`read_to`**`(std::string& dst)`
    * `protected StreamChannel::`**`StreamChannel`**`()`

Intermediate base class for byte stream channels. When a stream channel is
added to a dispatch set, the callback function will be invoked whenever more
data arrives. The string argument to the callback is a buffer internal to the
dispatch manager; it starts out empty, with incoming data appended to it
before each callback. Data will never be removed from the buffer by the
dispatch manager; the callback function is responsible for removing any data
it has consumed.

Stream channels should implement `read()` as well as the usual `Channel`
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
    * `virtual bool TrueChannel::`**`multiplex`**`() const noexcept` _= true_
* `class` **`FalseChannel`**`: public EventChannel`
    * `virtual bool FalsChannel::`**`multiplex`**`() const noexcept` _= true_

Trivial event channels whose `wait()` functions always succeed immediately
(`TrueChannel`) or always time out (`FalseChannel`).

### Class TimerChannel ###

* `class` **`TimerChannel`**`: public EventChannel, public Interval`
    * `TimerChannel::`**`TimerChannel`**`() noexcept`
    * `template <typename R, typename P> explicit TimerChannel::`**`TimerChannel`**`(std::chrono::duration<R, P> t) noexcept`
    * `virtual bool TimerChannel::`**`multiplex`**`() const noexcept` _= true_
    * `void TimerChannel::`**`flush`**`() noexcept`
    * `Interval::time TimerChannel::`**`next`**`() const noexcept`

An event channel that delivers one tick every interval, starting at one
interval after the time of construction. Multiple ticks may be delivered at
once (represented by repeated calls to `wait()` returning success immediately)
if multiple intervals have elapsed since the last check.

The `next()` function returns the time of the next tick (this may be in the
past if multiple ticks are pending); `flush()` discards any pending ticks.
These are async safe and can be called from any thread.

### Class GeneratorChannel ###

* `template <typename T> class` **`GeneratorChannel`**`: public MessageChannel<T>`
    * `using GeneratorChannel::`**`generator`** `= std::function<T()>`
    * `GeneratorChannel::`**`GeneratorChannel`**`()`
    * `template <typename F> explicit GeneratorChannel::`**`GeneratorChannel`**`(F f)`

A channel that calls a function to generate incoming messages. A default
constructed channel is always closed. Calling `wait()` on an open channel
always succeeds immediately, apart from any delay involved in the callback
function itself.

### Class QueueChannel ###

* `template <typename T> class` **`QueueChannel`**`: public MessageChannel<T>`
    * `QueueChannel::`**`QueueChannel`**`()`
    * `virtual bool QueueChannel::`**`multiplex`**`() const noexcept` _= true_
    * `void QueueChannel::`**`clear`**`() noexcept`
    * `bool QueueChannel::`**`write`**`(const T& t)`
    * `bool QueueChannel::`**`write`**`(T&& t)`

A last in, first out message queue.

### Class ValueChannel ###

* `template <typename T> class` **`ValueChannel`**`: public MessageChannel<T>`
    * `ValueChannel::`**`ValueChannel`**`()`
    * `explicit ValueChannel::`**`ValueChannel`**`(const T& t)`
    * `virtual bool ValueChannel::`**`multiplex`**`() const noexcept` _= true_
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
    * `bool BufferChannel::`**`write`**`(const void* src, size_t len)`
    * `bool BufferChannel::`**`write_str`**`(const std::string& src)`

A byte stream buffer channel. Message boundaries are not preserved; calling
`read()` will extract all pending data up to the length limit, regardless of
whether it was written as a single block or multiple smaller blocks.

## Dispatch control class ##

* `class` **`Dispatch`**`: public Interval`
    * `enum class Dispatch::`**`mode`**
        * `Dispatch::mode::`**`sync`**
        * `Dispatch::mode::`**`async`**
    * `enum class Dispatch::`**`reason`**
        * `Dispatch::reason::`**`empty`** _- The dispatch set is empty_
        * `Dispatch::reason::`**`closed`** _- A channel was closed_
        * `Dispatch::reason::`**`error`** _- A message handler callback threw an exception_
    * `struct Dispatch::`**`result_type`**
        * `Channel* result_type::`**`channel`** `= nullptr`
        * `std::exception_ptr result_type::`**`error`** `= nullptr`
        * `void result_type::`**`rethrow`**`() const`
        * `Dispatch::reason result_type::`**`why`**`() const noexcept`
    * `static constexpr auto Dispatch::`**`default_interval`** `= 1ms`
    * `Dispatch::`**`Dispatch`**`() noexcept`
    * `Dispatch::`**`~Dispatch`**`() noexcept`
    * `template <typename F> void Dispatch::`**`add`**`(EventChannel& chan, mode m, F func)`
    * `template <typename T, typename F> void Dispatch::`**`add`**`(MessageChannel<T>& chan, mode m, F func)`
    * `template <typename F> void Dispatch::`**`add`**`(StreamChannel& chan, mode m, F func)`
    * `void Dispatch::`**`drop`**`(Channel& chan) noexcept`
    * `bool Dispatch::`**`empty`**`() const noexcept`
    * `result_type Dispatch::`**`run`**`() noexcept`
    * `void Dispatch::`**`stop`**`() noexcept`

The message dispatch manager class. Create one instance of this class
(normally in the application's main thread), add channels and their associated
callback functions to it, and call `run()` to run the dispatch loop. Behaviour
is undefined if `run()` is called on more than one `Dispatch` object at the
same time.

`Dispatch` member functions must only be called synchronously (i.e. not from
inside an asynchronous callback function). Channels can safely be destroyed
while in a dispatch set (the `Dispatch` object will notice this and silently
drop the channel), provided this is done synchronously.

The `add()` functions start a synchronous or asynchronous task reading from
the channel. They will throw `invalid_argument` if any of these conditions is
true:

* The same channel is added more than once, and the channel is not multiplex (`Channel::multiplex()` is false).
* The mode flag is not one of the `Dispatch::mode` enumeration values.
* The channel is synchronous (`Channel::sync()` is true), but the mode is `async`.
* The callback function is null.

The `drop()` function removes a channel from the dispatch set (without closing
it).

The `run()` function runs until a channel is closed or a callback function
throws an exception; it returns immediately if the dispatch set is empty.
Asynchronous message handlers will continue to run in other threads regardless
of whether the dispatch thread is currently calling `run()`.

The return value from `run()` indicates which channel was responsible for
ending the run and the exception it threw, if any; both members will be null
if the dispatch set is empty. The result type's `rethrow()` function will
rethrow the exception if it is not null, otherwise do nothing. The `why()`
function returns a flag summarizing the reason for the run to stop.

The `stop()` function closes all channels and waits for them to finish (the
destructor calls this).
