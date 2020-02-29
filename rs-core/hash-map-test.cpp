#include "rs-core/hash-map.hpp"
#include "rs-core/unit-test.hpp"
#include <iterator>
#include <string>

using namespace RS;
using namespace std::literals;

using Integer = Accountable<int>;
using String = Accountable<std::string>;

void test_core_hash_map_int_string_map_basic_insert_and_lookup() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<Integer, String> map;
    std::string s;

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0u);
    TEST_EQUAL(map.table_size(), 0u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 0);

    TEST(! map.contains(10));

    map.insert({10, "alpha"s});
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 1u);
    TEST_EQUAL(map.table_size(), 16u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 1);

    TEST(map.contains(10));
    TEST(! map.contains(20));
    s = map[10].get(); TEST_EQUAL(s, "alpha"s);

    map.insert({20, "bravo"s});
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 2u);
    TEST_EQUAL(map.table_size(), 16u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 2);

    TEST(map.contains(10));
    TEST(map.contains(20));
    TEST(! map.contains(30));
    s = map[10].get(); TEST_EQUAL(s, "alpha"s);
    s = map[20].get(); TEST_EQUAL(s, "bravo"s);

    map.insert({30, "charlie"s});
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 3u);
    TEST_EQUAL(map.table_size(), 16u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 3);

    TEST(map.contains(10));
    TEST(map.contains(20));
    TEST(map.contains(30));
    TEST(! map.contains(40));
    s = map[10].get(); TEST_EQUAL(s, "alpha"s);
    s = map[20].get(); TEST_EQUAL(s, "bravo"s);
    s = map[30].get(); TEST_EQUAL(s, "charlie"s);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_int_string_map_large_insertions() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<Integer, String> map;
    std::string s, t;
    int i = 1;

    for (; i <= 12; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({i, s});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 16u);
        t = map[i].get();
        TEST_EQUAL(t, s);
    }

    for (; i <= 24; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({i, s});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 32u);
        t = map[i].get();
        TEST_EQUAL(t, s);
    }

    for (; i <= 48; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({i, s});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 64u);
        t = map[i].get();
        TEST_EQUAL(t, s);
    }

    for (; i <= 96; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({i, s});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 128u);
        t = map[i].get();
        TEST_EQUAL(t, s);
    }

    for (; i <= 192; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({i, s});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 256u);
        t = map[i].get();
        TEST_EQUAL(t, s);
    }

    for (; i <= 384; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({i, s});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 512u);
        t = map[i].get();
        TEST_EQUAL(t, s);
    }

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_int_string_map_erasure() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<Integer, String> map;
    HashMap<Integer, String>::const_iterator it;
    size_t n = 0;

    map.insert({10, "alpha"s});
    map.insert({20, "bravo"s});
    map.insert({30, "charlie"s});
    map.insert({40, "delta"s});
    map.insert({50, "echo"s});
    TEST_EQUAL(map.size(), 5u);

    n = map.erase(30);
    TEST_EQUAL(n, 1);
    TEST_EQUAL(map.size(), 4u);
    it = map.find(30);
    TEST(it == map.end());
    it = map.find(40);
    TEST(it != map.end());
    TEST_EQUAL(it->first, 40);
    TEST_EQUAL(it->second, "delta"s);

    map.erase(it);
    TEST_EQUAL(map.size(), 3u);
    it = map.find(40);
    TEST(it == map.end());

    n = map.erase(40);
    TEST_EQUAL(n, 0);
    TEST_EQUAL(map.size(), 3u);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_int_string_map_initializer_list() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<Integer, String> map {
        { 10, "alpha"s },
        { 20, "bravo"s },
        { 30, "charlie"s },
    };

    TEST_EQUAL(map.size(), 3u);
    TEST(map.contains(10));
    TEST(map.contains(20));
    TEST(map.contains(30));
    TEST(! map.contains(40));
    TEST_EQUAL(map[10], "alpha"s);
    TEST_EQUAL(map[20], "bravo"s);
    TEST_EQUAL(map[30], "charlie"s);

    map = {
        { 40, "delta"s },
        { 50, "echo"s },
        { 60, "foxtrot"s },
        { 70, "golf"s },
    };

    TEST_EQUAL(map.size(), 4u);
    TEST(! map.contains(30));
    TEST(map.contains(40));
    TEST(map.contains(50));
    TEST(map.contains(60));
    TEST(map.contains(70));
    TEST(! map.contains(80));
    TEST_EQUAL(map[40], "delta"s);
    TEST_EQUAL(map[50], "echo"s);
    TEST_EQUAL(map[60], "foxtrot"s);
    TEST_EQUAL(map[70], "golf"s);

    map = {};

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0u);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_int_string_map_emplace() {

    HashMap<int, std::string> map;

    map.emplace(10, 1, 'a');
    map.emplace(20, 2, 'b');
    map.emplace(30, 3, 'c');

    TEST_EQUAL(map.size(), 3u);
    TEST(map.contains(10));
    TEST(map.contains(20));
    TEST(map.contains(30));
    TEST_EQUAL(map[10], "a"s);
    TEST_EQUAL(map[20], "bb"s);
    TEST_EQUAL(map[30], "ccc"s);

}

void test_core_hash_map_int_string_map_implicit_insert() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<Integer, String> map;

    map[10] = "alpha"s;
    map[20] = "bravo"s;
    map[30] = "charlie"s;

    TEST_EQUAL(map.size(), 3u);
    TEST(map.contains(10));
    TEST(map.contains(20));
    TEST(map.contains(30));
    TEST_EQUAL(map[10], "alpha"s);
    TEST_EQUAL(map[20], "bravo"s);
    TEST_EQUAL(map[30], "charlie"s);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_int_map_basic_insert_and_lookup() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<String, Integer> map;
    int i = 0;

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0u);
    TEST_EQUAL(map.table_size(), 0u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 0);

    TEST(! map.contains("alpha"s));

    map.insert({"alpha"s, 10});
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 1u);
    TEST_EQUAL(map.table_size(), 16u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 1);

    TEST(map.contains("alpha"s));
    TEST(! map.contains("bravo"s));
    i = map["alpha"s].get(); TEST_EQUAL(i, 10);

    map.insert({"bravo"s, 20});
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 2u);
    TEST_EQUAL(map.table_size(), 16u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 2);

    TEST(map.contains("alpha"s));
    TEST(map.contains("bravo"s));
    TEST(! map.contains("charlie"s));
    i = map["alpha"s].get(); TEST_EQUAL(i, 10);
    i = map["bravo"s].get(); TEST_EQUAL(i, 20);

    map.insert({"charlie"s, 30});
    TEST(! map.empty());
    TEST_EQUAL(map.size(), 3u);
    TEST_EQUAL(map.table_size(), 16u);
    TEST_EQUAL(std::distance(map.begin(), map.end()), 3);

    TEST(map.contains("alpha"s));
    TEST(map.contains("bravo"s));
    TEST(map.contains("charlie"s));
    TEST(! map.contains("delta"s));
    i = map["alpha"s].get(); TEST_EQUAL(i, 10);
    i = map["bravo"s].get(); TEST_EQUAL(i, 20);
    i = map["charlie"s].get(); TEST_EQUAL(i, 30);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_int_map_large_insertions() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<String, Integer> map;
    std::string s;
    int i = 1, j = 0;

    for (; i <= 12; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({s, i});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 16u);
        j = map[s].get();
        TEST_EQUAL(j, i);
    }

    for (; i <= 24; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({s, i});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 32u);
        j = map[s].get();
        TEST_EQUAL(j, i);
    }

    for (; i <= 48; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({s, i});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 64u);
        j = map[s].get();
        TEST_EQUAL(j, i);
    }

    for (; i <= 96; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({s, i});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 128u);
        j = map[s].get();
        TEST_EQUAL(j, i);
    }

    for (; i <= 192; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({s, i});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 256u);
        j = map[s].get();
        TEST_EQUAL(j, i);
    }

    for (; i <= 384; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        map.insert({s, i});
        TEST_EQUAL(map.size(), size_t(i));
        TEST_EQUAL(map.table_size(), 512u);
        j = map[s].get();
        TEST_EQUAL(j, i);
    }

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_int_map_erasure() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<String, Integer> map;
    HashMap<String, Integer>::const_iterator it;
    size_t n = 0;

    map.insert({"alpha"s, 10});
    map.insert({"bravo"s, 20});
    map.insert({"charlie"s, 30});
    map.insert({"delta"s, 40});
    map.insert({"echo"s, 50});
    TEST_EQUAL(map.size(), 5u);

    n = map.erase("charlie"s);
    TEST_EQUAL(n, 1u);
    TEST_EQUAL(map.size(), 4u);
    it = map.find("charlie"s);
    TEST(it == map.end());
    it = map.find("delta"s);
    TEST(it != map.end());
    TEST_EQUAL(it->first, "delta"s);
    TEST_EQUAL(it->second, 40);

    map.erase(it);
    TEST_EQUAL(map.size(), 3u);
    it = map.find("delta"s);
    TEST(it == map.end());

    n = map.erase("delta"s);
    TEST_EQUAL(n, 0);
    TEST_EQUAL(map.size(), 3u);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_int_map_initializer_list() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<String, Integer> map {
        { "alpha"s, 10 },
        { "bravo"s, 20 },
        { "charlie"s, 30 },
    };

    TEST_EQUAL(map.size(), 3u);
    TEST(map.contains("alpha"s));
    TEST(map.contains("bravo"s));
    TEST(map.contains("charlie"s));
    TEST(! map.contains("delta"s));
    TEST_EQUAL(map["alpha"s], 10);
    TEST_EQUAL(map["bravo"s], 20);
    TEST_EQUAL(map["charlie"s], 30);

    map = {
        { "delta"s, 40 },
        { "echo"s, 50 },
        { "foxtrot"s, 60 },
        { "golf"s, 70 },
    };

    TEST_EQUAL(map.size(), 4u);
    TEST(! map.contains("charlie"s));
    TEST(map.contains("delta"s));
    TEST(map.contains("echo"s));
    TEST(map.contains("foxtrot"s));
    TEST(map.contains("golf"s));
    TEST(! map.contains("hotel"s));
    TEST_EQUAL(map["delta"s], 40);
    TEST_EQUAL(map["echo"s], 50);
    TEST_EQUAL(map["foxtrot"s], 60);
    TEST_EQUAL(map["golf"s], 70);

    map = {};

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0u);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_int_map_emplace() {

    HashMap<std::string, int> map;

    map.emplace("alpha", 10);
    map.emplace("bravo", 20);
    map.emplace("charlie", 30);

    TEST_EQUAL(map.size(), 3u);
    TEST(map.contains("alpha"s));
    TEST(map.contains("bravo"s));
    TEST(map.contains("charlie"s));
    TEST_EQUAL(map["alpha"s], 10);
    TEST_EQUAL(map["bravo"s], 20);
    TEST_EQUAL(map["charlie"s], 30);

}

void test_core_hash_map_string_int_map_implicit_insert() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashMap<String, Integer> map;

    map["alpha"s] = 10;
    map["bravo"s] = 20;
    map["charlie"s] = 30;

    TEST_EQUAL(map.size(), 3u);
    TEST(map.contains("alpha"s));
    TEST(map.contains("bravo"s));
    TEST(map.contains("charlie"s));
    TEST_EQUAL(map["alpha"s], 10);
    TEST_EQUAL(map["bravo"s], 20);
    TEST_EQUAL(map["charlie"s], 30);

    TRY(map.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_set_basic_insert_and_lookup() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashSet<String> set;

    TEST(set.empty());
    TEST_EQUAL(set.size(), 0u);
    TEST_EQUAL(set.table_size(), 0u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 0);

    TEST(! set.contains("alpha"s));

    set.insert("alpha"s);
    TEST(! set.empty());
    TEST_EQUAL(set.size(), 1u);
    TEST_EQUAL(set.table_size(), 16u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 1);

    TEST(set.contains("alpha"s));
    TEST(! set.contains("bravo"s));

    set.insert("bravo"s);
    TEST(! set.empty());
    TEST_EQUAL(set.size(), 2u);
    TEST_EQUAL(set.table_size(), 16u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 2);

    TEST(set.contains("alpha"s));
    TEST(set.contains("bravo"s));
    TEST(! set.contains("charlie"s));

    set.insert("charlie"s);
    TEST(! set.empty());
    TEST_EQUAL(set.size(), 3u);
    TEST_EQUAL(set.table_size(), 16u);
    TEST_EQUAL(std::distance(set.begin(), set.end()), 3);

    TEST(set.contains("alpha"s));
    TEST(set.contains("bravo"s));
    TEST(set.contains("charlie"s));
    TEST(! set.contains("delta"s));

    TRY(set.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_set_large_insertions() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashSet<String> set;
    std::string s;
    int i = 1;

    for (; i <= 12; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        set.insert(s);
        TEST_EQUAL(set.size(), size_t(i));
        TEST_EQUAL(set.table_size(), 16u);
        TEST(set.contains(s));
    }

    for (; i <= 24; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        set.insert(s);
        TEST_EQUAL(set.size(), size_t(i));
        TEST_EQUAL(set.table_size(), 32u);
        TEST(set.contains(s));
    }

    for (; i <= 48; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        set.insert(s);
        TEST_EQUAL(set.size(), size_t(i));
        TEST_EQUAL(set.table_size(), 64u);
        TEST(set.contains(s));
    }

    for (; i <= 96; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        set.insert(s);
        TEST_EQUAL(set.size(), size_t(i));
        TEST_EQUAL(set.table_size(), 128u);
        TEST(set.contains(s));
    }

    for (; i <= 192; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        set.insert(s);
        TEST_EQUAL(set.size(), size_t(i));
        TEST_EQUAL(set.table_size(), 256u);
        TEST(set.contains(s));
    }

    for (; i <= 384; ++i)
    {
        char c = 'a' + i % 26;
        s += c;
        set.insert(s);
        TEST_EQUAL(set.size(), size_t(i));
        TEST_EQUAL(set.table_size(), 512u);
        TEST(set.contains(s));
    }

    TRY(set.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_set_erasure() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashSet<String> set;
    HashSet<String>::const_iterator it;
    size_t n = 0;

    set.insert("alpha"s);
    set.insert("bravo"s);
    set.insert("charlie"s);
    set.insert("delta"s);
    set.insert("echo"s);
    TEST_EQUAL(set.size(), 5u);

    n = set.erase("charlie"s);
    TEST_EQUAL(n, 1);
    TEST_EQUAL(set.size(), 4u);
    it = set.find("charlie"s);
    TEST(it == set.end());
    it = set.find("delta"s);
    TEST(it != set.end());
    TEST_EQUAL(*it, "delta"s);

    set.erase(it);
    TEST_EQUAL(set.size(), 3u);
    it = set.find("delta"s);
    TEST(it == set.end());

    n = set.erase("delta"s);
    TEST_EQUAL(n, 0);
    TEST_EQUAL(set.size(), 3u);

    TRY(set.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_set_initializer_list() {

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

    HashSet<String> set {
        "alpha"s,
        "bravo"s,
        "charlie"s,
    };

    TEST_EQUAL(set.size(), 3u);
    TEST(set.contains("alpha"s));
    TEST(set.contains("bravo"s));
    TEST(set.contains("charlie"s));
    TEST(! set.contains("delta"s));

    set = {
        "delta"s,
        "echo"s,
        "foxtrot"s,
        "golf"s,
    };

    TEST_EQUAL(set.size(), 4u);
    TEST(! set.contains("charlie"s));
    TEST(set.contains("delta"s));
    TEST(set.contains("echo"s));
    TEST(set.contains("foxtrot"s));
    TEST(set.contains("golf"s));
    TEST(! set.contains("hotel"s));

    set = {};

    TEST(set.empty());
    TEST_EQUAL(set.size(), 0u);

    TRY(set.clear());

    TEST_EQUAL(Integer::count(), 0);
    TEST_EQUAL(String::count(), 0);

}

void test_core_hash_map_string_set_emplace() {

    HashSet<std::string> set;

    set.emplace(1, 'a');
    set.emplace(2, 'b');
    set.emplace(3, 'c');

    TEST_EQUAL(set.size(), 3u);
    TEST(set.contains("a"s));
    TEST(set.contains("bb"s));
    TEST(set.contains("ccc"s));

}
