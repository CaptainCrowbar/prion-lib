# I/O Utilities #

By Ross Smith

* `#include "rs-core/io.hpp"`

## Contents ##

[TOC]

## I/O abstract base class ##

* `class` **`IO`**

This is an abstract base class for classes that implement file and string I/O.
Classes derived from this do not normally throw exceptions from I/O operation
failures, including failure to open a file in the first place (but may throw
for other reasons, e.g. allocation failure); instead the `error()` and
`check()` methods can be used to test for errors and optionally throw.

* `class IO::`**`line_iterator`**
    * `using IO::line_iterator::`**`iterator_category`** `= std::input_iterator_category`
    * `using IO::line_iterator::`**`value_type`** `= std::string`

An iterator over the lines in a text file (see `read_line()` for the
semantics).

* `enum class IO::`**`mode`**
    * `IO::mode::`**`read_only`**      _- Open for read only; fail if the file does not exist_
    * `IO::mode::`**`write_only`**     _- Open for write only; always create a new file, replacing it if it already exists_
    * `IO::mode::`**`append`**         _- Open for append; open the file if it already exists, otherwise create a new one_
    * `IO::mode::`**`create_always`**  _- Open for read and write; always create a new file, replacing it if it already exists_
    * `IO::mode::`**`open_always`**    _- Open for read and write; open the file if it already exists, otherwise create a new one_
    * `IO::mode::`**`open_existing`**  _- Open for read and write; open the file if it already exists, otherwise fail_

Flags for some of the commonly used opening modes.

* `virtual IO::`**`~IO`**`()`

Destructor.

* `virtual void IO::`**`close`**`() noexcept`

Close the file. This is only needed if you want to check for an error
afterwards; any open file is closed by the destructor.

* `virtual void IO::`**`flush`**`() noexcept`

Flush the stream. The semantics of this depends on the particular derived
class. Flushing an input stream usually does nothing.

* `virtual bool IO::`**`is_open`**`() const noexcept`

True if the file is open.

* `virtual bool IO::`**`is_terminal`**`() const noexcept`

True if the I/O stream is reading or writing to a terminal.

* `virtual size_t IO::`**`read`**`(void* ptr, size_t maxlen) noexcept`
* `virtual std::string IO::`**`read_line`**`()`
* `virtual int IO::`**`getc`**`() noexcept`
* `std::string IO::`**`read_all`**`()`
* `size_t IO::`**`read_n`**`(std::string& s, size_t maxlen = 1024)`
* `std::string IO::`**`read_str`**`(size_t maxlen)`

Input functions. The `read()` function reads up to `maxlen` bytes into the
destination buffer, returning the number of bytes actually read. The
`read_line()` function reads up to the next line feed (or EOF), returning the
line as a string (the line feed is discarded). The `getc()` function reads one
byte, returning the byte value or `EOF`.

The `read_all()` function reads all available data until it reaches EOF or an
error occurs. The `read_n()` and `read_str()` functions call `read()`, writing
the incoming data into a string instead of a user supplied buffer.

* `virtual void IO::`**`seek`**`(ptrdiff_t offset, int which = SEEK_CUR) noexcept`
* `virtual ptrdiff_t IO::`**`tell`**`() noexcept`

Set or query the current position in the file. The `which` argument takes the
standard values `SEEK_CUR`, `SEEK_SET`, or `SEEK_END`. These may not work on
all streams (it is not usually possible to seek on standard input or output).

* `virtual size_t IO::`**`write`**`(const void* ptr, size_t len)`
* `virtual void IO::`**`write_n`**`(size_t n, char c)`
* `virtual void IO::`**`putc`**`(char c)`
* `void IO::`**`write_line`**`()`
* `void IO::`**`write_line`**`(const std::string& str)`
* `size_t IO::`**`write_str`**`(const std::string& str)`
* `template <typename... Args> void IO::`**`format`**`(const U8string& pattern, const Args&... args)`
* `template <typename... Args> void IO::`**`print`**`(const Args&... args)`

Output functions. The `write()` function attempts to write the given block of
bytes, returning the number successfully written (on most I/O streams this
will be less than `len` only if something goes wrong, but this is not
guaranteed). The `write_n()` function writes `n` identical bytes equal to `c`.
The `putc()` function writes one byte.

The `write_str()` function calls `write()` with the string contents, repeating
the call if an incomplete write occurs but is not reported as an error;
`write_line()` does the same thing, but will add a line feed if the string did
not already end with one (the empty version of `write_line()` simply writes a
line feed). The `format()` function calls `fmt(pattern,args...)` and writes
the resulting string to the output stream. The `print()` function formats each
argument using `to_str()`, writing them to the output stream with a space
between each pair of arguments, followed by a line feed.

* `void IO::`**`check`**`(const U8string& detail = "") const`
* `void IO::`**`clear_error`**`() noexcept`
* `std::error_code IO::`**`error`**`() const noexcept`

Error handling functions. The `error()` function returns any pending error
status; `check()` throws `std::system_error` if an error is set. An optional
argument can be added to `check()` to supply additional information for the
exception's error message, such as a file name. The error status set by an I/O
operation is retained only until the next I/O operation, which may change the
error status or clear it, or `clear_error()` is called.

* `Irange<line_iterator> IO::`**`lines`**`()`

Return iterators over the lines in a file. These are obtained by calling
`read_line()`.

* `bool IO::`**`ok`**`() const noexcept`

True if the file is open and there is no error set.

## C standard I/O ##

* `class` **`Cstdio`**`: public IO`

This class implements standard buffered I/O, using the `FILE*` based system
API in `<cstdio>` or `<stdio.h>`

* `using Cstdio::`**`handle_type`** `= FILE*`

The native file handle type.

* `explicit Cstdio::`**`Cstdio`**`(FILE* f, bool owner = true) noexcept`
* `explicit Cstdio::`**`Cstdio`**`(const File& f, mode m = mode::read_only)`
* `Cstdio::`**`Cstdio`**`(const File& f, const U8string& iomode)`

The first constructor wraps a `Cstdio` object around an existing file pointer;
if `owner` is true, the destructor will close the file. The second constructor
opens the file using one of the standard `IO::mode` values, defaulting to read
only. The third constructor opens the file using the same mode string used in
`fopen()`.

* `Cstdio::`**`Cstdio`**`() noexcept`
* `virtual Cstdio::`**`~Cstdio`**`() noexcept`
* `Cstdio::`**`Cstdio`**`(Cstdio&& io) noexcept`
* `Cstdio& Cstdio::`**`operator=`**`(Cstdio&& io) noexcept`

Other life cycle operations.

* `int Cstdio::`**`fd`**`() const noexcept`

Returns the underlying file descriptor.

* `FILE* Cstdio::`**`get`**`() const noexcept`
* `FILE* Cstdio::`**`release`**`() noexcept`

Return the native file handle. The `release()` function sets the internal
handle to null and abandons ownership of the stream.

* `void Cstdio::`**`ungetc`**`(char c)`

Calls the native `ungetc()` function to push one byte back into the stream
buffer.

* `static Cstdio Cstdio::`**`dev_null`**`() noexcept`
* `static Cstdio Cstdio::`**`std_input`**`() noexcept`
* `static Cstdio Cstdio::`**`std_output`**`() noexcept`
* `static Cstdio Cstdio::`**`std_error`**`() noexcept`

Standard streams.

## Unix file descriptor I/O ##

* `class` **`Fdio`**`: public IO`

This class implements standard Posix unbuffered I/O, using the file descriptor
based system API.

* `using Fdio::`**`handle_type`** `= int`

The native file descriptor type.

* `explicit Fdio::`**`Fdio`**`(int f, bool owner = true) noexcept`
* `explicit Fdio::`**`Fdio`**`(const File& f, mode m = mode::read_only)`
* `Fdio::`**`Fdio`**`(const File& f, int iomode, int perm = 0666)`

The first constructor wraps an `Fdio` object around an existing file
descriptor; if `owner` is true, the destructor will close the file. The second
constructor opens the file using one of the standard `IO::mode` values,
defaulting to read only. The third constructor opens the file using the same
flags used in the Posix `open()` function.

| Flag                  | Description                                    | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| ----                  | -----------                                    | -----    | -----    | ------   | -----    | ----     |
| _Access mode flags_   |                                                |          |          |          |          |          |
| `O_EXEC`              | Open for execute only                          | Posix    | &mdash;  | Cygwin   | &mdash;  | &mdash;  |
| `O_RDONLY`            | Open for reading only                          | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| `O_RDWR`              | Open for reading and writing                   | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| `O_SEARCH`            | Open directory for search only                 | Posix    | &mdash;  | Cygwin   | &mdash;  | &mdash;  |
| `O_WRONLY`            | Open for writing only                          | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| _Other Posix flags_   |                                                |          |          |          |          |          |
| `O_APPEND`            | Set append mode                                | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| `O_CLOEXEC`           | Implicitly set `FD_CLOEXEC`                    | Posix    | Apple    | Cygwin   | Linux    | &mdash;  |
| `O_CREAT`             | Create if file does not exist                  | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| `O_DIRECTORY`         | Must be a directory                            | Posix    | Apple    | Cygwin   | Linux    | &mdash;  |
| `O_DSYNC`             | Synchronize data                               | Posix    | &mdash;  | Cygwin   | Linux    | &mdash;  |
| `O_EXCL`              | Error if file already exists                   | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| `O_NOCTTY`            | Don't assign controlling terminal              | Posix    | Apple    | Cygwin   | Linux    | &mdash;  |
| `O_NOFOLLOW`          | Don't follow symlinks                          | Posix    | Apple    | Cygwin   | Linux    | &mdash;  |
| `O_NONBLOCK`          | No delay                                       | Posix    | Apple    | Cygwin   | Linux    | &mdash;  |
| `O_RSYNC`             | Synchronize read operations                    | Posix    | &mdash;  | Cygwin   | Linux    | &mdash;  |
| `O_SYNC`              | Synchronize write operations                   | Posix    | &mdash;  | Cygwin   | Linux    | &mdash;  |
| `O_TRUNC`             | Truncate to zero length                        | Posix    | Apple    | Cygwin   | Linux    | MSVC     |
| `O_TTY_INIT`          | Set `termios` structure if this is a terminal  | Posix    | &mdash;  | Cygwin   | &mdash;  | &mdash;  |
| _Non-standard flags_  |                                                |          |          |          |          |          |
| `O_ASYNC`             | Signal process group when data ready           | &mdash;  | Apple    | &mdash;  | Linux    | &mdash;  |
| `O_BINARY`            | Open in binary mode                            | &mdash;  | &mdash;  | Cygwin   | &mdash;  | MSVC     |
| `O_DIRECT`            | Minimize caching                               | &mdash;  | &mdash;  | Cygwin   | Linux    | &mdash;  |
| `O_EVTONLY`           | Descriptor for event notification only         | &mdash;  | Apple    | &mdash;  | &mdash;  | &mdash;  |
| `O_EXLOCK`            | Open with exclusive file lock                  | &mdash;  | Apple    | &mdash;  | Linux    | &mdash;  |
| `O_FSYNC`             | Synchronous writes                             | &mdash;  | Apple    | &mdash;  | Linux    | &mdash;  |
| `O_LARGEFILE`         | Allow files too large for a 32-bit offset      | &mdash;  | &mdash;  | &mdash;  | Linux    | &mdash;  |
| `O_NDELAY`            | Synonym for `O_NONBLOCK`                       | &mdash;  | Apple    | Cygwin   | Linux    | &mdash;  |
| `O_NOATIME`           | Don't update last access time                  | &mdash;  | &mdash;  | &mdash;  | Linux    | &mdash;  |
| `O_NOINHERIT`         | Prevent creation of a shared descriptor        | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |
| `O_PATH`              | Path-only handle, do not open file             | &mdash;  | &mdash;  | &mdash;  | Linux    | &mdash;  |
| `O_RANDOM`            | Optimize for random access                     | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |
| `O_SEQUENTIAL`        | Optimize for sequential access                 | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |
| `O_SHLOCK`            | Open with shared file lock                     | &mdash;  | Apple    | &mdash;  | Linux    | &mdash;  |
| `O_SHORT_LIVED`       | If possible do not flush to disk               | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |
| `O_SYMLINK`           | Allow opening a symlink                        | &mdash;  | Apple    | &mdash;  | &mdash;  | &mdash;  |
| `O_TEMPORARY`         | Delete file when last descriptor is closed     | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |
| `O_TMPFILE`           | Delete file when last descriptor is closed     | &mdash;  | &mdash;  | &mdash;  | Linux    | &mdash;  |
| `O_TEXT`              | Open in text mode                              | &mdash;  | &mdash;  | Cygwin   | &mdash;  | MSVC     |
| `O_U8TEXT`            | Open in UTF-8 text mode                        | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |
| `O_U16TEXT`           | Open in UTF-16 text mode                       | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |
| `O_WTEXT`             | Open in UTF-16 text mode, check for BOM        | &mdash;  | &mdash;  | &mdash;  | &mdash;  | MSVC     |

* `Fdio::`**`Fdio`**`() noexcept`
* `virtual Fdio::`**`~Fdio`**`() noexcept`
* `Fdio::`**`Fdio`**`(Fdio&& io) noexcept`
* `Fdio& Fdio::`**`operator=`**`(Fdio&& io) noexcept`

Other life cycle operations.

* `Fdio Fdio::`**`dup`**`() noexcept`
* `Fdio Fdio::`**`dup`**`(int f) noexcept`

These call the native `dup()` or `dup2()` functions to duplicate a file
descriptor.

* `int Fdio::`**`get`**`() const noexcept`
* `int Fdio::`**`release`**`() noexcept`

Return the native file descriptor. The `release()` function sets the internal
descriptor to -1 and abandons ownership of the stream.

* `static Fdio Fdio::`**`dev_null`**`() noexcept`
* `static Fdio Fdio::`**`std_input`**`() noexcept`
* `static Fdio Fdio::`**`std_output`**`() noexcept`
* `static Fdio Fdio::`**`std_error`**`() noexcept`

Standard streams.

* `static std::pair<Fdio, Fdio> Fdio::`**`pipe`**`(size_t winmem = 1024)`

Opens a pipe (by calling the native `pipe()` function or the equivalent),
returning the read and write ends of the pipe. On Windows a size limit is
required when creating a pipe; this argument is ignored on Unix.

## Windows file handle I/O ##

* `class` **`Winio`**`: public IO`

This class implements I/O using the Win32 system API.

* `using Winio::`**`handle_type`** `= HANDLE`

The native file handle type.

* `explicit Winio::`**`Winio`**`(void* f, bool owner = true) noexcept`
* `explicit Winio::`**`Winio`**`(const File& f, mode m = mode::read_only)`
* `Winio::`**`Winio`**`(const File& f, uint32_t desired_access, uint32_t share_mode, LPSECURITY_ATTRIBUTES security_attributes, uint32_t creation_disposition, uint32_t flags_and_attributes = 0, HANDLE template_file = nullptr)`

The first constructor wraps a `Winio` object around an existing file handle;
if `owner` is true, the destructor will close the file. The second constructor
opens the file using one of the standard `IO::mode` values, defaulting to read
only. The third constructor opens the file using the same arguments used in
`CreateFile()`.

* `Winio::`**`Winio`**`() noexcept`
* `virtual Winio::`**`~Winio`**`() noexcept`
* `Winio::`**`Winio`**`(Winio&& io) noexcept`
* `Winio& Winio::`**`operator=`**`(Winio&& io) noexcept`

Other life cycle operations.

* `void* Winio::`**`get`**`() const noexcept`
* `void* Winio::`**`release`**`() noexcept`

Return the native file handle. The `release()` function sets the internal
handle to null and abandons ownership of the stream.

* `static Winio Winio::`**`dev_null`**`() noexcept`
* `static Winio Winio::`**`std_input`**`() noexcept`
* `static Winio Winio::`**`std_output`**`() noexcept`
* `static Winio Winio::`**`std_error`**`() noexcept`

Standard streams.

## String buffer ##

* `class` **`StringBuffer`**`: public IO`

This class implements file-like read and write operations on a string buffer.
The string used as a buffer can be supplied to the constructor, or created
internally. Writing to the `StringBuffer` appends data to the end of the
buffer; reading removes it from the beginning. Seeking forward will discard
bytes from the read end of the buffer; seeking backward has no effect.

The actual contents of the buffer are managed internally; read and write
operations move internal pointers within the string, but to avoid excessive
copying, data will only be trimmed from the whole buffer when heuristics
suggest that it is growing too large. Calling `flush()` will force an
immediate trim to contain only the active portion of the buffer. Calling
`close()` removes all data (equivalent to `clear()`; the `StringBuffer` does
not actually maintain an open/closed state).

* `StringBuffer::`**`StringBuffer`**`() noexcept`
* `explicit StringBuffer::`**`StringBuffer`**`(std::string& s)`
* `virtual StringBuffer::`**`~StringBuffer`**`() noexcept`
* `StringBuffer::`**`StringBuffer`**`(StringBuffer&& src) noexcept`
* `StringBuffer& StringBuffer::`**`operator=`**`(StringBuffer&& src) noexcept`

Life cycle operations. A moved-from `StringBuffer` is empty.

* `char* StringBuffer::`**`begin`**`() noexcept`
* `const char* StringBuffer::`**`begin`**`() const noexcept`
* `char* StringBuffer::`**`end`**`() noexcept`
* `const char* StringBuffer::`**`end`**`() const noexcept`

These return pointers to the ends of the currently valid part of the buffer.

* `void StringBuffer::`**`clear`**`() noexcept`

Empties the buffer.

* `bool StringBuffer::`**`empty`**`() const noexcept`

True if the buffer is empty.

* `size_t StringBuffer::`**`size`**`() const noexcept`

The number of bytes currently in the valid part of the buffer.

* `std::string StringBuffer::`**`str`**`() const`

Returns the buffer contents as a string.
