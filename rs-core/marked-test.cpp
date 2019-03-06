#include "rs-core/marked.hpp"
#include "rs-core/meta.hpp"
#include "rs-core/serial.hpp"
#include "rs-core/unit-test.hpp"
#include <complex>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>

using namespace RS;
using namespace std::literals;

void test_source_marked_value_access() {

    class Foo { RS_NO_INSTANCE(Foo); };

    using FooStr = Marked<std::string, Foo>;

    FooStr foo;

    TEST_EQUAL(foo.id_name(), "Foo");
    TEST_EQUAL(foo.get(), "");
    TEST_EQUAL(*foo, "");
    TRY(*foo = "Hello world");
    TEST_EQUAL(*foo, "Hello world");
    TRY(foo.set("Don't panic"));
    TEST_EQUAL(foo.get(), "Don't panic");
    TEST_EQUAL(*foo, "Don't panic");

    TEST_EQUAL((Marked<int, void>::id_name()), "void");
    TEST_EQUAL((Marked<int, std::string>::id_name()), "basic_string");
    TEST_EQUAL((Marked<int, RS::Version>::id_name()), "Version");
    TEST_EQUAL((Marked<int, RS::Meta::Nil>::id_name()), "Typelist");

}

void test_source_marked_conversion_operators() {

    class Foo {};
    class Bar {};
    class Zap {};

    using FooStr = Marked<std::string, Foo>;
    using BarStr = Marked<std::string, Bar, Mark::implicit_in>;
    using ZapStr = Marked<std::string, Zap, Mark::implicit_out>;

    FooStr foo;
    BarStr bar;
    ZapStr zap;
    Ustring str;

    TRY(*foo = "Hello world");
    TRY(*bar = "Goodnight moon");
    TRY(*zap = "Don't panic");

    str.clear();  TRY(str = Ustring(bar));  TEST_EQUAL(str, "Goodnight moon");
    str.clear();  TRY(str = Ustring(zap));  TEST_EQUAL(str, "Don't panic");
    str.clear();  TRY(str = zap);           TEST_EQUAL(str, "Don't panic");

    TRY(*bar = "");  TRY(bar = BarStr("Goodnight moon"s));  TEST_EQUAL(*bar, "Goodnight moon");
    TRY(*bar = "");  TRY(bar = "Goodnight moon"s);          TEST_EQUAL(*bar, "Goodnight moon");
    TRY(*zap = "");  TRY(zap = ZapStr("Don't panic"s));     TEST_EQUAL(*zap, "Don't panic");

}

void test_source_marked_copying_and_moving() {

    class Foo {};
    class Bar {};
    class Zap {};

    using FooInt = Marked<int, Foo, Mark::no_copy>;
    using BarInt = Marked<int, Bar, Mark::no_move>;
    using ZapInt = Marked<int, Zap, Mark::reset_on_move>;

    FooInt foo1, foo2;
    BarInt bar1;
    ZapInt zap1, zap2;
    Ustring str;

    TRY(*foo1 = 42);
    TRY(*bar1 = 86);
    TRY(*zap1 = 99);

    TRY(*foo2 = -1);  TRY(foo2 = std::move(foo1));  TEST_EQUAL(*foo1, 42);  TEST_EQUAL(*foo2, 42);
    TRY(*zap2 = -1);  TRY(zap2 = std::move(zap1));  TEST_EQUAL(*zap1, 0);   TEST_EQUAL(*zap2, 99);

}

void test_source_marked_string_conversion() {

    class Foo {};
    class Bar {};

    using FooInt = Marked<int, Foo>;
    using BarInt = Marked<int, Bar, Mark::string_tag>;

    FooInt foo1, foo2;
    BarInt bar1, bar2;
    Ustring str;

    TRY(*foo1 = 86);
    TRY(*bar1 = 99);

    TRY(str = to_str(foo1));  TEST_EQUAL(str, "86");      TEST(from_str(str, foo2));  TEST_EQUAL(*foo2, 86);
    TRY(str = to_str(bar1));  TEST_EQUAL(str, "Bar:99");  TEST(from_str(str, bar2));  TEST_EQUAL(*bar2, 99);

}

void test_source_marked_json_conversion() {

    class Foo {};
    class Bar {};

    using FooInt = Marked<int, Foo>;
    using BarInt = Marked<int, Bar, Mark::json_tag>;

    FooInt foo1, foo2;
    BarInt bar1, bar2;
    json j;
    Ustring str;

    TRY(*foo1 = 86);
    TRY(*bar1 = 99);

    TRY(to_json(j, foo1));  TEST_EQUAL(j.dump(), "86");                                TRY(from_json(j, foo2));  TEST_EQUAL(*foo2, 86);
    TRY(to_json(j, bar1));  TEST_EQUAL(j.dump(), "{\"_type\":\"Bar\",\"value\":99}");  TRY(from_json(j, bar2));  TEST_EQUAL(*bar2, 99);

}

void test_source_marked_boolean_conversion() {

    class Foo {};
    class Bar {};

    using FooInt = Marked<int, Foo>;
    using BarPtr = Marked<std::unique_ptr<int>, Bar>;

    FooInt foo;
    BarPtr bar;
    bool b = false;

    TRY(b = bool(foo));  TEST(! b);
    TRY(b = bool(bar));  TEST(! b);

    TEST(! foo);
    TEST(! bar);

    TRY(*foo = 86);
    TRY(*bar = std::make_unique<int>(99));

    TRY(b = bool(foo));  TEST(b);
    TRY(b = bool(bar));  TEST(b);

    TEST(foo);
    TEST(bar);

}

void test_source_marked_comparison_operators() {

    class Foo {};
    class Bar {};

    using FooInt = Marked<int, Foo>;
    using BarCmp = Marked<std::complex<double>, Bar>;

    FooInt foo1, foo2;
    BarCmp bar1, bar2;

    TEST(Meta::has_equal_operator<FooInt>);             TEST(Meta::has_equal_operator<BarCmp>);
    TEST(Meta::has_not_equal_operator<FooInt>);         TEST(Meta::has_not_equal_operator<BarCmp>);
    TEST(Meta::has_less_than_operator<FooInt>);         TEST(! Meta::has_less_than_operator<BarCmp>);
    TEST(Meta::has_greater_than_operator<FooInt>);      TEST(! Meta::has_greater_than_operator<BarCmp>);
    TEST(Meta::has_less_or_equal_operator<FooInt>);     TEST(! Meta::has_less_or_equal_operator<BarCmp>);
    TEST(Meta::has_greater_or_equal_operator<FooInt>);  TEST(! Meta::has_greater_or_equal_operator<BarCmp>);

    TRY(*foo1 = 99);
    TRY(*foo2 = 42);
    TRY((*bar1 = {1,2}));
    TRY((*bar2 = {3,4}));

    TEST(! (*foo1 == *foo2));
    TEST(*foo1 != *foo2);
    TEST(! (*foo1 < *foo2));
    TEST(*foo1 > *foo2);
    TEST(! (*foo1 <= *foo2));
    TEST(*foo1 >= *foo2);
    TEST(! (*bar1 == *bar2));
    TEST(*bar1 != *bar2);

}

void test_source_marked_arithmetic_operators() {

    class Foo {};
    class Bar {};

    using FooInt = Marked<int, Foo, Mark::arithmetic>;
    using BarInt = Marked<int, Bar>;

    FooInt foo1, foo2, foo3;

    TEST(Meta::has_unary_plus_operator<FooInt>);          TEST(! Meta::has_unary_plus_operator<BarInt>);
    TEST(Meta::has_unary_minus_operator<FooInt>);         TEST(! Meta::has_unary_minus_operator<BarInt>);
    TEST(Meta::has_pre_increment_operator<FooInt>);       TEST(! Meta::has_pre_increment_operator<BarInt>);
    TEST(Meta::has_post_increment_operator<FooInt>);      TEST(! Meta::has_post_increment_operator<BarInt>);
    TEST(Meta::has_pre_decrement_operator<FooInt>);       TEST(! Meta::has_pre_decrement_operator<BarInt>);
    TEST(Meta::has_post_decrement_operator<FooInt>);      TEST(! Meta::has_post_decrement_operator<BarInt>);
    TEST(Meta::has_plus_operator<FooInt>);                TEST(! Meta::has_plus_operator<BarInt>);
    TEST(Meta::has_minus_operator<FooInt>);               TEST(! Meta::has_minus_operator<BarInt>);
    TEST(Meta::has_multiply_operator<FooInt>);            TEST(! Meta::has_multiply_operator<BarInt>);
    TEST(Meta::has_divide_operator<FooInt>);              TEST(! Meta::has_divide_operator<BarInt>);
    TEST(Meta::has_remainder_operator<FooInt>);           TEST(! Meta::has_remainder_operator<BarInt>);
    TEST(Meta::has_bitwise_and_operator<FooInt>);         TEST(! Meta::has_bitwise_and_operator<BarInt>);
    TEST(Meta::has_bitwise_or_operator<FooInt>);          TEST(! Meta::has_bitwise_or_operator<BarInt>);
    TEST(Meta::has_bitwise_xor_operator<FooInt>);         TEST(! Meta::has_bitwise_xor_operator<BarInt>);
    TEST(Meta::has_left_shift_operator<FooInt>);          TEST(! Meta::has_left_shift_operator<BarInt>);
    TEST(Meta::has_right_shift_operator<FooInt>);         TEST(! Meta::has_right_shift_operator<BarInt>);
    TEST(Meta::has_plus_assign_operator<FooInt>);         TEST(! Meta::has_plus_assign_operator<BarInt>);
    TEST(Meta::has_minus_assign_operator<FooInt>);        TEST(! Meta::has_minus_assign_operator<BarInt>);
    TEST(Meta::has_multiply_assign_operator<FooInt>);     TEST(! Meta::has_multiply_assign_operator<BarInt>);
    TEST(Meta::has_divide_assign_operator<FooInt>);       TEST(! Meta::has_divide_assign_operator<BarInt>);
    TEST(Meta::has_remainder_assign_operator<FooInt>);    TEST(! Meta::has_remainder_assign_operator<BarInt>);
    TEST(Meta::has_bitwise_and_assign_operator<FooInt>);  TEST(! Meta::has_bitwise_and_assign_operator<BarInt>);
    TEST(Meta::has_bitwise_or_assign_operator<FooInt>);   TEST(! Meta::has_bitwise_or_assign_operator<BarInt>);
    TEST(Meta::has_bitwise_xor_assign_operator<FooInt>);  TEST(! Meta::has_bitwise_xor_assign_operator<BarInt>);
    TEST(Meta::has_left_shift_assign_operator<FooInt>);   TEST(! Meta::has_left_shift_assign_operator<BarInt>);
    TEST(Meta::has_right_shift_assign_operator<FooInt>);  TEST(! Meta::has_right_shift_assign_operator<BarInt>);

    TRY(*foo1 = 86);

    #ifndef _MSC_VER

        TRY(foo2 = + foo1);  TEST_EQUAL(*foo2, 86);
        TRY(foo2 = - foo1);  TEST_EQUAL(*foo2, -86);

    #endif

    TRY(foo2 = ++foo1);  TEST_EQUAL(*foo1, 87);  TEST_EQUAL(*foo2, 87);
    TRY(foo2 = foo1++);  TEST_EQUAL(*foo1, 88);  TEST_EQUAL(*foo2, 87);
    TRY(foo2 = --foo1);  TEST_EQUAL(*foo1, 87);  TEST_EQUAL(*foo2, 87);
    TRY(foo2 = foo1--);  TEST_EQUAL(*foo1, 86);  TEST_EQUAL(*foo2, 87);

    TRY(*foo1 = 99);
    TRY(*foo2 = 42);

    TRY(foo3 = foo1 + foo2);  TEST_EQUAL(*foo3, 141);
    TRY(foo3 = foo1 - foo2);  TEST_EQUAL(*foo3, 57);
    TRY(foo3 = foo1 * foo2);  TEST_EQUAL(*foo3, 4158);
    TRY(foo3 = foo1 / foo2);  TEST_EQUAL(*foo3, 2);
    TRY(foo3 = foo1 % foo2);  TEST_EQUAL(*foo3, 15);
    TRY(foo3 = foo1 & foo2);  TEST_EQUAL(*foo3, 34);
    TRY(foo3 = foo1 | foo2);  TEST_EQUAL(*foo3, 107);
    TRY(foo3 = foo1 ^ foo2);  TEST_EQUAL(*foo3, 73);

    TRY(*foo1 = 99);  TRY(foo3 = foo1 += foo2);  TEST_EQUAL(*foo1, 141);   TEST_EQUAL(*foo3, 141);
    TRY(*foo1 = 99);  TRY(foo3 = foo1 -= foo2);  TEST_EQUAL(*foo1, 57);    TEST_EQUAL(*foo3, 57);
    TRY(*foo1 = 99);  TRY(foo3 = foo1 *= foo2);  TEST_EQUAL(*foo1, 4158);  TEST_EQUAL(*foo3, 4158);
    TRY(*foo1 = 99);  TRY(foo3 = foo1 /= foo2);  TEST_EQUAL(*foo1, 2);     TEST_EQUAL(*foo3, 2);
    TRY(*foo1 = 99);  TRY(foo3 = foo1 %= foo2);  TEST_EQUAL(*foo1, 15);    TEST_EQUAL(*foo3, 15);
    TRY(*foo1 = 99);  TRY(foo3 = foo1 &= foo2);  TEST_EQUAL(*foo1, 34);    TEST_EQUAL(*foo3, 34);
    TRY(*foo1 = 99);  TRY(foo3 = foo1 |= foo2);  TEST_EQUAL(*foo1, 107);   TEST_EQUAL(*foo3, 107);
    TRY(*foo1 = 99);  TRY(foo3 = foo1 ^= foo2);  TEST_EQUAL(*foo1, 73);    TEST_EQUAL(*foo3, 73);

}

void test_source_marked_function_call_operators() {

    struct ConstFunction {
        int value = 0;
        int operator()(int a, int b) const { return value + a + b; }
    };

    struct MutableFunction {
        int value = 0;
        int operator()(int a, int b) { value += a + b; return value; }
    };

    class Foo {};
    class Bar {};

    using FooFun = Marked<ConstFunction, Foo, Mark::function>;
    using BarFun = Marked<MutableFunction, Bar, Mark::function>;

    FooFun foo;
    BarFun bar;
    int x = 0;

    TRY(x = foo(1, 2));  TEST_EQUAL(x, 3);   TEST_EQUAL(foo->value, 0);
    TRY(x = foo(3, 4));  TEST_EQUAL(x, 7);   TEST_EQUAL(foo->value, 0);
    TRY(x = bar(1, 2));  TEST_EQUAL(x, 3);   TEST_EQUAL(bar->value, 3);
    TRY(x = bar(3, 4));  TEST_EQUAL(x, 10);  TEST_EQUAL(bar->value, 10);

}

void test_source_marked_subscript_operators() {

    class Foo {};
    class Bar {};
    class Zap {};

    using FooStr = Marked<std::string, Foo, Mark::subscript>;
    using BarStr = Marked<std::string, Bar>;
    using ZapInt = Marked<int, Zap, Mark::subscript>;

    FooStr foo;

    TEST((Meta::has_index_operator<FooStr, int>));
    TEST((! Meta::has_index_operator<BarStr, int>));
    TEST((! Meta::has_index_operator<ZapInt, int>));

    TRY(*foo = "Hello");
    TEST_EQUAL(foo[0], 'H');
    TRY(foo[0] = 'Y');
    TEST_EQUAL(*foo, "Yello");

}

void test_source_marked_hash_functions() {

    class Foo {};

    using FooInt = Marked<int, Foo>;

    std::unordered_set<FooInt> set;

    for (int i = 1; i <= 10; ++i)
        for (int j = 1; j <= i; ++j)
            set.insert(FooInt(i));
    TEST_EQUAL(set.size(), 10);
    for (int i = 1; i <= 10; ++i)
        TEST_EQUAL(set.count(FooInt(i)), 1);

}

void test_source_marked_checked_values() {

    struct to_lower {
        std::string operator()(const std::string& s) const {
            return ascii_lowercase(s);
        }
    };

    struct is_lower {
        const std::string& operator()(const std::string& s) const {
            for (char c: s)
                if (ascii_isupper(c))
                    throw std::invalid_argument("Not lower case");
            return s;
        }
    };

    class Foo {};
    class Bar {};
    class Zap {};

    using FooStr = Marked<std::string, Foo, Mark::implicit_in>;
    using BarStr = Marked<std::string, Bar, Mark::implicit_in, to_lower>;
    using ZapInt = Marked<std::string, Zap, Mark::implicit_in, is_lower>;

    FooStr foo;
    BarStr bar;
    ZapInt zap;

    TRY(foo = "hello world"s);  TEST_EQUAL(*foo, "hello world");
    TRY(bar = "hello world"s);  TEST_EQUAL(*bar, "hello world");
    TRY(zap = "hello world"s);  TEST_EQUAL(*zap, "hello world");

    TRY(foo = "Don't Panic"s);                                TEST_EQUAL(*foo, "Don't Panic");
    TRY(bar = "Don't Panic"s);                                TEST_EQUAL(*bar, "don't panic");
    TEST_THROW(zap = "Don't Panic"s, std::invalid_argument);  TEST_EQUAL(*zap, "hello world");

}
