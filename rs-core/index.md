# RS Core Library #

### Common utilities library by Ross Smith ###

* _General utilities_
    * [`"rs-core/common.hpp"`](common.html) - Common utilities
    * [`"rs-core/meta.hpp"`](meta.html) - Template metaprogramming
    * [`"rs-core/optional.hpp"`](optional.html) - Optional type
    * [`"rs-core/variant.hpp"`](variant.html) - Variant type
* _Compression_
    * [`"rs-core/zlib.hpp"`](zlib.html) - Zlib compression
* _Concurrency_
    * [`"rs-core/channel.hpp"`](channel.html) - Message dispatch
    * [`"rs-core/ipc.hpp"`](ipc.html) - Interprocess communication
    * [`"rs-core/process.hpp"`](process.html) - Process control
    * [`"rs-core/signal.hpp"`](signal.html) - Signal handling
    * [`"rs-core/thread-pool.hpp"`](thread-pool.html) - Thread pool
* _Containers_
    * [`"rs-core/array-map.hpp"`](array-map.html) - Sequence based associative array
    * [`"rs-core/blob.hpp"`](blob.html) - Binary large object
    * [`"rs-core/bounded-array.hpp"`](bounded-array.html) - Stack-based array with bounded size
    * [`"rs-core/compact-array.hpp"`](compact-array.html) - Array optimized for small sizes
    * [`"rs-core/grid.hpp"`](grid.html) - Multidimensional array
    * [`"rs-core/index-table.hpp"`](index-table.html) - Multi-index map
    * [`"rs-core/mirror-map.hpp"`](mirror-map.html) - Map with two-way lookup
    * [`"rs-core/ordered-map.hpp"`](ordered-map.html) - Map with insertion order preservation
    * [`"rs-core/ring-buffer.hpp"`](ring-buffer.html) - Ring buffer
    * [`"rs-core/scale-map.hpp"`](scale-map.html) - Interpolating associative array
    * [`"rs-core/stack.hpp"`](stack.html) - Deterministic stack
    * [`"rs-core/topological-order.hpp"`](topological-order.html) - Topological order
* _Functional programming_
    * [`"rs-core/algorithm.hpp"`](algorithm.html) -- Miscellaneous algorithms
    * [`"rs-core/cache.hpp"`](cache.html) -- Function memoization
    * [`"rs-core/range.hpp"`](range.html) -- Range algorithms
* _Input/output_
    * [`"rs-core/file-system.hpp"`](file-system.html) - File system information
    * [`"rs-core/io.hpp"`](io.html) - I/O utilities
    * [`"rs-core/net.hpp"`](net.html) - TCP/IP networking
    * [`"rs-core/terminal.hpp"`](terminal.html) - Terminal I/O
* _Numerics_
    * [`"rs-core/fixed-binary.hpp"`](fixed-binary.html) - Fixed precision integers
    * [`"rs-core/float.hpp"`](float.html) - Floating point utilities
    * [`"rs-core/mp-integer.hpp"`](mp-integer.html) -- Multiple precision integers
    * [`"rs-core/random.hpp"`](random.html) - Random numbers
    * [`"rs-core/rational.hpp"`](rational.html) -- Rational numbers
    * [`"rs-core/statistics.hpp"`](statistics.html) -- Statistics
    * [`"rs-core/vector.hpp"`](vector.html) - Vectors, matrices, and quaternions
* _Text processing_
    * [`"rs-core/digest.hpp"`](digest.html) -- Message digest algorithms
    * [`"rs-core/encoding.hpp"`](encoding.html) -- Binary and text encodings
    * [`"rs-core/serial.hpp"`](serial.html) -- JSON serialization
    * [`"rs-core/string.hpp"`](string.html) - String algorithms
    * [`"rs-core/table.hpp"`](table.html) - Simple table layout
    * [`"rs-core/url.hpp"`](url.html) - URL
    * [`"rs-core/uuid.hpp"`](uuid.html) - UUID
* _System facilities_
    * [`"rs-core/dso.hpp"`](dso.html) - Dynamic library
* _Time and date_
    * [`"rs-core/time.hpp"`](time.html) - Time and date algorithms
* _Unit testing_
    * [`"rs-core/unit-test.hpp"`](unit-test.html) - Unit testing

This library contains the utilities that I've found a common need for in
multiple projects.

Everything in this library (except 3rd party code) is in `namespace RS`, and
can be used either by explicit namespace qualification or via `using namespace
RS`.

There's a makefile but it's probably not as portable as it should be.

The library is written in C++14 (more or less); you will need an up-to-date
C++ compiler. It has been tested with:

* Apple Clang/Xcode 9 on MacOS
* GCC 7 on Linux and Cygwin
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

Rebuilding the documentation (`make doc`) requires Python 3.4+ and the
[Markdown module](https://pypi.python.org/pypi/Markdown). The scripts expect
Python 3 to be callable as `python3`.
