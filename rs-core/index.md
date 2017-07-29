# RS Core Library #

### Common utilities library by Ross Smith ###

* _Utility modules_
    * [`"rs-core/common.hpp"`](common.html) - Common utilities.
    * [`"rs-core/container.hpp"`](container.html) - Containers.
    * [`"rs-core/float.hpp"`](float.html) - Floating point utilities.
    * [`"rs-core/io.hpp"`](io.html) - I/O utilities.
    * [`"rs-core/kwargs.hpp"`](kwargs.html) - Keyword arguments.
    * [`"rs-core/meta.hpp"`](meta.html) - Template metaprogramming.
    * [`"rs-core/random.hpp"`](random.html) - Random numbers.
    * [`"rs-core/string.hpp"`](string.html) - String algorithms.
    * [`"rs-core/thread.hpp"`](thread.html) - Threads.
    * [`"rs-core/time.hpp"`](time.html) - Time and date algorithms.
    * [`"rs-core/uuid.hpp"`](uuid.html) - UUID.
    * [`"rs-core/vector.hpp"`](vector.html) - Vectors, matrices, and quaternions.
* _Testing modules_
    * [`"rs-core/unit-test.hpp"`](unit-test.html) - Unit testing.

This is a header-only library that contains the utilities that I've found a
common need for in multiple projects.

Everything in this library is in `namespace RS`, and can be used either by
explicit namespace qualification or via `using namespace RS`.

Since this library is header-only, it is not necessary to build anything
before using it with any other library. The makefile included here exists
mainly to be referenced by inclusion in other projects' makefiles; it can also
be used to run the library's own unit tests.

The library is written in C++14 (more or less); you will need an up-to-date
C++ compiler, probably a recent version of Clang or GCC. I developed it mainly
on macOS, currently using Clang 9.0, as well as testing with GCC 6/7 on
Windows. All test builds are made using strict compilation mode (`-Wall
-Wextra -Werror`). It will not work with any existing release of Microsoft
Visual C++.

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
