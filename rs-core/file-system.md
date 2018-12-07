# [RS Library](index.html): File System Information #

By Ross Smith

* `#include "rs-core/file-system.hpp"`

## Contents ##

* [TOC]

## Standard directories ##

* `enum class` **`SystemPath`**
    * `SystemPath::`**`apps`** _- System-wide application install directory_
    * `SystemPath::`**`os`** _- Base directory for operating system files_
* `enum class` **`UserPath`**
    * `UserPath::`**`apps`** _- User-specific application install directory_
    * `UserPath::`**`cache`** _- User-specific local application data cache_
    * `UserPath::`**`desktop`** _- User's desktop_
    * `UserPath::`**`documents`** _- User's documents_
    * `UserPath::`**`downloads`** _- User's downloads_
    * `UserPath::`**`home`** _- User's home directory_
    * `UserPath::`**`movies`** _- User's movies_
    * `UserPath::`**`music`** _- User's music_
    * `UserPath::`**`pictures`** _- User's pictures_
    * `UserPath::`**`settings`** _- User-specific application settings_
* `static Unicorn::Path` **`std_path`**`(SystemPath id)`
* `static Unicorn::Path` **`std_path`**`(UserPath id)`

Query some of the standard locations on the host file system. The `SystemPath`
enumeration values refer to system-wide standard directories, while the
`UserPath` values refer to User-specific directories. These functions will
throw `std::invalid_argument` if the argument is not one of the listed
enumeration values, or `std::system_error` if anything goes wrong with a
system API query (which probably means the operating system is incorrectly
configured, or the user associated with the calling process has been
incorrectly set up).

Typical locations on common operating systems:

Directory              | Interpretation                               | Linux               | Mac                                   | Windows
---------              | --------------                               | -----               | ---                                   | -------
`SystemPath::apps`     | System-wide application install directory    | `/usr/local/bin`    | `/Applications`                       | `C:\Program Files`
`SystemPath::os`       | Base directory for operating system files    | `/`                 | `/`                                   | `C:\Windows`
`UserPath::apps`       | User-specific application install directory  | `<home>/bin`        | `<home>/Applications`                 | `<home>\AppData\Local\Programs`
`UserPath::cache`      | User-specific local application data cache   | `<home>/.cache`     | `<home>/Library/Caches`               | `<home>\AppData\Local`
`UserPath::desktop`    | User's desktop                               | `<home>/Desktop`    | `<home>/Desktop`                      | `<home>\Desktop`
`UserPath::documents`  | User's documents                             | `<home>/Documents`  | `<home>/Documents`                    | `<home>\Documents`
`UserPath::downloads`  | User's downloads                             | `<home>/Downloads`  | `<home>/Downloads`                    | `<home>\Downloads`
`UserPath::home`       | User's home directory                        | `/home/<user>`      | `/Users/<user>`                       | `C:\Users\<user>`
`UserPath::movies`     | User's movies                                | `<home>/Videos`     | `<home>/Movies`                       | `<home>\Videos`
`UserPath::music`      | User's music                                 | `<home>/Music`      | `<home>/Music`                        | `<home>\Music`
`UserPath::pictures`   | User's pictures                              | `<home>/Pictures`   | `<home>/Pictures`                     | `<home>\Pictures`
`UserPath::settings`   | User-specific application settings           | `<home>/.config`    | `<home>/Library/Application Support`  | `<home>\AppData\Roaming`

## Search path ##

(Unfortunately the name `SearchPath` is defined as a macro on Windows.)

* `class` **`PathList`**`: public std::vector<Unicorn::Path>`

A list of directories in which files are expected to be found.

* `using PathList::`**`flag_type`** `= [unsigned integer]`

Member type.

* `static constexpr char PathList::`**`delimiter`** `= [':' on Unix, ';' on Windows]`

The delimiter used to separate directory names in a search path.

* `static constexpr flag_type PathList::`**`env`**        _- The string is the name of an environment variable_
* `static constexpr flag_type PathList::`**`no_dups`**    _- Remove duplicate entries_
* `static constexpr flag_type PathList::`**`only_dirs`**  _- Remove entries that do not exist or are not directories_

Flags used in search path functions.

* `PathList::`**`PathList`**`()`
* `explicit PathList::`**`PathList`**`(Uview text, flag_type flags = 0)`
* `PathList::`**`~PathList`**`() noexcept`
* `PathList::`**`PathList`**`(const PathList& pl)`
* `PathList::`**`PathList`**`(PathList&& pl) noexcept`
* `PathList& PathList::`**`operator=`**`(const PathList& pl)`
* `PathList& PathList::`**`operator=`**`(PathList&& pl) noexcept`

Life cycle functions. The default constructor creates an empty search path.
The second constructor accepts a string that can be either an explicit list of
directories, or (if the `env` flag is set) the name of an environment variable
from which a search path will be read. If the `no_dups` flag is set, duplicate
entries will be removed from the list (this only checks for identical
directory names; it does not resolve the names to check if different names may
refer to the same file). If the `only_dirs` flag is set, only paths that refer
to existing directories are included.

* `bool PathList::`**`contains`**`(const Unicorn::Path& dir) const noexcept`

True if the directory is in the search path (this only checks for an exact
string match on the name).

* `void PathList::`**`erase_all`**`(const Unicorn::Path& dir)`

Remove all matching directories.

* `Unicorn::Path PathList::`**`find`**`(Uview name)`
* `Unicorn::Path PathList::`**`find`**`(Uview name, bool case_sensitive)`
* `Unicorn::Path PathList::`**`find`**`(const Unicorn::Regex& pattern)`

Find the first matching file in the search path. Only the immediate children
of each directory in the path are checked; a target name that contains a
directory delimiter will never match anything. Search path entries that do not
exist, or are not directories, are skipped. Case sensitivity follows the
operating system standard by default. The version that takes a regex performs
a match against the leaf name only.

* `std::vector<Unicorn::Path> PathList::`**`find_all`**`(Uview name)`
* `std::vector<Unicorn::Path> PathList::`**`find_all`**`(Uview name, bool case_sensitive)`
* `std::vector<Unicorn::Path> PathList::`**`find_all`**`(const Unicorn::Regex& pattern)`

Find all matching files in the search path. Matching rules are the same as
`find()`. The returned vector may contain duplicate entries if the search path
contained duplicate directories.

* `void PathList::`**`prune`**`(flag_type flags = no_dups | only_dirs)`

Apply the `no_dups` or `only_dirs` flags (both by default) to the list.

* `std::string PathList::`**`str`**`() const`
* `std::string` **`to_str`**`(const PathList& pl)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const PathList& pl)`

Return the search path as a string, following the conventions of the host OS.

* `static PathList PathList::`**`path`**`(flag_type flags = 0)`

Equivalent to `PathList("PATH",PathList::env)`. The `no_dups` and `only_dirs`
flags have their normal effect.
