#include "rs-core/ring-buffer.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;

void test_skeleton_ring_buffer_basics() {

    RingBuffer<int> r(5);
    U8string s;
    int i = 0;

    TEST_EQUAL(r.capacity(), 5);
    TEST_EQUAL(r.size(), 0);
    TEST(r.empty());
    TEST(! r.full());
    TRY(s = to_str(r));
    TEST_EQUAL(s, "[]");

    TRY(r.push(1));   TEST_EQUAL(r.size(), 1);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 1);  TEST_EQUAL(r.back(), 1);   TRY(s = to_str(r));  TEST_EQUAL(s, "[1]");
    TRY(r.push(2));   TEST_EQUAL(r.size(), 2);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 1);  TEST_EQUAL(r.back(), 2);   TRY(s = to_str(r));  TEST_EQUAL(s, "[1,2]");
    TRY(r.push(3));   TEST_EQUAL(r.size(), 3);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 1);  TEST_EQUAL(r.back(), 3);   TRY(s = to_str(r));  TEST_EQUAL(s, "[1,2,3]");
    TRY(r.push(4));   TEST_EQUAL(r.size(), 4);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 1);  TEST_EQUAL(r.back(), 4);   TRY(s = to_str(r));  TEST_EQUAL(s, "[1,2,3,4]");
    TRY(r.push(5));   TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 1);  TEST_EQUAL(r.back(), 5);   TRY(s = to_str(r));  TEST_EQUAL(s, "[1,2,3,4,5]");
    TRY(r.push(6));   TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 2);  TEST_EQUAL(r.back(), 6);   TRY(s = to_str(r));  TEST_EQUAL(s, "[2,3,4,5,6]");
    TRY(r.push(7));   TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 3);  TEST_EQUAL(r.back(), 7);   TRY(s = to_str(r));  TEST_EQUAL(s, "[3,4,5,6,7]");
    TRY(r.push(8));   TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 4);  TEST_EQUAL(r.back(), 8);   TRY(s = to_str(r));  TEST_EQUAL(s, "[4,5,6,7,8]");
    TRY(r.push(9));   TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 5);  TEST_EQUAL(r.back(), 9);   TRY(s = to_str(r));  TEST_EQUAL(s, "[5,6,7,8,9]");
    TRY(r.push(10));  TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 6);  TEST_EQUAL(r.back(), 10);  TRY(s = to_str(r));  TEST_EQUAL(s, "[6,7,8,9,10]");
    TRY(r.push(11));  TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 7);  TEST_EQUAL(r.back(), 11);  TRY(s = to_str(r));  TEST_EQUAL(s, "[7,8,9,10,11]");
    TRY(r.push(12));  TEST_EQUAL(r.size(), 5);  TEST(! r.empty());  TEST(r.full());    TEST_EQUAL(r.front(), 8);  TEST_EQUAL(r.back(), 12);  TRY(s = to_str(r));  TEST_EQUAL(s, "[8,9,10,11,12]");

    TRY(i = r.pop());  TEST_EQUAL(i, 8);   TEST_EQUAL(r.size(), 4);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 9);   TEST_EQUAL(r.back(), 12);  TRY(s = to_str(r));  TEST_EQUAL(s, "[9,10,11,12]");
    TRY(i = r.pop());  TEST_EQUAL(i, 9);   TEST_EQUAL(r.size(), 3);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 10);  TEST_EQUAL(r.back(), 12);  TRY(s = to_str(r));  TEST_EQUAL(s, "[10,11,12]");
    TRY(i = r.pop());  TEST_EQUAL(i, 10);  TEST_EQUAL(r.size(), 2);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 11);  TEST_EQUAL(r.back(), 12);  TRY(s = to_str(r));  TEST_EQUAL(s, "[11,12]");
    TRY(i = r.pop());  TEST_EQUAL(i, 11);  TEST_EQUAL(r.size(), 1);  TEST(! r.empty());  TEST(! r.full());  TEST_EQUAL(r.front(), 12);  TEST_EQUAL(r.back(), 12);  TRY(s = to_str(r));  TEST_EQUAL(s, "[12]");

    TRY(i = r.pop());
    TEST_EQUAL(i, 12);
    TEST_EQUAL(r.size(), 0);
    TEST(r.empty());
    TEST(! r.full());
    TRY(s = to_str(r));
    TEST_EQUAL(s, "[]");

}

void test_skeleton_ring_buffer_accounting_with_copy() {

    using account = Accountable<int>;

    RingBuffer<account> r(5);
    account a;

    TEST_EQUAL(account::count(), 1);
    TEST_EQUAL(r.capacity(), 5);
    TEST_EQUAL(r.size(), 0);
    TEST(r.empty());
    TEST(! r.full());

    TRY(r.push(1));   TEST_EQUAL(r.size(), 1);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 1);   TEST_EQUAL(account::count(), 2);
    TRY(r.push(2));   TEST_EQUAL(r.size(), 2);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 2);   TEST_EQUAL(account::count(), 3);
    TRY(r.push(3));   TEST_EQUAL(r.size(), 3);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 3);   TEST_EQUAL(account::count(), 4);
    TRY(r.push(4));   TEST_EQUAL(r.size(), 4);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 4);   TEST_EQUAL(account::count(), 5);
    TRY(r.push(5));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 5);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(6));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 2);  TEST_EQUAL(r.back().get(), 6);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(7));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 3);  TEST_EQUAL(r.back().get(), 7);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(8));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 4);  TEST_EQUAL(r.back().get(), 8);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(9));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 5);  TEST_EQUAL(r.back().get(), 9);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(10));  TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 6);  TEST_EQUAL(r.back().get(), 10);  TEST_EQUAL(account::count(), 6);
    TRY(r.push(11));  TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 7);  TEST_EQUAL(r.back().get(), 11);  TEST_EQUAL(account::count(), 6);
    TRY(r.push(12));  TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 8);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 6);

    TRY(a = r.pop());  TEST_EQUAL(a.get(), 8);   TEST_EQUAL(r.size(), 4);  TEST_EQUAL(r.front().get(), 9);   TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 5);
    TRY(a = r.pop());  TEST_EQUAL(a.get(), 9);   TEST_EQUAL(r.size(), 3);  TEST_EQUAL(r.front().get(), 10);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 4);
    TRY(a = r.pop());  TEST_EQUAL(a.get(), 10);  TEST_EQUAL(r.size(), 2);  TEST_EQUAL(r.front().get(), 11);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 3);
    TRY(a = r.pop());  TEST_EQUAL(a.get(), 11);  TEST_EQUAL(r.size(), 1);  TEST_EQUAL(r.front().get(), 12);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 2);

    TRY(a = r.pop());
    TEST_EQUAL(a.get(), 12);
    TEST_EQUAL(r.size(), 0);
    TEST(r.empty());
    TEST(! r.full());
    TEST_EQUAL(account::count(), 1);

}

void test_skeleton_ring_buffer_accounting_with_move() {

    using account = Accountable<int, false>;

    RingBuffer<account> r(5);
    account a;

    TEST_EQUAL(account::count(), 1);
    TEST_EQUAL(r.capacity(), 5);
    TEST_EQUAL(r.size(), 0);
    TEST(r.empty());
    TEST(! r.full());

    TRY(r.push(1));   TEST_EQUAL(r.size(), 1);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 1);   TEST_EQUAL(account::count(), 2);
    TRY(r.push(2));   TEST_EQUAL(r.size(), 2);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 2);   TEST_EQUAL(account::count(), 3);
    TRY(r.push(3));   TEST_EQUAL(r.size(), 3);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 3);   TEST_EQUAL(account::count(), 4);
    TRY(r.push(4));   TEST_EQUAL(r.size(), 4);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 4);   TEST_EQUAL(account::count(), 5);
    TRY(r.push(5));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 1);  TEST_EQUAL(r.back().get(), 5);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(6));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 2);  TEST_EQUAL(r.back().get(), 6);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(7));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 3);  TEST_EQUAL(r.back().get(), 7);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(8));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 4);  TEST_EQUAL(r.back().get(), 8);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(9));   TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 5);  TEST_EQUAL(r.back().get(), 9);   TEST_EQUAL(account::count(), 6);
    TRY(r.push(10));  TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 6);  TEST_EQUAL(r.back().get(), 10);  TEST_EQUAL(account::count(), 6);
    TRY(r.push(11));  TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 7);  TEST_EQUAL(r.back().get(), 11);  TEST_EQUAL(account::count(), 6);
    TRY(r.push(12));  TEST_EQUAL(r.size(), 5);  TEST_EQUAL(r.front().get(), 8);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 6);

    TRY(a = r.pop());  TEST_EQUAL(a.get(), 8);   TEST_EQUAL(r.size(), 4);  TEST_EQUAL(r.front().get(), 9);   TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 5);
    TRY(a = r.pop());  TEST_EQUAL(a.get(), 9);   TEST_EQUAL(r.size(), 3);  TEST_EQUAL(r.front().get(), 10);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 4);
    TRY(a = r.pop());  TEST_EQUAL(a.get(), 10);  TEST_EQUAL(r.size(), 2);  TEST_EQUAL(r.front().get(), 11);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 3);
    TRY(a = r.pop());  TEST_EQUAL(a.get(), 11);  TEST_EQUAL(r.size(), 1);  TEST_EQUAL(r.front().get(), 12);  TEST_EQUAL(r.back().get(), 12);  TEST_EQUAL(account::count(), 2);

    TRY(a = r.pop());
    TEST_EQUAL(a.get(), 12);
    TEST_EQUAL(r.size(), 0);
    TEST(r.empty());
    TEST(! r.full());
    TEST_EQUAL(account::count(), 1);

}
