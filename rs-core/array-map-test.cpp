#include "rs-core/array-map.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <complex>
#include <functional>
#include <ostream>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using namespace RS;

void test_core_array_map_array_map() {

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
    TEST(map.insert(map.cend(), {10, "goodbye"}).second);
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

void test_core_array_map_array_set() {

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
    TEST(set.insert(set.cend(), "goodbye").second);
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

namespace {

    struct EqualityType:
    public EqualityComparable<EqualityType> {
        std::string str;
        EqualityType(const std::string& s = {}): str(s) {}
        size_t hash() const noexcept { return std::hash<std::string>()(str); }
        friend bool operator==(const EqualityType& lhs, const EqualityType& rhs) noexcept { return lhs.str == rhs.str; }
        friend std::ostream& operator<<(std::ostream& out, const EqualityType& t) { return out << t.str; }
    };

    struct HashedType:
    public EqualityComparable<HashedType> {
        std::string str;
        HashedType(const std::string& s = {}): str(s) {}
        size_t hash() const noexcept { return std::hash<std::string>()(str); }
        friend bool operator==(const HashedType& lhs, const HashedType& rhs) noexcept { return lhs.str == rhs.str; }
        friend std::ostream& operator<<(std::ostream& out, const HashedType& t) { return out << t.str; }
    };

    struct OrderedType:
    public LessThanComparable<OrderedType> {
        std::string str;
        OrderedType(const std::string& s = {}): str(s) {}
        friend RS_ATTR_UNUSED bool operator==(const OrderedType& lhs, const OrderedType& rhs) noexcept { return lhs.str == rhs.str; }
        friend bool operator<(const OrderedType& lhs, const OrderedType& rhs) noexcept { return lhs.str < rhs.str; }
        friend std::ostream& operator<<(std::ostream& out, const OrderedType& t) { return out << t.str; }
    };

}

RS_DEFINE_STD_HASH(HashedType);

void test_core_array_map_general_associative_containers() {

    { using T1 = BasicSet<int>;                        using T2 = std::unordered_set<int>;          TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<int, true>;                  using T2 = std::set<int>;                    TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<std::string>;                using T2 = std::unordered_set<std::string>;  TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<std::string, true>;          using T2 = std::set<std::string>;            TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<std::vector<int>>;           using T2 = std::set<std::vector<int>>;       TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<std::vector<int>, true>;     using T2 = std::set<std::vector<int>>;       TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<std::complex<float>>;        using T2 = ArraySet<std::complex<float>>;    TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<std::complex<float>, true>;  using T2 = ArraySet<std::complex<float>>;    TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<EqualityType>;               using T2 = ArraySet<EqualityType>;           TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<EqualityType, true>;         using T2 = ArraySet<EqualityType>;           TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<HashedType>;                 using T2 = std::unordered_set<HashedType>;   TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<HashedType, true>;           using T2 = std::unordered_set<HashedType>;   TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<OrderedType>;                using T2 = std::set<OrderedType>;            TEST_TYPE(T1, T2); }
    { using T1 = BasicSet<OrderedType, true>;          using T2 = std::set<OrderedType>;            TEST_TYPE(T1, T2); }

    { using T1 = BasicMap<int, long>;                        using T2 = std::unordered_map<int, long>;          TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<int, long, true>;                  using T2 = std::map<int, long>;                    TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<std::string, long>;                using T2 = std::unordered_map<std::string, long>;  TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<std::string, long, true>;          using T2 = std::map<std::string, long>;            TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<std::vector<int>, long>;           using T2 = std::map<std::vector<int>, long>;       TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<std::vector<int>, long, true>;     using T2 = std::map<std::vector<int>, long>;       TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<std::complex<float>, long>;        using T2 = ArrayMap<std::complex<float>, long>;    TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<std::complex<float>, long, true>;  using T2 = ArrayMap<std::complex<float>, long>;    TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<EqualityType, long>;               using T2 = ArrayMap<EqualityType, long>;           TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<EqualityType, long, true>;         using T2 = ArrayMap<EqualityType, long>;           TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<HashedType, long>;                 using T2 = std::unordered_map<HashedType, long>;   TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<HashedType, long, true>;           using T2 = std::unordered_map<HashedType, long>;   TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<OrderedType, long>;                using T2 = std::map<OrderedType, long>;            TEST_TYPE(T1, T2); }
    { using T1 = BasicMap<OrderedType, long, true>;          using T2 = std::map<OrderedType, long>;            TEST_TYPE(T1, T2); }

}
