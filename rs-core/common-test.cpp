#include "rs-core/common.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <set>
#include <sstream>
#include <string>
#include <thread>
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
using namespace std::chrono;
using namespace std::literals;

// These tests are a bit incestuous since the unit test module itself uses
// some of the facilities in the core module. The shared functions are mostly
// simple enough that it should be clear where the error lies if anything goes
// wrong, and in any case the alternative of duplicating large parts of the
// core module in the unit test module would not be an improvement.

namespace {

    template <typename T>
    Ustring make_str(const T& t) {
        std::ostringstream out;
        out << t;
        return out.str();
    }

    #define MAKE_STR_FOR_CONTAINER(Con) \
        template <typename T> \
        Ustring make_str(const Con<T>& c) { \
            if (c.empty()) \
                return "[]"; \
            std::ostringstream out; \
            out << '['; \
            for (auto& t: c) \
                out << t << ','; \
            Ustring s = out.str(); \
            s.back() = ']'; \
            return s; \
        }

    MAKE_STR_FOR_CONTAINER(std::set)
    MAKE_STR_FOR_CONTAINER(std::vector)

    Ustring f1(int n) { return Ustring(size_t(n), '*'); }
    Ustring f1(Ustring s) { return '[' + s + ']'; }
    Ustring f2() { return "Hello"; }
    Ustring f2(Ustring s) { return '[' + s + ']'; }
    Ustring f2(int x, int y) { return dec(x * y); }

    enum class ZapEnum: uint32_t {
        alpha = 1,
        bravo = 2,
        charlie = 4,
    };

    RS_BITMASK_OPERATORS(ZapEnum);

    struct Adapt {
        int a = 10;
        AutoMove<int> b;
        AutoMove<int, 20> c;
        AutoMove<Ustring> d;
        NoTransfer<int> e;
        NoTransfer<int, 30> f;
        NoTransfer<Ustring> g;
    };

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

    Ustring fa0() { return "hello"; }
    Ustring fa1(char c) { return {c}; }
    Ustring fa2(size_t n, char c) { return Ustring(n, c); }

    Ustring (*fb0)() = &fa0;
    Ustring (*fb1)(char c) = &fa1;
    Ustring (*fb2)(size_t n, char c) = &fa2;

    struct FC0 { Ustring operator()() { return "hello"; } };
    struct FC1 { Ustring operator()(char c) { return {c}; } };
    struct FC2 { Ustring operator()(size_t n, char c) { return Ustring(n, c); } };

    struct FD0 { Ustring operator()() const { return "hello"; } };
    struct FD1 { Ustring operator()(char c) const { return {c}; } };
    struct FD2 { Ustring operator()(size_t n, char c) const { return Ustring(n, c); } };

    FC0 fc0;
    FC1 fc1;
    FC2 fc2;

    const FD0 fd0 = {};
    const FD1 fd1 = {};
    const FD2 fd2 = {};

    auto fe0 = [] () -> Ustring { return "hello"; };
    auto fe1 = [] (char c) -> Ustring { return {c}; };
    auto fe2 = [] (size_t n, char c) -> Ustring { return Ustring(n, c); };

    using tuple0 = std::tuple<>;
    using tuple1 = std::tuple<char>;
    using tuple2 = std::tuple<size_t, char>;

}

void test_core_common_accountable() {

    using A = Accountable<uint64_t>;
    using B = Accountable<uint64_t, false>;
    using C = Accountable<void>;
    using D = Accountable<void, false>;

    TEST_EQUAL(A::count(), 0);
    TEST_EQUAL(B::count(), 0);
    TEST_EQUAL(C::count(), 0);
    TEST_EQUAL(D::count(), 0);

    A a1, a2;
    B b1, b2;
    C c1, c2;
    D d1, d2;

    TEST_EQUAL(A::count(), 2);
    TEST_EQUAL(B::count(), 2);
    TEST_EQUAL(C::count(), 2);
    TEST_EQUAL(D::count(), 2);

    TEST_EQUAL(a1.get(), 0);
    TEST_EQUAL(a2.get(), 0);
    TEST_EQUAL(b1.get(), 0);
    TEST_EQUAL(b2.get(), 0);

    TRY(a1 = 42);
    TRY(b1 = 42);

    TEST_EQUAL(a1.get(), 42);
    TEST_EQUAL(a2.get(), 0);
    TEST_EQUAL(b1.get(), 42);
    TEST_EQUAL(b2.get(), 0);

    TRY(a2 = a1);
    TRY(c2 = c1);

    TEST_EQUAL(a1.get(), 42);
    TEST_EQUAL(a2.get(), 42);
    TEST_EQUAL(b1.get(), 42);
    TEST_EQUAL(b2.get(), 0);

    TRY(a2 = std::move(a1));
    TRY(b2 = std::move(b1));
    TRY(c2 = std::move(c1));
    TRY(d2 = std::move(d1));

    TEST_EQUAL(a1.get(), 0);
    TEST_EQUAL(a2.get(), 42);
    TEST_EQUAL(b1.get(), 0);
    TEST_EQUAL(b2.get(), 42);

    TEST_EQUAL(A::count(), 2);
    TEST_EQUAL(B::count(), 2);
    TEST_EQUAL(C::count(), 2);
    TEST_EQUAL(D::count(), 2);

    TRY(A::reset());
    TRY(B::reset());
    TRY(C::reset());
    TRY(D::reset());

    TEST_EQUAL(A::count(), 0);
    TEST_EQUAL(B::count(), 0);
    TEST_EQUAL(C::count(), 0);
    TEST_EQUAL(D::count(), 0);

}

void test_core_common_preprocessor_macros() {

    std::vector<int> iv = {1, 2, 3, 4, 5}, iv2 = {2, 3, 5, 7, 11};
    Strings result, sv = {"Neddie", "Eccles", "Bluebottle"};
    ZapEnum x = {}, y = {}, z = {};

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

    TEST_TYPE(std::underlying_type_t<ZapEnum>, uint32_t);
    TEST_EQUAL(sizeof(ZapEnum), 4);

    x = ZapEnum::alpha;
    y = ZapEnum::bravo;
    z = ZapEnum(0);

    TEST(x);    TEST(bool(x));
    TEST(y);    TEST(bool(y));
    TEST(! z);  TEST(! bool(z));

    TRY(z = ~ x);             TEST_EQUAL(uint32_t(z), 0xfffffffe);
    TRY(z = ~ y);             TEST_EQUAL(uint32_t(z), 0xfffffffd);
    TRY(z = ~ ZapEnum(0));    TEST_EQUAL(uint32_t(z), 0xffffffff);
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

void test_core_common_integer_types() {

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
        auto u8 = uint8_t(i);
        auto u16 = uint16_t(i);
        auto u32 = uint32_t(i);
        auto u64 = uint64_t(i);
        auto i8 = int8_t(i);
        auto i16 = int16_t(i);
        auto i32 = int32_t(i);
        auto i64 = int64_t(i);
        TRY(h1 = std::hash<uint8_t>()(u8));
        TRY(h2 = std::hash<BigEndian<uint8_t>>()(BigEndian<uint8_t>(u8)));
        TRY(h3 = std::hash<LittleEndian<uint8_t>>()(LittleEndian<uint8_t>(u8)));
        TEST_EQUAL(h1, h2);
        TEST_EQUAL(h1, h3);
        TRY(h1 = std::hash<uint16_t>()(u16));
        TRY(h2 = std::hash<BigEndian<uint16_t>>()(BigEndian<uint16_t>(u16)));
        TRY(h3 = std::hash<LittleEndian<uint16_t>>()(LittleEndian<uint16_t>(u16)));
        TEST_EQUAL(h1, h2);
        TEST_EQUAL(h1, h3);
        TRY(h1 = std::hash<uint32_t>()(u32));
        TRY(h2 = std::hash<BigEndian<uint32_t>>()(BigEndian<uint32_t>(u32)));
        TRY(h3 = std::hash<LittleEndian<uint32_t>>()(LittleEndian<uint32_t>(u32)));
        TEST_EQUAL(h1, h2);
        TEST_EQUAL(h1, h3);
        TRY(h1 = std::hash<uint64_t>()(u64));
        TRY(h2 = std::hash<BigEndian<uint64_t>>()(BigEndian<uint64_t>(u64)));
        TRY(h3 = std::hash<LittleEndian<uint64_t>>()(LittleEndian<uint64_t>(u64)));
        TEST_EQUAL(h1, h2);
        TEST_EQUAL(h1, h3);
        TRY(h1 = std::hash<int8_t>()(i8));
        TRY(h2 = std::hash<BigEndian<int8_t>>()(BigEndian<int8_t>(i8)));
        TRY(h3 = std::hash<LittleEndian<int8_t>>()(LittleEndian<int8_t>(i8)));
        TEST_EQUAL(h1, h2);
        TEST_EQUAL(h1, h3);
        TRY(h1 = std::hash<int16_t>()(i16));
        TRY(h2 = std::hash<BigEndian<int16_t>>()(BigEndian<int16_t>(i16)));
        TRY(h3 = std::hash<LittleEndian<int16_t>>()(LittleEndian<int16_t>(i16)));
        TEST_EQUAL(h1, h2);
        TEST_EQUAL(h1, h3);
        TRY(h1 = std::hash<int32_t>()(i32));
        TRY(h2 = std::hash<BigEndian<int32_t>>()(BigEndian<int32_t>(i32)));
        TRY(h3 = std::hash<LittleEndian<int32_t>>()(LittleEndian<int32_t>(i32)));
        TEST_EQUAL(h1, h2);
        TEST_EQUAL(h1, h3);
        TRY(h1 = std::hash<int64_t>()(i64));
        TRY(h2 = std::hash<BigEndian<int64_t>>()(BigEndian<int64_t>(i64)));
        TRY(h3 = std::hash<LittleEndian<int64_t>>()(LittleEndian<int64_t>(i64)));
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

void test_core_common_string_types() {

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

void test_core_common_metaprogramming() {

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

void test_core_common_smart_pointers() {

    using A = Accountable<int>;

    class C:
    public Accountable<long> {
    public:
        C() = default;
        C(int i): Accountable<long>(i) {}
        C* clone() const { return new C(*this); }
    };

    {

        A::reset();
        CopyPtr<A> a;
        TEST(! a);
        TEST_EQUAL(A::count(), 0);

        CopyPtr<A> b(new A(42));
        REQUIRE(b);
        TEST_EQUAL(b->get(), 42);
        TEST_EQUAL(A::count(), 1);

        CopyPtr<A> c(std::move(b));
        TEST(! b);
        REQUIRE(c);
        TEST_EQUAL(c->get(), 42);
        TEST_EQUAL(A::count(), 1);

        CopyPtr<A> d(c);
        REQUIRE(c);
        REQUIRE(d);
        TEST_EQUAL(c->get(), 42);
        TEST_EQUAL(d->get(), 42);
        TEST_EQUAL(A::count(), 2);

        TRY(a = std::move(b));
        TEST(! a);
        TEST(! b);
        TEST_EQUAL(A::count(), 2);

        TRY(a = std::move(c));
        REQUIRE(a);
        TEST(! c);
        TEST_EQUAL(a->get(), 42);
        TEST_EQUAL(A::count(), 2);

        TRY(b = a);
        REQUIRE(a);
        REQUIRE(b);
        TEST_EQUAL(a->get(), 42);
        TEST_EQUAL(b->get(), 42);
        TEST_EQUAL(A::count(), 3);

        TRY(a.reset());
        TEST(! a);
        TEST_EQUAL(A::count(), 2);

        TRY(b = nullptr);
        TEST(! b);
        TEST_EQUAL(A::count(), 1);

    }

    TEST_EQUAL(A::count(), 0);

    CopyPtr<std::string> p;
    TRY(p = make_copy_ptr<std::string>(5, 'a'));
    REQUIRE(p);
    TEST_EQUAL(*p, "aaaaa");

    {

        C::reset();
        ClonePtr<C> a;
        TEST(! a);
        TEST_EQUAL(C::count(), 0);

        ClonePtr<C> b(new C(42));
        REQUIRE(b);
        TEST_EQUAL(b->get(), 42);
        TEST_EQUAL(C::count(), 1);

        ClonePtr<C> c(std::move(b));
        TEST(! b);
        REQUIRE(c);
        TEST_EQUAL(c->get(), 42);
        TEST_EQUAL(C::count(), 1);

        ClonePtr<C> d(c);
        REQUIRE(c);
        REQUIRE(d);
        TEST_EQUAL(c->get(), 42);
        TEST_EQUAL(d->get(), 42);
        TEST_EQUAL(C::count(), 2);

        TRY(a = std::move(b));
        TEST(! a);
        TEST(! b);
        TEST_EQUAL(C::count(), 2);

        TRY(a = std::move(c));
        REQUIRE(a);
        TEST(! c);
        TEST_EQUAL(a->get(), 42);
        TEST_EQUAL(C::count(), 2);

        TRY(b = a);
        REQUIRE(a);
        REQUIRE(b);
        TEST_EQUAL(a->get(), 42);
        TEST_EQUAL(b->get(), 42);
        TEST_EQUAL(C::count(), 3);

        TRY(a.reset());
        TEST(! a);
        TEST_EQUAL(C::count(), 2);

        TRY(b = nullptr);
        TEST(! b);
        TEST_EQUAL(C::count(), 1);

        TRY(a = make_clone_ptr<C>(86));
        REQUIRE(a);
        TEST_EQUAL(a->get(), 86);
        TEST_EQUAL(C::count(), 2);

    }

    TEST_EQUAL(C::count(), 0);

    ClonePtr<C> q;
    TRY(q = make_clone_ptr<C>(99));
    REQUIRE(q);
    TEST_EQUAL(q->get(), 99);

}

void test_core_common_type_adapters() {

    Adapt x, y;

    TEST_EQUAL(x.a, 10);
    TEST_EQUAL(x.b.value, 0);
    TEST_EQUAL(x.c.value, 20);
    TEST_EQUAL(x.d.value, "");
    TEST_EQUAL(x.e.value, 0);
    TEST_EQUAL(x.f.value, 30);
    TEST_EQUAL(x.g.value, "");

    TRY((y = {100, 200, 300, "Hello"s, 400, 500, "Goodbye"s}));

    TEST_EQUAL(y.a, 100);
    TEST_EQUAL(y.b.value, 200);
    TEST_EQUAL(y.c.value, 300);
    TEST_EQUAL(y.d.value, "Hello");
    TEST_EQUAL(y.e.value, 0);
    TEST_EQUAL(y.f.value, 30);
    TEST_EQUAL(y.g.value, "");

    TRY(y.e.value = 400);
    TRY(y.f.value = 500);
    TRY(y.g.value = "Goodbye");
    TEST_EQUAL(y.e.value, 400);
    TEST_EQUAL(y.f.value, 500);
    TEST_EQUAL(y.g.value, "Goodbye");

    TRY(x = std::move(y));

    TEST_EQUAL(x.a, 100);
    TEST_EQUAL(x.b.value, 200);
    TEST_EQUAL(x.c.value, 300);
    TEST_EQUAL(x.d.value, "Hello");
    TEST_EQUAL(x.e.value, 0);
    TEST_EQUAL(x.f.value, 30);
    TEST_EQUAL(x.g.value, "");

    TEST_EQUAL(y.a, 100);
    TEST_EQUAL(y.b.value, 0);
    TEST_EQUAL(y.c.value, 20);
    TEST_EQUAL(y.d.value, "");
    TEST_EQUAL(y.e.value, 400);
    TEST_EQUAL(y.f.value, 500);
    TEST_EQUAL(y.g.value, "Goodbye");

}

void test_core_common_type_related_functions() {

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

void test_core_common_arithmetic_literals() {

    #ifdef __GNUC__
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Woverflow"
    #endif

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
    TEST_EQUAL(dec(pt), "0");
    TEST_EQUAL(hex(pt, 1), "0");
    TRY(pt = 12345_pt);
    TEST_EQUAL(pt, 12345_pt);
    TEST_EQUAL(dec(pt), "12345");
    TRY(pt = 0x0_pt);
    TEST_EQUAL(dec(pt), "0");
    TRY(pt = 0x12345_pt);
    TEST_EQUAL(hex(pt, 1), "12345");
    TEST_EQUAL(dec(pt), "74565");

    TRY(sz = 0_sz);
    TEST_EQUAL(sz, 0_sz);
    TEST_EQUAL(dec(sz), "0");
    TEST_EQUAL(hex(sz, 1), "0");
    TRY(sz = 12345_sz);
    TEST_EQUAL(sz, 12345_sz);
    TEST_EQUAL(dec(sz), "12345");
    TRY(sz = 0x0_sz);
    TEST_EQUAL(dec(sz), "0");
    TRY(sz = 0x12345_sz);
    TEST_EQUAL(hex(sz, 1), "12345");
    TEST_EQUAL(dec(sz), "74565");

    if constexpr (sizeof(size_t) == 8) {

        TRY(pt = 0_pt);
        TEST_EQUAL(hex(pt), "0000000000000000");
        TRY(pt = 9'223'372'036'854'775'807_pt); // 2^63-1
        TEST_EQUAL(pt, 9'223'372'036'854'775'807_pt);
        TEST_EQUAL(dec(pt), "9223372036854775807");
        TRY(pt = -9'223'372'036'854'775'807_pt); // -(2^63-1)
        TEST_EQUAL(pt, -9'223'372'036'854'775'807_pt);
        TEST_EQUAL(dec(pt), "-9223372036854775807");
        TRY(pt = -9'223'372'036'854'775'808_pt); // -2^63
        TEST_EQUAL(pt, -9'223'372'036'854'775'808_pt);
        TEST_EQUAL(dec(pt), "-9223372036854775808");
        TRY(pt = 0x7fffffffffffffff_pt);
        TEST_EQUAL(hex(pt), "7fffffffffffffff");
        TEST_EQUAL(dec(pt), "9223372036854775807");

        TRY(sz = 0_sz);
        TEST_EQUAL(hex(sz), "0000000000000000");
        TRY(sz = 18'446'744'073'709'551'615_sz); // 2^64-1
        TEST_EQUAL(sz, 18'446'744'073'709'551'615_sz);
        TEST_EQUAL(dec(sz), "18446744073709551615");
        TRY(sz = 0xffffffffffffffff_sz);
        TEST_EQUAL(hex(sz), "ffffffffffffffff");
        TEST_EQUAL(dec(sz), "18446744073709551615");

    } else {

        TRY(pt = 0_pt);
        TEST_EQUAL(hex(pt), "00000000");
        TRY(pt = 2'147'483'647_pt); // 2^31-1
        TEST_EQUAL(pt, 2'147'483'647_pt);
        TEST_EQUAL(dec(pt), "2147483647");
        TRY(pt = -2'147'483'647_pt); // -(2^31-1)
        TEST_EQUAL(pt, -2'147'483'647_pt);
        TEST_EQUAL(dec(pt), "-2147483647");
        TRY(pt = -2'147'483'648_pt); // -2^31
        TEST_EQUAL(pt, -2'147'483'648_pt);
        TEST_EQUAL(dec(pt), "-2147483648");
        TRY(pt = 0x7fffffff_pt);
        TEST_EQUAL(hex(pt), "7fffffff");
        TEST_EQUAL(dec(pt), "2147483647");

        TRY(sz = 0_sz);
        TEST_EQUAL(hex(sz), "00000000");
        TRY(sz = 4'294'967'295_sz); // 2^32-1
        TEST_EQUAL(sz, 4'294'967'295_sz);
        TEST_EQUAL(dec(sz), "4294967295");
        TRY(sz = 0xffffffff_sz);
        TEST_EQUAL(hex(sz), "ffffffff");
        TEST_EQUAL(dec(sz), "4294967295");

    }

    #ifdef __GNUC__
        #pragma GCC diagnostic pop
    #endif

}

void test_core_common_generic_algorithms() {

    Ustring s;
    Ustring::const_iterator it;
    std::vector<int> v1, v2, v3, v4;
    int n = 0;
    char c = 0;
    char array[] {'H','e','l','l','o'};
    const char* cp = array;

    auto ar = array_range(array, sizeof(array));
    TEST_EQUAL(ar.size(), 5);
    TRY(std::copy(ar.begin(), ar.end(), overwrite(s)));
    TEST_EQUAL(s, "Hello");

    auto car = array_range(cp, sizeof(array));
    TEST_EQUAL(car.size(), 5);
    TRY(std::copy(car.begin(), car.end(), overwrite(s)));
    TEST_EQUAL(s, "Hello");

    int a[5] = {1,2,3,4,5};
    std::array<int, 5> b = {{6,7,8,9,10}};

    auto t = array_to_tuple(a);
    auto u = array_to_tuple(b);

    TEST_EQUAL(std::get<0>(t), 1);
    TEST_EQUAL(std::get<1>(t), 2);
    TEST_EQUAL(std::get<2>(t), 3);
    TEST_EQUAL(std::get<3>(t), 4);
    TEST_EQUAL(std::get<4>(t), 5);
    TEST_EQUAL(std::get<0>(u), 6);
    TEST_EQUAL(std::get<1>(u), 7);
    TEST_EQUAL(std::get<2>(u), 8);
    TEST_EQUAL(std::get<3>(u), 9);
    TEST_EQUAL(std::get<4>(u), 10);

    s = "Hello";

    TRY(c = at_index(s, 0));          TEST_EQUAL(c, 'H');
    TRY(c = at_index(s, 4));          TEST_EQUAL(c, 'o');
    TRY(c = at_index(s, 5));          TEST_EQUAL(c, '\0');
    TRY(c = at_index(s, npos));       TEST_EQUAL(c, '\0');
    TRY(c = at_index(s, 0, '*'));     TEST_EQUAL(c, 'H');
    TRY(c = at_index(s, 4, '*'));     TEST_EQUAL(c, 'o');
    TRY(c = at_index(s, 5, '*'));     TEST_EQUAL(c, '*');
    TRY(c = at_index(s, npos, '*'));  TEST_EQUAL(c, '*');

    const auto is_alpha = [] (char c) { return isalpha(c); };
    const auto same_case = [] (char a, char b) { return islower(a) == islower(b) && isupper(a) == isupper(b); };

    s = "";                 TRY(con_remove(s, 'c'));                        TEST_EQUAL(s, "");
    s = "xyzxyzxyz";        TRY(con_remove(s, 'c'));                        TEST_EQUAL(s, "xyzxyzxyz");
    s = "abcabcabc";        TRY(con_remove(s, 'c'));                        TEST_EQUAL(s, "ababab");
    s = "";                 TRY(con_remove_if(s, is_alpha));                TEST_EQUAL(s, "");
    s = "1234567890";       TRY(con_remove_if(s, is_alpha));                TEST_EQUAL(s, "1234567890");
    s = "abc123abc123";     TRY(con_remove_if(s, is_alpha));                TEST_EQUAL(s, "123123");
    s = "";                 TRY(con_remove_if_not(s, is_alpha));            TEST_EQUAL(s, "");
    s = "abcdefghijklm";    TRY(con_remove_if_not(s, is_alpha));            TEST_EQUAL(s, "abcdefghijklm");
    s = "abc123abc123";     TRY(con_remove_if_not(s, is_alpha));            TEST_EQUAL(s, "abcabc");
    s = "";                 TRY(con_unique(s));                             TEST_EQUAL(s, "");
    s = "abcdeabcde";       TRY(con_unique(s));                             TEST_EQUAL(s, "abcdeabcde");
    s = "abbcccddddeeeee";  TRY(con_unique(s));                             TEST_EQUAL(s, "abcde");
    s = "";                 TRY(con_unique(s, same_case));                  TEST_EQUAL(s, "");
    s = "AbCaBcAbCaBc";     TRY(con_unique(s, same_case));                  TEST_EQUAL(s, "AbCaBcAbCaBc");
    s = "ABCabcABCabc";     TRY(con_unique(s, same_case));                  TEST_EQUAL(s, "AaAa");
    s = "";                 TRY(con_sort_unique(s));                        TEST_EQUAL(s, "");
    s = "zyxwvutsrqpon";    TRY(con_sort_unique(s));                        TEST_EQUAL(s, "nopqrstuvwxyz");
    s = "abcdeabcdabcaba";  TRY(con_sort_unique(s));                        TEST_EQUAL(s, "abcde");
    s = "";                 TRY(con_sort_unique(s, std::greater<char>()));  TEST_EQUAL(s, "");
    s = "nopqrstuvwxyz";    TRY(con_sort_unique(s, std::greater<char>()));  TEST_EQUAL(s, "zyxwvutsrqpon");
    s = "abcdeabcdabcaba";  TRY(con_sort_unique(s, std::greater<char>()));  TEST_EQUAL(s, "edcba");
    s = "";                 TRY(con_trim(s, 'a'));                          TEST_EQUAL(s, "");
    s = "bbbaaaccc";        TRY(con_trim(s, 'a'));                          TEST_EQUAL(s, "bbbaaaccc");
    s = "aaabbbaaacccaaa";  TRY(con_trim(s, 'a'));                          TEST_EQUAL(s, "bbbaaaccc");
    s = "";                 TRY(con_trim_left(s, 'a'));                     TEST_EQUAL(s, "");
    s = "cccbbbaaa";        TRY(con_trim_left(s, 'a'));                     TEST_EQUAL(s, "cccbbbaaa");
    s = "aaabbbaaacccaaa";  TRY(con_trim_left(s, 'a'));                     TEST_EQUAL(s, "bbbaaacccaaa");
    s = "";                 TRY(con_trim_right(s, 'a'));                    TEST_EQUAL(s, "");
    s = "aaabbbccc";        TRY(con_trim_right(s, 'a'));                    TEST_EQUAL(s, "aaabbbccc");
    s = "aaabbbaaacccaaa";  TRY(con_trim_right(s, 'a'));                    TEST_EQUAL(s, "aaabbbaaaccc");
    s = "";                 TRY(con_trim_if(s, is_alpha));                  TEST_EQUAL(s, "");
    s = "123abc123";        TRY(con_trim_if(s, is_alpha));                  TEST_EQUAL(s, "123abc123");
    s = "abc123abc123abc";  TRY(con_trim_if(s, is_alpha));                  TEST_EQUAL(s, "123abc123");
    s = "";                 TRY(con_trim_left_if(s, is_alpha));             TEST_EQUAL(s, "");
    s = "123abc123abc";     TRY(con_trim_left_if(s, is_alpha));             TEST_EQUAL(s, "123abc123abc");
    s = "abc123abc123abc";  TRY(con_trim_left_if(s, is_alpha));             TEST_EQUAL(s, "123abc123abc");
    s = "";                 TRY(con_trim_right_if(s, is_alpha));            TEST_EQUAL(s, "");
    s = "abc123abc123";     TRY(con_trim_right_if(s, is_alpha));            TEST_EQUAL(s, "abc123abc123");
    s = "abc123abc123abc";  TRY(con_trim_right_if(s, is_alpha));            TEST_EQUAL(s, "abc123abc123");

    v2 = {1, 2, 3};
    v3 = {4, 5, 6};
    v4 = {7, 8, 9};

    TRY(v1 = concatenate(v2));          TEST_EQUAL(make_str(v1), "[1,2,3]");
    TRY(v1 = concatenate(v2, v3));      TEST_EQUAL(make_str(v1), "[1,2,3,4,5,6]");
    TRY(v1 = concatenate(v2, v3, v4));  TEST_EQUAL(make_str(v1), "[1,2,3,4,5,6,7,8,9]");

    auto f1 = [&] { s += "Hello"; };
    auto f2 = [&] (size_t i) { s += dec(i) + ";"; };

    s.clear();  TRY(do_n(0, f1));   TEST_EQUAL(s, "");
    s.clear();  TRY(do_n(1, f1));   TEST_EQUAL(s, "Hello");
    s.clear();  TRY(do_n(2, f1));   TEST_EQUAL(s, "HelloHello");
    s.clear();  TRY(do_n(3, f1));   TEST_EQUAL(s, "HelloHelloHello");
    s.clear();  TRY(for_n(0, f2));  TEST_EQUAL(s, "");
    s.clear();  TRY(for_n(1, f2));  TEST_EQUAL(s, "0;");
    s.clear();  TRY(for_n(2, f2));  TEST_EQUAL(s, "0;1;");
    s.clear();  TRY(for_n(3, f2));  TEST_EQUAL(s, "0;1;2;");

    std::map<int, Ustring> m = {
        {1, "alpha"},
        {2, "bravo"},
        {3, "charlie"},
    };

    TRY(s = find_in_map(m, 1));          TEST_EQUAL(s, "alpha");
    TRY(s = find_in_map(m, 2));          TEST_EQUAL(s, "bravo");
    TRY(s = find_in_map(m, 3));          TEST_EQUAL(s, "charlie");
    TRY(s = find_in_map(m, 4));          TEST_EQUAL(s, "");
    TRY(s = find_in_map(m, 1, "none"));  TEST_EQUAL(s, "alpha");
    TRY(s = find_in_map(m, 2, "none"));  TEST_EQUAL(s, "bravo");
    TRY(s = find_in_map(m, 3, "none"));  TEST_EQUAL(s, "charlie");
    TRY(s = find_in_map(m, 4, "none"));  TEST_EQUAL(s, "none");

    s = "Hello world";

    TRY(it = find_last(s.begin(), s.end(), 'o'));                   TEST_EQUAL(it - s.begin(), 7);
    TRY(it = find_last(s.begin(), s.end(), 'z'));                   TEST_EQUAL(it - s.begin(), 11);
    TRY(it = find_last_not(s.begin(), s.end(), 'd'));               TEST_EQUAL(it - s.begin(), 9);
    TRY(it = find_last_not(s.begin(), s.end(), 'z'));               TEST_EQUAL(it - s.begin(), 10);
    TRY(it = find_last_if(s.begin(), s.end(), ascii_isupper));      TEST_EQUAL(it - s.begin(), 0);
    TRY(it = find_last_if(s.begin(), s.end(), ascii_islower));      TEST_EQUAL(it - s.begin(), 10);
    TRY(it = find_last_if(s.begin(), s.end(), ascii_isdigit));      TEST_EQUAL(it - s.begin(), 11);
    TRY(it = find_last_if_not(s.begin(), s.end(), ascii_islower));  TEST_EQUAL(it - s.begin(), 5);
    TRY(it = find_last_if_not(s.begin(), s.end(), ascii_isupper));  TEST_EQUAL(it - s.begin(), 10);
    TRY(it = find_last_if_not(s.begin(), s.end(), ascii_isprint));  TEST_EQUAL(it - s.begin(), 11);
    TRY(it = find_not(s.begin(), s.end(), 'H'));                    TEST_EQUAL(it - s.begin(), 1);
    TRY(it = find_not(s.begin(), s.end(), 'z'));                    TEST_EQUAL(it - s.begin(), 0);

    std::set<int> is1, is2;
    TEST(! sets_intersect(is1, is2));
    is1 = {1,2,3,4,5};
    TEST(! sets_intersect(is1, is2));
    is2 = {3,6,9};
    TEST(sets_intersect(is1, is2));
    is2 = {10,20};
    TEST(! sets_intersect(is1, is2));

    s = "edcba";  TRY(sort_list());                              TEST_EQUAL(s, "edcba");
    s = "edcba";  TRY(sort_list(s[0]));                          TEST_EQUAL(s, "edcba");
    s = "edcba";  TRY(sort_list(s[0], s[1]));                    TEST_EQUAL(s, "decba");
    s = "edcba";  TRY(sort_list(s[0], s[1], s[2]));              TEST_EQUAL(s, "cdeba");
    s = "edcba";  TRY(sort_list(s[0], s[1], s[2], s[3]));        TEST_EQUAL(s, "bcdea");
    s = "edcba";  TRY(sort_list(s[0], s[1], s[2], s[3], s[4]));  TEST_EQUAL(s, "abcde");

    s = "abcde";  TRY(sort_list_by(std::greater<>()));                                TEST_EQUAL(s, "abcde");
    s = "abcde";  TRY(sort_list_by(std::greater<>(), s[0]));                          TEST_EQUAL(s, "abcde");
    s = "abcde";  TRY(sort_list_by(std::greater<>(), s[0], s[1]));                    TEST_EQUAL(s, "bacde");
    s = "abcde";  TRY(sort_list_by(std::greater<>(), s[0], s[1], s[2]));              TEST_EQUAL(s, "cbade");
    s = "abcde";  TRY(sort_list_by(std::greater<>(), s[0], s[1], s[2], s[3]));        TEST_EQUAL(s, "dcbae");
    s = "abcde";  TRY(sort_list_by(std::greater<>(), s[0], s[1], s[2], s[3], s[4]));  TEST_EQUAL(s, "edcba");

    std::vector<int> iv0, iv5 = {1,2,3,4,5};
    const std::vector<int>& civ5(iv5);
    double d = 0;
    double da5[] = {10,20,30,40,50};

    TRY(n = sum_of(iv0));       TEST_EQUAL(n, 0);
    TRY(n = sum_of(iv5));       TEST_EQUAL(n, 15);
    TRY(n = sum_of(civ5));      TEST_EQUAL(n, 15);
    TRY(n = product_of(iv0));   TEST_EQUAL(n, 1);
    TRY(n = product_of(iv5));   TEST_EQUAL(n, 120);
    TRY(n = product_of(civ5));  TEST_EQUAL(n, 120);
    TRY(d = sum_of(da5));       TEST_EQUAL(d, 150);
    TRY(d = product_of(da5));   TEST_EQUAL(d, 1.2e7);

}

void test_core_common_integer_sequences() {

    std::vector<int> v;
    Ustring s;

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

}

void test_core_common_memory_algorithms() {

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

void test_core_common_generic_arithmetic() {

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

void test_core_common_integer_arithmetic() {

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

void test_core_common_function_traits() {

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

    TEST_TYPE(ReturnType<FTA0>, Ustring);  TEST_TYPE(ReturnType<FTA1>, Ustring);  TEST_TYPE(ReturnType<FTA2>, Ustring);
    TEST_TYPE(ReturnType<FTB0>, Ustring);  TEST_TYPE(ReturnType<FTB1>, Ustring);  TEST_TYPE(ReturnType<FTB2>, Ustring);
    TEST_TYPE(ReturnType<FTC0>, Ustring);  TEST_TYPE(ReturnType<FTC1>, Ustring);  TEST_TYPE(ReturnType<FTC2>, Ustring);
    TEST_TYPE(ReturnType<FTD0>, Ustring);  TEST_TYPE(ReturnType<FTD1>, Ustring);  TEST_TYPE(ReturnType<FTD2>, Ustring);
    TEST_TYPE(ReturnType<FTE0>, Ustring);  TEST_TYPE(ReturnType<FTE1>, Ustring);  TEST_TYPE(ReturnType<FTE2>, Ustring);

}

void test_core_common_function_operations() {

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

void test_core_common_generic_function_objects() {

    CallRef<void()> cr1;
    CallRef<int(int, int)> cr2;
    Ustring s;
    int n = 0;

    TEST(! cr1);
    TEST(! cr2);
    TEST_THROW(cr1(), std::bad_function_call);
    TEST_THROW(cr2(2, 3), std::bad_function_call);
    TRY(cr1 = [&] { s = "Hello"; });
    TRY(cr2 = [] (int x, int y) { return x + y; });
    TEST(cr1);
    TEST(cr2);
    TRY(cr1());
    TEST_EQUAL(s, "Hello");
    TRY(n = cr2(2, 3));
    TEST_EQUAL(n, 5);

    int n1 = 42, n2 = 0;
    Ustring s1 = "Hello world", s2;
    Ustring& sr(s1);
    const Ustring& csr(s1);

    TRY(n2 = identity(n1));              TEST_EQUAL(n2, 42);
    TRY(n2 = identity(100));             TEST_EQUAL(n2, 100);
    TRY(s2 = identity(s1));              TEST_EQUAL(s2, "Hello world");
    TRY(s2 = identity(sr));              TEST_EQUAL(s2, "Hello world");
    TRY(s2 = identity(csr));             TEST_EQUAL(s2, "Hello world");
    TRY(s2 = identity("Goodbye"));       TEST_EQUAL(s2, "Goodbye");
    TRY(s2 = identity("Hello again"s));  TEST_EQUAL(s2, "Hello again");

    int a[] = {1,2,3};
    int b[] = {1,2,3};
    int c[] = {1,2,4};

    TEST(range_equal(a, b));
    TEST(! range_equal(a, c));
    TEST(! range_compare(a, b));
    TEST(range_compare(a, c));
    TEST(! range_compare(c, a));

}

void test_core_common_hash_functions() {

    size_t h1 = 0, h2 = 0, h3 = 0, h4 = 0, h5 = 0;
    std::string s0, s1 = "Hello world";

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

    std::hash<int> ih;
    std::hash<std::string> sh;

    TRY(h1 = ih(42));          TRY(h2 = std_hash(42));          TEST_EQUAL(h1, h2);
    TRY(h1 = ih(86));          TRY(h2 = std_hash(86));          TEST_EQUAL(h1, h2);
    TRY(h1 = ih(99));          TRY(h2 = std_hash(99));          TEST_EQUAL(h1, h2);
    TRY(h1 = sh("hello"s));    TRY(h2 = std_hash("hello"s));    TEST_EQUAL(h1, h2);
    TRY(h1 = sh("world"s));    TRY(h2 = std_hash("world"s));    TEST_EQUAL(h1, h2);
    TRY(h1 = sh("goodbye"s));  TRY(h2 = std_hash("goodbye"s));  TEST_EQUAL(h1, h2);

}

void test_core_common_scope_guards() {

    int n = 0;
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
        TRY(r.reset(&n));
        TEST(r);
        TEST_EQUAL(r.get(), &n);
    }

    {
        Resource<void*> r;
        TEST(! r);
        TRY(r.reset(&n));
        TEST(r);
        TEST_EQUAL(r.get(), &n);
    }

    s.clear();
    {
        ScopedTransaction t;
        TRY(t([&] { s += 'a'; }, [&] { s += 'z'; }));
        TRY(t([&] { s += 'b'; }, [&] { s += 'y'; }));
        TRY(t([&] { s += 'c'; }, [&] { s += 'x'; }));
        TEST_EQUAL(s, "abc");
    }
    TEST_EQUAL(s, "abcxyz");

    s.clear();
    {
        ScopedTransaction t;
        TRY(t([&] { s += 'a'; }, [&] { s += 'z'; }));
        TRY(t([&] { s += 'b'; }, [&] { s += 'y'; }));
        TRY(t([&] { s += 'c'; }, [&] { s += 'x'; }));
        TEST_EQUAL(s, "abc");
        TRY(t.rollback());
        TEST_EQUAL(s, "abcxyz");
    }
    TEST_EQUAL(s, "abcxyz");

    s.clear();
    {
        ScopedTransaction t;
        TRY(t([&] { s += 'a'; }, [&] { s += 'z'; }));
        TRY(t([&] { s += 'b'; }, [&] { s += 'y'; }));
        TRY(t([&] { s += 'c'; }, [&] { s += 'x'; }));
        TEST_EQUAL(s, "abc");
        TRY(t.commit());
        TEST_EQUAL(s, "abc");
        TRY(t.rollback());
        TEST_EQUAL(s, "abc");
    }
    TEST_EQUAL(s, "abc");

}

void test_core_common_logging() {

    TRY(logx("Log test:", 1, 2, 3, 4, 5));
    TRY(logx("Log test:", 6, 7, 8, 9, 10));

}

namespace {

    std::mutex wit_mtx;

    struct WriteInThread {
        Ustring src;
        Ustring* dst = nullptr;
        void operator()() {
            std::this_thread::sleep_for(100ms);
            auto lock = make_lock(wit_mtx);
            if (dst)
                *dst += src;
        }
    };

}

void test_core_common_thread_class() {

    Thread t1, t2;
    Ustring s;

    TEST(! t1.joinable());
    TRY(t1 = Thread(WriteInThread{"Hello", &s}));
    TEST(t1.joinable());
    TRY(t1.join());
    TEST(! t1.joinable());
    TEST_EQUAL(s, "Hello");

    s.clear();
    TRY(t1 = Thread(WriteInThread{"Hello", &s}));
    TEST(t1.joinable());
    TRY(t1 = {});
    TEST(! t1.joinable());
    TEST_EQUAL(s, "Hello");

    s.clear();
    TRY(t1 = Thread(WriteInThread{"Hello", &s}));
    TRY(t2 = Thread(WriteInThread{"Goodbye", &s}));
    TEST(t1.joinable());
    TEST(t2.joinable());
    TRY(t1.swap(t2));
    TEST(t1.joinable());
    TEST(t2.joinable());
    TRY(t1.join());
    TRY(t2.join());
    TEST(! t1.joinable());
    TEST(! t2.joinable());
    TEST(s == "HelloGoodbye" || s == "GoodbyeHello");

}
