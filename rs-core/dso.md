# Shared Library #

by Ross Smith

* `#include "rs-core/dso.hpp"`

## Contents ##

[TOC]

## Class Dso ##

* `class` **`Dso`**

This class loads a dynamic library when constructed. Symbols from the library
can be loaded at run time.

* `using Dso::`**`flag_type`** `= [int on Unix, uint32_t on Windows]`
* `using Dso::`**`handle_type`** `= [void* on Unix, HMODULE on Windows]`

Member types.

* _Posix `dlopen()` flags_
    * `static constexpr flag_type Dso::`**`lazy`**                `= RTLD_LAZY`                            _(Relocations may be delayed)_
    * `static constexpr flag_type Dso::`**`now`**                 `= RTLD_NOW`                             _(Relocations are performed immediately)_
    * `static constexpr flag_type Dso::`**`global`**              `= RTLD_GLOBAL`                          _(Symbols are available when relocating other libraries)_
    * `static constexpr flag_type Dso::`**`local`**               `= RTLD_LOCAL`                           _(Symbols are not visible)_
* _Apple specific `dlopen()` flags_
    * `static constexpr flag_type Dso::`**`noload`**              `= RTLD_NOLOAD`                          _(Do not load, succeed only if already loaded)_
    * `static constexpr flag_type Dso::`**`nodelete`**            `= RTLD_NODELETE`                        _(Never unload)_
    * `static constexpr flag_type Dso::`**`first`**               `= RTLD_FIRST`                           _(Search only this library for symbols)_
* _Windows `LoadLibrary()` flags_
    * `static constexpr flag_type Dso::`**`ignore_authz`**        `= LOAD_IGNORE_CODE_AUTHZ_LEVEL`         _(Do not check software restriction policies)_
    * `static constexpr flag_type Dso::`**`datafile`**            `= LOAD_LIBRARY_AS_DATAFILE`             _(Map address space as data, do not execute)_
    * `static constexpr flag_type Dso::`**`datafile_exclusive`**  `= LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE`   _(Map address space as data, with exclusive write access)_
    * `static constexpr flag_type Dso::`**`image_resource`**      `= LOAD_LIBRARY_AS_IMAGE_RESOURCE`       _(Map address space as image, do not execute)_
    * `static constexpr flag_type Dso::`**`search_application`**  `= LOAD_LIBRARY_SEARCH_APPLICATION_DIR`  _(Search application directory only)_
    * `static constexpr flag_type Dso::`**`search_default`**      `= LOAD_LIBRARY_SEARCH_DEFAULT_DIRS`     _(Search application, system, and user-added directories)_
    * `static constexpr flag_type Dso::`**`search_dll`**          `= LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR`     _(Search DLL directory for its dependencies)_
    * `static constexpr flag_type Dso::`**`search_system32`**     `= LOAD_LIBRARY_SEARCH_SYSTEM32`         _(Search system directory only)_
    * `static constexpr flag_type Dso::`**`search_user`**         `= LOAD_LIBRARY_SEARCH_USER_DIRS`        _(Search user-added directories only)_
    * `static constexpr flag_type Dso::`**`altered_search`**      `= LOAD_WITH_ALTERED_SEARCH_PATH`        _(Use alternative standard search path)_

Flags passed to the constructor and related functions, controlling the way the
library is loaded. Flags not relevant to the target system are set to zero.

* `Dso::`**`Dso`**`() noexcept`
* `explicit Dso::`**`Dso`**`(const File& file, flag_type flags = 0)`
* `Dso::`**`~Dso`**`() noexcept`
* `Dso::`**`Dso`**`(Dso&&) noexcept`
* `Dso& Dso::`**`operator=`**`(Dso&&) noexcept`

Life cycle functions. The default constructor does not load anything;
`symbol()` will always fail. The second constructor attempts to load the named
library. The search strategy depends on the format of the file name, and may
vary slightly between systems; it may also be modified by some of the flags.

File name format  | Search strategy
----------------  | ---------------
Empty string      | The executable for the calling process is loaded
Unqualified name  | The standard dynamic library search path is used
Absolute path     | Only that exact file is checked
Relative path     | Behaviour is implementation defined

If the library is not found, the constructor will throw `std::system_error`.

* `explicit Dso::`**`operator bool`**`() const noexcept`

True if the object holds a handle on a valid library (this is false for
default constructed and moved from objects).

* `handle_type Dso::`**`get`**`() const noexcept`

Returns the native operating system handle on the library. This will be null
if `bool(*this)` is false.

* `File Dso::`**`file`**`() const`

Returns the file name (as supplied to the constructor; this does not attempt
to determine the actual absolute path to the loaded library).

* `template <typename Sym> Sym Dso::`**`symbol`**`(const Ustring& name)`
* `template <typename Sym> bool Dso::`**`symbol`**`(const Ustring& name, Sym& sym) noexcept`

Load a symbol from the library. The first version takes a symbol type
explicitly, and returns the symbol, or throws `std::system_error` if the
symbol is not found. The second version returns the symbol by reference,
returns a flag indicating whether or not the symbol was successfully loaded,
and does not throw anything.

It is up to the caller to ensure that the symbol type supplied matches that of
the actual symbol in the library; there is no way to check that through the
standard dynamic library API on any supported system. Behaviour is undefined
if the types do not match.

* `static Dso Dso::`**`search`**`(const Strings& names, flag_type flags = 0)`
* `template <typename... Args> static Dso Dso::`**`search`**`(Args... args)`

Search for a library that may have any of several names. Any number of names
may be passed, either as variadic arguments compatible with string views, or a
vector of strings; either can optionally be followed by a bitmask of flags. If
a name is an unqualified file name, in addition to the exact name supplied, it
will also be tried with the standard dynamic library suffix for the target
system (`".dll"`, `".dylib"`, or `".so"`), and with a `"lib"` prefix (on
Cygwin the `"cyg"` prefix will also be tried). A `std::system_error` exception
will be thrown if none of these work.

* `static Dso Dso::`**`self`**`(flag_type flags = 0)`

Obtains a handle on the executable of the current process (equivalent to
`Dso("")`).
