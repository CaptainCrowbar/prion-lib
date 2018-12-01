#pragma once

// Unicorn/utility must go first because it defines the compilation control
// macros that other headers need to see.

#include "unicorn/utility.hpp"
#include <algorithm>
#include <array>
#include <cerrno>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <ostream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

#ifdef __APPLE__
    #include <mach/mach.h>
#endif

#ifdef _WIN32
    #ifndef __CYGWIN__
        #include <winsock2.h>
        #include <ws2tcpip.h>
    #endif
    #include <windows.h>
    #ifndef __CYGWIN__
        #include <io.h>
    #endif
#endif

#ifdef _XOPEN_SOURCE
    #include <sched.h>
    #include <sys/select.h>
    #include <sys/time.h>
    #include <unistd.h>
#endif

RS_LDLIB(rs-core);

// GNU brain damage

#ifdef __GNUC__
    #ifdef major
        #undef major
    #endif
    #ifdef minor
        #undef minor
    #endif
#endif

// Preprocessor macros

#define RS_BITMASK_OPERATORS(EC) \
    inline constexpr bool operator!(EC x) noexcept { return std::underlying_type_t<EC>(x) == 0; } \
    inline constexpr EC operator~(EC x) noexcept { return EC(~ std::underlying_type_t<EC>(x)); } \
    inline constexpr EC operator&(EC lhs, EC rhs) noexcept { return EC(std::underlying_type_t<EC>(lhs) & std::underlying_type_t<EC>(rhs)); } \
    inline constexpr EC operator|(EC lhs, EC rhs) noexcept { return EC(std::underlying_type_t<EC>(lhs) | std::underlying_type_t<EC>(rhs)); } \
    inline constexpr EC operator^(EC lhs, EC rhs) noexcept { return EC(std::underlying_type_t<EC>(lhs) ^ std::underlying_type_t<EC>(rhs)); } \
    inline constexpr EC& operator&=(EC& lhs, EC rhs) noexcept { return lhs = lhs & rhs; } \
    inline constexpr EC& operator|=(EC& lhs, EC rhs) noexcept { return lhs = lhs | rhs; } \
    inline constexpr EC& operator^=(EC& lhs, EC rhs) noexcept { return lhs = lhs ^ rhs; }

#define RS_OVERLOAD(f) [] (auto&&... args) { return f(std::forward<decltype(args)>(args)...); }

namespace RS {

    // Implementation details

    namespace RS_Detail {

        // https://stackoverflow.com/questions/41207774/how-do-i-create-a-tuple-of-n-ts-from-an-array-of-t
        template <size_t... Sequence, typename Array>
        constexpr auto array_to_tuple_helper(const Array& array, std::index_sequence<Sequence...>) {
            return std::make_tuple(array[Sequence]...);
        }

    }

    template <typename T, size_t N>
    constexpr auto array_to_tuple(const T (&array)[N]) {
        return RS_Detail::array_to_tuple_helper(array, std::make_index_sequence<N>{});
    }

    template <typename T, size_t N>
    constexpr auto array_to_tuple(const std::array<T, N> &array) {
        return RS_Detail::array_to_tuple_helper(array, std::make_index_sequence<N>{});
    }

    namespace RS_Detail {

        void append_hex_byte(uint8_t b, std::string& s);

        template <typename CharIterator>
        inline int decode_hex_byte(CharIterator& i, CharIterator end) noexcept {
            auto j = i;
            if (end - i >= 2 && j[0] == '0' && (j[1] == 'X' || j[1] == 'x'))
                j += 2;
            if (end - j < 2)
                return -1;
            int n = 0;
            for (auto k = j + 2; j != k; ++j) {
                n <<= 4;
                if (*j >= '0' && *j <= '9')
                    n += *j - '0';
                else if (*j >= 'A' && *j <= 'F')
                    n += *j - 'A' + 10;
                else if (*j >= 'a' && *j <= 'f')
                    n += *j - 'a' + 10;
                else
                    return -1;
            }
            i = j;
            return n;
        }

    }

    // [Types]

    // Endian integers

    enum ByteOrder {
        big_endian = 1,
        little_endian = 2,
        native_endian = big_endian_target ? big_endian : little_endian
    };

    namespace RS_Detail {

        template <typename T>
        constexpr T swap_ends(T t, size_t N = sizeof(T)) noexcept {
            using U = std::make_unsigned_t<T>;
            return N == 1 ? t : T((swap_ends(U(t) & ((U(1) << (4 * N)) - 1), N / 2) << (4 * N)) | swap_ends(U(t) >> (4 * N), N / 2));
        }

        template <ByteOrder B, typename T>
        constexpr T order_bytes(T t) noexcept {
            return (B == big_endian) == big_endian_target ? t : swap_ends(t);
        }

    }

    template <typename T, ByteOrder B>
    class Endian {
    public:
        using value_type = T;
        static constexpr auto byte_order = B;
        constexpr Endian() noexcept: value(0) {}
        constexpr Endian(T t) noexcept: value(RS_Detail::order_bytes<B>(t)) {}
        explicit Endian(const void* p) noexcept { memcpy(&value, p, sizeof(T)); }
        constexpr operator T() const noexcept { return get(); }
        constexpr T get() const noexcept { return RS_Detail::order_bytes<B>(value); }
        constexpr const T* ptr() const noexcept { return &value; }
        T* ptr() noexcept { return &value; }
        constexpr T rep() const noexcept { return value; }
        T& rep() noexcept { return value; }
    private:
        T value;
    };

    template <typename T> using BigEndian = Endian<T, big_endian>;
    template <typename T> using LittleEndian = Endian<T, little_endian>;

    template <typename T, ByteOrder B> inline std::ostream& operator<<(std::ostream& out, Endian<T, B> t) { return out << t.get(); }

    static_assert(sizeof(BigEndian<uint8_t>) == 1);
    static_assert(sizeof(BigEndian<uint16_t>) == 2);
    static_assert(sizeof(BigEndian<uint32_t>) == 4);
    static_assert(sizeof(BigEndian<uint64_t>) == 8);
    static_assert(sizeof(LittleEndian<uint8_t>) == 1);
    static_assert(sizeof(LittleEndian<uint16_t>) == 2);
    static_assert(sizeof(LittleEndian<uint32_t>) == 4);
    static_assert(sizeof(LittleEndian<uint64_t>) == 8);

    // Exceptions

    inline void rethrow(std::exception_ptr p) { if (p) std::rethrow_exception(p); }

    // Metaprogramming and type traits

    namespace RS_Detail {

        template <size_t Bits> struct IntegerType;
        template <> struct IntegerType<8> { using signed_type = int8_t; using unsigned_type = uint8_t; };
        template <> struct IntegerType<16> { using signed_type = int16_t; using unsigned_type = uint16_t; };
        template <> struct IntegerType<32> { using signed_type = int32_t; using unsigned_type = uint32_t; };
        template <> struct IntegerType<64> { using signed_type = int64_t; using unsigned_type = uint64_t; };

    }

    template <typename T> using BinaryType = typename RS_Detail::IntegerType<8 * sizeof(T)>::unsigned_type;
    template <typename T1, typename T2> using CopyConst = std::conditional_t<std::is_const<T1>::value, std::add_const_t<T2>, std::remove_const_t<T2>>;
    template <size_t Bits> using SignedInteger = typename RS_Detail::IntegerType<Bits>::signed_type;
    template <size_t Bits> using UnsignedInteger = typename RS_Detail::IntegerType<Bits>::unsigned_type;

    // Mixins

    template <typename T>
    class NumericLimitsBase {
    public:
        // Universal properties
        static constexpr bool is_specialized = true;                                   // Always true
        static constexpr bool is_bounded = false;                                      // Finite set of values
        static constexpr bool is_exact = false;                                        // Exact representation
        static constexpr bool is_integer = false;                                      // Integer type
        static constexpr bool is_modulo = false;                                       // Modulo arithmetic
        static constexpr bool is_signed = false;                                       // Signed type
        static constexpr bool traps = false;                                           // Trap values exist
        static constexpr int radix = 0;                                                // Basis of representation
        // Bounded type properties
        static constexpr int digits = 0;                                               // Radix digits in significand
        static constexpr int digits10 = 0;                                             // Decimals represented without loss
        static constexpr T lowest() noexcept { return T(); }                           // Minimum finite value
        static constexpr T max() noexcept { return T(); }                              // Maximum finite value
        static constexpr T min() noexcept { return T(); }                              // Minimum finite integer or positive float
        // Floating point properties
        static constexpr bool has_denorm_loss = false;                                 // Loss of accuracy as denormalization
        static constexpr bool has_infinity = false;                                    // Has positive infinity
        static constexpr bool has_quiet_NaN = false;                                   // Has quiet NaN
        static constexpr bool has_signaling_NaN = false;                               // Has signalling NaN
        static constexpr bool is_iec559 = false;                                       // IEC 559 standard
        static constexpr bool tinyness_before = false;                                 // Tinyness is detected before rounding
        static constexpr int max_digits10 = 0;                                         // Decimals required for different values
        static constexpr int max_exponent = 0;                                         // Maximum exponent
        static constexpr int max_exponent10 = 0;                                       // Maximum decimal exponent
        static constexpr int min_exponent = 0;                                         // Minimum exponent
        static constexpr int min_exponent10 = 0;                                       // Minimum decimal exponent
        static constexpr std::float_denorm_style has_denorm = std::denorm_absent;      // Has denormalized values
        static constexpr std::float_round_style round_style = std::round_toward_zero;  // Rounding style
        static constexpr T denorm_min() noexcept { return T(); }                       // Minimum positive subnormal value
        static constexpr T epsilon() noexcept { return T(); }                          // Difference between 1 and next value
        static constexpr T infinity() noexcept { return T(); }                         // Positive infinity
        static constexpr T quiet_NaN() noexcept { return T(); }                        // Quiet NaN
        static constexpr T round_error() noexcept { return T(); }                      // Maximum rounding error
        static constexpr T signaling_NaN() noexcept { return T(); }                    // Signalling NaN
    protected:
        static constexpr int digits2_to_10(size_t bits) noexcept { return int(bits * 12655ull / 42039ull); }
    };

    // Smart pointers

    template <typename T>
    class CopyPtr {
    public:
        using element_type = T;
        CopyPtr() = default;
        CopyPtr(T* p) noexcept: ptr(p) {}
        CopyPtr(std::nullptr_t) noexcept {}
        CopyPtr(const CopyPtr& cp) { if (cp) ptr = new T(*cp); }
        CopyPtr(CopyPtr&& cp) noexcept: ptr(std::exchange(cp.ptr, nullptr)) {}
        ~CopyPtr() noexcept { reset(); }
        CopyPtr& operator=(T* p) noexcept { CopyPtr cp2(p); swap(*this, cp2); return *this; }
        CopyPtr& operator=(std::nullptr_t) noexcept { reset(); return *this; }
        CopyPtr& operator=(const CopyPtr& cp) { CopyPtr cp2(cp); swap(*this, cp2); return *this; }
        CopyPtr& operator=(CopyPtr&& cp) noexcept { reset(); ptr = std::exchange(cp.ptr, nullptr); return *this; }
        T& operator*() const noexcept { return *ptr; }
        T* operator->() const noexcept { return ptr; }
        explicit operator bool() const noexcept { return ptr != nullptr; }
        T* get() const noexcept { return ptr; }
        T* release() noexcept { return std::exchange(ptr, nullptr); }
        void reset(T* p = nullptr) noexcept { delete ptr; ptr = p; }
        friend void swap(CopyPtr& a, CopyPtr& b) noexcept { std::swap(a.ptr, b.ptr); }
    private:
        T* ptr = nullptr;
    };

    template <typename T, typename... Args>
    CopyPtr<T> make_copy_ptr(Args&&... args) {
        return CopyPtr<T>(new T(std::forward<Args>(args)...));
    }

    template <typename T>
    class ClonePtr {
    public:
        using element_type = T;
        ClonePtr() = default;
        ClonePtr(T* p) noexcept: ptr(p) {}
        ClonePtr(std::nullptr_t) noexcept {}
        ClonePtr(const ClonePtr& cp) { if (cp) ptr = cp->clone(); }
        ClonePtr(ClonePtr&& cp) noexcept: ptr(std::exchange(cp.ptr, nullptr)) {}
        ~ClonePtr() noexcept { reset(); }
        ClonePtr& operator=(T* p) noexcept { ClonePtr cp2(p); swap(*this, cp2); return *this; }
        ClonePtr& operator=(std::nullptr_t) noexcept { reset(); return *this; }
        ClonePtr& operator=(const ClonePtr& cp) { ClonePtr cp2(cp); swap(*this, cp2); return *this; }
        ClonePtr& operator=(ClonePtr&& cp) noexcept { reset(); ptr = std::exchange(cp.ptr, nullptr); return *this; }
        T& operator*() const noexcept { return *ptr; }
        T* operator->() const noexcept { return ptr; }
        explicit operator bool() const noexcept { return ptr != nullptr; }
        T* get() const noexcept { return ptr; }
        T* release() noexcept { return std::exchange(ptr, nullptr); }
        void reset(T* p = nullptr) noexcept { delete ptr; ptr = p; }
        friend void swap(ClonePtr& a, ClonePtr& b) noexcept { std::swap(a.ptr, b.ptr); }
    private:
        T* ptr = nullptr;
    };

    template <typename T, typename... Args>
    ClonePtr<T> make_clone_ptr(Args&&... args) {
        return ClonePtr<T>(new T(std::forward<Args>(args)...));
    }

    // Type adapters

    namespace RS_Detail {

        template <typename T, int Def, bool Conv = std::is_constructible<T, int>::value>
        struct DefaultTo;

        template <typename T, int Def>
        struct DefaultTo<T, Def, true> {
            static T default_value() { return static_cast<T>(Def); }
        };

        template <typename T>
        struct DefaultTo<T, 0, false> {
            static T default_value() { return T(); }
        };

    }

    template <typename T, int Def = 0>
    struct AutoMove:
    private RS_Detail::DefaultTo<T, Def> {
        using value_type = T;
        T value;
        AutoMove(): value(this->default_value()) {}
        AutoMove(const T& t): value(t) {}
        AutoMove(T&& t): value(std::move(t)) {}
        ~AutoMove() = default;
        AutoMove(const AutoMove& x) = default;
        AutoMove(AutoMove&& x): value(std::exchange(x.value, this->default_value())) {}
        AutoMove& operator=(const AutoMove& x) = default;
        AutoMove& operator=(AutoMove&& x) { if (&x != this) { value = std::exchange(x.value, this->default_value()); } return *this; }
    };

    template <typename T, int Def = 0>
    struct NoTransfer:
    private RS_Detail::DefaultTo<T, Def> {
        using value_type = T;
        T value;
        NoTransfer(): value(this->default_value()) {}
        NoTransfer(const T& t): value(t) {}
        NoTransfer(T&& t): value(std::move(t)) {}
        ~NoTransfer() = default;
        NoTransfer(const NoTransfer&) {}
        NoTransfer(NoTransfer&&) {}
        NoTransfer& operator=(const NoTransfer&) { return *this; }
        NoTransfer& operator=(NoTransfer&&) { return *this; }
    };

    // Type names

    std::string demangle(const std::string& name);
    inline std::string type_name(const std::type_info& t) { return demangle(t.name()); }
    inline std::string type_name(const std::type_index& t) { return demangle(t.name()); }
    template <typename T> std::string type_name() { return type_name(typeid(T)); }
    template <typename T> std::string type_name(const T& t) { return type_name(typeid(t)); }

    // Type related functions

    template <typename T2, typename T1> bool is(const T1& ref) noexcept { return dynamic_cast<const T2*>(&ref) != nullptr; }
    template <typename T2, typename T1> bool is(const T1* ptr) noexcept { return dynamic_cast<const T2*>(ptr) != nullptr; }
    template <typename T2, typename T1> bool is(const std::unique_ptr<T1>& ptr) noexcept { return dynamic_cast<const T2*>(ptr.get()) != nullptr; }
    template <typename T2, typename T1> bool is(const std::shared_ptr<T1>& ptr) noexcept { return dynamic_cast<const T2*>(ptr.get()) != nullptr; }

    template <typename T2, typename T1> T2& as(T1& ref) { return dynamic_cast<T2&>(ref); }
    template <typename T2, typename T1> const T2& as(const T1& ref) { return dynamic_cast<const T2&>(ref); }
    template <typename T2, typename T1> T2& as(T1* ptr) { if (ptr) return dynamic_cast<T2&>(*ptr); else throw std::bad_cast(); }
    template <typename T2, typename T1> const T2& as(const T1* ptr) { if (ptr) return dynamic_cast<const T2&>(*ptr); else throw std::bad_cast(); }
    template <typename T2, typename T1> T2& as(std::unique_ptr<T1>& ptr) { if (ptr) return dynamic_cast<T2&>(*ptr); else throw std::bad_cast(); }
    template <typename T2, typename T1> T2& as(const std::unique_ptr<T1>& ptr) { if (ptr) return dynamic_cast<T2&>(*ptr); else throw std::bad_cast(); }
    template <typename T2, typename T1> T2& as(std::shared_ptr<T1>& ptr) { if (ptr) return dynamic_cast<T2&>(*ptr); else throw std::bad_cast(); }
    template <typename T2, typename T1> T2& as(const std::shared_ptr<T1>& ptr) { if (ptr) return dynamic_cast<T2&>(*ptr); else throw std::bad_cast(); }

    template <typename T2, typename T1>
    inline T2 binary_cast(const T1& t) noexcept {
        static_assert(sizeof(T2) == sizeof(T1));
        T2 t2;
        memcpy(&t2, &t, sizeof(t));
        return t2;
    }

    template <typename T2, typename T1>
    inline T2 implicit_cast(const T1& t) {
        static_assert(std::is_convertible<T1, T2>::value);
        return static_cast<T2>(t);
    }

    // [Constants and literals]

    // Arithmetic literals

    namespace Literals {

        constexpr int8_t operator""_s8(unsigned long long n) noexcept { return int8_t(n); }
        constexpr uint8_t operator""_u8(unsigned long long n) noexcept { return uint8_t(n); }
        constexpr int16_t operator""_s16(unsigned long long n) noexcept { return int16_t(n); }
        constexpr uint16_t operator""_u16(unsigned long long n) noexcept { return uint16_t(n); }
        constexpr int32_t operator""_s32(unsigned long long n) noexcept { return int32_t(n); }
        constexpr uint32_t operator""_u32(unsigned long long n) noexcept { return uint32_t(n); }
        constexpr int64_t operator""_s64(unsigned long long n) noexcept { return int64_t(n); }
        constexpr uint64_t operator""_u64(unsigned long long n) noexcept { return uint64_t(n); }
        constexpr ptrdiff_t operator""_pt(unsigned long long n) noexcept { return ptrdiff_t(n); }
        constexpr size_t operator""_sz(unsigned long long n) noexcept { return size_t(n); }

    }

    namespace RS_Detail {

        using namespace RS::Literals;

    }

    // Other constants

    constexpr unsigned KB = 1u << 10;
    constexpr unsigned long MB = 1ul << 20;
    constexpr unsigned long GB = 1ul << 30;
    constexpr unsigned long long TB = 1ull << 40;

    // [Algorithms and ranges]

    // Generic algorithms

    template <typename Iterator>
    void advance_by(Iterator& i, ptrdiff_t n, Iterator end) {
        using category = typename std::iterator_traits<Iterator>::iterator_category;
        if (std::is_same<category, std::random_access_iterator_tag>::value) {
            n = std::min(n, std::distance(i, end));
            std::advance(i, n);
        } else {
            for (; i != end && n > 0; ++i, --n) {}
        }
    }

    template <typename T> constexpr Irange<T*> array_range(T* ptr, size_t len) { return {ptr, ptr + len}; }

    template <typename Range, typename T>
    Meta::RangeValue<Range> at_index(const Range& r, size_t index, const T& def) {
        using std::begin;
        using std::end;
        auto b = begin(r), e = end(r);
        if (index < size_t(e - b))
            return b[index];
        else
            return def;
    }

    template <typename Range>
    auto at_index(const Range& r, size_t index) {
        return at_index(r, index, Meta::RangeValue<Range>());
    }

    template <typename InputRange, typename Container>
    void con_append(const InputRange& src, Container& dst) {
        using std::begin;
        using std::end;
        std::copy(begin(src), end(src), append(dst));
    }

    template <typename InputRange, typename Container>
    void con_overwrite(const InputRange& src, Container& dst) {
        using std::begin;
        using std::end;
        std::copy(begin(src), end(src), overwrite(dst));
    }

    template <typename Container, typename T>
    void con_remove(Container& con, const T& t) {
        con.erase(std::remove(con.begin(), con.end(), t), con.end());
    }

    template <typename Container, typename Predicate>
    void con_remove_if(Container& con, Predicate p) {
        con.erase(std::remove_if(con.begin(), con.end(), p), con.end());
    }

    template <typename Container, typename Predicate>
    void con_remove_if_not(Container& con, Predicate p) {
        con.erase(std::remove_if(con.begin(), con.end(), [p] (const auto& x) { return ! p(x); }), con.end());
    }

    template <typename Container>
    void con_unique(Container& con) {
        con.erase(std::unique(con.begin(), con.end()), con.end());
    }

    template <typename Container, typename BinaryPredicate>
    void con_unique(Container& con, BinaryPredicate p) {
        con.erase(std::unique(con.begin(), con.end(), p), con.end());
    }

    template <typename Container>
    void con_sort_unique(Container& con) {
        std::sort(con.begin(), con.end());
        con_unique(con);
    }

    template <typename Container, typename Compare>
    void con_sort_unique(Container& con, Compare cmp) {
        std::sort(con.begin(), con.end(), cmp);
        con_unique(con, [cmp] (const auto& a, const auto& b) { return ! cmp(a, b); });
    }

    template <typename Container, typename T>
    void con_trim_left(Container& con, const T& t) {
        auto begin = con.begin(), end = con.end(), it = begin;
        while (it != end && *it == t)
            ++it;
        con.erase(begin, it);
    }

    template <typename Container, typename Predicate>
    void con_trim_left_if(Container& con, Predicate p) {
        auto begin = con.begin(), end = con.end(), it = begin;
        while (it != end && p(*it))
            ++it;
        con.erase(begin, it);
    }

    template <typename Container, typename T>
    void con_trim_right(Container& con, const T& t) {
        auto begin = con.begin(), end = con.end(), it = end;
        while (it != begin) {
            auto pr = std::prev(it);
            if (*pr != t)
                break;
            it = pr;
        }
        con.erase(it, end);
    }

    template <typename Container, typename Predicate>
    void con_trim_right_if(Container& con, Predicate p) {
        auto begin = con.begin(), end = con.end(), it = end;
        while (it != begin) {
            auto pr = std::prev(it);
            if (! p(*pr))
                break;
            it = pr;
        }
        con.erase(it, end);
    }

    template <typename Container, typename T>
    void con_trim(Container& con, const T& t) {
        con_trim_left(con, t);
        con_trim_right(con, t);
    }

    template <typename Container, typename Predicate>
    void con_trim_if(Container& con, Predicate p) {
        con_trim_left_if(con, p);
        con_trim_right_if(con, p);
    }

    namespace RS_Detail {

        template <typename T>
        void concat_helper(std::vector<T>&) noexcept {}

        template <typename T, typename R, typename... RS>
        void concat_helper(std::vector<T>& v, const R& r, const RS&... rs) noexcept {
            using std::begin;
            using std::end;
            v.insert(v.end(), begin(r), end(r));
            concat_helper(v, rs...);
        }

    }

    template <typename InputRange, typename... Ranges>
    std::vector<Meta::RangeValue<InputRange>> concatenate(const InputRange& range, const Ranges&... ranges) {
        std::vector<Meta::RangeValue<InputRange>> v;
        RS_Detail::concat_helper(v, range, ranges...);
        return v;
    }

    template <typename Container>
    void ensure_min(Container& con, size_t n) {
        if (con.size() < n)
            con.resize(n);
    }

    template <typename Container, typename T>
    void ensure_min(Container& con, size_t n, const T& t) {
        if (con.size() < n)
            con.resize(n, t);
    }

    template <typename Container>
    void ensure_max(Container& con, size_t n) {
        if (con.size() > n)
            con.resize(n);
    }

    template <typename T, typename F>
    void do_n(T n, F f) {
        static_assert(std::is_integral<T>::value);
        for (T i = T(0); i < n; ++i)
            f();
    }

    template <typename T, typename F>
    void for_n(T n, F f) {
        static_assert(std::is_integral<T>::value);
        for (T i = T(0); i < n; ++i)
            f(i);
    }

    template <typename M, typename K, typename T>
    typename M::mapped_type find_in_map(const M& map, const K& key, const T& def) {
        auto i = map.find(key);
        if (i == map.end())
            return def;
        else
            return i->second;
    }

    template <typename M, typename K>
    typename M::mapped_type find_in_map(const M& map, const K& key) {
        typename M::mapped_type t{};
        return find_in_map(map, key, t);
    }

    template <typename Range1, typename Range2, typename Compare>
    bool sets_intersect(const Range1& r1, const Range2& r2, Compare c) {
        using std::begin;
        using std::end;
        auto i = begin(r1), end1 = end(r1);
        auto j = begin(r2), end2 = end(r2);
        while (i != end1 && j != end2) {
            if (c(*i, *j))
                ++i;
            else if (c(*j, *i))
                ++j;
            else
                return true;
        }
        return false;
    }

    template <typename Range1, typename Range2>
    bool sets_intersect(const Range1& r1, const Range2& r2) {
        return sets_intersect(r1, r2, std::less<>());
    }

    inline void sort_list() noexcept {}
    template <typename T> inline void sort_list(T&) noexcept {}

    template <typename T>
    inline void sort_list(T& t1, T& t2) noexcept {
        using std::swap;
        if (t2 < t1)
            swap(t1, t2);
    }

    template <typename T, typename... Args>
    inline void sort_list(T& t, Args&... args) {
        static constexpr size_t N = sizeof...(Args) + 1;
        std::array<T, N> array{{t, args...}};
        std::sort(array.begin(), array.end());
        std::tie(t, args...) = array_to_tuple(array);
    }

    template <typename Compare> inline void sort_list_by(Compare) noexcept {}
    template <typename Compare, typename T> inline void sort_list_by(Compare, T&) noexcept {}

    template <typename Compare, typename T>
    inline void sort_list_by(Compare c, T& t1, T& t2) {
        using std::swap;
        if (c(t2, t1))
            swap(t1, t2);
    }

    template <typename Compare, typename T, typename... Args>
    inline void sort_list_by(Compare c, T& t, Args&... args) {
        static constexpr size_t N = sizeof...(Args) + 1;
        std::array<T, N> array{{t, args...}};
        std::sort(array.begin(), array.end(), c);
        std::tie(t, args...) = array_to_tuple(array);
    }

    template <typename InputRange>
    Meta::RangeValue<InputRange> sum_of(const InputRange& r) {
        auto t = Meta::RangeValue<InputRange>();
        for (auto& x: r)
            t = t + x;
        return t;
    }

    template <typename InputRange>
    Meta::RangeValue<InputRange> sum_of(InputRange&& r) {
        auto t = Meta::RangeValue<InputRange>();
        for (auto& x: r)
            t = t + x;
        return t;
    }

    template <typename InputRange>
    Meta::RangeValue<InputRange> product_of(const InputRange& r) {
        auto t = Meta::RangeValue<InputRange>(1);
        for (auto& x: r)
            t = t * x;
        return t;
    }

    template <typename InputRange>
    Meta::RangeValue<InputRange> product_of(InputRange&& r) {
        auto t = Meta::RangeValue<InputRange>(1);
        for (auto& x: r)
            t = t * x;
        return t;
    }

    // Memory algorithms

    int mem_compare(const void* lhs, size_t n1, const void* rhs, size_t n2) noexcept;
    size_t mem_match(const void* lhs, const void* rhs, size_t n) noexcept;
    void mem_swap(void* ptr1, void* ptr2, size_t n) noexcept;

    // Integer sequences

    template <typename T>
    class IntegerSequenceIterator:
    public RandomAccessIterator<IntegerSequenceIterator<T>, const T> {
    public:
        IntegerSequenceIterator() = default;
        IntegerSequenceIterator(T init, T delta): cur(init), del(delta) {}
        const T& operator*() const noexcept { return cur; }
        IntegerSequenceIterator& operator+=(ptrdiff_t n) noexcept { cur = T(cur + n * del); return *this; }
        ptrdiff_t operator-(const IntegerSequenceIterator& rhs) const noexcept { return (ptrdiff_t(cur) - ptrdiff_t(rhs.cur)) / ptrdiff_t(del); }
    private:
        T cur, del;
    };

    namespace RS_Detail {

        template <typename T>
        inline void adjust_integer_sequence(T& init, T& stop, T& delta, bool closed) noexcept {
            if (delta == T(0)) {
                stop = init + T(int(closed));
                delta = T(1);
                return;
            }
            if (stop != init && (stop > init) != (delta > T())) {
                stop = init;
                return;
            }
            T rem = (stop - init) % delta;
            if (rem != T())
                stop += delta - rem;
            else if (closed)
                stop += delta;
        }

    }

    template <typename T>
    Irange<IntegerSequenceIterator<T>> iseq(T init, T stop, T delta) noexcept {
        RS_Detail::adjust_integer_sequence(init, stop, delta, true);
        return {{init, delta}, {stop, delta}};
    }

    template <typename T>
    Irange<IntegerSequenceIterator<T>> iseq(T init, T stop) noexcept {
        T delta = stop >= init ? T(1) : T(-1);
        return iseq(init, stop, delta);
    }

    template <typename T>
    Irange<IntegerSequenceIterator<T>> iseq(T stop) noexcept {
        return iseq(T(), stop);
    }

    template <typename T>
    Irange<IntegerSequenceIterator<T>> xseq(T init, T stop, T delta) noexcept {
        RS_Detail::adjust_integer_sequence(init, stop, delta, false);
        return {{init, delta}, {stop, delta}};
    }

    template <typename T>
    Irange<IntegerSequenceIterator<T>> xseq(T init, T stop) noexcept {
        T delta = stop >= init ? T(1) : T(-1);
        return xseq(init, stop, delta);
    }

    template <typename T>
    Irange<IntegerSequenceIterator<T>> xseq(T stop) noexcept {
        return xseq(T(), stop);
    }

    // [Arithmetic functions]

    // Generic arithmetic functions

    namespace RS_Detail {

        template <typename T>
        T global_abs(T t) noexcept {
            using limits = std::numeric_limits<T>;
            if constexpr (! limits::is_specialized || limits::is_signed) {
                using std::abs;
                return abs(t);
            } else {
                return t;
            }
        }

        template <typename T>
        struct NumberMode {
            static constexpr char value =
                std::is_floating_point<T>::value ? 'F' :
                std::is_signed<T>::value ? 'S' :
                std::is_unsigned<T>::value ? 'U' : 'X';
        };

        template <typename T, bool Symmetric = false, char Mode = NumberMode<T>::value>
        struct Divide {
            std::pair<T, T> operator()(T x, T y) const noexcept {
                return {x / y, x % y};
            }
        };

        template <typename T>
        struct Divide<T, false, 'S'> {
            std::pair<T, T> operator()(T x, T y) const noexcept {
                using std::abs;
                auto q = x / y, r = x % y;
                if (r < T(0)) {
                    q += y < T(0) ? T(1) : T(-1);
                    r += abs(y);
                }
                return {q, r};
            }
        };

        template <typename T>
        struct Divide<T, false, 'F'> {
            std::pair<T, T> operator()(T x, T y) const noexcept {
                using std::abs;
                auto q = std::floor(x / y), r = std::fmod(x, y);
                if (r < T(0))
                    r += abs(y);
                if (y < T(0) && r != T(0))
                    q += T(1);
                return {q, r};
            }
        };

        template <typename T, char Mode>
        struct Divide<T, true, Mode> {
            std::pair<T, T> operator()(T x, T y) const noexcept {
                static_assert(Mode != 'U', "Symmetric division on unsigned type");
                using std::abs;
                auto qr = Divide<T>()(x, y);
                if (qr.second > abs(y) / T(2)) {
                    qr.first += y > T(0) ? T(1) : T(-1);
                    qr.second -= abs(y);
                }
                return qr;
            }
        };

        template <typename T> constexpr T unsigned_gcd(T a, T b) noexcept { return b == T(0) ? a : unsigned_gcd(b, a % b); }

        template <typename T, char Mode = NumberMode<T>::value>
        struct ShiftLeft;

        template <typename T>
        struct ShiftLeft<T, 'S'> {
            static constexpr int maxbits = 8 * sizeof(T);
            T operator()(T t, int n) const {
                if (n <= - maxbits || n >= maxbits)
                    return 0;
                else if (n >= 0)
                    return t << n;
                else if (t < 0)
                    return - (- t >> - n);
                else
                    return t >> - n;
            }
        };

        template <typename T>
        struct ShiftLeft<T, 'U'> {
            static constexpr int maxbits = 8 * sizeof(T);
            T operator()(T t, int n) const {
                if (n <= - maxbits || n >= maxbits)
                    return 0;
                else if (n >= 0)
                    return t << n;
                else
                    return t >> - n;
            }
        };

        template <typename T>
        struct ShiftLeft<T, 'F'> {
            T operator()(T t, int n) const {
                using std::ldexp;
                return ldexp(t, n);
            }
        };

        template <typename T, char Mode = NumberMode<T>::value>
        struct SignOf;

        template <typename T>
        struct SignOf<T, 'S'> {
            constexpr int operator()(T t) const noexcept { return t < T(0) ? -1 : t == T(0) ? 0 : 1; }
        };

        template <typename T>
        struct SignOf<T, 'U'> {
            constexpr int operator()(T t) const noexcept { return t != T(0); }
        };

        template <typename T>
        struct SignOf<T, 'F'> {
            constexpr int operator()(T t) const noexcept { return t < T(0) ? -1 : t == T(0) ? 0 : 1; }
        };

    }

    template <typename T> std::pair<T, T> divide(T x, T y) noexcept { return RS_Detail::Divide<T>()(x, y); }
    template <typename T> T quo(T x, T y) noexcept { return divide(x, y).first; }
    template <typename T> T rem(T x, T y) noexcept { return divide(x, y).second; }
    template <typename T> std::pair<T, T> symmetric_divide(T x, T y) noexcept { return RS_Detail::Divide<T, true>()(x, y); }
    template <typename T> T symmetric_quotient(T x, T y) noexcept { return symmetric_divide(x, y).first; }
    template <typename T> T symmetric_remainder(T x, T y) noexcept { return symmetric_divide(x, y).second; }
    template <typename T> T shift_left(T t, int n) noexcept { return RS_Detail::ShiftLeft<T>()(t, n); }
    template <typename T> T shift_right(T t, int n) noexcept { return RS_Detail::ShiftLeft<T>()(t, - n); }
    template <typename T> int sign_of(T t) noexcept { return RS_Detail::SignOf<T>()(t); }

    // Integer arithmetic functions

    double xbinomial(int a, int b) noexcept;

    template <typename T>
    T binomial(T a, T b) noexcept {
        if (b < T(0) || b > a)
            return T(0);
        if (b == T(0) || b == a)
            return T(1);
        if (b > a / T(2))
            b = a - b;
        T n = T(1), d = T(1);
        while (b > 0) {
            n *= a--;
            d *= b--;
        }
        return n / d;
    }

    template <typename T>
    constexpr T gcd(T a, T b) noexcept {
        using namespace RS_Detail;
        return RS_Detail::unsigned_gcd(global_abs(a), global_abs(b));
    }

    template <typename T>
    constexpr T lcm(T a, T b) noexcept {
        using namespace RS_Detail;
        return a == T(0) || b == T(0) ? T(0) : global_abs((a / gcd(a, b)) * b);
    }

    template <typename T>
    T int_power(T x, T y) noexcept {
        T z = T(1);
        while (y) {
            if (y & T(1))
                z *= x;
            x *= x;
            y >>= 1;
        }
        return z;
    }

    template <typename T>
    T int_sqrt(T t) noexcept {
        if constexpr (std::numeric_limits<T>::digits < std::numeric_limits<double>::digits) {
            return T(std::floor(std::sqrt(double(t))));
        } else {
            auto u = as_unsigned(t);
            using U = decltype(u);
            U result = 0, test = U(1) << (8 * sizeof(U) - 2);
            while (test > u)
                test >>= 2;
            while (test) {
                if (u >= result + test) {
                    u -= result + test;
                    result += test * 2;
                }
                result >>= 1;
                test >>= 2;
            }
            return T(result);
        }
    }

    // [Functional utilities]

    // Function traits

    // Based on code by Kennytm and Victor Laskin
    // http://stackoverflow.com/questions/7943525/is-it-possible-to-figure-out-the-parameter-type-and-return-type-of-a-lambda
    // http://vitiy.info/c11-functional-decomposition-easy-way-to-do-aop/

    namespace RS_Detail {

        template <typename Function>
        struct FunctionTraits:
        FunctionTraits<decltype(&Function::operator())> {};

        template <typename ReturnType, typename... Args>
        struct FunctionTraits<ReturnType (Args...)> {
            static constexpr size_t arity = sizeof...(Args);
            using argument_tuple = std::tuple<Args...>;
            using return_type = ReturnType;
            using signature = return_type(Args...);
            using std_function = std::function<signature>;
        };

        template <typename ReturnType, typename... Args>
        struct FunctionTraits<ReturnType (*)(Args...)>:
        FunctionTraits<ReturnType (Args...)> {};

        template <typename ClassType, typename ReturnType, typename... Args>
        struct FunctionTraits<ReturnType (ClassType::*)(Args...)>:
        FunctionTraits<ReturnType (Args...)> {};

        template <typename ClassType, typename ReturnType, typename... Args>
        struct FunctionTraits<ReturnType (ClassType::*)(Args...) const>:
        FunctionTraits<ReturnType (Args...)> {};

    }

    template <typename Function> struct Arity { static constexpr size_t value = RS_Detail::FunctionTraits<Function>::arity; };
    template <typename Function> using ArgumentTuple = typename RS_Detail::FunctionTraits<Function>::argument_tuple;
    template <typename Function, size_t Index> using ArgumentType = std::tuple_element_t<Index, ArgumentTuple<Function>>;
    template <typename Function> using ReturnType = typename RS_Detail::FunctionTraits<Function>::return_type;
    template <typename Function> using FunctionSignature = typename RS_Detail::FunctionTraits<Function>::signature;
    template <typename Function> using StdFunction = typename RS_Detail::FunctionTraits<Function>::std_function;

    // Function operations

    template <typename Function> StdFunction<Function> stdfun(Function& lambda) { return StdFunction<Function>(lambda); }

    // tuple_invoke() is based on code from Cppreference.com
    // http://en.cppreference.com/w/cpp/utility/integer_sequence

    namespace RS_Detail {

        template <typename Function, typename Tuple, size_t... Indices>
        decltype(auto) invoke_helper(Function&& f, Tuple&& t, std::index_sequence<Indices...>) {
            (void)t; // MSVC bug
            return f(std::get<Indices>(std::forward<Tuple>(t))...);
        }

    }

    template<typename Function, typename Tuple>
    decltype(auto) tuple_invoke(Function&& f, Tuple&& t) {
        constexpr auto size = std::tuple_size<std::decay_t<Tuple>>::value;
        return RS_Detail::invoke_helper(std::forward<Function>(f), std::forward<Tuple>(t), std::make_index_sequence<size>{});
    }

    // Generic function objects

    template <typename Signature>
    class CallRef {
    public:
        using function_type = std::function<Signature>;
        using signature_type = Signature;
        CallRef() = default;
        CallRef(std::nullptr_t) noexcept: fptr() {}
        template <typename F> CallRef(F f): fptr(std::make_shared<function_type>(f)) {}
        explicit operator bool() const noexcept { return fptr && *fptr; }
        template <typename... Args> auto operator()(Args... args) const {
            if (! fptr)
                throw std::bad_function_call();
            return (*fptr)(std::forward<Args>(args)...);
        }
    private:
        std::shared_ptr<function_type> fptr;
    };

    struct DoNothing {
        void operator()() const noexcept {}
        template <typename T> void operator()(T&) const noexcept {}
        template <typename T> void operator()(const T&) const noexcept {}
    };

    struct Identity {
        template <typename T> T& operator()(T& t) const noexcept { return t; }
        template <typename T> const T& operator()(const T& t) const noexcept { return t; }
    };

    constexpr DoNothing do_nothing {};
    constexpr Identity identity {};

    struct RangeEqual {
        template <typename Range> bool operator()(const Range& r1, const Range& r2) const {
            using std::begin;
            using std::end;
            return std::equal(begin(r1), end(r1), begin(r2), end(r2));
        }
    };

    struct RangeCompare {
        template <typename Range> bool operator()(const Range& r1, const Range& r2) const {
            using std::begin;
            using std::end;
            return std::lexicographical_compare(begin(r1), end(r1), begin(r2), end(r2));
        }
    };

    constexpr RangeEqual range_equal {};
    constexpr RangeCompare range_compare {};

    // Hash functions

    template <typename T>
    inline size_t std_hash(const T& t) noexcept {
        return std::hash<T>()(t);
    }

    inline size_t hash_mix(size_t h1, size_t h2) noexcept {
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }

    inline void hash_combine(size_t&) noexcept {}

    template <typename T>
    void hash_combine(size_t& hash, const T& t) noexcept {
        hash = hash_mix(hash, std_hash(t));
    }

    template <typename T, typename... Args>
    void hash_combine(size_t& hash, const T& t, const Args&... args) noexcept {
        hash_combine(hash, t);
        hash_combine(hash, args...);
    }

    template <typename... Args>
    size_t hash_value(const Args&... args) noexcept {
        size_t hash = 0;
        hash_combine(hash, args...);
        return hash;
    }

    template <typename InputRange>
    size_t hash_range(const InputRange& range) noexcept {
        size_t hash = 0;
        for (auto&& x: range)
            hash_combine(hash, x);
        return hash;
    }

    template <typename InputRange>
    void hash_range(size_t& hash, const InputRange& range) noexcept {
        for (auto&& x: range)
            hash_combine(hash, x);
    }

    template <typename... Args> struct TupleHash { size_t operator()(const std::tuple<Args...>& t) const { return tuple_invoke(hash_value<Args...>, t); } };
    template <typename... Args> struct TupleHash<std::tuple<Args...>>: TupleHash<Args...> {};

    // Scope guards

    namespace RS_Detail {

        template <typename T, int Def, bool = std::is_constructible<T, int>::value> struct ResourceDefault;
        template <typename T, int Def> struct ResourceDefault<T, Def, true> { static T get() { return T(Def); } };
        template <typename T> struct ResourceDefault<T, 0, false> { static T get() { return T(); } };

    }

    template <typename T, int Def = 0>
    class Resource {
    public:
        using delete_function = std::function<void(T&)>;
        using resource_type = T;
        Resource() = default;
        explicit Resource(T t): res(t), del() {}
        template <typename Del> Resource(T t, Del d):
            res(t), del() {
                try { del = delete_function(d); }
                catch (...) { d(res); throw; }
            }
        Resource(Resource&& r) noexcept: res(std::exchange(r.res, def())), del(std::exchange(r.del, nullptr)) {}
        ~Resource() noexcept { reset(); }
        Resource& operator=(Resource&& r) noexcept {
            reset();
            res = std::exchange(r.res, def());
            del = std::exchange(r.del, nullptr);
            return *this;
        }
        explicit operator bool() const noexcept { return res != def(); }
        T& get() noexcept { return res; }
        T get() const noexcept { return res; }
        T release() noexcept { del = nullptr; return std::exchange(res, def()); }
        void reset() noexcept {
            if (del && res != def()) {
                try { del(res); }
                catch (...) {}
            }
            del = nullptr;
            res = def();
        }
        void reset(T t) noexcept { reset(); res = t; }
        static T def() noexcept { return RS_Detail::ResourceDefault<T, Def>::get(); }
    private:
        T res = def();
        delete_function del;
        Resource(const Resource&) = delete;
        Resource& operator=(const Resource&) = delete;
    };

    template <typename T>
    class Resource<T*> {
    public:
        using delete_function = std::function<void(T*)>;
        using resource_type = T*;
        using value_type = T;
        Resource() = default;
        explicit Resource(T* t): res(t), del() {}
        template <typename Del> Resource(T* t, Del d): res(t), del() {
            try { del = delete_function(d); }
            catch (...) { if (res) d(res); throw; }
        }
        Resource(Resource&& r) noexcept: res(std::exchange(r.res, nullptr)), del(std::exchange(r.del, nullptr)) {}
        ~Resource() noexcept { reset(); }
        Resource& operator=(Resource&& r) noexcept {
            reset();
            res = std::exchange(r.res, nullptr);
            del = std::exchange(r.del, nullptr);
            return *this;
        }
        explicit operator bool() const noexcept { return res != nullptr; }
        T& operator*() noexcept { return *res; }
        const T& operator*() const noexcept { return *res; }
        T* operator->() const noexcept { return res; }
        T*& get() noexcept { return res; }
        T* get() const noexcept { return res; }
        T* release() noexcept { del = nullptr; return std::exchange(res, nullptr); }
        void reset() noexcept {
            if (del && res) {
                try { del(res); }
                catch (...) {}
            }
            del = nullptr;
            res = nullptr;
        }
        void reset(T* t) noexcept { reset(); res = t; }
        static T* def() noexcept { return nullptr; }
    private:
        T* res = nullptr;
        delete_function del = nullptr;
        Resource(const Resource&) = delete;
        Resource& operator=(const Resource&) = delete;
    };

    template <>
    class Resource<void*> {
    public:
        using delete_function = std::function<void(void*)>;
        using resource_type = void*;
        using value_type = void;
        Resource() = default;
        explicit Resource(void* t): res(t), del() {}
        template <typename Del> Resource(void* t, const Del& d): res(t) {
            try { del = delete_function(d); }
            catch (...) { if (res) d(res); throw; }
        }
        Resource(Resource&& r) noexcept: res(std::exchange(r.res, def())), del(std::exchange(r.del, nullptr)) {}
        ~Resource() noexcept { reset(); }
        Resource& operator=(Resource&& r) noexcept {
            reset();
            res = std::exchange(r.res, nullptr);
            del = std::exchange(r.del, nullptr);
            return *this;
        }
        explicit operator bool() const noexcept { return res != nullptr; }
        void*& get() noexcept { return res; }
        void* get() const noexcept { return res; }
        void* release() noexcept { del = nullptr; return std::exchange(res, nullptr); }
        void reset() noexcept {
            if (del && res) {
                try { del(res); }
                catch (...) {}
            }
            del = nullptr;
            res = nullptr;
        }
        void reset(void* t) noexcept { reset(); res = t; }
        static void* def() noexcept { return nullptr; }
    private:
        void* res = nullptr;
        delete_function del = nullptr;
        Resource(const Resource&) = delete;
        Resource& operator=(const Resource&) = delete;
    };

    template <typename T, typename Del>
    Resource<T> make_resource(T t, Del d) {
        return {t, d};
    }

    class ScopedTransaction {
    public:
        RS_NO_COPY_MOVE(ScopedTransaction)
        using callback = std::function<void()>;
        ScopedTransaction() noexcept {}
        ~ScopedTransaction() noexcept { rollback(); }
        void operator()(callback func, callback undo) {
            stack.push_back(nullptr);
            if (func)
                func();
            stack.back().swap(undo);
        }
        void commit() noexcept { stack.clear(); }
        void rollback() noexcept {
            for (auto i = stack.rbegin(); i != stack.rend(); ++i)
                if (*i) try { (*i)(); } catch (...) {}
            stack.clear();
        }
    private:
        std::vector<callback> stack;
    };

    // [I/O utilities]

    // Logging

    namespace RS_Detail {

        void log_message(const Ustring& msg);

    }

    void logx(const Ustring& msg) noexcept;
    void logx(const char* msg) noexcept;

    template <typename... Args>
    void logx(Args... args) noexcept {
        try {
            Strings v{to_str(args)...};
            Ustring msg;
            for (auto& s: v)
                msg += s += ' ';
            if (! msg.empty())
                msg.pop_back();
            RS_Detail::log_message(msg);
        }
        catch (...) {}
    }

    // [Multithreading]

    // Thread class

    class Thread:
    public std::thread {
    public:
        Thread() = default;
        template <typename F, typename... Args> explicit Thread(F&& f, Args&&... args): std::thread(std::forward<F>(f), std::forward<Args>(args)...) {}
        ~Thread() noexcept { close(); }
        Thread(const Thread&) = delete;
        Thread(Thread&&) = default;
        Thread& operator=(const Thread&) = delete;
        Thread& operator=(Thread&& t) noexcept { close(); std::thread::operator=(std::move(t)); return *this; }
        void swap(Thread& t) noexcept { t.std::thread::swap(*this); }
    private:
        void close() noexcept { try { if (joinable()) join(); } catch (...) {} }
    };

}

namespace std {

    template <typename T, RS::ByteOrder B> struct hash<RS::Endian<T, B>> {
        size_t operator()(RS::Endian<T, B> t) const noexcept { return std::hash<T>()(t.get()); }
    };

}
