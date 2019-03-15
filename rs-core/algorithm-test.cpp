#include "rs-core/algorithm.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <functional>
#include <iterator>
#include <vector>

using namespace RS;
using namespace std::literals;

namespace {

    template <typename R, typename DiffList>
    Ustring report_diff(const R& lhs, const R& rhs, const DiffList& delta) {
        auto lbegin = std::begin(lhs), rbegin = std::begin(rhs);
        Ustring s;
        for (auto& d: delta) {
            s += "A" + dec(d.del.begin() - lbegin);
            if (d.del.begin() != d.del.end())
                s += "+" + dec(d.del.end() - d.del.begin());
            s += "=>B" + dec(d.ins.begin() - rbegin);
            if (d.ins.begin() != d.ins.end())
                s += "+" + dec(d.ins.end() - d.ins.begin());
            s += ";";
        }
        return s;
    }

}

void test_core_algorithm_diff() {

    using vector_type = std::vector<int>;
    using diff_list = decltype(diff(vector_type(), vector_type()));

    vector_type a, b;
    diff_list delta;
    Ustring s;

    TRY(delta = diff(a, b));
    TRY(s = report_diff(a, b, delta));
    TEST_EQUAL(s, "");

    a = {1,2,3};
    b = {1,2,3,4,5};
    TRY(delta = diff(a, b));
    TRY(s = report_diff(a, b, delta));
    TEST_EQUAL(s,
        "A3=>B3+2;"
    );

    a = {1,2,3,4,5};
    b = {3,4,5};
    TRY(delta = diff(a, b));
    TRY(s = report_diff(a, b, delta));
    TEST_EQUAL(s,
        "A0+2=>B0;"
    );

    a = {2,4,6};
    b = {1,2,3,4,5,6};
    TRY(delta = diff(a, b));
    TRY(s = report_diff(a, b, delta));
    TEST_EQUAL(s,
        "A0=>B0+1;"
        "A1=>B2+1;"
        "A2=>B4+1;"
    );

    a = {1,2,3,4,5,6};
    b = {1,3,5};
    TRY(delta = diff(a, b));
    TRY(s = report_diff(a, b, delta));
    TEST_EQUAL(s,
        "A1+1=>B1;"
        "A3+1=>B2;"
        "A5+1=>B3;"
    );

    a = {1,2,3,4,5,6,10,11,12};
    b = {1,2,3,7,8,9,10,11,12};
    TRY(delta = diff(a, b));
    TRY(s = report_diff(a, b, delta));
    TEST_EQUAL(s,
        "A3+3=>B3+3;"
    );

}

void test_core_algorithm_edit_distance() {

    TEST_EQUAL(edit_distance(""s, ""s), 0);
    TEST_EQUAL(edit_distance("Hello"s, ""s), 5);
    TEST_EQUAL(edit_distance(""s, "Hello"s), 5);
    TEST_EQUAL(edit_distance("Hello"s, "Hello"s), 0);
    TEST_EQUAL(edit_distance("Hello"s, "hello"s), 1);
    TEST_EQUAL(edit_distance("Hell"s, "Hello"s), 1);
    TEST_EQUAL(edit_distance("Hello"s, "Hell"s), 1);
    TEST_EQUAL(edit_distance("Hello"s, "Hel"s), 2);
    TEST_EQUAL(edit_distance("Hello"s, "He"s), 3);
    TEST_EQUAL(edit_distance("Hello"s, "H"s), 4);
    TEST_EQUAL(edit_distance("Hello"s, "World"s), 4);
    TEST_EQUAL(edit_distance(""s, ""s, 3, 2, 5), 0);
    TEST_EQUAL(edit_distance("Hello"s, ""s, 3, 2, 5), 10);
    TEST_EQUAL(edit_distance(""s, "Hello"s, 3, 2, 5), 15);
    TEST_EQUAL(edit_distance("Hello"s, "Hello"s, 3, 2, 5), 0);
    TEST_EQUAL(edit_distance("Hell"s, "Hello"s, 3, 2, 5), 3);
    TEST_EQUAL(edit_distance("Hello"s, "Hell"s, 3, 2, 5), 2);
    TEST_EQUAL(edit_distance("Hello"s, "Hel"s, 3, 2, 5), 4);
    TEST_EQUAL(edit_distance("Hello"s, "He"s, 3, 2, 5), 6);
    TEST_EQUAL(edit_distance("Hello"s, "H"s, 3, 2, 5), 8);
    TEST_EQUAL(edit_distance("Hello"s, "hello"s, 3, 2, 1), 1);
    TEST_EQUAL(edit_distance("Hello"s, "hello"s, 3, 2, 2), 2);
    TEST_EQUAL(edit_distance("Hello"s, "hello"s, 3, 2, 3), 3);
    TEST_EQUAL(edit_distance("Hello"s, "hello"s, 3, 2, 4), 4);
    TEST_EQUAL(edit_distance("Hello"s, "hello"s, 3, 2, 5), 5);
    TEST_EQUAL(edit_distance("Hello"s, "hello"s, 3, 2, 6), 5);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 1, 1, 5), 8);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 2, 2, 5), 16);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 3, 3, 5), 20);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 4, 4, 5), 20);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 3, 2, 1), 4);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 3, 2, 2), 8);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 3, 2, 3), 12);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 3, 2, 4), 16);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 3, 2, 5), 20);
    TEST_EQUAL(edit_distance("Hello"s, "World"s, 3, 2, 6), 20);

}

void test_core_algorithm_find_optimum() {

    Strings vec;
    auto i = vec.begin();
    auto str_size = [] (const Ustring& s) { return s.size(); };

    TRY(i = find_optimum(vec, str_size));
    TEST(i == vec.end());

    vec = {"alpha"};
    TRY(i = find_optimum(vec, str_size));
    REQUIRE(i != vec.end());
    TEST_EQUAL(*i, "alpha");

    vec = {"alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf", "hotel"};
    TRY(i = find_optimum(vec, str_size));
    REQUIRE(i != vec.end());
    TEST_EQUAL(*i, "echo");
    TRY(i = find_optimum(vec, str_size, std::greater<size_t>()));
    REQUIRE(i != vec.end());
    TEST_EQUAL(*i, "charlie");

}

void test_core_algorithm_order_by_index() {

    std::vector<int> iv;
    Strings sv;

    TRY(order_by_index(sv, iv));
    TEST_EQUAL(to_str(sv), "[]");

    iv = {0,1,2,3,4,5,6,7,8};
    sv = {"alpha","bravo","charlie","delta","echo","foxtrot","golf","hotel","india"};
    TRY(order_by_index(sv, iv));
    TEST_EQUAL(to_str(sv), "[alpha,bravo,charlie,delta,echo,foxtrot,golf,hotel,india]");

    iv = {8,7,6,5,4,3,2,1,0};
    sv = {"alpha","bravo","charlie","delta","echo","foxtrot","golf","hotel","india"};
    TRY(order_by_index(sv, iv));
    TEST_EQUAL(to_str(sv), "[india,hotel,golf,foxtrot,echo,delta,charlie,bravo,alpha]");

    iv = {4,5,3,6,2,7,1,8,0};
    sv = {"alpha","bravo","charlie","delta","echo","foxtrot","golf","hotel","india"};
    TRY(order_by_index(sv, iv));
    TEST_EQUAL(to_str(sv), "[echo,foxtrot,delta,golf,charlie,hotel,bravo,india,alpha]");

}

void test_core_algorithm_paired_for_each() {

    std::vector<int> iv;
    Strings sv;
    int n = 0;
    auto f = [&n] (int i, Ustring s) { n += i * int(s.size()); };
    TRY(paired_for_each(iv, sv, f));
    TEST_EQUAL(n, 0);

    iv = {10, 20, 30, 40, 50};
    sv = {"alpha", "bravo", "charlie"};
    n = 0;
    TRY(paired_for_each(iv, sv, f));
    TEST_EQUAL(n, 360);

}

void test_core_algorithm_paired_sort() {

    std::vector<int> iv;
    Strings sv;

    TRY(paired_sort(iv, sv));
    TEST_EQUAL(to_str(iv), "[]");
    TEST_EQUAL(to_str(sv), "[]");

    iv = {1,2,3,4,5,6,7,8,9};
    sv = {"alpha","bravo","charlie","delta","echo","foxtrot","golf","hotel","india"};
    TRY(paired_sort(iv, sv));
    TEST_EQUAL(to_str(iv), "[1,2,3,4,5,6,7,8,9]");
    TEST_EQUAL(to_str(sv), "[alpha,bravo,charlie,delta,echo,foxtrot,golf,hotel,india]");

    iv = {9,8,7,6,5,4,3,2,1};
    sv = {"alpha","bravo","charlie","delta","echo","foxtrot","golf","hotel","india"};
    TRY(paired_sort(iv, sv));
    TEST_EQUAL(to_str(iv), "[1,2,3,4,5,6,7,8,9]");
    TEST_EQUAL(to_str(sv), "[india,hotel,golf,foxtrot,echo,delta,charlie,bravo,alpha]");

    iv = {5,6,4,7,3,8,2,9,1};
    sv = {"alpha","bravo","charlie","delta","echo","foxtrot","golf","hotel","india"};
    TRY(paired_sort(iv, sv));
    TEST_EQUAL(to_str(iv), "[1,2,3,4,5,6,7,8,9]");
    TEST_EQUAL(to_str(sv), "[india,golf,echo,charlie,alpha,bravo,delta,foxtrot,hotel]");

    iv = {5,6,4,7,3,8,2,9,1};
    sv = {"alpha","bravo","charlie","delta","echo","foxtrot","golf","hotel","india"};
    TRY(paired_sort(iv, sv, std::greater<int>()));
    TEST_EQUAL(to_str(iv), "[9,8,7,6,5,4,3,2,1]");
    TEST_EQUAL(to_str(sv), "[hotel,foxtrot,delta,bravo,alpha,charlie,echo,golf,india]");

}

void test_core_algorithm_paired_transform() {

    std::vector<int> iv;
    Strings sv1, sv2;
    auto f = [] (int i, Ustring s) -> Ustring {
        Ustring r;
        for (int j = 0; j < i; ++j)
            r += s;
        return r;
    };
    TRY(paired_transform(iv, sv1, overwrite(sv2), f));
    TEST_EQUAL(to_str(sv2), "[]");

    iv = {1, 2, 3, 4, 5, 6};
    sv1 = {"abc", "def", "ghi", "jkl"};
    TRY(paired_transform(iv, sv1, overwrite(sv2), f));
    TEST_EQUAL(to_str(sv2), "[abc,defdef,ghighighi,jkljkljkljkl]");

}
