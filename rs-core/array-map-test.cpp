#include "rs-core/array-map.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;

void test_core_array_map_class() {

    ArrayMap<int, Ustring> map;
    ArrayMap<int, Ustring>::iterator i;
    Ustring s;

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0);
    TEST(! map.has(10));
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{}");

    TEST(map.insert({30, "hello"}).second);
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 1);
    TEST(map.has(30));
    TEST(! map.has(20));
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{30:hello}");

    TEST(map.insert({20, "world"}).second);
    TEST(map.insert({10, "goodbye"}).second);
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 3);
    TEST(map.has(30));
    TEST(map.has(20));
    TEST(map.has(10));
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{30:hello,20:world,10:goodbye}");

    TRY(map[30] = "foo");
    TRY(map[40] = "bar");
    TEST_EQUAL(map.size(), 4);
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{30:foo,20:world,10:goodbye,40:bar}");

    TEST(! map.insert({30, "hello again"}).second);
    TEST_EQUAL(map.size(), 4);
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{30:foo,20:world,10:goodbye,40:bar}");

    TRY(i = map.find(10));
    REQUIRE(i != map.end());
    TEST_EQUAL(i->first, 10);
    TEST_EQUAL(i->second, "goodbye");

    TRY(i = map.find(50));
    TEST(i == map.end());

    TRY(map.erase(map.begin()));
    TEST_EQUAL(map.size(), 3);
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{20:world,10:goodbye,40:bar}");

    TEST(map.erase(10));
    TEST_EQUAL(map.size(), 2);
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{20:world,40:bar}");

    TEST(! map.erase(30));
    TEST_EQUAL(map.size(), 2);
    TRY(s = to_str(map));
    TEST_EQUAL(s, "{20:world,40:bar}");

    TRY(map.clear());
    TEST(map.empty());

}

void test_core_array_map_set() {

    ArraySet<Ustring> set;
    ArraySet<Ustring>::iterator i;
    Ustring s;

    TEST(set.empty());
    TEST_EQUAL(set.size(), 0);
    TEST(! set.has("hello"));
    TRY(s = to_str(set));
    TEST_EQUAL(s, "[]");

    TEST(set.insert("hello").second);
    TEST(! set.empty());
    TEST_EQUAL(set.size(), 1);
    TEST(set.has("hello"));
    TEST(! set.has("goodbye"));
    TRY(s = to_str(set));
    TEST_EQUAL(s, "[hello]");

    TEST(set.insert("world").second);
    TEST(set.insert("goodbye").second);
    TEST(! set.empty());
    TEST_EQUAL(set.size(), 3);
    TEST(set.has("hello"));
    TEST(set.has("goodbye"));
    TEST(set.has("world"));
    TRY(s = to_str(set));
    TEST_EQUAL(s, "[hello,world,goodbye]");

    TEST(! set.insert("hello").second);

    TRY(i = set.find("world"));
    REQUIRE(i != set.end());
    TEST_EQUAL(*i, "world");

    TRY(i = set.find("foobar"));
    TEST(i == set.end());

    TRY(set.erase(set.begin()));
    TEST_EQUAL(set.size(), 2);
    TRY(s = to_str(set));
    TEST_EQUAL(s, "[world,goodbye]");

    TEST(set.erase("goodbye"));
    TEST_EQUAL(set.size(), 1);
    TRY(s = to_str(set));
    TEST_EQUAL(s, "[world]");

    TEST(! set.erase("hello"));
    TEST_EQUAL(set.size(), 1);
    TRY(s = to_str(set));
    TEST_EQUAL(s, "[world]");

    TRY(set.clear());
    TEST(set.empty());

}
