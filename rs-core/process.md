# Process Control #

By Ross Smith

* `#include "rs-core/process.hpp"`

## Contents ##

[TOC]

## Shell commands ##

* `std::string` **`shell`**`(const Ustring& cmd)`

Runs a shell command synchronously (via `popen()` or the equivalent),
capturing standard output. No error indication is returned; standard error
still goes to its usual destination, unless explicitly redirected in the
command line.

## Spawned process channels ##

These classes call `popen()` to create a child process on construction, with
its standard output connected to the channel. Closing or destroying the
channel calls `pclose()`.

### Class StreamProcess ###

* `class` **`StreamProcess`**`: public StreamChannel`
    * `explicit StreamProcess::`**`StreamProcess`**`(const Ustring& cmd)`
    * `virtual StreamProcess::`**`~StreamProcess`**`() noexcept`
    * `int StreamProcess::`**`status`**`() const noexcept`

This runs a process whose output is treated as an undifferentiated stream of
bytes. The `status()` function returns the value returned by `pclose()`, or -1
if the process has not been closed yet.

### Class TextProcess ###

* `class` **`TextProcess`**`: public MessageChannel<Ustring>`
    * `explicit TextProcess::`**`TextProcess`**`(const Ustring& cmd)`
    * `virtual TextProcess::`**`~TextProcess`**`() noexcept`
    * `std::string TextProcess::`**`read_all`**`()`
    * `int TextProcess::`**`status`**`() const noexcept`

This runs a process whose output is treated as a text stream. Each call to
`read()` yields one line of text (if any is available), with the terminating
LF or CRLF stripped.
