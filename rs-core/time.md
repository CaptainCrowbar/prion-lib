# Core Time and Date Algorithms #

By Ross Smith

* `#include "rs-core/time.hpp"`

## Contents ##

[TOC]

## Time and date operations ##

In this documentation, the `std::chrono::` prefix on the standard date and
time types is left out for brevity. For all functions here that accept or
return a broken down date, results are unspecified if the date is outside the
range of a `time_t` (1970-2038 on 32-bit systems), or if the fields are set to
values that do not represent a valid date.

### Time and date types ###

* `using` **`Dseconds`** `= duration<double>`
* `using` **`Ddays`** `= duration<double, ratio<86400>>`
* `using` **`Dyears`** `= duration<double, ratio<31557600>>`

Floating point duration types.

* `using` **`ReliableClock`** `= [high_resolution_clock or steady_clock]`

This is the highest resolution clock that can be trusted to be steady. It will
be `high_resolution_clock` if `high_resolution_clock::is_steady` is true,
otherwise `steady_clock`.

### General time and date operations ###

* `enum class` **`Zone`**
    * `Zone::`**`utc`**
    * `Zone::`**`local`**

This is passed to some of the time and date functions to indicate whether a
broken down date is expressed in UTC or the local time zone. For all functions
that take a `Zone` argument, behaviour is unspecified if the argument is not
one of these two values.

* `template <typename R, typename P> void` **`from_seconds`**`(double s, duration<R, P>& d) noexcept`
* `template <typename R, typename P> double` **`to_seconds`**`(const duration<R, P>& d) noexcept`

Convenience functions to convert between a `duration` and a floating point
number of seconds.

* `system_clock::time_point` **`make_date`**`(int year, int month, int day, int hour = 0, int min = 0, double sec = 0, Zone z = Zone::utc) noexcept`

Converts a broken down date into a time point. Behaviour if any of the date
arguments are invalid follows the same rules as `mktime()`.

### Time and date formatting ###

* `U8string` **`format_date`**`(system_clock::time_point tp, int prec = 0, Zone z = Zone::utc)`
* `U8string` **`format_date`**`(system_clock::time_point tp, const U8string& format, Zone z = Zone::utc)`

These convert a time point into a broken down date and format it. The first
version writes the date in ISO 8601 format (`"yyyy-mm-dd hh:mm:ss"`). If
`prec` is greater than zero, the specified number of decimal places will be
added to the seconds field.

The second version writes the date using the conventions of `strftime()`. This
will return an empty string if anything goes wrong; there is no way to
distinguish between a conversion error and a legitimately empty result (this
is a limitation of `strftime()`).

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

* `template <typename R, typename P> U8string` **`format_time`**`(const duration<R, P>& time, int prec = 0)`

Formats a time duration in days, hours, minutes, seconds, and (if `prec>0`)
fractions of a second.

## Timing utilities ##

* `class` **`Stopwatch`**
    * `explicit Stopwatch::`**`Stopwatch`**`(const U8string& name, int precision = 3) noexcept`
    * `explicit Stopwatch::`**`Stopwatch`**`(const char* name, int precision = 3) noexcept`
    * `Stopwatch::`**`~Stopwatch`**`() noexcept`

Simple timer for debugging. The destructor will write (using `logx()`) the
name and the elapsed time since construction. The note on exceptions for
`logx()` above applies here too.

### System specific time and date conversions ###

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
