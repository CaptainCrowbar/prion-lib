#include "rs-core/interval.hpp"
#include "rs-core/random.hpp"
#include "rs-core/unit-test.hpp"
#include <iostream>
#include <iterator>
#include <vector>

using namespace RS;

using Itype = Interval<int>;
using Iset = IntervalSet<int>;
using Imap = IntervalMap<int, Ustring>;
using IB = IntervalBound;
using IC = IntervalCategory;
using IM = IntervalMatch;
using IO = IntervalOrder;

void test_core_interval_integral_basic_properties() {

    Itype in;

    TEST_TYPE(Itype::value_type, int);
    TEST_EQUAL(Itype::category, IC::integral);
    TEST(in.is_empty());
    TEST_EQUAL(in.size(), 0);
    TEST(! in);
    TEST_EQUAL(to_str(in), "{}");
    TEST_EQUAL(in.match(0), IM::empty);
    TEST_EQUAL(in.match(42), IM::empty);
    TEST(! in(0));
    TEST(! in(42));

}

void test_core_interval_integral_construction() {

    Itype in;
    Ustring str;

    TRY(in = Itype());                             TRY(str = to_str(in));  TEST_EQUAL(str, "{}");
    TRY(in = Itype(1));                            TRY(str = to_str(in));  TEST_EQUAL(str, "1");
    TRY(in = Itype(1, IB::closed, IB::closed));    TRY(str = to_str(in));  TEST_EQUAL(str, "1");
    TRY(in = Itype(1, IB::closed, IB::open));      TRY(str = to_str(in));  TEST_EQUAL(str, "{}");
    TRY(in = Itype(1, IB::open, IB::closed));      TRY(str = to_str(in));  TEST_EQUAL(str, "{}");
    TRY(in = Itype(1, IB::closed, IB::unbound));   TRY(str = to_str(in));  TEST_EQUAL(str, ">=1");
    TRY(in = Itype(1, IB::unbound, IB::closed));   TRY(str = to_str(in));  TEST_EQUAL(str, "<=1");
    TRY(in = Itype(1, IB::open, IB::unbound));     TRY(str = to_str(in));  TEST_EQUAL(str, ">=2");
    TRY(in = Itype(1, IB::unbound, IB::open));     TRY(str = to_str(in));  TEST_EQUAL(str, "<=0");
    TRY(in = Itype(1, IB::unbound, IB::unbound));  TRY(str = to_str(in));  TEST_EQUAL(str, "*");
    TRY(in = Itype(1, 5));                         TRY(str = to_str(in));  TEST_EQUAL(str, "[1,5]");
    TRY(in = Itype(1, 5, IB::closed));             TRY(str = to_str(in));  TEST_EQUAL(str, "[1,5]");
    TRY(in = Itype(1, 5, IB::open));               TRY(str = to_str(in));  TEST_EQUAL(str, "[2,4]");
    TRY(in = Itype(1, 5, IB::closed, IB::open));   TRY(str = to_str(in));  TEST_EQUAL(str, "[1,4]");
    TRY(in = Itype(1, 5, IB::open, IB::closed));   TRY(str = to_str(in));  TEST_EQUAL(str, "[2,5]");
    TRY(in = Itype(1, 5, "[]"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[1,5]");
    TRY(in = Itype(1, 5, "()"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[2,4]");
    TRY(in = Itype(1, 5, "[)"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[1,4]");
    TRY(in = Itype(1, 5, "(]"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[2,5]");
    TRY(in = Itype(1, 5, "<"));                    TRY(str = to_str(in));  TEST_EQUAL(str, "<=4");
    TRY(in = Itype(1, 5, "<="));                   TRY(str = to_str(in));  TEST_EQUAL(str, "<=5");
    TRY(in = Itype(1, 5, ">"));                    TRY(str = to_str(in));  TEST_EQUAL(str, ">=2");
    TRY(in = Itype(1, 5, ">="));                   TRY(str = to_str(in));  TEST_EQUAL(str, ">=1");
    TRY(in = Itype(5, 1));                         TRY(str = to_str(in));  TEST_EQUAL(str, "{}");

    TRY(in = make_interval(1));                            TRY(str = to_str(in));  TEST_EQUAL(str, "1");
    TRY(in = make_interval(1, IB::closed, IB::closed));    TRY(str = to_str(in));  TEST_EQUAL(str, "1");
    TRY(in = make_interval(1, IB::closed, IB::open));      TRY(str = to_str(in));  TEST_EQUAL(str, "{}");
    TRY(in = make_interval(1, IB::open, IB::closed));      TRY(str = to_str(in));  TEST_EQUAL(str, "{}");
    TRY(in = make_interval(1, IB::closed, IB::unbound));   TRY(str = to_str(in));  TEST_EQUAL(str, ">=1");
    TRY(in = make_interval(1, IB::unbound, IB::closed));   TRY(str = to_str(in));  TEST_EQUAL(str, "<=1");
    TRY(in = make_interval(1, IB::open, IB::unbound));     TRY(str = to_str(in));  TEST_EQUAL(str, ">=2");
    TRY(in = make_interval(1, IB::unbound, IB::open));     TRY(str = to_str(in));  TEST_EQUAL(str, "<=0");
    TRY(in = make_interval(1, IB::unbound, IB::unbound));  TRY(str = to_str(in));  TEST_EQUAL(str, "*");
    TRY(in = make_interval(1, 5));                         TRY(str = to_str(in));  TEST_EQUAL(str, "[1,5]");
    TRY(in = make_interval(1, 5, IB::closed));             TRY(str = to_str(in));  TEST_EQUAL(str, "[1,5]");
    TRY(in = make_interval(1, 5, IB::open));               TRY(str = to_str(in));  TEST_EQUAL(str, "[2,4]");
    TRY(in = make_interval(1, 5, IB::closed, IB::open));   TRY(str = to_str(in));  TEST_EQUAL(str, "[1,4]");
    TRY(in = make_interval(1, 5, IB::open, IB::closed));   TRY(str = to_str(in));  TEST_EQUAL(str, "[2,5]");
    TRY(in = make_interval(1, 5, "[]"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[1,5]");
    TRY(in = make_interval(1, 5, "()"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[2,4]");
    TRY(in = make_interval(1, 5, "[)"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[1,4]");
    TRY(in = make_interval(1, 5, "(]"));                   TRY(str = to_str(in));  TEST_EQUAL(str, "[2,5]");
    TRY(in = make_interval(1, 5, "<"));                    TRY(str = to_str(in));  TEST_EQUAL(str, "<=4");
    TRY(in = make_interval(1, 5, "<="));                   TRY(str = to_str(in));  TEST_EQUAL(str, "<=5");
    TRY(in = make_interval(1, 5, ">"));                    TRY(str = to_str(in));  TEST_EQUAL(str, ">=2");
    TRY(in = make_interval(1, 5, ">="));                   TRY(str = to_str(in));  TEST_EQUAL(str, ">=1");
    TRY(in = make_interval(5, 1));                         TRY(str = to_str(in));  TEST_EQUAL(str, "{}");

    TRY(in = ordered_interval(5, 1, IB::closed));             TRY(str = to_str(in));  TEST_EQUAL(str, "[1,5]");
    TRY(in = ordered_interval(5, 1, IB::open));               TRY(str = to_str(in));  TEST_EQUAL(str, "[2,4]");
    TRY(in = ordered_interval(5, 1, IB::closed, IB::open));   TRY(str = to_str(in));  TEST_EQUAL(str, "[2,5]");
    TRY(in = ordered_interval(5, 1, IB::open, IB::closed));   TRY(str = to_str(in));  TEST_EQUAL(str, "[1,4]");

}

void test_core_interval_integral_string_parsing() {

    Itype in;

    TRY(in = from_str<Itype>(""));       TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("{}"));     TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("*"));      TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), "*");
    TRY(in = from_str<Itype>("1"));      TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 1);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "1");
    TRY(in = from_str<Itype>("=1"));     TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 1);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "1");
    TRY(in = from_str<Itype>("<1"));     TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "<=0");
    TRY(in = from_str<Itype>("<=1"));    TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 1);  TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "<=1");
    TRY(in = from_str<Itype>("1-"));     TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 1);  TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "<=1");
    TRY(in = from_str<Itype>(">1"));     TEST_EQUAL(in.min(), 2);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), ">=2");
    TRY(in = from_str<Itype>(">=1"));    TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), ">=1");
    TRY(in = from_str<Itype>("1+"));     TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), ">=1");
    TRY(in = from_str<Itype>("(1,5)"));  TEST_EQUAL(in.min(), 2);  TEST_EQUAL(in.max(), 4);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[2,4]");
    TRY(in = from_str<Itype>("[1,5)"));  TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 4);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[1,4]");
    TRY(in = from_str<Itype>("(1,5]"));  TEST_EQUAL(in.min(), 2);  TEST_EQUAL(in.max(), 5);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[2,5]");
    TRY(in = from_str<Itype>("[1,5]"));  TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 5);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[1,5]");
    TRY(in = from_str<Itype>("(5,1)"));  TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[5,1)"));  TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("(5,1]"));  TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[5,1]"));  TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("(1,1)"));  TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[1,1)"));  TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("(1,1]"));  TEST_EQUAL(in.min(), 0);  TEST_EQUAL(in.max(), 0);  TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[1,1]"));  TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 1);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "1");
    TRY(in = from_str<Itype>("1..<5"));  TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 4);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[1,4]");
    TRY(in = from_str<Itype>("1..5"));   TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 5);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[1,5]");
    TRY(in = from_str<Itype>("1...5"));  TEST_EQUAL(in.min(), 1);  TEST_EQUAL(in.max(), 5);  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[1,5]");

}

void test_core_interval_integral_iterators() {

    Itype in;
    Itype::iterator it1 = {}, it2 = {};
    ptrdiff_t diff = 0;

    TRY(in = {});           TEST_EQUAL(in.size(), 0);  TRY(it1 = in.begin());  TRY(it2 = in.end());  TRY(diff = std::distance(it1, it2));  TEST_EQUAL(diff, 0);
    TRY(in = 1);            TEST_EQUAL(in.size(), 1);  TRY(it1 = in.begin());  TRY(it2 = in.end());  TRY(diff = std::distance(it1, it2));  TEST_EQUAL(diff, 1);
    TRY(in = Itype(1, 5));  TEST_EQUAL(in.size(), 5);  TRY(it1 = in.begin());  TRY(it2 = in.end());  TRY(diff = std::distance(it1, it2));  TEST_EQUAL(diff, 5);

    TEST_EQUAL(*it1, 1);  TRY(++it1);
    TEST_EQUAL(*it1, 2);  TRY(++it1);
    TEST_EQUAL(*it1, 3);  TRY(++it1);
    TEST_EQUAL(*it1, 4);  TRY(++it1);
    TEST_EQUAL(*it1, 5);  TRY(++it1);
    TEST(it1 == it2);

}

void test_core_interval_integral_inverse() {

    Itype in;
    Iset set;
    Ustring str;

    TRY(in = {});                          TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{*}");
    TRY(in = Itype::all());                TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{}");
    TRY(in = from_str<Itype>("42"));       TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=41,>=43}");
    TRY(in = from_str<Itype>("<42"));      TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{>=42}");
    TRY(in = from_str<Itype>("<=42"));     TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{>=43}");
    TRY(in = from_str<Itype>(">42"));      TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=42}");
    TRY(in = from_str<Itype>(">=42"));     TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=41}");
    TRY(in = from_str<Itype>("(86,99)"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=86,>=99}");
    TRY(in = from_str<Itype>("(86,99]"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=86,>=100}");
    TRY(in = from_str<Itype>("[86,99)"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=85,>=99}");
    TRY(in = from_str<Itype>("[86,99]"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=85,>=100}");

}

void test_core_interval_integral_binary_operations() {

    struct test_info {
        int line;
        Ustring lhs;
        Ustring rhs;
        int compare;
        bool includes;
        bool overlaps;
        bool touches;
        Ustring envelope;
        Ustring set_intersection;
        Ustring set_union;
        Ustring set_difference;
        Ustring set_symmetric_difference;
    };

    static const std::vector<test_info> tests = {

        // line      lhs       rhs       cmp  incl    over    touch   envel     inter     union           diff            symm              --
        { __LINE__,  "{}",     "{}",     0,   false,  false,  false,  "{}",     "{}",     "{}",           "{}",           "{}",             },
        { __LINE__,  "*",      "{}",     1,   false,  false,  false,  "*",      "{}",     "{*}",          "{*}",          "{*}",            },
        { __LINE__,  "*",      "*",      0,   true,   true,   true,   "*",      "*",      "{*}",          "{}",           "{}",             },
        { __LINE__,  "3",      "{}",     1,   false,  false,  false,  "3",      "{}",     "{3}",          "{3}",          "{3}",            },
        { __LINE__,  "3",      "*",      1,   false,  true,   true,   "*",      "3",      "{*}",          "{}",           "{<=2,>=4}",      },
        { __LINE__,  "3",      "1",      1,   false,  false,  false,  "[1,3]",  "{}",     "{1,3}",        "{3}",          "{1,3}",          },
        { __LINE__,  "3",      "2",      1,   false,  false,  true,   "[2,3]",  "{}",     "{[2,3]}",      "{3}",          "{[2,3]}",        },
        { __LINE__,  "3",      "3",      0,   true,   true,   true,   "3",      "3",      "{3}",          "{}",           "{}",             },
        { __LINE__,  "3",      "4",      -1,  false,  false,  true,   "[3,4]",  "{}",     "{[3,4]}",      "{3}",          "{[3,4]}",        },
        { __LINE__,  "3",      "5",      -1,  false,  false,  false,  "[3,5]",  "{}",     "{3,5}",        "{3}",          "{3,5}",          },
        { __LINE__,  "<3",     "{}",     1,   false,  false,  false,  "<=2",    "{}",     "{<=2}",        "{<=2}",        "{<=2}",          },
        { __LINE__,  "<3",     "*",      -1,  false,  true,   true,   "*",      "<=2",    "{*}",          "{}",           "{>=3}",          },
        { __LINE__,  "<3",     "1",      -1,  true,   true,   true,   "<=2",    "1",      "{<=2}",        "{<=0,2}",      "{<=0,2}",        },
        { __LINE__,  "<3",     "2",      -1,  true,   true,   true,   "<=2",    "2",      "{<=2}",        "{<=1}",        "{<=1}",          },
        { __LINE__,  "<3",     "3",      -1,  false,  false,  true,   "<=3",    "{}",     "{<=3}",        "{<=2}",        "{<=3}",          },
        { __LINE__,  "<3",     "4",      -1,  false,  false,  false,  "<=4",    "{}",     "{<=2,4}",      "{<=2}",        "{<=2,4}",        },
        { __LINE__,  "<3",     "5",      -1,  false,  false,  false,  "<=5",    "{}",     "{<=2,5}",      "{<=2}",        "{<=2,5}",        },
        { __LINE__,  "<3",     "<1",     1,   true,   true,   true,   "<=2",    "<=0",    "{<=2}",        "{[1,2]}",      "{[1,2]}",        },
        { __LINE__,  "<3",     "<2",     1,   true,   true,   true,   "<=2",    "<=1",    "{<=2}",        "{2}",          "{2}",            },
        { __LINE__,  "<3",     "<3",     0,   true,   true,   true,   "<=2",    "<=2",    "{<=2}",        "{}",           "{}",             },
        { __LINE__,  "<3",     "<4",     -1,  false,  true,   true,   "<=3",    "<=2",    "{<=3}",        "{}",           "{3}",            },
        { __LINE__,  "<3",     "<5",     -1,  false,  true,   true,   "<=4",    "<=2",    "{<=4}",        "{}",           "{[3,4]}",        },
        { __LINE__,  "<=3",    "{}",     1,   false,  false,  false,  "<=3",    "{}",     "{<=3}",        "{<=3}",        "{<=3}",          },
        { __LINE__,  "<=3",    "*",      -1,  false,  true,   true,   "*",      "<=3",    "{*}",          "{}",           "{>=4}",          },
        { __LINE__,  "<=3",    "1",      -1,  true,   true,   true,   "<=3",    "1",      "{<=3}",        "{<=0,[2,3]}",  "{<=0,[2,3]}",    },
        { __LINE__,  "<=3",    "2",      -1,  true,   true,   true,   "<=3",    "2",      "{<=3}",        "{<=1,3}",      "{<=1,3}",        },
        { __LINE__,  "<=3",    "3",      -1,  true,   true,   true,   "<=3",    "3",      "{<=3}",        "{<=2}",        "{<=2}",          },
        { __LINE__,  "<=3",    "4",      -1,  false,  false,  true,   "<=4",    "{}",     "{<=4}",        "{<=3}",        "{<=4}",          },
        { __LINE__,  "<=3",    "5",      -1,  false,  false,  false,  "<=5",    "{}",     "{<=3,5}",      "{<=3}",        "{<=3,5}",        },
        { __LINE__,  "<=3",    "<1",     1,   true,   true,   true,   "<=3",    "<=0",    "{<=3}",        "{[1,3]}",      "{[1,3]}",        },
        { __LINE__,  "<=3",    "<2",     1,   true,   true,   true,   "<=3",    "<=1",    "{<=3}",        "{[2,3]}",      "{[2,3]}",        },
        { __LINE__,  "<=3",    "<3",     1,   true,   true,   true,   "<=3",    "<=2",    "{<=3}",        "{3}",          "{3}",            },
        { __LINE__,  "<=3",    "<4",     0,   true,   true,   true,   "<=3",    "<=3",    "{<=3}",        "{}",           "{}",             },
        { __LINE__,  "<=3",    "<5",     -1,  false,  true,   true,   "<=4",    "<=3",    "{<=4}",        "{}",           "{4}",            },
        { __LINE__,  "<=3",    "<=1",    1,   true,   true,   true,   "<=3",    "<=1",    "{<=3}",        "{[2,3]}",      "{[2,3]}",        },
        { __LINE__,  "<=3",    "<=2",    1,   true,   true,   true,   "<=3",    "<=2",    "{<=3}",        "{3}",          "{3}",            },
        { __LINE__,  "<=3",    "<=3",    0,   true,   true,   true,   "<=3",    "<=3",    "{<=3}",        "{}",           "{}",             },
        { __LINE__,  "<=3",    "<=4",    -1,  false,  true,   true,   "<=4",    "<=3",    "{<=4}",        "{}",           "{4}",            },
        { __LINE__,  "<=3",    "<=5",    -1,  false,  true,   true,   "<=5",    "<=3",    "{<=5}",        "{}",           "{[4,5]}",        },
        { __LINE__,  ">3",     "{}",     1,   false,  false,  false,  ">=4",    "{}",     "{>=4}",        "{>=4}",        "{>=4}",          },
        { __LINE__,  ">3",     "*",      1,   false,  true,   true,   "*",      ">=4",    "{*}",          "{}",           "{<=3}",          },
        { __LINE__,  ">3",     "1",      1,   false,  false,  false,  ">=1",    "{}",     "{1,>=4}",      "{>=4}",        "{1,>=4}",        },
        { __LINE__,  ">3",     "2",      1,   false,  false,  false,  ">=2",    "{}",     "{2,>=4}",      "{>=4}",        "{2,>=4}",        },
        { __LINE__,  ">3",     "3",      1,   false,  false,  true,   ">=3",    "{}",     "{>=3}",        "{>=4}",        "{>=3}",          },
        { __LINE__,  ">3",     "4",      1,   true,   true,   true,   ">=4",    "4",      "{>=4}",        "{>=5}",        "{>=5}",          },
        { __LINE__,  ">3",     "5",      -1,  true,   true,   true,   ">=4",    "5",      "{>=4}",        "{4,>=6}",      "{4,>=6}",        },
        { __LINE__,  ">3",     "<1",     1,   false,  false,  false,  "*",      "{}",     "{<=0,>=4}",    "{>=4}",        "{<=0,>=4}",      },
        { __LINE__,  ">3",     "<2",     1,   false,  false,  false,  "*",      "{}",     "{<=1,>=4}",    "{>=4}",        "{<=1,>=4}",      },
        { __LINE__,  ">3",     "<3",     1,   false,  false,  false,  "*",      "{}",     "{<=2,>=4}",    "{>=4}",        "{<=2,>=4}",      },
        { __LINE__,  ">3",     "<4",     1,   false,  false,  true,   "*",      "{}",     "{*}",          "{>=4}",        "{*}",            },
        { __LINE__,  ">3",     "<5",     1,   false,  true,   true,   "*",      "4",      "{*}",          "{>=5}",        "{<=3,>=5}",      },
        { __LINE__,  ">3",     "<=1",    1,   false,  false,  false,  "*",      "{}",     "{<=1,>=4}",    "{>=4}",        "{<=1,>=4}",      },
        { __LINE__,  ">3",     "<=2",    1,   false,  false,  false,  "*",      "{}",     "{<=2,>=4}",    "{>=4}",        "{<=2,>=4}",      },
        { __LINE__,  ">3",     "<=3",    1,   false,  false,  true,   "*",      "{}",     "{*}",          "{>=4}",        "{*}",            },
        { __LINE__,  ">3",     "<=4",    1,   false,  true,   true,   "*",      "4",      "{*}",          "{>=5}",        "{<=3,>=5}",      },
        { __LINE__,  ">3",     "<=5",    1,   false,  true,   true,   "*",      "[4,5]",  "{*}",          "{>=6}",        "{<=3,>=6}",      },
        { __LINE__,  ">3",     ">1",     1,   false,  true,   true,   ">=2",    ">=4",    "{>=2}",        "{}",           "{[2,3]}",        },
        { __LINE__,  ">3",     ">2",     1,   false,  true,   true,   ">=3",    ">=4",    "{>=3}",        "{}",           "{3}",            },
        { __LINE__,  ">3",     ">3",     0,   true,   true,   true,   ">=4",    ">=4",    "{>=4}",        "{}",           "{}",             },
        { __LINE__,  ">3",     ">4",     -1,  true,   true,   true,   ">=4",    ">=5",    "{>=4}",        "{4}",          "{4}",            },
        { __LINE__,  ">3",     ">5",     -1,  true,   true,   true,   ">=4",    ">=6",    "{>=4}",        "{[4,5]}",      "{[4,5]}",        },
        { __LINE__,  ">=3",    "{}",     1,   false,  false,  false,  ">=3",    "{}",     "{>=3}",        "{>=3}",        "{>=3}",          },
        { __LINE__,  ">=3",    "*",      1,   false,  true,   true,   "*",      ">=3",    "{*}",          "{}",           "{<=2}",          },
        { __LINE__,  ">=3",    "1",      1,   false,  false,  false,  ">=1",    "{}",     "{1,>=3}",      "{>=3}",        "{1,>=3}",        },
        { __LINE__,  ">=3",    "2",      1,   false,  false,  true,   ">=2",    "{}",     "{>=2}",        "{>=3}",        "{>=2}",          },
        { __LINE__,  ">=3",    "3",      1,   true,   true,   true,   ">=3",    "3",      "{>=3}",        "{>=4}",        "{>=4}",          },
        { __LINE__,  ">=3",    "4",      -1,  true,   true,   true,   ">=3",    "4",      "{>=3}",        "{3,>=5}",      "{3,>=5}",        },
        { __LINE__,  ">=3",    "5",      -1,  true,   true,   true,   ">=3",    "5",      "{>=3}",        "{[3,4],>=6}",  "{[3,4],>=6}",    },
        { __LINE__,  ">=3",    "<1",     1,   false,  false,  false,  "*",      "{}",     "{<=0,>=3}",    "{>=3}",        "{<=0,>=3}",      },
        { __LINE__,  ">=3",    "<2",     1,   false,  false,  false,  "*",      "{}",     "{<=1,>=3}",    "{>=3}",        "{<=1,>=3}",      },
        { __LINE__,  ">=3",    "<3",     1,   false,  false,  true,   "*",      "{}",     "{*}",          "{>=3}",        "{*}",            },
        { __LINE__,  ">=3",    "<4",     1,   false,  true,   true,   "*",      "3",      "{*}",          "{>=4}",        "{<=2,>=4}",      },
        { __LINE__,  ">=3",    "<5",     1,   false,  true,   true,   "*",      "[3,4]",  "{*}",          "{>=5}",        "{<=2,>=5}",      },
        { __LINE__,  ">=3",    "<=1",    1,   false,  false,  false,  "*",      "{}",     "{<=1,>=3}",    "{>=3}",        "{<=1,>=3}",      },
        { __LINE__,  ">=3",    "<=2",    1,   false,  false,  true,   "*",      "{}",     "{*}",          "{>=3}",        "{*}",            },
        { __LINE__,  ">=3",    "<=3",    1,   false,  true,   true,   "*",      "3",      "{*}",          "{>=4}",        "{<=2,>=4}",      },
        { __LINE__,  ">=3",    "<=4",    1,   false,  true,   true,   "*",      "[3,4]",  "{*}",          "{>=5}",        "{<=2,>=5}",      },
        { __LINE__,  ">=3",    "<=5",    1,   false,  true,   true,   "*",      "[3,5]",  "{*}",          "{>=6}",        "{<=2,>=6}",      },
        { __LINE__,  ">=3",    ">1",     1,   false,  true,   true,   ">=2",    ">=3",    "{>=2}",        "{}",           "{2}",            },
        { __LINE__,  ">=3",    ">2",     0,   true,   true,   true,   ">=3",    ">=3",    "{>=3}",        "{}",           "{}",             },
        { __LINE__,  ">=3",    ">3",     -1,  true,   true,   true,   ">=3",    ">=4",    "{>=3}",        "{3}",          "{3}",            },
        { __LINE__,  ">=3",    ">4",     -1,  true,   true,   true,   ">=3",    ">=5",    "{>=3}",        "{[3,4]}",      "{[3,4]}",        },
        { __LINE__,  ">=3",    ">5",     -1,  true,   true,   true,   ">=3",    ">=6",    "{>=3}",        "{[3,5]}",      "{[3,5]}",        },
        { __LINE__,  ">=3",    ">=1",    1,   false,  true,   true,   ">=1",    ">=3",    "{>=1}",        "{}",           "{[1,2]}",        },
        { __LINE__,  ">=3",    ">=2",    1,   false,  true,   true,   ">=2",    ">=3",    "{>=2}",        "{}",           "{2}",            },
        { __LINE__,  ">=3",    ">=3",    0,   true,   true,   true,   ">=3",    ">=3",    "{>=3}",        "{}",           "{}",             },
        { __LINE__,  ">=3",    ">=4",    -1,  true,   true,   true,   ">=3",    ">=4",    "{>=3}",        "{3}",          "{3}",            },
        { __LINE__,  ">=3",    ">=5",    -1,  true,   true,   true,   ">=3",    ">=5",    "{>=3}",        "{[3,4]}",      "{[3,4]}",        },
        { __LINE__,  "[3,6]",  "{}",     1,   false,  false,  false,  "[3,6]",  "{}",     "{[3,6]}",      "{[3,6]}",      "{[3,6]}",        },
        { __LINE__,  "[3,6]",  "*",      1,   false,  true,   true,   "*",      "[3,6]",  "{*}",          "{}",           "{<=2,>=7}",      },
        { __LINE__,  "[3,6]",  "1",      1,   false,  false,  false,  "[1,6]",  "{}",     "{1,[3,6]}",    "{[3,6]}",      "{1,[3,6]}",      },
        { __LINE__,  "[3,6]",  "2",      1,   false,  false,  true,   "[2,6]",  "{}",     "{[2,6]}",      "{[3,6]}",      "{[2,6]}",        },
        { __LINE__,  "[3,6]",  "3",      1,   true,   true,   true,   "[3,6]",  "3",      "{[3,6]}",      "{[4,6]}",      "{[4,6]}",        },
        { __LINE__,  "[3,6]",  "4",      -1,  true,   true,   true,   "[3,6]",  "4",      "{[3,6]}",      "{3,[5,6]}",    "{3,[5,6]}",      },
        { __LINE__,  "[3,6]",  "5",      -1,  true,   true,   true,   "[3,6]",  "5",      "{[3,6]}",      "{[3,4],6}",    "{[3,4],6}",      },
        { __LINE__,  "[3,6]",  "6",      -1,  true,   true,   true,   "[3,6]",  "6",      "{[3,6]}",      "{[3,5]}",      "{[3,5]}",        },
        { __LINE__,  "[3,6]",  "7",      -1,  false,  false,  true,   "[3,7]",  "{}",     "{[3,7]}",      "{[3,6]}",      "{[3,7]}",        },
        { __LINE__,  "[3,6]",  "8",      -1,  false,  false,  false,  "[3,8]",  "{}",     "{[3,6],8}",    "{[3,6]}",      "{[3,6],8}",      },
        { __LINE__,  "[3,6]",  "<1",     1,   false,  false,  false,  "<=6",    "{}",     "{<=0,[3,6]}",  "{[3,6]}",      "{<=0,[3,6]}",    },
        { __LINE__,  "[3,6]",  "<2",     1,   false,  false,  false,  "<=6",    "{}",     "{<=1,[3,6]}",  "{[3,6]}",      "{<=1,[3,6]}",    },
        { __LINE__,  "[3,6]",  "<3",     1,   false,  false,  true,   "<=6",    "{}",     "{<=6}",        "{[3,6]}",      "{<=6}",          },
        { __LINE__,  "[3,6]",  "<4",     1,   false,  true,   true,   "<=6",    "3",      "{<=6}",        "{[4,6]}",      "{<=2,[4,6]}",    },
        { __LINE__,  "[3,6]",  "<5",     1,   false,  true,   true,   "<=6",    "[3,4]",  "{<=6}",        "{[5,6]}",      "{<=2,[5,6]}",    },
        { __LINE__,  "[3,6]",  "<6",     1,   false,  true,   true,   "<=6",    "[3,5]",  "{<=6}",        "{6}",          "{<=2,6}",        },
        { __LINE__,  "[3,6]",  "<7",     1,   false,  true,   true,   "<=6",    "[3,6]",  "{<=6}",        "{}",           "{<=2}",          },
        { __LINE__,  "[3,6]",  "<8",     1,   false,  true,   true,   "<=7",    "[3,6]",  "{<=7}",        "{}",           "{<=2,7}",        },
        { __LINE__,  "[3,6]",  "<=1",    1,   false,  false,  false,  "<=6",    "{}",     "{<=1,[3,6]}",  "{[3,6]}",      "{<=1,[3,6]}",    },
        { __LINE__,  "[3,6]",  "<=2",    1,   false,  false,  true,   "<=6",    "{}",     "{<=6}",        "{[3,6]}",      "{<=6}",          },
        { __LINE__,  "[3,6]",  "<=3",    1,   false,  true,   true,   "<=6",    "3",      "{<=6}",        "{[4,6]}",      "{<=2,[4,6]}",    },
        { __LINE__,  "[3,6]",  "<=4",    1,   false,  true,   true,   "<=6",    "[3,4]",  "{<=6}",        "{[5,6]}",      "{<=2,[5,6]}",    },
        { __LINE__,  "[3,6]",  "<=5",    1,   false,  true,   true,   "<=6",    "[3,5]",  "{<=6}",        "{6}",          "{<=2,6}",        },
        { __LINE__,  "[3,6]",  "<=6",    1,   false,  true,   true,   "<=6",    "[3,6]",  "{<=6}",        "{}",           "{<=2}",          },
        { __LINE__,  "[3,6]",  "<=7",    1,   false,  true,   true,   "<=7",    "[3,6]",  "{<=7}",        "{}",           "{<=2,7}",        },
        { __LINE__,  "[3,6]",  "<=8",    1,   false,  true,   true,   "<=8",    "[3,6]",  "{<=8}",        "{}",           "{<=2,[7,8]}",    },
        { __LINE__,  "[3,6]",  ">1",     1,   false,  true,   true,   ">=2",    "[3,6]",  "{>=2}",        "{}",           "{2,>=7}",        },
        { __LINE__,  "[3,6]",  ">2",     -1,  false,  true,   true,   ">=3",    "[3,6]",  "{>=3}",        "{}",           "{>=7}",          },
        { __LINE__,  "[3,6]",  ">3",     -1,  false,  true,   true,   ">=3",    "[4,6]",  "{>=3}",        "{3}",          "{3,>=7}",        },
        { __LINE__,  "[3,6]",  ">4",     -1,  false,  true,   true,   ">=3",    "[5,6]",  "{>=3}",        "{[3,4]}",      "{[3,4],>=7}",    },
        { __LINE__,  "[3,6]",  ">5",     -1,  false,  true,   true,   ">=3",    "6",      "{>=3}",        "{[3,5]}",      "{[3,5],>=7}",    },
        { __LINE__,  "[3,6]",  ">6",     -1,  false,  false,  true,   ">=3",    "{}",     "{>=3}",        "{[3,6]}",      "{>=3}",          },
        { __LINE__,  "[3,6]",  ">7",     -1,  false,  false,  false,  ">=3",    "{}",     "{[3,6],>=8}",  "{[3,6]}",      "{[3,6],>=8}",    },
        { __LINE__,  "[3,6]",  ">8",     -1,  false,  false,  false,  ">=3",    "{}",     "{[3,6],>=9}",  "{[3,6]}",      "{[3,6],>=9}",    },
        { __LINE__,  "[3,6]",  ">=1",    1,   false,  true,   true,   ">=1",    "[3,6]",  "{>=1}",        "{}",           "{[1,2],>=7}",    },
        { __LINE__,  "[3,6]",  ">=2",    1,   false,  true,   true,   ">=2",    "[3,6]",  "{>=2}",        "{}",           "{2,>=7}",        },
        { __LINE__,  "[3,6]",  ">=3",    -1,  false,  true,   true,   ">=3",    "[3,6]",  "{>=3}",        "{}",           "{>=7}",          },
        { __LINE__,  "[3,6]",  ">=4",    -1,  false,  true,   true,   ">=3",    "[4,6]",  "{>=3}",        "{3}",          "{3,>=7}",        },
        { __LINE__,  "[3,6]",  ">=5",    -1,  false,  true,   true,   ">=3",    "[5,6]",  "{>=3}",        "{[3,4]}",      "{[3,4],>=7}",    },
        { __LINE__,  "[3,6]",  ">=6",    -1,  false,  true,   true,   ">=3",    "6",      "{>=3}",        "{[3,5]}",      "{[3,5],>=7}",    },
        { __LINE__,  "[3,6]",  ">=7",    -1,  false,  false,  true,   ">=3",    "{}",     "{>=3}",        "{[3,6]}",      "{>=3}",          },
        { __LINE__,  "[3,6]",  ">=8",    -1,  false,  false,  false,  ">=3",    "{}",     "{[3,6],>=8}",  "{[3,6]}",      "{[3,6],>=8}",    },
        { __LINE__,  "[3,6]",  "[1,1]",  1,   false,  false,  false,  "[1,6]",  "{}",     "{1,[3,6]}",    "{[3,6]}",      "{1,[3,6]}",      },
        { __LINE__,  "[3,6]",  "[1,2]",  1,   false,  false,  true,   "[1,6]",  "{}",     "{[1,6]}",      "{[3,6]}",      "{[1,6]}",        },
        { __LINE__,  "[3,6]",  "[1,3]",  1,   false,  true,   true,   "[1,6]",  "3",      "{[1,6]}",      "{[4,6]}",      "{[1,2],[4,6]}",  },
        { __LINE__,  "[3,6]",  "[1,4]",  1,   false,  true,   true,   "[1,6]",  "[3,4]",  "{[1,6]}",      "{[5,6]}",      "{[1,2],[5,6]}",  },
        { __LINE__,  "[3,6]",  "[1,5]",  1,   false,  true,   true,   "[1,6]",  "[3,5]",  "{[1,6]}",      "{6}",          "{[1,2],6}",      },
        { __LINE__,  "[3,6]",  "[1,6]",  1,   false,  true,   true,   "[1,6]",  "[3,6]",  "{[1,6]}",      "{}",           "{[1,2]}",        },
        { __LINE__,  "[3,6]",  "[1,7]",  1,   false,  true,   true,   "[1,7]",  "[3,6]",  "{[1,7]}",      "{}",           "{[1,2],7}",      },
        { __LINE__,  "[3,6]",  "[1,8]",  1,   false,  true,   true,   "[1,8]",  "[3,6]",  "{[1,8]}",      "{}",           "{[1,2],[7,8]}",  },
        { __LINE__,  "[3,6]",  "[2,2]",  1,   false,  false,  true,   "[2,6]",  "{}",     "{[2,6]}",      "{[3,6]}",      "{[2,6]}",        },
        { __LINE__,  "[3,6]",  "[2,3]",  1,   false,  true,   true,   "[2,6]",  "3",      "{[2,6]}",      "{[4,6]}",      "{2,[4,6]}",      },
        { __LINE__,  "[3,6]",  "[2,4]",  1,   false,  true,   true,   "[2,6]",  "[3,4]",  "{[2,6]}",      "{[5,6]}",      "{2,[5,6]}",      },
        { __LINE__,  "[3,6]",  "[2,5]",  1,   false,  true,   true,   "[2,6]",  "[3,5]",  "{[2,6]}",      "{6}",          "{2,6}",          },
        { __LINE__,  "[3,6]",  "[2,6]",  1,   false,  true,   true,   "[2,6]",  "[3,6]",  "{[2,6]}",      "{}",           "{2}",            },
        { __LINE__,  "[3,6]",  "[2,7]",  1,   false,  true,   true,   "[2,7]",  "[3,6]",  "{[2,7]}",      "{}",           "{2,7}",          },
        { __LINE__,  "[3,6]",  "[2,8]",  1,   false,  true,   true,   "[2,8]",  "[3,6]",  "{[2,8]}",      "{}",           "{2,[7,8]}",      },
        { __LINE__,  "[3,6]",  "[3,3]",  1,   true,   true,   true,   "[3,6]",  "3",      "{[3,6]}",      "{[4,6]}",      "{[4,6]}",        },
        { __LINE__,  "[3,6]",  "[3,4]",  1,   true,   true,   true,   "[3,6]",  "[3,4]",  "{[3,6]}",      "{[5,6]}",      "{[5,6]}",        },
        { __LINE__,  "[3,6]",  "[3,5]",  1,   true,   true,   true,   "[3,6]",  "[3,5]",  "{[3,6]}",      "{6}",          "{6}",            },
        { __LINE__,  "[3,6]",  "[3,6]",  0,   true,   true,   true,   "[3,6]",  "[3,6]",  "{[3,6]}",      "{}",           "{}",             },
        { __LINE__,  "[3,6]",  "[3,7]",  -1,  false,  true,   true,   "[3,7]",  "[3,6]",  "{[3,7]}",      "{}",           "{7}",            },
        { __LINE__,  "[3,6]",  "[3,8]",  -1,  false,  true,   true,   "[3,8]",  "[3,6]",  "{[3,8]}",      "{}",           "{[7,8]}",        },
        { __LINE__,  "[3,6]",  "[4,4]",  -1,  true,   true,   true,   "[3,6]",  "4",      "{[3,6]}",      "{3,[5,6]}",    "{3,[5,6]}",      },
        { __LINE__,  "[3,6]",  "[4,5]",  -1,  true,   true,   true,   "[3,6]",  "[4,5]",  "{[3,6]}",      "{3,6}",        "{3,6}",          },
        { __LINE__,  "[3,6]",  "[4,6]",  -1,  true,   true,   true,   "[3,6]",  "[4,6]",  "{[3,6]}",      "{3}",          "{3}",            },
        { __LINE__,  "[3,6]",  "[4,7]",  -1,  false,  true,   true,   "[3,7]",  "[4,6]",  "{[3,7]}",      "{3}",          "{3,7}",          },
        { __LINE__,  "[3,6]",  "[4,8]",  -1,  false,  true,   true,   "[3,8]",  "[4,6]",  "{[3,8]}",      "{3}",          "{3,[7,8]}",      },
        { __LINE__,  "[3,6]",  "[5,5]",  -1,  true,   true,   true,   "[3,6]",  "5",      "{[3,6]}",      "{[3,4],6}",    "{[3,4],6}",      },
        { __LINE__,  "[3,6]",  "[5,6]",  -1,  true,   true,   true,   "[3,6]",  "[5,6]",  "{[3,6]}",      "{[3,4]}",      "{[3,4]}",        },
        { __LINE__,  "[3,6]",  "[5,7]",  -1,  false,  true,   true,   "[3,7]",  "[5,6]",  "{[3,7]}",      "{[3,4]}",      "{[3,4],7}",      },
        { __LINE__,  "[3,6]",  "[5,8]",  -1,  false,  true,   true,   "[3,8]",  "[5,6]",  "{[3,8]}",      "{[3,4]}",      "{[3,4],[7,8]}",  },
        { __LINE__,  "[3,6]",  "[6,6]",  -1,  true,   true,   true,   "[3,6]",  "6",      "{[3,6]}",      "{[3,5]}",      "{[3,5]}",        },
        { __LINE__,  "[3,6]",  "[6,7]",  -1,  false,  true,   true,   "[3,7]",  "6",      "{[3,7]}",      "{[3,5]}",      "{[3,5],7}",      },
        { __LINE__,  "[3,6]",  "[6,8]",  -1,  false,  true,   true,   "[3,8]",  "6",      "{[3,8]}",      "{[3,5]}",      "{[3,5],[7,8]}",  },
        { __LINE__,  "[3,6]",  "[7,7]",  -1,  false,  false,  true,   "[3,7]",  "{}",     "{[3,7]}",      "{[3,6]}",      "{[3,7]}",        },
        { __LINE__,  "[3,6]",  "[7,8]",  -1,  false,  false,  true,   "[3,8]",  "{}",     "{[3,8]}",      "{[3,6]}",      "{[3,8]}",        },
        { __LINE__,  "[3,6]",  "[8,8]",  -1,  false,  false,  false,  "[3,8]",  "{}",     "{[3,6],8}",    "{[3,6]}",      "{[3,6],8}",      },

    };

    Itype in1, in2;
    int cmp = 0;
    bool flag = false;
    Ustring str;

    for (auto& t: tests) {

        int errors = 0;

        TRY(in1 = from_str<Itype>(t.lhs));
        TRY(in2 = from_str<Itype>(t.rhs));

        TRY(cmp = in1.compare(in2));                           TEST_EQUAL(cmp, t.compare);                   errors += int(cmp != t.compare);
        TRY(flag = in1.includes(in2));                         TEST_EQUAL(flag, t.includes);                 errors += int(flag != t.includes);
        TRY(flag = in1.overlaps(in2));                         TEST_EQUAL(flag, t.overlaps);                 errors += int(flag != t.overlaps);
        TRY(flag = in1.touches(in2));                          TEST_EQUAL(flag, t.touches);                  errors += int(flag != t.touches);
        TRY(str = to_str(in1.envelope(in2)));                  TEST_EQUAL(str, t.envelope);                  errors += int(str != t.envelope);
        TRY(str = to_str(in1.set_intersection(in2)));          TEST_EQUAL(str, t.set_intersection);          errors += int(str != t.set_intersection);
        TRY(str = to_str(in1.set_union(in2)));                 TEST_EQUAL(str, t.set_union);                 errors += int(str != t.set_union);
        TRY(str = to_str(in1.set_difference(in2)));            TEST_EQUAL(str, t.set_difference);            errors += int(str != t.set_difference);
        TRY(str = to_str(in1.set_symmetric_difference(in2)));  TEST_EQUAL(str, t.set_symmetric_difference);  errors += int(str != t.set_symmetric_difference);

        if (errors)
            std::cout << "... [" << t.line << "] " << t.lhs << " " << t.rhs << "\n";

    }

}

void test_core_interval_integral_arithmetic() {

    Itype in;
    std::string str;

    TRY(in = + Itype());            TRY(str = to_str(in));  TEST_EQUAL(str, "{}");
    TRY(in = + Itype::all());       TRY(str = to_str(in));  TEST_EQUAL(str, "*");
    TRY(in = + Itype(1));           TRY(str = to_str(in));  TEST_EQUAL(str, "1");
    TRY(in = - Itype());            TRY(str = to_str(in));  TEST_EQUAL(str, "{}");
    TRY(in = - Itype::all());       TRY(str = to_str(in));  TEST_EQUAL(str, "*");
    TRY(in = - Itype(1));           TRY(str = to_str(in));  TEST_EQUAL(str, "-1");
    TRY(in = - Itype(1, 5, "[]"));  TRY(str = to_str(in));  TEST_EQUAL(str, "[-5,-1]");
    TRY(in = - Itype(1, 5, "()"));  TRY(str = to_str(in));  TEST_EQUAL(str, "[-4,-2]");
    TRY(in = - Itype(1, 5, "[)"));  TRY(str = to_str(in));  TEST_EQUAL(str, "[-4,-1]");
    TRY(in = - Itype(1, 5, "(]"));  TRY(str = to_str(in));  TEST_EQUAL(str, "[-5,-2]");
    TRY(in = - Itype(0, 5, "<"));   TRY(str = to_str(in));  TEST_EQUAL(str, ">=-4");
    TRY(in = - Itype(0, 5, "<="));  TRY(str = to_str(in));  TEST_EQUAL(str, ">=-5");
    TRY(in = - Itype(5, 0, ">"));   TRY(str = to_str(in));  TEST_EQUAL(str, "<=-6");
    TRY(in = - Itype(5, 0, ">="));  TRY(str = to_str(in));  TEST_EQUAL(str, "<=-5");

    TRY(in = Itype() + Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");
    TRY(in = Itype() - Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");
    TRY(in = Itype() * Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");
    TRY(in = Itype() / Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");

    TRY(in = Itype());  TRY(in += Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");
    TRY(in = Itype());  TRY(in -= Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");
    TRY(in = Itype());  TRY(in *= Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");
    TRY(in = Itype());  TRY(in /= Itype());  TRY(str = to_str(in));  //TEST_EQUAL(str, "");

}

void test_core_interval_integral_set_construct_insert_erase() {

    Iset set, inv;
    Iset::iterator it;
    Itype in;
    Ustring str;

    TEST(set.empty());
    TRY(set = 42);
    TEST_EQUAL(set.size(), 1);
    TRY(it = set.begin());
    TEST_EQUAL(it->str(), "42");
    TRY(++it);
    TEST(it == set.end());
    TRY((in = {5,10}));
    TRY(set = in);
    TEST_EQUAL(set.size(), 1);
    TRY(it = set.begin());
    TEST_EQUAL(it->str(), "[5,10]");
    TRY(++it);
    TEST(it == set.end());
    TRY((set = {{5,10},{15,20},{25,30}}));
    TRY(it = set.begin());
    TEST_EQUAL(it->str(), "[5,10]");
    TRY(++it);
    TEST_EQUAL(it->str(), "[15,20]");
    TRY(++it);
    TEST_EQUAL(it->str(), "[25,30]");
    TRY(++it);
    TEST(it == set.end());

    TRY(set = from_str<Iset>(""));                         TRY(str = to_str(set));  TEST_EQUAL(str, "{}");
    TRY(set = from_str<Iset>("{}"));                       TRY(str = to_str(set));  TEST_EQUAL(str, "{}");
    TRY(set = from_str<Iset>("(3,6)"));                    TRY(str = to_str(set));  TEST_EQUAL(str, "{[4,5]}");
    TRY(set = from_str<Iset>("{(3,6)}"));                  TRY(str = to_str(set));  TEST_EQUAL(str, "{[4,5]}");
    TRY(set = from_str<Iset>("{(0,5),(10,15),(20,25)}"));  TRY(str = to_str(set));  TEST_EQUAL(str, "{[1,4],[11,14],[21,24]}");

    TRY((set = {{3,6},{9,12},{15,18}}));
    TRY(inv = set.inverse());
    TRY(str = to_str(set));
    TEST_EQUAL(str, "{[3,6],[9,12],[15,18]}");
    TRY(str = to_str(inv));
    TEST_EQUAL(str, "{<=2,[7,8],[13,14],>=19}");

    TEST(! set.contains(1));   TEST(inv.contains(1));
    TEST(! set.contains(2));   TEST(inv.contains(2));
    TEST(set.contains(3));     TEST(! inv.contains(3));
    TEST(set.contains(4));     TEST(! inv.contains(4));
    TEST(set.contains(5));     TEST(! inv.contains(5));
    TEST(set.contains(6));     TEST(! inv.contains(6));
    TEST(! set.contains(7));   TEST(inv.contains(7));
    TEST(! set.contains(8));   TEST(inv.contains(8));
    TEST(set.contains(9));     TEST(! inv.contains(9));
    TEST(set.contains(10));    TEST(! inv.contains(10));
    TEST(set.contains(11));    TEST(! inv.contains(11));
    TEST(set.contains(12));    TEST(! inv.contains(12));
    TEST(! set.contains(13));  TEST(inv.contains(13));
    TEST(! set.contains(14));  TEST(inv.contains(14));
    TEST(set.contains(15));    TEST(! inv.contains(15));
    TEST(set.contains(16));    TEST(! inv.contains(16));
    TEST(set.contains(17));    TEST(! inv.contains(17));
    TEST(set.contains(18));    TEST(! inv.contains(18));
    TEST(! set.contains(19));  TEST(inv.contains(19));
    TEST(! set.contains(20));  TEST(inv.contains(20));

    TRY(set.clear());
    TEST(set.empty());

    TRY((set.insert({10,20})));       TRY(str = to_str(set));  TEST_EQUAL(str, "{[10,20]}");
    TRY((set.insert({20,30,"()"})));  TRY(str = to_str(set));  TEST_EQUAL(str, "{[10,29]}");
    TRY((set.erase({5,10,"()"})));    TRY(str = to_str(set));  TEST_EQUAL(str, "{[10,29]}");
    TRY((set.erase({5,10})));         TRY(str = to_str(set));  TEST_EQUAL(str, "{[11,29]}");
    TRY((set.erase({12,14})));        TRY(str = to_str(set));  TEST_EQUAL(str, "{11,[15,29]}");
    TRY((set.erase({16,18,"()"})));   TRY(str = to_str(set));  TEST_EQUAL(str, "{11,[15,16],[18,29]}");
    TRY((set.insert({9,11})));        TRY(str = to_str(set));  TEST_EQUAL(str, "{[9,11],[15,16],[18,29]}");
    TRY((set.insert({29,31,"()"})));  TRY(str = to_str(set));  TEST_EQUAL(str, "{[9,11],[15,16],[18,30]}");

}

void test_core_interval_integral_set_operations() {

    Iset set[2], i_set, u_set, d_set, sd_set;
    std::vector<Itype> vec[2];
    Iset::iterator it;
    Itype in;
    Xoshiro rng(42);

    static constexpr int iterations = 1000;
    static constexpr int max_size = 10;
    static constexpr int max_value = 50;

    for (int i = 0; i < iterations; ++i) {

        for (int j = 0; j < 2; ++j) {
            TRY(set[j].clear());
            vec[j].clear();
            int size = random_integer(1, max_size)(rng);
            for (int k = 0; k < size; ++k) {
                int a = random_integer(1, max_value)(rng);
                int b = random_integer(1, max_value)(rng);
                auto l = IB(random_integer(0, 3)(rng));
                auto r = IB(random_integer(0, 3)(rng));
                if ((l == IB::empty) == (r == IB::empty)) {
                    TRY(in = Itype(a, b, l, r));
                    TRY(set[j].insert(in));
                    vec[j].push_back(in);
                }
            }
        }

        TRY(i_set = set[0] & set[1]);
        TRY(u_set = set[0] | set[1]);
        TRY(d_set = set[0] - set[1]);
        TRY(sd_set = set[0] ^ set[1]);

        for (int x = 0; x <= max_value + 1; ++x) {
            bool member[2];
            for (int j = 0; j < 2; ++j) {
                member[j] = false;
                for (auto& item: vec[j])
                    member[j] |= item(x);
            }
            bool i_expect = member[0] && member[1], i_test = false;
            bool u_expect = member[0] || member[1], u_test = false;
            bool d_expect = member[0] && ! member[1], d_test = false;
            bool sd_expect = member[0] != member[1], sd_test = false;
            TRY(i_test = i_set(x));
            TRY(u_test = u_set(x));
            TRY(d_test = d_set(x));
            TRY(sd_test = sd_set(x));
            TEST_EQUAL(i_test, i_expect);
            TEST_EQUAL(u_test, u_expect);
            TEST_EQUAL(d_test, d_expect);
            TEST_EQUAL(sd_test, sd_expect);
            if ((i_test != i_expect) || (u_test != u_expect) || (d_test != d_expect) || (sd_test != sd_expect)) {
                for (int j = 0; j < 2; ++j)
                    std::cout << "... " << to_str(vec[j]) << " => " << set[j] << "\n";
                std::cout << "... x=" << x << "\n";
            }
        }

    }

}

void test_core_interval_integral_map() {

    Imap map;
    Imap::iterator it;

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0);
    TEST_EQUAL(map.default_value(), "");
    TEST_EQUAL(map[42], "");
    TEST_EQUAL(to_str(map), "{}");

    TRY(map.insert(from_str<Itype>("<=2"), "alpha"));
    TRY(map.insert(from_str<Itype>("(3,7)"), "bravo"));
    TRY(map.insert(from_str<Itype>(">=8"), "charlie"));
    TEST_EQUAL(map.size(), 3);
    TEST_EQUAL(map[1], "alpha");
    TEST_EQUAL(map[2], "alpha");
    TEST_EQUAL(map[3], "");
    TEST_EQUAL(map[4], "bravo");
    TEST_EQUAL(map[5], "bravo");
    TEST_EQUAL(map[6], "bravo");
    TEST_EQUAL(map[7], "");
    TEST_EQUAL(map[8], "charlie");
    TEST_EQUAL(map[9], "charlie");
    TRY(map.default_value("nil"));
    TEST_EQUAL(map[1], "alpha");
    TEST_EQUAL(map[2], "alpha");
    TEST_EQUAL(map[3], "nil");
    TEST_EQUAL(map[4], "bravo");
    TEST_EQUAL(map[5], "bravo");
    TEST_EQUAL(map[6], "bravo");
    TEST_EQUAL(map[7], "nil");
    TEST_EQUAL(map[8], "charlie");
    TEST_EQUAL(map[9], "charlie");
    TEST_EQUAL(to_str(map), "{<=2:alpha,[4,6]:bravo,>=8:charlie}");

    TEST(map.contains(1));
    TEST(map.contains(2));
    TEST(! map.contains(3));
    TEST(map.contains(4));
    TEST(map.contains(5));
    TEST(map.contains(6));
    TEST(! map.contains(7));
    TEST(map.contains(8));
    TEST(map.contains(9));

    TRY(it = map.find(1));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.find(2));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.find(3));  TEST(it == map.end());
    TRY(it = map.find(4));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.find(5));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.find(6));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.find(7));  TEST(it == map.end());
    TRY(it = map.find(8));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.find(9));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");

    TRY(it = map.lower_bound(1));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.lower_bound(2));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.lower_bound(3));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound(4));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound(5));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound(6));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound(7));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.lower_bound(8));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.lower_bound(9));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");

    TRY(it = map.upper_bound(1));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.upper_bound(2));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.upper_bound(3));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.upper_bound(4));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound(5));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound(6));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound(7));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound(8));  TEST(it == map.end());
    TRY(it = map.upper_bound(9));  TEST(it == map.end());

    TRY(map.clear());
    TEST(map.empty());
    TRY((map = {
        {from_str<Itype>("<=20"), "alpha"},
        {from_str<Itype>("(30,70)"), "bravo"},
        {from_str<Itype>(">=80"), "charlie"},
    }));
    TEST_EQUAL(map.size(), 3);
    TEST_EQUAL(to_str(map), "{<=20:alpha,[31,69]:bravo,>=80:charlie}");

    TRY(map.clear());
    TRY(map.default_value("nil"));
    TRY(map.insert(from_str<Itype>("<=10"), "alpha"));
    TRY(map.insert(from_str<Itype>("[1,5]"), "bravo"));
    TRY(map.insert(from_str<Itype>("[5,6]"), "charlie"));
    TRY(map.insert(from_str<Itype>("[2,3]"), "delta"));
    TRY(map.insert(from_str<Itype>("[7,8]"), "charlie"));
    TEST_EQUAL(map.size(), 6);
    TEST_EQUAL(to_str(map),
        "{<=0:alpha,"
        "1:bravo,"
        "[2,3]:delta,"
        "4:bravo,"
        "[5,8]:charlie,"
        "[9,10]:alpha}");
    TEST_EQUAL(map[0], "alpha");
    TEST_EQUAL(map[1], "bravo");
    TEST_EQUAL(map[2], "delta");
    TEST_EQUAL(map[3], "delta");
    TEST_EQUAL(map[4], "bravo");
    TEST_EQUAL(map[5], "charlie");
    TEST_EQUAL(map[6], "charlie");
    TEST_EQUAL(map[7], "charlie");
    TEST_EQUAL(map[8], "charlie");
    TEST_EQUAL(map[9], "alpha");
    TEST_EQUAL(map[10], "alpha");
    TEST_EQUAL(map[11], "nil");
    TEST_EQUAL(map[12], "nil");

    TRY(map.erase(from_str<Itype>("[1,2]")));
    TRY(map.erase(from_str<Itype>("[6,7]")));
    TRY(map.erase(from_str<Itype>(">=10")));
    TEST_EQUAL(map.size(), 6);
    TEST_EQUAL(to_str(map),
        "{<=0:alpha,"
        "3:delta,"
        "4:bravo,"
        "5:charlie,"
        "8:charlie,"
        "9:alpha}");
    TEST_EQUAL(map[0], "alpha");
    TEST_EQUAL(map[1], "nil");
    TEST_EQUAL(map[2], "nil");
    TEST_EQUAL(map[3], "delta");
    TEST_EQUAL(map[4], "bravo");
    TEST_EQUAL(map[5], "charlie");
    TEST_EQUAL(map[6], "nil");
    TEST_EQUAL(map[7], "nil");
    TEST_EQUAL(map[8], "charlie");
    TEST_EQUAL(map[9], "alpha");
    TEST_EQUAL(map[10], "nil");
    TEST_EQUAL(map[11], "nil");
    TEST_EQUAL(map[12], "nil");

}
