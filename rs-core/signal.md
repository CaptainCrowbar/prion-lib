# Signal Handling #

By Ross Smith

* `#include "rs-core/signal.hpp"`

## Contents ##

[TOC]

## Class PosixSignal ##

* `class` **`PosixSignal`**`: public MessageChannel<int>`
    * `using PosixSignal::`**`signal_list`** `= std::vector<int>`
    * `PosixSignal::`**`PosixSignal`**`(std::initializer_list<int> list)`
    * `explicit PosixSignal::`**`PosixSignal`**`(const signal_list& list)`
    * `virtual PosixSignal::`**`~PosixSignal`**`() noexcept`
    * `virtual bool PosixSignal::`**`is_async`**`() const noexcept` _= false_
    * `static Ustring` **`name`**`(int s)`

A channel that intercepts Unix signals, reporting them as channel messages. An
attempt to listen for an unblockable signal such as `SIGKILL` will be quietly
ignored. On Windows signals will never happen; waiting will always fail.

The `name()` function returns the signal name corresponding to the given
signal number. If the argument is not a valid signal number, this will simply
return the integer value as a string.
