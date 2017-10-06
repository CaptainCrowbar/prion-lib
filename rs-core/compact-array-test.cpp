#include "rs-core/compact-array.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <map>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

using namespace RS;

namespace {

    void check_compact_array_construction() {

        using ICA = CompactArray<int, 5>;

        ICA a;
        U8string s;

        TEST(a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 0);
        TEST_EQUAL(a.capacity(), 5);
        TEST_THROW(a.at(0), std::out_of_range);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[]");

        ICA b(5, 42);

        TEST(! b.empty());
        TEST(b.is_compact());
        TEST_EQUAL(b.size(), 5);
        TEST_EQUAL(b.capacity(), 5);
        TEST_EQUAL(b[0], 42);
        TEST_EQUAL(b[4], 42);
        TEST_EQUAL(b.at(0), 42);
        TEST_EQUAL(b.at(4), 42);
        TEST_THROW(b.at(5), std::out_of_range);
        TRY(s = to_str(b));
        TEST_EQUAL(s, "[42,42,42,42,42]");

        TRY(a = b);
        TEST(! a.empty());
        TEST(! b.empty());
        TEST(a.is_compact());
        TEST(b.is_compact());
        TEST_EQUAL(a.size(), 5);
        TEST_EQUAL(b.size(), 5);
        TEST_EQUAL(a.capacity(), 5);
        TEST_EQUAL(b.capacity(), 5);
        TEST_EQUAL(a[0], 42);
        TEST_EQUAL(a[4], 42);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42,42]");

        TRY(a = std::move(b));
        TEST(! a.empty());
        TEST(b.empty());
        TEST(a.is_compact());
        TEST(b.is_compact());
        TEST_EQUAL(a.size(), 5);
        TEST_EQUAL(b.size(), 0);
        TEST_EQUAL(a.capacity(), 5);
        TEST_EQUAL(b.capacity(), 5);
        TEST_EQUAL(a[0], 42);
        TEST_EQUAL(a[4], 42);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42,42]");

        ICA c(10, 42);

        TEST(! c.empty());
        TEST(! c.is_compact());
        TEST_EQUAL(c.size(), 10);
        TEST_EQUAL(c.capacity(), 16);
        TEST_EQUAL(c[0], 42);
        TEST_EQUAL(c[9], 42);
        TRY(s = to_str(c));
        TEST_EQUAL(s, "[42,42,42,42,42,42,42,42,42,42]");

        TRY(a = c);
        TEST(! a.empty());
        TEST(! c.empty());
        TEST(! a.is_compact());
        TEST(! c.is_compact());
        TEST_EQUAL(a.size(), 10);
        TEST_EQUAL(c.size(), 10);
        TEST_EQUAL(a.capacity(), 16);
        TEST_EQUAL(c.capacity(), 16);
        TEST_EQUAL(a[0], 42);
        TEST_EQUAL(a[4], 42);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42,42,42,42,42,42,42]");

        TRY(a = std::move(c));
        TEST(! a.empty());
        TEST(c.empty());
        TEST(! a.is_compact());
        TEST(c.is_compact());
        TEST_EQUAL(a.size(), 10);
        TEST_EQUAL(c.size(), 0);
        TEST_EQUAL(a.capacity(), 16);
        TEST_EQUAL(c.capacity(), 5);
        TEST_EQUAL(a[0], 42);
        TEST_EQUAL(a[4], 42);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42,42,42,42,42,42,42]");

        TRY((a = ICA{1,2,3,4,5}));
        TEST(! a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 5);
        TEST_EQUAL(a.capacity(), 5);
        TEST_EQUAL(a[0], 1);
        TEST_EQUAL(a[4], 5);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[1,2,3,4,5]");

        TRY((a = ICA{1,2,3,4,5,6,7,8,9,10}));
        TEST(! a.empty());
        TEST(! a.is_compact());
        TEST_EQUAL(a.size(), 10);
        TEST_EQUAL(a.capacity(), 16);
        TEST_EQUAL(a[0], 1);
        TEST_EQUAL(a[9], 10);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10]");

        TRY((a = {1,2,3,4,5}));
        TEST(! a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 5);
        TEST_EQUAL(a.capacity(), 5);
        TEST_EQUAL(a[0], 1);
        TEST_EQUAL(a[4], 5);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[1,2,3,4,5]");

        TRY((a = {1,2,3,4,5,6,7,8,9,10}));
        TEST(! a.empty());
        TEST(! a.is_compact());
        TEST_EQUAL(a.size(), 10);
        TEST_EQUAL(a.capacity(), 16);
        TEST_EQUAL(a[0], 1);
        TEST_EQUAL(a[9], 10);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10]");

        std::vector<int> v = {1,2,3,4,5};
        TRY(a = ICA(v.begin(), v.end()));
        TEST(! a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 5);
        TEST_EQUAL(a.capacity(), 5);
        TEST_EQUAL(a[0], 1);
        TEST_EQUAL(a[4], 5);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[1,2,3,4,5]");

        v = {1,2,3,4,5,6,7,8,9,10};
        TRY(a = ICA(v.begin(), v.end()));
        TEST(! a.empty());
        TEST(! a.is_compact());
        TEST_EQUAL(a.size(), 10);
        TEST_EQUAL(a.capacity(), 16);
        TEST_EQUAL(a[0], 1);
        TEST_EQUAL(a[9], 10);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10]");

        TRY(a.clear());
        TEST(a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 0);
        TEST_EQUAL(a.capacity(), 5);

    }

    void check_compact_array_capacity() {

        using ICA = CompactArray<int, 5>;

        ICA a;
        U8string s;

        TRY(a.resize(5, 42));
        TEST(! a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 5);
        TEST_EQUAL(a.capacity(), 5);
        TEST_EQUAL(a[0], 42);
        TEST_EQUAL(a[4], 42);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42,42]");

        TRY(a.resize(10));
        TEST(! a.empty());
        TEST(! a.is_compact());
        TEST_EQUAL(a.size(), 10);
        TEST_EQUAL(a.capacity(), 16);
        TEST_EQUAL(a[0], 42);
        TEST_EQUAL(a[9], 0);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42,42,0,0,0,0,0]");

        TRY(a.shrink_to_fit());
        TEST(! a.empty());
        TEST(! a.is_compact());
        TEST_EQUAL(a.size(), 10);
        TEST_EQUAL(a.capacity(), 10);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42,42,0,0,0,0,0]");

        TRY(a.resize(4));
        TEST(! a.empty());
        TEST(! a.is_compact());
        TEST_EQUAL(a.size(), 4);
        TEST_EQUAL(a.capacity(), 10);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42]");

        TRY(a.shrink_to_fit());
        TEST(! a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 4);
        TEST_EQUAL(a.capacity(), 5);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[42,42,42,42]");

        TRY(a.clear());
        TEST(a.empty());
        TEST(a.is_compact());
        TEST_EQUAL(a.size(), 0);
        TEST_EQUAL(a.capacity(), 5);

    }

    void check_compact_array_insertion() {

        using ICA = CompactArray<int, 5>;
        using SCA = CompactArray<U8string, 5>;

        ICA a;
        SCA b;
        ICA::iterator ai;
        SCA::iterator bi;
        U8string s;
        std::vector<int> v;

        for (size_t i = 1; i <= 16; ++i) {
            TRY(a.push_back(42));
            TEST_EQUAL(a.size(), i);
            TEST_EQUAL(a.capacity(), i <= 5 ? 5 : i <= 8 ? 8 : 16);
            TEST_EQUAL(a[i - 1], 42);
            TRY(s = to_str(a));
            TEST_EQUAL(s, "[" + repeat("42", i, ",") + "]");
        }

        for (size_t i = 1; i <= 16; ++i) {
            size_t n = 16 - i;
            TRY(a.pop_back());
            TEST_EQUAL(a.size(), n);
            TEST_EQUAL(a.capacity(), 16);
            if (n)
                TEST_EQUAL(a[n - 1], 42);
            TRY(s = to_str(a));
            TEST_EQUAL(s, "[" + repeat("42", n, ",") + "]");
        }

        TEST(a.empty());
        TEST_EQUAL(a.size(), 0);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[]");

        for (size_t i = 1; i <= 16; ++i) {
            TRY(b.emplace_back(3, 'a'));
            TEST_EQUAL(b.size(), i);
            TEST_EQUAL(b.capacity(), i <= 5 ? 5 : i <= 8 ? 8 : 16);
            TEST_EQUAL(b[i - 1], "aaa");
            TRY(s = to_str(b));
            TEST_EQUAL(s, "[" + repeat("aaa", i, ",") + "]");
        }

        for (size_t i = 1; i <= 16; ++i) {
            size_t n = 16 - i;
            TRY(b.pop_back());
            TEST_EQUAL(b.size(), n);
            TEST_EQUAL(b.capacity(), 16);
            if (n)
                TEST_EQUAL(b[n - 1], "aaa");
            TRY(s = to_str(b));
            TEST_EQUAL(s, "[" + repeat("aaa", n, ",") + "]");
        }

        TEST(b.empty());
        TEST_EQUAL(b.size(), 0);
        TRY(s = to_str(b));
        TEST_EQUAL(s, "[]");

        TRY((a = {1,2,3}));
        TEST_EQUAL(a.size(), 3);
        TRY(s = to_str(a));
        TEST_EQUAL(s, "[1,2,3]");

        TRY(ai = a.insert(a.begin() + 3, 10));  TEST_EQUAL(a.size(), 4);   TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,10]");
        TRY(ai = a.insert(a.begin() + 3, 20));  TEST_EQUAL(a.size(), 5);   TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,20,10]");
        TRY(ai = a.insert(a.begin() + 3, 30));  TEST_EQUAL(a.size(), 6);   TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,30,20,10]");
        TRY(ai = a.insert(a.begin() + 3, 40));  TEST_EQUAL(a.size(), 7);   TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,40,30,20,10]");
        TRY(ai = a.insert(a.begin() + 3, 50));  TEST_EQUAL(a.size(), 8);   TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,50,40,30,20,10]");
        TRY(ai = a.insert(a.begin() + 3, 60));  TEST_EQUAL(a.size(), 9);   TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,60,50,40,30,20,10]");
        TRY(ai = a.insert(a.begin() + 3, 70));  TEST_EQUAL(a.size(), 10);  TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,70,60,50,40,30,20,10]");
        TRY(ai = a.insert(a.begin() + 3, 80));  TEST_EQUAL(a.size(), 11);  TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,80,70,60,50,40,30,20,10]");
        TRY(ai = a.insert(a.begin() + 3, 90));  TEST_EQUAL(a.size(), 12);  TEST_EQUAL(ai - a.begin(), 3);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,90,80,70,60,50,40,30,20,10]");

        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 11);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,80,70,60,50,40,30,20,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 10);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,70,60,50,40,30,20,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 9);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,60,50,40,30,20,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,50,40,30,20,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 7);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,40,30,20,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 6);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,30,20,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 5);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,20,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,10]");
        TRY(a.erase(a.begin() + 3));  TEST_EQUAL(a.size(), 3);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3]");

        TRY((b = {"x","y","z"}));
        TEST_EQUAL(b.size(), 3);
        TRY(s = to_str(b));
        TEST_EQUAL(s, "[x,y,z]");

        TRY(bi = b.emplace(b.begin() + 3, 3, 'a'));  TEST_EQUAL(b.size(), 4);   TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'b'));  TEST_EQUAL(b.size(), 5);   TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,bbb,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'c'));  TEST_EQUAL(b.size(), 6);   TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,ccc,bbb,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'd'));  TEST_EQUAL(b.size(), 7);   TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,ddd,ccc,bbb,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'e'));  TEST_EQUAL(b.size(), 8);   TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,eee,ddd,ccc,bbb,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'f'));  TEST_EQUAL(b.size(), 9);   TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,fff,eee,ddd,ccc,bbb,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'g'));  TEST_EQUAL(b.size(), 10);  TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,ggg,fff,eee,ddd,ccc,bbb,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'h'));  TEST_EQUAL(b.size(), 11);  TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,hhh,ggg,fff,eee,ddd,ccc,bbb,aaa]");
        TRY(bi = b.emplace(b.begin() + 3, 3, 'i'));  TEST_EQUAL(b.size(), 12);  TEST_EQUAL(bi - b.begin(), 3);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,iii,hhh,ggg,fff,eee,ddd,ccc,bbb,aaa]");

        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 11);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,hhh,ggg,fff,eee,ddd,ccc,bbb,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 10);  TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,ggg,fff,eee,ddd,ccc,bbb,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 9);   TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,fff,eee,ddd,ccc,bbb,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 8);   TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,eee,ddd,ccc,bbb,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 7);   TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,ddd,ccc,bbb,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 6);   TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,ccc,bbb,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 5);   TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,bbb,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 4);   TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z,aaa]");
        TRY(b.erase(b.begin() + 3));  TEST_EQUAL(b.size(), 3);   TRY(s = to_str(b));  TEST_EQUAL(s, "[x,y,z]");

        TRY(a.clear());
        TEST(a.empty());

        v = {1,2,3,4};      TRY(a.insert(a.begin(), v.begin(), v.end()));      TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        v = {5,6,7,8};      TRY(a.insert(a.begin() + 2, v.begin(), v.end()));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,5,6,7,8,3,4]");
        v = {9,10,11,12};   TRY(a.insert(a.begin() + 4, v.begin(), v.end()));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,5,6,9,10,11,12,7,8,3,4]");
        v = {13,14,15,16};  TRY(a.insert(a.begin() + 6, v.begin(), v.end()));  TEST_EQUAL(a.size(), 16);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,5,6,9,10,13,14,15,16,11,12,7,8,3,4]");

        TRY(a.erase(a.begin() + 6, a.begin() + 10));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,5,6,9,10,11,12,7,8,3,4]");
        TRY(a.erase(a.begin() + 4, a.begin() + 8));   TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,5,6,7,8,3,4]");
        TRY(a.erase(a.begin() + 2, a.begin() + 6));   TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(a.erase(a.begin(), a.begin() + 4));       TEST_EQUAL(a.size(), 0);   TRY(s = to_str(a));  TEST_EQUAL(s, "[]");

        TEST(a.empty());

        v = {1,2,3,4};      TRY(a.insert(a.begin(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        v = {5,6,7,8};      TRY(a.insert(a.begin(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[5,6,7,8,1,2,3,4]");
        v = {9,10,11,12};   TRY(a.insert(a.begin(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[9,10,11,12,5,6,7,8,1,2,3,4]");
        v = {13,14,15,16};  TRY(a.insert(a.begin(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 16);  TRY(s = to_str(a));  TEST_EQUAL(s, "[13,14,15,16,9,10,11,12,5,6,7,8,1,2,3,4]");

        TRY(a.erase(a.begin(), a.begin() + 4));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[9,10,11,12,5,6,7,8,1,2,3,4]");
        TRY(a.erase(a.begin(), a.begin() + 4));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[5,6,7,8,1,2,3,4]");
        TRY(a.erase(a.begin(), a.begin() + 4));  TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(a.erase(a.begin(), a.begin() + 4));  TEST_EQUAL(a.size(), 0);   TRY(s = to_str(a));  TEST_EQUAL(s, "[]");

        TEST(a.empty());

        v = {1,2,3,4};      TRY(a.insert(a.end(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        v = {5,6,7,8};      TRY(a.insert(a.end(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8]");
        v = {9,10,11,12};   TRY(a.insert(a.end(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10,11,12]");
        v = {13,14,15,16};  TRY(a.insert(a.end(), v.begin(), v.end()));  TEST_EQUAL(a.size(), 16);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]");

        TRY(a.erase(a.end() - 4, a.end()));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10,11,12]");
        TRY(a.erase(a.end() - 4, a.end()));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8]");
        TRY(a.erase(a.end() - 4, a.end()));  TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        TRY(a.erase(a.end() - 4, a.end()));  TEST_EQUAL(a.size(), 0);   TRY(s = to_str(a));  TEST_EQUAL(s, "[]");

        TEST(a.empty());

        v = {1,2,3,4};      TRY(a.append(v));  TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        v = {5,6,7,8};      TRY(a.append(v));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8]");
        v = {9,10,11,12};   TRY(a.append(v));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10,11,12]");
        v = {13,14,15,16};  TRY(a.append(v));  TEST_EQUAL(a.size(), 16);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]");

        TRY(a.clear());
        TEST(a.empty());

        v = {1,2,3,4};      TRY(a.append(std::move(v)));  TEST_EQUAL(a.size(), 4);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4]");
        v = {5,6,7,8};      TRY(a.append(std::move(v)));  TEST_EQUAL(a.size(), 8);   TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8]");
        v = {9,10,11,12};   TRY(a.append(std::move(v)));  TEST_EQUAL(a.size(), 12);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10,11,12]");
        v = {13,14,15,16};  TRY(a.append(std::move(v)));  TEST_EQUAL(a.size(), 16);  TRY(s = to_str(a));  TEST_EQUAL(s, "[1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]");

    }

    void check_compact_array_accounting() {

        using AI = Accountable<int>;
        using CA = CompactArray<AI, 5>;

        const std::vector<int> v5 = {1,2,3,4,5};
        const std::vector<int> v10 = {1,2,3,4,5,6,7,8,9,10};

        CA a;                                                TEST_EQUAL(AI::count(), 0);
        CA b(5, 42);                                         TEST_EQUAL(AI::count(), 5);
        TRY(a = b);                                          TEST_EQUAL(AI::count(), 10);
        TRY(a = std::move(b));                               TEST_EQUAL(AI::count(), 5);
        CA c(10, 42);                                        TEST_EQUAL(AI::count(), 15);
        TRY(a = c);                                          TEST_EQUAL(AI::count(), 20);
        TRY(a = std::move(c));                               TEST_EQUAL(AI::count(), 10);
        TRY((a = CA{1,2,3,4,5}));                            TEST_EQUAL(AI::count(), 5);
        TRY((a = CA{1,2,3,4,5,6,7,8,9,10}));                 TEST_EQUAL(AI::count(), 10);
        TRY((a = {1,2,3,4,5}));                              TEST_EQUAL(AI::count(), 5);
        TRY((a = {1,2,3,4,5,6,7,8,9,10}));                   TEST_EQUAL(AI::count(), 10);
        TRY(a = CA(v5.begin(), v5.end()));                   TEST_EQUAL(AI::count(), 5);
        TRY(a = CA(v10.begin(), v10.end()));                 TEST_EQUAL(AI::count(), 10);
        TRY(a.clear());                                      TEST_EQUAL(AI::count(), 0);
        TRY(a.resize(5, 42));                                TEST_EQUAL(AI::count(), 5);
        TRY(a.resize(10));                                   TEST_EQUAL(AI::count(), 10);
        TRY(a.shrink_to_fit());                              TEST_EQUAL(AI::count(), 10);
        TRY(a.resize(4));                                    TEST_EQUAL(AI::count(), 4);
        TRY(a.shrink_to_fit());                              TEST_EQUAL(AI::count(), 4);
        TRY(a.clear());                                      TEST_EQUAL(AI::count(), 0);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 1);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 2);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 3);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 4);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 5);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 6);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 7);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 8);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 9);
        TRY(a.push_back(42));                                TEST_EQUAL(AI::count(), 10);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 9);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 8);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 7);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 6);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 5);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 4);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 3);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 2);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 1);
        TRY(a.pop_back());                                   TEST_EQUAL(AI::count(), 0);
        TRY((a = {1,2,3}));                                  TEST_EQUAL(AI::count(), 3);
        TRY(a.insert(a.begin() + 3, 42));                    TEST_EQUAL(AI::count(), 4);
        TRY(a.insert(a.begin() + 3, 42));                    TEST_EQUAL(AI::count(), 5);
        TRY(a.insert(a.begin() + 3, 42));                    TEST_EQUAL(AI::count(), 6);
        TRY(a.insert(a.begin() + 3, 42));                    TEST_EQUAL(AI::count(), 7);
        TRY(a.insert(a.begin() + 3, 42));                    TEST_EQUAL(AI::count(), 8);
        TRY(a.insert(a.begin() + 3, 42));                    TEST_EQUAL(AI::count(), 9);
        TRY(a.insert(a.begin() + 3, 42));                    TEST_EQUAL(AI::count(), 10);
        TRY(a.erase(a.begin() + 3));                         TEST_EQUAL(AI::count(), 9);
        TRY(a.erase(a.begin() + 3));                         TEST_EQUAL(AI::count(), 8);
        TRY(a.erase(a.begin() + 3));                         TEST_EQUAL(AI::count(), 7);
        TRY(a.erase(a.begin() + 3));                         TEST_EQUAL(AI::count(), 6);
        TRY(a.erase(a.begin() + 3));                         TEST_EQUAL(AI::count(), 5);
        TRY(a.erase(a.begin() + 3));                         TEST_EQUAL(AI::count(), 4);
        TRY(a.erase(a.begin() + 3));                         TEST_EQUAL(AI::count(), 3);
        TRY(a.clear());                                      TEST_EQUAL(AI::count(), 0);
        TRY(a.insert(a.begin(), v5.begin(), v5.end()));      TEST_EQUAL(AI::count(), 5);
        TRY(a.insert(a.begin(), v5.begin(), v5.end()));      TEST_EQUAL(AI::count(), 10);
        TRY(a.insert(a.begin(), v5.begin(), v5.end()));      TEST_EQUAL(AI::count(), 15);
        TRY(a.insert(a.begin(), v5.begin(), v5.end()));      TEST_EQUAL(AI::count(), 20);
        TRY(a.erase(a.begin(), a.begin() + 5));              TEST_EQUAL(AI::count(), 15);
        TRY(a.erase(a.begin(), a.begin() + 5));              TEST_EQUAL(AI::count(), 10);
        TRY(a.erase(a.begin(), a.begin() + 5));              TEST_EQUAL(AI::count(), 5);
        TRY(a.erase(a.begin(), a.begin() + 5));              TEST_EQUAL(AI::count(), 0);
        TRY(a.insert(a.begin(), v5.begin(), v5.end()));      TEST_EQUAL(AI::count(), 5);
        TRY(a.insert(a.begin() + 1, v5.begin(), v5.end()));  TEST_EQUAL(AI::count(), 10);
        TRY(a.insert(a.begin() + 2, v5.begin(), v5.end()));  TEST_EQUAL(AI::count(), 15);
        TRY(a.insert(a.begin() + 3, v5.begin(), v5.end()));  TEST_EQUAL(AI::count(), 20);
        TRY(a.erase(a.begin() + 3, a.begin() + 8));          TEST_EQUAL(AI::count(), 15);
        TRY(a.erase(a.begin() + 2, a.begin() + 7));          TEST_EQUAL(AI::count(), 10);
        TRY(a.erase(a.begin() + 1, a.begin() + 6));          TEST_EQUAL(AI::count(), 5);
        TRY(a.erase(a.begin(), a.begin() + 5));              TEST_EQUAL(AI::count(), 0);
        TRY(a.insert(a.end(), v5.begin(), v5.end()));        TEST_EQUAL(AI::count(), 5);
        TRY(a.insert(a.end(), v5.begin(), v5.end()));        TEST_EQUAL(AI::count(), 10);
        TRY(a.insert(a.end(), v5.begin(), v5.end()));        TEST_EQUAL(AI::count(), 15);
        TRY(a.insert(a.end(), v5.begin(), v5.end()));        TEST_EQUAL(AI::count(), 20);
        TRY(a.erase(a.end() - 5, a.end()));                  TEST_EQUAL(AI::count(), 15);
        TRY(a.erase(a.end() - 5, a.end()));                  TEST_EQUAL(AI::count(), 10);
        TRY(a.erase(a.end() - 5, a.end()));                  TEST_EQUAL(AI::count(), 5);
        TRY(a.erase(a.end() - 5, a.end()));                  TEST_EQUAL(AI::count(), 0);
        TRY(a.append(v5));                                   TEST_EQUAL(AI::count(), 5);
        TRY(a.append(v5));                                   TEST_EQUAL(AI::count(), 10);
        TRY(a.append(v5));                                   TEST_EQUAL(AI::count(), 15);
        TRY(a.append(v5));                                   TEST_EQUAL(AI::count(), 20);
        TRY(a.append(std::move(v5)));                        TEST_EQUAL(AI::count(), 25);

    }

    void check_compact_array_keys() {

        using ICA = CompactArray<int, 5>;
        using Omap = std::map<ICA, U8string>;
        using Umap = std::unordered_map<ICA, U8string>;

        ICA a;
        Omap omap;
        Umap umap;
        U8string s;

        TRY(s = to_str(omap));  TEST_EQUAL(s, "{}");
        TRY(s = to_str(umap));  TEST_EQUAL(s, "{}");

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

        TRY(s = to_str(omap));
        TEST_EQUAL(s,
            "{[1,2]:A,"
            "[2,3,4]:BB,"
            "[3,4,5,6]:CCC,"
            "[4,5,6,7,8]:DDDD,"
            "[5,6,7,8,9,10]:EEEEE}"
        );

    }

}

TEST_MODULE(core, compact_array) {

    check_compact_array_construction();
    check_compact_array_capacity();
    check_compact_array_insertion();
    check_compact_array_accounting();
    check_compact_array_keys();

}
