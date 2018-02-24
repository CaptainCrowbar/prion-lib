#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include "rs-core/string.hpp"
#include <functional>
#include <ostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>

namespace RS {

    namespace RS_Detail {

        template <typename T, bool = std::is_constructible<bool, const T&>::value>
        struct ConvertToBool {
            bool operator()(const T& t) const { return bool(t); }
        };

        template <typename T>
        struct ConvertToBool<T, false> {
            bool operator()(const T&) const { return true; }
        };

        template <>
        struct ConvertToBool<Nil, false> {
            bool operator()(Nil) const { return false; }
        };

        enum class BranchOp {
            copy_construct,
            copy_assign,
            move_construct,
            move_assign,
            destroy,
            compare_equal,
            compare_less,
            convert_to_bool,
            convert_to_string,
            hash,
            output,
        };

        template <typename T, BranchOp Op>
        struct BranchFunction;

        template <typename T>
        struct BranchFunction<T, BranchOp::copy_construct> {
            using function_type = std::function<void(const void* from, void* to)>;
            void operator()(const void* from, void* to) const {
                auto t = static_cast<const T*>(from);
                new (to) T(*t);
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::copy_assign> {
            using function_type = std::function<void(const void* from, void* to)>;
            void operator()(const void* from, void* to) const {
                auto t1 = static_cast<const T*>(from);
                auto t2 = static_cast<T*>(to);
                *t2 = *t1;
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::move_construct> {
            using function_type = std::function<void(void* from, void* to)>;
            void operator()(void* from, void* to) const noexcept {
                auto t = static_cast<T*>(from);
                new (to) T(std::move(*t));
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::move_assign> {
            using function_type = std::function<void(void* from, void* to)>;
            void operator()(void* from, void* to) const noexcept {
                auto t1 = static_cast<T*>(from);
                auto t2 = static_cast<T*>(to);
                *t2 = std::move(*t1);
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::destroy> {
            using function_type = std::function<void(void* ptr)>;
            void operator()(void* ptr) const noexcept {
                auto t = static_cast<T*>(ptr);
                t->~T();
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::compare_equal> {
            using function_type = std::function<bool(const void* lhs, const void* rhs)>;
            bool operator()(const void* lhs, const void* rhs) const noexcept {
                auto t1 = static_cast<const T*>(lhs);
                auto t2 = static_cast<const T*>(rhs);
                return *t1 == *t2;
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::compare_less> {
            using function_type = std::function<bool(const void* lhs, const void* rhs)>;
            bool operator()(const void* lhs, const void* rhs) const noexcept {
                auto t1 = static_cast<const T*>(lhs);
                auto t2 = static_cast<const T*>(rhs);
                return std::less<T>()(*t1, *t2);
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::convert_to_bool> {
            using function_type = std::function<bool(const void* ptr)>;
            bool operator()(const void* ptr) const {
                auto t = static_cast<const T*>(ptr);
                return ConvertToBool<T>()(*t);
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::convert_to_string> {
            using function_type = std::function<std::string(const void* ptr)>;
            std::string operator()(const void* ptr) const {
                auto t = static_cast<const T*>(ptr);
                return to_str(*t);
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::hash> {
            using function_type = std::function<size_t(const void* ptr)>;
            size_t operator()(const void* ptr) const noexcept {
                auto t = static_cast<const T*>(ptr);
                return std::hash<T>()(*t);
            }
        };

        template <typename T>
        struct BranchFunction<T, BranchOp::output> {
            using function_type = std::function<void(const void* ptr, std::ostream& out)>;
            void operator()(const void* ptr, std::ostream& out) const {
                auto t = static_cast<const T*>(ptr);
                out << *t;
            }
        };

    }

    class VariantError:
    public std::runtime_error {
    public:
        VariantError(): std::runtime_error("Unexpected type in variant") {}
    };

    template <typename... TS>
    class Variant {
    public:
        using typelist = Meta::Typelist<TS...>;
        using head = Meta::Head<typelist>;
        static constexpr bool is_nullable = std::is_same<head, Nil>::value;
        static_assert(Meta::is_unique<typelist>, "Variant contains duplicate types");
        static_assert((Meta::find_type<typelist, Nil> <= 0), "Nil can only be the first branch of a variant");
        static_assert(Meta::all_of<typelist, std::is_default_constructible>, "Variant branch types are not all default constructible");
        static_assert(Meta::all_of<typelist, std::is_move_constructible>, "Variant branch types are not all move constructible");
        static_assert(Meta::all_of<typelist, std::is_move_assignable>, "Variant branch types are not all move assignable");
        Variant() noexcept(is_nullable) { new (&data) head{}; }
        template <typename T> Variant(const T& t);
        template <typename T> Variant(T&& t) noexcept;
        ~Variant() noexcept;
        Variant(const Variant& v);
        Variant(Variant&& v) noexcept;
        Variant& operator=(const Variant& v);
        Variant& operator=(Variant&& v) noexcept;
        explicit operator bool() const;
        template <typename T> T& as();
        template <typename T> const T& as() const;
        template <int Index> Meta::TypeAt<typelist, Index>& at();
        template <int Index> const Meta::TypeAt<typelist, Index>& at() const;
        template <typename T> T get_as(const T& def = {}) const;
        template <int Index> Meta::TypeAt<typelist, Index> get_at(const Meta::TypeAt<typelist, Index>& def = {}) const;
        template <typename T> T& ref_as() noexcept;
        template <typename T> const T& ref_as() const noexcept;
        template <int Index> Meta::TypeAt<typelist, Index>& ref_at() noexcept;
        template <int Index> const Meta::TypeAt<typelist, Index>& ref_at() const noexcept;
        template <typename T, typename F> void call_if(F f);
        template <typename T, typename F> void call_if(F f) const;
        template <typename T, typename F> std::decay_t<decltype(std::declval<F>()(std::declval<T>()))> call_or(F f) const;
        template <typename T, typename F, typename RT> RT call_or(F f, const RT& def) const;
        bool empty() const noexcept { return is_nullable && which == 0; }
        size_t hash() const noexcept;
        int index() const noexcept { return which; }
        template <typename T> bool is() const noexcept { return which == Meta::find_type<typelist, T>; }
        bool is_equal(const Variant& v) const noexcept;
        bool is_greater(const Variant& v) const noexcept { return v.is_less(*this); }
        bool is_less(const Variant& v) const noexcept;
        template <typename T> bool is_equal(const T& t) const noexcept;
        template <typename T> bool is_greater(const T& t) const noexcept;
        template <typename T> bool is_less(const T& t) const noexcept;
        void reset() noexcept(is_nullable);
        std::string str() const;
        void write(std::ostream& out) const;
    private:
        using union_type = std::aligned_union_t<1, TS...>;
        union_type data;
        int which = 0;
        template <RS_Detail::BranchOp Op> static typename RS_Detail::BranchFunction<void, Op>::function_type branch_op(int i) noexcept;
    };

    template <> class Variant<>;

    template <typename V, int Index> using VariantTypeAt = Meta::TypeAt<typename V::typelist, Index>;
    template <typename V> struct IsVariant: std::false_type {};
    template <typename... TS> struct IsVariant<Variant<TS...>>: std::true_type {};
    template <typename V, typename T> struct VariantHasType: std::integral_constant<bool, Meta::count_type<typename V::typelist, T>> {};
    template <typename V, typename T> using VariantIndexOf = Meta::FindType<typename V::typelist, T>;
    template <typename V> using VariantLength = Meta::LengthOf<typename V::typelist>;
    template <typename V> constexpr bool is_variant = IsVariant<V>::value;
    template <typename V, typename T> constexpr bool variant_has_type = VariantHasType<V, T>::value;
    template <typename V, typename T> constexpr int variant_index_of = VariantIndexOf<V, T>::value;
    template <typename V> constexpr int variant_length = VariantLength<V>::value;

    template <typename V, typename T>
    constexpr void static_check_variant_type() noexcept {
        static_assert(is_variant<V>, "Not a variant type");
        static_assert(Meta::count_type<typename V::typelist, T>, "Type is not a branch of the variant");
    }

    template <typename V, int Index>
    constexpr void static_check_variant_index() noexcept {
        static_assert(is_variant<V>, "Not a variant type");
        static_assert(Index >= 0 && Index < variant_length<V>, "Variant index is out of bounds");
    }

    template <typename... TS>
    template <typename T>
    Variant<TS...>::Variant(const T& t) {
        using namespace RS_Detail;
        static_check_variant_type<Variant, T>();
        which = variant_index_of<Variant, T>;
        branch_op<BranchOp::copy_construct>(which)(&t, &data);
    }

    template <typename... TS>
    template <typename T>
    Variant<TS...>::Variant(T&& t) noexcept {
        using namespace RS_Detail;
        static_check_variant_type<Variant, T>();
        which = variant_index_of<Variant, T>;
        branch_op<BranchOp::move_construct>(which)(&t, &data);
    }

    template <typename... TS>
    Variant<TS...>::~Variant() noexcept {
        using namespace RS_Detail;
        branch_op<BranchOp::destroy>(which)(&data);
    }

    template <typename... TS>
    Variant<TS...>::Variant(const Variant& v) {
        using namespace RS_Detail;
        branch_op<BranchOp::copy_construct>(v.which)(&v.data, &data);
        which = v.which;
    }

    template <typename... TS>
    Variant<TS...>::Variant(Variant&& v) noexcept {
        using namespace RS_Detail;
        branch_op<BranchOp::move_construct>(v.which)(&v.data, &data);
        which = v.which;
        if (is_nullable)
            v.reset();
    }

    template <typename... TS>
    Variant<TS...>& Variant<TS...>::operator=(const Variant& v) {
        using namespace RS_Detail;
        if (&v == this) {
            // do nothing
        } else if (which == v.which) {
            branch_op<BranchOp::copy_assign>(which)(&v.data, &data);
        } else {
            branch_op<BranchOp::destroy>(which)(&data);
            which = v.which;
            branch_op<BranchOp::copy_construct>(which)(&v.data, &data);
        }
        return *this;
    }

    template <typename... TS>
    Variant<TS...>& Variant<TS...>::operator=(Variant&& v) noexcept {
        using namespace RS_Detail;
        if (&v == this) {
            // do nothing
        } else if (which == v.which) {
            branch_op<BranchOp::move_assign>(which)(&v.data, &data);
            if (is_nullable)
                v.reset();
        } else {
            branch_op<BranchOp::destroy>(which)(&data);
            which = v.which;
            branch_op<BranchOp::move_construct>(which)(&v.data, &data);
            if (is_nullable)
                v.reset();
        }
        return *this;
    }

    template <typename... TS>
    Variant<TS...>::operator bool() const {
        using namespace RS_Detail;
        return branch_op<BranchOp::convert_to_bool>(which)(&data);
    }

    template <typename... TS>
    template <typename T>
    T& Variant<TS...>::as() {
        static_check_variant_type<Variant, T>();
        if (! is<T>())
            throw VariantError();
        return ref_as<T>();
    }

    template <typename... TS>
    template <typename T>
    const T& Variant<TS...>::as() const {
        static_check_variant_type<Variant, T>();
        if (! is<T>())
            throw VariantError();
        return ref_as<T>();
    }

    template <typename... TS>
    template <int Index>
    Meta::TypeAt<Meta::Typelist<TS...>, Index>& Variant<TS...>::at() {
        static_check_variant_index<Variant, Index>();
        if (which != Index)
            throw VariantError();
        return ref_at<Index>();
    }

    template <typename... TS>
    template <int Index>
    const Meta::TypeAt<Meta::Typelist<TS...>, Index>& Variant<TS...>::at() const {
        static_check_variant_index<Variant, Index>();
        if (which != Index)
            throw VariantError();
        return ref_at<Index>();
    }

    template <typename... TS>
    template <typename T>
    T Variant<TS...>::get_as(const T& def) const {
        static_check_variant_type<Variant, T>();
        if (is<T>())
            return ref_as<T>();
        else
            return def;
    }

    template <typename... TS>
    template <int Index>
    Meta::TypeAt<Meta::Typelist<TS...>, Index> Variant<TS...>::get_at(const Meta::TypeAt<Meta::Typelist<TS...>, Index>& def) const {
        static_check_variant_index<Variant, Index>();
        if (which == Index)
            return ref_at<Index>();
        else
            return def;
    }

    template <typename... TS>
    template <typename T>
    T& Variant<TS...>::ref_as() noexcept {
        static_check_variant_type<Variant, T>();
        return reinterpret_cast<T&>(data);
    }

    template <typename... TS>
    template <typename T>
    const T& Variant<TS...>::ref_as() const noexcept {
        static_check_variant_type<Variant, T>();
        return reinterpret_cast<const T&>(data);
    }

    template <typename... TS>
    template <int Index>
    Meta::TypeAt<Meta::Typelist<TS...>, Index>& Variant<TS...>::ref_at() noexcept {
        static_check_variant_index<Variant, Index>();
        using T = Meta::TypeAt<typelist, Index>;
        return reinterpret_cast<T&>(data);
    }

    template <typename... TS>
    template <int Index>
    const Meta::TypeAt<Meta::Typelist<TS...>, Index>& Variant<TS...>::ref_at() const noexcept {
        static_check_variant_index<Variant, Index>();
        using T = Meta::TypeAt<typelist, Index>;
        return reinterpret_cast<const T&>(data);
    }

    template <typename... TS>
    template <typename T, typename F>
    void Variant<TS...>::call_if(F f) {
        if (is<T>())
            f(ref_as<T>());
    }

    template <typename... TS>
    template <typename T, typename F>
    void Variant<TS...>::call_if(F f) const {
        if (is<T>())
            f(ref_as<T>());
    }

    template <typename... TS>
    template <typename T, typename F>
    std::decay_t<decltype(std::declval<F>()(std::declval<T>()))> Variant<TS...>::call_or(F f) const {
        if (is<T>())
            return f(ref_as<T>());
        else
            return {};
    }

    template <typename... TS>
    template <typename T, typename F, typename RT>
    RT Variant<TS...>::call_or(F f, const RT& def) const {
        if (is<T>())
            return f(ref_as<T>());
        else
            return def;
    }

    template <typename... TS>
    size_t Variant<TS...>::hash() const noexcept {
        using namespace RS_Detail;
        return branch_op<BranchOp::hash>(which)(&data);
    }

    template <typename... TS>
    bool Variant<TS...>::is_equal(const Variant& v) const noexcept {
        using namespace RS_Detail;
        return which == v.which && branch_op<BranchOp::compare_equal>(which)(&data, &v.data);
    }

    template <typename... TS>
    bool Variant<TS...>::is_less(const Variant& v) const noexcept {
        using namespace RS_Detail;
        if (which == v.which)
            return branch_op<BranchOp::compare_less>(which)(&data, &v.data);
        else
            return which < v.which;
    }

    template <typename... TS>
    template <typename T>
    bool Variant<TS...>::is_equal(const T& t) const noexcept {
        using namespace RS_Detail;
        static_check_variant_type<Variant, T>();
        return which == variant_index_of<Variant, T> && branch_op<BranchOp::compare_equal>(which)(&data, &t);
    }

    template <typename... TS>
    template <typename T>
    bool Variant<TS...>::is_greater(const T& t) const noexcept {
        using namespace RS_Detail;
        static constexpr int i = variant_index_of<Variant, T>;
        static_check_variant_type<Variant, T>();
        if (which == i)
            return branch_op<BranchOp::compare_less>(which)(&t, &data);
        else
            return which > i;
    }

    template <typename... TS>
    template <typename T>
    bool Variant<TS...>::is_less(const T& t) const noexcept {
        using namespace RS_Detail;
        static constexpr int i = variant_index_of<Variant, T>;
        static_check_variant_type<Variant, T>();
        if (which == i)
            return branch_op<BranchOp::compare_less>(which)(&data, &t);
        else
            return which < i;
    }

    template <typename... TS>
    void Variant<TS...>::reset() noexcept(is_nullable) {
        using namespace RS_Detail;
        if (which == 0) {
            *reinterpret_cast<head*>(&data) = head();
        } else {
            branch_op<BranchOp::destroy>(which)(&data);
            which = 0;
            new (&data) head{};
        }
    }

    template <typename... TS>
    std::string Variant<TS...>::str() const {
        using namespace RS_Detail;
        return branch_op<BranchOp::convert_to_string>(which)(&data);
    }

    template <typename... TS>
    void Variant<TS...>::write(std::ostream& out) const {
        using namespace RS_Detail;
        branch_op<BranchOp::output>(which)(&data, out);
    }

    template <typename... TS>
    template <RS_Detail::BranchOp Op>
    typename RS_Detail::BranchFunction<void, Op>::function_type Variant<TS...>::branch_op(int i) noexcept {
        using namespace RS_Detail;
        static const typename BranchFunction<void, Op>::function_type functions[] = { BranchFunction<TS, Op>()... };
        return functions[i];
    }

    template <typename... TS>
    inline std::ostream& operator<<(std::ostream& out, const Variant<TS...>& v) {
        v.write(out);
        return out;
    }

    template <typename... TS>
    inline std::string to_str(const Variant<TS...>& v) {
        return v.str();
    }

    template <typename... TS> inline bool operator==(const Variant<TS...>& lhs, const Variant<TS...>& rhs) noexcept { return lhs.is_equal(rhs); }
    template <typename... TS> inline bool operator!=(const Variant<TS...>& lhs, const Variant<TS...>& rhs) noexcept { return ! lhs.is_equal(rhs); }
    template <typename... TS> inline bool operator<(const Variant<TS...>& lhs, const Variant<TS...>& rhs) noexcept { return lhs.is_less(rhs); }
    template <typename... TS> inline bool operator>(const Variant<TS...>& lhs, const Variant<TS...>& rhs) noexcept { return rhs.is_less(lhs); }
    template <typename... TS> inline bool operator<=(const Variant<TS...>& lhs, const Variant<TS...>& rhs) noexcept { return ! rhs.is_less(lhs); }
    template <typename... TS> inline bool operator>=(const Variant<TS...>& lhs, const Variant<TS...>& rhs) noexcept { return ! lhs.is_less(rhs); }
    template <typename T, typename... TS> inline bool operator==(const Variant<TS...>& lhs, const T& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return lhs.is_equal(rhs); }
    template <typename T, typename... TS> inline bool operator!=(const Variant<TS...>& lhs, const T& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return ! lhs.is_equal(rhs); }
    template <typename T, typename... TS> inline bool operator<(const Variant<TS...>& lhs, const T& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return lhs.is_less(rhs); }
    template <typename T, typename... TS> inline bool operator>(const Variant<TS...>& lhs, const T& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return lhs.is_greater(rhs); }
    template <typename T, typename... TS> inline bool operator<=(const Variant<TS...>& lhs, const T& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return ! lhs.is_greater(rhs); }
    template <typename T, typename... TS> inline bool operator>=(const Variant<TS...>& lhs, const T& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return ! lhs.is_less(rhs); }
    template <typename T, typename... TS> inline bool operator==(const T& lhs, const Variant<TS...>& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return rhs.is_equal(lhs); }
    template <typename T, typename... TS> inline bool operator!=(const T& lhs, const Variant<TS...>& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return ! rhs.is_equal(lhs); }
    template <typename T, typename... TS> inline bool operator<(const T& lhs, const Variant<TS...>& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return rhs.is_greater(lhs); }
    template <typename T, typename... TS> inline bool operator>(const T& lhs, const Variant<TS...>& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return rhs.is_less(lhs); }
    template <typename T, typename... TS> inline bool operator<=(const T& lhs, const Variant<TS...>& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return ! rhs.is_less(lhs); }
    template <typename T, typename... TS> inline bool operator>=(const T& lhs, const Variant<TS...>& rhs) noexcept { static_check_variant_type<Variant<TS...>, T>(); return ! rhs.is_greater(lhs); }
    template <typename... TS> inline bool operator==(const Variant<TS...>& lhs, Nil /*rhs*/) noexcept { return lhs.empty(); }
    template <typename... TS> inline bool operator!=(const Variant<TS...>& lhs, Nil /*rhs*/) noexcept { return ! lhs.empty(); }
    template <typename... TS> inline bool operator<(const Variant<TS...>& /*lhs*/, Nil /*rhs*/) noexcept { return false; }
    template <typename... TS> inline bool operator>(const Variant<TS...>& lhs, Nil /*rhs*/) noexcept { return ! lhs.empty(); }
    template <typename... TS> inline bool operator<=(const Variant<TS...>& lhs, Nil /*rhs*/) noexcept { return lhs.empty(); }
    template <typename... TS> inline bool operator>=(const Variant<TS...>& /*lhs*/, Nil /*rhs*/) noexcept { return true; }
    template <typename... TS> inline bool operator==(Nil /*lhs*/, const Variant<TS...>& rhs) noexcept { return rhs.empty(); }
    template <typename... TS> inline bool operator!=(Nil /*lhs*/, const Variant<TS...>& rhs) noexcept { return ! rhs.empty(); }
    template <typename... TS> inline bool operator<(Nil /*lhs*/, const Variant<TS...>& rhs) noexcept { return ! rhs.empty(); }
    template <typename... TS> inline bool operator>(Nil /*lhs*/, const Variant<TS...>& /*rhs*/) noexcept { return false; }
    template <typename... TS> inline bool operator<=(Nil /*lhs*/, const Variant<TS...>& /*rhs*/) noexcept { return true; }
    template <typename... TS> inline bool operator>=(Nil /*lhs*/, const Variant<TS...>& rhs) noexcept { return rhs.empty(); }

}

namespace std {

    template <typename... TS> struct hash<RS::Variant<TS...>> {
        using argument_type = RS::Variant<TS...>;
        using result_type = size_t;
        size_t operator()(RS::Variant<TS...> t) const noexcept { return t.hash(); }
    };

}
