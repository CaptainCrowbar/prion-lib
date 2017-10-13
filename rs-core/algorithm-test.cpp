#include "rs-core/algorithm.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <functional>
#include <iterator>
#include <vector>

using namespace RS;
using namespace std::literals;

namespace {

    template <typename Range, typename DiffList>
    U8string report_diff(const Range& lhs, const Range& rhs, const DiffList& delta) {
        auto lbegin = std::begin(lhs), rbegin = std::begin(rhs);
        U8string s;
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
    U8string s;

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
    auto str_size = [] (const U8string& s) { return s.size(); };

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

void test_core_algorithm_paired_for_each() {

    std::vector<int> iv;
    Strings sv;
    int n = 0;
    auto f = [&n] (int i, U8string s) { n += i * int(s.size()); };
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

    iv = {5, 6, 4, 7, 3, 8, 2, 9, 1};
    sv = {"alpha", "bravo", "charlie", "delta", "echo", "foxtrot", "golf", "hotel", "india"};
    TRY(paired_sort(iv, sv));
    TEST_EQUAL(to_str(iv), "[1,2,3,4,5,6,7,8,9]");
    TEST_EQUAL(to_str(sv), "[india,golf,echo,charlie,alpha,bravo,delta,foxtrot,hotel]");

}

void test_core_algorithm_paired_transform() {

    std::vector<int> iv;
    Strings sv1, sv2;
    auto f = [] (int i, U8string s) -> U8string {
        U8string r;
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

void test_core_algorithm_topological_order() {

    TopologicalOrder<int> graph;
    std::vector<int> v;
    U8string s;
    int n = 0;

    TEST(graph.empty());
    TEST_EQUAL(graph.size(), 0);
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s, "");
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s, "");
    TRY(graph.insert(1));
    TEST(! graph.empty());
    TEST_EQUAL(graph.size(), 1);
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s, "1\n");
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s, "[1]\n");

    TRY(graph.insert(2, 3));
    TEST_EQUAL(graph.size(), 3);
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1\n"
        "2 => [3]\n"
        "3\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1,2]\n"
        "[3]\n"
    );

    TRY(graph.insert(4, 5));
    TEST_EQUAL(graph.size(), 5);
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1\n"
        "2 => [3]\n"
        "3\n"
        "4 => [5]\n"
        "5\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1,2,4]\n"
        "[3,5]\n"
    );

    TRY(graph.insert(2, 4));
    TEST_EQUAL(graph.size(), 5);
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1\n"
        "2 => [3,4]\n"
        "3\n"
        "4 => [5]\n"
        "5\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1,2]\n"
        "[3,4]\n"
        "[5]\n"
    );

    TRY(graph.insert(1, 4));
    TEST_EQUAL(graph.size(), 5);
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1 => [4]\n"
        "2 => [3,4]\n"
        "3\n"
        "4 => [5]\n"
        "5\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1,2]\n"
        "[3,4]\n"
        "[5]\n"
    );

    TRY(graph.clear());
    TEST_EQUAL(graph.size(), 0);
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s, "");
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s, "");

    TRY(graph.insert_mn({1,2,3}, {4,5,6}));
    TEST_EQUAL(graph.size(), 6);

    TEST(! graph.has(0));  TEST(! graph.is_front(0));  TEST(! graph.is_back(0));
    TEST(graph.has(1));    TEST(graph.is_front(1));    TEST(! graph.is_back(1));
    TEST(graph.has(2));    TEST(graph.is_front(2));    TEST(! graph.is_back(2));
    TEST(graph.has(3));    TEST(graph.is_front(3));    TEST(! graph.is_back(3));
    TEST(graph.has(4));    TEST(! graph.is_front(4));  TEST(graph.is_back(4));
    TEST(graph.has(5));    TEST(! graph.is_front(5));  TEST(graph.is_back(5));
    TEST(graph.has(6));    TEST(! graph.is_front(6));  TEST(graph.is_back(6));
    TEST(! graph.has(7));  TEST(! graph.is_front(7));  TEST(! graph.is_back(7));

    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[1,2,3]");
    TRY(v = graph.back_set());   TEST_EQUAL(to_str(v), "[4,5,6]");

    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1 => [4,5,6]\n"
        "2 => [4,5,6]\n"
        "3 => [4,5,6]\n"
        "4\n"
        "5\n"
        "6\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1,2,3]\n"
        "[4,5,6]\n"
    );

    TRY(graph.clear());
    TRY(graph.insert(1, 2, 3, 4, 5));
    TEST_EQUAL(graph.size(), 5);

    TEST(! graph.has(0));  TEST(! graph.is_front(0));  TEST(! graph.is_back(0));
    TEST(graph.has(1));    TEST(graph.is_front(1));    TEST(! graph.is_back(1));
    TEST(graph.has(2));    TEST(! graph.is_front(2));  TEST(! graph.is_back(2));
    TEST(graph.has(3));    TEST(! graph.is_front(3));  TEST(! graph.is_back(3));
    TEST(graph.has(4));    TEST(! graph.is_front(4));  TEST(! graph.is_back(4));
    TEST(graph.has(5));    TEST(! graph.is_front(5));  TEST(graph.is_back(5));
    TEST(! graph.has(6));  TEST(! graph.is_front(6));  TEST(! graph.is_back(6));

    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[1]");
    TRY(v = graph.back_set());   TEST_EQUAL(to_str(v), "[5]");

    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1 => [2,3,4,5]\n"
        "2 => [3,4,5]\n"
        "3 => [4,5]\n"
        "4 => [5]\n"
        "5\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1]\n"
        "[2]\n"
        "[3]\n"
        "[4]\n"
        "[5]\n"
    );

    TRY(graph.clear());
    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[]");
    TRY(v = graph.back_set());   TEST_EQUAL(to_str(v), "[]");
    TEST_THROW(graph.front(), TopologicalOrderEmpty);
    TEST_THROW(graph.back(), TopologicalOrderEmpty);

    TRY(graph.insert(1));
    TRY(graph.insert(2, 3));
    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[1,2]");
    TRY(v = graph.back_set());   TEST_EQUAL(to_str(v), "[1,3]");
    TRY(n = graph.front());        TEST_EQUAL(n, 1);
    TRY(n = graph.back());         TEST_EQUAL(n, 1);
    TEST(! graph.erase(0));
    TEST(graph.erase(1));
    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[2]");
    TRY(v = graph.back_set());   TEST_EQUAL(to_str(v), "[3]");
    TRY(n = graph.front());        TEST_EQUAL(n, 2);
    TRY(n = graph.back());         TEST_EQUAL(n, 3);
    TEST(graph.erase(2));
    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[3]");
    TRY(v = graph.back_set());   TEST_EQUAL(to_str(v), "[3]");
    TRY(n = graph.front());        TEST_EQUAL(n, 3);
    TRY(n = graph.back());         TEST_EQUAL(n, 3);
    TEST(graph.erase(3));
    TEST(graph.empty());

    TRY(graph.clear());
    TRY(graph.insert_mn({1}, {3,4}));
    TRY(graph.insert_mn({2}, {3,5}));
    TRY(graph.insert_mn({3}, {4,5}));
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1 => [3,4]\n"
        "2 => [3,5]\n"
        "3 => [4,5]\n"
        "4\n"
        "5\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1,2]\n"
        "[3]\n"
        "[4,5]\n"
    );

    TEST(graph.erase(3));
    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "1 => [4]\n"
        "2 => [5]\n"
        "4\n"
        "5\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[1,2]\n"
        "[4,5]\n"
    );

    TRY(graph.clear());
    TRY(graph.insert(1));
    TRY(graph.insert(2, 3));
    TRY(n = graph.pop_front());  TEST_EQUAL(n, 1);
    TRY(n = graph.pop_front());  TEST_EQUAL(n, 2);
    TRY(n = graph.pop_front());  TEST_EQUAL(n, 3);
    TEST(graph.empty());

    TRY(graph.clear());
    TRY(graph.insert(1));
    TRY(graph.insert(2, 3));
    TRY(n = graph.pop_back());  TEST_EQUAL(n, 1);
    TRY(n = graph.pop_back());  TEST_EQUAL(n, 3);
    TRY(n = graph.pop_back());  TEST_EQUAL(n, 2);
    TEST(graph.empty());

    TRY(graph.clear());
    TRY(graph.insert(1));
    TRY(graph.insert(2, 3));
    TRY(v = graph.pop_front_set());  TEST_EQUAL(to_str(v), "[1,2]");
    TRY(v = graph.pop_front_set());  TEST_EQUAL(to_str(v), "[3]");
    TEST(graph.empty());
    TRY(v = graph.pop_front_set());  TEST_EQUAL(to_str(v), "[]");

    TRY(graph.clear());
    TRY(graph.insert(1));
    TRY(graph.insert(2, 3));
    TRY(v = graph.pop_back_set());  TEST_EQUAL(to_str(v), "[1,3]");
    TRY(v = graph.pop_back_set());  TEST_EQUAL(to_str(v), "[2]");
    TEST(graph.empty());
    TRY(v = graph.pop_back_set());  TEST_EQUAL(to_str(v), "[]");

    TRY(graph.clear());
    TRY(graph.insert(1, 2));
    TRY(graph.insert(2, 3));
    TRY(graph.insert(3, 4));
    TRY(graph.insert(4, 2));
    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[1]");
    TRY(n = graph.pop_front());
    TEST_EQUAL(n, 1);
    TEST_THROW(graph.front_set(), TopologicalOrderCycle);
    TEST_THROW(graph.back_set(), TopologicalOrderCycle);
    TEST_THROW(graph.pop_front(), TopologicalOrderCycle);
    TEST_THROW(graph.pop_front_set(), TopologicalOrderCycle);
    TEST_THROW(graph.pop_back(), TopologicalOrderCycle);
    TEST_THROW(graph.pop_back_set(), TopologicalOrderCycle);

}

void test_core_algorithm_reverse_topological_order() {

    TopologicalOrder<int, std::greater<>> graph;
    std::vector<int> v;
    U8string s;

    TRY(graph.insert_mn({1,2,3}, {4,5,6}));
    TEST_EQUAL(graph.size(), 6);

    TEST(! graph.has(0));  TEST(! graph.is_front(0));  TEST(! graph.is_back(0));
    TEST(graph.has(1));    TEST(graph.is_front(1));    TEST(! graph.is_back(1));
    TEST(graph.has(2));    TEST(graph.is_front(2));    TEST(! graph.is_back(2));
    TEST(graph.has(3));    TEST(graph.is_front(3));    TEST(! graph.is_back(3));
    TEST(graph.has(4));    TEST(! graph.is_front(4));  TEST(graph.is_back(4));
    TEST(graph.has(5));    TEST(! graph.is_front(5));  TEST(graph.is_back(5));
    TEST(graph.has(6));    TEST(! graph.is_front(6));  TEST(graph.is_back(6));
    TEST(! graph.has(7));  TEST(! graph.is_front(7));  TEST(! graph.is_back(7));

    TRY(v = graph.front_set());  TEST_EQUAL(to_str(v), "[3,2,1]");
    TRY(v = graph.back_set());   TEST_EQUAL(to_str(v), "[6,5,4]");

    TRY(s = graph.format_by_node());
    TEST_EQUAL(s,
        "6\n"
        "5\n"
        "4\n"
        "3 => [6,5,4]\n"
        "2 => [6,5,4]\n"
        "1 => [6,5,4]\n"
    );
    TRY(s = graph.format_by_set());
    TEST_EQUAL(s,
        "[3,2,1]\n"
        "[6,5,4]\n"
    );

}
