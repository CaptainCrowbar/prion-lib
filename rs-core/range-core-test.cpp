#include "rs-core/range-core.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <forward_list>
#include <iterator>
#include <map>

using namespace RS;
using namespace RS::Range;
using namespace std::literals;

void test_core_range_core_output() {

    Ustring s;

    s.clear();  TRY("Hello"s >> output(std::back_inserter(s)));                                                              TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> output(std::back_inserter(s)));                                               TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> passthrough >> output(std::back_inserter(s)));                                TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> passthrough >> passthrough >> output(std::back_inserter(s)));                 TEST_EQUAL(s, "Hello");
    s.clear();  TRY("Hello"s >> passthrough >> passthrough >> passthrough >> passthrough >> output(std::back_inserter(s)));  TEST_EQUAL(s, "Hello");

}

void test_core_range_core_passthrough() {

    Ustring s;

    TRY("Hello"s >> overwrite(s));                                                              TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> overwrite(s));                                               TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> passthrough >> overwrite(s));                                TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> passthrough >> passthrough >> overwrite(s));                 TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> passthrough >> passthrough >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");

}

void test_core_range_core_collect() {

    Ustring s;
    std::forward_list<char> fl = {'H','e','l','l','o'};

    auto r1 = fl >> collect;
    TRY(std::reverse(r1.begin(), r1.end()));
    TRY(r1 >> overwrite(s));
    TEST_EQUAL(s, "olleH");

    auto r2 = fl >> passthrough >> collect;
    TRY(std::reverse(r2.begin(), r2.end()));
    TRY(r2 >> passthrough >> overwrite(s));
    TEST_EQUAL(s, "olleH");

}

void test_core_range_core_each() {

    Ustring s;

    auto f1 = [&s] (char c) { s += c; s += '.'; };
    TRY("Hello"s >> each(f1));
    TEST_EQUAL(s, "H.e.l.l.o.");
    s.clear();
    TRY("Hello"s >> passthrough >> each(f1));
    TEST_EQUAL(s, "H.e.l.l.o.");

    auto f2 = [] (char& c) { c = ascii_toupper(c); };
    s = "Hello";
    TRY(s << each(f2));
    TEST_EQUAL(s, "HELLO");

    std::map<int, Ustring> m = {{1, "alpha"}, {2, "bravo"}, {3, "charlie"}};

    auto f3 = [&s] (int k, Ustring v) { s += dec(k) + ':' + v + ';'; };
    s.clear();
    TRY(m >> each_pair(f3));
    TEST_EQUAL(s, "1:alpha;2:bravo;3:charlie;");
    s.clear();
    TRY(m >> passthrough >> each_pair(f3));
    TEST_EQUAL(s, "1:alpha;2:bravo;3:charlie;");

    auto f4 = [] (int k, Ustring& v) { v += '.' + dec(k); };
    TRY(m << each_pair(f4));
    TRY(s = to_str(m));
    TEST_EQUAL(s, "{1:alpha.1,2:bravo.2,3:charlie.3}");

}

void test_core_range_core_combinator() {

    Ustring s;

    auto c1 = passthrough * passthrough;
    TRY("Hello"s >> c1 >> overwrite(s));
    TEST_EQUAL(s, "Hello");

    auto f1 = [] (char& c) { c = ascii_toupper(c); };
    auto f2 = [] (char& c) { c += 0x20; };
    auto c2 = each(f1) * each(f2);
    s = "Hello";
    TRY(s << c2);
    TEST_EQUAL(s, "hello");

}
