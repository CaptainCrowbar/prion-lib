#pragma once

#include "rs-core/common.hpp"
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <exception>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <new>
#include <ostream>
#include <regex>
#include <sstream>
#include <string>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#ifdef __GNUC__
    #include <cxxabi.h>
#endif

#define FAIL_POINT "[", __FILE__, ":", __LINE__, "] "

#define TRY_IMPL(expr, desc) \
    do { \
        try { \
            expr; \
        } \
        catch (const std::exception& ex) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, desc, " threw exception: ", ex.what()); \
        } \
        catch (...) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, desc, " threw exception of unknown type"); \
        } \
    } while (false)

#define TEST_IMPL(status, expr, desc) \
    do { \
        status = true; \
        TRY_IMPL(status = bool(expr), desc); \
    } while (false)

#define FAIL(msg) \
    do { \
        RS::UnitTest::record_failure(); \
        RS::UnitTest::print_fail(FAIL_POINT, (msg)); \
    } while (false)

#define MESSAGE(msg) \
    do { \
        RS::UnitTest::print_fail("*** [", __FILE__, ":", __LINE__, "] ", (msg)); \
    } while (false)

#define REQUIRE(expr) \
    do { \
        bool local_test_status = false; \
        TEST_IMPL(local_test_status, (expr), #expr); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr " is false"); \
            return; \
        } \
    } while (false)

#define TEST(expr) \
    do { \
        bool local_test_status = false; \
        TEST_IMPL(local_test_status, (expr), #expr); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr " is false"); \
        } \
    } while (false)

#define TEST_COMPARE(lhs, op, rhs) \
    do { \
        bool local_test_status = false; \
        TEST_IMPL(local_test_status, (lhs) op (rhs), #lhs " " #op " " #rhs); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #lhs " " #op " " #rhs " failed: lhs = ", lhs, ", rhs = ", rhs); \
        } \
    } while (false)

#define TEST_EQUAL(lhs, rhs) \
    TEST_COMPARE((lhs), ==, (rhs))

#define TEST_EQUAL_RANGE(lhs, rhs) \
    do { \
        bool local_test_status = false; \
        TEST_IMPL(local_test_status, RS::UnitTest::range_equal((lhs), (rhs)), #lhs " == " #rhs); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #lhs " != " #rhs ": lhs = ", RS::UnitTest::format_range(lhs), \
                ", rhs = ", RS::UnitTest::format_range(rhs)); \
        } \
    } while (false)

#define TEST_MATCH(str, pattern) \
    do { \
        static const std::regex local_test_regex(pattern); \
        bool local_test_status = false; \
        std::string local_test_string(str); \
        TEST_IMPL(local_test_status, std::regex_search(local_test_string, local_test_regex), \
            "regex match(" #str ", " #pattern ")"); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, "regex match(" #str ", " #pattern ") failed: lhs = ", str); \
        } \
    } while (false)

#define TEST_MATCH_ICASE(str, pattern) \
    do { \
        static const std::regex local_test_regex(pattern, std::regex::icase); \
        bool local_test_status = false; \
        std::string local_test_string(str); \
        TEST_IMPL(local_test_status, std::regex_search(local_test_string, local_test_regex), \
            "regex match(" #str ", " #pattern ")"); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, "regex match(" #str ", " #pattern ", icase) failed: lhs = ", str); \
        } \
    } while (false)

#define TEST_NEAR_EPSILON(lhs, rhs, tolerance) \
    do { \
        bool local_test_status = false; \
        TEST_IMPL(local_test_status, std::abs(double(lhs) - double(rhs)) <= double(tolerance), \
            #lhs " approx == " #rhs); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #lhs " and " #rhs " not approximately equal: lhs = ", \
                (lhs), ", rhs = ", (rhs), ", tolerance = ", (tolerance)); \
        } \
    } while (false)

#define TEST_NEAR_EPSILON_RANGE(lhs, rhs, tolerance) \
    do { \
        bool local_test_status = false; \
        TEST_IMPL(local_test_status, RS::UnitTest::range_near_abs((lhs), (rhs), double(tolerance)), \
            #lhs " approx == " #rhs); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #lhs " and " #rhs " not approximately equal: lhs = ", \
                RS::UnitTest::format_range(lhs), ", rhs = ", RS::UnitTest::format_range(rhs)); \
        } \
    } while (false)

#define TEST_NEAR_RELATIVE(lhs, rhs, tolerance) \
    do { \
        bool local_test_status = false; \
        auto local_test_lhs = double(lhs); \
        auto local_test_rhs = double(rhs); \
        TEST_IMPL(local_test_status, std::abs(local_test_lhs - local_test_rhs) / std::abs(local_test_rhs) <= double(tolerance), \
            #lhs " approx == " #rhs); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #lhs " and " #rhs " not approximately equal: lhs = ", \
                (lhs), ", rhs = ", (rhs), ", tolerance = ", (tolerance)); \
        } \
    } while (false)

#define TEST_NEAR_RELATIVE_RANGE(lhs, rhs, tolerance) \
    do { \
        bool local_test_status = false; \
        TEST_IMPL(local_test_status, RS::UnitTest::range_near_rel((lhs), (rhs), double(tolerance)), \
            #lhs " approx == " #rhs); \
        if (! local_test_status) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #lhs " and " #rhs " not approximately equal: lhs = ", \
                RS::UnitTest::format_range(lhs), ", rhs = ", RS::UnitTest::format_range(rhs)); \
        } \
    } while (false)

#define TEST_NEAR(lhs, rhs) \
    TEST_NEAR_EPSILON((lhs), (rhs), 1e-6)

#define TEST_NEAR_RANGE(lhs, rhs) \
    TEST_NEAR_EPSILON_RANGE((lhs), (rhs), 1e-6)

#define TEST_THROW(expr, expect) \
    do { \
        try { \
            expr; \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " failed to throw exception: expected ", #expect); \
        } \
        catch (const expect&) {} \
        catch (const std::exception& ex) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got ", ex.what()); \
        } \
        catch (...) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got unknown exception"); \
        } \
    } while (false)

#define TEST_THROW_EQUAL(expr, expect, message) \
    do { \
        try { \
            expr; \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " failed to throw exception: expected ", #expect); \
        } \
        catch (const expect& exception) { \
            TEST_EQUAL(std::string(exception.what()), message); \
        } \
        catch (const std::exception& ex) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got ", ex.what()); \
        } \
        catch (...) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got unknown exception"); \
        } \
    } while (false)

#define TEST_THROW_MATCH(expr, expect, pattern) \
    do { \
        try { \
            expr; \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " failed to throw exception: expected ", #expect); \
        } \
        catch (const expect& exception) { \
            TEST_MATCH(exception.what(), pattern); \
        } \
        catch (const std::exception& ex) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got ", ex.what()); \
        } \
        catch (...) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got unknown exception"); \
        } \
    } while (false)

#define TEST_THROW_MATCH_ICASE(expr, expect, pattern) \
    do { \
        try { \
            expr; \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " failed to throw exception: expected ", #expect); \
        } \
        catch (const expect& exception) { \
            TEST_MATCH_ICASE(exception.what(), pattern); \
        } \
        catch (const std::exception& ex) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got ", ex.what()); \
        } \
        catch (...) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, #expr, " threw wrong exception: expected ", #expect, \
                ", got unknown exception"); \
        } \
    } while (false)

#define TEST_TYPE(type1, type2) \
    do { \
        using local_test_type1 = std::decay_t<type1>; \
        using local_test_type2 = std::decay_t<type2>; \
        if (! std::is_same<local_test_type1, local_test_type2>::value) { \
            RS::UnitTest::record_failure(); \
            RS::UnitTest::print_fail(FAIL_POINT, "Type mismatch: ", #type1, " != ", #type2, \
                "; type 1 = ", RS::UnitTest::type_name(typeid(local_test_type1)), ", ", \
                "type 2 = ", RS::UnitTest::type_name(typeid(local_test_type2))); \
        } \
    } while (false)

#define TEST_TYPE_OF(object, type) \
    TEST_TYPE(decltype(object), type)

#define TEST_TYPES_OF(object1, object2) \
    TEST_TYPE(decltype(object1), decltype(object2))

#define TRY(expr) \
    TRY_IMPL((expr), #expr)

namespace RS {

    template <typename T, bool Copy = true>
    class Accountable {
    public:
        Accountable(): value_() { ++number(); }
        Accountable(const T& t): value_(t) { ++number(); }
        Accountable(const Accountable& a): value_(a.value_) { ++number(); }
        Accountable(Accountable&& a) noexcept: value_(std::exchange(a.value_, T())) { ++number(); }
        ~Accountable() noexcept { --number(); }
        Accountable& operator=(const Accountable& a) { value_ = a.value_; return *this; }
        Accountable& operator=(Accountable&& a) noexcept { if (&a != this) value_ = std::exchange(a.value_, T()); return *this; }
        const T& get() const noexcept { return value_; }
        void set(const T& t) { value_ = t; }
        static int count() noexcept { return number(); }
        static void reset() noexcept { number() = 0; }
        friend bool operator==(const Accountable& lhs, const Accountable& rhs) { return lhs.value_ == rhs.value_; }
        friend bool operator!=(const Accountable& lhs, const Accountable& rhs) { return ! (lhs == rhs); }
        friend std::ostream& operator<<(std::ostream& out, const Accountable& a) { return out << a.value_; }
    private:
        T value_;
        static std::atomic<int>& number() noexcept { static std::atomic<int> n(0); return n; }
    };

    template <typename T>
    class Accountable<T, false> {
    public:
        Accountable(): value_() { ++number(); }
        Accountable(const T& t): value_(t) { ++number(); }
        Accountable(const Accountable& a) = delete;
        Accountable(Accountable&& a) noexcept: value_(std::exchange(a.value_, T())) { ++number(); }
        ~Accountable() noexcept { --number(); }
        Accountable& operator=(const Accountable& a) = delete;
        Accountable& operator=(Accountable&& a) noexcept { if (&a != this) value_ = std::exchange(a.value_, T()); return *this; }
        const T& get() const noexcept { return value_; }
        void set(const T& t) { value_ = t; }
        static int count() noexcept { return number(); }
        static void reset() noexcept { number() = 0; }
        friend bool operator==(const Accountable& lhs, const Accountable& rhs) { return lhs.value_ == rhs.value_; }
        friend bool operator!=(const Accountable& lhs, const Accountable& rhs) { return ! (lhs == rhs); }
        friend std::ostream& operator<<(std::ostream& out, const Accountable& a) { return out << a.value_; }
    private:
        T value_;
        static std::atomic<int>& number() noexcept { static std::atomic<int> n(0); return n; }
    };

    template <>
    class Accountable<void, true> {
    public:
        Accountable() noexcept { ++number(); }
        Accountable(const Accountable&) noexcept { ++number(); }
        Accountable(Accountable&&) noexcept { ++number(); }
        ~Accountable() noexcept { --number(); }
        Accountable& operator=(const Accountable&) noexcept { return *this; }
        Accountable& operator=(Accountable&&) noexcept { return *this; }
        static int count() noexcept { return number(); }
        static void reset() noexcept { number() = 0; }
    private:
        static std::atomic<int>& number() noexcept { static std::atomic<int> n(0); return n; }
    };

    template <>
    class Accountable<void, false> {
    public:
        Accountable() noexcept { ++number(); }
        Accountable(const Accountable&) = delete;
        Accountable(Accountable&&) noexcept { ++number(); }
        ~Accountable() noexcept { --number(); }
        Accountable& operator=(const Accountable&) = delete;
        Accountable& operator=(Accountable&&) noexcept { return *this; }
        static int count() noexcept { return number(); }
        static void reset() noexcept { number() = 0; }
    private:
        static std::atomic<int>& number() noexcept { static std::atomic<int> n(0); return n; }
    };

    struct UnitTest {

        using test_function = void (*)();
        using test_index = std::vector<std::pair<std::string, test_function>>;

        static constexpr const char* x_reset = "\x1b[0m";
        static constexpr const char* x_head = "\x1b[38;5;220m";
        static constexpr const char* x_decor = "\x1b[38;5;244m";
        static constexpr const char* x_info = "\x1b[38;5;228m";
        static constexpr const char* x_good = "\x1b[38;5;83m";
        static constexpr const char* x_fail = "\x1b[38;5;208m";

        static std::atomic<size_t>& test_failures() noexcept {
            static std::atomic<size_t> fails;
            return fails;
        }

        static void record_failure() {
            ++test_failures();
        }

        static void print_build(std::ostringstream&) {}

        template <typename T, typename... Args>
        static void print_build(std::ostringstream& out, const T& t, const Args&... args) {
            out << preformat(t);
            print_build(out, args...);
        }

        static void print_out(const std::string& str) {
            static std::mutex mtx;
            auto lock = make_lock(mtx);
            std::cout << str << std::endl;
        }

        template <typename... Args>
        static void print(const Args&... args) {
            std::ostringstream out;
            print_build(out, args...);
            print_out(out.str());
        }

        template <typename... Args>
        static void print_fail(const Args&... args) {
            std::ostringstream out;
            out << "\x1b[38;5;208m*** ";
            print_build(out, args...);
            out << "\x1b[0m";
            print_out(out.str());
        }

        template <typename R1, typename R2>
        static bool range_equal(const R1& r1, const R2& r2) {
            auto i = std::begin(r1), e1 = std::end(r1);
            auto j = std::begin(r2), e2 = std::end(r2);
            for (; i != e1 && j != e2 && *i == *j; ++i, ++j) {}
            return i == e1 && j == e2;
        }

        template <typename R1, typename R2>
        static bool range_near_abs(const R1& r1, const R2& r2, double tolerance) {
            auto i = std::begin(r1), e1 = std::end(r1);
            auto j = std::begin(r2), e2 = std::end(r2);
            for (; i != e1 && j != e2 && std::abs(double(*i) - double(*j)) <= tolerance; ++i, ++j) {}
            return i == e1 && j == e2;
        }

        template <typename R1, typename R2>
        static bool range_near_rel(const R1& r1, const R2& r2, double tolerance) {
            auto i = std::begin(r1), e1 = std::end(r1);
            auto j = std::begin(r2), e2 = std::end(r2);
            for (; i != e1 && j != e2 && std::abs(double(*i) - double(*j)) / std::abs(double(*j)) <= tolerance; ++i, ++j) {}
            return i == e1 && j == e2;
        }

        static std::string preformat_hex_bytes(const uint8_t* ptr, size_t len) {
            static constexpr char digits[] = "0123456789abcdef";
            std::string s;
            s.reserve(2 * len);
            for (size_t i = 0; i < len; ++i) {
                s += digits[ptr[i] >> 4];
                s += digits[ptr[i] & 15];
            }
            return s;
        }

        template <typename C>
        static std::string preformat_string(const std::basic_string<C>& t) {
            using utype = std::make_unsigned_t<C>;
            std::string result;
            for (C c: t) {
                auto uc = utype(c);
                switch (uc) {
                    case '\0': result += "\\0"; break;
                    case '\t': result += "\\t"; break;
                    case '\n': result += "\\n"; break;
                    case '\f': result += "\\f"; break;
                    case '\r': result += "\\r"; break;
                    case '\\': result += "\\\\"; break;
                    default:
                        if (uc >= 0x20 && uc <= 0x7e) {
                            result += char(uc);
                        } else if (uc <= 0xff) {
                            result += "\\x";
                            result += hex(uc, 2);
                        } else {
                            result += "\\x{";
                            result += hex(uc, 1);
                            result += "}";
                        }
                        break;
                }
            }
            return result;
        }

        template <typename T> static T preformat(const T& t) { return t; }
        template <typename T> static const void* preformat(T* t) noexcept { return t; }
        static unsigned preformat(unsigned char t) noexcept { return t; }
        static int preformat(signed char t) noexcept { return t; }
        static std::string preformat(char16_t t) noexcept { return "U+" + hex(t, 4); }
        static std::string preformat(char32_t t) noexcept { return "U+" + hex(t, 4); }
        static std::string preformat(wchar_t t) noexcept { return "U+" + hex(std::make_unsigned_t<wchar_t>(t), 4); }
        static std::string preformat(const std::string& t) { return preformat_string(t); }
        static std::string preformat(const std::u16string& t) { return preformat_string(t); }
        static std::string preformat(const std::u32string& t) { return preformat_string(t); }
        static std::string preformat(const std::wstring& t) { return preformat_string(t); }
        static std::string preformat(const char* t) { return t ? preformat(std::string(t)) : "null"; }
        static std::string preformat(const char16_t* t) { return t ? preformat(std::u16string(t)) : "null"; }
        static std::string preformat(const char32_t* t) { return t ? preformat(std::u32string(t)) : "null"; }
        static std::string preformat(const wchar_t* t) { return t ? preformat(std::wstring(t)) : "null"; }
        static std::string preformat(const std::vector<uint8_t>& t) { return preformat_hex_bytes(t.data(), t.size()); }
        template <size_t N> static std::string preformat(const std::array<uint8_t, N>& t) { return preformat_hex_bytes(t.data(), t.size()); }
        static std::string preformat(std::nullptr_t) { return "null"; }
        template <typename T> static T preformat(const std::atomic<T>& t) { return t; }

        template <typename T1, typename T2>
        static std::string preformat(const std::pair<T1, T2>& p) {
            std::ostringstream out;
            out << '(' << preformat(p.first) << ',' << preformat(p.second) << ')';
            return out.str();
        }

        template <typename R, typename I = decltype(std::begin(std::declval<R>())),
            typename V = typename std::iterator_traits<I>::value_type>
        struct FormatRange {
            std::string operator()(const R& r) const {
                std::ostringstream out;
                out << '[';
                for (auto& x: r)
                    out << preformat(x) << ',';
                std::string s = out.str();
                if (s.size() > 1)
                    s.pop_back();
                s += ']';
                return s;
            }
        };

        template <typename R, typename I, typename K, typename V>
        struct FormatRange<R, I, std::pair<K, V>> {
            std::string operator()(const R& r) const {
                std::ostringstream out;
                out << '{';
                for (auto& x: r)
                    out << preformat(x.first) << ':' << preformat(x.second) << ',';
                std::string s = out.str();
                if (s.size() > 1)
                    s.pop_back();
                s += '}';
                return s;
            }
        };

        template <typename R>
        static std::string format_range(const R& r) {
            return FormatRange<R>()(r);
        }

        static std::string type_name(const std::type_info& info) {
            #ifdef __GNUC__
                std::string mangled = info.name();
                std::shared_ptr<char> demangled;
                int status = 0;
                for (;;) {
                    if (mangled.empty())
                        return info.name();
                    demangled.reset(abi::__cxa_demangle(mangled.data(), nullptr, nullptr, &status), free);
                    if (status == -1)
                        throw std::bad_alloc();
                    if (status == 0 && demangled)
                        return demangled.get();
                    if (mangled[0] != '_')
                        return info.name();
                    mangled.erase(0, 1);
                }
            #else
                return info.name();
            #endif
        }

        static int test_main(const test_index& index) {
            using namespace std::chrono;
            std::regex unit_pattern;
            const char* unit_ptr = getenv("UNIT");
            if (unit_ptr)
                unit_pattern = std::regex(unit_ptr, std::regex::icase);
            else
                unit_pattern = std::regex(".");
            std::cout << std::endl;
            try {
                size_t test_count = 0;
                std::string rule(20, '=');
                std::cout << x_head << "Running test modules" << x_reset << std::endl;
                std::cout << x_decor << rule << x_reset << std::endl;
                auto start = system_clock::now();
                for (auto& test: index) {
                    if (std::regex_search(test.first, unit_pattern)) {
                        std::cout << x_info << "Testing " << test.first << x_reset << std::endl;
                        test.second();
                        ++test_count;
                    }
                }
                auto seconds = duration_cast<duration<double>>(system_clock::now() - start).count();
                std::cout << x_decor << rule << x_reset << std::endl;
                if (! test_count)
                    std::cout << x_fail << "No tests were run" << x_reset << std::endl;
                else if (test_failures() > 0)
                    std::cout << x_fail << "*** Test failures: " << test_failures() << x_reset << std::endl;
                else
                    std::cout << x_good << "OK: all tests passed" << x_reset << std::endl;
                std::cout << x_info << "Time: " << seconds << " s" << x_reset << std::endl;
            }
            catch (const std::exception& ex) {
                std::cout << x_fail << "*** " << ex.what() << x_reset << std::endl;
            }
            std::cout << std::endl;
            return test_failures() > 0;
        }

    };

}

namespace std {

    template <typename T, bool Copy>
    struct hash<RS::Accountable<T, Copy>> {
        size_t operator()(const RS::Accountable<T, Copy>& a) const noexcept {
            return std::hash<T>()(a.get());
        }
    };

    template <bool Copy>
    struct hash<RS::Accountable<void, Copy>> {
        size_t operator()(const RS::Accountable<void, Copy>&) const noexcept {
            return 1;
        }
    };

}
