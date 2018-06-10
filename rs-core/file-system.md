# [RS Library](index.html): File System Information #

By Ross Smith

* `#include "rs-core/file-system.hpp"`

## Contents ##

* [TOC]

## Standard directories ##

* `enum class` **`SystemPath`**
    * `SystemPath::`**`apps`**
    * `SystemPath::`**`os`**
* `enum class` **`UserPath`**
    * `UserPath::`**`apps`**
    * `UserPath::`**`cache`**
    * `UserPath::`**`desktop`**
    * `UserPath::`**`documents`**
    * `UserPath::`**`downloads`**
    * `UserPath::`**`home`**
    * `UserPath::`**`movies`**
    * `UserPath::`**`music`**
    * `UserPath::`**`pictures`**
    * `UserPath::`**`settings`**
* `static Unicorn::Path` **`std_path`**`(SystemPath id)`
* `static Unicorn::Path` **`std_path`**`(UserPath id)`

TODO - throws `invalid_argument,system_error`

Typical locations on common operating systems:

Directory              | Interpretation                               | Linux               | Mac                                   | Windows
---------              | --------------                               | -----               | ---                                   | -------
`SystemPath::apps`     | System-wide application install directory    | `/usr/local/bin`    | `/Applications`                       | `C:\Program Files`
`SystemPath::os`       | Root directory for operating system files    | `/`                 | `/`                                   | `C:\Windows`
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

(Unfortunately `SearchPath` is defined as a macro on Windows.)

* `class` **`PathList`**`: public std::vector<Unicorn::Path>`

TODO

* `using PathList::`**`flag_type`** `= Unicorn::Path::flag_type`

TODO

* `static constexpr char PathList::`**`delimiter`** `= [':' on Unix, ';' on Windows]`

TODO

* `static constexpr flag_type PathList::`**`env`**      _- The string is the name of an environment variable_
* `static constexpr flag_type PathList::`**`no_dups`**  _- Remove duplicate entries_

TODO

* `PathList::`**`PathList`**`()`
* `explicit PathList::`**`PathList`**`(Uview text, flag_type flags = 0)`

TODO - flags `env,no_dups`

* `PathList::`**`~PathList`**`() noexcept`
* `PathList::`**`PathList`**`(const PathList& pl)`
* `PathList::`**`PathList`**`(PathList&& pl) noexcept`
* `PathList& PathList::`**`operator=`**`(const PathList& pl)`
* `PathList& PathList::`**`operator=`**`(PathList&& pl) noexcept`

TODO

* `bool PathList::`**`contains`**`(const Unicorn::Path& dir) const noexcept`

TODO

* `void PathList::`**`erase_all`**`(const Unicorn::Path& dir)`

TODO

* `void PathList::`**`erase_dups`**`()`

TODO

* `Unicorn::Path PathList::`**`find`**`(Uview name)`
* `Unicorn::Path PathList::`**`find`**`(Uview name, bool case_sensitive)`
* `Unicorn::Path PathList::`**`find`**`(const Regex& pattern)`

TODO - throws `system_error`

* `std::vector<Unicorn::Path> PathList::`**`find_all`**`(Uview name)`
* `std::vector<Unicorn::Path> PathList::`**`find_all`**`(Uview name, bool case_sensitive)`
* `std::vector<Unicorn::Path> PathList::`**`find_all`**`(const Regex& pattern)`

TODO - throws `system_error`

* `std::string PathList::`**`str`**`() const`
* `std::string` **`to_str`**`(const PathList& pl)`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const PathList& pl)`

TODO

* `static PathList PathList::`**`path`**`(flag_type flags = 0)`

TODO - flags `no_dups`
