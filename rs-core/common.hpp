#pragma once

// Compilation environment control macros

#ifdef __APPLE__
    #ifndef _DARWIN_C_SOURCE
        #define _DARWIN_C_SOURCE 1
    #endif
#endif

#if defined(__CYGWIN__) || ! defined(_WIN32)
    #ifndef _XOPEN_SOURCE
        #define _XOPEN_SOURCE 700 // Posix 2008
    #endif
    #ifndef _REENTRANT
        #define _REENTRANT 1
    #endif
#endif

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX 1
    #endif
    #ifndef UNICODE
        #define UNICODE 1
    #endif
    #ifndef _UNICODE
        #define _UNICODE 1
    #endif
    #ifndef WINVER
        #define WINVER 0x601 // Windows 7
    #endif
    #ifndef _WIN32_WINNT
        #define _WIN32_WINNT 0x601
    #endif
#endif

// Includes go here so anything that needs the macros above will see them

#include <algorithm>
#include <array>
#include <cerrno>
#include <chrono>
#include <clocale>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cwchar>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <mutex>
#include <ostream>
#include <random>
#include <sstream>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
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

// Temporary measure until all my compilers upport std::string_view

#if defined(__GNUC__) && ! defined(__clang__) && __GNUC__ < 7
    #include <experimental/string_view>
    using std::experimental::basic_string_view;
    using std::experimental::string_view;
    using std::experimental::u16string_view;
    using std::experimental::u32string_view;
    using std::experimental::wstring_view;
#else
    #include <string_view>
    using std::basic_string_view;
    using std::string_view;
    using std::u16string_view;
    using std::u32string_view;
    using std::wstring_view;
#endif

// GNU brain damage

#ifdef __GNUC__
    #ifdef major
        #undef major
    #endif
    #ifdef minor
        #undef minor
    #endif
#endif

// Microsoft brain damage

#ifdef _MSC_VER
    #pragma warning(disable: 4127) // conditional expression is constant
    #pragma warning(disable: 4310) // cast truncates constant value
#endif

// Other preprocessor macros

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

#ifdef __GNUC__
    #define RS_ATTR_UNUSED __attribute__((__unused__))
#else
    #define RS_ATTR_UNUSED
#endif

#define RS_ENUM_IMPLEMENTATION(EnumType, IntType, class_tag, name_prefix, first_value, first_name, ...) \
    enum class_tag EnumType: IntType { first_name = first_value, __VA_ARGS__, RS_enum_sentinel }; \
    inline RS_ATTR_UNUSED std::ostream& operator<<(std::ostream& out, EnumType t) { \
        ::RS::RS_Detail::write_enum(out, t, first_value, name_prefix, #first_name "," #__VA_ARGS__); \
        return out; \
    } \
    constexpr RS_ATTR_UNUSED bool enum_is_valid(EnumType t) noexcept { \
        return IntType(t) >= IntType(first_value) && IntType(t) < IntType(EnumType::RS_enum_sentinel); \
    } \
    inline RS_ATTR_UNUSED std::vector<EnumType> RS_enum_values(EnumType) { \
        IntType n = IntType(EnumType::RS_enum_sentinel) - IntType(first_value); \
        std::vector<EnumType> v(size_t(n), {}); \
        for (IntType i = 0; i < n; ++i) \
            v[i] = EnumType(first_value + i); \
        return v; \
    }

#define RS_ENUM(EnumType, IntType, first_value, first_name, ...) \
    RS_ENUM_IMPLEMENTATION(EnumType, IntType,, "", first_value, first_name, __VA_ARGS__)
#define RS_ENUM_CLASS(EnumType, IntType, first_value, first_name, ...) \
    RS_ENUM_IMPLEMENTATION(EnumType, IntType, class, #EnumType "::", first_value, first_name, __VA_ARGS__)

#define RS_LDLIB(libs)

#define RS_MOVE_ONLY(T) \
    T(const T&) = delete; \
    T(T&&) = default; \
    T& operator=(const T&) = delete; \
    T& operator=(T&&) = default;
#define RS_NO_COPY_MOVE(T) \
    T(const T&) = delete; \
    T(T&&) = delete; \
    T& operator=(const T&) = delete; \
    T& operator=(T&&) = delete;

// For internal use only
// Must be used in the global namespace
#define RS_DEFINE_STD_HASH(T) \
    namespace std { \
        template <> struct hash<T> { \
            using argument_type = T; \
            using result_type = size_t; \
            size_t operator()(const T& t) const noexcept { return t.hash(); } \
        }; \
    }

namespace RS {

    // Things needed early

    #ifdef _XOPEN_SOURCE
        using NativeCharacter = char;
    #else
        #define RS_NATIVE_WCHAR 1
        using NativeCharacter = wchar_t;
    #endif

    #if WCHAR_MAX < 0x7fffffff
        #define RS_WCHAR_UTF16 1
        using WcharEquivalent = char16_t;
    #else
        #define RS_WCHAR_UTF32 1
        using WcharEquivalent = char32_t;
    #endif

    using Ustring = std::string;
    using Uview = string_view;
    using Strings = std::vector<std::string>;
    using NativeString = std::basic_string<NativeCharacter>;
    using WstringEquivalent = std::basic_string<WcharEquivalent>;

    #if defined(_WIN32) || (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
        constexpr bool big_endian_target = false;
        constexpr bool little_endian_target = true;
    #else
        constexpr bool big_endian_target = true;
        constexpr bool little_endian_target = false;
    #endif

    namespace RS_Detail {

        template <typename> struct CommonTrue: std::true_type {};
        template <typename T> auto common_adl_begin(int) -> CommonTrue<decltype(begin(std::declval<T>()))>;
        template <typename T> auto common_adl_begin(long) -> std::false_type;
        template <typename T> auto common_std_begin(int) -> CommonTrue<decltype(std::begin(std::declval<T>()))>;
        template <typename T> auto common_std_begin(long) -> std::false_type;
        template <typename T> struct CommonAdlBegin: decltype(common_adl_begin<T>(0)) {};
        template <typename T> struct CommonStdBegin: decltype(common_std_begin<T>(0)) {};
        template <typename T> struct CommonRangeType { static constexpr bool value = CommonAdlBegin<T>::value || CommonStdBegin<T>::value; };

        template <typename R, bool A = CommonAdlBegin<R>::value, bool S = CommonStdBegin<R>::value> struct CommonRangeTraits;
        template <typename R, bool S> struct CommonRangeTraits<R, true, S> {
            using iterator = decltype(begin(std::declval<R&>()));
            using value_type = typename std::iterator_traits<iterator>::value_type;
        };
        template <typename R> struct CommonRangeTraits<R, false, true> {
            using iterator = decltype(std::begin(std::declval<R&>()));
            using value_type = typename std::iterator_traits<iterator>::value_type;
        };

        // https://stackoverflow.com/questions/41207774/how-do-i-create-a-tuple-of-n-ts-from-an-array-of-t
        template <size_t... Sequence, typename Array>
        constexpr auto array_to_tuple_helper(const Array& array, std::index_sequence<Sequence...>) {
            return std::make_tuple(array[Sequence]...);
        }

    }

    template <typename Range> using RangeIterator = typename RS_Detail::CommonRangeTraits<Range>::iterator;
    template <typename Range> using RangeValue = typename RS_Detail::CommonRangeTraits<Range>::value_type;

    constexpr const char* ascii_whitespace = "\t\n\v\f\r ";
    constexpr size_t npos = size_t(-1);

    template <typename T, size_t N>
    constexpr auto array_to_tuple(const T (&array)[N]) {
        return RS_Detail::array_to_tuple_helper(array, std::make_index_sequence<N>{});
    }

    template <typename T, size_t N>
    constexpr auto array_to_tuple(const std::array<T, N> &array) {
        return RS_Detail::array_to_tuple_helper(array, std::make_index_sequence<N>{});
    }

    template <typename T> constexpr auto as_signed(T t) noexcept { return static_cast<std::make_signed_t<T>>(t); }
    template <typename T> constexpr auto as_unsigned(T t) noexcept { return static_cast<std::make_unsigned_t<T>>(t); }
    template <typename T> std::vector<T> enum_values() { return RS_enum_values(T()); }

    template <typename C>
    std::basic_string<C> cstr(const C* ptr) {
        using S = std::basic_string<C>;
        return ptr ? S(ptr) : S();
    }

    template <typename C>
    std::basic_string<C> cstr(const C* ptr, size_t n) {
        using S = std::basic_string<C>;
        return ptr ? S(ptr, n) : S(n, '\0');
    }

    template <typename C>
    size_t cstr_size(const C* ptr) {
        if (! ptr)
            return 0;
        if (sizeof(C) == 1)
            return std::strlen(reinterpret_cast<const char*>(ptr));
        if (sizeof(C) == sizeof(wchar_t))
            return std::wcslen(reinterpret_cast<const wchar_t*>(ptr));
        size_t n = 0;
        while (ptr[n] != C(0))
            ++n;
        return n;
    }

    namespace RS_Detail {

        inline void append_hex_byte(uint8_t b, std::string& s) {
            static constexpr const char* digits = "0123456789abcdef";
            s += digits[b / 16];
            s += digits[b % 16];
        }

        inline int decode_hex_byte(std::string::const_iterator& i, std::string::const_iterator end) noexcept {
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

        template <typename T, bool = std::is_signed<T>::value> struct SimpleAbs { constexpr T operator()(T t) const noexcept { return t; } };
        template <typename T> struct SimpleAbs<T, true> { constexpr T operator()(T t) const noexcept { return t < T(0) ? - t : t; } };

        template <typename T>
        Ustring int_to_string(T x, int base, size_t digits) {
            bool neg = x < T(0);
            auto b = as_unsigned(T(base));
            auto y = as_unsigned(SimpleAbs<T>()(x));
            Ustring s;
            do {
                auto d = int(y % b);
                s += char((d < 10 ? '0' : 'a' - 10) + d);
                y /= b;
            } while (y || s.size() < digits);
            if (neg)
                s += '-';
            std::reverse(s.begin(), s.end());
            return s;
        }

        template <typename T> Ustring decfmt(T x, size_t digits = 1) { return RS_Detail::int_to_string(x, 10, digits); }
        template <typename T> Ustring hexfmt(T x, size_t digits = 2 * sizeof(T)) { return RS_Detail::int_to_string(x, 16, digits); }

        template <typename EnumType>
        void write_enum(std::ostream& out, EnumType t, long first_value, const char* prefix, const char* names) {
            using U = std::underlying_type_t<EnumType>;
            size_t index = U(t) - first_value;
            for (size_t i = 0; i < index; ++i) {
                names = strchr(names, ',');
                if (names == nullptr) {
                    out << U(t);
                    return;
                }
                names += strspn(names, " ,");
            }
            out << prefix;
            out.write(names, ptrdiff_t(strcspn(names, " ,")));
        }

    }

    // [Types]

    // Mixins

    template <typename T>
    struct EqualityComparable {
        friend bool operator!=(const T& lhs, const T& rhs) noexcept { return ! (lhs == rhs); }
    };

    template <typename T>
    struct LessThanComparable:
    EqualityComparable<T> {
        friend bool operator>(const T& lhs, const T& rhs) noexcept { return rhs < lhs; }
        friend bool operator<=(const T& lhs, const T& rhs) noexcept { return ! (rhs < lhs); }
        friend bool operator>=(const T& lhs, const T& rhs) noexcept { return ! (lhs < rhs); }
    };

    template <typename T, typename CV>
    struct InputIterator:
    EqualityComparable<T> {
        using difference_type = ptrdiff_t;
        using iterator_category = std::input_iterator_tag;
        using pointer = CV*;
        using reference = CV&;
        using value_type = std::remove_const_t<CV>;
        CV* operator->() const noexcept { return &*static_cast<const T&>(*this); }
        friend T operator++(T& t, int) { T rc = t; ++t; return rc; }
    };

    template <typename T>
    struct OutputIterator {
        using difference_type = void;
        using iterator_category = std::output_iterator_tag;
        using pointer = void;
        using reference = void;
        using value_type = void;
        T& operator*() noexcept { return static_cast<T&>(*this); }
        friend T& operator++(T& t) noexcept { return t; }
        friend T operator++(T& t, int) noexcept { return t; }
    };

    template <typename T, typename CV>
    struct ForwardIterator:
    InputIterator<T, CV> {
        using iterator_category = std::forward_iterator_tag;
    };

    template <typename T, typename CV>
    struct BidirectionalIterator:
    ForwardIterator<T, CV> {
        using iterator_category = std::bidirectional_iterator_tag;
        friend T operator--(T& t, int) { T rc = t; --t; return rc; }
    };

    template <typename T, typename CV>
    struct RandomAccessIterator:
    BidirectionalIterator<T, CV>,
    LessThanComparable<T> {
        using iterator_category = std::random_access_iterator_tag;
        CV& operator[](ptrdiff_t i) const noexcept { T t = static_cast<const T&>(*this); t += i; return *t; }
        friend T& operator++(T& t) { t += 1; return t; }
        friend T& operator--(T& t) { t += -1; return t; }
        friend T& operator-=(T& lhs, ptrdiff_t rhs) { return lhs += - rhs; }
        friend T operator+(const T& lhs, ptrdiff_t rhs) { T t = lhs; t += rhs; return t; }
        friend T operator+(ptrdiff_t lhs, const T& rhs) { T t = rhs; t += lhs; return t; }
        friend T operator-(const T& lhs, ptrdiff_t rhs) { T t = lhs; t -= rhs; return t; }
        friend bool operator==(const T& lhs, const T& rhs) noexcept { return lhs - rhs == 0; }
        friend bool operator<(const T& lhs, const T& rhs) noexcept { return lhs - rhs < 0; }
    };

    template <typename T, typename CV>
    struct FlexibleRandomAccessIterator:
    BidirectionalIterator<T, CV>,
    LessThanComparable<T> {
        using iterator_category = std::random_access_iterator_tag;
        CV& operator[](ptrdiff_t i) const noexcept { T t = static_cast<const T&>(*this); t += i; return *t; }
        friend T& operator-=(T& lhs, ptrdiff_t rhs) { return lhs += - rhs; }
        friend T operator+(const T& lhs, ptrdiff_t rhs) { T t = lhs; t += rhs; return t; }
        friend T operator+(ptrdiff_t lhs, const T& rhs) { T t = rhs; t += lhs; return t; }
        friend T operator-(const T& lhs, ptrdiff_t rhs) { T t = lhs; t -= rhs; return t; }
        friend bool operator<(const T& lhs, const T& rhs) noexcept { return lhs - rhs < 0; }
    };

    // Endian integers

    enum ByteOrder {
        big_endian = 1,
        little_endian = 2,
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

    template <typename Container, typename T>
    void append_to(Container& con, const T& t) {
        con.insert(con.end(), t);
    }

    template <typename Container>
    class AppendIterator:
    public OutputIterator<AppendIterator<Container>> {
    public:
        using value_type = typename Container::value_type;
        AppendIterator() = default;
        explicit AppendIterator(Container& c): con(&c) {}
        AppendIterator& operator=(const value_type& v) { append_to(*con, v); return *this; }
    private:
        Container* con;
    };

    template <typename Container> AppendIterator<Container> append(Container& con) { return AppendIterator<Container>(con); }
    template <typename Container> AppendIterator<Container> overwrite(Container& con) { con.clear(); return AppendIterator<Container>(con); }

    template <typename Range1, typename Range2, typename Compare>
    int compare_3way(const Range1& r1, const Range2& r2, Compare cmp) {
        using std::begin;
        using std::end;
        auto i = begin(r1), e1 = end(r1);
        auto j = begin(r2), e2 = end(r2);
        for (; i != e1 && j != e2; ++i, ++j) {
            if (cmp(*i, *j))
                return -1;
            else if (cmp(*j, *i))
                return 1;
        }
        return i != e1 ? 1 : j != e2 ? -1 : 0;
    }

    template <typename Range1, typename Range2>
    int compare_3way(const Range1& r1, const Range2& r2) {
        return compare_3way(r1, r2, std::less<>());
    }

    template <typename Range, typename Container>
    void con_append(const Range& src, Container& dst) {
        using std::begin;
        using std::end;
        std::copy(begin(src), end(src), append(dst));
    }

    template <typename Range, typename Container>
    void con_overwrite(const Range& src, Container& dst) {
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

    template <typename Range, typename... Ranges>
    std::vector<RangeValue<Range>> concatenate(const Range& range, const Ranges&... ranges) {
        std::vector<RangeValue<Range>> v;
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

    template <typename Range>
    RangeValue<Range> sum_of(const Range& r) {
        auto t = RangeValue<Range>();
        for (auto& x: r)
            t = t + x;
        return t;
    }

    template <typename Range>
    RangeValue<Range> sum_of(Range&& r) {
        auto t = RangeValue<Range>();
        for (auto& x: r)
            t = t + x;
        return t;
    }

    template <typename Range>
    RangeValue<Range> product_of(const Range& r) {
        auto t = RangeValue<Range>(1);
        for (auto& x: r)
            t = t * x;
        return t;
    }

    template <typename Range>
    RangeValue<Range> product_of(Range&& r) {
        auto t = RangeValue<Range>(1);
        for (auto& x: r)
            t = t * x;
        return t;
    }

    // Memory algorithms

    inline int mem_compare(const void* lhs, size_t n1, const void* rhs, size_t n2) noexcept {
        if (! lhs || ! rhs)
            return rhs ? -1 : lhs ? 1 : 0;
        int rc = memcmp(lhs, rhs, std::min(n1, n2));
        return rc < 0 ? -1 : rc > 0 ? 1 : n1 < n2 ? -1 : n1 > n2 ? 1 : 0;
    }

    inline size_t mem_match(const void* lhs, const void* rhs, size_t n) noexcept {
        if (! (lhs && rhs))
            return 0;
        auto cp1 = static_cast<const char*>(lhs);
        auto cp2 = static_cast<const char*>(rhs);
        size_t i = 0;
        while (i < n && cp1[i] == cp2[i])
            ++i;
        return i;
    }

    inline void mem_swap(void* ptr1, void* ptr2, size_t n) noexcept {
        if (ptr1 == ptr2)
            return;
        uint8_t b;
        auto p = static_cast<uint8_t*>(ptr1), endp = p + n, q = static_cast<uint8_t*>(ptr2);
        while (p != endp) {
            b = *p;
            *p++ = *q;
            *q++ = b;
        }
    }

    // Range traits

    namespace RS_Detail {

        template <typename T> struct ArrayCount;
        template <typename T, size_t N> struct ArrayCount<T[N]> { static constexpr size_t value = N; };

    }

    template <typename T> constexpr size_t array_count(T&&) noexcept { return RS_Detail::ArrayCount<std::remove_reference_t<T>>::value; }

    template <typename Range>
    size_t range_count(const Range& r) {
        using std::begin;
        using std::end;
        return std::distance(begin(r), end(r));
    }

    template <typename Range>
    bool range_empty(const Range& r) {
        using std::begin;
        using std::end;
        return begin(r) == end(r);
    }

    // Range types

    template <typename Iterator>
    struct Irange {
        using iterator = Iterator;
        using value_type = typename std::iterator_traits<Iterator>::value_type;
        Iterator first, second;
        constexpr Iterator begin() const { return first; }
        constexpr Iterator end() const { return second; }
        constexpr bool empty() const { return first == second; }
        constexpr size_t size() const { return std::distance(first, second); }
    };

    template <typename Iterator> constexpr Irange<Iterator> irange(const Iterator& i, const Iterator& j) { return {i, j}; }
    template <typename Iterator> constexpr Irange<Iterator> irange(const std::pair<Iterator, Iterator>& p) { return {p.first, p.second}; }
    template <typename T> constexpr Irange<T*> array_range(T* ptr, size_t len) { return {ptr, ptr + len}; }

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
                auto q = x / y, r = x % y;
                if (r < T(0)) {
                    q += y < T(0) ? T(1) : T(-1);
                    r += std::abs(y);
                }
                return {q, r};
            }
        };

        template <typename T>
        struct Divide<T, false, 'F'> {
            std::pair<T, T> operator()(T x, T y) const noexcept {
                using std::abs;
                using std::floor;
                using std::fmod;
                auto q = floor(x / y), r = fmod(x, y);
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

    template <typename T, typename T2, typename T3>
    constexpr T clamp(const T& x, const T2& min, const T3& max) noexcept {
        static_assert(std::is_convertible<T2, T>::value && std::is_convertible<T3, T>::value);
        return x < T(min) ? T(min) : T(max) < x ? T(max) : x;
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

    inline double xbinomial(int a, int b) noexcept {
        if (b < 0 || b > a)
            return 0;
        if (b == 0 || b == a)
            return 1;
        if (b > a / 2)
            b = a - b;
        double n = 1, d = 1;
        while (b > 0) {
            n *= a--;
            d *= b--;
        }
        return n / d;
    }

    template <typename T>
    constexpr T gcd(T a, T b) noexcept {
        using std::abs;
        return RS_Detail::unsigned_gcd(abs(a), abs(b));
    }

    template <typename T>
    constexpr T lcm(T a, T b) noexcept {
        using std::abs;
        return a == T(0) || b == T(0) ? T(0) : abs((a / gcd(a, b)) * b);
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
        if (std::numeric_limits<T>::digits < std::numeric_limits<double>::digits)
            return T(floor(sqrt(double(t))));
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

    // Bitwise operations

    namespace RS_Detail {

        template <typename T>
        constexpr T rotl_helper(T t, int n) noexcept {
            static_assert(std::is_integral<T>::value);
            int tbits = 8 * sizeof(T);
            int nbits = n % tbits;
            return nbits ? T(t << nbits) | T(t >> (tbits - nbits)) : t;
        }

        template <typename T>
        constexpr T rotr_helper(T t, int n) noexcept {
            static_assert(std::is_integral<T>::value);
            int tbits = 8 * sizeof(T);
            int nbits = n % tbits;
            return nbits ? T(t >> nbits) | T(t << (tbits - nbits)) : t;
        }

    }

    #ifdef __GNUC__

        template <typename T>
        constexpr size_t ibits(T t) noexcept {
            static_assert(std::is_integral<T>::value);
            return __builtin_popcountll(uint64_t(t));
        }

        template <typename T>
        constexpr size_t ilog2p1(T t) noexcept {
            static_assert(std::is_integral<T>::value);
            return t ? 64 - __builtin_clzll(uint64_t(t)) : 0;
        }

    #else

        template <typename T>
        constexpr size_t ibits(T t) noexcept {
            static constexpr uint8_t bits16[16] = {0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4};
            size_t n = 0;
            for (; t; t >>= 4) { n += bits16[t & 0xf]; }
            return n;
        }

        template <typename T>
        constexpr size_t ilog2p1(T t) noexcept {
            size_t n = 0;
            for (; t > 0xff; t >>= 8) { n += 8; }
            for (; t; t >>= 1) { ++n; }
            return n;
        }

    #endif

    template <typename T>
    constexpr T ifloor2(T t) noexcept {
        static_assert(std::is_integral<T>::value);
        return t ? T(1) << (ilog2p1(t) - 1) : 0;
    }

    template <typename T>
    constexpr T iceil2(T t) noexcept {
        static_assert(std::is_integral<T>::value);
        return t > 1 ? T(1) << (ilog2p1(t - 1) - 1) << 1 : t;
    }

    template <typename T>
    constexpr bool ispow2(T t) noexcept {
        static_assert(std::is_integral<T>::value);
        return ibits(t) == 1;
    }

    constexpr uint64_t letter_to_mask(char c) noexcept {
        return c >= 'A' && c <= 'Z' ? 1ull << (c - 'A') : c >= 'a' && c <= 'z' ? 1ull << (c - 'a' + 26) : 0;
    }

    template <typename T>
    constexpr T rotl(T t, int n) noexcept {
        return n == 0 ? t : n < 0 ? RS_Detail::rotr_helper(t, - n) : RS_Detail::rotl_helper(t, n);
    }

    template <typename T>
    constexpr T rotr(T t, int n) noexcept {
        return n == 0 ? t : n < 0 ? RS_Detail::rotl_helper(t, - n) : RS_Detail::rotr_helper(t, n);
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

    // Hash functions

    class Djb2a {
    public:
        Djb2a& operator()(const void* ptr, size_t n) noexcept {
            auto p = static_cast<const uint8_t*>(ptr);
            if (p)
                for (size_t i = 0; i < n; ++i)
                    hash = ((hash << 5) + hash) ^ p[i];
            return *this;
        }
        Djb2a& operator()(const std::string& s) noexcept { return (*this)(s.data(), s.size()); }
        operator uint32_t() const noexcept { return hash; }
    private:
        uint32_t hash = 5381;
    };

    inline uint32_t djb2a(const void* ptr, size_t n) noexcept {
        Djb2a d;
        d(ptr, n);
        return d;
    }

    inline size_t hash_mix(size_t h1, size_t h2) noexcept {
        return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }

    inline void hash_combine(size_t&) noexcept {}

    template <typename T>
    void hash_combine(size_t& hash, const T& t) noexcept {
        hash = hash_mix(hash, std::hash<T>()(t));
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

    template <typename Range>
    size_t hash_range(const Range& range) noexcept {
        size_t hash = 0;
        for (auto&& x: range)
            hash_combine(hash, x);
        return hash;
    }

    template <typename Range>
    void hash_range(size_t& hash, const Range& range) noexcept {
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

    template <typename F, char Mode>
    class BasicScopeGuard {
    public:
        BasicScopeGuard(F&& f) try:
            func(std::forward<F>(f)), inflight(std::uncaught_exceptions()) {}
            catch (...) {
                if (Mode != 's')
                    try { f(); } catch (...) {}
                throw;
            }
        BasicScopeGuard(BasicScopeGuard&& sg) noexcept:
            func(std::move(sg.func)), inflight(std::exchange(sg.inflight, -1)) {}
        ~BasicScopeGuard() noexcept {
            if (inflight >= 0 && (Mode == 'e' || (Mode == 'f') == (std::uncaught_exceptions() > inflight)))
                try { func(); } catch (...) {}
        }
        BasicScopeGuard(const BasicScopeGuard&) = delete;
        BasicScopeGuard& operator=(const BasicScopeGuard&) = delete;
        BasicScopeGuard& operator=(BasicScopeGuard&&) = delete;
        void release() noexcept { inflight = -1; }
    private:
        F func;
        int inflight;
    };

    template <typename F> inline BasicScopeGuard<F, 'e'> scope_exit(F&& f) { return BasicScopeGuard<F, 'e'>(std::forward<F>(f)); }
    template <typename F> inline BasicScopeGuard<F, 'f'> scope_fail(F&& f) { return BasicScopeGuard<F, 'f'>(std::forward<F>(f)); }
    template <typename F> inline BasicScopeGuard<F, 's'> scope_success(F&& f) { return BasicScopeGuard<F, 's'>(std::forward<F>(f)); }

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

    template <typename T> inline auto make_lock(T& t) { return std::unique_lock<T>(t); }

    // [Things that need to go at the end because of dependencies]

    // Logging

    namespace RS_Detail {

        template <typename T>
        int hash_xcolour(T t) noexcept {
            using namespace std::chrono;
            static const auto c = uint32_t(duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count());
            auto v = uint32_t(std::hash<T>()(t));
            std::minstd_rand rng(c ^ v);
            std::uniform_int_distribution<> dist(17, 230);
            return dist(rng);
        }

        inline void log_message(const Ustring& msg) {
            using namespace std::chrono;
            static std::mutex mtx;
            auto lock = make_lock(mtx);
            Ustring text = "\x1b[38;5;";
            text += decfmt(hash_xcolour(std::this_thread::get_id()));
            text += "m# ";
            text += msg;
            text += "\x1b[0m\n";
            fwrite(text.data(), 1, text.size(), stdout);
            fflush(stdout);
        }

        template <typename T>
        Ustring make_str(const T& t) {
            std::ostringstream out;
            out << t;
            return out.str();
        }

    }

    inline void logx(const Ustring& msg) noexcept {
        try { RS_Detail::log_message(msg); }
        catch (...) {}
    }

    inline void logx(const char* msg) noexcept {
        try { RS_Detail::log_message(cstr(msg)); }
        catch (...) {}
    }

    template <typename... Args> inline void logx(Args... args) noexcept {
        try {
            Strings v{RS_Detail::make_str(args)...};
            Ustring msg;
            for (auto& s: v)
                msg += s += ' ';
            if (! msg.empty())
                msg.pop_back();
            RS_Detail::log_message(msg);
        }
        catch (...) {}
    }

    // Version numbers

    class Version:
    public LessThanComparable<Version> {
    public:
        using value_type = unsigned;
        Version() noexcept {}
        template <typename... Args> Version(unsigned n, Args... args) { append(n, args...); trim(); }
        explicit Version(const Ustring& s);
        unsigned operator[](size_t i) const noexcept { return i < ver.size() ? ver[i] : 0; }
        const unsigned* begin() const noexcept { return ver.data(); }
        const unsigned* end() const noexcept { return ver.data() + ver.size(); }
        unsigned major() const noexcept { return (*this)[0]; }
        unsigned minor() const noexcept { return (*this)[1]; }
        unsigned patch() const noexcept { return (*this)[2]; }
        size_t size() const noexcept { return std::max(ver.size(), size_t(1)); }
        std::string str(size_t min_elements = 2) const;
        std::string suffix() const { return suf; }
        uint32_t to32() const noexcept;
        static Version from32(uint32_t n) noexcept;
        friend bool operator==(const Version& lhs, const Version& rhs) noexcept { return lhs.ver == rhs.ver && lhs.suf == rhs.suf; }
        friend bool operator<(const Version& lhs, const Version& rhs) noexcept { int c = compare_3way(lhs.ver, rhs.ver); return c == 0 ? lhs.suf < rhs.suf : c == -1; }
    private:
        std::vector<unsigned> ver;
        Ustring suf;
        template <typename... Args> void append(unsigned n, Args... args) { ver.push_back(n); append(args...); }
        void append(const Ustring& s) { suf = s; }
        void append() {}
        void trim() { while (! ver.empty() && ver.back() == 0) ver.pop_back(); }
        static bool is_digit(char c) noexcept { return c >= '0' && c <= '9'; }
    };

    inline Version::Version(const Ustring& s) {
        auto i = s.begin(), end = s.end();
        while (i != end) {
            auto j = std::find_if_not(i, end, is_digit);
            if (i == j)
                break;
            Ustring part(i, j);
            ver.push_back(unsigned(strtoul(part.data(), nullptr, 10)));
            i = j;
            if (i == end || *i != '.')
                break;
            ++i;
            if (i == end || ! is_digit(*i))
                break;
        }
        suf.assign(i, end);
        trim();
    }

    inline std::string Version::str(size_t min_elements) const {
        std::ostringstream out;
        for (auto& v: ver)
            out << v << '.';
        for (size_t i = ver.size(); i < min_elements; ++i)
            out << "0.";
        Ustring pre = out.str();
        if (! pre.empty())
            pre.pop_back();
        return pre + suf;
    }

    inline uint32_t Version::to32() const noexcept {
        uint32_t v = 0;
        for (size_t i = 0; i < 4; ++i)
            v = (v << 8) | ((*this)[i] & 0xff);
        return v;
    }

    inline Version Version::from32(uint32_t n) noexcept {
        Version v;
        for (int i = 24; i >= 0 && n != 0; i -= 8)
            v.ver.push_back((n >> i) & 0xff);
        v.trim();
        return v;
    }

    inline std::ostream& operator<<(std::ostream& o, const Version& v) {
        return o << v.str();
    }

    inline Ustring to_str(const Version& v) {
        return v.str();
    }

}

namespace std {
    template <typename T, RS::ByteOrder B> struct hash<RS::Endian<T, B>> {
        using argument_type = RS::Endian<T, B>;
        using result_type = size_t;
        size_t operator()(RS::Endian<T, B> t) const noexcept { return std::hash<T>()(t.get()); }
    };
}
