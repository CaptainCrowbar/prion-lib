# Core Time and Date Algorithms #

By Ross Smith

* `#include "rs-core/time.hpp"`

In this documentation, the `std::chrono::` prefix on the standard date and
time types is left out for brevity. For all functions here that accept or
return a broken down date, results are unspecified if the date is outside the
range of a `time_t` (1970-2038 on 32-bit systems), or if the fields are set to
values that do not represent a valid date.

## Contents ##

[TOC]

## Time and date types ##

* `using` **`Dseconds`** `= duration<double>`
* `using` **`Ddays`** `= duration<double, ratio<86400>>`
* `using` **`Dyears`** `= duration<double, ratio<31557600>>`

Floating point duration types.

* `using` **`ReliableClock`** `= [high_resolution_clock or steady_clock]`

This is the highest resolution clock that can be trusted to be steady. It will
be `high_resolution_clock` if `high_resolution_clock::is_steady` is true,
otherwise `steady_clock`.

## Constants ##

* `constexpr uint32_t` **`ymd_order`**
* `constexpr uint32_t` **`dmy_order`**
* `constexpr uint32_t` **`mdy_order`**

One of these is passed to the `parse_date()` function to indicate which order
the date elements are in. If none of these are present, the default is
`ymd_order`.

## General time and date operations ##

* `template <typename C1, typename D1, typename C2, typename D2> void` **`convert_time_point`**`(time_point<C1, D1> src, time_point<C2, D2>& dst)`
* `template <typename TP2, typename C1, typename D1> TP2` **`convert_time_point`**`(time_point<C1, D1> src)`

Convert a time point from one representation to another, possibly on a
different clock. This will call `time_point_cast()` if possible; otherwise, it
will check the current times on both clocks and use that to convert from one
clock to the other.

## System specific time and date conversions ##

* _Unix_
    * `template <typename R, typename P> timespec` **`duration_to_timespec`**`(const duration<R, P>& d) noexcept`
    * `template <typename R, typename P> timeval` **`duration_to_timeval`**`(const duration<R, P>& d) noexcept`
    * `timespec` **`timepoint_to_timespec`**`(const system_clock::time_point& tp) noexcept`
    * `timeval` **`timepoint_to_timeval`**`(const system_clock::time_point& tp) noexcept`
    * `template <typename R, typename P> void` **`timespec_to_duration`**`(const timespec& ts, duration<R, P>& d) noexcept`
    * `system_clock::time_point` **`timespec_to_timepoint`**`(const timespec& ts) noexcept`
    * `template <typename R, typename P> void` **`timeval_to_duration`**`(const timeval& tv, duration<R, P>& d) noexcept`
    * `system_clock::time_point` **`timeval_to_timepoint`**`(const timeval& tv) noexcept`
* _Windows_
    * `system_clock::time_point` **`filetime_to_timepoint`**`(const FILETIME& ft) noexcept`
    * `FILETIME` **`timepoint_to_filetime`**`(const system_clock::time_point& tp) noexcept`

Conversion functions between C++ chrono types and system API types. Some of
these return their result through a reference argument to avoid having to
explicitly specify the duration type. Behaviour is undefined if the value
being represented is out of range for either the source or destination type.

The Windows functions are only defined on Win32 builds; the Unix functions are
always defined (since the relevant time structures are defined in the Windows
API).

For reference, the system time types are:

* _Unix_
    * `#include <time.h>`
        * `struct` **`timespec`**
            * `time_t timespec::`**`tv_sec`** `// seconds`
            * `long timespec::`**`tv_nsec`** `// nanoseconds`
    * `#include <sys/time.h>`
        * `struct` **`timeval`**
            * `time_t timeval::`**`tv_sec`** `// seconds`
            * `suseconds_t timeval::`**`tv_usec`** `// microseconds`
* _Windows_
    * `#include <windows.h>`
        * `struct` **`FILETIME`**
            * `DWORD FILETIME::`**`dwLowDateTime`** `// low 32 bits`
            * `DWORD FILETIME::`**`dwHighDateTime`** `// high 32 bits`

## Time and date formatting ##

* `class` **`DateFormat`**
    * `DateFormat::`**`DateFormat`**`()`
    * `explicit DateFormat::`**`DateFormat`**`(Uview format, uint32_t flags = utc_zone)`
    * `Ustring DateFormat::`**`operator()`**`(system_clock::time_point tp, uint32_t flags = 0) const`
* `Ustring` **`date_format`**`(system_clock::time_point tp, Uview format, uint32_t flags = utc_zone)`

Alternative date formatting function (see also `format_date()` in
`[core/common]()`, which uses a `strftime()` based format). If a zone flag is
supplied to the function call operator, it overrides the one originally
supplied to the constructor.

Code    | Description                           | Code      | Description
----    | -----------                           | ----      | -----------
`yy`    | Last 2 digits of the year             | `H`       | 1-2 digit hour number (`0-23`)
`yyyy`  | Full year                             | `HH`      | 2 digit hour number (`00-23`)
`m`     | 1-2 digit month number (`1-12`)       | `M`       | 1-2 digit minute number (`0-59`)
`mm`    | 2 digit month number (`01-12`)        | `MM`      | 2 digit minute number (`00-59`)
`mmm`   | Month abbreviation (not localised)    | `S`       | 1-2 digit second number (`0-59`)
`d`     | 1-2 digit day of month (`1-31`)       | `SS`      | 2 digit second number (`00-59`)
`dd`    | 2 digit day of month (`01-31`)        | `sss...`  | Fraction of a second
`www`   | Weekday abbreviation (not localised)  | `+ZZZZ`   | Time zone offset

The `"mmm"` and `"www"` codes can be written in lower case, title case, or
upper case, determining the case of the output. Alphanumeric characters that
are not part of a placeholder code are not allowed. The constructor of
`DateFormat`, and the `date_format()` function, will throw
`std::invalid_argument` if the format string is invalid.

## Time and date parsing ##

* `system_clock::time_point` **`parse_date`**`(Uview str, uint32_t flags = utc_zone | ymd_order)`

Parse a date expressed in broken down format (e.g. `"2017-11-04 11:53:00"`).
Year, month, and day are required, in the order specified by the `flags`
argument; the month can be a number, an abbreviation, or a full English name
(case insensitive). Hours, minutes, seconds, and fractions of a second are
optional. Fields can be separated with spaces or any ASCII punctuation marks;
following ISO convention, a `"T"` can also be used between the date and time.
This function does not make any attempt to interpret a time zone in the
string. It will throw `std::invalid_argument` if the format is invalid or an
invalid combination of flags is supplied; behaviour is unspecified if the
format is correct but the string does not represent a valid date; behaviour is
undefined if the date is outside the representable range of the system clock.

* `template <typename R, typename P> void` **`parse_time`**`(Uview str, duration<R, P>& t)`
* `template <typename D> D` **`parse_time`**`(Uview str)`

Parse a time expressed in Julian years, days, hours, minutes, and seconds
(e.g. `"12y 345d 12h 34m 56s"`). Spaces are ignored; a leading sign is
allowed; any number can be a floating point number with decimals and
exponents; units can be abbreviations or full words; years can have any SI
prefix with a positive exponent (e.g. `"65 My"`); seconds can have any SI
prefix with a negative exponent (e.g. `"100 µs"`); either `"u"` or `"µ"` can
be used for "micro". These will throw `std::invalid_argument` if the format is
invalid; behaviour is undefined if the format is correct but the resulting
time cannot be represented by the duration type, or if the second version of
`parse_time()` is called with a return type that is not an instantiation of
`std::chrono::duration`.

## Timing base classes ##

* `class` **`Wait`**
    * `using Wait::`**`clock_type`**` = ReliableClock`
    * `using Wait::`**`duration`**` = clock_type::duration`
    * `using Wait::`**`time_point`**` = clock_type::time_point`
    * `virtual Wait::`**`~Wait()`** `noexcept = 0`
    * `virtual bool Wait::`**`is_shared`**`() const noexcept`
    * `virtual bool Wait::`**`poll`**`()`
    * `virtual void Wait::`**`wait`**`()`
    * `template <typename R, typename P> bool Wait::`**`wait_for`**`(std::chrono::duration<R, P> t)`
    * `template <typename C, typename D> bool Wait::`**`wait_until`**`(std::chrono::time_point<C, D> t)`
    * `protected Wait::`**`Wait`**`() noexcept`
    * `protected virtual bool Wait::`**`do_wait_for`**`(duration t)`
    * `protected virtual bool Wait::`**`do_wait_until`**`(time_point t)`

A general purpose base class for waitable objects. A derived class must
implement at least one of `do_wait_for()` or `do_wait_until()`, whichever is
more convenient for the particular class (the default implementations call
each other); optionally it may also implement either or both of `poll()` or
`wait()`, if a more efficient implementation than the default (calling
`do_wait_*()`) is available.

The `is_shared()` method indicates whether or not it is safe for multiple
threads to wait on the same object simultaneously; this is false by default.

* `class` **`PollWait`**`: public Wait`
    * `virtual bool PollWait::`**`poll`**`() override = 0`
    * `virtual void PollWait::`**`wait`**`() override`
    * `duration PollWait::`**`min_interval`**`() const noexcept`
    * `duration PollWait::`**`max_interval`**`() const noexcept`
    * `template <typename R, typename P> void PollWait::`**`set_interval`**`(std::chrono::duration<R, P> t)`
    * `template <typename R1, typename P1, typename R2, typename P2> void PollWait::`**`set_interval`**`(std::chrono::duration<R1, P1> t1, std::chrono::duration<R2, P2> t2)`
    * `protected PollWait::`**`PollWait`**`() noexcept`

An intermediate base class for waitable objects that must be implemented by
polling when no more efficient implementation is available. This implements a
backoff strategy, starting out by waiting for the minimum interval, increasing
the interval on each wait until the maximum interval is reached. A derived
class must implement `poll()`; it may also implement `wait()` if a more
efficient implementation is available (this is intended for situations where
polling and indefinite wait APIs are available, but no wait with timeout is
provided).

The `set_interval()` functions set the minimum and maximum wait times; the
first version sets both intervals to the same value, producing a fixed wait
time. The default minimum and maximum intervals are 10 microseconds and 10
milliseconds respectively.

## Timing utilities ##

* `class` **`PollCondition`**`: public PollWait`
    * `template <typename Predicate> explicit PollCondition::`**`PollCondition`**`(Predicate p)`

A simple wait class that polls until a condition is true.

* `class` **`Stopwatch`**
    * `explicit Stopwatch::`**`Stopwatch`**`(Uview name, int precision = 3) noexcept`
    * `Stopwatch::`**`~Stopwatch`**`() noexcept`

This is a simple timer specialized for debugging. The destructor will write
(using `logx()`) the name and the elapsed time since construction.

* `template <typename D> class` **`Timer`**
    * `using Timer::`**`duration`** `= D`
    * `Timer::`**`Timer`**`()`
    * `Timer::`**`~Timer`**`() noexcept`
    * `Timer::`**`operator duration`**`() const`
    * `duration Timer::`**`get`**`() const`
    * `void Timer::`**`reset`**`()`

Another simple timer (not specialized for debugging). The template argument
must be an instantiation of `std::chrono::duration`. The timer is started on
construction, and restarted when `reset()` is called; `get()` or the
conversion operator return the time since the last start.
