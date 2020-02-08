#include "rs-core/hash-map.hpp"
#include "rs-core/unit-test.hpp"
#include <iterator>
#include <string>

#ifdef __clang__
    #pragma GCC diagnostic warning "-Wc++14-compat"
#endif

using namespace RS;
using namespace std::literals;

using Integer = Accountable<int>;
using String = Accountable<std::string>;
using Map = HashMap<Integer, String>;
using Set = HashSet<Integer>;

void test_core_hash_map_map_insert() {

    Map map;

    REQUIRE(Integer::count() == 0);
    REQUIRE(String::count() == 0);

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0u);
    TEST_EQUAL(map.table_size(), 0u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 0);
    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    TRY(map.insert({10, "alpha"s}));
    TEST_EQUAL(map.size(), 1u);
    TEST_EQUAL(map.table_size(), 23u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 1);
    TEST_EQUAL(Integer::count(), 1);
    TEST_EQUAL(String::count(), 1);

    TRY(map.insert({20, "bravo"s}));
    TEST_EQUAL(map.size(), 2u);
    TEST_EQUAL(map.table_size(), 23u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 2);
    TEST_EQUAL(Integer::count(), 2);
    TEST_EQUAL(String::count(), 2);

    TRY(map.insert({30, "charlie"s}));
    TEST_EQUAL(map.size(), 3u);
    TEST_EQUAL(map.table_size(), 23u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 3);
    TEST_EQUAL(Integer::count(), 3);
    TEST_EQUAL(String::count(), 3);

    // TODO

}

void test_core_hash_map_set_insert() {

    Set set;

    REQUIRE(Integer::count() == 0);
    REQUIRE(String::count() == 0);

    TEST(set.empty());
    TEST_EQUAL(set.size(), 0u);
    TEST_EQUAL(set.table_size(), 0u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 0);
    TEST_EQUAL(Integer::count(), 0);

    TRY(set.insert(10));
    TEST_EQUAL(set.size(), 1u);
    TEST_EQUAL(set.table_size(), 23u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 1);
    TEST_EQUAL(Integer::count(), 1);

    TRY(set.insert(20));
    TEST_EQUAL(set.size(), 2u);
    TEST_EQUAL(set.table_size(), 23u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 2);
    TEST_EQUAL(Integer::count(), 2);

    TRY(set.insert(30));
    TEST_EQUAL(set.size(), 3u);
    TEST_EQUAL(set.table_size(), 23u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 3);
    TEST_EQUAL(Integer::count(), 3);

    // TODO

}
