#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include "rs-core/serial.hpp"
#include <functional>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace RS {

    namespace Detail {

        inline std::string_view unqualified_type(std::string_view name) {
            size_t depth = 0, i = 0, size = name.size(), start = 0, stop = npos;
            while (i < size) {
                if (name[i] == '<') {
                    if (stop == npos)
                        stop = i;
                    ++depth;
                    ++i;
                } else if (name[i] == '>') {
                    --depth;
                    ++i;
                } else if (name[i] == ':' && depth == 0 && name[i + 1] == ':') {
                    i += 2;
                    start = i;
                    stop = npos;
                } else {
                    ++i;
                }
            }
            return name.substr(start, stop - start);
        }

    }

    enum class Mark: uint32_t {
        none           = 0,                 // Empty bitmask
        implicit_in    = uint32_t(1) << 0,  // Implicit conversion from T to Marked
        implicit_out   = uint32_t(1) << 1,  // Implicit conversion from Marked to T
        no_copy        = uint32_t(1) << 2,  // Marked type is move only
        no_move        = uint32_t(1) << 3,  // Marked type is not copyable or movable
        reset_on_move  = uint32_t(1) << 4,  // Moving resets the contained T to its default (implies no_copy)
        arithmetic     = uint32_t(1) << 5,  // Define arithmetic operators
        function       = uint32_t(1) << 6,  // Define function call operators
        subscript      = uint32_t(1) << 7,  // Define subscript operators
        string_tag     = uint32_t(1) << 8,  // String format is tagged with ID
        json_tag       = uint32_t(1) << 9,  // JSON format is tagged with ID
    };

    RS_BITMASK_OPERATORS(Mark);

    template <typename T, typename ID, Mark Flags = Mark::none, typename Check = void>
    class Marked {
    private:
        static_assert(! std::is_abstract_v<T> && ! std::is_reference_v<T> && ! std::is_const_v<T> && ! std::is_volatile_v<T>,
            "Marked type must be concrete, not a reference, and not cv-qualified");
        static_assert(std::is_copy_constructible_v<T> == std::is_copy_assignable_v<T>, "Marked type has inconsisent copy semantics");
        static_assert(std::is_move_constructible_v<T> == std::is_move_assignable_v<T>, "Marked type has inconsisent move semantics");
        static constexpr bool has_implicit_in = bool(Flags & Mark::implicit_in);
        static constexpr bool has_implicit_out = bool(Flags & Mark::implicit_out);
        static constexpr bool t_is_defaulted = std::is_default_constructible_v<T>;
        static constexpr bool t_is_copyable = std::is_copy_constructible_v<T>;
        static constexpr bool t_is_movable = std::is_move_constructible_v<T>;
        static constexpr bool is_copyable = t_is_copyable && ! (Flags & (Mark::no_copy | Mark::no_move | Mark::reset_on_move));
        static constexpr bool is_movable = t_is_movable && ! (Flags & Mark::no_move);
        static constexpr bool is_reset_on_move = bool(Flags & Mark::reset_on_move);
        static constexpr bool has_function = bool(Flags & Mark::function);
        static constexpr bool has_subscript = bool(Flags & Mark::subscript);
        static constexpr bool is_checked = ! std::is_same_v<Check, void>;
        static_assert(! is_reset_on_move || t_is_defaulted, "Marked type that is reset on move must be default constructible");
        static_assert(! is_checked || t_is_copyable, "Marked type that is value checked must be copyable");
        static_assert(! has_implicit_in || ! has_implicit_out, "Inconsistent flags on marked class");
        static_assert(is_movable || ! is_reset_on_move, "Inconsistent flags on marked class");
        using implicit_copy_in_type = std::conditional_t<has_implicit_in && is_copyable, T, DummyTemplate<__LINE__>>;
        using explicit_copy_in_type = std::conditional_t<! has_implicit_in && is_copyable, T, DummyTemplate<__LINE__>>;
        using implicit_move_in_type = std::conditional_t<has_implicit_in && is_movable && ! is_checked, T, DummyTemplate<__LINE__>>;
        using explicit_move_in_type = std::conditional_t<! has_implicit_in && is_movable && ! is_checked, T, DummyTemplate<__LINE__>>;
        using copy_constructor_type = std::conditional_t<is_copyable, Marked, DummyTemplate<__LINE__>>;
        using move_constructor_type = std::conditional_t<is_movable, Marked, DummyTemplate<__LINE__>>;
        using copy_assign_type = std::conditional_t<is_copyable, Marked, DummyTemplate<__LINE__>>;
        using move_assign_type = std::conditional_t<is_movable, Marked, DummyTemplate<__LINE__>>;
        using copy_set_type = std::conditional_t<t_is_copyable, T, DummyTemplate<__LINE__>>;
        using move_set_type = std::conditional_t<t_is_movable && ! is_checked, T, DummyTemplate<__LINE__>>;
        using null_assign_type = std::conditional_t<t_is_defaulted, std::initializer_list<DummyTemplate<__LINE__>>, DummyTemplate<__LINE__>>;
        using implicit_out_type = std::conditional_t<has_implicit_out, T, DummyTemplate<__LINE__>>;
        using explicit_out_type = std::conditional_t<has_implicit_out, DummyTemplate<__LINE__>, T>;
        using operator_bool_type = std::conditional_t<std::is_constructible_v<bool, T>, bool, DummyTemplate<__LINE__>>;
        using check_function_type = std::conditional_t<is_checked, Check, Identity>;
    public:
        using value_type = T;
        using id_type = ID;
        using check_type = Check;
        static constexpr Mark flags = Flags;
        Marked() = default;
        Marked(const copy_constructor_type& m): value_(m.value_) {}
        Marked(move_constructor_type&& m) noexcept: value_(std::move(m.value_)) { post_move(m.value_); }
        Marked(const implicit_copy_in_type& t): value_(check_function_type()(t)) {}
        explicit Marked(const explicit_copy_in_type& t): value_(check_function_type()(t)) {}
        Marked(implicit_move_in_type&& t) noexcept: value_(std::move(t)) { post_move(t); }
        explicit Marked(explicit_move_in_type&& t) noexcept: value_(std::move(t)) { post_move(t); }
        ~Marked() noexcept = default;
        Marked& operator=(const copy_assign_type& m) { value_ = m.value_; return *this; }
        Marked& operator=(move_assign_type&& m) noexcept
            { if (&m != this) { value_ = std::move(m.value_); post_move(m.value_); } return *this; }
        Marked& operator=(const implicit_copy_in_type& t) { value_ = check_function_type()(t); return *this; }
        Marked& operator=(implicit_move_in_type&& t) noexcept { value_ = std::move(t); post_move(t); return *this; }
        Marked& operator=(null_assign_type) noexcept { value_ = {}; return *this; }
        const T& get() const noexcept { return value_; }
        void set(const copy_set_type& t) { value_ = check_function_type()(t); }
        void set(move_set_type&& t) noexcept { value_ = std::move(t); post_move(t); }
        operator const implicit_out_type&() const noexcept { return value_; }
        explicit operator const explicit_out_type&() const noexcept { return value_; }
        explicit operator operator_bool_type() const { return static_cast<bool>(value_); }
        template <typename T2 = T> std::enable_if_t<! is_checked, T2&> operator*() noexcept { return value_; }
        const T& operator*() const noexcept { return value_; }
        template <typename T2 = T> std::enable_if_t<! is_checked, T2*> operator->() noexcept { return &value_; }
        const T* operator->() const noexcept { return &value_; }
        template <typename... Args> auto operator()(Args&&... args)
            -> std::enable_if_t<has_function, decltype(std::declval<T&>()(std::forward<Args>(args)...))>
            { return value_(std::forward<Args>(args)...); }
        template <typename... Args> auto operator()(Args&&... args) const
            -> std::enable_if_t<has_function, decltype(std::declval<const T&>()(std::forward<Args>(args)...))>
            { return value_(std::forward<Args>(args)...); }
        template <typename I> auto operator[](I i)
            -> std::enable_if_t<has_subscript && ! is_checked, decltype(std::declval<T&>()[i])>
            { return value_[i]; }
        template <typename I> auto operator[](I i) const
            -> std::enable_if_t<has_subscript, decltype(std::declval<const T&>()[i])>
            { return value_[i]; }
        static const std::string& id_name() { static const std::string name(Detail::unqualified_type(type_name<ID>())); return name; }
    private:
        T value_ = {};
        template <typename T2> static void post_move(T2& t) noexcept { (void)t; if constexpr (is_reset_on_move) t = {}; }
    };

    #define RS_MARKED_COMPARISON_OPERATOR(op) \
        template <typename T, typename I, Mark F, typename C> \
        auto operator op(const Marked<T,I,F,C>& a, const Marked<T,I,F,C>& b) -> decltype(*a op *b) \
        { return *a op *b; }

    #define RS_MARKED_UNARY_OPERATOR(op) \
        template <typename T, typename I, Mark F, typename C> \
        auto operator op(const Marked<T,I,F,C>& a) \
        -> std::enable_if_t<bool(F & Mark::arithmetic) && std::is_convertible_v<decltype(op *a), T>, Marked<T,I,F,C>> \
        { return Marked<T,I,F,C>(op *a); }

    #define RS_MARKED_BINARY_OPERATOR(op) \
        template <typename T, typename I, Mark F, typename C> \
        auto operator op(const Marked<T,I,F,C>& a, const Marked<T,I,F,C>& b) \
        -> std::enable_if_t<bool(F & Mark::arithmetic) && std::is_convertible_v<decltype(*a op *b), T>, Marked<T,I,F,C>> \
        { return Marked<T,I,F,C>(*a op *b); }

    #define RS_MARKED_FUSED_OPERATOR(op) \
        template <typename T, typename I, Mark F, typename C> \
        auto operator op(Marked<T,I,F,C>& a, const Marked<T,I,F,C>& b) \
        -> std::enable_if_t<bool(F & Mark::arithmetic) && std::is_convertible_v<decltype(*a op *b), T&>, Marked<T,I,F,C>&> \
        { *a op *b; return a; }

    RS_MARKED_COMPARISON_OPERATOR(==);
    RS_MARKED_COMPARISON_OPERATOR(!=);
    RS_MARKED_COMPARISON_OPERATOR(<);
    RS_MARKED_COMPARISON_OPERATOR(>);
    RS_MARKED_COMPARISON_OPERATOR(<=);
    RS_MARKED_COMPARISON_OPERATOR(>=);

    RS_MARKED_UNARY_OPERATOR(+);
    RS_MARKED_UNARY_OPERATOR(-);
    RS_MARKED_UNARY_OPERATOR(~);

    RS_MARKED_BINARY_OPERATOR(+);
    RS_MARKED_BINARY_OPERATOR(-);
    RS_MARKED_BINARY_OPERATOR(*);
    RS_MARKED_BINARY_OPERATOR(/);
    RS_MARKED_BINARY_OPERATOR(%);
    RS_MARKED_BINARY_OPERATOR(&);
    RS_MARKED_BINARY_OPERATOR(|);
    RS_MARKED_BINARY_OPERATOR(^);
    RS_MARKED_BINARY_OPERATOR(<<);
    RS_MARKED_BINARY_OPERATOR(>>);

    RS_MARKED_FUSED_OPERATOR(+=);
    RS_MARKED_FUSED_OPERATOR(-=);
    RS_MARKED_FUSED_OPERATOR(*=);
    RS_MARKED_FUSED_OPERATOR(/=);
    RS_MARKED_FUSED_OPERATOR(%=);
    RS_MARKED_FUSED_OPERATOR(&=);
    RS_MARKED_FUSED_OPERATOR(|=);
    RS_MARKED_FUSED_OPERATOR(^=);
    RS_MARKED_FUSED_OPERATOR(<<=);
    RS_MARKED_FUSED_OPERATOR(>>=);

    template <typename T, typename I, Mark F, typename C>
    auto operator++(Marked<T,I,F,C>& a)
    -> std::enable_if_t<bool(F & Mark::arithmetic) && std::is_convertible_v<decltype(++*a), T&>, Marked<T,I,F,C>&> {
        ++*a;
        return a;
    }

    template <typename T, typename I, Mark F, typename C>
    auto operator++(Marked<T,I,F,C>& a, int)
    -> std::enable_if_t<bool(F & Mark::arithmetic) && std::is_convertible_v<decltype((*a)++), T>, Marked<T,I,F,C>> {
        auto b = a;
        (*a)++;
        return b;
    }

    template <typename T, typename I, Mark F, typename C>
    auto operator--(Marked<T,I,F,C>& a)
    -> std::enable_if_t<bool(F & Mark::arithmetic) && std::is_convertible_v<decltype(--*a), T&>, Marked<T,I,F,C>&> {
        --*a;
        return a;
    }

    template <typename T, typename I, Mark F, typename C>
    auto operator--(Marked<T,I,F,C>& a, int)
    -> std::enable_if_t<bool(F & Mark::arithmetic) && std::is_convertible_v<decltype((*a)--), T>, Marked<T,I,F,C>> {
        auto b = a;
        (*a)--;
        return b;
    }

    template <typename T, typename I, Mark F, typename C>
    std::string to_str(const Marked<T,I,F,C>& m) {
        static const auto prefix = m.id_name() + ':';
        std::string s;
        if constexpr (bool(F & Mark::string_tag))
            s = prefix;
        s += to_str(*m);
        return s;
    }

    template <typename T, typename I, Mark F, typename C>
    bool from_str(std::string_view s, Marked<T,I,F,C>& m) {
        static const auto prefix = m.id_name() + ':';
        if constexpr (bool(F & Mark::string_tag)) {
            if (! starts_with(s, prefix))
                return false;
            s = s.substr(prefix.size(), npos);
        }
        return from_str(s, *m);
    }

    template <typename T, typename I, Mark F, typename C>
    void to_json(json& j, const Marked<T,I,F,C>& m) {
        if constexpr (bool(F & Mark::json_tag)) {
            j = json::object();
            j["_type"] = m.id_name();
            j["value"] = *m;
        } else {
            j = *m;
        }
    }

    template <typename T, typename I, Mark F, typename C>
    void from_json(const json& j, Marked<T,I,F,C>& m) {
        if constexpr (bool(F & Mark::json_tag)) {
            if (! j.is_object() || ! j.count("_type") || j["_type"] != m.id_name() || ! j.count("value"))
                throw std::invalid_argument("Invalid serialized " + m.id_name());
            *m = j["value"].get<T>();
        } else {
            *m = j.get<T>();
        }
    }

}

namespace std {

    template <typename T, typename I, RS::Mark F, typename C>
    struct hash<RS::Marked<T,I,F,C>> {
        using value_type = std::conditional_t<RS::Meta::is_hashable<T>, RS::Marked<T,I,F,C>, RS::DummyTemplate<__LINE__>>;
        size_t operator()(const value_type& m) const {
            return std::hash<T>()(*m);
        }
    };

}
