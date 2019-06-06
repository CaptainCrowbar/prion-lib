#include "rs-core/interval.hpp"
#include "rs-core/random.hpp"
#include "rs-core/unit-test.hpp"
#include <iostream>
#include <vector>

using namespace RS;
using namespace std::literals;

using Itype = Interval<Ustring>;
using Iset = IntervalSet<Ustring>;
using Imap = IntervalMap<Ustring, Ustring>;
using IB = IntervalBound;
using IC = IntervalCategory;
using IM = IntervalMatch;
using IO = IntervalOrder;

void test_source_interval_ordered_basic_properties() {

    Itype in;

    TEST_TYPE(Itype::value_type, Ustring);
    TEST_EQUAL(Itype::category, IC::ordered);
    TEST(in.is_empty());
    TEST(! in);
    TEST_EQUAL(to_str(in), "{}");
    TEST_EQUAL(in.match(""), IM::empty);
    TEST_EQUAL(in.match("hello"), IM::empty);
    TEST(! in(""));
    TEST(! in("hello"));

}

void test_source_interval_ordered_string_parsing() {

    Itype in;

    TRY(in = from_str<Itype>(""));       TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("{}"));     TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("*"));      TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), "*");
    TRY(in = from_str<Itype>("a"));      TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "a");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "a");
    TRY(in = from_str<Itype>("=a"));     TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "a");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "a");
    TRY(in = from_str<Itype>("<a"));     TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "a");  TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::open);     TEST_EQUAL(to_str(in), "<a");
    TRY(in = from_str<Itype>("<=a"));    TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "a");  TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "<=a");
    TRY(in = from_str<Itype>("a-"));     TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "a");  TEST_EQUAL(in.left(), IB::unbound);  TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "<=a");
    TRY(in = from_str<Itype>(">a"));     TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::open);     TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), ">a");
    TRY(in = from_str<Itype>(">=a"));    TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), ">=a");
    TRY(in = from_str<Itype>("a+"));     TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::unbound);  TEST_EQUAL(to_str(in), ">=a");
    TRY(in = from_str<Itype>("(a,b)"));  TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "b");  TEST_EQUAL(in.left(), IB::open);     TEST_EQUAL(in.right(), IB::open);     TEST_EQUAL(to_str(in), "(a,b)");
    TRY(in = from_str<Itype>("[a,b)"));  TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "b");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::open);     TEST_EQUAL(to_str(in), "[a,b)");
    TRY(in = from_str<Itype>("(a,b]"));  TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "b");  TEST_EQUAL(in.left(), IB::open);     TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "(a,b]");
    TRY(in = from_str<Itype>("[a,b]"));  TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "b");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[a,b]");
    TRY(in = from_str<Itype>("(b,a)"));  TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[b,a)"));  TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("(b,a]"));  TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[b,a]"));  TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("(a,a)"));  TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[a,a)"));  TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("(a,a]"));  TEST_EQUAL(in.min(), "");   TEST_EQUAL(in.max(), "");   TEST_EQUAL(in.left(), IB::empty);    TEST_EQUAL(in.right(), IB::empty);    TEST_EQUAL(to_str(in), "{}");
    TRY(in = from_str<Itype>("[a,a]"));  TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "a");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "a");
    TRY(in = from_str<Itype>("a..<b"));  TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "b");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::open);     TEST_EQUAL(to_str(in), "[a,b)");
    TRY(in = from_str<Itype>("a..b"));   TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "b");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[a,b]");
    TRY(in = from_str<Itype>("a...b"));  TEST_EQUAL(in.min(), "a");  TEST_EQUAL(in.max(), "b");  TEST_EQUAL(in.left(), IB::closed);   TEST_EQUAL(in.right(), IB::closed);   TEST_EQUAL(to_str(in), "[a,b]");

}

void test_source_interval_ordered_inverse() {

    Itype in;
    Iset set;
    Ustring str;

    TRY(in = {});                                TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{*}");
    TRY(in = Itype::all());                      TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{}");
    TRY(in = from_str<Itype>("hello"));          TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<hello,>hello}");
    TRY(in = from_str<Itype>("<hello"));         TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{>=hello}");
    TRY(in = from_str<Itype>("<=hello"));        TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{>hello}");
    TRY(in = from_str<Itype>(">hello"));         TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=hello}");
    TRY(in = from_str<Itype>(">=hello"));        TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<hello}");
    TRY(in = from_str<Itype>("(hello,world)"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=hello,>=world}");
    TRY(in = from_str<Itype>("(hello,world]"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<=hello,>world}");
    TRY(in = from_str<Itype>("[hello,world)"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<hello,>=world}");
    TRY(in = from_str<Itype>("[hello,world]"));  TRY(set = in.inverse());  TRY(str = to_str(set));  TEST_EQUAL(str, "{<hello,>world}");

}

void test_source_interval_ordered_binary_operations() {

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

        // line      lhs       rhs       cmp  incl    over    touch   envel     inter     union             diff              symm              --
        { __LINE__,  "{}",     "{}",     0,   false,  false,  false,  "{}",     "{}",     "{}",             "{}",             "{}",             },
        { __LINE__,  "*",      "{}",     1,   false,  false,  false,  "*",      "{}",     "{*}",            "{*}",            "{*}",            },
        { __LINE__,  "*",      "*",      0,   true,   true,   true,   "*",      "*",      "{*}",            "{}",             "{}",             },
        { __LINE__,  "b",      "{}",     1,   false,  false,  false,  "b",      "{}",     "{b}",            "{b}",            "{b}",            },
        { __LINE__,  "b",      "*",      1,   false,  true,   true,   "*",      "b",      "{*}",            "{}",             "{<b,>b}",        },
        { __LINE__,  "b",      "a",      1,   false,  false,  false,  "[a,b]",  "{}",     "{a,b}",          "{b}",            "{a,b}",          },
        { __LINE__,  "b",      "b",      0,   true,   true,   true,   "b",      "b",      "{b}",            "{}",             "{}",             },
        { __LINE__,  "b",      "c",      -1,  false,  false,  false,  "[b,c]",  "{}",     "{b,c}",          "{b}",            "{b,c}",          },
        { __LINE__,  "<b",     "{}",     1,   false,  false,  false,  "<b",     "{}",     "{<b}",           "{<b}",           "{<b}",           },
        { __LINE__,  "<b",     "*",      -1,  false,  true,   true,   "*",      "<b",     "{*}",            "{}",             "{>=b}",          },
        { __LINE__,  "<b",     "a",      -1,  true,   true,   true,   "<b",     "a",      "{<b}",           "{<a,(a,b)}",     "{<a,(a,b)}",     },
        { __LINE__,  "<b",     "b",      -1,  false,  false,  true,   "<=b",    "{}",     "{<=b}",          "{<b}",           "{<=b}",          },
        { __LINE__,  "<b",     "c",      -1,  false,  false,  false,  "<=c",    "{}",     "{<b,c}",         "{<b}",           "{<b,c}",         },
        { __LINE__,  "<b",     "<a",     1,   true,   true,   true,   "<b",     "<a",     "{<b}",           "{[a,b)}",        "{[a,b)}",        },
        { __LINE__,  "<b",     "<b",     0,   true,   true,   true,   "<b",     "<b",     "{<b}",           "{}",             "{}",             },
        { __LINE__,  "<b",     "<c",     -1,  false,  true,   true,   "<c",     "<b",     "{<c}",           "{}",             "{[b,c)}",        },
        { __LINE__,  "<=b",    "{}",     1,   false,  false,  false,  "<=b",    "{}",     "{<=b}",          "{<=b}",          "{<=b}",          },
        { __LINE__,  "<=b",    "*",      -1,  false,  true,   true,   "*",      "<=b",    "{*}",            "{}",             "{>b}",           },
        { __LINE__,  "<=b",    "a",      -1,  true,   true,   true,   "<=b",    "a",      "{<=b}",          "{<a,(a,b]}",     "{<a,(a,b]}",     },
        { __LINE__,  "<=b",    "b",      -1,  true,   true,   true,   "<=b",    "b",      "{<=b}",          "{<b}",           "{<b}",           },
        { __LINE__,  "<=b",    "c",      -1,  false,  false,  false,  "<=c",    "{}",     "{<=b,c}",        "{<=b}",          "{<=b,c}",        },
        { __LINE__,  "<=b",    "<a",     1,   true,   true,   true,   "<=b",    "<a",     "{<=b}",          "{[a,b]}",        "{[a,b]}",        },
        { __LINE__,  "<=b",    "<b",     1,   true,   true,   true,   "<=b",    "<b",     "{<=b}",          "{b}",            "{b}",            },
        { __LINE__,  "<=b",    "<c",     -1,  false,  true,   true,   "<c",     "<=b",    "{<c}",           "{}",             "{(b,c)}",        },
        { __LINE__,  "<=b",    "<=a",    1,   true,   true,   true,   "<=b",    "<=a",    "{<=b}",          "{(a,b]}",        "{(a,b]}",        },
        { __LINE__,  "<=b",    "<=b",    0,   true,   true,   true,   "<=b",    "<=b",    "{<=b}",          "{}",             "{}",             },
        { __LINE__,  "<=b",    "<=c",    -1,  false,  true,   true,   "<=c",    "<=b",    "{<=c}",          "{}",             "{(b,c]}",        },
        { __LINE__,  ">b",     "{}",     1,   false,  false,  false,  ">b",     "{}",     "{>b}",           "{>b}",           "{>b}",           },
        { __LINE__,  ">b",     "*",      1,   false,  true,   true,   "*",      ">b",     "{*}",            "{}",             "{<=b}",          },
        { __LINE__,  ">b",     "a",      1,   false,  false,  false,  ">=a",    "{}",     "{a,>b}",         "{>b}",           "{a,>b}",         },
        { __LINE__,  ">b",     "b",      1,   false,  false,  true,   ">=b",    "{}",     "{>=b}",          "{>b}",           "{>=b}",          },
        { __LINE__,  ">b",     "c",      -1,  true,   true,   true,   ">b",     "c",      "{>b}",           "{(b,c),>c}",     "{(b,c),>c}",     },
        { __LINE__,  ">b",     "<a",     1,   false,  false,  false,  "*",      "{}",     "{<a,>b}",        "{>b}",           "{<a,>b}",        },
        { __LINE__,  ">b",     "<b",     1,   false,  false,  false,  "*",      "{}",     "{<b,>b}",        "{>b}",           "{<b,>b}",        },
        { __LINE__,  ">b",     "<c",     1,   false,  true,   true,   "*",      "(b,c)",  "{*}",            "{>=c}",          "{<=b,>=c}",      },
        { __LINE__,  ">b",     "<=a",    1,   false,  false,  false,  "*",      "{}",     "{<=a,>b}",       "{>b}",           "{<=a,>b}",       },
        { __LINE__,  ">b",     "<=b",    1,   false,  false,  true,   "*",      "{}",     "{*}",            "{>b}",           "{*}",            },
        { __LINE__,  ">b",     "<=c",    1,   false,  true,   true,   "*",      "(b,c]",  "{*}",            "{>c}",           "{<=b,>c}",       },
        { __LINE__,  ">b",     ">a",     1,   false,  true,   true,   ">a",     ">b",     "{>a}",           "{}",             "{(a,b]}",        },
        { __LINE__,  ">b",     ">b",     0,   true,   true,   true,   ">b",     ">b",     "{>b}",           "{}",             "{}",             },
        { __LINE__,  ">b",     ">c",     -1,  true,   true,   true,   ">b",     ">c",     "{>b}",           "{(b,c]}",        "{(b,c]}",        },
        { __LINE__,  ">=b",    "{}",     1,   false,  false,  false,  ">=b",    "{}",     "{>=b}",          "{>=b}",          "{>=b}",          },
        { __LINE__,  ">=b",    "*",      1,   false,  true,   true,   "*",      ">=b",    "{*}",            "{}",             "{<b}",           },
        { __LINE__,  ">=b",    "a",      1,   false,  false,  false,  ">=a",    "{}",     "{a,>=b}",        "{>=b}",          "{a,>=b}",        },
        { __LINE__,  ">=b",    "b",      1,   true,   true,   true,   ">=b",    "b",      "{>=b}",          "{>b}",           "{>b}",           },
        { __LINE__,  ">=b",    "c",      -1,  true,   true,   true,   ">=b",    "c",      "{>=b}",          "{[b,c),>c}",     "{[b,c),>c}",     },
        { __LINE__,  ">=b",    "<a",     1,   false,  false,  false,  "*",      "{}",     "{<a,>=b}",       "{>=b}",          "{<a,>=b}",       },
        { __LINE__,  ">=b",    "<b",     1,   false,  false,  true,   "*",      "{}",     "{*}",            "{>=b}",          "{*}",            },
        { __LINE__,  ">=b",    "<c",     1,   false,  true,   true,   "*",      "[b,c)",  "{*}",            "{>=c}",          "{<b,>=c}",       },
        { __LINE__,  ">=b",    "<=a",    1,   false,  false,  false,  "*",      "{}",     "{<=a,>=b}",      "{>=b}",          "{<=a,>=b}",      },
        { __LINE__,  ">=b",    "<=b",    1,   false,  true,   true,   "*",      "b",      "{*}",            "{>b}",           "{<b,>b}",        },
        { __LINE__,  ">=b",    "<=c",    1,   false,  true,   true,   "*",      "[b,c]",  "{*}",            "{>c}",           "{<b,>c}",        },
        { __LINE__,  ">=b",    ">a",     1,   false,  true,   true,   ">a",     ">=b",    "{>a}",           "{}",             "{(a,b)}",        },
        { __LINE__,  ">=b",    ">b",     -1,  true,   true,   true,   ">=b",    ">b",     "{>=b}",          "{b}",            "{b}",            },
        { __LINE__,  ">=b",    ">c",     -1,  true,   true,   true,   ">=b",    ">c",     "{>=b}",          "{[b,c]}",        "{[b,c]}",        },
        { __LINE__,  ">=b",    ">=a",    1,   false,  true,   true,   ">=a",    ">=b",    "{>=a}",          "{}",             "{[a,b)}",        },
        { __LINE__,  ">=b",    ">=b",    0,   true,   true,   true,   ">=b",    ">=b",    "{>=b}",          "{}",             "{}",             },
        { __LINE__,  ">=b",    ">=c",    -1,  true,   true,   true,   ">=b",    ">=c",    "{>=b}",          "{[b,c)}",        "{[b,c)}",        },
        { __LINE__,  "(b,d)",  "{}",     1,   false,  false,  false,  "(b,d)",  "{}",     "{(b,d)}",        "{(b,d)}",        "{(b,d)}",        },
        { __LINE__,  "(b,d)",  "*",      1,   false,  true,   true,   "*",      "(b,d)",  "{*}",            "{}",             "{<=b,>=d}",      },
        { __LINE__,  "(b,d)",  "a",      1,   false,  false,  false,  "[a,d)",  "{}",     "{a,(b,d)}",      "{(b,d)}",        "{a,(b,d)}",      },
        { __LINE__,  "(b,d)",  "b",      1,   false,  false,  true,   "[b,d)",  "{}",     "{[b,d)}",        "{(b,d)}",        "{[b,d)}",        },
        { __LINE__,  "(b,d)",  "c",      -1,  true,   true,   true,   "(b,d)",  "c",      "{(b,d)}",        "{(b,c),(c,d)}",  "{(b,c),(c,d)}",  },
        { __LINE__,  "(b,d)",  "d",      -1,  false,  false,  true,   "(b,d]",  "{}",     "{(b,d]}",        "{(b,d)}",        "{(b,d]}",        },
        { __LINE__,  "(b,d)",  "e",      -1,  false,  false,  false,  "(b,e]",  "{}",     "{(b,d),e}",      "{(b,d)}",        "{(b,d),e}",      },
        { __LINE__,  "(b,d)",  "<a",     1,   false,  false,  false,  "<d",     "{}",     "{<a,(b,d)}",     "{(b,d)}",        "{<a,(b,d)}",     },
        { __LINE__,  "(b,d)",  "<b",     1,   false,  false,  false,  "<d",     "{}",     "{<b,(b,d)}",     "{(b,d)}",        "{<b,(b,d)}",     },
        { __LINE__,  "(b,d)",  "<c",     1,   false,  true,   true,   "<d",     "(b,c)",  "{<d}",           "{[c,d)}",        "{<=b,[c,d)}",    },
        { __LINE__,  "(b,d)",  "<d",     1,   false,  true,   true,   "<d",     "(b,d)",  "{<d}",           "{}",             "{<=b}",          },
        { __LINE__,  "(b,d)",  "<e",     1,   false,  true,   true,   "<e",     "(b,d)",  "{<e}",           "{}",             "{<=b,[d,e)}",    },
        { __LINE__,  "(b,d)",  "<=a",    1,   false,  false,  false,  "<d",     "{}",     "{<=a,(b,d)}",    "{(b,d)}",        "{<=a,(b,d)}",    },
        { __LINE__,  "(b,d)",  "<=b",    1,   false,  false,  true,   "<d",     "{}",     "{<d}",           "{(b,d)}",        "{<d}",           },
        { __LINE__,  "(b,d)",  "<=c",    1,   false,  true,   true,   "<d",     "(b,c]",  "{<d}",           "{(c,d)}",        "{<=b,(c,d)}",    },
        { __LINE__,  "(b,d)",  "<=d",    1,   false,  true,   true,   "<=d",    "(b,d)",  "{<=d}",          "{}",             "{<=b,d}",        },
        { __LINE__,  "(b,d)",  "<=e",    1,   false,  true,   true,   "<=e",    "(b,d)",  "{<=e}",          "{}",             "{<=b,[d,e]}",    },
        { __LINE__,  "(b,d)",  ">a",     1,   false,  true,   true,   ">a",     "(b,d)",  "{>a}",           "{}",             "{(a,b],>=d}",    },
        { __LINE__,  "(b,d)",  ">b",     -1,  false,  true,   true,   ">b",     "(b,d)",  "{>b}",           "{}",             "{>=d}",          },
        { __LINE__,  "(b,d)",  ">c",     -1,  false,  true,   true,   ">b",     "(c,d)",  "{>b}",           "{(b,c]}",        "{(b,c],>=d}",    },
        { __LINE__,  "(b,d)",  ">d",     -1,  false,  false,  false,  ">b",     "{}",     "{(b,d),>d}",     "{(b,d)}",        "{(b,d),>d}",     },
        { __LINE__,  "(b,d)",  ">e",     -1,  false,  false,  false,  ">b",     "{}",     "{(b,d),>e}",     "{(b,d)}",        "{(b,d),>e}",     },
        { __LINE__,  "(b,d)",  ">=a",    1,   false,  true,   true,   ">=a",    "(b,d)",  "{>=a}",          "{}",             "{[a,b],>=d}",    },
        { __LINE__,  "(b,d)",  ">=b",    1,   false,  true,   true,   ">=b",    "(b,d)",  "{>=b}",          "{}",             "{b,>=d}",        },
        { __LINE__,  "(b,d)",  ">=c",    -1,  false,  true,   true,   ">b",     "[c,d)",  "{>b}",           "{(b,c)}",        "{(b,c),>=d}",    },
        { __LINE__,  "(b,d)",  ">=d",    -1,  false,  false,  true,   ">b",     "{}",     "{>b}",           "{(b,d)}",        "{>b}",           },
        { __LINE__,  "(b,d)",  ">=e",    -1,  false,  false,  false,  ">b",     "{}",     "{(b,d),>=e}",    "{(b,d)}",        "{(b,d),>=e}",    },
        { __LINE__,  "(b,d)",  "(a,b)",  1,   false,  false,  false,  "(a,d)",  "{}",     "{(a,b),(b,d)}",  "{(b,d)}",        "{(a,b),(b,d)}",  },
        { __LINE__,  "(b,d)",  "(a,c)",  1,   false,  true,   true,   "(a,d)",  "(b,c)",  "{(a,d)}",        "{[c,d)}",        "{(a,b],[c,d)}",  },
        { __LINE__,  "(b,d)",  "(a,d)",  1,   false,  true,   true,   "(a,d)",  "(b,d)",  "{(a,d)}",        "{}",             "{(a,b]}",        },
        { __LINE__,  "(b,d)",  "(a,e)",  1,   false,  true,   true,   "(a,e)",  "(b,d)",  "{(a,e)}",        "{}",             "{(a,b],[d,e)}",  },
        { __LINE__,  "(b,d)",  "(b,c)",  1,   true,   true,   true,   "(b,d)",  "(b,c)",  "{(b,d)}",        "{[c,d)}",        "{[c,d)}",        },
        { __LINE__,  "(b,d)",  "(b,d)",  0,   true,   true,   true,   "(b,d)",  "(b,d)",  "{(b,d)}",        "{}",             "{}",             },
        { __LINE__,  "(b,d)",  "(b,e)",  -1,  false,  true,   true,   "(b,e)",  "(b,d)",  "{(b,e)}",        "{}",             "{[d,e)}",        },
        { __LINE__,  "(b,d)",  "(c,d)",  -1,  true,   true,   true,   "(b,d)",  "(c,d)",  "{(b,d)}",        "{(b,c]}",        "{(b,c]}",        },
        { __LINE__,  "(b,d)",  "(c,e)",  -1,  false,  true,   true,   "(b,e)",  "(c,d)",  "{(b,e)}",        "{(b,c]}",        "{(b,c],[d,e)}",  },
        { __LINE__,  "(b,d)",  "(d,e)",  -1,  false,  false,  false,  "(b,e)",  "{}",     "{(b,d),(d,e)}",  "{(b,d)}",        "{(b,d),(d,e)}",  },
        { __LINE__,  "[b,d)",  "{}",     1,   false,  false,  false,  "[b,d)",  "{}",     "{[b,d)}",        "{[b,d)}",        "{[b,d)}",        },
        { __LINE__,  "[b,d)",  "*",      1,   false,  true,   true,   "*",      "[b,d)",  "{*}",            "{}",             "{<b,>=d}",       },
        { __LINE__,  "[b,d)",  "a",      1,   false,  false,  false,  "[a,d)",  "{}",     "{a,[b,d)}",      "{[b,d)}",        "{a,[b,d)}",      },
        { __LINE__,  "[b,d)",  "b",      1,   true,   true,   true,   "[b,d)",  "b",      "{[b,d)}",        "{(b,d)}",        "{(b,d)}",        },
        { __LINE__,  "[b,d)",  "c",      -1,  true,   true,   true,   "[b,d)",  "c",      "{[b,d)}",        "{[b,c),(c,d)}",  "{[b,c),(c,d)}",  },
        { __LINE__,  "[b,d)",  "d",      -1,  false,  false,  true,   "[b,d]",  "{}",     "{[b,d]}",        "{[b,d)}",        "{[b,d]}",        },
        { __LINE__,  "[b,d)",  "e",      -1,  false,  false,  false,  "[b,e]",  "{}",     "{[b,d),e}",      "{[b,d)}",        "{[b,d),e}",      },
        { __LINE__,  "[b,d)",  "<a",     1,   false,  false,  false,  "<d",     "{}",     "{<a,[b,d)}",     "{[b,d)}",        "{<a,[b,d)}",     },
        { __LINE__,  "[b,d)",  "<b",     1,   false,  false,  true,   "<d",     "{}",     "{<d}",           "{[b,d)}",        "{<d}",           },
        { __LINE__,  "[b,d)",  "<c",     1,   false,  true,   true,   "<d",     "[b,c)",  "{<d}",           "{[c,d)}",        "{<b,[c,d)}",     },
        { __LINE__,  "[b,d)",  "<d",     1,   false,  true,   true,   "<d",     "[b,d)",  "{<d}",           "{}",             "{<b}",           },
        { __LINE__,  "[b,d)",  "<e",     1,   false,  true,   true,   "<e",     "[b,d)",  "{<e}",           "{}",             "{<b,[d,e)}",     },
        { __LINE__,  "[b,d)",  "<=a",    1,   false,  false,  false,  "<d",     "{}",     "{<=a,[b,d)}",    "{[b,d)}",        "{<=a,[b,d)}",    },
        { __LINE__,  "[b,d)",  "<=b",    1,   false,  true,   true,   "<d",     "b",      "{<d}",           "{(b,d)}",        "{<b,(b,d)}",     },
        { __LINE__,  "[b,d)",  "<=c",    1,   false,  true,   true,   "<d",     "[b,c]",  "{<d}",           "{(c,d)}",        "{<b,(c,d)}",     },
        { __LINE__,  "[b,d)",  "<=d",    1,   false,  true,   true,   "<=d",    "[b,d)",  "{<=d}",          "{}",             "{<b,d}",         },
        { __LINE__,  "[b,d)",  "<=e",    1,   false,  true,   true,   "<=e",    "[b,d)",  "{<=e}",          "{}",             "{<b,[d,e]}",     },
        { __LINE__,  "[b,d)",  ">a",     1,   false,  true,   true,   ">a",     "[b,d)",  "{>a}",           "{}",             "{(a,b),>=d}",    },
        { __LINE__,  "[b,d)",  ">b",     -1,  false,  true,   true,   ">=b",    "(b,d)",  "{>=b}",          "{b}",            "{b,>=d}",        },
        { __LINE__,  "[b,d)",  ">c",     -1,  false,  true,   true,   ">=b",    "(c,d)",  "{>=b}",          "{[b,c]}",        "{[b,c],>=d}",    },
        { __LINE__,  "[b,d)",  ">d",     -1,  false,  false,  false,  ">=b",    "{}",     "{[b,d),>d}",     "{[b,d)}",        "{[b,d),>d}",     },
        { __LINE__,  "[b,d)",  ">e",     -1,  false,  false,  false,  ">=b",    "{}",     "{[b,d),>e}",     "{[b,d)}",        "{[b,d),>e}",     },
        { __LINE__,  "[b,d)",  ">=a",    1,   false,  true,   true,   ">=a",    "[b,d)",  "{>=a}",          "{}",             "{[a,b),>=d}",    },
        { __LINE__,  "[b,d)",  ">=b",    -1,  false,  true,   true,   ">=b",    "[b,d)",  "{>=b}",          "{}",             "{>=d}",          },
        { __LINE__,  "[b,d)",  ">=c",    -1,  false,  true,   true,   ">=b",    "[c,d)",  "{>=b}",          "{[b,c)}",        "{[b,c),>=d}",    },
        { __LINE__,  "[b,d)",  ">=d",    -1,  false,  false,  true,   ">=b",    "{}",     "{>=b}",          "{[b,d)}",        "{>=b}",          },
        { __LINE__,  "[b,d)",  ">=e",    -1,  false,  false,  false,  ">=b",    "{}",     "{[b,d),>=e}",    "{[b,d)}",        "{[b,d),>=e}",    },
        { __LINE__,  "[b,d)",  "(a,b)",  1,   false,  false,  true,   "(a,d)",  "{}",     "{(a,d)}",        "{[b,d)}",        "{(a,d)}",        },
        { __LINE__,  "[b,d)",  "(a,c)",  1,   false,  true,   true,   "(a,d)",  "[b,c)",  "{(a,d)}",        "{[c,d)}",        "{(a,b),[c,d)}",  },
        { __LINE__,  "[b,d)",  "(a,d)",  1,   false,  true,   true,   "(a,d)",  "[b,d)",  "{(a,d)}",        "{}",             "{(a,b)}",        },
        { __LINE__,  "[b,d)",  "(a,e)",  1,   false,  true,   true,   "(a,e)",  "[b,d)",  "{(a,e)}",        "{}",             "{(a,b),[d,e)}",  },
        { __LINE__,  "[b,d)",  "(b,c)",  -1,  true,   true,   true,   "[b,d)",  "(b,c)",  "{[b,d)}",        "{b,[c,d)}",      "{b,[c,d)}",      },
        { __LINE__,  "[b,d)",  "(b,d)",  -1,  true,   true,   true,   "[b,d)",  "(b,d)",  "{[b,d)}",        "{b}",            "{b}",            },
        { __LINE__,  "[b,d)",  "(b,e)",  -1,  false,  true,   true,   "[b,e)",  "(b,d)",  "{[b,e)}",        "{b}",            "{b,[d,e)}",      },
        { __LINE__,  "[b,d)",  "(c,d)",  -1,  true,   true,   true,   "[b,d)",  "(c,d)",  "{[b,d)}",        "{[b,c]}",        "{[b,c]}",        },
        { __LINE__,  "[b,d)",  "(c,e)",  -1,  false,  true,   true,   "[b,e)",  "(c,d)",  "{[b,e)}",        "{[b,c]}",        "{[b,c],[d,e)}",  },
        { __LINE__,  "[b,d)",  "(d,e)",  -1,  false,  false,  false,  "[b,e)",  "{}",     "{[b,d),(d,e)}",  "{[b,d)}",        "{[b,d),(d,e)}",  },
        { __LINE__,  "[b,d)",  "[a,b)",  1,   false,  false,  true,   "[a,d)",  "{}",     "{[a,d)}",        "{[b,d)}",        "{[a,d)}",        },
        { __LINE__,  "[b,d)",  "[a,c)",  1,   false,  true,   true,   "[a,d)",  "[b,c)",  "{[a,d)}",        "{[c,d)}",        "{[a,b),[c,d)}",  },
        { __LINE__,  "[b,d)",  "[a,d)",  1,   false,  true,   true,   "[a,d)",  "[b,d)",  "{[a,d)}",        "{}",             "{[a,b)}",        },
        { __LINE__,  "[b,d)",  "[a,e)",  1,   false,  true,   true,   "[a,e)",  "[b,d)",  "{[a,e)}",        "{}",             "{[a,b),[d,e)}",  },
        { __LINE__,  "[b,d)",  "[b,c)",  1,   true,   true,   true,   "[b,d)",  "[b,c)",  "{[b,d)}",        "{[c,d)}",        "{[c,d)}",        },
        { __LINE__,  "[b,d)",  "[b,d)",  0,   true,   true,   true,   "[b,d)",  "[b,d)",  "{[b,d)}",        "{}",             "{}",             },
        { __LINE__,  "[b,d)",  "[b,e)",  -1,  false,  true,   true,   "[b,e)",  "[b,d)",  "{[b,e)}",        "{}",             "{[d,e)}",        },
        { __LINE__,  "[b,d)",  "[c,d)",  -1,  true,   true,   true,   "[b,d)",  "[c,d)",  "{[b,d)}",        "{[b,c)}",        "{[b,c)}",        },
        { __LINE__,  "[b,d)",  "[c,e)",  -1,  false,  true,   true,   "[b,e)",  "[c,d)",  "{[b,e)}",        "{[b,c)}",        "{[b,c),[d,e)}",  },
        { __LINE__,  "[b,d)",  "[d,e)",  -1,  false,  false,  true,   "[b,e)",  "{}",     "{[b,e)}",        "{[b,d)}",        "{[b,e)}",        },
        { __LINE__,  "(b,d]",  "{}",     1,   false,  false,  false,  "(b,d]",  "{}",     "{(b,d]}",        "{(b,d]}",        "{(b,d]}",        },
        { __LINE__,  "(b,d]",  "*",      1,   false,  true,   true,   "*",      "(b,d]",  "{*}",            "{}",             "{<=b,>d}",       },
        { __LINE__,  "(b,d]",  "a",      1,   false,  false,  false,  "[a,d]",  "{}",     "{a,(b,d]}",      "{(b,d]}",        "{a,(b,d]}",      },
        { __LINE__,  "(b,d]",  "b",      1,   false,  false,  true,   "[b,d]",  "{}",     "{[b,d]}",        "{(b,d]}",        "{[b,d]}",        },
        { __LINE__,  "(b,d]",  "c",      -1,  true,   true,   true,   "(b,d]",  "c",      "{(b,d]}",        "{(b,c),(c,d]}",  "{(b,c),(c,d]}",  },
        { __LINE__,  "(b,d]",  "d",      -1,  true,   true,   true,   "(b,d]",  "d",      "{(b,d]}",        "{(b,d)}",        "{(b,d)}",        },
        { __LINE__,  "(b,d]",  "e",      -1,  false,  false,  false,  "(b,e]",  "{}",     "{(b,d],e}",      "{(b,d]}",        "{(b,d],e}",      },
        { __LINE__,  "(b,d]",  "<a",     1,   false,  false,  false,  "<=d",    "{}",     "{<a,(b,d]}",     "{(b,d]}",        "{<a,(b,d]}",     },
        { __LINE__,  "(b,d]",  "<b",     1,   false,  false,  false,  "<=d",    "{}",     "{<b,(b,d]}",     "{(b,d]}",        "{<b,(b,d]}",     },
        { __LINE__,  "(b,d]",  "<c",     1,   false,  true,   true,   "<=d",    "(b,c)",  "{<=d}",          "{[c,d]}",        "{<=b,[c,d]}",    },
        { __LINE__,  "(b,d]",  "<d",     1,   false,  true,   true,   "<=d",    "(b,d)",  "{<=d}",          "{d}",            "{<=b,d}",        },
        { __LINE__,  "(b,d]",  "<e",     1,   false,  true,   true,   "<e",     "(b,d]",  "{<e}",           "{}",             "{<=b,(d,e)}",    },
        { __LINE__,  "(b,d]",  "<=a",    1,   false,  false,  false,  "<=d",    "{}",     "{<=a,(b,d]}",    "{(b,d]}",        "{<=a,(b,d]}",    },
        { __LINE__,  "(b,d]",  "<=b",    1,   false,  false,  true,   "<=d",    "{}",     "{<=d}",          "{(b,d]}",        "{<=d}",          },
        { __LINE__,  "(b,d]",  "<=c",    1,   false,  true,   true,   "<=d",    "(b,c]",  "{<=d}",          "{(c,d]}",        "{<=b,(c,d]}",    },
        { __LINE__,  "(b,d]",  "<=d",    1,   false,  true,   true,   "<=d",    "(b,d]",  "{<=d}",          "{}",             "{<=b}",          },
        { __LINE__,  "(b,d]",  "<=e",    1,   false,  true,   true,   "<=e",    "(b,d]",  "{<=e}",          "{}",             "{<=b,(d,e]}",    },
        { __LINE__,  "(b,d]",  ">a",     1,   false,  true,   true,   ">a",     "(b,d]",  "{>a}",           "{}",             "{(a,b],>d}",     },
        { __LINE__,  "(b,d]",  ">b",     -1,  false,  true,   true,   ">b",     "(b,d]",  "{>b}",           "{}",             "{>d}",           },
        { __LINE__,  "(b,d]",  ">c",     -1,  false,  true,   true,   ">b",     "(c,d]",  "{>b}",           "{(b,c]}",        "{(b,c],>d}",     },
        { __LINE__,  "(b,d]",  ">d",     -1,  false,  false,  true,   ">b",     "{}",     "{>b}",           "{(b,d]}",        "{>b}",           },
        { __LINE__,  "(b,d]",  ">e",     -1,  false,  false,  false,  ">b",     "{}",     "{(b,d],>e}",     "{(b,d]}",        "{(b,d],>e}",     },
        { __LINE__,  "(b,d]",  ">=a",    1,   false,  true,   true,   ">=a",    "(b,d]",  "{>=a}",          "{}",             "{[a,b],>d}",     },
        { __LINE__,  "(b,d]",  ">=b",    1,   false,  true,   true,   ">=b",    "(b,d]",  "{>=b}",          "{}",             "{b,>d}",         },
        { __LINE__,  "(b,d]",  ">=c",    -1,  false,  true,   true,   ">b",     "[c,d]",  "{>b}",           "{(b,c)}",        "{(b,c),>d}",     },
        { __LINE__,  "(b,d]",  ">=d",    -1,  false,  true,   true,   ">b",     "d",      "{>b}",           "{(b,d)}",        "{(b,d),>d}",     },
        { __LINE__,  "(b,d]",  ">=e",    -1,  false,  false,  false,  ">b",     "{}",     "{(b,d],>=e}",    "{(b,d]}",        "{(b,d],>=e}",    },
        { __LINE__,  "(b,d]",  "(a,b)",  1,   false,  false,  false,  "(a,d]",  "{}",     "{(a,b),(b,d]}",  "{(b,d]}",        "{(a,b),(b,d]}",  },
        { __LINE__,  "(b,d]",  "(a,c)",  1,   false,  true,   true,   "(a,d]",  "(b,c)",  "{(a,d]}",        "{[c,d]}",        "{(a,b],[c,d]}",  },
        { __LINE__,  "(b,d]",  "(a,d)",  1,   false,  true,   true,   "(a,d]",  "(b,d)",  "{(a,d]}",        "{d}",            "{(a,b],d}",      },
        { __LINE__,  "(b,d]",  "(a,e)",  1,   false,  true,   true,   "(a,e)",  "(b,d]",  "{(a,e)}",        "{}",             "{(a,b],(d,e)}",  },
        { __LINE__,  "(b,d]",  "(b,c)",  1,   true,   true,   true,   "(b,d]",  "(b,c)",  "{(b,d]}",        "{[c,d]}",        "{[c,d]}",        },
        { __LINE__,  "(b,d]",  "(b,d)",  1,   true,   true,   true,   "(b,d]",  "(b,d)",  "{(b,d]}",        "{d}",            "{d}",            },
        { __LINE__,  "(b,d]",  "(b,e)",  -1,  false,  true,   true,   "(b,e)",  "(b,d]",  "{(b,e)}",        "{}",             "{(d,e)}",        },
        { __LINE__,  "(b,d]",  "(c,d)",  -1,  true,   true,   true,   "(b,d]",  "(c,d)",  "{(b,d]}",        "{(b,c],d}",      "{(b,c],d}",      },
        { __LINE__,  "(b,d]",  "(c,e)",  -1,  false,  true,   true,   "(b,e)",  "(c,d]",  "{(b,e)}",        "{(b,c]}",        "{(b,c],(d,e)}",  },
        { __LINE__,  "(b,d]",  "(d,e)",  -1,  false,  false,  true,   "(b,e)",  "{}",     "{(b,e)}",        "{(b,d]}",        "{(b,e)}",        },
        { __LINE__,  "(b,d]",  "[a,b)",  1,   false,  false,  false,  "[a,d]",  "{}",     "{[a,b),(b,d]}",  "{(b,d]}",        "{[a,b),(b,d]}",  },
        { __LINE__,  "(b,d]",  "[a,c)",  1,   false,  true,   true,   "[a,d]",  "(b,c)",  "{[a,d]}",        "{[c,d]}",        "{[a,b],[c,d]}",  },
        { __LINE__,  "(b,d]",  "[a,d)",  1,   false,  true,   true,   "[a,d]",  "(b,d)",  "{[a,d]}",        "{d}",            "{[a,b],d}",      },
        { __LINE__,  "(b,d]",  "[a,e)",  1,   false,  true,   true,   "[a,e)",  "(b,d]",  "{[a,e)}",        "{}",             "{[a,b],(d,e)}",  },
        { __LINE__,  "(b,d]",  "[b,c)",  1,   false,  true,   true,   "[b,d]",  "(b,c)",  "{[b,d]}",        "{[c,d]}",        "{b,[c,d]}",      },
        { __LINE__,  "(b,d]",  "[b,d)",  1,   false,  true,   true,   "[b,d]",  "(b,d)",  "{[b,d]}",        "{d}",            "{b,d}",          },
        { __LINE__,  "(b,d]",  "[b,e)",  1,   false,  true,   true,   "[b,e)",  "(b,d]",  "{[b,e)}",        "{}",             "{b,(d,e)}",      },
        { __LINE__,  "(b,d]",  "[c,d)",  -1,  true,   true,   true,   "(b,d]",  "[c,d)",  "{(b,d]}",        "{(b,c),d}",      "{(b,c),d}",      },
        { __LINE__,  "(b,d]",  "[c,e)",  -1,  false,  true,   true,   "(b,e)",  "[c,d]",  "{(b,e)}",        "{(b,c)}",        "{(b,c),(d,e)}",  },
        { __LINE__,  "(b,d]",  "[d,e)",  -1,  false,  true,   true,   "(b,e)",  "d",      "{(b,e)}",        "{(b,d)}",        "{(b,d),(d,e)}",  },
        { __LINE__,  "(b,d]",  "(a,b]",  1,   false,  false,  true,   "(a,d]",  "{}",     "{(a,d]}",        "{(b,d]}",        "{(a,d]}",        },
        { __LINE__,  "(b,d]",  "(a,c]",  1,   false,  true,   true,   "(a,d]",  "(b,c]",  "{(a,d]}",        "{(c,d]}",        "{(a,b],(c,d]}",  },
        { __LINE__,  "(b,d]",  "(a,d]",  1,   false,  true,   true,   "(a,d]",  "(b,d]",  "{(a,d]}",        "{}",             "{(a,b]}",        },
        { __LINE__,  "(b,d]",  "(a,e]",  1,   false,  true,   true,   "(a,e]",  "(b,d]",  "{(a,e]}",        "{}",             "{(a,b],(d,e]}",  },
        { __LINE__,  "(b,d]",  "(b,c]",  1,   true,   true,   true,   "(b,d]",  "(b,c]",  "{(b,d]}",        "{(c,d]}",        "{(c,d]}",        },
        { __LINE__,  "(b,d]",  "(b,d]",  0,   true,   true,   true,   "(b,d]",  "(b,d]",  "{(b,d]}",        "{}",             "{}",             },
        { __LINE__,  "(b,d]",  "(b,e]",  -1,  false,  true,   true,   "(b,e]",  "(b,d]",  "{(b,e]}",        "{}",             "{(d,e]}",        },
        { __LINE__,  "(b,d]",  "(c,d]",  -1,  true,   true,   true,   "(b,d]",  "(c,d]",  "{(b,d]}",        "{(b,c]}",        "{(b,c]}",        },
        { __LINE__,  "(b,d]",  "(c,e]",  -1,  false,  true,   true,   "(b,e]",  "(c,d]",  "{(b,e]}",        "{(b,c]}",        "{(b,c],(d,e]}",  },
        { __LINE__,  "(b,d]",  "(d,e]",  -1,  false,  false,  true,   "(b,e]",  "{}",     "{(b,e]}",        "{(b,d]}",        "{(b,e]}",        },
        { __LINE__,  "[b,d]",  "{}",     1,   false,  false,  false,  "[b,d]",  "{}",     "{[b,d]}",        "{[b,d]}",        "{[b,d]}",        },
        { __LINE__,  "[b,d]",  "*",      1,   false,  true,   true,   "*",      "[b,d]",  "{*}",            "{}",             "{<b,>d}",        },
        { __LINE__,  "[b,d]",  "a",      1,   false,  false,  false,  "[a,d]",  "{}",     "{a,[b,d]}",      "{[b,d]}",        "{a,[b,d]}",      },
        { __LINE__,  "[b,d]",  "b",      1,   true,   true,   true,   "[b,d]",  "b",      "{[b,d]}",        "{(b,d]}",        "{(b,d]}",        },
        { __LINE__,  "[b,d]",  "c",      -1,  true,   true,   true,   "[b,d]",  "c",      "{[b,d]}",        "{[b,c),(c,d]}",  "{[b,c),(c,d]}",  },
        { __LINE__,  "[b,d]",  "d",      -1,  true,   true,   true,   "[b,d]",  "d",      "{[b,d]}",        "{[b,d)}",        "{[b,d)}",        },
        { __LINE__,  "[b,d]",  "e",      -1,  false,  false,  false,  "[b,e]",  "{}",     "{[b,d],e}",      "{[b,d]}",        "{[b,d],e}",      },
        { __LINE__,  "[b,d]",  "<a",     1,   false,  false,  false,  "<=d",    "{}",     "{<a,[b,d]}",     "{[b,d]}",        "{<a,[b,d]}",     },
        { __LINE__,  "[b,d]",  "<b",     1,   false,  false,  true,   "<=d",    "{}",     "{<=d}",          "{[b,d]}",        "{<=d}",          },
        { __LINE__,  "[b,d]",  "<c",     1,   false,  true,   true,   "<=d",    "[b,c)",  "{<=d}",          "{[c,d]}",        "{<b,[c,d]}",     },
        { __LINE__,  "[b,d]",  "<d",     1,   false,  true,   true,   "<=d",    "[b,d)",  "{<=d}",          "{d}",            "{<b,d}",         },
        { __LINE__,  "[b,d]",  "<e",     1,   false,  true,   true,   "<e",     "[b,d]",  "{<e}",           "{}",             "{<b,(d,e)}",     },
        { __LINE__,  "[b,d]",  "<=a",    1,   false,  false,  false,  "<=d",    "{}",     "{<=a,[b,d]}",    "{[b,d]}",        "{<=a,[b,d]}",    },
        { __LINE__,  "[b,d]",  "<=b",    1,   false,  true,   true,   "<=d",    "b",      "{<=d}",          "{(b,d]}",        "{<b,(b,d]}",     },
        { __LINE__,  "[b,d]",  "<=c",    1,   false,  true,   true,   "<=d",    "[b,c]",  "{<=d}",          "{(c,d]}",        "{<b,(c,d]}",     },
        { __LINE__,  "[b,d]",  "<=d",    1,   false,  true,   true,   "<=d",    "[b,d]",  "{<=d}",          "{}",             "{<b}",           },
        { __LINE__,  "[b,d]",  "<=e",    1,   false,  true,   true,   "<=e",    "[b,d]",  "{<=e}",          "{}",             "{<b,(d,e]}",     },
        { __LINE__,  "[b,d]",  ">a",     1,   false,  true,   true,   ">a",     "[b,d]",  "{>a}",           "{}",             "{(a,b),>d}",     },
        { __LINE__,  "[b,d]",  ">b",     -1,  false,  true,   true,   ">=b",    "(b,d]",  "{>=b}",          "{b}",            "{b,>d}",         },
        { __LINE__,  "[b,d]",  ">c",     -1,  false,  true,   true,   ">=b",    "(c,d]",  "{>=b}",          "{[b,c]}",        "{[b,c],>d}",     },
        { __LINE__,  "[b,d]",  ">d",     -1,  false,  false,  true,   ">=b",    "{}",     "{>=b}",          "{[b,d]}",        "{>=b}",          },
        { __LINE__,  "[b,d]",  ">e",     -1,  false,  false,  false,  ">=b",    "{}",     "{[b,d],>e}",     "{[b,d]}",        "{[b,d],>e}",     },
        { __LINE__,  "[b,d]",  ">=a",    1,   false,  true,   true,   ">=a",    "[b,d]",  "{>=a}",          "{}",             "{[a,b),>d}",     },
        { __LINE__,  "[b,d]",  ">=b",    -1,  false,  true,   true,   ">=b",    "[b,d]",  "{>=b}",          "{}",             "{>d}",           },
        { __LINE__,  "[b,d]",  ">=c",    -1,  false,  true,   true,   ">=b",    "[c,d]",  "{>=b}",          "{[b,c)}",        "{[b,c),>d}",     },
        { __LINE__,  "[b,d]",  ">=d",    -1,  false,  true,   true,   ">=b",    "d",      "{>=b}",          "{[b,d)}",        "{[b,d),>d}",     },
        { __LINE__,  "[b,d]",  ">=e",    -1,  false,  false,  false,  ">=b",    "{}",     "{[b,d],>=e}",    "{[b,d]}",        "{[b,d],>=e}",    },
        { __LINE__,  "[b,d]",  "(a,b)",  1,   false,  false,  true,   "(a,d]",  "{}",     "{(a,d]}",        "{[b,d]}",        "{(a,d]}",        },
        { __LINE__,  "[b,d]",  "(a,c)",  1,   false,  true,   true,   "(a,d]",  "[b,c)",  "{(a,d]}",        "{[c,d]}",        "{(a,b),[c,d]}",  },
        { __LINE__,  "[b,d]",  "(a,d)",  1,   false,  true,   true,   "(a,d]",  "[b,d)",  "{(a,d]}",        "{d}",            "{(a,b),d}",      },
        { __LINE__,  "[b,d]",  "(a,e)",  1,   false,  true,   true,   "(a,e)",  "[b,d]",  "{(a,e)}",        "{}",             "{(a,b),(d,e)}",  },
        { __LINE__,  "[b,d]",  "(b,c)",  -1,  true,   true,   true,   "[b,d]",  "(b,c)",  "{[b,d]}",        "{b,[c,d]}",      "{b,[c,d]}",      },
        { __LINE__,  "[b,d]",  "(b,d)",  -1,  true,   true,   true,   "[b,d]",  "(b,d)",  "{[b,d]}",        "{b,d}",          "{b,d}",          },
        { __LINE__,  "[b,d]",  "(b,e)",  -1,  false,  true,   true,   "[b,e)",  "(b,d]",  "{[b,e)}",        "{b}",            "{b,(d,e)}",      },
        { __LINE__,  "[b,d]",  "(c,d)",  -1,  true,   true,   true,   "[b,d]",  "(c,d)",  "{[b,d]}",        "{[b,c],d}",      "{[b,c],d}",      },
        { __LINE__,  "[b,d]",  "(c,e)",  -1,  false,  true,   true,   "[b,e)",  "(c,d]",  "{[b,e)}",        "{[b,c]}",        "{[b,c],(d,e)}",  },
        { __LINE__,  "[b,d]",  "(d,e)",  -1,  false,  false,  true,   "[b,e)",  "{}",     "{[b,e)}",        "{[b,d]}",        "{[b,e)}",        },
        { __LINE__,  "[b,d]",  "[a,b)",  1,   false,  false,  true,   "[a,d]",  "{}",     "{[a,d]}",        "{[b,d]}",        "{[a,d]}",        },
        { __LINE__,  "[b,d]",  "[a,c)",  1,   false,  true,   true,   "[a,d]",  "[b,c)",  "{[a,d]}",        "{[c,d]}",        "{[a,b),[c,d]}",  },
        { __LINE__,  "[b,d]",  "[a,d)",  1,   false,  true,   true,   "[a,d]",  "[b,d)",  "{[a,d]}",        "{d}",            "{[a,b),d}",      },
        { __LINE__,  "[b,d]",  "[a,e)",  1,   false,  true,   true,   "[a,e)",  "[b,d]",  "{[a,e)}",        "{}",             "{[a,b),(d,e)}",  },
        { __LINE__,  "[b,d]",  "[b,c)",  1,   true,   true,   true,   "[b,d]",  "[b,c)",  "{[b,d]}",        "{[c,d]}",        "{[c,d]}",        },
        { __LINE__,  "[b,d]",  "[b,d)",  1,   true,   true,   true,   "[b,d]",  "[b,d)",  "{[b,d]}",        "{d}",            "{d}",            },
        { __LINE__,  "[b,d]",  "[b,e)",  -1,  false,  true,   true,   "[b,e)",  "[b,d]",  "{[b,e)}",        "{}",             "{(d,e)}",        },
        { __LINE__,  "[b,d]",  "[c,d)",  -1,  true,   true,   true,   "[b,d]",  "[c,d)",  "{[b,d]}",        "{[b,c),d}",      "{[b,c),d}",      },
        { __LINE__,  "[b,d]",  "[c,e)",  -1,  false,  true,   true,   "[b,e)",  "[c,d]",  "{[b,e)}",        "{[b,c)}",        "{[b,c),(d,e)}",  },
        { __LINE__,  "[b,d]",  "[d,e)",  -1,  false,  true,   true,   "[b,e)",  "d",      "{[b,e)}",        "{[b,d)}",        "{[b,d),(d,e)}",  },
        { __LINE__,  "[b,d]",  "(a,b]",  1,   false,  true,   true,   "(a,d]",  "b",      "{(a,d]}",        "{(b,d]}",        "{(a,b),(b,d]}",  },
        { __LINE__,  "[b,d]",  "(a,c]",  1,   false,  true,   true,   "(a,d]",  "[b,c]",  "{(a,d]}",        "{(c,d]}",        "{(a,b),(c,d]}",  },
        { __LINE__,  "[b,d]",  "(a,d]",  1,   false,  true,   true,   "(a,d]",  "[b,d]",  "{(a,d]}",        "{}",             "{(a,b)}",        },
        { __LINE__,  "[b,d]",  "(a,e]",  1,   false,  true,   true,   "(a,e]",  "[b,d]",  "{(a,e]}",        "{}",             "{(a,b),(d,e]}",  },
        { __LINE__,  "[b,d]",  "(b,c]",  -1,  true,   true,   true,   "[b,d]",  "(b,c]",  "{[b,d]}",        "{b,(c,d]}",      "{b,(c,d]}",      },
        { __LINE__,  "[b,d]",  "(b,d]",  -1,  true,   true,   true,   "[b,d]",  "(b,d]",  "{[b,d]}",        "{b}",            "{b}",            },
        { __LINE__,  "[b,d]",  "(b,e]",  -1,  false,  true,   true,   "[b,e]",  "(b,d]",  "{[b,e]}",        "{b}",            "{b,(d,e]}",      },
        { __LINE__,  "[b,d]",  "(c,d]",  -1,  true,   true,   true,   "[b,d]",  "(c,d]",  "{[b,d]}",        "{[b,c]}",        "{[b,c]}",        },
        { __LINE__,  "[b,d]",  "(c,e]",  -1,  false,  true,   true,   "[b,e]",  "(c,d]",  "{[b,e]}",        "{[b,c]}",        "{[b,c],(d,e]}",  },
        { __LINE__,  "[b,d]",  "(d,e]",  -1,  false,  false,  true,   "[b,e]",  "{}",     "{[b,e]}",        "{[b,d]}",        "{[b,e]}",        },
        { __LINE__,  "[b,d]",  "[a,a]",  1,   false,  false,  false,  "[a,d]",  "{}",     "{a,[b,d]}",      "{[b,d]}",        "{a,[b,d]}",      },
        { __LINE__,  "[b,d]",  "[a,b]",  1,   false,  true,   true,   "[a,d]",  "b",      "{[a,d]}",        "{(b,d]}",        "{[a,b),(b,d]}",  },
        { __LINE__,  "[b,d]",  "[a,c]",  1,   false,  true,   true,   "[a,d]",  "[b,c]",  "{[a,d]}",        "{(c,d]}",        "{[a,b),(c,d]}",  },
        { __LINE__,  "[b,d]",  "[a,d]",  1,   false,  true,   true,   "[a,d]",  "[b,d]",  "{[a,d]}",        "{}",             "{[a,b)}",        },
        { __LINE__,  "[b,d]",  "[a,e]",  1,   false,  true,   true,   "[a,e]",  "[b,d]",  "{[a,e]}",        "{}",             "{[a,b),(d,e]}",  },
        { __LINE__,  "[b,d]",  "[b,c]",  1,   true,   true,   true,   "[b,d]",  "[b,c]",  "{[b,d]}",        "{(c,d]}",        "{(c,d]}",        },
        { __LINE__,  "[b,d]",  "[b,d]",  0,   true,   true,   true,   "[b,d]",  "[b,d]",  "{[b,d]}",        "{}",             "{}",             },
        { __LINE__,  "[b,d]",  "[b,e]",  -1,  false,  true,   true,   "[b,e]",  "[b,d]",  "{[b,e]}",        "{}",             "{(d,e]}",        },
        { __LINE__,  "[b,d]",  "[c,d]",  -1,  true,   true,   true,   "[b,d]",  "[c,d]",  "{[b,d]}",        "{[b,c)}",        "{[b,c)}",        },
        { __LINE__,  "[b,d]",  "[c,e]",  -1,  false,  true,   true,   "[b,e]",  "[c,d]",  "{[b,e]}",        "{[b,c)}",        "{[b,c),(d,e]}",  },
        { __LINE__,  "[b,d]",  "[d,e]",  -1,  false,  true,   true,   "[b,e]",  "d",      "{[b,e]}",        "{[b,d)}",        "{[b,d),(d,e]}",  },

    };

    Itype in1, in2;
    int cmp = 0;
    bool flag = false;
    Ustring str;

    for (auto& t: tests) {

        int errors = 0;

        TRY(in1 = from_str<Itype>(t.lhs));
        TRY(in2 = from_str<Itype>(t.rhs));

        TRY(cmp = in1.compare(in2));           TEST_EQUAL(cmp, t.compare);                   errors += int(cmp != t.compare);
        TRY(flag = in1.includes(in2));         TEST_EQUAL(flag, t.includes);                 errors += int(flag != t.includes);
        TRY(flag = in1.overlaps(in2));         TEST_EQUAL(flag, t.overlaps);                 errors += int(flag != t.overlaps);
        TRY(flag = in1.touches(in2));          TEST_EQUAL(flag, t.touches);                  errors += int(flag != t.touches);
        TRY(str = to_str(in1.envelope(in2)));  TEST_EQUAL(str, t.envelope);                  errors += int(str != t.envelope);
        TRY(str = to_str(in1 & in2));          TEST_EQUAL(str, t.set_intersection);          errors += int(str != t.set_intersection);
        TRY(str = to_str(in1 | in2));          TEST_EQUAL(str, t.set_union);                 errors += int(str != t.set_union);
        TRY(str = to_str(in1 - in2));          TEST_EQUAL(str, t.set_difference);            errors += int(str != t.set_difference);
        TRY(str = to_str(in1 ^ in2));          TEST_EQUAL(str, t.set_symmetric_difference);  errors += int(str != t.set_symmetric_difference);

        if (errors)
            std::cout << "... [" << t.line << "] " << t.lhs << " " << t.rhs << "\n";

    }

}

void test_source_interval_ordered_set_construct_insert_erase() {

    Iset set, inv;
    Iset::iterator it;
    Itype in;
    Ustring str;

    TEST(set.empty());
    TRY(set = "hello"s);
    TEST_EQUAL(set.size(), 1);
    TRY(it = set.begin());
    TEST_EQUAL(it->str(), "hello");
    TRY(++it);
    TEST(it == set.end());
    TRY((in = {"hello","world"}));
    TRY(set = in);
    TEST_EQUAL(set.size(), 1);
    TRY(it = set.begin());
    TEST_EQUAL(it->str(), "[hello,world]");
    TRY(++it);
    TEST(it == set.end());
    TRY((set = {{"abc","def"},{"ghi","jkl"},{"mno","pqr"}}));
    TRY(it = set.begin());
    TEST_EQUAL(it->str(), "[abc,def]");
    TRY(++it);
    TEST_EQUAL(it->str(), "[ghi,jkl]");
    TRY(++it);
    TEST_EQUAL(it->str(), "[mno,pqr]");
    TRY(++it);
    TEST(it == set.end());

    TRY(set = from_str<Iset>(""));                     TRY(str = to_str(set));  TEST_EQUAL(str, "{}");
    TRY(set = from_str<Iset>("{}"));                   TRY(str = to_str(set));  TEST_EQUAL(str, "{}");
    TRY(set = from_str<Iset>("(a,b)"));                TRY(str = to_str(set));  TEST_EQUAL(str, "{(a,b)}");
    TRY(set = from_str<Iset>("{(a,b)}"));              TRY(str = to_str(set));  TEST_EQUAL(str, "{(a,b)}");
    TRY(set = from_str<Iset>("{(a,b),(c,d),(e,f)}"));  TRY(str = to_str(set));  TEST_EQUAL(str, "{(a,b),(c,d),(e,f)}");
    TRY(set = from_str<Iset>("{[a,b],[c,d],[e,f]}"));  TRY(str = to_str(set));  TEST_EQUAL(str, "{[a,b],[c,d],[e,f]}");

    TRY((set = {{"a","b","[)"},{"c","d","(]"},{"e","f","()"}}));
    TRY(inv = set.inverse());
    TRY(str = to_str(set));
    TEST_EQUAL(str, "{[a,b),(c,d],(e,f)}");
    TRY(str = to_str(inv));
    TEST_EQUAL(str, "{<a,[b,c],(d,e],>=f}");

    TEST(! set.contains("@x"));  TEST(inv.contains("@x"));
    TEST(set.contains("a"));     TEST(! inv.contains("a"));
    TEST(set.contains("ax"));    TEST(! inv.contains("ax"));
    TEST(! set.contains("b"));   TEST(inv.contains("b"));
    TEST(! set.contains("bx"));  TEST(inv.contains("bx"));
    TEST(! set.contains("c"));   TEST(inv.contains("c"));
    TEST(set.contains("cx"));    TEST(! inv.contains("cx"));
    TEST(set.contains("d"));     TEST(! inv.contains("d"));
    TEST(! set.contains("dx"));  TEST(inv.contains("dx"));
    TEST(! set.contains("e"));   TEST(inv.contains("e"));
    TEST(set.contains("ex"));    TEST(! inv.contains("ex"));
    TEST(! set.contains("f"));   TEST(inv.contains("f"));
    TEST(! set.contains("fx"));  TEST(inv.contains("fx"));
    TEST(! set.contains("g"));   TEST(inv.contains("g"));

    TRY(set.clear());
    TEST(set.empty());

    TRY((set.insert({"jj","tt"})));       TRY(str = to_str(set));  TEST_EQUAL(str, "{[jj,tt]}");
    TRY((set.insert({"tt","xx","()"})));  TRY(str = to_str(set));  TEST_EQUAL(str, "{[jj,xx)}");
    TRY((set.erase({"ee","jj","()"})));   TRY(str = to_str(set));  TEST_EQUAL(str, "{[jj,xx)}");
    TRY((set.erase({"ee","jj"})));        TRY(str = to_str(set));  TEST_EQUAL(str, "{(jj,xx)}");
    TRY((set.erase({"ll","nn"})));        TRY(str = to_str(set));  TEST_EQUAL(str, "{(jj,ll),(nn,xx)}");
    TRY((set.erase({"pp","rr","()"})));   TRY(str = to_str(set));  TEST_EQUAL(str, "{(jj,ll),(nn,pp],[rr,xx)}");
    TRY((set.insert({"ii","kk"})));       TRY(str = to_str(set));  TEST_EQUAL(str, "{[ii,ll),(nn,pp],[rr,xx)}");
    TRY((set.insert({"ww","yy","()"})));  TRY(str = to_str(set));  TEST_EQUAL(str, "{[ii,ll),(nn,pp],[rr,yy)}");

}

void test_source_interval_ordered_set_operations() {

    Iset set[2], i_set, u_set, d_set, sd_set;
    std::vector<Itype> vec[2];
    Iset::iterator it;
    Itype in;
    Xoshiro rng(42);
    Ustring x;

    static constexpr int iterations = 1000;
    static constexpr int max_size = 10;

    for (int i = 0; i < iterations; ++i) {

        for (int j = 0; j < 2; ++j) {
            TRY(set[j].clear());
            vec[j].clear();
            int size = random_integer(1, max_size)(rng);
            for (int k = 0; k < size; ++k) {
                auto a = Ustring(2, char(random_integer('a', 'z')(rng)));
                auto b = Ustring(2, char(random_integer('a', 'z')(rng)));
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

        for (char y = 'a'; y <= 'z'; ++y) {
            for (char z = 'a'; z <= 'z'; ++z) {
                x = {y,z};
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

}

void test_source_interval_ordered_map() {

    Imap map;
    Imap::iterator it;

    TEST(map.empty());
    TEST_EQUAL(map.size(), 0);
    TEST_EQUAL(map.default_value(), "");
    TEST_EQUAL(map["hello"], "");
    TEST_EQUAL(to_str(map), "{}");

    TRY(map.insert(from_str<Itype>("<=b"), "alpha"));
    TRY(map.insert(from_str<Itype>("(c,g)"), "bravo"));
    TRY(map.insert(from_str<Itype>(">=h"), "charlie"));
    TEST_EQUAL(map.size(), 3);
    TEST_EQUAL(map["a"], "alpha");
    TEST_EQUAL(map["b"], "alpha");
    TEST_EQUAL(map["c"], "");
    TEST_EQUAL(map["d"], "bravo");
    TEST_EQUAL(map["e"], "bravo");
    TEST_EQUAL(map["f"], "bravo");
    TEST_EQUAL(map["g"], "");
    TEST_EQUAL(map["h"], "charlie");
    TEST_EQUAL(map["i"], "charlie");
    TRY(map.default_value("nil"));
    TEST_EQUAL(map["a"], "alpha");
    TEST_EQUAL(map["b"], "alpha");
    TEST_EQUAL(map["c"], "nil");
    TEST_EQUAL(map["d"], "bravo");
    TEST_EQUAL(map["e"], "bravo");
    TEST_EQUAL(map["f"], "bravo");
    TEST_EQUAL(map["g"], "nil");
    TEST_EQUAL(map["h"], "charlie");
    TEST_EQUAL(map["i"], "charlie");
    TEST_EQUAL(to_str(map), "{<=b:alpha,(c,g):bravo,>=h:charlie}");

    TEST(map.contains("a"));
    TEST(map.contains("b"));
    TEST(! map.contains("c"));
    TEST(map.contains("d"));
    TEST(map.contains("e"));
    TEST(map.contains("f"));
    TEST(! map.contains("g"));
    TEST(map.contains("h"));
    TEST(map.contains("i"));

    TRY(it = map.find("a"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.find("b"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.find("c"));  TEST(it == map.end());
    TRY(it = map.find("d"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.find("e"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.find("f"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.find("g"));  TEST(it == map.end());
    TRY(it = map.find("h"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.find("i"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");

    TRY(it = map.lower_bound("a"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.lower_bound("b"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "alpha");
    TRY(it = map.lower_bound("c"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound("d"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound("e"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound("f"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.lower_bound("g"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.lower_bound("h"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.lower_bound("i"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");

    TRY(it = map.upper_bound("a"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.upper_bound("b"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.upper_bound("c"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "bravo");
    TRY(it = map.upper_bound("d"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound("e"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound("f"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound("g"));  REQUIRE(it != map.end());  TEST_EQUAL(it->second, "charlie");
    TRY(it = map.upper_bound("h"));  TEST(it == map.end());
    TRY(it = map.upper_bound("i"));  TEST(it == map.end());

    TRY(map.clear());
    TEST(map.empty());
    TRY((map = {
        {from_str<Itype>("<=bb"), "alpha"},
        {from_str<Itype>("(cc,gg)"), "bravo"},
        {from_str<Itype>(">=hh"), "charlie"},
    }));
    TEST_EQUAL(map.size(), 3);
    TEST_EQUAL(to_str(map), "{<=bb:alpha,(cc,gg):bravo,>=hh:charlie}");

    TRY(map.clear());
    TRY(map.default_value("nil"));
    TRY(map.insert(from_str<Itype>("<=j"), "alpha"));
    TRY(map.insert(from_str<Itype>("[a,e]"), "bravo"));
    TRY(map.insert(from_str<Itype>("[e,f]"), "charlie"));
    TRY(map.insert(from_str<Itype>("(a,d)"), "delta"));
    TRY(map.insert(from_str<Itype>("[f,h]"), "charlie"));
    TEST_EQUAL(map.size(), 6);
    TEST_EQUAL(to_str(map),
        "{<a:alpha,"
        "a:bravo,"
        "(a,d):delta,"
        "[d,e):bravo,"
        "[e,h]:charlie,"
        "(h,j]:alpha}");
    TEST_EQUAL(map["!"], "alpha");
    TEST_EQUAL(map["a"], "bravo");
    TEST_EQUAL(map["b"], "delta");
    TEST_EQUAL(map["c"], "delta");
    TEST_EQUAL(map["d"], "bravo");
    TEST_EQUAL(map["e"], "charlie");
    TEST_EQUAL(map["f"], "charlie");
    TEST_EQUAL(map["g"], "charlie");
    TEST_EQUAL(map["h"], "charlie");
    TEST_EQUAL(map["i"], "alpha");
    TEST_EQUAL(map["j"], "alpha");
    TEST_EQUAL(map["k"], "nil");
    TEST_EQUAL(map["l"], "nil");

    TRY(map.erase(from_str<Itype>("[a,b]")));
    TRY(map.erase(from_str<Itype>("(f,h)")));
    TRY(map.erase(from_str<Itype>(">=j")));
    TEST_EQUAL(map.size(), 6);
    TEST_EQUAL(to_str(map),
        "{<a:alpha,"
        "(b,d):delta,"
        "[d,e):bravo,"
        "[e,f]:charlie,"
        "h:charlie,"
        "(h,j):alpha}");
    TEST_EQUAL(map["!"], "alpha");
    TEST_EQUAL(map["a"], "nil");
    TEST_EQUAL(map["b"], "nil");
    TEST_EQUAL(map["c"], "delta");
    TEST_EQUAL(map["d"], "bravo");
    TEST_EQUAL(map["e"], "charlie");
    TEST_EQUAL(map["f"], "charlie");
    TEST_EQUAL(map["g"], "nil");
    TEST_EQUAL(map["h"], "charlie");
    TEST_EQUAL(map["i"], "alpha");
    TEST_EQUAL(map["j"], "nil");
    TEST_EQUAL(map["k"], "nil");
    TEST_EQUAL(map["l"], "nil");

}
