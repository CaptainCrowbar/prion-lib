#include "rs-core/ordered-map.hpp"
#include "rs-core/unit-test.hpp"
#include <tuple>
#include <utility>

using namespace RS;

void test_core_ordered_map_insertion() {

    using OM = OrderedMap<int, Ustring>;

    OM om;
    const OM& com = om;
    OM::iterator i;
    OM::const_iterator ci;
    Ustring s;
    bool b = false;

    TEST(com.empty());
    TEST_EQUAL(com.size(), 0);
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{}");

    TRY(std::tie(i, b) = om.insert(3, "charlie"));  TEST_EQUAL(i->first, 3);  TEST_EQUAL(i->second, "charlie");  TEST(b);
    TRY(std::tie(i, b) = om.insert(2, "bravo"));    TEST_EQUAL(i->first, 2);  TEST_EQUAL(i->second, "bravo");    TEST(b);
    TRY(std::tie(i, b) = om.insert(1, "alpha"));    TEST_EQUAL(i->first, 1);  TEST_EQUAL(i->second, "alpha");    TEST(b);

    TEST(! com.empty());
    TEST_EQUAL(com.size(), 3);
    TEST(com.has(1));
    TEST(com.has(2));
    TEST(com.has(3));
    TEST(! com.has(4));
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{3:charlie,2:bravo,1:alpha}");

    TEST_EQUAL(om[1], "alpha");
    TEST_EQUAL(om[2], "bravo");
    TEST_EQUAL(om[3], "charlie");
    TRY(om[4] = "delta");
    TEST_EQUAL(com.size(), 4);
    TEST(com.has(4));
    TEST_EQUAL(om[4], "delta");
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{3:charlie,2:bravo,1:alpha,4:delta}");

    TRY(std::tie(i, b) = om.insert(4, "daffodil"));  TEST_EQUAL(i->first, 4);  TEST_EQUAL(i->second, "delta");     TEST(! b);
    TRY(i = om.set(4, "daffodil"));                  TEST_EQUAL(i->first, 4);  TEST_EQUAL(i->second, "daffodil");
    TRY(i = om.set(5, "echo"));                      TEST_EQUAL(i->first, 5);  TEST_EQUAL(i->second, "echo");

    TEST_EQUAL(com.size(), 5);
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{3:charlie,2:bravo,1:alpha,4:daffodil,5:echo}");

    TRY(i = om.find(1));  TEST(i != om.end());  TEST_EQUAL(i->first, 1);  TEST_EQUAL(i->second, "alpha");
    TRY(i = om.find(2));  TEST(i != om.end());  TEST_EQUAL(i->first, 2);  TEST_EQUAL(i->second, "bravo");
    TRY(i = om.find(3));  TEST(i != om.end());  TEST_EQUAL(i->first, 3);  TEST_EQUAL(i->second, "charlie");
    TRY(i = om.find(4));  TEST(i != om.end());  TEST_EQUAL(i->first, 4);  TEST_EQUAL(i->second, "daffodil");
    TRY(i = om.find(5));  TEST(i != om.end());  TEST_EQUAL(i->first, 5);  TEST_EQUAL(i->second, "echo");
    TRY(i = om.find(6));  TEST(i == om.end());

    TRY(ci = com.find(1));  TEST(ci != com.end());  TEST_EQUAL(ci->first, 1);  TEST_EQUAL(ci->second, "alpha");
    TRY(ci = com.find(2));  TEST(ci != com.end());  TEST_EQUAL(ci->first, 2);  TEST_EQUAL(ci->second, "bravo");
    TRY(ci = com.find(3));  TEST(ci != com.end());  TEST_EQUAL(ci->first, 3);  TEST_EQUAL(ci->second, "charlie");
    TRY(ci = com.find(4));  TEST(ci != com.end());  TEST_EQUAL(ci->first, 4);  TEST_EQUAL(ci->second, "daffodil");
    TRY(ci = com.find(5));  TEST(ci != com.end());  TEST_EQUAL(ci->first, 5);  TEST_EQUAL(ci->second, "echo");
    TRY(ci = com.find(6));  TEST(ci == com.end());

    TEST(om.erase(4));
    TEST_EQUAL(com.size(), 4);
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{3:charlie,2:bravo,1:alpha,5:echo}");
    TEST(om.erase(1));
    TEST_EQUAL(com.size(), 3);
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{3:charlie,2:bravo,5:echo}");
    TRY(om.erase(com.begin()));
    TEST_EQUAL(com.size(), 2);
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{2:bravo,5:echo}");
    TEST(! om.erase(1));

    TRY(om.clear());
    TEST(com.empty());
    TEST_EQUAL(com.size(), 0);
    TRY(s = to_str(com));
    TEST_EQUAL(s, "{}");

}

void test_core_ordered_map_comparison() {

    using OM = OrderedMap<int, Ustring>;

    OM om1;
    TRY(om1.insert(1, "alpha"));
    TRY(om1.insert(2, "bravo"));
    TRY(om1.insert(3, "charlie"));

    OM om2(om1);

    OM om3;
    TRY(om3 = om1);
    TRY(om3[3] = "CHARLIE");

    OM om4x(om1);
    OM om4(std::move(om4x));
    TRY(om4.insert(4, "delta"));

    OM om5x(om1);
    OM om5;
    TRY(om5 = std::move(om5x));
    TRY(om5.clear());
    TRY(om5.insert(3, "charlie"));
    TRY(om5.insert(2, "bravo"));
    TRY(om5.insert(1, "alpha"));

    Ustring s;

    TRY(s = to_str(om1));  TEST_EQUAL(s, "{1:alpha,2:bravo,3:charlie}");
    TRY(s = to_str(om2));  TEST_EQUAL(s, "{1:alpha,2:bravo,3:charlie}");
    TRY(s = to_str(om3));  TEST_EQUAL(s, "{1:alpha,2:bravo,3:CHARLIE}");
    TRY(s = to_str(om4));  TEST_EQUAL(s, "{1:alpha,2:bravo,3:charlie,4:delta}");
    TRY(s = to_str(om5));  TEST_EQUAL(s, "{3:charlie,2:bravo,1:alpha}");

    TEST(om1 == om2);  TEST(om2 == om1);
    TEST(om1 <= om2);  TEST(om2 <= om1);
    TEST(om1 >= om2);  TEST(om2 >= om1);
    TEST(om1 != om3);  TEST(om3 != om1);
    TEST(om1 > om3);   TEST(om3 < om1);
    TEST(om1 >= om3);  TEST(om3 <= om1);
    TEST(om1 != om4);  TEST(om4 != om1);
    TEST(om1 < om4);   TEST(om4 > om1);
    TEST(om1 <= om4);  TEST(om4 >= om1);
    TEST(om1 != om5);  TEST(om5 != om1);
    TEST(om1 < om5);   TEST(om5 > om1);
    TEST(om1 <= om5);  TEST(om5 >= om1);

}
