# File Class #

By Ross Smith

* `#include "rs-core/file.hpp"`

## Contents ##

[TOC]

## Class File ##

* `class` **`File`**

A class holding a file name. Path delimiters are normalized to slashes on all
systems (the original backslash delimited form can be recovered on Windows
through the `native()` function).

### Constants ###

* `static constexpr uint32_t File::`**`append`**
* `static constexpr uint32_t File::`**`recurse`**
* `static constexpr uint32_t File::`**`require`**

Flags used to control the behaviour of some of the member functions. (These
are bitmasks to allow for future expansion that may require combinations of
flags, but in the current version no functions accepts more than one of
these.)

### Life cycle operations ###

* `File::`**`File`**`() noexcept`

Default constructor. This sets the file name to an empty string.

* `File::`**`File`**`(const std::string& name)`
* `File::`**`File`**`(const char* name)`
* `File::`**`File`**`(const std::wstring& name)` _- Windows only_
* `File::`**`File`**`(const wchar_t* name)` _- Windows only_

Constructors from a file name. On Unix, any string will be accepted as a file
name with no validity checking. On Windows, backslashes are replaced with
slashes; if the name is supplied as a wide string, it will be converted to
UTF-8, with the standard replacement character (`U+FFFD`) replacing any
invalid Unicode. (File names on Windows in their native form are nominally
UTF-16 but can contain invalid encoding; no attempt is made to support such
names in this class.)

* `File::`**`~File`**`() noexcept`
* `File::`**`File`**`(const File& f)`
* `File::`**`File`**`(File&& f) noexcept`
* `File& File::`**`operator=`**`(const File& f)`
* `File& File::`**`operator=`**`(File&& f) noexcept`

Other life cycle operations.

### File name operations ###

* `const std::string& File::`**`name`**`() const`
* `const char* File::`**`c_name`**`() const noexcept`

Return the file name.

* `bool File::`**`empty`**`() const noexcept`

True if the file name is empty.

* `bool File::`**`is_absolute`**`() const noexcept`

True if this is an absolute path. On WIndows this will return false for "drive
absolute" (e.g. `"\foo"`) and "drive relative" (e.g. `"C:foo"`) paths.

* `File File::`**`parent`**`() const`
* `File File::`**`leaf`**`() const`

Split the file name at the last slash, into a parent directory and a leaf name (the last part of the name, local to the directory).
For example, `"/foo/bar/hello.txt"` will be split into `"/foo/bar"` and `"hello.txt"`.

* `std::string File::`**`base`**`() const`
* `std::string File::`**`ext`**`() const`

Split the file's leaf name at the last dot, into base and extension parts; the
parent directory path is discarded. The extension includes the delimiting dot;
if the leaf name contains no dot, the base part is the whole leaf name and the
extension is empty. For example, `"/foo/bar/hello.txt"` will be split into
`"hello"` and `".txt"`. A leading dot is not counted as an extension
delimiter.

* `[see below] File::`**`native`**`() const`

Returns the operating system's native form of the name. On Windows (including
Cygwin), this returns a wide string containing the backslash delimited form of
the name. On Unix systems other than Cygwin, this just returns the same string
as `name()`.

* `File& File::`**`operator/=`**`(const File& rhs)`
* `File` **`operator/`**`(const File& lhs, const File& rhs)`

Path concatenation operators. If the RHS is an absolute path, the LHS is
ignored and the RHS returned unchanged. Otherwise, the returned path is the
result of applying the RHS path relative to the LHS.

* `bool` **`operator==`**`(const File& lhs, const File& rhs) noexcept`
* `bool` **`operator!=`**`(const File& lhs, const File& rhs) noexcept`
* `bool` **`operator<`**`(const File& lhs, const File& rhs) noexcept`
* `bool` **`operator>`**`(const File& lhs, const File& rhs) noexcept`
* `bool` **`operator<=`**`(const File& lhs, const File& rhs) noexcept`
* `bool` **`operator>=`**`(const File& lhs, const File& rhs) noexcept`

Comparison operators. These perform simple string comparison on the stored
path. No attempt is made to allow for case insensitive names.

* `std::ostream&` **`operator<<`**`(std::ostream& out, const File& f)`
* `std::string` **`to_str`**`(const File& f)`

Formatting a `File` object simply returns the stored path.

### File system query operations ###

* `bool File::`**`exists`**`() const noexcept`

True if the file exists. This will give a false negative if the file is not
detectable by the calling process due to access permissions.

* `bool File::`**`is_directory`**`() const noexcept`

True if the file exists and is a directory, or a symlink to a directory.

* `bool File::`**`is_symlink`**`() const noexcept`

True if the file is a symlink. I haven't tried to support the symlink-like
concepts on NTFS; this simply always returns false on Windows.

* `std::vector<File> File::`**`list`**`() const`

If the file is a directory, this returns a list of the files in it; otherwise
it returns an empty list. The files may be listed in any order.

* `uint64_t File::`**`size`**`() const`

Returns the size of the file, if it exists and its size can be determined. The
return value is unspecified if the file is a directory.

### File system update operations ###

All of these functions will throw `std::system_error` if the underlying system
API reports an error.

* `void File::`**`mkdir`**`(uint32_t flags = 0) const`

Create the file as a directory. If the `File::recurse` flag is set, this will
also create any missing parent directories (like `mkdir -p`). If the directory
already exists, this will do nothing. If the file exists but is not a
directory, behaviour depends on the operating system; it may fail or overwrite
the file.

* `void File::`**`move_to`**`(const File& dst) const`

Rename the file. This will fail if the file does not exist. Behaviour is
operating system dependent if the target file already exists (it may overwrite
the file or fail), or if the old and new paths are on different physical file
systems (on most systems this will fail).

* `void File::`**`remove`**`(uint32_t flags = 0) const`

Delete the file. This will do nothing if the file does not exist to begin
with. If the file is a non-empty directory, this will delete recursively (not
following symlinks) if the `File::recurse` flag is set; otherwise it will
fail.

### I/O operations ###

* `std::string File::`**`load`**`(size_t limit = npos, uint32_t flags = 0) const`

Read the contents of the file into a string. Optionally, a maximum size can be
specified. By default, if the file does not exist or cannot be read (e.g. a
directory), this will simply return an empty string; if the `File::require`
flag is set, it will throw `std::system_error` if anything goes wrong.

* `void File::`**`save`**`(const void* ptr, size_t len, uint32_t flags = 0) const`
* `void File::`**`save`**`(const std::string& content, uint32_t flags = 0) const`

Write the contents of a string or a block of memory to a file. By default,
this will overwrite any existing file with the same name, if possible; if the
`File::append` flag is set, the new data will be appended to any existing file
instead (the `append` flag has no effect if the file does not already exist).
This will throw `std::system_error` if anything goes wrong.

### Standard locations ###

* `void File::`**`set_cwd`**`() const`
* `static File File::`**`cwd`**`()`

Query or set the current working directory.

* `static File File::`**`user_home`**`()`
* `static File File::`**`user_documents`**`()`
* `static File File::`**`user_settings`**`()`

Query some standard directory locations. On Unix, these will check the value
of the `HOME` environment variable first, and only consult the system API if
that fails.

Typical values on common operating systems:

Function            | Linux               | Mac                                   | Windows
--------            | -----               | ---                                   | -------
`user_home()`       | `/home/<username>`  | `/Users/<username>`                   | `C:/Users/<username>`
`user_documents()`  | `<home>/Documents`  | `<home>/Documents`                    | `<home>/Documents`
`user_settings()`   | `<home>/.config`    | `<home>/Library/Application Support`  | `<home>/AppData/Roaming`
