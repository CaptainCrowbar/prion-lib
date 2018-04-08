#pragma once

#include "rs-core/common.hpp"
#include <type_traits>

namespace RS {

    template <typename T, int ID> struct Kwptr { const T* ptr; };
    template <typename T, int ID = 0> struct Kwarg { constexpr Kwptr<T, ID> operator=(const T& t) const noexcept { return {&t}; } };

    template <typename T, int ID> constexpr bool kwtest(Kwarg<T, ID>) { return false; }
    template <typename T, int ID, typename... Args> constexpr bool kwtest(Kwarg<T, ID>, Kwptr<T, ID>, Args...) { return true; }
    template <int ID, typename... Args> constexpr bool kwtest(Kwarg<bool, ID>, Kwarg<bool, ID>, Args...) { return true; }
    template <typename T, int ID, typename Arg1, typename... Args> constexpr bool kwtest(Kwarg<T, ID> key, Arg1, Args... args) { return kwtest(key, args...); }

    template <typename T, int ID> T kwget(Kwarg<T, ID>, const T& def) { return def; }
    template <typename T, int ID, typename... Args> T kwget(Kwarg<T, ID>, const T&, Kwptr<T, ID> p, Args...) { return *p.ptr; }
    template <int ID, typename... Args> bool kwget(Kwarg<bool, ID>, bool, Kwarg<bool, ID>, Args...) { return true; }
    template <typename T, int ID, typename Arg1, typename... Args> T kwget(Kwarg<T, ID> key, const T& def, Arg1, Args... args) { return kwget(key, def, args...); }

}
