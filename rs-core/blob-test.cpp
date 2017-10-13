#include "rs-core/blob.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;

void test_core_blob_class() {

    Blob b;

    TEST(b.empty());
    TEST_EQUAL(b.size(), 0);
    TEST_EQUAL(b.hex(), "");
    TEST_EQUAL(b.str(), "");
    TEST_EQUAL(U8string(b.chars().begin(), b.chars().end()), "");

    TRY(b = Blob(nullptr, 1));
    TEST(b.empty());
    TEST_EQUAL(b.size(), 0);

    TRY(b = Blob(5, 'a'));
    TEST(! b.empty());
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(b.hex(), "61 61 61 61 61");
    TEST_EQUAL(b.str(), "aaaaa");
    TEST_EQUAL(U8string(b.chars().begin(), b.chars().end()), "aaaaa");

    TRY(b.fill('z'));
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(b.hex(), "7a 7a 7a 7a 7a");
    TEST_EQUAL(b.str(), "zzzzz");

    TRY(b.clear());
    TEST(b.empty());
    TEST_EQUAL(b.size(), 0);
    TEST_EQUAL(b.hex(), "");
    TEST_EQUAL(b.str(), "");

    TRY(b.reset(10, 'a'));
    TEST(! b.empty());
    TEST_EQUAL(b.size(), 10);
    TEST_EQUAL(b.hex(), "61 61 61 61 61 61 61 61 61 61");
    TEST_EQUAL(b.str(), "aaaaaaaaaa");

    TRY(b.copy("hello", 5));
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(b.hex(), "68 65 6c 6c 6f");
    TEST_EQUAL(b.str(), "hello");

    U8string s, h = "hello", w = "world";

    TRY(b.clear());
    TRY(b.reset(&h[0], 5, [&] (void*) { s += "a"; }));
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(b.str(), "hello");
    TEST_EQUAL(s, "");
    TRY(b = Blob(&w[0], 5, [&] (void*) { s += "b"; }));
    TEST_EQUAL(b.size(), 5);
    TEST_EQUAL(b.str(), "world");
    TEST_EQUAL(s, "a");
    TRY(b.clear());
    TEST_EQUAL(s, "ab");

}
