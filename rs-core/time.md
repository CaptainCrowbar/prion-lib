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

* `constexpr uint32_t` **`utc_zone`**
* `constexpr uint32_t` **`local_zone`**

One of these is passed to some of the time and date functions to indicate
whether a broken down date is expressed in UTC or the local time zone. If none
of these are present, the default is `utc_zone`.

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

* `template <typename R, typename P> void` **`from_seconds`**`(double s, duration<R, P>& d) noexcept`
* `template <typename R, typename P> double` **`to_seconds`**`(const duration<R, P>& d) noexcept`

Convenience functions to convert between a `duration` and a floating point
number of seconds.

* `system_clock::time_point` **`make_date`**`(int year, int month, int day, int hour = 0, int min = 0, double sec = 0, uint32_t flags = utc_zone)`

Converts a broken down date into a time point. Behaviour if any of the date
arguments are invalid follows the same rules as `mktime()`. This will throw
`std::invalid_argument` if an invalid combination of flags is passed.

## Time and date formatting ##

* `Ustring` **`format_date`**`(system_clock::time_point tp, int prec = 0, uint32_t flags = utc_zone)`
* `Ustring` **`format_date`**`(system_clock::time_point tp, Uview format, uint32_t flags = utc_zone)`

These convert a time point into a broken down date and format it. The first
version writes the date in ISO 8601 format (`"yyyy-mm-dd hh:mm:ss"`). If
`prec` is greater than zero, the specified number of decimal places will be
added to the seconds field.

The second version writes the date using the conventions of `strftime()`. This
will return an empty string if anything goes wrong (there is no way to
distinguish between a conversion error and a legitimately empty result; this
is a limitation of `strftime()`).

Both of these will throw `std::invalid_argument` if an invalid combination of
flags is passed.

For reference, the portable subset of the `strftime()` formatting codes are:

| Code  | Description                          | Code    | Description                          |
| ----  | -----------                          | ----    | -----------                          |
|       **Date elements**                      | |       **Weekday elements**                   | |
| `%Y`  | Year number                          | `%a`    | Local weekday abbreviation           |
| `%y`  | Last 2 digits of the year (`00-99`)  | `%w`    | Sunday-based weekday number (`0-6`)  |
| `%m`  | Month number (`00-12`)               | `%A`    | Local weekday name                   |
| `%B`  | Local month name                     | `%U`    | Sunday-based week number (`00-53`)   |
| `%b`  | Local month abbreviation             | `%W`    | Monday-based week number (`00-53`)   |
| `%d`  | Day of the month (`01-31`)           |         **Other elements**                     | |
|       **Time of day elements**               | | `%c`  | Local standard date/time format      |
| `%H`  | Hour on 24-hour clock (`00-23`)      | `%x`    | Local standard date format           |
| `%I`  | Hour on 12-hour clock (`01-12`)      | `%X`    | Local standard time format           |
| `%p`  | Local equivalent of a.m./p.m.        | `%j`    | Day of the year (`001-366`)          |
| `%M`  | Minute (`00-59`)                     | `%Z`    | Time zone name                       |
| `%S`  | Second (`00-60`)                     | `%z`    | Time zone offset                     |

* `template <typename R, typename P> Ustring` **`format_time`**`(const duration<R, P>& time, int prec = 0)`

Formats a time duration in days, hours, minutes, seconds, and (if `prec>0`)
fractions of a second.

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

## Timing utilities ##

* `class` **`Backoff`**
    * `Backoff::`**`Backoff`**`() noexcept`
    * `template <typename R1, typename P1, typename R2, typename P2> Backoff::`**`Backoff`**`(duration<R1, P1> min_interval, duration<R2, P2> max_interval) noexcept`
    * `Backoff::`**`~Backoff`**`() noexcept`
    * `Backoff::`**`Backoff`**`(const Backoff& b) noexcept`
    * `Backoff::`**`Backoff`**`(Backoff&& b) noexcept`
    * `Backoff& Backoff::`**`operator=`**`(const Backoff& b) noexcept`
    * `Backoff& Backoff::`**`operator=`**`(Backoff&& b) noexcept`
    * `[duration type] Backoff::`**`min`**`() const noexcept`
    * `[duration type] Backoff::`**`max`**`() const noexcept`
    * `template <typename Predicate> void Backoff::`**`wait`**`(Predicate pred) const`
    * `template <typename Predicate, typename R, typename P> bool Backoff::`**`wait_for`**`(Predicate pred, duration<R, P> timeout) const`
    * `template <typename Predicate, typename C, typename D> bool Backoff::`**`wait_until`**`(Predicate pred, time_point<C, D> timeout) const`

This implements a backoff wait algorithm. The constructor takes a minimum and
maximum poll interval, defaulting to 10 microseconds and 10 milliseconds
respectively. The `min()` and `max()` query functions return the intervals
expressed in an unspecified duration type (the same type for both functions).
When one of the wait functions is called, after testing the predicate, it will
wait for the minimum interval before testing again, doubling the interval
after each test until the maximum interval is reached, after which it will
continue testing at that interval. The `wait()` function will return when the
predicate is true; the other wait functions will return true when the
predicate is true, or false when the timeout expires, whichever comes first.
All of the wait functions will propagate any exceptions thrown by the
predicate.

* `class` **`Stopwatch`**
    * `explicit Stopwatch::`**`Stopwatch`**`(Uview name, int precision = 3) noexcept`
    * `Stopwatch::`**`~Stopwatch`**`() noexcept`

Simple timer for debugging. The destructor will write (using `logx()`) the
name and the elapsed time since construction. The note on exceptions for
`logx()` above applies here too.

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
