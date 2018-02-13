# URL #

By Ross Smith

* `#include "rs-core/url.hpp"`

## Contents ##

[TOC]

## URL class ##

* `class` **`Url`**

This class holds a URL, assumed to follow the format:

    scheme: [//] [ [user [:password] @] host [:port] ] [/path] [?query] [#fragment]

* `Url::`**`Url`**`()`
* `explicit Url::`**`Url`**`(Uview s)`
* `Url::`**`Url`**`(Uview scheme, Uview user, Uview password, Uview host, uint16_t port = 0, Uview path = {}, Uview query = {}, Uview fragment = {})`

Constructors. The default constructor produces an empty URL; the second
constructor parses a URL supplied as a string; the third assembles a URL from
its component parts (following the same rules as the individual `set_*()`
functions described below). The second and third constructors will throw
`std::invalid_argument` if their arguments do not form a valid URL.

* `Url::`**`~Url`**`() noexcept`
* `Url::`**`Url`**`(const Url& u)`
* `Url::`**`Url`**`(Url&& u) noexcept`
* `Url& Url::`**`operator=`**`(const Url& u)`
* `Url& Url::`**`operator=`**`(Url&& u) noexcept`

Other life cycle functions.

* `bool Url::`**`has_scheme`**`() const noexcept`
* `bool Url::`**`has_slashes`**`() const noexcept`
* `bool Url::`**`has_user`**`() const noexcept`
* `bool Url::`**`has_password`**`() const noexcept`
* `bool Url::`**`has_host`**`() const noexcept`
* `bool Url::`**`has_port`**`() const noexcept`
* `bool Url::`**`has_path`**`() const noexcept`
* `bool Url::`**`has_query`**`() const noexcept`
* `bool Url::`**`has_fragment`**`() const noexcept`

Query whether a given URL element is present.

* `Ustring Url::`**`scheme`**`() const`
* `Ustring Url::`**`user`**`() const`
* `Ustring Url::`**`password`**`() const`
* `Ustring Url::`**`host`**`() const`
* `uint16_t Url::`**`port`**`() const noexcept`
* `Ustring Url::`**`path`**`() const`
* `Ustring Url::`**`query`**`() const`
* `Ustring Url::`**`fragment`**`() const`

Query the value of a given URL element. The returned strings do not include
the associated punctuation (e.g. `scheme()` will not have a trailing colon).
In some cases this means that the corresponding `has_*()` function must also
be checked in order to distinguish between an element that is absent and one
that is present but with an empty value (e.g. a URL ending in a question mark
is considered to include an empty query string, which is not the same thing as
one with no query part).

* `void Url::`**`set_scheme`**`(Uview new_scheme)`
* `void Url::`**`set_user`**`(Uview new_user)`
* `void Url::`**`set_password`**`(Uview new_password)`
* `void Url::`**`set_host`**`(Uview new_host)`
* `void Url::`**`set_port`**`(uint16_t new_port)`
* `void Url::`**`set_path`**`(Uview new_path)`
* `void Url::`**`set_query`**`(Uview new_query)`
* `void Url::`**`set_fragment`**`(Uview new_fragment)`

Change the value of a given URL element. The string supplied will be escaped
when necessary. Passing an empty string is equivalent to clearing the element
only for elements that do not distinguish between an empty value and a missing
element; this is not true of the query and fragment elements (e.g.
`set_user("")` is equivalent to `clear_user()`, but `set_query("")` is not
equivalent to `clear_query()`). By default, `set_scheme()` will follow the
scheme with two slashes if the previous URL had slashes there, or if it was
empty; this can be overridden by explicitly ending the new scheme in `":"` or
`"://"`. All of these can throw `std::invalid_argument`: all except
`set_scheme()` will throw if the URL is empty and (for elements other than
query and fragment) the value is not empty; `set_scheme()` will throw if the
scheme is invalid (it is expected to start with an ASCII letter and contain
only letters, digits, dots, and plus and minus signs); `set_user()` will throw
if the host is empty; `set_password()` will throw if the user or host is
empty; `set_path()` will throw if the path contains two consecutive slashes.

* `void Url::`**`clear_user`**`() noexcept`
* `void Url::`**`clear_password`**`() noexcept`
* `void Url::`**`clear_host`**`() noexcept`
* `void Url::`**`clear_port`**`() noexcept`
* `void Url::`**`clear_path`**`() noexcept`
* `void Url::`**`clear_query`**`() noexcept`
* `void Url::`**`clear_fragment`**`() noexcept`

Remove a given URL element. There is no function to clear the scheme because
the scheme is always required.

* `void Url::`**`append_path`**`(Uview new_path)`
* `Url& Url::`**`operator/=`**`(Uview rhs)`
* `Url` **`operator/`**`(const Url& lhs, Uview rhs)`

Append one or more file path elements to the URL's path. Appending an absolute
path (one that starts with a slash) will discard the original path and behave
like `set_path()`. These will throw `std::invalid_argument` if the path is
invalid (contains two consecutive slashes).

* `File Url::`**`as_file`**`() const`
* `static Url Url::`**`from_file`**`(const File& f)`

Convert between a file name and a file URL. The `as_file()` function discards
all elements other than host and path, and will throw `std::invalid_argument`
if the URL is not a file URL (except that an empty URL will return an empty
file name). The `from_file()` function always treats its argument as an
absolute path, even if it does not start with a slash (or a drive letter on
Windows).

* `void Url::`**`clear`**`() noexcept;`

Resets the URL to an empty string.

* `Url Url::`**`doc`**`() const`
* `Url Url::`**`base`**`() const`
* `Url Url::`**`parent`**`() const`
* `Url Url::`**`root`**`() const`

These return related URLs. The `doc()` function discards the fragment part
(equivalent to `clear_fragment()`); `base()` discards the query and fragment
parts; `parent()` discards the last element of the path, if any; `root()`
discards the entire path and returns the root of the host directory tree
(equivalent to `set_path("/")`).

* `bool Url::`**`empty`**`() const noexcept`

True if the URL is an empty string.

* `bool Url::`**`is_root`**`() const noexcept`

True if the URL refers to the root of the host directory tree (the path part
is empty or `"/"`).

* `Ustring Url::`**`path_dir`**`() const`
* `Ustring Url::`**`path_leaf`**`() const`

Split the path element into a directory path and a leaf name. If `path_dir()`
is not empty, it will always start and end with a slash. If the whole path
ends with a slash, it is assumed to be a directory name, and `path_leaf()` is
empty.

* `Ustring Url::`**`str`**`() const`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const Url& u)`
* `Ustring` **`to_str`**`(const Url& u)`

Return the URL as a string.

* `bool Url::`**`try_parse`**`(Uview s)`

Attempts to parse the given string as a URL. On success, this changes the
current object to hold the new URL and returns true; on failure, it returns
false and leaves the object unchanged.

* `static Ustring Url::`**`encode`**`(Uview s, Uview exempt = "")`
* `static Ustring Url::`**`decode`**`(Uview s)`

These apply percent encoding to a string. Safe bytes, left unencoded, are the
ASCII alphanumerics plus `[-._~]`; all other bytes are encoded unless they
appear in the exempt string.

* `template <typename R> static Ustring Url::`**`make_query`**`(const R& range, char delimiter = '&', uint32_t flags = 0)`
* `static std::vector<std::pair<Ustring, Ustring>> Url::`**`parse_query`**`(Uview query, char delimiter = '\0')`

Construct or deconstruct a query string. The range argument to `make_query()`
is expected to contain pairs whose first and second fields are string views or
convertible to them; the delimiter character is inserted between key/value
pairs; an equals sign is inserted between keys and values, except that it will
be left out if the value is empty and the `lone_keys` flag is set. The
`parse_query()` function breaks down a query string into a vector of key/value
pairs; if no delimiter is explicitly supplied, the first of `'&'` or `';'`
that appears in the string will be used. Key and value strings are percent
encoded by `make_query()` and decoded by `parse_query()`.

* `bool` **`operator==`**`(const Url& lhs, const Url& rhs) noexcept`
* `bool` **`operator!=`**`(const Url& lhs, const Url& rhs) noexcept`
* `bool` **`operator<`**`(const Url& lhs, const Url& rhs) noexcept`
* `bool` **`operator>`**`(const Url& lhs, const Url& rhs) noexcept`
* `bool` **`operator<=`**`(const Url& lhs, const Url& rhs) noexcept`
* `bool` **`operator>=`**`(const Url& lhs, const Url& rhs) noexcept`

Comparison operators. These perform a simple string comparison on the URL
string.

* `namespace RS::Literals`
    * `Url` **`operator""_url`**`(const char* p, size_t n)`

URL literal operator. This calls the constructor from a string view,
and will throw the same exceptions.
