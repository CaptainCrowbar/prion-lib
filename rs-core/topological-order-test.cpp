#include "rs-core/topological-order.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <functional>
#include <vector>

using namespace RS;

void test_core_topological_order_algorithm() {

    TopologicalOrder<int> graph;
    std::vector<int> v;
    Ustring s;
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

void test_core_topological_order_reverse() {

    TopologicalOrder<int, std::greater<>> graph;
    std::vector<int> v;
    Ustring s;

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
