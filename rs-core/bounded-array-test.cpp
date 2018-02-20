#include "rs-core/bounded-array.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace RS;

void test_core_bounded_array_construction() {

    using IBA = BoundedArray<int, 10>;

    IBA a;

    TEST(a.empty());
    TEST_EQUAL(a.size(), 0);
    TEST_EQUAL(a.capacity(), 10);
    TEST_THROW(a.at(0), std::out_of_range);
    TEST_EQUAL(to_str(a), "[]");

    IBA b(5, 42);

    TEST(! b.empty());
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(b.capacity(), 10);
    TEST_EQUAL(b[0], 42);
    TEST_EQUAL(b[4], 42);
    TEST_EQUAL(b.at(0), 42);
    TEST_EQUAL(b.at(4), 42);
    TEST_THROW(b.at(5), std::out_of_range);
    TEST_EQUAL(to_str(b), "[42,42,42,42,42]");

    TRY(a = b);
    TEST(! a.empty());
    TEST(! b.empty());
    TEST_EQUAL(a.size(), 5);
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(b.capacity(), 10);
    TEST_EQUAL(a[0], 42);
    TEST_EQUAL(a[4], 42);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42]");

    TRY(a = std::move(b));
    TEST(! a.empty());
    TEST(! b.empty());
    TEST_EQUAL(a.size(), 5);
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(b.capacity(), 10);
    TEST_EQUAL(a[0], 42);
    TEST_EQUAL(a[4], 42);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42]");

    IBA c(10, 42);

    TEST(! c.empty());
    TEST_EQUAL(c.size(), 10);
    TEST_EQUAL(c.capacity(), 10);
    TEST_EQUAL(c[0], 42);
    TEST_EQUAL(c[9], 42);
    TEST_EQUAL(to_str(c), "[42,42,42,42,42,42,42,42,42,42]");

    TRY(a = c);
    TEST(! a.empty());
    TEST(! c.empty());
    TEST_EQUAL(a.size(), 10);
    TEST_EQUAL(c.size(), 10);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(c.capacity(), 10);
    TEST_EQUAL(a[0], 42);
    TEST_EQUAL(a[4], 42);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42,42,42,42,42,42]");

    TRY(a = std::move(c));
    TEST(! a.empty());
    TEST(! c.empty());
    TEST_EQUAL(a.size(), 10);
    TEST_EQUAL(c.size(), 10);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(c.capacity(), 10);
    TEST_EQUAL(a[0], 42);
    TEST_EQUAL(a[4], 42);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42,42,42,42,42,42]");

    TRY((a = IBA{1,2,3,4,5}));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 5);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(a[0], 1);
    TEST_EQUAL(a[4], 5);
    TEST_EQUAL(to_str(a), "[1,2,3,4,5]");

    TRY((a = IBA{1,2,3,4,5,6,7,8,9,10}));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 10);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(a[0], 1);
    TEST_EQUAL(a[9], 10);
    TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8,9,10]");

    TRY((a = {1,2,3,4,5}));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 5);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(a[0], 1);
    TEST_EQUAL(a[4], 5);
    TEST_EQUAL(to_str(a), "[1,2,3,4,5]");

    TRY((a = {1,2,3,4,5,6,7,8,9,10}));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 10);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(a[0], 1);
    TEST_EQUAL(a[9], 10);
    TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8,9,10]");

    std::vector<int> v = {1,2,3,4,5};
    TRY(a = IBA(v.begin(), v.end()));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 5);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(a[0], 1);
    TEST_EQUAL(a[4], 5);
    TEST_EQUAL(to_str(a), "[1,2,3,4,5]");

    v = {1,2,3,4,5,6,7,8,9,10};
    TRY(a = IBA(v.begin(), v.end()));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 10);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(a[0], 1);
    TEST_EQUAL(a[9], 10);
    TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8,9,10]");

    TRY(a.clear());
    TEST(a.empty());
    TEST_EQUAL(a.size(), 0);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(to_str(a), "[]");

}

void test_core_bounded_array_capacity() {

    using IBA = BoundedArray<int, 10>;

    IBA a;

    TRY(a.resize(5, 42));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 5);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42]");

    TEST_THROW(a.resize(20), std::length_error);
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 5);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42]");

    TRY(a.resize(10));
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 10);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42,0,0,0,0,0]");

    TEST_THROW(a.push_back(99), std::length_error);
    TEST(! a.empty());
    TEST_EQUAL(a.size(), 10);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(to_str(a), "[42,42,42,42,42,0,0,0,0,0]");

    TRY(a.clear());
    TEST(a.empty());
    TEST_EQUAL(a.size(), 0);
    TEST_EQUAL(a.capacity(), 10);
    TEST_EQUAL(to_str(a), "[]");

}

void test_core_bounded_array_insertion() {

    using IBA = BoundedArray<int, 10>;
    using SBA = BoundedArray<Ustring, 10>;

    IBA a;
    SBA b;
    IBA::iterator ai;
    SBA::iterator bi;
    std::vector<int> v;

    for (size_t i = 1; i <= 10; ++i) {
        TRY(a.push_back(42));
        TEST_EQUAL(a.size(), i);
        TEST_EQUAL(a.capacity(), 10);
        TEST_EQUAL(a[i - 1], 42);
        TEST_EQUAL(to_str(a), "[" + repeat("42", i, ",") + "]");
    }

    for (size_t i = 1; i <= 10; ++i) {
        size_t n = 10 - i;
        TRY(a.pop_back());
        TEST_EQUAL(a.size(), n);
        TEST_EQUAL(a.capacity(), 10);
        if (n)
            TEST_EQUAL(a[n - 1], 42);
        TEST_EQUAL(to_str(a), "[" + repeat("42", n, ",") + "]");
    }

    TEST(a.empty());
    TEST_EQUAL(a.size(), 0);
    TEST_EQUAL(to_str(a), "[]");

    for (size_t i = 1; i <= 10; ++i) {
        TRY(b.emplace_back(3, 'a'));
        TEST_EQUAL(b.size(), i);
        TEST_EQUAL(b.capacity(), 10);
        TEST_EQUAL(b[i - 1], "aaa");
        TEST_EQUAL(to_str(b), "[" + repeat("aaa", i, ",") + "]");
    }

    for (size_t i = 1; i <= 10; ++i) {
        size_t n = 10 - i;
        TRY(b.pop_back());
        TEST_EQUAL(b.size(), n);
        TEST_EQUAL(b.capacity(), 10);
        if (n)
            TEST_EQUAL(b[n - 1], "aaa");
        TEST_EQUAL(to_str(b), "[" + repeat("aaa", n, ",") + "]");
    }

    TEST(b.empty());
    TEST_EQUAL(b.size(), 0);
    TEST_EQUAL(to_str(b), "[]");

    TRY((a = {1,2,3}));
    TEST_EQUAL(a.size(), 3);
    TEST_EQUAL(to_str(a), "[1,2,3]");

    TRY(ai = a.insert(a.begin() + 3, 10));                       TEST_EQUAL(a.size(), 4);   TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,10]");
    TRY(ai = a.insert(a.begin() + 3, 20));                       TEST_EQUAL(a.size(), 5);   TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,20,10]");
    TRY(ai = a.insert(a.begin() + 3, 30));                       TEST_EQUAL(a.size(), 6);   TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,30,20,10]");
    TRY(ai = a.insert(a.begin() + 3, 40));                       TEST_EQUAL(a.size(), 7);   TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,40,30,20,10]");
    TRY(ai = a.insert(a.begin() + 3, 50));                       TEST_EQUAL(a.size(), 8);   TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,50,40,30,20,10]");
    TRY(ai = a.insert(a.begin() + 3, 60));                       TEST_EQUAL(a.size(), 9);   TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,60,50,40,30,20,10]");
    TRY(ai = a.insert(a.begin() + 3, 70));                       TEST_EQUAL(a.size(), 10);  TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,70,60,50,40,30,20,10]");
    TEST_THROW(a.insert(a.begin() + 3, 80), std::length_error);  TEST_EQUAL(a.size(), 10);  TEST_EQUAL(ai - a.begin(), 3);  TEST_EQUAL(to_str(a), "[1,2,3,70,60,50,40,30,20,10]");

    TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 9);  TEST_EQUAL(to_str(a), "[1,2,3,60,50,40,30,20,10]");
    TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,3,50,40,30,20,10]");
    TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 7);  TEST_EQUAL(to_str(a), "[1,2,3,40,30,20,10]");
    TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 6);  TEST_EQUAL(to_str(a), "[1,2,3,30,20,10]");
    TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 5);  TEST_EQUAL(to_str(a), "[1,2,3,20,10]");
    TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,10]");
    TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 3);  TEST_EQUAL(to_str(a), "[1,2,3]");

    TRY((b = {"x","y","z"}));
    TEST_EQUAL(b.size(), 3);
    TEST_EQUAL(to_str(b), "[x,y,z]");

    TRY(bi = b.emplace(b.begin() + 3, 3, 'a'));                       TEST_EQUAL(b.size(), 4);   TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,aaa]");
    TRY(bi = b.emplace(b.begin() + 3, 3, 'b'));                       TEST_EQUAL(b.size(), 5);   TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,bbb,aaa]");
    TRY(bi = b.emplace(b.begin() + 3, 3, 'c'));                       TEST_EQUAL(b.size(), 6);   TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,ccc,bbb,aaa]");
    TRY(bi = b.emplace(b.begin() + 3, 3, 'd'));                       TEST_EQUAL(b.size(), 7);   TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,ddd,ccc,bbb,aaa]");
    TRY(bi = b.emplace(b.begin() + 3, 3, 'e'));                       TEST_EQUAL(b.size(), 8);   TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,eee,ddd,ccc,bbb,aaa]");
    TRY(bi = b.emplace(b.begin() + 3, 3, 'f'));                       TEST_EQUAL(b.size(), 9);   TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,fff,eee,ddd,ccc,bbb,aaa]");
    TRY(bi = b.emplace(b.begin() + 3, 3, 'g'));                       TEST_EQUAL(b.size(), 10);  TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,ggg,fff,eee,ddd,ccc,bbb,aaa]");
    TEST_THROW(b.emplace(b.begin() + 3, 3, 'h'), std::length_error);  TEST_EQUAL(b.size(), 10);  TEST_EQUAL(bi - b.begin(), 3);  TEST_EQUAL(to_str(b), "[x,y,z,ggg,fff,eee,ddd,ccc,bbb,aaa]");

    TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 9);  TEST_EQUAL(to_str(b), "[x,y,z,fff,eee,ddd,ccc,bbb,aaa]");
    TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 8);  TEST_EQUAL(to_str(b), "[x,y,z,eee,ddd,ccc,bbb,aaa]");
    TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 7);  TEST_EQUAL(to_str(b), "[x,y,z,ddd,ccc,bbb,aaa]");
    TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 6);  TEST_EQUAL(to_str(b), "[x,y,z,ccc,bbb,aaa]");
    TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 5);  TEST_EQUAL(to_str(b), "[x,y,z,bbb,aaa]");
    TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 4);  TEST_EQUAL(to_str(b), "[x,y,z,aaa]");
    TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 3);  TEST_EQUAL(to_str(b), "[x,y,z]");

    TRY(a.clear());
    TEST(a.empty());

    v = {1,2,3,4};     TRY(a.insert(a.begin(), v.begin(), v.end()));                                TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    v = {5,6,7,8};     TRY(a.insert(a.begin() + 2, v.begin(), v.end()));                            TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,5,6,7,8,3,4]");
    v = {9,10,11,12};  TEST_THROW(a.insert(a.begin() + 4, v.begin(), v.end()), std::length_error);  TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,5,6,7,8,3,4]");

    TRY(a.erase(a.begin() + 2, a.begin() + 6));  TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    TRY(a.erase(a.begin(), a.begin() + 4));      TEST_EQUAL(a.size(), 0);  TEST_EQUAL(to_str(a), "[]");

    TEST(a.empty());

    v = {1,2,3,4};     TRY(a.insert(a.begin(), v.begin(), v.end()));                            TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    v = {5,6,7,8};     TRY(a.insert(a.begin(), v.begin(), v.end()));                            TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[5,6,7,8,1,2,3,4]");
    v = {9,10,11,12};  TEST_THROW(a.insert(a.begin(), v.begin(), v.end()), std::length_error);  TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[5,6,7,8,1,2,3,4]");

    TRY(a.erase(a.begin(), a.begin() + 4));  TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    TRY(a.erase(a.begin(), a.begin() + 4));  TEST_EQUAL(a.size(), 0);  TEST_EQUAL(to_str(a), "[]");

    TEST(a.empty());

    v = {1,2,3,4};     TRY(a.insert(a.end(), v.begin(), v.end()));                            TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    v = {5,6,7,8};     TRY(a.insert(a.end(), v.begin(), v.end()));                            TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8]");
    v = {9,10,11,12};  TEST_THROW(a.insert(a.end(), v.begin(), v.end()), std::length_error);  TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8]");

    TRY(a.erase(a.end() - 4, a.end()));  TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    TRY(a.erase(a.end() - 4, a.end()));  TEST_EQUAL(a.size(), 0);  TEST_EQUAL(to_str(a), "[]");

    TEST(a.empty());

    v = {1,2,3,4};     TRY(a.append(v));                            TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    v = {5,6,7,8};     TRY(a.append(v));                            TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8]");
    v = {9,10,11,12};  TEST_THROW(a.append(v), std::length_error);  TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8]");

    TRY(a.clear());
    TEST(a.empty());

    v = {1,2,3,4};     TRY(a.append(std::move(v)));                            TEST_EQUAL(a.size(), 4);  TEST_EQUAL(to_str(a), "[1,2,3,4]");
    v = {5,6,7,8};     TRY(a.append(std::move(v)));                            TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8]");
    v = {9,10,11,12};  TEST_THROW(a.append(std::move(v)), std::length_error);  TEST_EQUAL(a.size(), 8);  TEST_EQUAL(to_str(a), "[1,2,3,4,5,6,7,8]");

}

void test_core_bounded_array_accounting() {

    using AI = Accountable<int>;
    using BA = BoundedArray<AI, 10>;

    const std::vector<int> v5 = {1,2,3,4,5};
    const std::vector<int> v10 = {1,2,3,4,5,6,7,8,9,10};

    BA a;                                                                          TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    BA b(5, 42);                                                                   TEST_EQUAL(b.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a = b);                                                                    TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 10);
    TRY(a = std::move(b));                                                         TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 10);
    TRY(b.clear());                                                                TEST_EQUAL(b.size(), 0);   TEST_EQUAL(AI::count(), 5);
    BA c(10, 42);                                                                  TEST_EQUAL(c.size(), 10);  TEST_EQUAL(AI::count(), 15);
    TRY(a = c);                                                                    TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 20);
    TRY(a = std::move(c));                                                         TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 20);
    TRY(c.clear());                                                                TEST_EQUAL(c.size(), 0);   TEST_EQUAL(AI::count(), 10);
    TRY((a = BA{1,2,3,4,5}));                                                      TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY((a = BA{1,2,3,4,5,6,7,8,9,10}));                                           TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY((a = {1,2,3,4,5}));                                                        TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY((a = {1,2,3,4,5,6,7,8,9,10}));                                             TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a = BA(v5.begin(), v5.end()));                                             TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a = BA(v10.begin(), v10.end()));                                           TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a.clear());                                                                TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    TRY(a.resize(5, 42));                                                          TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.resize(10));                                                             TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TEST_THROW(a.resize(20), std::length_error);                                   TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a.resize(4));                                                              TEST_EQUAL(a.size(), 4);   TEST_EQUAL(AI::count(), 4);
    TRY(a.clear());                                                                TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 1);   TEST_EQUAL(AI::count(), 1);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 2);   TEST_EQUAL(AI::count(), 2);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 3);   TEST_EQUAL(AI::count(), 3);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 4);   TEST_EQUAL(AI::count(), 4);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 6);   TEST_EQUAL(AI::count(), 6);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 7);   TEST_EQUAL(AI::count(), 7);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 8);   TEST_EQUAL(AI::count(), 8);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 9);   TEST_EQUAL(AI::count(), 9);
    TRY(a.push_back(42));                                                          TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TEST_THROW(a.push_back(42), std::length_error);                                TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 9);   TEST_EQUAL(AI::count(), 9);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 8);   TEST_EQUAL(AI::count(), 8);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 7);   TEST_EQUAL(AI::count(), 7);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 6);   TEST_EQUAL(AI::count(), 6);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 4);   TEST_EQUAL(AI::count(), 4);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 3);   TEST_EQUAL(AI::count(), 3);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 2);   TEST_EQUAL(AI::count(), 2);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 1);   TEST_EQUAL(AI::count(), 1);
    TRY(a.pop_back());                                                             TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    TRY((a = {1,2,3}));                                                            TEST_EQUAL(a.size(), 3);   TEST_EQUAL(AI::count(), 3);
    TRY(a.insert(a.begin() + 3, 42));                                              TEST_EQUAL(a.size(), 4);   TEST_EQUAL(AI::count(), 4);
    TRY(a.insert(a.begin() + 3, 42));                                              TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.insert(a.begin() + 3, 42));                                              TEST_EQUAL(a.size(), 6);   TEST_EQUAL(AI::count(), 6);
    TRY(a.insert(a.begin() + 3, 42));                                              TEST_EQUAL(a.size(), 7);   TEST_EQUAL(AI::count(), 7);
    TRY(a.insert(a.begin() + 3, 42));                                              TEST_EQUAL(a.size(), 8);   TEST_EQUAL(AI::count(), 8);
    TRY(a.insert(a.begin() + 3, 42));                                              TEST_EQUAL(a.size(), 9);   TEST_EQUAL(AI::count(), 9);
    TRY(a.insert(a.begin() + 3, 42));                                              TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TEST_THROW(a.insert(a.begin() + 3, 42), std::length_error);                    TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a.erase(a.begin() + 3));                                                   TEST_EQUAL(a.size(), 9);   TEST_EQUAL(AI::count(), 9);
    TRY(a.erase(a.begin() + 3));                                                   TEST_EQUAL(a.size(), 8);   TEST_EQUAL(AI::count(), 8);
    TRY(a.erase(a.begin() + 3));                                                   TEST_EQUAL(a.size(), 7);   TEST_EQUAL(AI::count(), 7);
    TRY(a.erase(a.begin() + 3));                                                   TEST_EQUAL(a.size(), 6);   TEST_EQUAL(AI::count(), 6);
    TRY(a.erase(a.begin() + 3));                                                   TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.erase(a.begin() + 3));                                                   TEST_EQUAL(a.size(), 4);   TEST_EQUAL(AI::count(), 4);
    TRY(a.erase(a.begin() + 3));                                                   TEST_EQUAL(a.size(), 3);   TEST_EQUAL(AI::count(), 3);
    TRY(a.clear());                                                                TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    TRY(a.insert(a.begin(), v5.begin(), v5.end()));                                TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.insert(a.begin(), v5.begin(), v5.end()));                                TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TEST_THROW(a.insert(a.begin(), v5.begin(), v5.end()), std::length_error);      TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a.erase(a.begin(), a.begin() + 5));                                        TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.erase(a.begin(), a.begin() + 5));                                        TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    TRY(a.insert(a.begin(), v5.begin(), v5.end()));                                TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.insert(a.begin() + 1, v5.begin(), v5.end()));                            TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TEST_THROW(a.insert(a.begin() + 2, v5.begin(), v5.end()), std::length_error);  TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a.erase(a.begin() + 1, a.begin() + 6));                                    TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.erase(a.begin(), a.begin() + 5));                                        TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    TRY(a.insert(a.end(), v5.begin(), v5.end()));                                  TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.insert(a.end(), v5.begin(), v5.end()));                                  TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TEST_THROW(a.insert(a.end(), v5.begin(), v5.end()), std::length_error);        TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TRY(a.erase(a.end() - 5, a.end()));                                            TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.erase(a.end() - 5, a.end()));                                            TEST_EQUAL(a.size(), 0);   TEST_EQUAL(AI::count(), 0);
    TRY(a.append(v5));                                                             TEST_EQUAL(a.size(), 5);   TEST_EQUAL(AI::count(), 5);
    TRY(a.append(std::move(v5)));                                                  TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);
    TEST_THROW(a.append(std::move(v5)), std::length_error);                        TEST_EQUAL(a.size(), 10);  TEST_EQUAL(AI::count(), 10);

}

void test_core_bounded_array_keys() {

    using IBA = BoundedArray<int, 10>;
    using Omap = std::map<IBA, Ustring>;
    using Umap = std::unordered_map<IBA, Ustring>;

    IBA a;
    Omap omap;
    Umap umap;
    Ustring s;

    TEST_EQUAL(to_str(omap), "{}");
    TEST_EQUAL(to_str(umap), "{}");

    for (int i = 5; i >= 1; --i) {
        TRY(a.clear());
        for (int j = i; j <= 2 * i; ++j)
            TRY(a.push_back(j));
        TEST_EQUAL(a.size(), size_t(i + 1));
        s.assign(i, char('@' + i));
        TRY(omap[a] = s);
        TRY(umap[a] = s);
    }

    TEST_EQUAL(omap.size(), 5);
    TEST_EQUAL(umap.size(), 5);

    TEST_EQUAL(to_str(omap),
        "{[1,2]:A,"
        "[2,3,4]:BB,"
        "[3,4,5,6]:CCC,"
        "[4,5,6,7,8]:DDDD,"
        "[5,6,7,8,9,10]:EEEEE}"
    );

}
