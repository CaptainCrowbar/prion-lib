#include "rs-core/common.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <ctime>
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <tuple>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace RS;
using namespace RS::Literals;
using namespace std::literals;

TEST_MAIN;

// These tests are a bit incestuous since the unit test module itself uses
// some of the facilities in the core module. The shared functions are mostly
// simple enough that it should be clear where the error lies if anything goes
// wrong, and in any case the alternative of duplicating large parts of the
// core module in the unit test module would not be an improvement.

namespace {

    using RS_Detail::decfmt;
    using RS_Detail::hexfmt;

    U8string make_str(std::nullptr_t) { return "null"; }

    template <typename T>
    U8string make_str(const T& t) {
        std::ostringstream out;
        out << t;
        return out.str();
    }

    #define MAKE_STR_FOR_CONTAINER(Con) \
        template <typename T> \
        U8string make_str(const std::Con<T>& c) { \
            if (c.empty()) \
                return "[]"; \
            std::ostringstream out; \
            out << '['; \
            for (auto& t: c) \
                out << t << ','; \
            U8string s = out.str(); \
            s.back() = ']'; \
            return s; \
        }

    MAKE_STR_FOR_CONTAINER(set)
    MAKE_STR_FOR_CONTAINER(vector)

    U8string f1(int n) { return U8string(n, '*'); }
    U8string f1(U8string s) { return '[' + s + ']'; }
    U8string f2() { return "Hello"; }
    U8string f2(U8string s) { return '[' + s + ']'; }
    U8string f2(int x, int y) { return decfmt(x * y); }

    RS_ENUM(FooEnum, int16_t, 1, alpha, bravo, charlie)
    RS_ENUM_CLASS(BarEnum, int32_t, 1, alpha, bravo, charlie)

    enum class ZapEnum: uint32_t {
        alpha = 1,
        bravo = 2,
        charlie = 4,
    };

    RS_BITMASK_OPERATORS(ZapEnum)

    void check_preprocessor_macros() {

        int n = int(std::time(nullptr));
        RS_ASSUME(n > 0);
        if (RS_LIKELY(n > 1000))
            ++n;
        if (RS_UNLIKELY(n < 1000))
            ++n;
        switch (n - n) {
            case 0:
                break;
            default:
                RS_NOTREACHED;
                break;
        }

        std::vector<int> iv = {1, 2, 3, 4, 5}, iv2 = {2, 3, 5, 7, 11};
        Strings result, sv = {"Neddie", "Eccles", "Bluebottle"};

        TRY(std::transform(iv.begin(), iv.end(), overwrite(result), RS_OVERLOAD(f1)));
        TEST_EQUAL_RANGE(result, (Strings{"*", "**", "***", "****", "*****"}));
        TRY(std::transform(sv.begin(), sv.end(), overwrite(result), RS_OVERLOAD(f1)));
        TEST_EQUAL_RANGE(result, (Strings{"[Neddie]", "[Eccles]", "[Bluebottle]"}));

        TRY(std::generate_n(overwrite(result), 3, RS_OVERLOAD(f2)));
        TEST_EQUAL_RANGE(result, (Strings{"Hello", "Hello", "Hello"}));
        TRY(std::transform(sv.begin(), sv.end(), overwrite(result), RS_OVERLOAD(f2)));
        TEST_EQUAL_RANGE(result, (Strings{"[Neddie]", "[Eccles]", "[Bluebottle]"}));
        auto of2 = RS_OVERLOAD(f2);
        auto out = overwrite(result);
        for (size_t i = 0; i < iv.size() && i < iv2.size(); ++i)
            TRY(*out++ = of2(iv[i], iv2[i]));
        TEST_EQUAL_RANGE(result, (Strings{"2", "6", "15", "28", "55"}));

        std::vector<FooEnum> vf;
        std::vector<BarEnum> vb;

        TEST_TYPE(std::underlying_type_t<FooEnum>, int16_t);
        TEST_TYPE(std::underlying_type_t<BarEnum>, int32_t);
        TEST_TYPE(std::underlying_type_t<ZapEnum>, uint32_t);

        TEST_EQUAL(sizeof(FooEnum), 2);
        TEST_EQUAL(sizeof(BarEnum), 4);
        TEST_EQUAL(sizeof(ZapEnum), 4);

        TEST_EQUAL(int(alpha), 1);
        TEST_EQUAL(int(bravo), 2);
        TEST_EQUAL(int(charlie), 3);

        TEST_EQUAL(make_str(alpha), "alpha");
        TEST_EQUAL(make_str(bravo), "bravo");
        TEST_EQUAL(make_str(charlie), "charlie");
        TEST_EQUAL(make_str(FooEnum(0)), "0");
        TEST_EQUAL(make_str(FooEnum(4)), "4");
        TEST_EQUAL(make_str(FooEnum(99)), "99");
        TEST_EQUAL(make_str(nullptr), "null");

        TEST(! enum_is_valid(FooEnum(0)));
        TEST(enum_is_valid(FooEnum(1)));
        TEST(enum_is_valid(FooEnum(2)));
        TEST(enum_is_valid(FooEnum(3)));
        TEST(! enum_is_valid(FooEnum(4)));

        TRY(vf = enum_values<FooEnum>());
        TEST_EQUAL(vf.size(), 3);
        TEST_EQUAL(make_str(vf), "[alpha,bravo,charlie]");

        TEST_EQUAL(int(BarEnum::alpha), 1);
        TEST_EQUAL(int(BarEnum::bravo), 2);
        TEST_EQUAL(int(BarEnum::charlie), 3);

        TEST_EQUAL(make_str(BarEnum::alpha), "BarEnum::alpha");
        TEST_EQUAL(make_str(BarEnum::bravo), "BarEnum::bravo");
        TEST_EQUAL(make_str(BarEnum::charlie), "BarEnum::charlie");
        TEST_EQUAL(make_str(BarEnum(0)), "0");
        TEST_EQUAL(make_str(BarEnum(4)), "4");
        TEST_EQUAL(make_str(BarEnum(99)), "99");

        TEST(! enum_is_valid(BarEnum(0)));
        TEST(enum_is_valid(BarEnum(1)));
        TEST(enum_is_valid(BarEnum(2)));
        TEST(enum_is_valid(BarEnum(3)));
        TEST(! enum_is_valid(BarEnum(4)));

        TRY(vb = enum_values<BarEnum>());
        TEST_EQUAL(vb.size(), 3);
        TEST_EQUAL(make_str(vb), "[BarEnum::alpha,BarEnum::bravo,BarEnum::charlie]");

        ZapEnum x = ZapEnum::alpha, y = ZapEnum::bravo, z = ZapEnum(0);

        TEST(x);    TEST(bool(x));
        TEST(y);    TEST(bool(y));
        TEST(! z);  TEST(! bool(z));

        TRY(z = ~ x);             TEST_EQUAL(uint32_t(z), 0xfffffffe);
        TRY(z = ~ y);             TEST_EQUAL(uint32_t(z), 0xfffffffd);
        TRY(z = ~ ZapEnum(0));        TEST_EQUAL(uint32_t(z), 0xffffffff);
        TRY(z = x & y);           TEST_EQUAL(uint32_t(z), 0);
        TRY(z = x | y);           TEST_EQUAL(uint32_t(z), 3);
        TRY(z = x ^ y);           TEST_EQUAL(uint32_t(z), 3);
        TRY(z = x); TRY(z &= y);  TEST_EQUAL(uint32_t(z), 0);
        TRY(z = x); TRY(z |= y);  TEST_EQUAL(uint32_t(z), 3);
        TRY(z = x); TRY(z ^= y);  TEST_EQUAL(uint32_t(z), 3);

        TEST(ZapEnum::alpha == ZapEnum::alpha);   TEST(ZapEnum::alpha < ZapEnum::bravo);    TEST(ZapEnum::alpha < ZapEnum::charlie);
        TEST(ZapEnum::bravo > ZapEnum::alpha);    TEST(ZapEnum::bravo == ZapEnum::bravo);   TEST(ZapEnum::bravo < ZapEnum::charlie);
        TEST(ZapEnum::charlie > ZapEnum::alpha);  TEST(ZapEnum::charlie > ZapEnum::bravo);  TEST(ZapEnum::charlie == ZapEnum::charlie);

    }

    void check_integer_types() {

        uint8_t a8;
        uint16_t a16;
        uint32_t a32;
        uint64_t a64;

        constexpr uint8_t c8 = 0x12u;
        constexpr uint16_t c16 = 0x1234u;
        constexpr uint32_t c32 = 0x12345678ul;
        constexpr uint64_t c64 = 0x123456789abcdef0ull;

        a8 = RS_Detail::swap_ends(c8);    TEST_EQUAL(a8, 0x12u);
        a16 = RS_Detail::swap_ends(c16);  TEST_EQUAL(a16, 0x3412u);
        a32 = RS_Detail::swap_ends(c32);  TEST_EQUAL(a32, 0x78563412ul);
        a64 = RS_Detail::swap_ends(c64);  TEST_EQUAL(a64, 0xf0debc9a78563412ull);

        a8 = RS_Detail::order_bytes<big_endian>(c8);       TEST_EQUAL(a8, 0x12u);
        a16 = RS_Detail::order_bytes<big_endian>(c16);     TEST_EQUAL(a16, 0x3412u);
        a32 = RS_Detail::order_bytes<big_endian>(c32);     TEST_EQUAL(a32, 0x78563412ul);
        a64 = RS_Detail::order_bytes<big_endian>(c64);     TEST_EQUAL(a64, 0xf0debc9a78563412ull);
        a8 = RS_Detail::order_bytes<little_endian>(c8);    TEST_EQUAL(a8, 0x12u);
        a16 = RS_Detail::order_bytes<little_endian>(c16);  TEST_EQUAL(a16, 0x1234u);
        a32 = RS_Detail::order_bytes<little_endian>(c32);  TEST_EQUAL(a32, 0x12345678ul);
        a64 = RS_Detail::order_bytes<little_endian>(c64);  TEST_EQUAL(a64, 0x123456789abcdef0ull);

        constexpr BigEndian<uint8_t> cbe8 = c8;
        constexpr BigEndian<uint16_t> cbe16 = c16;
        constexpr BigEndian<uint32_t> cbe32 = c32;
        constexpr BigEndian<uint64_t> cbe64 = c64;
        constexpr LittleEndian<uint8_t> cle8 = c8;
        constexpr LittleEndian<uint16_t> cle16 = c16;
        constexpr LittleEndian<uint32_t> cle32 = c32;
        constexpr LittleEndian<uint64_t> cle64 = c64;

        TEST_EQUAL(cbe8, 0x12u);                   TEST_EQUAL(cbe8.rep(), 0x12u);
        TEST_EQUAL(cbe16, 0x1234u);                TEST_EQUAL(cbe16.rep(), 0x3412u);
        TEST_EQUAL(cbe32, 0x12345678ul);           TEST_EQUAL(cbe32.rep(), 0x78563412ul);
        TEST_EQUAL(cbe64, 0x123456789abcdef0ull);  TEST_EQUAL(cbe64.rep(), 0xf0debc9a78563412ull);
        TEST_EQUAL(cle8, 0x12u);                   TEST_EQUAL(cle8.rep(), 0x12u);
        TEST_EQUAL(cle16, 0x1234u);                TEST_EQUAL(cle16.rep(), 0x1234u);
        TEST_EQUAL(cle32, 0x12345678ul);           TEST_EQUAL(cle32.rep(), 0x12345678ul);
        TEST_EQUAL(cle64, 0x123456789abcdef0ull);  TEST_EQUAL(cle64.rep(), 0x123456789abcdef0ull);

        BigEndian<uint8_t> be8;
        BigEndian<uint16_t> be16;
        BigEndian<uint32_t> be32;
        BigEndian<uint64_t> be64;
        LittleEndian<uint8_t> le8;
        LittleEndian<uint16_t> le16;
        LittleEndian<uint32_t> le32;
        LittleEndian<uint64_t> le64;

        be8.rep() = c8;    TEST_EQUAL(be8, 0x12u);
        be16.rep() = c16;  TEST_EQUAL(be16, 0x3412u);
        be32.rep() = c32;  TEST_EQUAL(be32, 0x78563412ul);
        be64.rep() = c64;  TEST_EQUAL(be64, 0xf0debc9a78563412ull);
        le8.rep() = c8;    TEST_EQUAL(le8, 0x12u);
        le16.rep() = c16;  TEST_EQUAL(le16, 0x1234u);
        le32.rep() = c32;  TEST_EQUAL(le32, 0x12345678ul);
        le64.rep() = c64;  TEST_EQUAL(le64, 0x123456789abcdef0ull);

        size_t h1 = 0, h2 = 0, h3 = 0;

        for (unsigned i = 0; i <= 1000000; i += 100) {
            TRY(h1 = std::hash<uint8_t>()(uint8_t(i)));
            TRY(h2 = std::hash<BigEndian<uint8_t>>()(BigEndian<uint8_t>(i)));
            TRY(h3 = std::hash<LittleEndian<uint8_t>>()(LittleEndian<uint8_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
            TRY(h1 = std::hash<uint16_t>()(uint16_t(i)));
            TRY(h2 = std::hash<BigEndian<uint16_t>>()(BigEndian<uint16_t>(i)));
            TRY(h3 = std::hash<LittleEndian<uint16_t>>()(LittleEndian<uint16_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
            TRY(h1 = std::hash<uint32_t>()(uint32_t(i)));
            TRY(h2 = std::hash<BigEndian<uint32_t>>()(BigEndian<uint32_t>(i)));
            TRY(h3 = std::hash<LittleEndian<uint32_t>>()(LittleEndian<uint32_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
            TRY(h1 = std::hash<uint64_t>()(uint64_t(i)));
            TRY(h2 = std::hash<BigEndian<uint64_t>>()(BigEndian<uint64_t>(i)));
            TRY(h3 = std::hash<LittleEndian<uint64_t>>()(LittleEndian<uint64_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
            TRY(h1 = std::hash<int8_t>()(int8_t(i)));
            TRY(h2 = std::hash<BigEndian<int8_t>>()(BigEndian<int8_t>(i)));
            TRY(h3 = std::hash<LittleEndian<int8_t>>()(LittleEndian<int8_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
            TRY(h1 = std::hash<int16_t>()(int16_t(i)));
            TRY(h2 = std::hash<BigEndian<int16_t>>()(BigEndian<int16_t>(i)));
            TRY(h3 = std::hash<LittleEndian<int16_t>>()(LittleEndian<int16_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
            TRY(h1 = std::hash<int32_t>()(int32_t(i)));
            TRY(h2 = std::hash<BigEndian<int32_t>>()(BigEndian<int32_t>(i)));
            TRY(h3 = std::hash<LittleEndian<int32_t>>()(LittleEndian<int32_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
            TRY(h1 = std::hash<int64_t>()(int64_t(i)));
            TRY(h2 = std::hash<BigEndian<int64_t>>()(BigEndian<int64_t>(i)));
            TRY(h3 = std::hash<LittleEndian<int64_t>>()(LittleEndian<int64_t>(i)));
            TEST_EQUAL(h1, h2);
            TEST_EQUAL(h1, h3);
        }

        BigEndian<long> be = 20, be1 = 10, be2 = 20, be3 = 30;
        LittleEndian<long> le = 20, le1 = 10, le2 = 20, le3 = 30;

        TEST_COMPARE(be, !=, be1);  TEST_COMPARE(be, >, be1);   TEST_COMPARE(be, >=, be1);
        TEST_COMPARE(be, ==, be2);  TEST_COMPARE(be, <=, be2);  TEST_COMPARE(be, >=, be2);
        TEST_COMPARE(be, !=, be3);  TEST_COMPARE(be, <, be3);   TEST_COMPARE(be, <=, be3);
        TEST_COMPARE(be, !=, 10);  TEST_COMPARE(be, >, 10);   TEST_COMPARE(be, >=, 10);
        TEST_COMPARE(be, ==, 20);  TEST_COMPARE(be, <=, 20);  TEST_COMPARE(be, >=, 20);
        TEST_COMPARE(be, !=, 30);  TEST_COMPARE(be, <, 30);   TEST_COMPARE(be, <=, 30);
        TEST_COMPARE(10, !=, be);  TEST_COMPARE(10, <, be);   TEST_COMPARE(10, <=, be);
        TEST_COMPARE(20, ==, be);  TEST_COMPARE(20, >=, be);  TEST_COMPARE(20, <=, be);
        TEST_COMPARE(30, !=, be);  TEST_COMPARE(30, >, be);   TEST_COMPARE(30, >=, be);

        TEST_COMPARE(le, !=, le1);  TEST_COMPARE(le, >, le1);   TEST_COMPARE(le, >=, le1);
        TEST_COMPARE(le, ==, le2);  TEST_COMPARE(le, <=, le2);  TEST_COMPARE(le, >=, le2);
        TEST_COMPARE(le, !=, le3);  TEST_COMPARE(le, <, le3);   TEST_COMPARE(le, <=, le3);
        TEST_COMPARE(le, !=, 10);  TEST_COMPARE(le, >, 10);   TEST_COMPARE(le, >=, 10);
        TEST_COMPARE(le, ==, 20);  TEST_COMPARE(le, <=, 20);  TEST_COMPARE(le, >=, 20);
        TEST_COMPARE(le, !=, 30);  TEST_COMPARE(le, <, 30);   TEST_COMPARE(le, <=, 30);
        TEST_COMPARE(10, !=, le);  TEST_COMPARE(10, <, le);   TEST_COMPARE(10, <=, le);
        TEST_COMPARE(20, ==, le);  TEST_COMPARE(20, >=, le);  TEST_COMPARE(20, <=, le);
        TEST_COMPARE(30, !=, le);  TEST_COMPARE(30, >, le);   TEST_COMPARE(30, >=, le);

        TEST_COMPARE(be, !=, le1);  TEST_COMPARE(be, >, le1);   TEST_COMPARE(be, >=, le1);
        TEST_COMPARE(be, ==, le2);  TEST_COMPARE(be, <=, le2);  TEST_COMPARE(be, >=, le2);
        TEST_COMPARE(be, !=, le3);  TEST_COMPARE(be, <, le3);   TEST_COMPARE(be, <=, le3);
        TEST_COMPARE(le, !=, be1);  TEST_COMPARE(le, >, be1);   TEST_COMPARE(le, >=, be1);
        TEST_COMPARE(le, ==, be2);  TEST_COMPARE(le, <=, be2);  TEST_COMPARE(le, >=, be2);
        TEST_COMPARE(le, !=, be3);  TEST_COMPARE(le, <, be3);   TEST_COMPARE(le, <=, be3);

    }

    void check_string_types() {

        const char* p0 = nullptr;
        const char* p1 = "";
        const char* p2 = "Hello";
        const char16_t* q0 = nullptr;
        const char16_t* q1 = u"";
        const char16_t* q2 = u"Hello";
        const char32_t* r0 = nullptr;
        const char32_t* r1 = U"";
        const char32_t* r2 = U"Hello";

        TEST_EQUAL(cstr(p0), ""s);
        TEST_EQUAL(cstr(p1), ""s);
        TEST_EQUAL(cstr(p2), "Hello"s);
        TEST_EQUAL(cstr(p0, 0), ""s);
        TEST_EQUAL(cstr(p1, 0), ""s);
        TEST_EQUAL(cstr(p2, 0), ""s);
        TEST_EQUAL(cstr(p0, 5), "\0\0\0\0\0"s);
        TEST_EQUAL(cstr(p2, 5), "Hello"s);
        TEST_EQUAL(cstr(q0), u""s);
        TEST_EQUAL(cstr(q1), u""s);
        TEST_EQUAL(cstr(q2), u"Hello"s);
        TEST_EQUAL(cstr(q0, 0), u""s);
        TEST_EQUAL(cstr(q1, 0), u""s);
        TEST_EQUAL(cstr(q2, 0), u""s);
        TEST_EQUAL(cstr(q0, 5), u"\0\0\0\0\0"s);
        TEST_EQUAL(cstr(q2, 5), u"Hello"s);
        TEST_EQUAL(cstr(r0), U""s);
        TEST_EQUAL(cstr(r1), U""s);
        TEST_EQUAL(cstr(r2), U"Hello"s);
        TEST_EQUAL(cstr(r0, 0), U""s);
        TEST_EQUAL(cstr(r1, 0), U""s);
        TEST_EQUAL(cstr(r2, 0), U""s);
        TEST_EQUAL(cstr(r0, 5), U"\0\0\0\0\0"s);
        TEST_EQUAL(cstr(r2, 5), U"Hello"s);

        TEST_EQUAL(cstr_size(p0), 0);
        TEST_EQUAL(cstr_size(p1), 0);
        TEST_EQUAL(cstr_size(p2), 5);
        TEST_EQUAL(cstr_size(q0), 0);
        TEST_EQUAL(cstr_size(q1), 0);
        TEST_EQUAL(cstr_size(q2), 5);
        TEST_EQUAL(cstr_size(r0), 0);
        TEST_EQUAL(cstr_size(r1), 0);
        TEST_EQUAL(cstr_size(r2), 5);

    }

    void check_exceptions() {

        #ifdef _WIN32

            U8string s;

            TRY(s = windows_category().message(ERROR_INVALID_FUNCTION));
            TEST_EQUAL(s, "Incorrect function.");
            try {
                throw std::system_error(ERROR_INVALID_FUNCTION, windows_category(), "SomeFunction()");
            }
            catch (const std::exception& ex) {
                TEST_MATCH(std::string(ex.what()), "^SomeFunction\\(\\).+Incorrect function\\.$");
            }

        #endif

    }

    void check_metaprogramming_and_type_traits() {

        TEST_TYPE(BinaryType<char>, uint8_t);
        TEST_TYPE(BinaryType<float>, uint32_t);
        TEST_TYPE(BinaryType<double>, uint64_t);

        {  using type = CopyConst<int, std::string>;              TEST_TYPE(type, std::string);        }
        {  using type = CopyConst<int, const std::string>;        TEST_TYPE(type, std::string);        }
        {  using type = CopyConst<const int, std::string>;        TEST_TYPE(type, const std::string);  }
        {  using type = CopyConst<const int, const std::string>;  TEST_TYPE(type, const std::string);  }

        TEST_TYPE(SignedInteger<8>, int8_t);
        TEST_TYPE(SignedInteger<16>, int16_t);
        TEST_TYPE(SignedInteger<32>, int32_t);
        TEST_TYPE(SignedInteger<64>, int64_t);
        TEST_TYPE(UnsignedInteger<8>, uint8_t);
        TEST_TYPE(UnsignedInteger<16>, uint16_t);
        TEST_TYPE(UnsignedInteger<32>, uint32_t);
        TEST_TYPE(UnsignedInteger<64>, uint64_t);

    }

    struct Adapt {
        Movable<U8string> s;
        Movable<int> i;
        Movable<int, 10> j;
        int k = 20;
    };

    void check_type_adapters() {

        Adapt a, b;

        TEST_EQUAL(*a.s, "");
        TEST_EQUAL(*a.i, 0);
        TEST_EQUAL(*a.j, 10);
        TEST_EQUAL(a.k, 20);

        TRY((b = {"Hello"s, 100, 200, 300}));

        TEST_EQUAL(*b.s, "Hello");
        TEST_EQUAL(*b.i, 100);
        TEST_EQUAL(*b.j, 200);
        TEST_EQUAL(b.k, 300);

        TRY(a = std::move(b));

        TEST_EQUAL(*a.s, "Hello");
        TEST_EQUAL(*a.i, 100);
        TEST_EQUAL(*a.j, 200);
        TEST_EQUAL(a.k, 300);

        TEST_EQUAL(*b.s, "");
        TEST_EQUAL(*b.i, 0);
        TEST_EQUAL(*b.j, 10);
        TEST_EQUAL(b.k, 300);

    }

    class Base {
    public:
        virtual ~Base() noexcept {}
        virtual int get() const = 0;
    };

    class Derived1: public Base {
    public:
        virtual int get() const { return 1; }
    };

    class Derived2: public Base {
    public:
        virtual int get() const { return 2; }
    };

    void check_type_related_functions() {

        std::shared_ptr<Base> b1 = std::make_shared<Derived1>();
        std::shared_ptr<Base> b2 = std::make_shared<Derived2>();

        TEST(is<Derived1>(b1));
        TEST(! is<Derived1>(b2));
        TEST(! is<Derived2>(b1));
        TEST(is<Derived2>(b2));

        TEST(is<Derived1>(*b1));
        TEST(! is<Derived1>(*b2));
        TEST(! is<Derived2>(*b1));
        TEST(is<Derived2>(*b2));

        TEST_EQUAL(as<Derived1>(b1).get(), 1);
        TEST_THROW(as<Derived2>(b1).get(), std::bad_cast);
        TEST_THROW(as<Derived1>(b2).get(), std::bad_cast);
        TEST_EQUAL(as<Derived2>(b2).get(), 2);

        TEST_EQUAL(as<Derived1>(*b1).get(), 1);
        TEST_THROW(as<Derived2>(*b1).get(), std::bad_cast);
        TEST_THROW(as<Derived1>(*b2).get(), std::bad_cast);
        TEST_EQUAL(as<Derived2>(*b2).get(), 2);

        int16_t i;
        uint16_t u;

        i = 42;      TRY(u = binary_cast<uint16_t>(i));  TEST_EQUAL(u, 42);
        i = -32767;  TRY(u = binary_cast<uint16_t>(i));  TEST_EQUAL(u, 32769);

    }

    void check_version_number() {

        Version v;

        TEST_EQUAL(v[0], 0);
        TEST_EQUAL(v[1], 0);
        TEST_EQUAL(v[2], 0);
        TEST_EQUAL(v.major(), 0);
        TEST_EQUAL(v.minor(), 0);
        TEST_EQUAL(v.patch(), 0);
        TEST_EQUAL(v.suffix(), "");
        TEST_EQUAL(v.str(), "0.0");
        TEST_EQUAL(v.str(0), "");
        TEST_EQUAL(v.str(1), "0");
        TEST_EQUAL(v.str(2), "0.0");
        TEST_EQUAL(v.str(3), "0.0.0");
        TEST_EQUAL(v.str(4), "0.0.0.0");
        TEST_EQUAL(v.to32(), 0);

        TRY((v = {42}));
        TEST_EQUAL(v[0], 42);
        TEST_EQUAL(v[1], 0);
        TEST_EQUAL(v[2], 0);
        TEST_EQUAL(v.major(), 42);
        TEST_EQUAL(v.minor(), 0);
        TEST_EQUAL(v.patch(), 0);
        TEST_EQUAL(v.suffix(), "");
        TEST_EQUAL(v.str(), "42.0");
        TEST_EQUAL(v.to32(), 0x2a000000);

        TRY((v = {42,"beta"}));
        TEST_EQUAL(v[0], 42);
        TEST_EQUAL(v[1], 0);
        TEST_EQUAL(v[2], 0);
        TEST_EQUAL(v.major(), 42);
        TEST_EQUAL(v.minor(), 0);
        TEST_EQUAL(v.patch(), 0);
        TEST_EQUAL(v.suffix(), "beta");
        TEST_EQUAL(v.str(), "42.0beta");
        TEST_EQUAL(v.to32(), 0x2a000000);

        TRY((v = {1,2,3}));
        TEST_EQUAL(v[0], 1);
        TEST_EQUAL(v[1], 2);
        TEST_EQUAL(v[2], 3);
        TEST_EQUAL(v.major(), 1);
        TEST_EQUAL(v.minor(), 2);
        TEST_EQUAL(v.patch(), 3);
        TEST_EQUAL(v.suffix(), "");
        TEST_EQUAL(v.str(), "1.2.3");
        TEST_EQUAL(v.str(0), "1.2.3");
        TEST_EQUAL(v.str(1), "1.2.3");
        TEST_EQUAL(v.str(2), "1.2.3");
        TEST_EQUAL(v.str(3), "1.2.3");
        TEST_EQUAL(v.str(4), "1.2.3.0");
        TEST_EQUAL(v.to32(), 0x01020300);

        TRY((v = {1,2,3,"beta"}));
        TEST_EQUAL(v[0], 1);
        TEST_EQUAL(v[1], 2);
        TEST_EQUAL(v[2], 3);
        TEST_EQUAL(v.major(), 1);
        TEST_EQUAL(v.minor(), 2);
        TEST_EQUAL(v.patch(), 3);
        TEST_EQUAL(v.suffix(), "beta");
        TEST_EQUAL(v.str(), "1.2.3beta");
        TEST_EQUAL(v.str(0), "1.2.3beta");
        TEST_EQUAL(v.str(1), "1.2.3beta");
        TEST_EQUAL(v.str(2), "1.2.3beta");
        TEST_EQUAL(v.str(3), "1.2.3beta");
        TEST_EQUAL(v.str(4), "1.2.3.0beta");
        TEST_EQUAL(v.to32(), 0x01020300);

        TRY(v = Version::from32(0x12345678));
        TEST_EQUAL(v[0], 18);
        TEST_EQUAL(v[1], 52);
        TEST_EQUAL(v[2], 86);
        TEST_EQUAL(v[3], 120);
        TEST_EQUAL(v.major(), 18);
        TEST_EQUAL(v.minor(), 52);
        TEST_EQUAL(v.patch(), 86);
        TEST_EQUAL(v.suffix(), "");
        TEST_EQUAL(v.str(), "18.52.86.120");
        TEST_EQUAL(v.to32(), 0x12345678);

        TRY(v = Version(""));
        TEST_EQUAL(v[0], 0);
        TEST_EQUAL(v[1], 0);
        TEST_EQUAL(v[2], 0);
        TEST_EQUAL(v.major(), 0);
        TEST_EQUAL(v.minor(), 0);
        TEST_EQUAL(v.patch(), 0);
        TEST_EQUAL(v.suffix(), "");
        TEST_EQUAL(v.str(), "0.0");
        TEST_EQUAL(v.to32(), 0);

        TRY(v = Version("1.2.3"));
        TEST_EQUAL(v[0], 1);
        TEST_EQUAL(v[1], 2);
        TEST_EQUAL(v[2], 3);
        TEST_EQUAL(v.major(), 1);
        TEST_EQUAL(v.minor(), 2);
        TEST_EQUAL(v.patch(), 3);
        TEST_EQUAL(v.suffix(), "");
        TEST_EQUAL(v.str(), "1.2.3");
        TEST_EQUAL(v.to32(), 0x01020300);

        TRY(v = Version("1.2.3beta"));
        TEST_EQUAL(v[0], 1);
        TEST_EQUAL(v[1], 2);
        TEST_EQUAL(v[2], 3);
        TEST_EQUAL(v.major(), 1);
        TEST_EQUAL(v.minor(), 2);
        TEST_EQUAL(v.patch(), 3);
        TEST_EQUAL(v.suffix(), "beta");
        TEST_EQUAL(v.str(), "1.2.3beta");
        TEST_EQUAL(v.to32(), 0x01020300);

        TRY(v = Version("beta"));
        TEST_EQUAL(v[0], 0);
        TEST_EQUAL(v[1], 0);
        TEST_EQUAL(v[2], 0);
        TEST_EQUAL(v.major(), 0);
        TEST_EQUAL(v.minor(), 0);
        TEST_EQUAL(v.patch(), 0);
        TEST_EQUAL(v.suffix(), "beta");
        TEST_EQUAL(v.str(), "0.0beta");
        TEST_EQUAL(v.to32(), 0);

    }

    void check_arithmetic_literals() {

        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Woverflow"

        auto a = 123_s8;
        auto b = 123_u8;
        auto c = 123_s16;
        auto d = 123_u16;
        auto e = 123_s32;
        auto f = 123_u32;
        auto g = 123_s64;
        auto h = 123_u64;

        TEST_TYPE_OF(a, int8_t);
        TEST_TYPE_OF(b, uint8_t);
        TEST_TYPE_OF(c, int16_t);
        TEST_TYPE_OF(d, uint16_t);
        TEST_TYPE_OF(e, int32_t);
        TEST_TYPE_OF(f, uint32_t);
        TEST_TYPE_OF(g, int64_t);
        TEST_TYPE_OF(h, uint64_t);

        TEST_EQUAL(a, int8_t(123));
        TEST_EQUAL(b, uint8_t(123));
        TEST_EQUAL(c, int16_t(123));
        TEST_EQUAL(d, uint16_t(123));
        TEST_EQUAL(e, int32_t(123));
        TEST_EQUAL(f, uint32_t(123));
        TEST_EQUAL(g, int64_t(123));
        TEST_EQUAL(h, uint64_t(123));

        ptrdiff_t pt = 0;
        size_t sz = 0;

        TRY(pt = 0_pt);
        TEST_EQUAL(pt, 0_pt);
        TEST_EQUAL(decfmt(pt), "0");
        TEST_EQUAL(hexfmt(pt, 1), "0");
        TRY(pt = 12345_pt);
        TEST_EQUAL(pt, 12345_pt);
        TEST_EQUAL(decfmt(pt), "12345");
        TRY(pt = 0x0_pt);
        TEST_EQUAL(decfmt(pt), "0");
        TRY(pt = 0x12345_pt);
        TEST_EQUAL(hexfmt(pt, 1), "12345");
        TEST_EQUAL(decfmt(pt), "74565");

        TRY(sz = 0_sz);
        TEST_EQUAL(sz, 0_sz);
        TEST_EQUAL(decfmt(sz), "0");
        TEST_EQUAL(hexfmt(sz, 1), "0");
        TRY(sz = 12345_sz);
        TEST_EQUAL(sz, 12345_sz);
        TEST_EQUAL(decfmt(sz), "12345");
        TRY(sz = 0x0_sz);
        TEST_EQUAL(decfmt(sz), "0");
        TRY(sz = 0x12345_sz);
        TEST_EQUAL(hexfmt(sz, 1), "12345");
        TEST_EQUAL(decfmt(sz), "74565");

        if (sizeof(size_t) == 8) {

            TRY(pt = 0_pt);
            TEST_EQUAL(hexfmt(pt), "0000000000000000");
            TRY(pt = 9'223'372'036'854'775'807_pt); // 2^63-1
            TEST_EQUAL(pt, 9'223'372'036'854'775'807_pt);
            TEST_EQUAL(decfmt(pt), "9223372036854775807");
            TRY(pt = -9'223'372'036'854'775'807_pt); // -(2^63-1)
            TEST_EQUAL(pt, -9'223'372'036'854'775'807_pt);
            TEST_EQUAL(decfmt(pt), "-9223372036854775807");
            TRY(pt = -9'223'372'036'854'775'808_pt); // -2^63
            TEST_EQUAL(pt, -9'223'372'036'854'775'808_pt);
            TEST_EQUAL(decfmt(pt), "-9223372036854775808");
            TRY(pt = 0x7fffffffffffffff_pt);
            TEST_EQUAL(hexfmt(pt), "7fffffffffffffff");
            TEST_EQUAL(decfmt(pt), "9223372036854775807");

            TRY(sz = 0_sz);
            TEST_EQUAL(hexfmt(sz), "0000000000000000");
            TRY(sz = 18'446'744'073'709'551'615_sz); // 2^64-1
            TEST_EQUAL(sz, 18'446'744'073'709'551'615_sz);
            TEST_EQUAL(decfmt(sz), "18446744073709551615");
            TRY(sz = 0xffffffffffffffff_sz);
            TEST_EQUAL(hexfmt(sz), "ffffffffffffffff");
            TEST_EQUAL(decfmt(sz), "18446744073709551615");

        } else {

            TRY(pt = 0_pt);
            TEST_EQUAL(hexfmt(pt), "00000000");
            TRY(pt = 2'147'483'647_pt); // 2^31-1
            TEST_EQUAL(pt, 2'147'483'647_pt);
            TEST_EQUAL(decfmt(pt), "2147483647");
            TRY(pt = -2'147'483'647_pt); // -(2^31-1)
            TEST_EQUAL(pt, -2'147'483'647_pt);
            TEST_EQUAL(decfmt(pt), "-2147483647");
            TRY(pt = -2'147'483'648_pt); // -2^31
            TEST_EQUAL(pt, -2'147'483'648_pt);
            TEST_EQUAL(decfmt(pt), "-2147483648");
            TRY(pt = 0x7fffffff_pt);
            TEST_EQUAL(hexfmt(pt), "7fffffff");
            TEST_EQUAL(decfmt(pt), "2147483647");

            TRY(sz = 0_sz);
            TEST_EQUAL(hexfmt(sz), "00000000");
            TRY(sz = 4'294'967'295_sz); // 2^32-1
            TEST_EQUAL(sz, 4'294'967'295_sz);
            TEST_EQUAL(decfmt(sz), "4294967295");
            TRY(sz = 0xffffffff_sz);
            TEST_EQUAL(hexfmt(sz), "ffffffff");
            TEST_EQUAL(decfmt(sz), "4294967295");

        }

        #pragma GCC diagnostic pop

    }

    void check_generic_algorithms() {

        U8string s1, s2;
        std::vector<int> v1, v2, v3, v4;
        int n;

        s1 = "Hello";
        TRY(std::copy(s1.begin(), s1.end(), append(s2)));
        TEST_EQUAL(s2, "Hello");
        s1 = " world";
        TRY(std::copy(s1.begin(), s1.end(), append(s2)));
        TEST_EQUAL(s2, "Hello world");
        s1 = "Goodbye";
        TRY(std::copy(s1.begin(), s1.end(), overwrite(s2)));
        TEST_EQUAL(s2, "Goodbye");

        std::set<int> set1, set2;
        set1 = {1, 2, 3};
        TRY(std::copy(set1.begin(), set1.end(), append(set2)));
        TEST_EQUAL(make_str(set2), "[1,2,3]");
        set1 = {4, 5, 6};
        TRY(std::copy(set1.begin(), set1.end(), append(set2)));
        TEST_EQUAL(make_str(set2), "[1,2,3,4,5,6]");
        set1 = {1, 2, 3};
        TRY(std::copy(set1.begin(), set1.end(), append(set2)));
        TEST_EQUAL(make_str(set2), "[1,2,3,4,5,6]");
        set1 = {7, 8, 9};
        TRY(std::copy(set1.begin(), set1.end(), overwrite(set2)));
        TEST_EQUAL(make_str(set2), "[7,8,9]");

        s1 = "";               s2 = "";               TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 0);
        s1 = "";               s2 = "hello";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
        s1 = "hello";          s2 = "";               TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
        s1 = "hello";          s2 = "world";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
        s1 = "hello";          s2 = "hello";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 0);
        s1 = "hello";          s2 = "goodbye";        TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
        s1 = "hello";          s2 = "hello world";    TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
        s1 = "hello world";    s2 = "hello";          TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
        s1 = "hello goodbye";  s2 = "hello world";    TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, -1);
        s1 = "hello world";    s2 = "hello goodbye";  TRY(n = compare_3way(s1, s2));                    TEST_EQUAL(n, 1);
        s1 = "";               s2 = "";               TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 0);
        s1 = "";               s2 = "hello";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);
        s1 = "hello";          s2 = "";               TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
        s1 = "hello";          s2 = "world";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
        s1 = "hello";          s2 = "hello";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 0);
        s1 = "hello";          s2 = "goodbye";        TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);
        s1 = "hello";          s2 = "hello world";    TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);
        s1 = "hello world";    s2 = "hello";          TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
        s1 = "hello goodbye";  s2 = "hello world";    TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, 1);
        s1 = "hello world";    s2 = "hello goodbye";  TRY(n = compare_3way(s1, s2, std::greater<>()));  TEST_EQUAL(n, -1);

        const auto is_alpha = [] (char c) { return isalpha(c); };
        const auto same_case = [] (char a, char b) { return islower(a) == islower(b) && isupper(a) == isupper(b); };

        s1 = "abcabcabc";        TRY(con_remove(s1, 'c'));                        TEST_EQUAL(s1, "ababab");
        s1 = "abc123abc123";     TRY(con_remove_if(s1, is_alpha));                TEST_EQUAL(s1, "123123");
        s1 = "abc123abc123";     TRY(con_remove_if_not(s1, is_alpha));            TEST_EQUAL(s1, "abcabc");
        s1 = "abbcccddddeeeee";  TRY(con_unique(s1));                             TEST_EQUAL(s1, "abcde");
        s1 = "ABCabcABCabc";     TRY(con_unique(s1, same_case));                  TEST_EQUAL(s1, "AaAa");
        s1 = "abcdeabcdabcaba";  TRY(con_sort_unique(s1));                        TEST_EQUAL(s1, "abcde");
        s1 = "abcdeabcdabcaba";  TRY(con_sort_unique(s1, std::greater<char>()));  TEST_EQUAL(s1, "edcba");

        v2 = {1, 2, 3};
        v3 = {4, 5, 6};
        v4 = {7, 8, 9};

        TRY(v1 = concatenate(v2));          TEST_EQUAL(make_str(v1), "[1,2,3]");
        TRY(v1 = concatenate(v2, v3));      TEST_EQUAL(make_str(v1), "[1,2,3,4,5,6]");
        TRY(v1 = concatenate(v2, v3, v4));  TEST_EQUAL(make_str(v1), "[1,2,3,4,5,6,7,8,9]");

        auto f1 = [&] { s1 += "Hello"; };
        auto f2 = [&] (size_t i) { s1 += decfmt(i) + ";"; };

        s1.clear();  TRY(do_n(0, f1));   TEST_EQUAL(s1, "");
        s1.clear();  TRY(do_n(1, f1));   TEST_EQUAL(s1, "Hello");
        s1.clear();  TRY(do_n(2, f1));   TEST_EQUAL(s1, "HelloHello");
        s1.clear();  TRY(do_n(3, f1));   TEST_EQUAL(s1, "HelloHelloHello");
        s1.clear();  TRY(for_n(0, f2));  TEST_EQUAL(s1, "");
        s1.clear();  TRY(for_n(1, f2));  TEST_EQUAL(s1, "0;");
        s1.clear();  TRY(for_n(2, f2));  TEST_EQUAL(s1, "0;1;");
        s1.clear();  TRY(for_n(3, f2));  TEST_EQUAL(s1, "0;1;2;");

        std::map<int, U8string> m = {
            {1, "alpha"},
            {2, "bravo"},
            {3, "charlie"},
        };
        U8string s;

        TRY(s = find_in_map(m, 1));          TEST_EQUAL(s, "alpha");
        TRY(s = find_in_map(m, 2));          TEST_EQUAL(s, "bravo");
        TRY(s = find_in_map(m, 3));          TEST_EQUAL(s, "charlie");
        TRY(s = find_in_map(m, 4));          TEST_EQUAL(s, "");
        TRY(s = find_in_map(m, 1, "none"));  TEST_EQUAL(s, "alpha");
        TRY(s = find_in_map(m, 2, "none"));  TEST_EQUAL(s, "bravo");
        TRY(s = find_in_map(m, 3, "none"));  TEST_EQUAL(s, "charlie");
        TRY(s = find_in_map(m, 4, "none"));  TEST_EQUAL(s, "none");

        std::set<int> is1, is2;
        TEST(! sets_intersect(is1, is2));
        is1 = {1,2,3,4,5};
        TEST(! sets_intersect(is1, is2));
        is2 = {3,6,9};
        TEST(sets_intersect(is1, is2));
        is2 = {10,20};
        TEST(! sets_intersect(is1, is2));

        std::vector<int> iv0, iv5 = {1,2,3,4,5};
        const std::vector<int>& civ5(iv5);
        double d, da5[] = {10,20,30,40,50};

        TRY(n = sum_of(iv0));       TEST_EQUAL(n, 0);
        TRY(n = sum_of(iv5));       TEST_EQUAL(n, 15);
        TRY(n = sum_of(civ5));      TEST_EQUAL(n, 15);
        TRY(n = product_of(iv0));   TEST_EQUAL(n, 1);
        TRY(n = product_of(iv5));   TEST_EQUAL(n, 120);
        TRY(n = product_of(civ5));  TEST_EQUAL(n, 120);
        TRY(d = sum_of(da5));       TEST_EQUAL(d, 150);
        TRY(d = product_of(da5));   TEST_EQUAL(d, 1.2e7);

    }

    void check_integer_sequences() {

        std::vector<int> v;
        U8string s;

        TRY(con_overwrite(iseq(5), v));            TRY(s = make_str(v));  TEST_EQUAL(s, "[0,1,2,3,4,5]");
        TRY(con_overwrite(iseq(-5), v));           TRY(s = make_str(v));  TEST_EQUAL(s, "[0,-1,-2,-3,-4,-5]");
        TRY(con_overwrite(iseq(1, 5), v));         TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(iseq(-1, -5), v));       TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4,-5]");
        TRY(con_overwrite(iseq(1, -1, 1), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(iseq(1, 0, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(iseq(1, 1, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1]");
        TRY(con_overwrite(iseq(1, 2, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2]");
        TRY(con_overwrite(iseq(1, 3, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3]");
        TRY(con_overwrite(iseq(1, 4, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(iseq(1, 5, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(iseq(-1, 1, -1), v));    TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(iseq(-1, 0, -1), v));    TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(iseq(-1, -1, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1]");
        TRY(con_overwrite(iseq(-1, -2, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2]");
        TRY(con_overwrite(iseq(-1, -3, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3]");
        TRY(con_overwrite(iseq(-1, -4, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4]");
        TRY(con_overwrite(iseq(-1, -5, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4,-5]");
        TRY(con_overwrite(iseq(1, 10, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7,10]");
        TRY(con_overwrite(iseq(1, 11, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7,10]");
        TRY(con_overwrite(iseq(1, 12, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7,10]");
        TRY(con_overwrite(iseq(1, 13, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7,10,13]");
        TRY(con_overwrite(iseq(-1, -10, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7,-10]");
        TRY(con_overwrite(iseq(-1, -11, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7,-10]");
        TRY(con_overwrite(iseq(-1, -12, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7,-10]");
        TRY(con_overwrite(iseq(-1, -13, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7,-10,-13]");

        TRY(con_overwrite(xseq(5), v));            TRY(s = make_str(v));  TEST_EQUAL(s, "[0,1,2,3,4]");
        TRY(con_overwrite(xseq(-5), v));           TRY(s = make_str(v));  TEST_EQUAL(s, "[0,-1,-2,-3,-4]");
        TRY(con_overwrite(xseq(1, 5), v));         TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(xseq(-1, -5), v));       TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4]");
        TRY(con_overwrite(xseq(1, -1, 1), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(xseq(1, 0, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(xseq(1, 1, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(xseq(1, 2, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1]");
        TRY(con_overwrite(xseq(1, 3, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2]");
        TRY(con_overwrite(xseq(1, 4, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3]");
        TRY(con_overwrite(xseq(1, 5, 1), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(xseq(-1, 1, -1), v));    TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(xseq(-1, 0, -1), v));    TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(xseq(-1, -1, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[]");
        TRY(con_overwrite(xseq(-1, -2, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1]");
        TRY(con_overwrite(xseq(-1, -3, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2]");
        TRY(con_overwrite(xseq(-1, -4, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3]");
        TRY(con_overwrite(xseq(-1, -5, -1), v));   TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4]");
        TRY(con_overwrite(xseq(1, 10, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7]");
        TRY(con_overwrite(xseq(1, 11, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7,10]");
        TRY(con_overwrite(xseq(1, 12, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7,10]");
        TRY(con_overwrite(xseq(1, 13, 3), v));     TRY(s = make_str(v));  TEST_EQUAL(s, "[1,4,7,10]");
        TRY(con_overwrite(xseq(-1, -10, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7]");
        TRY(con_overwrite(xseq(-1, -11, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7,-10]");
        TRY(con_overwrite(xseq(-1, -12, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7,-10]");
        TRY(con_overwrite(xseq(-1, -13, -3), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-4,-7,-10]");

        TRY(con_overwrite(iseq(int8_t(1), int8_t(5)), v));          TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(iseq(int8_t(-1), int8_t(-5)), v));        TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4,-5]");
        TRY(con_overwrite(xseq(int8_t(1), int8_t(5)), v));          TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(xseq(int8_t(-1), int8_t(-5)), v));        TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4]");
        TRY(con_overwrite(iseq(uint8_t(1), uint8_t(5)), v));        TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(xseq(uint8_t(1), uint8_t(5)), v));        TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(iseq(int16_t(1), int16_t(5)), v));        TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(iseq(int16_t(-1), int16_t(-5)), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4,-5]");
        TRY(con_overwrite(xseq(int16_t(1), int16_t(5)), v));        TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(xseq(int16_t(-1), int16_t(-5)), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4]");
        TRY(con_overwrite(iseq(uint16_t(1), uint16_t(5)), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(xseq(uint16_t(1), uint16_t(5)), v));      TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(iseq(ptrdiff_t(1), ptrdiff_t(5)), v));    TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(iseq(ptrdiff_t(-1), ptrdiff_t(-5)), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4,-5]");
        TRY(con_overwrite(xseq(ptrdiff_t(1), ptrdiff_t(5)), v));    TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(con_overwrite(xseq(ptrdiff_t(-1), ptrdiff_t(-5)), v));  TRY(s = make_str(v));  TEST_EQUAL(s, "[-1,-2,-3,-4]");
        TRY(con_overwrite(iseq(size_t(1), size_t(5)), v));          TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4,5]");
        TRY(con_overwrite(xseq(size_t(1), size_t(5)), v));          TRY(s = make_str(v));  TEST_EQUAL(s, "[1,2,3,4]");

    }

    void check_memory_algorithms() {

        std::string s1, s2;
        int x = 0;
        size_t z = 0;

        s1 = "";             s2 = "";             TRY(x = mem_compare(s1.data(), s1.size(), s2.data(), s2.size()));  TEST_EQUAL(x, 0);
        s1 = "hello";        s2 = "";             TRY(x = mem_compare(s1.data(), s1.size(), s2.data(), s2.size()));  TEST_EQUAL(x, 1);
        s1 = "";             s2 = "world";        TRY(x = mem_compare(s1.data(), s1.size(), s2.data(), s2.size()));  TEST_EQUAL(x, -1);
        s1 = "hello";        s2 = "world";        TRY(x = mem_compare(s1.data(), s1.size(), s2.data(), s2.size()));  TEST_EQUAL(x, -1);
        s1 = "hello world";  s2 = "hello";        TRY(x = mem_compare(s1.data(), s1.size(), s2.data(), s2.size()));  TEST_EQUAL(x, 1);
        s1 = "hello";        s2 = "hello world";  TRY(x = mem_compare(s1.data(), s1.size(), s2.data(), s2.size()));  TEST_EQUAL(x, -1);

        TRY(x = mem_compare(nullptr, 42, s2.data(), s2.size()));  TEST_EQUAL(x, -1);
        TRY(x = mem_compare(s1.data(), s1.size(), nullptr, 42));  TEST_EQUAL(x, 1);
        TRY(x = mem_compare(nullptr, 42, nullptr, 42));           TEST_EQUAL(x, 0);

        s1 = "";             s2 = "";               TRY(z = mem_match(s1.data(), s2.data(), 0));   TEST_EQUAL(z, 0);
        s1 = "hello";        s2 = "hello";          TRY(z = mem_match(s1.data(), s2.data(), 0));   TEST_EQUAL(z, 0);
        s1 = "hello";        s2 = "hello";          TRY(z = mem_match(s1.data(), s2.data(), 5));   TEST_EQUAL(z, 5);
        s1 = "hello world";  s2 = "hello world";    TRY(z = mem_match(s1.data(), s2.data(), 11));  TEST_EQUAL(z, 11);
        s1 = "hello world";  s2 = "hello goodbye";  TRY(z = mem_match(s1.data(), s2.data(), 11));  TEST_EQUAL(z, 6);
        s1 = "hello world";  s2 = "jello world";    TRY(z = mem_match(s1.data(), s2.data(), 11));  TEST_EQUAL(z, 0);

        s1 = "hello";
        s2 = "world";
        TRY(mem_swap(&s1[0], &s2[0], 5));
        TEST_EQUAL(s1, "world");
        TEST_EQUAL(s2, "hello");

    }

    void check_range_traits() {

        char array[] {'H','e','l','l','o'};

        TEST_TYPE(RangeIterator<std::string>, std::string::iterator);
        TEST_TYPE(RangeIterator<const std::string>, std::string::const_iterator);
        TEST_TYPE(RangeIterator<std::vector<int>>, std::vector<int>::iterator);
        TEST_TYPE(RangeIterator<const std::vector<int>>, std::vector<int>::const_iterator);
        TEST_TYPE(RangeIterator<Irange<int*>>, int*);
        TEST_TYPE(RangeIterator<Irange<const int*>>, const int*);

        TEST_TYPE(RangeValue<std::string>, char);
        TEST_TYPE(RangeValue<std::vector<int>>, int);
        TEST_TYPE(RangeValue<Irange<int*>>, int);
        TEST_TYPE(RangeValue<Irange<const int*>>, int);

        TEST_EQUAL(array_count(""), 1);
        TEST_EQUAL(array_count("Hello"), 6);
        TEST_EQUAL(array_count(array), 5);
        TEST_EQUAL(range_count(""s), 0);
        TEST_EQUAL(range_count("Hello"s), 5);
        TEST_EQUAL(range_count(""), 1);
        TEST_EQUAL(range_count("Hello"), 6);
        TEST(range_empty(""s));
        TEST(! range_empty("Hello"s));

    }

    void check_range_types() {

        char array[] {'H','e','l','l','o'};
        const char* cp = array;
        std::string s1, s2;

        auto ar = array_range(array, sizeof(array));
        TEST_EQUAL(ar.size(), 5);
        TRY(std::copy(ar.begin(), ar.end(), overwrite(s1)));
        TEST_EQUAL(s1, "Hello");

        auto car = array_range(cp, sizeof(array));
        TEST_EQUAL(car.size(), 5);
        TRY(std::copy(car.begin(), car.end(), overwrite(s1)));
        TEST_EQUAL(s1, "Hello");

    }

    void check_generic_arithmetic_functions() {

        TEST_EQUAL(clamp(1, 2, 4), 2);
        TEST_EQUAL(clamp(2, 2, 4), 2);
        TEST_EQUAL(clamp(3, 2, 4), 3);
        TEST_EQUAL(clamp(4, 2, 4), 4);
        TEST_EQUAL(clamp(5, 2, 4), 4);
        TEST_EQUAL(clamp(1.0, 2, 4), 2);
        TEST_EQUAL(clamp(1.5, 2, 4), 2);
        TEST_EQUAL(clamp(2.0, 2, 4), 2);
        TEST_EQUAL(clamp(2.5, 2, 4), 2.5);
        TEST_EQUAL(clamp(3.0, 2, 4), 3);
        TEST_EQUAL(clamp(3.5, 2, 4), 3.5);
        TEST_EQUAL(clamp(4.0, 2, 4), 4);
        TEST_EQUAL(clamp(4.5, 2, 4), 4);
        TEST_EQUAL(clamp(5.0, 2, 4), 4);

        int sx, sy, sq, sr;

        sx = -6;  sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -5;  sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -4;  sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -3;  sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -2;  sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -1;  sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 0;   sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 1;   sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 2;   sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 3;   sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 4;   sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 5;   sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 6;   sy = 3;   TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -6;  sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -5;  sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -4;  sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -3;  sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -2;  sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -1;  sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 0;   sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 1;   sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 2;   sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 3;   sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 4;   sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 5;   sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 2);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 6;   sy = -3;  TRY(sq = quo(sx, sy));  TRY(sr = rem(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 0);  TEST_EQUAL(sy * sq + sr, sx);

        sx = -6;  sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -5;  sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -4;  sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -3;  sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -2;  sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -1;  sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 0;   sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 1;   sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 2;   sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 3;   sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 4;   sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 5;   sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 6;   sy = 3;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -6;  sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -5;  sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -4;  sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -3;  sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -2;  sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -1;  sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 0;   sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 1;   sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 2;   sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 3;   sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 4;   sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 5;   sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 6;   sy = -3;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);

        sx = -8;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -7;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -6;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -5;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -4;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -3;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -2;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -1;  sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 0;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 1;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 2;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 3;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 4;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 5;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 6;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 7;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 8;   sy = 4;   TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -8;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -7;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -6;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 2);   TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -5;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = -4;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -3;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -2;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 1);   TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = -1;  sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 0;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 1;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 2;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, 0);   TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 3;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 4;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 5;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 1);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 6;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -1);  TEST_EQUAL(sr, 2);   TEST_EQUAL(sy * sq + sr, sx);
        sx = 7;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, -1);  TEST_EQUAL(sy * sq + sr, sx);
        sx = 8;   sy = -4;  TRY(sq = symmetric_quotient(sx, sy));  TRY(sr = symmetric_remainder(sx, sy));  TEST_EQUAL(sq, -2);  TEST_EQUAL(sr, 0);   TEST_EQUAL(sy * sq + sr, sx);

        unsigned ux, uy, uq, ur;

        ux = 0;  uy = 3;  TRY(uq = quo(ux, uy));  TRY(ur = rem(ux, uy));  TEST_EQUAL(uq, 0);  TEST_EQUAL(ur, 0);  TEST_EQUAL(uy * uq + ur, ux);
        ux = 1;  uy = 3;  TRY(uq = quo(ux, uy));  TRY(ur = rem(ux, uy));  TEST_EQUAL(uq, 0);  TEST_EQUAL(ur, 1);  TEST_EQUAL(uy * uq + ur, ux);
        ux = 2;  uy = 3;  TRY(uq = quo(ux, uy));  TRY(ur = rem(ux, uy));  TEST_EQUAL(uq, 0);  TEST_EQUAL(ur, 2);  TEST_EQUAL(uy * uq + ur, ux);
        ux = 3;  uy = 3;  TRY(uq = quo(ux, uy));  TRY(ur = rem(ux, uy));  TEST_EQUAL(uq, 1);  TEST_EQUAL(ur, 0);  TEST_EQUAL(uy * uq + ur, ux);
        ux = 4;  uy = 3;  TRY(uq = quo(ux, uy));  TRY(ur = rem(ux, uy));  TEST_EQUAL(uq, 1);  TEST_EQUAL(ur, 1);  TEST_EQUAL(uy * uq + ur, ux);
        ux = 5;  uy = 3;  TRY(uq = quo(ux, uy));  TRY(ur = rem(ux, uy));  TEST_EQUAL(uq, 1);  TEST_EQUAL(ur, 2);  TEST_EQUAL(uy * uq + ur, ux);
        ux = 6;  uy = 3;  TRY(uq = quo(ux, uy));  TRY(ur = rem(ux, uy));  TEST_EQUAL(uq, 2);  TEST_EQUAL(ur, 0);  TEST_EQUAL(uy * uq + ur, ux);

        double fx, fy, fq, fr;

        fx = -6;  fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -5;  fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -4;  fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -3;  fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -2;  fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -1;  fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 0;   fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 1;   fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 2;   fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 3;   fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 4;   fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 5;   fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 6;   fy = 3;   TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -6;  fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -5;  fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -4;  fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -3;  fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -2;  fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -1;  fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 0;   fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 1;   fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 2;   fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 3;   fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 4;   fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 5;   fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 2);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 6;   fy = -3;  TRY(fq = quo(fx, fy));  TRY(fr = rem(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 0);  TEST_EQUAL(fy * fq + fr, fx);

        fx = -8;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -7;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -6;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -5;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -4;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -3;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -2;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -1;  fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 0;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 1;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 2;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 3;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 4;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 5;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 6;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 7;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 8;   fy = 4;   TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -8;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -7;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -6;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 2);   TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -5;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = -4;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -3;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -2;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 1);   TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = -1;  fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 0;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 1;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 2;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, 0);   TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 3;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 4;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 5;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 1);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 6;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -1);  TEST_EQUAL(fr, 2);   TEST_EQUAL(fy * fq + fr, fx);
        fx = 7;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, -1);  TEST_EQUAL(fy * fq + fr, fx);
        fx = 8;   fy = -4;  TRY(fq = symmetric_quotient(fx, fy));  TRY(fr = symmetric_remainder(fx, fy));  TEST_EQUAL(fq, -2);  TEST_EQUAL(fr, 0);   TEST_EQUAL(fy * fq + fr, fx);

        TEST_EQUAL(shift_left(42, 0), 42);           TEST_EQUAL(shift_right(42, 0), 42);
        TEST_EQUAL(shift_left(42, 1), 84);           TEST_EQUAL(shift_right(42, 1), 21);
        TEST_EQUAL(shift_left(42, 2), 168);          TEST_EQUAL(shift_right(42, 2), 10);
        TEST_EQUAL(shift_left(42, 3), 336);          TEST_EQUAL(shift_right(42, 3), 5);
        TEST_EQUAL(shift_left(42, 4), 672);          TEST_EQUAL(shift_right(42, 4), 2);
        TEST_EQUAL(shift_left(42, 5), 1344);         TEST_EQUAL(shift_right(42, 5), 1);
        TEST_EQUAL(shift_left(42, 6), 2688);         TEST_EQUAL(shift_right(42, 6), 0);
        TEST_EQUAL(shift_left(42, 7), 5376);         TEST_EQUAL(shift_right(42, 7), 0);
        TEST_EQUAL(shift_left(42, -1), 21);          TEST_EQUAL(shift_right(42, -1), 84);
        TEST_EQUAL(shift_left(42, -2), 10);          TEST_EQUAL(shift_right(42, -2), 168);
        TEST_EQUAL(shift_left(42, -3), 5);           TEST_EQUAL(shift_right(42, -3), 336);
        TEST_EQUAL(shift_left(42, -4), 2);           TEST_EQUAL(shift_right(42, -4), 672);
        TEST_EQUAL(shift_left(42, -5), 1);           TEST_EQUAL(shift_right(42, -5), 1344);
        TEST_EQUAL(shift_left(42, -6), 0);           TEST_EQUAL(shift_right(42, -6), 2688);
        TEST_EQUAL(shift_left(42, -7), 0);           TEST_EQUAL(shift_right(42, -7), 5376);
        TEST_EQUAL(shift_left(-42, 0), -42);         TEST_EQUAL(shift_right(-42, 0), -42);
        TEST_EQUAL(shift_left(-42, 1), -84);         TEST_EQUAL(shift_right(-42, 1), -21);
        TEST_EQUAL(shift_left(-42, 2), -168);        TEST_EQUAL(shift_right(-42, 2), -10);
        TEST_EQUAL(shift_left(-42, 3), -336);        TEST_EQUAL(shift_right(-42, 3), -5);
        TEST_EQUAL(shift_left(-42, 4), -672);        TEST_EQUAL(shift_right(-42, 4), -2);
        TEST_EQUAL(shift_left(-42, 5), -1344);       TEST_EQUAL(shift_right(-42, 5), -1);
        TEST_EQUAL(shift_left(-42, 6), -2688);       TEST_EQUAL(shift_right(-42, 6), 0);
        TEST_EQUAL(shift_left(-42, 7), -5376);       TEST_EQUAL(shift_right(-42, 7), 0);
        TEST_EQUAL(shift_left(-42, -1), -21);        TEST_EQUAL(shift_right(-42, -1), -84);
        TEST_EQUAL(shift_left(-42, -2), -10);        TEST_EQUAL(shift_right(-42, -2), -168);
        TEST_EQUAL(shift_left(-42, -3), -5);         TEST_EQUAL(shift_right(-42, -3), -336);
        TEST_EQUAL(shift_left(-42, -4), -2);         TEST_EQUAL(shift_right(-42, -4), -672);
        TEST_EQUAL(shift_left(-42, -5), -1);         TEST_EQUAL(shift_right(-42, -5), -1344);
        TEST_EQUAL(shift_left(-42, -6), 0);          TEST_EQUAL(shift_right(-42, -6), -2688);
        TEST_EQUAL(shift_left(-42, -7), 0);          TEST_EQUAL(shift_right(-42, -7), -5376);
        TEST_EQUAL(shift_left(42.0, 0), 42.0);       TEST_EQUAL(shift_right(42.0, 0), 42.0);
        TEST_EQUAL(shift_left(42.0, 1), 84.0);       TEST_EQUAL(shift_right(42.0, 1), 21.0);
        TEST_EQUAL(shift_left(42.0, 2), 168.0);      TEST_EQUAL(shift_right(42.0, 2), 10.5);
        TEST_EQUAL(shift_left(42.0, 3), 336.0);      TEST_EQUAL(shift_right(42.0, 3), 5.25);
        TEST_EQUAL(shift_left(42.0, 4), 672.0);      TEST_EQUAL(shift_right(42.0, 4), 2.625);
        TEST_EQUAL(shift_left(42.0, 5), 1344.0);     TEST_EQUAL(shift_right(42.0, 5), 1.3125);
        TEST_EQUAL(shift_left(42.0, 6), 2688.0);     TEST_EQUAL(shift_right(42.0, 6), 0.65625);
        TEST_EQUAL(shift_left(42.0, 7), 5376.0);     TEST_EQUAL(shift_right(42.0, 7), 0.328125);
        TEST_EQUAL(shift_left(42.0, -1), 21.0);      TEST_EQUAL(shift_right(42.0, -1), 84.0);
        TEST_EQUAL(shift_left(42.0, -2), 10.5);      TEST_EQUAL(shift_right(42.0, -2), 168.0);
        TEST_EQUAL(shift_left(42.0, -3), 5.25);      TEST_EQUAL(shift_right(42.0, -3), 336.0);
        TEST_EQUAL(shift_left(42.0, -4), 2.625);     TEST_EQUAL(shift_right(42.0, -4), 672.0);
        TEST_EQUAL(shift_left(42.0, -5), 1.3125);    TEST_EQUAL(shift_right(42.0, -5), 1344.0);
        TEST_EQUAL(shift_left(42.0, -6), 0.65625);   TEST_EQUAL(shift_right(42.0, -6), 2688.0);
        TEST_EQUAL(shift_left(42.0, -7), 0.328125);  TEST_EQUAL(shift_right(42.0, -7), 5376.0);

        TEST_EQUAL(sign_of(0), 0);
        TEST_EQUAL(sign_of(42), 1);
        TEST_EQUAL(sign_of(-42), -1);
        TEST_EQUAL(sign_of(0u), 0);
        TEST_EQUAL(sign_of(42u), 1);
        TEST_EQUAL(sign_of(0.0), 0);
        TEST_EQUAL(sign_of(0.1), 1);
        TEST_EQUAL(sign_of(42.0), 1);
        TEST_EQUAL(sign_of(1.234e56), 1);
        TEST_EQUAL(sign_of(-0.1), -1);
        TEST_EQUAL(sign_of(-42.0), -1);
        TEST_EQUAL(sign_of(-1.234e56), -1);

    }

    void check_integer_arithmetic_functions() {

        int8_t s8 = -42;
        uint8_t u8 = 42;
        int16_t s16 = -42;
        uint16_t u16 = 42;
        int32_t s32 = -42;
        uint32_t u32 = 42;
        int64_t s64 = -42;
        uint64_t u64 = 42;

        TEST_EQUAL(sizeof(as_signed(s8)), 1);
        TEST_EQUAL(sizeof(as_signed(u8)), 1);
        TEST_EQUAL(sizeof(as_unsigned(s8)), 1);
        TEST_EQUAL(sizeof(as_unsigned(u8)), 1);
        TEST_EQUAL(sizeof(as_signed(s16)), 2);
        TEST_EQUAL(sizeof(as_signed(u16)), 2);
        TEST_EQUAL(sizeof(as_unsigned(s16)), 2);
        TEST_EQUAL(sizeof(as_unsigned(u16)), 2);
        TEST_EQUAL(sizeof(as_signed(s32)), 4);
        TEST_EQUAL(sizeof(as_signed(u32)), 4);
        TEST_EQUAL(sizeof(as_unsigned(s32)), 4);
        TEST_EQUAL(sizeof(as_unsigned(u32)), 4);
        TEST_EQUAL(sizeof(as_signed(s64)), 8);
        TEST_EQUAL(sizeof(as_signed(u64)), 8);
        TEST_EQUAL(sizeof(as_unsigned(s64)), 8);
        TEST_EQUAL(sizeof(as_unsigned(u64)), 8);

        TEST(std::is_signed<decltype(as_signed(s8))>::value);
        TEST(std::is_signed<decltype(as_signed(u8))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(s8))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(u8))>::value);
        TEST(std::is_signed<decltype(as_signed(s16))>::value);
        TEST(std::is_signed<decltype(as_signed(u16))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(s16))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(u16))>::value);
        TEST(std::is_signed<decltype(as_signed(s32))>::value);
        TEST(std::is_signed<decltype(as_signed(u32))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(s32))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(u32))>::value);
        TEST(std::is_signed<decltype(as_signed(s64))>::value);
        TEST(std::is_signed<decltype(as_signed(u64))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(s64))>::value);
        TEST(std::is_unsigned<decltype(as_unsigned(u64))>::value);

        TEST_EQUAL(as_signed(s8), -42);
        TEST_EQUAL(as_signed(u8), 42);
        TEST_EQUAL(as_unsigned(s8), 214);
        TEST_EQUAL(as_unsigned(u8), 42);
        TEST_EQUAL(as_signed(s16), -42);
        TEST_EQUAL(as_signed(u16), 42);
        TEST_EQUAL(as_unsigned(s16), 65494);
        TEST_EQUAL(as_unsigned(u16), 42);
        TEST_EQUAL(as_signed(s32), -42);
        TEST_EQUAL(as_signed(u32), 42);
        TEST_EQUAL(as_unsigned(s32), 4'294'967'254ul);
        TEST_EQUAL(as_unsigned(u32), 42);
        TEST_EQUAL(as_signed(s64), -42);
        TEST_EQUAL(as_signed(u64), 42);
        TEST_EQUAL(as_unsigned(s64), 18'446'744'073'709'551'574ull);
        TEST_EQUAL(as_unsigned(u64), 42);

        TEST_EQUAL(binomial(-1, -1), 0);
        TEST_EQUAL(binomial(-1, 0), 0);
        TEST_EQUAL(binomial(-1, 1), 0);
        TEST_EQUAL(binomial(0, -1), 0);
        TEST_EQUAL(binomial(0, 0), 1);
        TEST_EQUAL(binomial(0, 1), 0);
        TEST_EQUAL(binomial(1, -1), 0);
        TEST_EQUAL(binomial(1, 0), 1);
        TEST_EQUAL(binomial(1, 1), 1);
        TEST_EQUAL(binomial(1, 2), 0);
        TEST_EQUAL(binomial(2, -1), 0);
        TEST_EQUAL(binomial(2, 0), 1);
        TEST_EQUAL(binomial(2, 1), 2);
        TEST_EQUAL(binomial(2, 2), 1);
        TEST_EQUAL(binomial(2, 3), 0);
        TEST_EQUAL(binomial(3, -1), 0);
        TEST_EQUAL(binomial(3, 0), 1);
        TEST_EQUAL(binomial(3, 1), 3);
        TEST_EQUAL(binomial(3, 2), 3);
        TEST_EQUAL(binomial(3, 3), 1);
        TEST_EQUAL(binomial(3, 4), 0);
        TEST_EQUAL(binomial(4, -1), 0);
        TEST_EQUAL(binomial(4, 0), 1);
        TEST_EQUAL(binomial(4, 1), 4);
        TEST_EQUAL(binomial(4, 2), 6);
        TEST_EQUAL(binomial(4, 3), 4);
        TEST_EQUAL(binomial(4, 4), 1);
        TEST_EQUAL(binomial(4, 5), 0);
        TEST_EQUAL(binomial(5, -1), 0);
        TEST_EQUAL(binomial(5, 0), 1);
        TEST_EQUAL(binomial(5, 1), 5);
        TEST_EQUAL(binomial(5, 2), 10);
        TEST_EQUAL(binomial(5, 3), 10);
        TEST_EQUAL(binomial(5, 4), 5);
        TEST_EQUAL(binomial(5, 5), 1);
        TEST_EQUAL(binomial(5, 6), 0);

        TEST_EQUAL(xbinomial(-1, -1), 0);
        TEST_EQUAL(xbinomial(-1, 0), 0);
        TEST_EQUAL(xbinomial(-1, 1), 0);
        TEST_EQUAL(xbinomial(0, -1), 0);
        TEST_EQUAL(xbinomial(0, 0), 1);
        TEST_EQUAL(xbinomial(0, 1), 0);
        TEST_EQUAL(xbinomial(1, -1), 0);
        TEST_EQUAL(xbinomial(1, 0), 1);
        TEST_EQUAL(xbinomial(1, 1), 1);
        TEST_EQUAL(xbinomial(1, 2), 0);
        TEST_EQUAL(xbinomial(2, -1), 0);
        TEST_EQUAL(xbinomial(2, 0), 1);
        TEST_EQUAL(xbinomial(2, 1), 2);
        TEST_EQUAL(xbinomial(2, 2), 1);
        TEST_EQUAL(xbinomial(2, 3), 0);
        TEST_EQUAL(xbinomial(3, -1), 0);
        TEST_EQUAL(xbinomial(3, 0), 1);
        TEST_EQUAL(xbinomial(3, 1), 3);
        TEST_EQUAL(xbinomial(3, 2), 3);
        TEST_EQUAL(xbinomial(3, 3), 1);
        TEST_EQUAL(xbinomial(3, 4), 0);
        TEST_EQUAL(xbinomial(4, -1), 0);
        TEST_EQUAL(xbinomial(4, 0), 1);
        TEST_EQUAL(xbinomial(4, 1), 4);
        TEST_EQUAL(xbinomial(4, 2), 6);
        TEST_EQUAL(xbinomial(4, 3), 4);
        TEST_EQUAL(xbinomial(4, 4), 1);
        TEST_EQUAL(xbinomial(4, 5), 0);
        TEST_EQUAL(xbinomial(5, -1), 0);
        TEST_EQUAL(xbinomial(5, 0), 1);
        TEST_EQUAL(xbinomial(5, 1), 5);
        TEST_EQUAL(xbinomial(5, 2), 10);
        TEST_EQUAL(xbinomial(5, 3), 10);
        TEST_EQUAL(xbinomial(5, 4), 5);
        TEST_EQUAL(xbinomial(5, 5), 1);
        TEST_EQUAL(xbinomial(5, 6), 0);

        int cd, cm, max = 100;

        for (int a = - max; a <= max; ++a) {
            for (int b = - max; b <= max; ++b) {
                TRY(cd = gcd(a, b));
                TRY(cm = lcm(a, b));
                if (a == 0 && b == 0)
                    TEST_EQUAL(cd, 0);
                else
                    TEST_COMPARE(cd, >, 0);
                if (a == 0 || b == 0) {
                    TEST_EQUAL(cd, std::abs(a + b));
                    TEST_EQUAL(cm, 0);
                } else  {
                    TEST_COMPARE(cm, >, 0);
                    TEST_COMPARE(cd, <=, std::min(std::abs(a), std::abs(b)));
                    if (a % b == 0) {
                        TEST_EQUAL(cd, std::abs(b));
                        TEST_EQUAL(cm, std::abs(a));
                    } else if (b % a == 0) {
                        TEST_EQUAL(cd, std::abs(a));
                        TEST_EQUAL(cm, std::abs(b));
                    } else {
                        TEST_EQUAL(a % cd, 0);
                        TEST_EQUAL(b % cd, 0);
                        for (int x = cd + 1, xmax = std::min(a, b) / 2; x <= xmax; ++x)
                            TEST(a % x != 0 || b % x != 0);
                        TEST_COMPARE(cm, >=, std::max(std::abs(a), std::abs(b)));
                        TEST_EQUAL(cm % a, 0);
                        TEST_EQUAL(cm % b, 0);
                    }
                }
                TEST_EQUAL(std::abs(a * b), cd * cm);
            }
        }

        TEST_EQUAL(int_power(0, 1), 0);
        TEST_EQUAL(int_power(0, 2), 0);
        TEST_EQUAL(int_power(0, 3), 0);

        TEST_EQUAL(int_power(1, 0), 1);
        TEST_EQUAL(int_power(1, 1), 1);
        TEST_EQUAL(int_power(1, 2), 1);
        TEST_EQUAL(int_power(1, 3), 1);

        TEST_EQUAL(int_power(-1, 0), 1);
        TEST_EQUAL(int_power(-1, 1), -1);
        TEST_EQUAL(int_power(-1, 2), 1);
        TEST_EQUAL(int_power(-1, 3), -1);

        TEST_EQUAL(int_power(2, 0), 1);
        TEST_EQUAL(int_power(2, 1), 2);
        TEST_EQUAL(int_power(2, 2), 4);
        TEST_EQUAL(int_power(2, 3), 8);
        TEST_EQUAL(int_power(2, 4), 16);
        TEST_EQUAL(int_power(2, 5), 32);
        TEST_EQUAL(int_power(2, 6), 64);
        TEST_EQUAL(int_power(2, 7), 128);
        TEST_EQUAL(int_power(2, 8), 256);
        TEST_EQUAL(int_power(2, 9), 512);
        TEST_EQUAL(int_power(2, 10), 1024);

        TEST_EQUAL(int_power(-2, 0), 1);
        TEST_EQUAL(int_power(-2, 1), -2);
        TEST_EQUAL(int_power(-2, 2), 4);
        TEST_EQUAL(int_power(-2, 3), -8);
        TEST_EQUAL(int_power(-2, 4), 16);
        TEST_EQUAL(int_power(-2, 5), -32);
        TEST_EQUAL(int_power(-2, 6), 64);
        TEST_EQUAL(int_power(-2, 7), -128);
        TEST_EQUAL(int_power(-2, 8), 256);
        TEST_EQUAL(int_power(-2, 9), -512);
        TEST_EQUAL(int_power(-2, 10), 1024);

        TEST_EQUAL(int_power(10ll, 0ll), 1ll);
        TEST_EQUAL(int_power(10ll, 1ll), 10ll);
        TEST_EQUAL(int_power(10ll, 2ll), 100ll);
        TEST_EQUAL(int_power(10ll, 3ll), 1000ll);
        TEST_EQUAL(int_power(10ll, 4ll), 10000ll);
        TEST_EQUAL(int_power(10ll, 5ll), 100000ll);
        TEST_EQUAL(int_power(10ll, 6ll), 1000000ll);
        TEST_EQUAL(int_power(10ll, 7ll), 10000000ll);
        TEST_EQUAL(int_power(10ll, 8ll), 100000000ll);
        TEST_EQUAL(int_power(10ll, 9ll), 1000000000ll);
        TEST_EQUAL(int_power(10ll, 10ll), 10000000000ll);

        TEST_EQUAL(int_power(-10ll, 0ll), 1ll);
        TEST_EQUAL(int_power(-10ll, 1ll), -10ll);
        TEST_EQUAL(int_power(-10ll, 2ll), 100ll);
        TEST_EQUAL(int_power(-10ll, 3ll), -1000ll);
        TEST_EQUAL(int_power(-10ll, 4ll), 10000ll);
        TEST_EQUAL(int_power(-10ll, 5ll), -100000ll);
        TEST_EQUAL(int_power(-10ll, 6ll), 1000000ll);
        TEST_EQUAL(int_power(-10ll, 7ll), -10000000ll);
        TEST_EQUAL(int_power(-10ll, 8ll), 100000000ll);
        TEST_EQUAL(int_power(-10ll, 9ll), -1000000000ll);
        TEST_EQUAL(int_power(-10ll, 10ll), 10000000000ll);

        TEST_EQUAL(int_power(10ull, 0ull), 1ull);
        TEST_EQUAL(int_power(10ull, 1ull), 10ull);
        TEST_EQUAL(int_power(10ull, 2ull), 100ull);
        TEST_EQUAL(int_power(10ull, 3ull), 1000ull);
        TEST_EQUAL(int_power(10ull, 4ull), 10000ull);
        TEST_EQUAL(int_power(10ull, 5ull), 100000ull);
        TEST_EQUAL(int_power(10ull, 6ull), 1000000ull);
        TEST_EQUAL(int_power(10ull, 7ull), 10000000ull);
        TEST_EQUAL(int_power(10ull, 8ull), 100000000ull);
        TEST_EQUAL(int_power(10ull, 9ull), 1000000000ull);
        TEST_EQUAL(int_power(10ull, 10ull), 10000000000ull);

        TEST_EQUAL(int_sqrt(0), 0);
        TEST_EQUAL(int_sqrt(1), 1);
        TEST_EQUAL(int_sqrt(2), 1);
        TEST_EQUAL(int_sqrt(3), 1);
        TEST_EQUAL(int_sqrt(4), 2);
        TEST_EQUAL(int_sqrt(5), 2);
        TEST_EQUAL(int_sqrt(6), 2);
        TEST_EQUAL(int_sqrt(7), 2);
        TEST_EQUAL(int_sqrt(8), 2);
        TEST_EQUAL(int_sqrt(9), 3);
        TEST_EQUAL(int_sqrt(10), 3);
        TEST_EQUAL(int_sqrt(11), 3);
        TEST_EQUAL(int_sqrt(12), 3);
        TEST_EQUAL(int_sqrt(13), 3);
        TEST_EQUAL(int_sqrt(14), 3);
        TEST_EQUAL(int_sqrt(15), 3);
        TEST_EQUAL(int_sqrt(16), 4);
        TEST_EQUAL(int_sqrt(17), 4);
        TEST_EQUAL(int_sqrt(18), 4);
        TEST_EQUAL(int_sqrt(19), 4);
        TEST_EQUAL(int_sqrt(20), 4);
        TEST_EQUAL(int_sqrt(21), 4);
        TEST_EQUAL(int_sqrt(22), 4);
        TEST_EQUAL(int_sqrt(23), 4);
        TEST_EQUAL(int_sqrt(24), 4);
        TEST_EQUAL(int_sqrt(25), 5);

        TEST_EQUAL(int_sqrt(123u), 11u);
        TEST_EQUAL(int_sqrt(123), 11);
        TEST_EQUAL(int_sqrt(1234u), 35u);
        TEST_EQUAL(int_sqrt(1234), 35);
        TEST_EQUAL(int_sqrt(12345u), 111u);
        TEST_EQUAL(int_sqrt(12345), 111);
        TEST_EQUAL(int_sqrt(123456ul), 351ul);
        TEST_EQUAL(int_sqrt(123456l), 351l);
        TEST_EQUAL(int_sqrt(1234567ul), 1111ul);
        TEST_EQUAL(int_sqrt(1234567l), 1111l);
        TEST_EQUAL(int_sqrt(12345678ul), 3513ul);
        TEST_EQUAL(int_sqrt(12345678l), 3513l);
        TEST_EQUAL(int_sqrt(123456789ul), 11111ul);
        TEST_EQUAL(int_sqrt(123456789l), 11111l);
        TEST_EQUAL(int_sqrt(1234567890ul), 35136ul);
        TEST_EQUAL(int_sqrt(1234567890l), 35136l);
        TEST_EQUAL(int_sqrt(12345678901ull), 111111ull);
        TEST_EQUAL(int_sqrt(12345678901ll), 111111ll);
        TEST_EQUAL(int_sqrt(123456789012ull), 351364ull);
        TEST_EQUAL(int_sqrt(123456789012ll), 351364ll);
        TEST_EQUAL(int_sqrt(1234567890123ull), 1111111ull);
        TEST_EQUAL(int_sqrt(1234567890123ll), 1111111ll);
        TEST_EQUAL(int_sqrt(12345678901234ull), 3513641ull);
        TEST_EQUAL(int_sqrt(12345678901234ll), 3513641ll);
        TEST_EQUAL(int_sqrt(123456789012345ull), 11111111ull);
        TEST_EQUAL(int_sqrt(123456789012345ll), 11111111ll);
        TEST_EQUAL(int_sqrt(1234567890123456ull), 35136418ull);
        TEST_EQUAL(int_sqrt(1234567890123456ll), 35136418ll);
        TEST_EQUAL(int_sqrt(12345678901234567ull), 111111110ull);
        TEST_EQUAL(int_sqrt(12345678901234567ll), 111111110ll);
        TEST_EQUAL(int_sqrt(123456789012345678ull), 351364182ull);
        TEST_EQUAL(int_sqrt(123456789012345678ll), 351364182ll);
        TEST_EQUAL(int_sqrt(1234567890123456789ull), 1111111106ull);
        TEST_EQUAL(int_sqrt(1234567890123456789ll), 1111111106ll);
        TEST_EQUAL(int_sqrt(12345678901234567890ull), 3513641828ull);

    }

    void check_bitwise_operations() {

        TEST_EQUAL(ibits(0), 0);
        TEST_EQUAL(ibits(1), 1);
        TEST_EQUAL(ibits(2), 1);
        TEST_EQUAL(ibits(3), 2);
        TEST_EQUAL(ibits(4), 1);
        TEST_EQUAL(ibits(5), 2);
        TEST_EQUAL(ibits(6), 2);
        TEST_EQUAL(ibits(7), 3);
        TEST_EQUAL(ibits(8), 1);
        TEST_EQUAL(ibits(9), 2);
        TEST_EQUAL(ibits(10), 2);
        TEST_EQUAL(ibits(0x7fff), 15);
        TEST_EQUAL(ibits(0x8000), 1);
        TEST_EQUAL(ibits(0x8888), 4);
        TEST_EQUAL(ibits(0xffff), 16);
        TEST_EQUAL(ibits(0x7fffffffffffffffull), 63);
        TEST_EQUAL(ibits(0x8000000000000000ull), 1);
        TEST_EQUAL(ibits(0xffffffffffffffffull), 64);

        TEST_EQUAL(ifloor2(0), 0);
        TEST_EQUAL(ifloor2(1), 1);
        TEST_EQUAL(ifloor2(2), 2);
        TEST_EQUAL(ifloor2(3), 2);
        TEST_EQUAL(ifloor2(4), 4);
        TEST_EQUAL(ifloor2(5), 4);
        TEST_EQUAL(ifloor2(6), 4);
        TEST_EQUAL(ifloor2(7), 4);
        TEST_EQUAL(ifloor2(8), 8);
        TEST_EQUAL(ifloor2(9), 8);
        TEST_EQUAL(ifloor2(10), 8);
        TEST_EQUAL(ifloor2(0x7ffful), 0x4000ul);
        TEST_EQUAL(ifloor2(0x8000ul), 0x8000ul);
        TEST_EQUAL(ifloor2(0x8001ul), 0x8000ul);
        TEST_EQUAL(ifloor2(0xfffful), 0x8000ul);

        TEST_EQUAL(iceil2(0), 0);
        TEST_EQUAL(iceil2(1), 1);
        TEST_EQUAL(iceil2(2), 2);
        TEST_EQUAL(iceil2(3), 4);
        TEST_EQUAL(iceil2(4), 4);
        TEST_EQUAL(iceil2(5), 8);
        TEST_EQUAL(iceil2(6), 8);
        TEST_EQUAL(iceil2(7), 8);
        TEST_EQUAL(iceil2(8), 8);
        TEST_EQUAL(iceil2(9), 16);
        TEST_EQUAL(iceil2(10), 16);
        TEST_EQUAL(iceil2(0x7ffful), 0x8000ul);
        TEST_EQUAL(iceil2(0x8000ul), 0x8000ul);
        TEST_EQUAL(iceil2(0x8001ul), 0x10000ul);
        TEST_EQUAL(iceil2(0xfffful), 0x10000ul);

        TEST(! ispow2(0));
        TEST(ispow2(1));
        TEST(ispow2(2));
        TEST(! ispow2(3));
        TEST(ispow2(4));
        TEST(! ispow2(5));
        TEST(! ispow2(6));
        TEST(! ispow2(7));
        TEST(ispow2(8));
        TEST(! ispow2(9));
        TEST(! ispow2(10));
        TEST(! ispow2(0x7fff));
        TEST(ispow2(0x8000));
        TEST(! ispow2(0x8001));
        TEST(! ispow2(0xffff));
        TEST(! ispow2(0x7fffffffffffffffull));
        TEST(ispow2(0x8000000000000000ull));
        TEST(! ispow2(0x8000000000000001ull));
        TEST(! ispow2(0xffffffffffffffffull));

        TEST_EQUAL(ilog2p1(0), 0);
        TEST_EQUAL(ilog2p1(1), 1);
        TEST_EQUAL(ilog2p1(2), 2);
        TEST_EQUAL(ilog2p1(3), 2);
        TEST_EQUAL(ilog2p1(4), 3);
        TEST_EQUAL(ilog2p1(5), 3);
        TEST_EQUAL(ilog2p1(6), 3);
        TEST_EQUAL(ilog2p1(7), 3);
        TEST_EQUAL(ilog2p1(8), 4);
        TEST_EQUAL(ilog2p1(9), 4);
        TEST_EQUAL(ilog2p1(10), 4);
        TEST_EQUAL(ilog2p1(0x7fff), 15);
        TEST_EQUAL(ilog2p1(0x8000), 16);
        TEST_EQUAL(ilog2p1(0x8888), 16);
        TEST_EQUAL(ilog2p1(0xffff), 16);
        TEST_EQUAL(ilog2p1(0x7fffffffffffffffull), 63);
        TEST_EQUAL(ilog2p1(0x8000000000000000ull), 64);
        TEST_EQUAL(ilog2p1(0xffffffffffffffffull), 64);

        TEST_EQUAL(letter_to_mask('A'), 1);
        TEST_EQUAL(letter_to_mask('B'), 2);
        TEST_EQUAL(letter_to_mask('C'), 4);
        TEST_EQUAL(letter_to_mask('Z'), 0x2000000ull);
        TEST_EQUAL(letter_to_mask('a'), 0x4000000ull);
        TEST_EQUAL(letter_to_mask('b'), 0x8000000ull);
        TEST_EQUAL(letter_to_mask('c'), 0x10000000ull);
        TEST_EQUAL(letter_to_mask('z'), 0x8000000000000ull);
        TEST_EQUAL(letter_to_mask('~'), 0);

        uint16_t u = 0x1234;

        TEST_EQUAL(rotl(u, 0), 0x1234);    TEST_EQUAL(rotr(u, 0), 0x1234);
        TEST_EQUAL(rotl(u, 4), 0x2341);    TEST_EQUAL(rotr(u, 4), 0x4123);
        TEST_EQUAL(rotl(u, 8), 0x3412);    TEST_EQUAL(rotr(u, 8), 0x3412);
        TEST_EQUAL(rotl(u, 12), 0x4123);   TEST_EQUAL(rotr(u, 12), 0x2341);
        TEST_EQUAL(rotl(u, 16), 0x1234);   TEST_EQUAL(rotr(u, 16), 0x1234);
        TEST_EQUAL(rotl(u, 20), 0x2341);   TEST_EQUAL(rotr(u, 20), 0x4123);
        TEST_EQUAL(rotl(u, 24), 0x3412);   TEST_EQUAL(rotr(u, 24), 0x3412);
        TEST_EQUAL(rotl(u, 28), 0x4123);   TEST_EQUAL(rotr(u, 28), 0x2341);
        TEST_EQUAL(rotl(u, 32), 0x1234);   TEST_EQUAL(rotr(u, 32), 0x1234);
        TEST_EQUAL(rotl(u, -4), 0x4123);   TEST_EQUAL(rotr(u, -4), 0x2341);
        TEST_EQUAL(rotl(u, -8), 0x3412);   TEST_EQUAL(rotr(u, -8), 0x3412);
        TEST_EQUAL(rotl(u, -12), 0x2341);  TEST_EQUAL(rotr(u, -12), 0x4123);
        TEST_EQUAL(rotl(u, -16), 0x1234);  TEST_EQUAL(rotr(u, -16), 0x1234);
        TEST_EQUAL(rotl(u, -20), 0x4123);  TEST_EQUAL(rotr(u, -20), 0x2341);
        TEST_EQUAL(rotl(u, -24), 0x3412);  TEST_EQUAL(rotr(u, -24), 0x3412);
        TEST_EQUAL(rotl(u, -28), 0x2341);  TEST_EQUAL(rotr(u, -28), 0x4123);
        TEST_EQUAL(rotl(u, -32), 0x1234);  TEST_EQUAL(rotr(u, -32), 0x1234);

        uint32_t v = 0x12345678;

        TEST_EQUAL(rotl(v, 0), 0x12345678);    TEST_EQUAL(rotr(v, 0), 0x12345678);
        TEST_EQUAL(rotl(v, 4), 0x23456781);    TEST_EQUAL(rotr(v, 4), 0x81234567);
        TEST_EQUAL(rotl(v, 8), 0x34567812);    TEST_EQUAL(rotr(v, 8), 0x78123456);
        TEST_EQUAL(rotl(v, 12), 0x45678123);   TEST_EQUAL(rotr(v, 12), 0x67812345);
        TEST_EQUAL(rotl(v, 16), 0x56781234);   TEST_EQUAL(rotr(v, 16), 0x56781234);
        TEST_EQUAL(rotl(v, 20), 0x67812345);   TEST_EQUAL(rotr(v, 20), 0x45678123);
        TEST_EQUAL(rotl(v, 24), 0x78123456);   TEST_EQUAL(rotr(v, 24), 0x34567812);
        TEST_EQUAL(rotl(v, 28), 0x81234567);   TEST_EQUAL(rotr(v, 28), 0x23456781);
        TEST_EQUAL(rotl(v, 32), 0x12345678);   TEST_EQUAL(rotr(v, 32), 0x12345678);
        TEST_EQUAL(rotl(v, 36), 0x23456781);   TEST_EQUAL(rotr(v, 36), 0x81234567);
        TEST_EQUAL(rotl(v, 40), 0x34567812);   TEST_EQUAL(rotr(v, 40), 0x78123456);
        TEST_EQUAL(rotl(v, 44), 0x45678123);   TEST_EQUAL(rotr(v, 44), 0x67812345);
        TEST_EQUAL(rotl(v, 48), 0x56781234);   TEST_EQUAL(rotr(v, 48), 0x56781234);
        TEST_EQUAL(rotl(v, 52), 0x67812345);   TEST_EQUAL(rotr(v, 52), 0x45678123);
        TEST_EQUAL(rotl(v, 56), 0x78123456);   TEST_EQUAL(rotr(v, 56), 0x34567812);
        TEST_EQUAL(rotl(v, 60), 0x81234567);   TEST_EQUAL(rotr(v, 60), 0x23456781);
        TEST_EQUAL(rotl(v, 64), 0x12345678);   TEST_EQUAL(rotr(v, 64), 0x12345678);
        TEST_EQUAL(rotl(v, -4), 0x81234567);   TEST_EQUAL(rotr(v, -4), 0x23456781);
        TEST_EQUAL(rotl(v, -8), 0x78123456);   TEST_EQUAL(rotr(v, -8), 0x34567812);
        TEST_EQUAL(rotl(v, -12), 0x67812345);  TEST_EQUAL(rotr(v, -12), 0x45678123);
        TEST_EQUAL(rotl(v, -16), 0x56781234);  TEST_EQUAL(rotr(v, -16), 0x56781234);
        TEST_EQUAL(rotl(v, -20), 0x45678123);  TEST_EQUAL(rotr(v, -20), 0x67812345);
        TEST_EQUAL(rotl(v, -24), 0x34567812);  TEST_EQUAL(rotr(v, -24), 0x78123456);
        TEST_EQUAL(rotl(v, -28), 0x23456781);  TEST_EQUAL(rotr(v, -28), 0x81234567);
        TEST_EQUAL(rotl(v, -32), 0x12345678);  TEST_EQUAL(rotr(v, -32), 0x12345678);
        TEST_EQUAL(rotl(v, -36), 0x81234567);  TEST_EQUAL(rotr(v, -36), 0x23456781);
        TEST_EQUAL(rotl(v, -40), 0x78123456);  TEST_EQUAL(rotr(v, -40), 0x34567812);
        TEST_EQUAL(rotl(v, -44), 0x67812345);  TEST_EQUAL(rotr(v, -44), 0x45678123);
        TEST_EQUAL(rotl(v, -48), 0x56781234);  TEST_EQUAL(rotr(v, -48), 0x56781234);
        TEST_EQUAL(rotl(v, -52), 0x45678123);  TEST_EQUAL(rotr(v, -52), 0x67812345);
        TEST_EQUAL(rotl(v, -56), 0x34567812);  TEST_EQUAL(rotr(v, -56), 0x78123456);
        TEST_EQUAL(rotl(v, -60), 0x23456781);  TEST_EQUAL(rotr(v, -60), 0x81234567);
        TEST_EQUAL(rotl(v, -64), 0x12345678);  TEST_EQUAL(rotr(v, -64), 0x12345678);

    }

    U8string fa0() { return "hello"; }
    U8string fa1(char c) { return {c}; }
    U8string fa2(size_t n, char c) { return U8string(n, c); }

    U8string (*fb0)() = &fa0;
    U8string (*fb1)(char c) = &fa1;
    U8string (*fb2)(size_t n, char c) = &fa2;

    struct FC0 { U8string operator()() { return "hello"; } };
    struct FC1 { U8string operator()(char c) { return {c}; } };
    struct FC2 { U8string operator()(size_t n, char c) { return U8string(n, c); } };

    struct FD0 { U8string operator()() const { return "hello"; } };
    struct FD1 { U8string operator()(char c) const { return {c}; } };
    struct FD2 { U8string operator()(size_t n, char c) const { return U8string(n, c); } };

    FC0 fc0;
    FC1 fc1;
    FC2 fc2;

    const FD0 fd0 = {};
    const FD1 fd1 = {};
    const FD2 fd2 = {};

    auto fe0 = [] () -> U8string { return "hello"; };
    auto fe1 = [] (char c) -> U8string { return {c}; };
    auto fe2 = [] (size_t n, char c) -> U8string { return U8string(n, c); };

    using tuple0 = std::tuple<>;
    using tuple1 = std::tuple<char>;
    using tuple2 = std::tuple<size_t, char>;

    void check_function_traits() {

        TEST_EQUAL(fa0(), "hello");  TEST_EQUAL(fa1('a'), "a");  TEST_EQUAL(fa2(5, 'z'), "zzzzz");
        TEST_EQUAL(fb0(), "hello");  TEST_EQUAL(fb1('a'), "a");  TEST_EQUAL(fb2(5, 'z'), "zzzzz");
        TEST_EQUAL(fc0(), "hello");  TEST_EQUAL(fc1('a'), "a");  TEST_EQUAL(fc2(5, 'z'), "zzzzz");
        TEST_EQUAL(fd0(), "hello");  TEST_EQUAL(fd1('a'), "a");  TEST_EQUAL(fd2(5, 'z'), "zzzzz");
        TEST_EQUAL(fe0(), "hello");  TEST_EQUAL(fe1('a'), "a");  TEST_EQUAL(fe2(5, 'z'), "zzzzz");

        using FTA0 = decltype(fa0);  using FTA1 = decltype(fa1);  using FTA2 = decltype(fa2);
        using FTB0 = decltype(fb0);  using FTB1 = decltype(fb1);  using FTB2 = decltype(fb2);
        using FTC0 = decltype(fc0);  using FTC1 = decltype(fc1);  using FTC2 = decltype(fc2);
        using FTD0 = decltype(fd0);  using FTD1 = decltype(fd1);  using FTD2 = decltype(fd2);
        using FTE0 = decltype(fe0);  using FTE1 = decltype(fe1);  using FTE2 = decltype(fe2);

        TEST_EQUAL(Arity<FTA0>::value, 0);  TEST_EQUAL(Arity<FTA1>::value, 1);  TEST_EQUAL(Arity<FTA2>::value, 2);
        TEST_EQUAL(Arity<FTB0>::value, 0);  TEST_EQUAL(Arity<FTB1>::value, 1);  TEST_EQUAL(Arity<FTB2>::value, 2);
        TEST_EQUAL(Arity<FTC0>::value, 0);  TEST_EQUAL(Arity<FTC1>::value, 1);  TEST_EQUAL(Arity<FTC2>::value, 2);
        TEST_EQUAL(Arity<FTD0>::value, 0);  TEST_EQUAL(Arity<FTD1>::value, 1);  TEST_EQUAL(Arity<FTD2>::value, 2);
        TEST_EQUAL(Arity<FTE0>::value, 0);  TEST_EQUAL(Arity<FTE1>::value, 1);  TEST_EQUAL(Arity<FTE2>::value, 2);

        TEST_TYPE(ArgumentTuple<FTA0>, tuple0);  TEST_TYPE(ArgumentTuple<FTA1>, tuple1);  TEST_TYPE(ArgumentTuple<FTA2>, tuple2);
        TEST_TYPE(ArgumentTuple<FTB0>, tuple0);  TEST_TYPE(ArgumentTuple<FTB1>, tuple1);  TEST_TYPE(ArgumentTuple<FTB2>, tuple2);
        TEST_TYPE(ArgumentTuple<FTC0>, tuple0);  TEST_TYPE(ArgumentTuple<FTC1>, tuple1);  TEST_TYPE(ArgumentTuple<FTC2>, tuple2);
        TEST_TYPE(ArgumentTuple<FTD0>, tuple0);  TEST_TYPE(ArgumentTuple<FTD1>, tuple1);  TEST_TYPE(ArgumentTuple<FTD2>, tuple2);
        TEST_TYPE(ArgumentTuple<FTE0>, tuple0);  TEST_TYPE(ArgumentTuple<FTE1>, tuple1);  TEST_TYPE(ArgumentTuple<FTE2>, tuple2);

        { using T = ArgumentType<FTA1, 0>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTA2, 0>; TEST_TYPE(T, size_t); }
        { using T = ArgumentType<FTA2, 1>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTB1, 0>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTB2, 0>; TEST_TYPE(T, size_t); }
        { using T = ArgumentType<FTB2, 1>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTC1, 0>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTC2, 0>; TEST_TYPE(T, size_t); }
        { using T = ArgumentType<FTC2, 1>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTD1, 0>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTD2, 0>; TEST_TYPE(T, size_t); }
        { using T = ArgumentType<FTD2, 1>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTE1, 0>; TEST_TYPE(T, char); }
        { using T = ArgumentType<FTE2, 0>; TEST_TYPE(T, size_t); }
        { using T = ArgumentType<FTE2, 1>; TEST_TYPE(T, char); }

        TEST_TYPE(ReturnType<FTA0>, U8string);  TEST_TYPE(ReturnType<FTA1>, U8string);  TEST_TYPE(ReturnType<FTA2>, U8string);
        TEST_TYPE(ReturnType<FTB0>, U8string);  TEST_TYPE(ReturnType<FTB1>, U8string);  TEST_TYPE(ReturnType<FTB2>, U8string);
        TEST_TYPE(ReturnType<FTC0>, U8string);  TEST_TYPE(ReturnType<FTC1>, U8string);  TEST_TYPE(ReturnType<FTC2>, U8string);
        TEST_TYPE(ReturnType<FTD0>, U8string);  TEST_TYPE(ReturnType<FTD1>, U8string);  TEST_TYPE(ReturnType<FTD2>, U8string);
        TEST_TYPE(ReturnType<FTE0>, U8string);  TEST_TYPE(ReturnType<FTE1>, U8string);  TEST_TYPE(ReturnType<FTE2>, U8string);

    }

    void check_function_operations() {

        auto t0 = tuple0{};
        auto t1 = tuple1{'a'};
        auto t2 = tuple2{5, 'z'};

        TEST_EQUAL(tuple_invoke(fa0, t0), "hello");  TEST_EQUAL(tuple_invoke(fa1, t1), "a");  TEST_EQUAL(tuple_invoke(fa2, t2), "zzzzz");
        TEST_EQUAL(tuple_invoke(fb0, t0), "hello");  TEST_EQUAL(tuple_invoke(fb1, t1), "a");  TEST_EQUAL(tuple_invoke(fb2, t2), "zzzzz");
        TEST_EQUAL(tuple_invoke(fc0, t0), "hello");  TEST_EQUAL(tuple_invoke(fc1, t1), "a");  TEST_EQUAL(tuple_invoke(fc2, t2), "zzzzz");
        TEST_EQUAL(tuple_invoke(fd0, t0), "hello");  TEST_EQUAL(tuple_invoke(fd1, t1), "a");  TEST_EQUAL(tuple_invoke(fd2, t2), "zzzzz");
        TEST_EQUAL(tuple_invoke(fe0, t0), "hello");  TEST_EQUAL(tuple_invoke(fe1, t1), "a");  TEST_EQUAL(tuple_invoke(fe2, t2), "zzzzz");

        auto lf1 = [] (int x) { return x * x; };
        auto sf1 = stdfun(lf1);
        TEST_TYPE_OF(sf1, std::function<int(int)>);
        TEST_EQUAL(sf1(5), 25);

        int z = 0;
        auto lf2 = [&] (int x, int y) { z = x * y; };
        auto sf2 = stdfun(lf2);
        TEST_TYPE_OF(sf2, std::function<void(int, int)>);
        TRY(sf2(6, 7));
        TEST_EQUAL(z, 42);

    }

    class Counted {
    public:
        Counted() { ++num; }
        Counted(const Counted&) { ++num; }
        Counted(Counted&&) = default;
        ~Counted() { --num; }
        Counted& operator=(const Counted&) = default;
        Counted& operator=(Counted&&) = default;
        static int count() { return num; }
        static void reset() { num = 0; }
    private:
        static int num;
    };

    int Counted::num = 0;

    void check_generic_function_objects() {

        Counted::reset();
        TEST_EQUAL(Counted::count(), 0);

        Counted* cp = nullptr;
        std::shared_ptr<Counted> csp;
        TRY(csp.reset(new Counted));
        TEST_EQUAL(Counted::count(), 1);
        TRY(csp.reset());
        TEST_EQUAL(Counted::count(), 0);
        TRY(csp.reset(new Counted, do_nothing));
        TEST_EQUAL(Counted::count(), 1);
        cp = csp.get();
        TRY(csp.reset());
        TEST_EQUAL(Counted::count(), 1);
        delete cp;
        TEST_EQUAL(Counted::count(), 0);

        int n1 = 42, n2 = 0;
        U8string s1 = "Hello world", s2;
        U8string& sr(s1);
        const U8string& csr(s1);

        TRY(n2 = identity(n1));              TEST_EQUAL(n2, 42);
        TRY(n2 = identity(100));             TEST_EQUAL(n2, 100);
        TRY(s2 = identity(s1));              TEST_EQUAL(s2, "Hello world");
        TRY(s2 = identity(sr));              TEST_EQUAL(s2, "Hello world");
        TRY(s2 = identity(csr));             TEST_EQUAL(s2, "Hello world");
        TRY(s2 = identity("Goodbye"));       TEST_EQUAL(s2, "Goodbye");
        TRY(s2 = identity("Hello again"s));  TEST_EQUAL(s2, "Hello again");

    }

    void check_hash_functions() {

        uint32_t u = 0;
        size_t h1 = 0, h2 = 0, h3 = 0, h4 = 0, h5 = 0;
        std::string s0, s1 = "Hello world";

        TRY(u = Djb2a());                      TEST_EQUAL(u, 0x00001505);
        TRY(u = Djb2a()(s0));                  TEST_EQUAL(u, 0x00001505);
        TRY(u = Djb2a()(s1));                  TEST_EQUAL(u, 0x33c13465);
        TRY(u = djb2a(s0.data(), s0.size()));  TEST_EQUAL(u, 0x00001505);
        TRY(u = djb2a(s1.data(), s1.size()));  TEST_EQUAL(u, 0x33c13465);

        TRY(hash_combine(h1, 42));
        TRY(hash_combine(h2, 42, s1));
        TRY(hash_combine(h3, 86, 99, s1));
        TEST_COMPARE(h1, !=, 0);
        TEST_COMPARE(h2, !=, 0);
        TEST_COMPARE(h3, !=, 0);
        TEST_COMPARE(h1, !=, h2);
        TEST_COMPARE(h1, !=, h3);
        TEST_COMPARE(h2, !=, h3);

        Strings sv {"hello", "world", "goodbye"};

        for (auto& s: sv)
            TRY(hash_combine(h4, s));
        TRY(h5 = hash_range(sv));
        TEST_EQUAL(h4, h5);

    }

    void check_scope_guards() {

        int n;
        std::string s;

        {
            n = 1;
            Resource<int> r;
            TRY((r = {2, [&] (int i) { n = i; }}));
            TEST(bool(r));
            TEST_EQUAL(r.get(), 2);
            TEST_EQUAL(n, 1);
        }
        TEST_EQUAL(n, 2);

        {
            n = 1;
            Resource<int> r;
            TRY((r = {2, [&] (int i) { n = i; }}));
            TEST(bool(r));
            TEST_EQUAL(r.get(), 2);
            TEST_EQUAL(n, 1);
            TRY(r = {});
            TEST_EQUAL(n, 2);
        }
        TEST_EQUAL(n, 2);

        {
            n = 1;
            Resource<int> r;
            TRY((r = {2, [&] (int i) { n = i; }}));
            TEST(bool(r));
            TEST_EQUAL(r.get(), 2);
            TEST_EQUAL(n, 1);
            TRY(r.release());
            TEST_EQUAL(n, 1);
        }
        TEST_EQUAL(n, 1);

        {
            n = 1;
            Resource<int> r;
            TRY((r = {2, [&] (int i) { n = i; }}));
            TEST(bool(r));
            TEST_EQUAL(r.get(), 2);
            TEST_EQUAL(n, 1);
            TRY(r.get() = 3);
            TEST_EQUAL(r.get(), 3);
            TEST_EQUAL(n, 1);
        }
        TEST_EQUAL(n, 3);

        {
            n = 1;
            Resource<int, -1> r;
            TEST_EQUAL(r.get(), -1);
            TEST(! r);
            TRY((r = {2, [&] (int i) { n = i; }}));
            TEST_EQUAL(r.get(), 2);
            TEST(r);
            TEST_EQUAL(n, 1);
            TRY(r.release());
            TEST_EQUAL(r.get(), -1);
            TEST(! r);
            TEST_EQUAL(n, 1);
        }
        TEST_EQUAL(n, 1);

        {
            Resource<int*> r;
            TEST(! r);
            TRY(r = &n);
            TEST(r);
            TEST_EQUAL(r.get(), &n);
        }

        {
            Resource<void*> r;
            TEST(! r);
            TRY(r = &n);
            TEST(r);
            TEST_EQUAL(r.get(), &n);
        }

        {
            n = 1;
            ScopeExit x([&] { n = 2; });
            TEST_EQUAL(n, 1);
        }
        TEST_EQUAL(n, 2);

        try {
            n = 1;
            ScopeExit x([&] { n = 2; });
            TEST_EQUAL(n, 1);
            throw std::runtime_error("fail");
        }
        catch (...) {}
        TEST_EQUAL(n, 2);

        {
            n = 1;
            ScopeSuccess x([&] { n = 2; });
            TEST_EQUAL(n, 1);
        }
        TEST_EQUAL(n, 2);

        try {
            n = 1;
            ScopeSuccess x([&] { n = 2; });
            TEST_EQUAL(n, 1);
            throw std::runtime_error("fail");
        }
        catch (...) {}
        TEST_EQUAL(n, 1);

        {
            n = 1;
            ScopeFailure x([&] { n = 2; });
            TEST_EQUAL(n, 1);
        }
        TEST_EQUAL(n, 1);

        try {
            n = 1;
            ScopeFailure x([&] { n = 2; });
            TEST_EQUAL(n, 1);
            throw std::runtime_error("fail");
        }
        catch (...) {}
        TEST_EQUAL(n, 2);

        {
            ScopeExit x(nullptr);
        }

        try {
            ScopeExit x(nullptr);
            throw std::runtime_error("fail");
        }
        catch (...) {}

        {
            ScopeSuccess x(nullptr);
        }

        try {
            ScopeSuccess x(nullptr);
            throw std::runtime_error("fail");
        }
        catch (...) {}

        {
            ScopeFailure x(nullptr);
        }

        try {
            ScopeFailure x(nullptr);
            throw std::runtime_error("fail");
        }
        catch (...) {}

        s = "hello";
        {
            SizeGuard g(s);
            s += " world";
            TEST_EQUAL(s, "hello world");
        }
        TEST_EQUAL(s, "hello world");

        s = "hello";
        try {
            SizeGuard g(s);
            s += " world";
            TEST_EQUAL(s, "hello world");
            throw std::runtime_error("fail");
        }
        catch (...) {}
        TEST_EQUAL(s, "hello");

        s = "hello";
        {
            SizeGuard g(s);
            s += " world";
            TEST_EQUAL(s, "hello world");
            TRY(g.release());
        }
        TEST_EQUAL(s, "hello world");

        s = "hello";
        try {
            SizeGuard g(s);
            s += " world";
            TEST_EQUAL(s, "hello world");
            TRY(g.release());
            throw std::runtime_error("fail");
        }
        catch (...) {}
        TEST_EQUAL(s, "hello world");

        s = "hello";
        {
            ValueGuard g(s);
            s = "goodbye";
            TEST_EQUAL(s, "goodbye");
        }
        TEST_EQUAL(s, "goodbye");

        s = "hello";
        try {
            ValueGuard g(s);
            s = "goodbye";
            TEST_EQUAL(s, "goodbye");
            throw std::runtime_error("fail");
        }
        catch (...) {}
        TEST_EQUAL(s, "hello");

        s = "hello";
        {
            ValueGuard g(s);
            s = "goodbye";
            TEST_EQUAL(s, "goodbye");
            TRY(g.release());
        }
        TEST_EQUAL(s, "goodbye");

        s = "hello";
        try {
            ValueGuard g(s);
            s = "goodbye";
            TEST_EQUAL(s, "goodbye");
            TRY(g.release());
            throw std::runtime_error("fail");
        }
        catch (...) {}
        TEST_EQUAL(s, "goodbye");

        s.clear();
        {
            ScopedTransaction t;
            TRY(t.call([&] { s += 'a'; }, [&] { s += 'z'; }));
            TRY(t.call([&] { s += 'b'; }, [&] { s += 'y'; }));
            TRY(t.call([&] { s += 'c'; }, [&] { s += 'x'; }));
            TEST_EQUAL(s, "abc");
        }
        TEST_EQUAL(s, "abcxyz");

        s.clear();
        {
            ScopedTransaction t;
            TRY(t.call([&] { s += 'a'; }, [&] { s += 'z'; }));
            TRY(t.call([&] { s += 'b'; }, [&] { s += 'y'; }));
            TRY(t.call([&] { s += 'c'; }, [&] { s += 'x'; }));
            TEST_EQUAL(s, "abc");
            TRY(t.rollback());
            TEST_EQUAL(s, "abcxyz");
        }
        TEST_EQUAL(s, "abcxyz");

        s.clear();
        {
            ScopedTransaction t;
            TRY(t.call([&] { s += 'a'; }, [&] { s += 'z'; }));
            TRY(t.call([&] { s += 'b'; }, [&] { s += 'y'; }));
            TRY(t.call([&] { s += 'c'; }, [&] { s += 'x'; }));
            TEST_EQUAL(s, "abc");
            TRY(t.commit());
            TEST_EQUAL(s, "abc");
            TRY(t.rollback());
            TEST_EQUAL(s, "abc");
        }
        TEST_EQUAL(s, "abc");

    }

}

TEST_MODULE(core, common) {

    check_preprocessor_macros();
    check_integer_types();
    check_string_types();
    check_exceptions();
    check_metaprogramming_and_type_traits();
    check_type_adapters();
    check_type_related_functions();
    check_version_number();
    check_arithmetic_literals();
    check_generic_algorithms();
    check_integer_sequences();
    check_memory_algorithms();
    check_range_traits();
    check_range_types();
    check_generic_arithmetic_functions();
    check_integer_arithmetic_functions();
    check_bitwise_operations();
    check_function_traits();
    check_function_operations();
    check_generic_function_objects();
    check_hash_functions();
    check_scope_guards();

}
