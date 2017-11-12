# RS Core Library #

### Common utilities library by Ross Smith ###

* _General utilities_
    * [`"rs-core/common.hpp"`](common.html) - Common utilities
    * [`"rs-core/kwargs.hpp"`](kwargs.html) - Keyword arguments
    * [`"rs-core/optional.hpp"`](optional.html) - Optional type
    * [`"rs-core/variant.hpp"`](variant.html) - Variant type
* _Compression_
    * [`"rs-core/zlib.hpp"`](zlib.html) - Zlib compression
* _Concurrency_
    * [`"rs-core/channel.hpp"`](channel.html) - Message dispatch
    * [`"rs-core/ipc.hpp"`](ipc.html) - Interprocess communication
    * [`"rs-core/process.hpp"`](process.html) - Process control
    * [`"rs-core/signal.hpp"`](signal.html) - Signal handling
* _Containers_
    * [`"rs-core/array-map.hpp"`](array-map.html) - Sequence based associative array
    * [`"rs-core/blob.hpp"`](blob.html) - Binary large object
    * [`"rs-core/compact-array.hpp"`](compact-array.html) - Compact array
    * [`"rs-core/grid.hpp"`](grid.html) - Multidimensional array
    * [`"rs-core/index-table.hpp"`](index-table.html) - Multi-index map
    * [`"rs-core/scale-map.hpp"`](scale-map.html) - Interpolating associative array
    * [`"rs-core/stack.hpp"`](stack.html) - Deterministic stack
    * [`"rs-core/topological-order.hpp"`](topological-order.html) - Topological order
* _Functional programming_
    * [`"rs-core/algorithm.hpp"`](algorithm.html) -- Algorithms
    * [`"rs-core/cache.hpp"`](cache.html) -- Function memoization
    * [`"rs-core/meta.hpp"`](meta.html) - Template metaprogramming
* _Input/output_
    * [`"rs-core/file.hpp"`](file.html) - File class
    * [`"rs-core/io.hpp"`](io.html) - I/O utilities
    * [`"rs-core/terminal.hpp"`](terminal.html) - Terminal I/O
* _Networking_
    * [`"rs-core/net.hpp"`](net.html) - TCP/IP networking
* _Numerics_
    * [`"rs-core/float.hpp"`](float.html) - Floating point utilities
    * [`"rs-core/mp-integer.hpp"`](mp-integer.html) -- Multiple precision integers
    * [`"rs-core/random.hpp"`](random.html) - Random numbers
    * [`"rs-core/rational.hpp"`](rational.html) -- Rational numbers
    * [`"rs-core/statistics.hpp"`](statistics.html) -- Statistics
    * [`"rs-core/vector.hpp"`](vector.html) - Vectors, matrices, and quaternions
* _Text processing_
    * [`"rs-core/digest.hpp"`](digest.html) -- Message digest algorithms
    * [`"rs-core/encoding.hpp"`](encoding.html) -- Binary and text encodings
    * [`"rs-core/string.hpp"`](string.html) - String algorithms
    * [`"rs-core/table.hpp"`](table.html) - Simple table layout
    * [`"rs-core/uuid.hpp"`](uuid.html) - UUID
* _Time and date_
    * [`"rs-core/time.hpp"`](time.html) - Time and date algorithms
* _Unit testing_
    * [`"rs-core/unit-test.hpp"`](unit-test.html) - Unit testing

This is a header-only library that contains the utilities that I've found a
common need for in multiple projects.

Everything in this library is in `namespace RS`, and can be used either by
explicit namespace qualification or via `using namespace RS`.

Since this library is header-only, it is not necessary to build anything
before using it with any other library. The makefile included here exists
mainly to be referenced by inclusion in other projects' makefiles; it can also
be used to run the library's own unit tests.

The library is written in C++14 (more or less); you will need an up-to-date
C++ compiler. It has been tested with:

* Apple Clang/Xcode 9 on MacOS
* GCC 6 and 7 on Linux and Cygwin
* Microsoft Visual Studio 2017 on Windows

All test builds are made using a strict diagnostic mode (`-Wall -Wextra
-Werror` on Clang and GCC, `/W4 /WX /permissive-` on MSVC).

Because I'm primarily targeting a short list of compilers rather than a
specific edition of the C++ standard, the code makes a few assumptions that
are not guaranteed by the standard but are reliably true on all the system I
care about. These include:

* `char` is always 8 bits.
* `float` and `double` are IEEE 32 and 64 bit types.
* Symbols from the C library are always in the global namespace.

On Microsoft Windows builds, including the core header will bring in the whole
of `<windows.h>`. This is annoying but unfortunately unavoidable for a library
that needs to make calls to system APIs while remaining header-only. (Under
Cygwin, this will only happen if `_WIN32` is defined.)

Rebuilding the documentation (`make doc`) requires Python 3.4+ and the
[Markdown module](https://pypi.python.org/pypi/Markdown). The scripts expect
Python 3 to be callable as `python3`.
