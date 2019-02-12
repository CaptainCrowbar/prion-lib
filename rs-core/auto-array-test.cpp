#include "rs-core/auto-array.hpp"
#include "rs-core/unit-test.hpp"
#include <list>
#include <vector>

using namespace RS;

void test_core_auto_array_auto_vector_construction() {

    AutoVector<Ustring> con;
    std::vector<Ustring> vec;

    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(0), "");

    TRY(con = AutoVector<Ustring>("hello"));
    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(0), "hello");

    TRY(con = AutoVector<Ustring>(3, "hello"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[hello,hello,hello]");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "");

    TRY(con = AutoVector<Ustring>(3, "hello", "goodbye"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[hello,hello,hello]");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "goodbye");

    vec = {"delta","echo","foxtrot"};
    TRY(con = AutoVector<Ustring>(vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[delta,echo,foxtrot]");
    TEST_EQUAL(con.get(2), "foxtrot");
    TEST_EQUAL(con.get(3), "");

    TRY(con = AutoVector<Ustring>(vec.begin(), vec.end(), "zulu"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[delta,echo,foxtrot]");
    TEST_EQUAL(con.get(2), "foxtrot");
    TEST_EQUAL(con.get(3), "zulu");

    TRY((con = AutoVector<Ustring>{"alpha","bravo","charlie"}));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie]");
    TEST_EQUAL(con.get(2), "charlie");
    TEST_EQUAL(con.get(3), "");

    TRY((con = {"delta","echo","foxtrot"}));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[delta,echo,foxtrot]");
    TEST_EQUAL(con.get(2), "foxtrot");
    TEST_EQUAL(con.get(3), "");

    TRY(con.clear());
    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(0), "");

}

void test_core_auto_array_auto_vector_defaults() {

    AutoVector<Ustring> con;

    TEST_EQUAL(con.get_default(), "");
    TEST_EQUAL(con.get(0), "");
    TRY(con.set_default("hello"));
    TEST_EQUAL(con.get_default(), "hello");
    TEST_EQUAL(con.get(0), "hello");

    TRY(con = AutoVector<Ustring>(3, "hello", "goodbye"));
    TEST_EQUAL(con.get_default(), "goodbye");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "goodbye");
    TRY(con.set_default("farewell"));
    TEST_EQUAL(con.get_default(), "farewell");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "farewell");

    TRY(con.clear("greetings"));
    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(0), "greetings");

    TRY((con = {"alpha","bravo","charlie"}));
    TRY(con.set_default("zulu"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie]");
    TRY(con.ref(5));
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie,zulu,zulu,zulu]");
    TRY(con.set_default("yankee"));
    TRY(con.trim());
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie,zulu,zulu,zulu]");
    TRY(con.set_default("zulu"));
    TRY(con.trim());
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie]");

}

void test_core_auto_array_auto_vector_element_access() {

    AutoVector<Ustring> con;

    TEST_EQUAL(con.get(2), "");
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.ref(2), "");
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[,,]");

    TRY(con = AutoVector<Ustring>("hello"));
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.ref(2), "hello");
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[hello,hello,hello]");

}

void test_core_auto_array_auto_vector_insert_erase() {

    AutoVector<Ustring> con("x");
    AutoVector<Ustring>::iterator it;
    std::list<Ustring> list;
    std::vector<Ustring> vec;

    TRY(it = con.insert(con.begin() + 2, "aa"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[x,x,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 2, "bb"));
    TEST_EQUAL(con.size(), 4);
    TEST_EQUAL(to_str(con), "[x,x,bb,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "bb");

    TRY(it = con.insert(con.begin() + 6, 3, "cc"));
    TEST_EQUAL(con.size(), 9);
    TEST_EQUAL(to_str(con), "[x,x,bb,aa,x,x,cc,cc,cc]");
    TEST_EQUAL(it - con.begin(), 6);
    TEST_EQUAL(*it, "cc");
    TRY(it = con.insert(con.begin() + 2, 2, "dd"));
    TEST_EQUAL(con.size(), 11);
    TEST_EQUAL(to_str(con), "[x,x,dd,dd,bb,aa,x,x,cc,cc,cc]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "dd");

    vec = {"ee","ff","gg"};
    TRY(it = con.insert(con.begin() + 13, vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 16);
    TEST_EQUAL(to_str(con), "[x,x,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg]");
    TEST_EQUAL(it - con.begin(), 13);
    TEST_EQUAL(*it, "ee");
    TRY(it = con.insert(con.begin() + 2, vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 19);
    TEST_EQUAL(to_str(con), "[x,x,ee,ff,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "ee");

    list = {"hh","ii","jj"};
    TRY(it = con.insert(con.begin() + 21, list.begin(), list.end()));
    TEST_EQUAL(con.size(), 24);
    TEST_EQUAL(to_str(con), "[x,x,ee,ff,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg,x,x,hh,ii,jj]");
    TEST_EQUAL(it - con.begin(), 21);
    TEST_EQUAL(*it, "hh");
    TRY(it = con.insert(con.begin() + 2, list.begin(), list.end()));
    TEST_EQUAL(con.size(), 27);
    TEST_EQUAL(to_str(con), "[x,x,hh,ii,jj,ee,ff,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg,x,x,hh,ii,jj]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "hh");

    TRY(it = con.insert(con.begin() + 29, {"kk","ll","mm"}));
    TEST_EQUAL(con.size(), 32);
    TEST_EQUAL(to_str(con), "[x,x,hh,ii,jj,ee,ff,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg,x,x,hh,ii,jj,x,x,kk,ll,mm]");
    TEST_EQUAL(it - con.begin(), 29);
    TEST_EQUAL(*it, "kk");
    TRY(it = con.insert(con.begin() + 2, {"nn","oo","pp"}));
    TEST_EQUAL(con.size(), 35);
    TEST_EQUAL(to_str(con), "[x,x,nn,oo,pp,hh,ii,jj,ee,ff,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg,x,x,hh,ii,jj,x,x,kk,ll,mm]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "nn");

    TRY(it = con.emplace(con.begin() + 37, 2, 'q'));
    TEST_EQUAL(con.size(), 38);
    TEST_EQUAL(to_str(con), "[x,x,nn,oo,pp,hh,ii,jj,ee,ff,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg,x,x,hh,ii,jj,x,x,kk,ll,mm,x,x,qq]");
    TEST_EQUAL(it - con.begin(), 37);
    TEST_EQUAL(*it, "qq");
    TRY(it = con.emplace(con.begin() + 2, 2, 'r'));
    TEST_EQUAL(con.size(), 39);
    TEST_EQUAL(to_str(con), "[x,x,rr,nn,oo,pp,hh,ii,jj,ee,ff,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg,x,x,hh,ii,jj,x,x,kk,ll,mm,x,x,qq]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "rr");

    TRY(it = con.erase(con.begin() + 10));
    TEST_EQUAL(con.size(), 38);
    TEST_EQUAL(to_str(con), "[x,x,rr,nn,oo,pp,hh,ii,jj,ee,gg,dd,dd,bb,aa,x,x,cc,cc,cc,x,x,ee,ff,gg,x,x,hh,ii,jj,x,x,kk,ll,mm,x,x,qq]");
    TEST_EQUAL(it - con.begin(), 10);
    TEST_EQUAL(*it, "gg");

    TRY(it = con.erase(con.begin() + 10, con.begin() + 20));
    TEST_EQUAL(con.size(), 28);
    TEST_EQUAL(to_str(con), "[x,x,rr,nn,oo,pp,hh,ii,jj,ee,x,x,ee,ff,gg,x,x,hh,ii,jj,x,x,kk,ll,mm,x,x,qq]");
    TEST_EQUAL(it - con.begin(), 10);
    TEST_EQUAL(*it, "x");

    TRY(it = con.erase(con.begin() + 10, con.begin() + 100));
    TEST_EQUAL(con.size(), 10);
    TEST_EQUAL(to_str(con), "[x,x,rr,nn,oo,pp,hh,ii,jj,ee]");
    TEST_EQUAL(it - con.begin(), 10);
    TEST_EQUAL(*it, "x");

}

void test_core_auto_array_auto_vector_comparison() {

    AutoVector<Ustring> a = {"alpha","bravo","charlie"};
    AutoVector<Ustring> b = {"alpha","bravo","charlie"};
    AutoVector<Ustring> c = {"delta","echo","foxtrot"};

    TEST_COMPARE(a, ==, b);
    TEST_COMPARE(a, <=, b);
    TEST_COMPARE(a, >=, b);
    TEST_COMPARE(a, !=, c);
    TEST_COMPARE(a, <, c);
    TEST_COMPARE(a, <=, c);

    TRY(a.ref(5));
    TEST_EQUAL(to_str(a), "[alpha,bravo,charlie,,,]");
    TEST_COMPARE(a, ==, b);
    TEST_COMPARE(a, <=, b);
    TEST_COMPARE(a, >=, b);
    TEST_COMPARE(b, ==, a);
    TEST_COMPARE(b, <=, a);
    TEST_COMPARE(b, >=, a);

    TRY(a.ref(5) = "zulu");
    TEST_EQUAL(to_str(a), "[alpha,bravo,charlie,,,zulu]");
    TEST_COMPARE(a, !=, b);
    TEST_COMPARE(a, >, b);
    TEST_COMPARE(a, >=, b);
    TEST_COMPARE(b, !=, a);
    TEST_COMPARE(b, <, a);
    TEST_COMPARE(b, <=, a);

}

void test_core_auto_array_auto_deque_construction() {

    AutoDeque<Ustring> con;
    std::vector<Ustring> vec;

    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(0), "");

    TRY(con = AutoDeque<Ustring>("hello"));
    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(0), "hello");

    TRY(con = AutoDeque<Ustring>(3, "hello"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[hello,hello,hello]");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "");

    TRY(con = AutoDeque<Ustring>(3, "hello", "goodbye"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[hello,hello,hello]");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "goodbye");

    vec = {"delta","echo","foxtrot"};
    TRY(con = AutoDeque<Ustring>(vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[delta,echo,foxtrot]");
    TEST_EQUAL(con.get(2), "foxtrot");
    TEST_EQUAL(con.get(3), "");

    TRY(con = AutoDeque<Ustring>(vec.begin(), vec.end(), "zulu"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[delta,echo,foxtrot]");
    TEST_EQUAL(con.get(2), "foxtrot");
    TEST_EQUAL(con.get(3), "zulu");

    TRY((con = AutoDeque<Ustring>{"alpha","bravo","charlie"}));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie]");
    TEST_EQUAL(con.get(2), "charlie");
    TEST_EQUAL(con.get(3), "");

    TRY((con = {"delta","echo","foxtrot"}));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[delta,echo,foxtrot]");
    TEST_EQUAL(con.get(2), "foxtrot");
    TEST_EQUAL(con.get(3), "");

    TRY(con.clear());
    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(0), "");

}

void test_core_auto_array_auto_deque_defaults() {

    AutoDeque<Ustring> con;

    TEST_EQUAL(con.get_default(), "");
    TEST_EQUAL(con.get(2), "");
    TEST_EQUAL(con.get(-2), "");
    TRY(con.set_default("hello"));
    TEST_EQUAL(con.get_default(), "hello");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(-2), "hello");

    TRY(con = AutoDeque<Ustring>(3, "hello", "goodbye"));
    TEST_EQUAL(con.get_default(), "goodbye");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "goodbye");
    TEST_EQUAL(con.get(-1), "goodbye");
    TRY(con.set_default("farewell"));
    TEST_EQUAL(con.get_default(), "farewell");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.get(3), "farewell");
    TEST_EQUAL(con.get(-1), "farewell");

    TRY(con.clear("greetings"));
    TEST(con.empty());
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(2), "greetings");
    TEST_EQUAL(con.get(-2), "greetings");

    TRY((con = {"alpha","bravo","charlie"}));
    TRY(con.set_default("zulu"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie]");
    TRY(con.ref(5));
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie,zulu,zulu,zulu]");
    TRY(con.set_default("yankee"));
    TRY(con.trim());
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie,zulu,zulu,zulu]");
    TRY(con.set_default("zulu"));
    TRY(con.trim());
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[alpha,bravo,charlie]");

}

void test_core_auto_array_auto_deque_element_access() {

    AutoDeque<Ustring> con;

    TEST_EQUAL(con.get(2), "");
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.ref(2), "");
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[,,]");
    TEST_EQUAL(con.ref(-2), "");
    TEST_EQUAL(con.size(), 5);
    TEST_EQUAL(con.min_index(), -2);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[,,,,]");

    TRY(con = AutoDeque<Ustring>("hello"));
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.get(2), "hello");
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");
    TEST_EQUAL(con.ref(2), "hello");
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[hello,hello,hello]");
    TEST_EQUAL(con.ref(-2), "hello");
    TEST_EQUAL(con.size(), 5);
    TEST_EQUAL(con.min_index(), -2);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[hello,hello,hello,hello,hello]");

    TRY(con = AutoDeque<Ustring>("x"));

    /**/                      TEST_EQUAL(con.size(), 0);   TEST_EQUAL(con.min_index(), 0);   TEST_EQUAL(con.max_index(), -1);  TEST_EQUAL(to_str(con), "[]");
    TRY(con.ref(1) = "aa");   TEST_EQUAL(con.size(), 2);   TEST_EQUAL(con.min_index(), 0);   TEST_EQUAL(con.max_index(), 1);   TEST_EQUAL(to_str(con), "[x,aa]");
    TRY(con.ref(-1) = "bb");  TEST_EQUAL(con.size(), 3);   TEST_EQUAL(con.min_index(), -1);  TEST_EQUAL(con.max_index(), 1);   TEST_EQUAL(to_str(con), "[bb,x,aa]");
    TRY(con.ref(3) = "cc");   TEST_EQUAL(con.size(), 5);   TEST_EQUAL(con.min_index(), -1);  TEST_EQUAL(con.max_index(), 3);   TEST_EQUAL(to_str(con), "[bb,x,aa,x,cc]");
    TRY(con.ref(-3) = "dd");  TEST_EQUAL(con.size(), 7);   TEST_EQUAL(con.min_index(), -3);  TEST_EQUAL(con.max_index(), 3);   TEST_EQUAL(to_str(con), "[dd,x,bb,x,aa,x,cc]");
    TRY(con.ref(5) = "ee");   TEST_EQUAL(con.size(), 9);   TEST_EQUAL(con.min_index(), -3);  TEST_EQUAL(con.max_index(), 5);   TEST_EQUAL(to_str(con), "[dd,x,bb,x,aa,x,cc,x,ee]");
    TRY(con.ref(-5) = "ff");  TEST_EQUAL(con.size(), 11);  TEST_EQUAL(con.min_index(), -5);  TEST_EQUAL(con.max_index(), 5);   TEST_EQUAL(to_str(con), "[ff,x,dd,x,bb,x,aa,x,cc,x,ee]");

}

void test_core_auto_array_auto_deque_insert_erase() {

    AutoDeque<Ustring> con;
    AutoDeque<Ustring>::const_iterator it;
    std::list<Ustring> list;
    std::vector<Ustring> vec;

    TRY(con.clear("x"));
    TRY(it = con.insert(con.begin() + 2, "aa"));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[x,x,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 2, "bb"));
    TEST_EQUAL(con.size(), 4);
    TEST_EQUAL(to_str(con), "[x,x,bb,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "bb");
    TRY(it = con.insert(con.begin() - 2, "cc"));
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(to_str(con), "[cc,x,x,x,bb,aa]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "cc");
    TRY(it = con.insert(con.begin() - 2, "dd"));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(to_str(con), "[dd,x,cc,x,x,x,bb,aa]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "dd");
    TRY(it = con.insert(con.begin() + 10, "ee"));
    TEST_EQUAL(con.size(), 11);
    TEST_EQUAL(to_str(con), "[dd,x,cc,x,x,x,bb,aa,x,x,ee]");
    TEST_EQUAL(it - con.begin(), 10);
    TEST_EQUAL(*it, "ee");

    TRY(con.clear("x"));
    TRY(it = con.insert(con.begin() + 2, 3, "aa"));
    TEST_EQUAL(con.size(), 5);
    TEST_EQUAL(to_str(con), "[x,x,aa,aa,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 2, 3, "bb"));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(to_str(con), "[x,x,bb,bb,bb,aa,aa,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "bb");
    TRY(it = con.insert(con.begin() - 2, 3, "cc"));
    TEST_EQUAL(con.size(), 12);
    TEST_EQUAL(to_str(con), "[cc,cc,cc,x,x,x,bb,bb,bb,aa,aa,aa]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "cc");
    TRY(it = con.insert(con.begin() - 2, 3, "dd"));
    TEST_EQUAL(con.size(), 16);
    TEST_EQUAL(to_str(con), "[dd,dd,dd,x,cc,cc,cc,x,x,x,bb,bb,bb,aa,aa,aa]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "dd");
    TRY(it = con.insert(con.begin() + 20, 3, "ee"));
    TEST_EQUAL(con.size(), 23);
    TEST_EQUAL(to_str(con), "[dd,dd,dd,x,cc,cc,cc,x,x,x,bb,bb,bb,aa,aa,aa,x,x,x,x,ee,ee,ee]");
    TEST_EQUAL(it - con.begin(), 20);
    TEST_EQUAL(*it, "ee");

    TRY(con.clear("x"));
    vec = {"aa","bb","cc"};
    TRY(it = con.insert(con.begin() + 2, vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 5);
    TEST_EQUAL(to_str(con), "[x,x,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 2, vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(to_str(con), "[x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() - 2, vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 12);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() - 2, vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 16);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 20, vec.begin(), vec.end()));
    TEST_EQUAL(con.size(), 23);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc,x,x,x,x,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 20);
    TEST_EQUAL(*it, "aa");

    TRY(con.clear("x"));
    list = {"aa","bb","cc"};
    TRY(it = con.insert(con.begin() + 2, list.begin(), list.end()));
    TEST_EQUAL(con.size(), 5);
    TEST_EQUAL(to_str(con), "[x,x,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 2, list.begin(), list.end()));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(to_str(con), "[x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() - 2, list.begin(), list.end()));
    TEST_EQUAL(con.size(), 12);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() - 2, list.begin(), list.end()));
    TEST_EQUAL(con.size(), 16);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 20, list.begin(), list.end()));
    TEST_EQUAL(con.size(), 23);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc,x,x,x,x,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 20);
    TEST_EQUAL(*it, "aa");

    TRY(con.clear("x"));
    TRY(it = con.insert(con.begin() + 2, {"aa","bb","cc"}));
    TEST_EQUAL(con.size(), 5);
    TEST_EQUAL(to_str(con), "[x,x,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 2, {"aa","bb","cc"}));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(to_str(con), "[x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() - 2, {"aa","bb","cc"}));
    TEST_EQUAL(con.size(), 12);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() - 2, {"aa","bb","cc"}));
    TEST_EQUAL(con.size(), 16);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.insert(con.begin() + 20, {"aa","bb","cc"}));
    TEST_EQUAL(con.size(), 23);
    TEST_EQUAL(to_str(con), "[aa,bb,cc,x,aa,bb,cc,x,x,x,aa,bb,cc,aa,bb,cc,x,x,x,x,aa,bb,cc]");
    TEST_EQUAL(it - con.begin(), 20);
    TEST_EQUAL(*it, "aa");

    TRY(con.clear("x"));
    TRY(it = con.emplace(con.begin() + 2, 2, 'a'));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(to_str(con), "[x,x,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "aa");
    TRY(it = con.emplace(con.begin() + 2, 2, 'b'));
    TEST_EQUAL(con.size(), 4);
    TEST_EQUAL(to_str(con), "[x,x,bb,aa]");
    TEST_EQUAL(it - con.begin(), 2);
    TEST_EQUAL(*it, "bb");
    TRY(it = con.emplace(con.begin() - 2, 2, 'c'));
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(to_str(con), "[cc,x,x,x,bb,aa]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "cc");
    TRY(it = con.emplace(con.begin() - 2, 2, 'd'));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(to_str(con), "[dd,x,cc,x,x,x,bb,aa]");
    TEST_EQUAL(it - con.begin(), 0);
    TEST_EQUAL(*it, "dd");
    TRY(it = con.emplace(con.begin() + 10, 2, 'e'));
    TEST_EQUAL(con.size(), 11);
    TEST_EQUAL(to_str(con), "[dd,x,cc,x,x,x,bb,aa,x,x,ee]");
    TEST_EQUAL(it - con.begin(), 10);
    TEST_EQUAL(*it, "ee");

    TRY((con = {"aa","bb","cc","dd","ee"}));
    TRY(con[-3] = "pp");
    TRY(con[-2] = "qq");
    TRY(con[-1] = "rr");
    TRY(con.set_default("x"));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), 4);
    TEST_EQUAL(to_str(con), "[pp,qq,rr,aa,bb,cc,dd,ee]");
    TRY(it = con.erase(con.begin() + 10));
    TEST_EQUAL(*it, "x");
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), 4);
    TEST_EQUAL(to_str(con), "[pp,qq,rr,aa,bb,cc,dd,ee]");
    TRY(it = con.erase(con.begin() + 6));
    TEST_EQUAL(*it, "ee");
    TEST_EQUAL(con.size(), 7);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), 3);
    TEST_EQUAL(to_str(con), "[pp,qq,rr,aa,bb,cc,ee]");
    TRY(it = con.erase(con.begin() + 1));
    TEST_EQUAL(*it, "rr");
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[pp,rr,aa,bb,cc,ee]");
    TRY(it = con.erase(con.begin() - 10));
    TEST_EQUAL(*it, "x");
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(con.min_index(), -4);
    TEST_EQUAL(con.max_index(), 1);
    TEST_EQUAL(to_str(con), "[pp,rr,aa,bb,cc,ee]");
    TRY(con.erase(con.begin()));
    TRY(con.erase(con.begin()));
    TRY(con.erase(con.begin()));
    TRY(con.erase(con.begin()));
    TRY(con.erase(con.begin()));
    TRY(con.erase(con.begin()));
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");

    TRY((con = {"aa","bb","cc","dd","ee"}));
    TRY(con[-3] = "pp");
    TRY(con[-2] = "qq");
    TRY(con[-1] = "rr");
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), 4);
    TEST_EQUAL(to_str(con), "[pp,qq,rr,aa,bb,cc,dd,ee]");
    TRY(it = con.erase(con.begin() + 10, con.begin() + 20));
    TEST_EQUAL(con.size(), 8);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), 4);
    TEST_EQUAL(to_str(con), "[pp,qq,rr,aa,bb,cc,dd,ee]");
    TRY(it = con.erase(con.begin() + 5, con.begin() + 7));
    TEST_EQUAL(con.size(), 6);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), 2);
    TEST_EQUAL(to_str(con), "[pp,qq,rr,aa,bb,ee]");
    TRY(it = con.erase(con.begin() + 1, con.begin() + 4));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), -3);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[pp,bb,ee]");
    TRY(it = con.erase(con.begin() - 20, con.begin() - 10));
    TEST_EQUAL(con.size(), 3);
    TEST_EQUAL(con.min_index(), -13);
    TEST_EQUAL(con.max_index(), -11);
    TEST_EQUAL(to_str(con), "[pp,bb,ee]");
    TRY(it = con.erase(con.begin() - 2, con.begin() + 1));
    TEST_EQUAL(con.size(), 2);
    TEST_EQUAL(con.min_index(), -15);
    TEST_EQUAL(con.max_index(), -14);
    TEST_EQUAL(to_str(con), "[bb,ee]");
    TRY(it = con.erase(con.begin() - 10, con.begin() + 10));
    TEST_EQUAL(con.size(), 0);
    TEST_EQUAL(con.min_index(), 0);
    TEST_EQUAL(con.max_index(), -1);
    TEST_EQUAL(to_str(con), "[]");

}

void test_core_auto_array_auto_deque_comparison() {

    AutoDeque<Ustring> a = {"alpha","bravo","charlie"};
    AutoDeque<Ustring> b = {"alpha","bravo","charlie"};
    AutoDeque<Ustring> c = {"delta","echo","foxtrot"};

    TEST_COMPARE(a, ==, b);
    TEST_COMPARE(a, <=, b);
    TEST_COMPARE(a, >=, b);
    TEST_COMPARE(a, !=, c);
    TEST_COMPARE(a, <, c);
    TEST_COMPARE(a, <=, c);

    TRY(a.ref(5));
    TEST_EQUAL(to_str(a), "[alpha,bravo,charlie,,,]");
    TEST_COMPARE(a, ==, b);
    TEST_COMPARE(a, <=, b);
    TEST_COMPARE(a, >=, b);
    TEST_COMPARE(b, ==, a);
    TEST_COMPARE(b, <=, a);
    TEST_COMPARE(b, >=, a);

    TRY(a.ref(5) = "zulu");
    TEST_EQUAL(to_str(a), "[alpha,bravo,charlie,,,zulu]");
    TEST_COMPARE(a, !=, b);
    TEST_COMPARE(a, >, b);
    TEST_COMPARE(a, >=, b);
    TEST_COMPARE(b, !=, a);
    TEST_COMPARE(b, <, a);
    TEST_COMPARE(b, <=, a);

    TRY((a = {"alpha","bravo","charlie"}));
    TRY(b.clear());
    TRY(b[-1] = "alpha");
    TRY(b[0] = "bravo");
    TRY(b[1] = "charlie");
    TEST_EQUAL(to_str(a), "[alpha,bravo,charlie]");
    TEST_EQUAL(to_str(b), "[alpha,bravo,charlie]");
    TEST_COMPARE(a, !=, b);
    TEST_COMPARE(a, <, b);
    TEST_COMPARE(a, <=, b);
    TEST_COMPARE(b, !=, a);
    TEST_COMPARE(b, >, a);
    TEST_COMPARE(b, >=, a);

    TRY(b = a);
    TRY(b.set_default("x"));
    TEST_COMPARE(a, !=, b);
    TEST_COMPARE(a, <, b);
    TEST_COMPARE(a, <=, b);
    TEST_COMPARE(b, !=, a);
    TEST_COMPARE(b, >, a);
    TEST_COMPARE(b, >=, a);

}
