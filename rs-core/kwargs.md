# Keyword Arguments #

By Ross Smith

* `#include "rs-core/kwargs.hpp"`

## Contents ##

[TOC]

## Keyword arguments ##

* `template <typename T, int ID = 0> struct` **`Kwarg`**
    * `constexpr ... Kwarg::`**`operator=`**`(const T& t) const noexcept`
* `template <typename T, int ID, typename... Args> constexpr bool` **`kwtest`**`(Kwarg<T, ID> key, Args... args)`
* `template <typename T, int ID, typename... Args> T` **`kwget`**`(Kwarg<T, ID> key, const T& def, Args... args)`

This provides a simple implementation of variadic keyword arguments for C++
functions.

Define a `Kwarg<T[,ID]>` object for each keyword argument, where `T` is the
argument type. The `ID` parameter is only needed to distinguish between
keywords with the same argument type. Functions that will take keyword
arguments should be declared with a variadic argument pack, possibly preceded
by ordinary positional arguments.

When calling the function, the keyword arguments should be supplied in the
form `key=value`, where `key` is a `Kwarg` object, and `value` is the argument
value. The value type must be convertible to `T`. If `T` is `bool`, the
keyword alone can be passed as an argument, with the value defaulting to
`true`.

In the function body, call `kwget()` or `kwtest()` for each possible keyword
argument, with the corresponding `Kwarg` object as the key, a default value
(for `kwget()`), and the variadic arguments from the enclosing function. The
`kwget()` function returns the value attached to the keyword, or the default
value if the keyword was not found in the argument list; `kwtest()` returns
whether or not the keyword was present. If the same keyword appears more than
once in the actual argument list, the first one found will be returned.

Example:

    class Window {
    public:
        static constexpr Kwarg<int, 1> width = {};
        static constexpr Kwarg<int, 2> height = {};
        static constexpr Kwarg<std::string> title = {};
        static constexpr Kwarg<bool> visible = {};
        template <typename... Args> explicit Window(Args... args) {
            int win_width = kwget(width, 640, args...);
            int win_height = kwget(height, 480, args...);
            string title_text = kwget(title, "New Window"s, args...);
            bool is_visible = kwget(visible, false, args...);
            // ...
        }
    };

    Window app_window(Window::title="Hello World", Window::width=1000, Window::height=750, Window::visible);
