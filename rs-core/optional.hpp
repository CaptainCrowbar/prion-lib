#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cstddef>
#include <functional>
#include <ostream>
#include <stdexcept>
#include <utility>

namespace RS {

    class NullOption:
    public std::runtime_error {
    public:
        NullOption(): std::runtime_error("Optional value is null") {}
    };

    template <typename T>
    class Optional {
    public:
        using value_type = T;
        Optional() noexcept {}
        Optional(const Optional& opt);
        Optional(Optional&& opt) noexcept;
        Optional(const T& t) { construct(t); }
        Optional(T&& t) noexcept { construct(std::move(t)); }
        Optional(std::nullptr_t) noexcept {}
        ~Optional() noexcept { reset(); }
        Optional& operator=(const Optional& opt);
        Optional& operator=(Optional&& opt) noexcept;
        Optional& operator=(const T& t);
        Optional& operator=(T&& t) noexcept;
        Optional& operator=(std::nullptr_t) noexcept;
        T& operator*() { return value(); }
        const T& operator*() const { return value(); }
        T* operator->() { return &value(); }
        const T* operator->() const { return &value(); }
        explicit operator bool() const noexcept { return state; }
        T* begin() noexcept { return &val; }
        const T* begin() const noexcept { return &val; }
        T* end() noexcept { return begin() + size_t(state); }
        const T* end() const noexcept { return begin() + size_t(state); }
        void check() const { if (! state) throw NullOption(); }
        template <typename... Args> void emplace(Args&&... args);
        bool has_value() const noexcept { return state; }
        size_t hash() const noexcept { return state ? std::hash<T>()(val) : 0; }
        void reset() noexcept;
        Ustring str() const { return state ? to_str(val) : "null"; }
        void swap(Optional& opt) noexcept;
        T& value() { check(); return val; }
        const T& value() const { check(); return val; }
        const T& value_or(const T& defval = {}) const { return state ? value() : defval; }
    private:
        union {
            uint8_t bytes[sizeof(T)];
            T val;
        };
        bool state = false;
        template <typename... Args> void construct(Args&&... args);
    };

    template <typename T>
    Optional<T>::Optional(const Optional& opt) {
        if (opt)
            construct(opt.val);
    }

    template <typename T>
    Optional<T>::Optional(Optional&& opt) noexcept {
        if (opt) {
            construct(std::move(opt.val));
            opt.reset();
        }
    }

    template <typename T>
    Optional<T>& Optional<T>::operator=(const Optional& opt) {
        if (state && opt.state)
            val = opt.val;
        else if (state)
            reset();
        else if (opt.state)
            construct(opt.val);
        return *this;
    }

    template <typename T>
    Optional<T>& Optional<T>::operator=(Optional&& opt) noexcept {
        if (state && opt.state)
            val = std::move(opt.val);
        else if (state)
            reset();
        else if (opt.state)
            construct(std::move(opt.val));
        opt.reset();
        return *this;
    }

    template <typename T>
    Optional<T>& Optional<T>::operator=(const T& t) {
        if (state)
            val = t;
        else
            construct(t);
        return *this;
    }

    template <typename T>
    Optional<T>& Optional<T>::operator=(T&& t) noexcept {
        if (state)
            val = std::move(t);
        else
            construct(std::move(t));
        return *this;
    }

    template <typename T>
    Optional<T>& Optional<T>::operator=(std::nullptr_t) noexcept {
        reset();
        return *this;
    }

    template <typename T>
    template <typename... Args>
    void Optional<T>::emplace(Args&&... args) {
        reset();
        construct(std::forward<Args>(args)...);
    }

    template <typename T>
    void Optional<T>::reset() noexcept {
        if (state) {
            val.~T();
            state = false;
        }
    }

    template <typename T>
    void Optional<T>::swap(Optional& opt) noexcept {
        using std::swap;
        if (state && opt.state)
            swap(val, opt.val);
        else if (state)
            opt = std::move(*this);
        else if (opt.state)
            *this = std::move(opt);
    }

    template <typename T>
    template <typename... Args>
    void Optional<T>::construct(Args&&... args) {
        new (&val) T(std::forward<Args>(args)...);
        state = true;
    }

    template <typename T> bool operator==(const Optional<T>& lhs, const Optional<T>& rhs) noexcept { return lhs && rhs ? *lhs == *rhs : ! lhs && ! rhs; }
    template <typename T> bool operator==(const Optional<T>& lhs, const T& rhs) noexcept { return lhs && *lhs == rhs; }
    template <typename T> bool operator==(const T& lhs, const Optional<T>& rhs) noexcept { return rhs && lhs == *rhs; }
    template <typename T> bool operator==(const Optional<T>& lhs, std::nullptr_t /*rhs*/) noexcept { return ! lhs; }
    template <typename T> bool operator==(std::nullptr_t /*lhs*/, const Optional<T>& rhs) noexcept { return ! rhs; }
    template <typename T> bool operator!=(const Optional<T>& lhs, const Optional<T>& rhs) noexcept { return ! (lhs == rhs); }
    template <typename T> bool operator!=(const Optional<T>& lhs, const T& rhs) noexcept { return ! (lhs == rhs); }
    template <typename T> bool operator!=(const T& lhs, const Optional<T>& rhs) noexcept { return ! (lhs == rhs); }
    template <typename T> bool operator!=(const Optional<T>& lhs, std::nullptr_t rhs) noexcept { return ! (lhs == rhs); }
    template <typename T> bool operator!=(std::nullptr_t lhs, const Optional<T>& rhs) noexcept { return ! (lhs == rhs); }
    template <typename T> bool operator<(const Optional<T>& lhs, const Optional<T>& rhs) noexcept { return lhs && rhs ? *lhs < *rhs : bool(rhs); }
    template <typename T> bool operator<(const Optional<T>& lhs, const T& rhs) noexcept { return ! lhs || *lhs < rhs; }
    template <typename T> bool operator<(const T& lhs, const Optional<T>& rhs) noexcept { return rhs && lhs < *rhs; }
    template <typename T> bool operator<(const Optional<T>& /*lhs*/, std::nullptr_t /*rhs*/) noexcept { return false; }
    template <typename T> bool operator<(std::nullptr_t /*lhs*/, const Optional<T>& rhs) noexcept { return bool(rhs); }
    template <typename T> bool operator>(const Optional<T>& lhs, const Optional<T>& rhs) noexcept { return rhs < lhs; }
    template <typename T> bool operator>(const Optional<T>& lhs, const T& rhs) noexcept { return rhs < lhs; }
    template <typename T> bool operator>(const T& lhs, const Optional<T>& rhs) noexcept { return rhs < lhs; }
    template <typename T> bool operator>(const Optional<T>& lhs, std::nullptr_t rhs) noexcept { return rhs < lhs; }
    template <typename T> bool operator>(std::nullptr_t lhs, const Optional<T>& rhs) noexcept { return rhs < lhs; }
    template <typename T> bool operator<=(const Optional<T>& lhs, const Optional<T>& rhs) noexcept { return ! (rhs < lhs); }
    template <typename T> bool operator<=(const Optional<T>& lhs, const T& rhs) noexcept { return ! (rhs < lhs); }
    template <typename T> bool operator<=(const T& lhs, const Optional<T>& rhs) noexcept { return ! (rhs < lhs); }
    template <typename T> bool operator<=(const Optional<T>& lhs, std::nullptr_t rhs) noexcept { return ! (rhs < lhs); }
    template <typename T> bool operator<=(std::nullptr_t lhs, const Optional<T>& rhs) noexcept { return ! (rhs < lhs); }
    template <typename T> bool operator>=(const Optional<T>& lhs, const Optional<T>& rhs) noexcept { return ! (lhs < rhs); }
    template <typename T> bool operator>=(const Optional<T>& lhs, const T& rhs) noexcept { return ! (lhs < rhs); }
    template <typename T> bool operator>=(const T& lhs, const Optional<T>& rhs) noexcept { return ! (lhs < rhs); }
    template <typename T> bool operator>=(const Optional<T>& lhs, std::nullptr_t rhs) noexcept { return ! (lhs < rhs); }
    template <typename T> bool operator>=(std::nullptr_t lhs, const Optional<T>& rhs) noexcept { return ! (lhs < rhs); }

    template <typename T> Optional<T> operator&(const Optional<T>& lhs, const Optional<T>& rhs) { return lhs && rhs ? rhs : Optional<T>(); }
    template <typename T> Optional<T> operator&(const Optional<T>& lhs, const T& rhs) { return lhs ? Optional<T>(rhs) : Optional<T>(); }
    template <typename T> Optional<T> operator&(const T& /*lhs*/, const Optional<T>& rhs) { return rhs ? rhs : Optional<T>(); }
    template <typename T> Optional<T> operator&(const Optional<T>& /*lhs*/, std::nullptr_t /*rhs*/) { return {}; }
    template <typename T> Optional<T> operator&(std::nullptr_t /*lhs*/, const Optional<T>& /*rhs*/) { return {}; }
    template <typename T> Optional<T> operator|(const Optional<T>& lhs, const Optional<T>& rhs) { return lhs ? lhs : rhs; }
    template <typename T> Optional<T> operator|(const Optional<T>& lhs, const T& rhs) { return lhs ? lhs : Optional<T>(rhs); }
    template <typename T> Optional<T> operator|(const T& lhs, const Optional<T>& /*rhs*/) { return Optional<T>(lhs); }
    template <typename T> Optional<T> operator|(const Optional<T>& lhs, std::nullptr_t /*rhs*/) { return lhs ? lhs : Optional<T>(); }
    template <typename T> Optional<T> operator|(std::nullptr_t /*lhs*/, const Optional<T>& rhs) { return rhs; }

    template <typename T> std::ostream& operator<<(std::ostream& out, const Optional<T>& opt) { return out << opt.str(); }
    template <typename T> Optional<T> make_optional(T&& t) { return Optional<T>(std::forward<T>(t)); }
    template <typename T> void swap(Optional<T>& lhs, Optional<T>& rhs) noexcept { lhs.swap(rhs); }
    template <typename T> Ustring to_str(const Optional<T>& opt) { return opt.str(); }

    namespace RS_Detail {

        template <typename F, typename T, typename... Args>
        struct OptionalResult1 {
            using type = decltype(std::declval<F>()(std::declval<T&>(), std::forward<Args>(std::declval<Args>())...));
        };

        template <typename F, typename T1, typename T2, typename... Args>
        struct OptionalResult2 {
            using type = decltype(std::declval<F>()(std::declval<T1&>(), std::declval<T2&>(), std::forward<Args>(std::declval<Args>())...));
        };

        template <typename Result>
        struct OptionalCall {
            template <typename F, typename Opt, typename... Args>
            Optional<Result> call1(F f, Opt& t, Args&&... args) const {
                if (t)
                    return f(*t, std::forward<Args>(args)...);
                else
                    return {};
            }
            template <typename F, typename Opt1, typename Opt2, typename... Args>
            Optional<Result> call2(F f, Opt1& t1, Opt2& t2, Args&&... args) const {
                if (t1 && t2)
                    return f(*t1, *t2, std::forward<Args>(args)...);
                else
                    return {};
            }
        };

        template <>
        struct OptionalCall<void> {
            template <typename F, typename Opt, typename... Args>
            void call1(F f, Opt& t, Args&&... args) const {
                if (t)
                    f(*t, std::forward<Args>(args)...);
            }
            template <typename F, typename Opt1, typename Opt2, typename... Args>
            void call2(F f, Opt1& t1, Opt2& t2, Args&&... args) const {
                if (t1 && t2)
                    f(*t1, *t2, std::forward<Args>(args)...);
            }
        };

    }

    template <typename F, typename T, typename... Args>
    auto opt_call(F f, Optional<T>& opt, Args&&... args) {
        using namespace RS_Detail;
        using R = typename OptionalResult1<F, T, Args...>::type;
        using C = OptionalCall<R>;
        return C().call1(f, opt, args...);
    }

    template <typename F, typename T, typename... Args>
    auto opt_call(F f, const Optional<T>& opt, Args&&... args) {
        using namespace RS_Detail;
        using R = typename OptionalResult1<F, const T, Args...>::type;
        using C = OptionalCall<R>;
        return C().call1(f, opt, args...);
    }

    template <typename F, typename T1, typename T2, typename... Args>
    auto opt_call(F f, Optional<T1>& opt1, Optional<T2>& opt2, Args&&... args) {
        using namespace RS_Detail;
        using R = typename OptionalResult2<F, T1, T2, Args...>::type;
        using C = OptionalCall<R>;
        return C().call2(f, opt1, opt2, args...);
    }

    template <typename F, typename T1, typename T2, typename... Args>
    auto opt_call(F f, const Optional<T1>& opt1, Optional<T2>& opt2, Args&&... args) {
        using namespace RS_Detail;
        using R = typename OptionalResult2<F, const T1, T2, Args...>::type;
        using C = OptionalCall<R>;
        return C().call2(f, opt1, opt2, args...);
    }

    template <typename F, typename T1, typename T2, typename... Args>
    auto opt_call(F f, Optional<T1>& opt1, const Optional<T2>& opt2, Args&&... args) {
        using namespace RS_Detail;
        using R = typename OptionalResult2<F, T1, const T2, Args...>::type;
        using C = OptionalCall<R>;
        return C().call2(f, opt1, opt2, args...);
    }

    template <typename F, typename T1, typename T2, typename... Args>
    auto opt_call(F f, const Optional<T1>& opt1, const Optional<T2>& opt2, Args&&... args) {
        using namespace RS_Detail;
        using R = typename OptionalResult2<F, const T1, const T2, Args...>::type;
        using C = OptionalCall<R>;
        return C().call2(f, opt1, opt2, args...);
    }

}

namespace std {

    template <typename T>
    struct hash<RS::Optional<T>> {
        using argument_type = RS::Optional<T>;
        using result_type = size_t;
        size_t operator()(RS::Optional<T> t) const noexcept { return t.hash(); }
    };

}
