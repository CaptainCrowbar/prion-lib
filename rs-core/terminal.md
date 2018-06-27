# Terminal I/O #

By Ross Smith

* `#include "rs-core/terminal.hpp"`

## Contents ##

[TOC]

## Progress bar ##

* `class` **`ProgressBar`**
    * `explicit ProgressBar::`**`ProgressBar`**`(Uview label, size_t length = 0, std::ostream& out = std::cout)`
    * `ProgressBar::`**`~ProgressBar`**`() noexcept`
    * `void ProgressBar::`**`operator()`**`(double x)`

This class draws a progress bar on the terminal, and updates it every time the
function call operator is called.

Constructor arguments are the label to print on the left of the progress bar,
the length of the bar in characters (calculated from the terminal width by
default), and the output stream to write it to. If an explicit length is
supplied, it is trusted to work with the available terminal dimensions; it is
up to the caller to ensure that the resulting output will not be too long
(allow for at least 16 characters in addition to the label and bar lengths).

The function call operator updates the progress bar to reflect the value
passed to it, and adds an estimated remaining time on the right. The value is
clamped to the unit range. Progress will never go backwards; a call with a
value less than the previous value will not update the bar, although it will
be taken into account in estimating the run time.

## Terminal controls ##

* `namespace RS::`**`Xterm`**

Contains definitions for common terminal control codes.

* `constexpr const char* Xterm::`**`up`** `=           <esc> [A    = Cursor up`
* `constexpr const char* Xterm::`**`down`** `=         <esc> [B    = Cursor down`
* `constexpr const char* Xterm::`**`right`** `=        <esc> [C    = Cursor right`
* `constexpr const char* Xterm::`**`left`** `=         <esc> [D    = Cursor left`
* `constexpr const char* Xterm::`**`erase_left`** `=   <esc> [1K   = Erase left`
* `constexpr const char* Xterm::`**`erase_right`** `=  <esc> [K    = Erase right`
* `constexpr const char* Xterm::`**`erase_above`** `=  <esc> [1J   = Erase above`
* `constexpr const char* Xterm::`**`erase_below`** `=  <esc> [J    = Erase below`
* `constexpr const char* Xterm::`**`erase_line`** `=   <esc> [2K   = Erase line`
* `constexpr const char* Xterm::`**`clear`** `=        <esc> [2J   = Clear screen`
* `constexpr const char* Xterm::`**`reset`** `=        <esc> [0m   = Reset attributes`
* `constexpr const char* Xterm::`**`bold`** `=         <esc> [1m   = Bold`
* `constexpr const char* Xterm::`**`under`** `=        <esc> [4m   = Underline`
* `constexpr const char* Xterm::`**`bold_off`** `=     <esc> [22m  = Bold off`
* `constexpr const char* Xterm::`**`under_off`** `=    <esc> [24m  = Underline off`
* `constexpr const char* Xterm::`**`colour_off`** `=   <esc> [39m  = Colour off`
* `constexpr const char* Xterm::`**`black`** `=        <esc> [30m  = Black fg`
* `constexpr const char* Xterm::`**`red`** `=          <esc> [31m  = Red fg`
* `constexpr const char* Xterm::`**`green`** `=        <esc> [32m  = Green fg`
* `constexpr const char* Xterm::`**`yellow`** `=       <esc> [33m  = Yellow fg`
* `constexpr const char* Xterm::`**`blue`** `=         <esc> [34m  = Blue fg`
* `constexpr const char* Xterm::`**`magenta`** `=      <esc> [35m  = Magenta fg`
* `constexpr const char* Xterm::`**`cyan`** `=         <esc> [36m  = Cyan fg`
* `constexpr const char* Xterm::`**`white`** `=        <esc> [37m  = White fg`
* `constexpr const char* Xterm::`**`black_bg`** `=     <esc> [40m  = Black bg`
* `constexpr const char* Xterm::`**`red_bg`** `=       <esc> [41m  = Red bg`
* `constexpr const char* Xterm::`**`green_bg`** `=     <esc> [42m  = Green bg`
* `constexpr const char* Xterm::`**`yellow_bg`** `=    <esc> [43m  = Yellow bg`
* `constexpr const char* Xterm::`**`blue_bg`** `=      <esc> [44m  = Blue bg`
* `constexpr const char* Xterm::`**`magenta_bg`** `=   <esc> [45m  = Magenta bg`
* `constexpr const char* Xterm::`**`cyan_bg`** `=      <esc> [46m  = Cyan bg`
* `constexpr const char* Xterm::`**`white_bg`** `=     <esc> [47m  = White bg`

Terminal cursor movement and output formatting codes.

* `Ustring Xterm::`**`move_up`**`(int n)     = <esc> x1b[ <n> A  = Cursor up n spaces`
* `Ustring Xterm::`**`move_down`**`(int n)   = <esc> x1b[ <n> B  = Cursor down n spaces`
* `Ustring Xterm::`**`move_right`**`(int n)  = <esc> x1b[ <n> C  = Cursor right n spaces`
* `Ustring Xterm::`**`move_left`**`(int n)   = <esc> x1b[ <n> D  = Cursor left n spaces`

Functions for generating variable terminal control codes.

## Terminal colours ##

* `class` **`Xcolour`**
    * `constexpr Xcolour::`**`Xcolour`**`()`
    * `constexpr explicit Xcolour::`**`Xcolour`**`(int grey) noexcept`
    * `explicit Xcolour::`**`Xcolour`**`(Int3 rgb) noexcept`
    * `constexpr Xcolour::`**`Xcolour`**`(int r, int g, int b) noexcept`
    * `constexpr Xcolour::`**`Xcolour`**`(const Xcolour& xc) noexcept`
    * `constexpr Xcolour::`**`Xcolour`**`(Xcolour&& xc) noexcept`
    * `constexpr Xcolour::`**`~Xcolour`**`() noexcept`
    * `constexpr Xcolour& Xcolour::`**`operator=`**`(const Xcolour& xc) noexcept`
    * `constexpr Xcolour& Xcolour::`**`operator=`**`(Xcolour&& xc) noexcept`
    * `constexpr bool Xcolour::`**`is_null`**`() const noexcept`
    * `constexpr bool Xcolour::`**`is_grey`**`() const noexcept`
    * `constexpr bool Xcolour::`**`is_rgb`**`() const noexcept`
    * `constexpr int Xcolour::`**`grey`**`() const noexcept`
    * `constexpr int Xcolour::`**`r`**`() const noexcept`
    * `constexpr int Xcolour::`**`g`**`() const noexcept`
    * `constexpr int Xcolour::`**`b`**`() const noexcept`
    * `constexpr int Xcolour::`**`index`**`() const noexcept`
    * `Int3 Xcolour::`**`as_rgb24`**`() const noexcept`
    * `Ustring Xcolour::`**`fg`**`() const`
    * `Ustring Xcolour::`**`bg`**`() const`
    * `Ustring Xcolour::`**`repr`**`() const`
    * `static constexpr Xcolour Xcolour::`**`from_index`**`(int i) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& out, Xcolour xc)`
* `Ustring` **`to_str`**`(Xcolour xc)`

This is used to generate 8-bit terminal colour control codes. The colour can
be null (which produces no output), a grey level from 1 to 24, or a set of RGB
levels from 0 to 5. Grey and RGB levels are clamped to their respective
ranges.

The `as_rgb24()` function converts the colour code to a 24-bit RGB triple. The
`index()` function returns the index (0-255) used in the control sequence.

The `fg()` and `bg()` functions return the terminal control strings to set the
foreground or background colour. The output operator and `to_str()` function
generate the same string as `fg()`.

    <esc> [38;5; <16-231> m   = Set foreground colour to an RGB value
    <esc> [38;5; <232-255> m  = Set foreground colour to a grey level
    <esc> [48;5; <16-231> m   = Set background colour to an RGB value
    <esc> [48;5; <232-255> m  = Set background colour to a grey level

## Terminal I/O functions ##

* `bool` **`is_stdout_redirected`**`() noexcept`

Attempts to detect whether standard output has been redirected to a file or
pipe (true), or is going directly to a terminal (false). This is not always
possible to detect reliably; this function is fairly reliable on Unix, less so
on Windows.
