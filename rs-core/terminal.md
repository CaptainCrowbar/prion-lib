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

## Terminal I/O operations ##

* `bool` **`is_stdout_redirected`**`() noexcept`

Attempts to detect whether standard output has been redirected to a file or
pipe (true), or is going directly to a terminal (false). This is not always
possible to detect reliably; this function is fairly reliable on Unix, less so
on Windows.

* `constexpr const char*` **`xt_up`** `=           <esc> [A    = Cursor up`
* `constexpr const char*` **`xt_down`** `=         <esc> [B    = Cursor down`
* `constexpr const char*` **`xt_right`** `=        <esc> [C    = Cursor right`
* `constexpr const char*` **`xt_left`** `=         <esc> [D    = Cursor left`
* `constexpr const char*` **`xt_erase_left`** `=   <esc> [1K   = Erase left`
* `constexpr const char*` **`xt_erase_right`** `=  <esc> [K    = Erase right`
* `constexpr const char*` **`xt_erase_above`** `=  <esc> [1J   = Erase above`
* `constexpr const char*` **`xt_erase_below`** `=  <esc> [J    = Erase below`
* `constexpr const char*` **`xt_erase_line`** `=   <esc> [2K   = Erase line`
* `constexpr const char*` **`xt_clear`** `=        <esc> [2J   = Clear screen`
* `constexpr const char*` **`xt_reset`** `=        <esc> [0m   = Reset attributes`
* `constexpr const char*` **`xt_bold`** `=         <esc> [1m   = Bold`
* `constexpr const char*` **`xt_under`** `=        <esc> [4m   = Underline`
* `constexpr const char*` **`xt_bold_off`** `=     <esc> [22m  = Bold off`
* `constexpr const char*` **`xt_under_off`** `=    <esc> [24m  = Underline off`
* `constexpr const char*` **`xt_colour_off`** `=   <esc> [39m  = Colour off`
* `constexpr const char*` **`xt_black`** `=        <esc> [30m  = Black foreground`
* `constexpr const char*` **`xt_red`** `=          <esc> [31m  = Red foreground`
* `constexpr const char*` **`xt_green`** `=        <esc> [32m  = Green foreground`
* `constexpr const char*` **`xt_yellow`** `=       <esc> [33m  = Yellow foreground`
* `constexpr const char*` **`xt_blue`** `=         <esc> [34m  = Blue foreground`
* `constexpr const char*` **`xt_magenta`** `=      <esc> [35m  = Magenta foreground`
* `constexpr const char*` **`xt_cyan`** `=         <esc> [36m  = Cyan foreground`
* `constexpr const char*` **`xt_white`** `=        <esc> [37m  = White foreground`
* `constexpr const char*` **`xt_black_bg`** `=     <esc> [40m  = Black background`
* `constexpr const char*` **`xt_red_bg`** `=       <esc> [41m  = Red background`
* `constexpr const char*` **`xt_green_bg`** `=     <esc> [42m  = Green background`
* `constexpr const char*` **`xt_yellow_bg`** `=    <esc> [43m  = Yellow background`
* `constexpr const char*` **`xt_blue_bg`** `=      <esc> [44m  = Blue background`
* `constexpr const char*` **`xt_magenta_bg`** `=   <esc> [45m  = Magenta background`
* `constexpr const char*` **`xt_cyan_bg`** `=      <esc> [46m  = Cyan background`
* `constexpr const char*` **`xt_white_bg`** `=     <esc> [47m  = White background`

Xterm cursor movement and output formatting codes.

* `string` **`xt_move_up`**`(int n)                  = <esc> [<n>A             = Cursor up n spaces`
* `string` **`xt_move_down`**`(int n)                = <esc> [<n>B             = Cursor down n spaces`
* `string` **`xt_move_right`**`(int n)               = <esc> [<n>C             = Cursor right n spaces`
* `string` **`xt_move_left`**`(int n)                = <esc> [<n>D             = Cursor left n spaces`
* `string` **`xt_colour`**`(int r, int g, int b)     = <esc> [38;5;<16-231>m   = Set foreground colour to an RGB value`
* `string` **`xt_colour`**`(Int3 rgb)                = <esc> [38;5;<16-231>m   = Set foreground colour to an RGB value`
* `string` **`xt_colour_bg`**`(int r, int g, int b)  = <esc> [48;5;<16-231>m   = Set background colour to an RGB value`
* `string` **`xt_colour_bg`**`(Int3 rgb)             = <esc> [38;5;<16-231>m   = Set foreground colour to an RGB value`
* `string` **`xt_grey`**`(int grey)                  = <esc> [38;5;<232-255>m  = Set foreground colour to a grey level`
* `string` **`xt_grey_bg`**`(int grey)               = <esc> [48;5;<232-255>m  = Set background colour to a grey level`

Functions for generating variable Xterm codes. The RGB levels passed to the
`xt_colour[_bg]()` functions are clamped to the range 0 to 5, yielding 216
different colours. The grey level passed to the `xt_grey[_bg]()` functions is
clamped to a range of 1-24.
