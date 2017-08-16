#include "rs-core/optional.hpp"
#include "rs-core/unit-test.hpp"
#include <string>
#include <utility>

using namespace RS;
using namespace std::literals;

namespace {

    void check_optional_basic_operations() {

        Optional<U8string> a, b;

        TEST(! a.has_value());
        TEST(! b.has_value());
        TEST(! a);
        TEST(! b);
        TEST_THROW(*a, NullOption);
        TEST_THROW(a.check(), NullOption);
        TEST_THROW(a.value(), NullOption);
        TEST_EQUAL(a.value_or(), "");
        TEST_EQUAL(a.value_or("Hello"), "Hello");
        TEST_EQUAL(a.str(), "null");
        TEST_EQUAL(to_str(a), "null");

        TRY(a = b);             TEST(! a);  TEST(! b);
        TRY(a = std::move(b));  TEST(! a);  TEST(! b);

        TRY(a = "Hello"s);
        TEST(a.has_value());
        TEST(bool(a));
        TRY(a.check());
        TEST_EQUAL(*a, "Hello");
        TEST_EQUAL(a.value(), "Hello");
        TEST_EQUAL(a.value_or(), "Hello");
        TEST_EQUAL(a.value_or("Goodbye"), "Hello");
        TEST_EQUAL(a.str(), "Hello");
        TEST_EQUAL(to_str(a), "Hello");

        TRY(a = "Hello");  TEST(bool(a));  TRY(a = nullptr);  TEST(! a);
        TRY(a = "Hello");  TEST(bool(a));  TRY(a = {});       TEST(! a);
        TRY(a = "Hello");  TEST(bool(a));  TRY(a.reset());    TEST(! a);

        TRY(a = "Hello"s);  TRY(b.reset());       TRY(a = b);             TEST(! a);      TEST(! b);
        TRY(a = "Hello"s);  TRY(b.reset());       TRY(a = std::move(b));  TEST(! a);      TEST(! b);
        TRY(a.reset());     TRY(b = "Hello"s);    TRY(a = b);             TEST(bool(a));  TEST(bool(b));  TEST_EQUAL(*a, "Hello");    TEST_EQUAL(*b, "Hello");
        TRY(a.reset());     TRY(b = "Hello"s);    TRY(a = std::move(b));  TEST(bool(a));  TEST(! b);      TEST_EQUAL(*a, "Hello");
        TRY(a = "Hello"s);  TRY(b = "Goodbye"s);  TRY(a = b);             TEST(bool(a));  TEST(bool(b));  TEST_EQUAL(*a, "Goodbye");  TEST_EQUAL(*b, "Goodbye");
        TRY(a = "Hello"s);  TRY(b = "Goodbye"s);  TRY(a = std::move(b));  TEST(bool(a));  TEST(! b);      TEST_EQUAL(*a, "Goodbye");

        TRY(a.reset());
        TRY(a.emplace(5, 'a'));  TEST(bool(a));  TEST_EQUAL(*a, "aaaaa");
        TRY(a.emplace(5, 'z'));  TEST(bool(a));  TEST_EQUAL(*a, "zzzzz");

        TRY(a.reset());     TRY(b.reset());       TRY(swap(a, b));  TEST(! a);      TEST(! b);
        TRY(a = "Hello"s);  TRY(b.reset());       TRY(swap(a, b));  TEST(! a);      TEST(bool(b));  TEST_EQUAL(*b, "Hello");
        TRY(a.reset());     TRY(b = "Hello"s);    TRY(swap(a, b));  TEST(bool(a));  TEST(! b);      TEST_EQUAL(*a, "Hello");
        TRY(a = "Hello"s);  TRY(b = "Goodbye"s);  TRY(swap(a, b));  TEST(bool(a));  TEST(bool(b));  TEST_EQUAL(*a, "Goodbye");  TEST_EQUAL(*b, "Hello");

        TRY(a = "Hello");  TRY(a = nullptr);  TEST(! a);
        TRY(a = "Hello");  TRY(a = {});       TEST(! a);

    }

    class Accountable {
    public:
        Accountable(): text("*") { ++number; }
        Accountable(const U8string& s): text(s) { ++number; }
        Accountable(const Accountable& a): text(a.text) { ++number; }
        Accountable(Accountable&& a): text(std::move(a.text)) { ++number; }
        ~Accountable() { --number; }
        Accountable& operator=(const U8string& s) { text = s; return *this; }
        Accountable& operator=(const Accountable&) = default;
        Accountable& operator=(Accountable&&) = default;
        U8string str() const { return text; }
        static int count() noexcept { return number; }
    private:
        U8string text;
        static int number;
    };

    int Accountable::number = 0;

    void check_optional_object_accounting() {

        Optional<Accountable> a, b;
        U8string s;

        TEST_EQUAL(Accountable::count(), 0);
        TRY(a = "Hello"s);
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a.reset());
        TEST_EQUAL(Accountable::count(), 0);

        TRY(a = "Hello"s);
        TRY(b = "Goodbye"s);
        TEST_EQUAL(Accountable::count(), 2);
        TRY(a = b);
        TEST_EQUAL(Accountable::count(), 2);
        TRY(a.reset());
        TRY(b.reset());
        TEST_EQUAL(Accountable::count(), 0);

        TRY(a = "Hello"s);
        TRY(b = "Goodbye"s);
        TEST_EQUAL(Accountable::count(), 2);
        TRY(a = std::move(b));
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a.reset());
        TRY(b.reset());
        TEST_EQUAL(Accountable::count(), 0);

        TRY(a = "Hello"s);
        TRY(s = "Goodbye"s);
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a = std::move(s));
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a.reset());
        TRY(b.reset());
        TEST_EQUAL(Accountable::count(), 0);

        TRY(a = "Hello"s);
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a = b);
        TEST_EQUAL(Accountable::count(), 0);

        TRY(a = "Hello"s);
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a = std::move(b));
        TEST_EQUAL(Accountable::count(), 0);

        TRY(b = "Hello"s);
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a = b);
        TEST_EQUAL(Accountable::count(), 2);
        TRY(a.reset());
        TRY(b.reset());
        TEST_EQUAL(Accountable::count(), 0);

        TRY(b = "Hello"s);
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a = std::move(b));
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a.reset());
        TRY(b.reset());
        TEST_EQUAL(Accountable::count(), 0);

        TRY(a = "Hello"s);
        TRY(b = "Goodbye"s);
        TEST_EQUAL(Accountable::count(), 2);
        TRY(swap(a, b));
        TEST_EQUAL(Accountable::count(), 2);
        TRY(a.reset());
        TRY(b.reset());
        TEST_EQUAL(Accountable::count(), 0);

        TRY(a = "Hello"s);
        TEST_EQUAL(Accountable::count(), 1);
        TRY(swap(a, b));
        TEST_EQUAL(Accountable::count(), 1);
        TRY(a.reset());
        TRY(b.reset());
        TEST_EQUAL(Accountable::count(), 0);

    }

    void check_optional_comparison_operators() {

        Optional<U8string> a, b;

        TRY(a = "Hello"s);    TRY(b = "Hello"s);    TEST_COMPARE(a, ==, b);  TEST_COMPARE(a, <=, b);  TEST_COMPARE(a, >=, b);
        TRY(a = "Hello"s);    TRY(b = "Goodbye"s);  TEST_COMPARE(a, >, b);   TEST_COMPARE(a, >=, b);  TEST_COMPARE(a, !=, b);
        TRY(a = "Goodbye"s);  TRY(b = "Hello"s);    TEST_COMPARE(a, <, b);   TEST_COMPARE(a, <=, b);  TEST_COMPARE(a, !=, b);
        TRY(a.reset());       TRY(b.reset());       TEST_COMPARE(a, ==, b);  TEST_COMPARE(a, <=, b);  TEST_COMPARE(a, >=, b);
        TRY(a = "Hello"s);    TRY(b.reset());       TEST_COMPARE(a, >, b);   TEST_COMPARE(a, >=, b);  TEST_COMPARE(a, !=, b);
        TRY(a.reset());       TRY(b = "Hello"s);    TEST_COMPARE(a, <, b);   TEST_COMPARE(a, <=, b);  TEST_COMPARE(a, !=, b);

        TRY(a = "Hello"s);    TEST_COMPARE(a, ==, "Hello"s);  TEST_COMPARE(a, <=, "Hello"s);  TEST_COMPARE(a, >=, "Hello"s);
        TRY(a = "Hello"s);    TEST_COMPARE("Hello"s, ==, a);  TEST_COMPARE("Hello"s, <=, a);  TEST_COMPARE("Hello"s, >=, a);
        TRY(a = "Goodbye"s);  TEST_COMPARE(a, <, "Hello"s);   TEST_COMPARE(a, <=, "Hello"s);  TEST_COMPARE(a, !=, "Hello"s);
        TRY(a = "Goodbye"s);  TEST_COMPARE("Hello"s, >, a);   TEST_COMPARE("Hello"s, >=, a);  TEST_COMPARE("Hello"s, !=, a);
        TRY(a.reset());       TEST_COMPARE(a, <, "Hello"s);   TEST_COMPARE(a, <=, "Hello"s);  TEST_COMPARE(a, !=, "Hello"s);
        TRY(a.reset());       TEST_COMPARE("Hello"s, >, a);   TEST_COMPARE("Hello"s, >=, a);  TEST_COMPARE("Hello"s, !=, a);
        TRY(a = "Hello"s);    TEST_COMPARE(a, >, nullptr);    TEST_COMPARE(a, >=, nullptr);   TEST_COMPARE(a, !=, nullptr);
        TRY(a = "Hello"s);    TEST_COMPARE(nullptr, <, a);    TEST_COMPARE(nullptr, <=, a);   TEST_COMPARE(nullptr, !=, a);
        TRY(a.reset());       TEST_COMPARE(a, ==, nullptr);   TEST_COMPARE(a, <=, nullptr);   TEST_COMPARE(a, >=, nullptr);
        TRY(a.reset());       TEST_COMPARE(nullptr, ==, a);   TEST_COMPARE(nullptr, <=, a);   TEST_COMPARE(nullptr, >=, a);

    }

    void check_optional_coalescing_operators() {

        Optional<U8string> a = "Hello"s, b = "Goodbye"s, c, d, e;

        TRY(c = a & b);         TEST_EQUAL(to_str(c), "Goodbye");
        TRY(c = b & a);         TEST_EQUAL(to_str(c), "Hello");
        TRY(c = a & d);         TEST_EQUAL(to_str(c), "null");
        TRY(c = d & a);         TEST_EQUAL(to_str(c), "null");
        TRY(c = a & "World"s);  TEST_EQUAL(to_str(c), "World");
        TRY(c = "World"s & a);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = a & nullptr);   TEST_EQUAL(to_str(c), "null");
        TRY(c = nullptr & a);   TEST_EQUAL(to_str(c), "null");

        TRY(c = a | b);         TEST_EQUAL(to_str(c), "Hello");
        TRY(c = b | a);         TEST_EQUAL(to_str(c), "Goodbye");
        TRY(c = a | d);         TEST_EQUAL(to_str(c), "Hello");
        TRY(c = d | a);         TEST_EQUAL(to_str(c), "Hello");
        TRY(c = a | "World"s);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = "World"s | a);  TEST_EQUAL(to_str(c), "World");
        TRY(c = a | nullptr);   TEST_EQUAL(to_str(c), "Hello");
        TRY(c = nullptr | a);   TEST_EQUAL(to_str(c), "Hello");

        TRY(c = "Goodbye"s);
        TRY(d = "Again"s);

        TRY(e = a & b & c & d);  TEST_EQUAL(to_str(e), "Again");
        TRY(e = a | b | c | d);  TEST_EQUAL(to_str(e), "Hello");

        TRY(b.reset());

        TRY(c = a & a & a & a);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = a & a & a & b);  TEST_EQUAL(to_str(c), "null");
        TRY(c = a & a & b & b);  TEST_EQUAL(to_str(c), "null");
        TRY(c = a & b & b & b);  TEST_EQUAL(to_str(c), "null");
        TRY(c = b & b & b & b);  TEST_EQUAL(to_str(c), "null");
        TRY(c = b & b & b & a);  TEST_EQUAL(to_str(c), "null");
        TRY(c = b & b & a & a);  TEST_EQUAL(to_str(c), "null");
        TRY(c = b & a & a & a);  TEST_EQUAL(to_str(c), "null");
        TRY(c = a | a | a | a);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = a | a | a | b);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = a | a | b | b);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = a | b | b | b);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = b | b | b | b);  TEST_EQUAL(to_str(c), "null");
        TRY(c = b | b | b | a);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = b | b | a | a);  TEST_EQUAL(to_str(c), "Hello");
        TRY(c = b | a | a | a);  TEST_EQUAL(to_str(c), "Hello");

    }

    void check_optional_function_calls() {

        Optional<U8string> sx;
        Optional<size_t> nx;

        auto fix_size = [] (U8string& s) { s.resize(10, '*'); };
        auto get_size = [] (const U8string& s) { return s.size(); };
        auto set_size = [] (U8string& s, size_t n, char c) { s.resize(n, c); };

        TRY(opt_call(fix_size, sx));
        TEST(! sx);

        TRY(nx = opt_call(get_size, sx));
        TEST(! nx);

        TRY(opt_call(set_size, sx, 15, '!'));
        TEST(! sx);

        TRY(sx = "Hello"s);

        TRY(opt_call(fix_size, sx));
        TEST(sx);
        TEST_EQUAL(*sx, "Hello*****");

        TRY(nx = opt_call(get_size, sx));
        TEST(nx);
        TEST_EQUAL(*nx, 10);

        TRY(opt_call(set_size, sx, 15, '!'));
        TEST(sx);
        TEST_EQUAL(*sx, "Hello*****!!!!!");

        TRY(sx.reset());
        TRY(nx.reset());
        TRY(opt_call(set_size, sx, nx, '*'));
        TEST(! sx);

        TRY(sx = "Hello"s);
        TRY(nx.reset());
        TRY(opt_call(set_size, sx, nx, '*'));
        TEST(sx);
        TEST_EQUAL(*sx, "Hello");

        TRY(sx.reset());
        TRY(nx = 10);
        TRY(opt_call(set_size, sx, nx, '*'));
        TEST(! sx);

        TRY(sx = "Hello"s);
        TRY(nx = 10);
        TRY(opt_call(set_size, sx, nx, '*'));
        TEST(sx);
        TEST_EQUAL(*sx, "Hello*****");

    }

}

TEST_MODULE(core, optional) {

    check_optional_basic_operations();
    check_optional_object_accounting();
    check_optional_comparison_operators();
    check_optional_coalescing_operators();
    check_optional_function_calls();

}
