#include "rs-core/range-expansion.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <functional>

using namespace RS;
using namespace RS::Range;
using namespace std::literals;

void test_core_range_expansion_combinations() {

    Ustring s;
    Strings v;

    TRY(""s >> combinations(0) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 0);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[]");

    v.clear();
    TRY("abc"s >> combinations(0) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 1);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[]");

    v.clear();
    TRY("abc"s >> combinations(1) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 3);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[a,b,c]");

    v.clear();
    TRY("abc"s >> combinations(2) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 9);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[aa,ab,ac,ba,bb,bc,ca,cb,cc]");

    v.clear();
    TRY("abc"s >> combinations(3) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 27);
    TRY(s = to_str(v));
    TEST_EQUAL(s,
        "[aaa,aab,aac,aba,abb,abc,aca,acb,acc,"
        "baa,bab,bac,bba,bbb,bbc,bca,bcb,bcc,"
        "caa,cab,cac,cba,cbb,cbc,cca,ccb,ccc]");

    v.clear();
    TRY("abc"s >> combinations(4) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 81);
    TRY(s = to_str(v));
    TEST_EQUAL(s,
        "[aaaa,aaab,aaac,aaba,aabb,aabc,aaca,aacb,aacc,"
        "abaa,abab,abac,abba,abbb,abbc,abca,abcb,abcc,"
        "acaa,acab,acac,acba,acbb,acbc,acca,accb,accc,"
        "baaa,baab,baac,baba,babb,babc,baca,bacb,bacc,"
        "bbaa,bbab,bbac,bbba,bbbb,bbbc,bbca,bbcb,bbcc,"
        "bcaa,bcab,bcac,bcba,bcbb,bcbc,bcca,bccb,bccc,"
        "caaa,caab,caac,caba,cabb,cabc,caca,cacb,cacc,"
        "cbaa,cbab,cbac,cbba,cbbb,cbbc,cbca,cbcb,cbcc,"
        "ccaa,ccab,ccac,ccba,ccbb,ccbc,ccca,cccb,cccc]");

}

void test_core_range_expansion_flat_map() {

    Ustring s1, s2 = "abcde";
    auto f = [] (char c) { return Ustring(c - 'a' + 1, c); };

    TRY("abcde"s >> flat_map(f) >> overwrite(s1));                                TEST_EQUAL(s1, "abbcccddddeeeee");
    TRY("abcde"s >> passthrough >> flat_map(f) >> passthrough >> overwrite(s1));  TEST_EQUAL(s1, "abbcccddddeeeee");
    TRY("abcde"s >> passthrough * flat_map(f) * passthrough >> overwrite(s1));    TEST_EQUAL(s1, "abbcccddddeeeee");
    TRY(s2 << flat_map(f));                                                       TEST_EQUAL(s2, "abbcccddddeeeee");

}

void test_core_range_expansion_flatten() {

    Ustring s;
    Strings v = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};

    TRY(v >> flatten >> overwrite(s));                                TEST_EQUAL(s, "AlphaBravoCharlieDeltaEcho");
    TRY(v >> passthrough >> flatten >> passthrough >> overwrite(s));  TEST_EQUAL(s, "AlphaBravoCharlieDeltaEcho");
    TRY(v >> passthrough * flatten * passthrough >> overwrite(s));    TEST_EQUAL(s, "AlphaBravoCharlieDeltaEcho");

}

void test_core_range_expansion_insert_algorithms() {

    Ustring s;

    TRY(""s >> insert_before('*') >> overwrite(s));                                     TEST_EQUAL(s, "");
    TRY("abcde"s >> insert_before('*') >> overwrite(s));                                TEST_EQUAL(s, "*a*b*c*d*e");
    TRY("abcde"s >> passthrough >> insert_before('*') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "*a*b*c*d*e");
    TRY("abcde"s >> passthrough * insert_before('*') * passthrough >> overwrite(s));    TEST_EQUAL(s, "*a*b*c*d*e");
    s = "abcde"; TRY(s << insert_before('*'));                                          TEST_EQUAL(s, "*a*b*c*d*e");

    TRY(""s >> insert_after('*') >> overwrite(s));                                     TEST_EQUAL(s, "");
    TRY("abcde"s >> insert_after('*') >> overwrite(s));                                TEST_EQUAL(s, "a*b*c*d*e*");
    TRY("abcde"s >> passthrough >> insert_after('*') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "a*b*c*d*e*");
    TRY("abcde"s >> passthrough * insert_after('*') * passthrough >> overwrite(s));    TEST_EQUAL(s, "a*b*c*d*e*");
    s = "abcde"; TRY(s << insert_after('*'));                                          TEST_EQUAL(s, "a*b*c*d*e*");

    TRY(""s >> insert_between('*') >> overwrite(s));                                     TEST_EQUAL(s, "");
    TRY("abcde"s >> insert_between('*') >> overwrite(s));                                TEST_EQUAL(s, "a*b*c*d*e");
    TRY("abcde"s >> passthrough >> insert_between('*') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "a*b*c*d*e");
    TRY("abcde"s >> passthrough * insert_between('*') * passthrough >> overwrite(s));    TEST_EQUAL(s, "a*b*c*d*e");
    s = "abcde"; TRY(s << insert_between('*'));                                          TEST_EQUAL(s, "a*b*c*d*e");

    TRY(""s >> insert_around('(', ')') >> overwrite(s));                                     TEST_EQUAL(s, "");
    TRY("abcde"s >> insert_around('(', ')') >> overwrite(s));                                TEST_EQUAL(s, "(a)(b)(c)(d)(e)");
    TRY("abcde"s >> passthrough >> insert_around('(', ')') >> passthrough >> overwrite(s));  TEST_EQUAL(s, "(a)(b)(c)(d)(e)");
    TRY("abcde"s >> passthrough * insert_around('(', ')') * passthrough >> overwrite(s));    TEST_EQUAL(s, "(a)(b)(c)(d)(e)");
    s = "abcde"; TRY(s << insert_around('(', ')'));                                          TEST_EQUAL(s, "(a)(b)(c)(d)(e)");

}

void test_core_range_expansion_permutations() {

    Ustring s;

    TRY("abcd"s >> permutations >> each([&] (auto& x) { s.append(x.begin(), x.end()); s += ";"; }));
    TEST_EQUAL(s,
        "abcd;abdc;acbd;acdb;adbc;adcb;"
        "bacd;badc;bcad;bcda;bdac;bdca;"
        "cabd;cadb;cbad;cbda;cdab;cdba;"
        "dabc;dacb;dbac;dbca;dcab;dcba;"
    );

    s.clear();
    TRY("dcba"s >> permutations(std::greater<>()) >> each([&] (auto& x) { s.append(x.begin(), x.end()); s += ";"; }));
    TEST_EQUAL(s,
        "dcba;dcab;dbca;dbac;dacb;dabc;"
        "cdba;cdab;cbda;cbad;cadb;cabd;"
        "bdca;bdac;bcda;bcad;badc;bacd;"
        "adcb;adbc;acdb;acbd;abdc;abcd;"
    );

}

void test_core_range_expansion_repeat() {

    using RS::Range::repeat;

    Ustring s;

    TRY("Hello"s >> repeat(0) >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> repeat(1) >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> repeat(2) >> overwrite(s));  TEST_EQUAL(s, "HelloHello");
    TRY("Hello"s >> repeat(3) >> overwrite(s));  TEST_EQUAL(s, "HelloHelloHello");
    TRY("Hello"s >> repeat(4) >> overwrite(s));  TEST_EQUAL(s, "HelloHelloHelloHello");
    TRY("Hello"s >> repeat(5) >> overwrite(s));  TEST_EQUAL(s, "HelloHelloHelloHelloHello");

    TRY("Hello"s >> passthrough >> repeat(0) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "");
    TRY("Hello"s >> passthrough >> repeat(1) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "Hello");
    TRY("Hello"s >> passthrough >> repeat(2) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "HelloHello");
    TRY("Hello"s >> passthrough >> repeat(3) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "HelloHelloHello");
    TRY("Hello"s >> passthrough >> repeat(4) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "HelloHelloHelloHello");
    TRY("Hello"s >> passthrough >> repeat(5) >> passthrough >> overwrite(s));  TEST_EQUAL(s, "HelloHelloHelloHelloHello");

    s = "Hello";  TRY(s << repeat(0));  TEST_EQUAL(s, "");
    s = "Hello";  TRY(s << repeat(1));  TEST_EQUAL(s, "Hello");
    s = "Hello";  TRY(s << repeat(2));  TEST_EQUAL(s, "HelloHello");
    s = "Hello";  TRY(s << repeat(3));  TEST_EQUAL(s, "HelloHelloHello");
    s = "Hello";  TRY(s << repeat(4));  TEST_EQUAL(s, "HelloHelloHelloHello");
    s = "Hello";  TRY(s << repeat(5));  TEST_EQUAL(s, "HelloHelloHelloHelloHello");

    TRY("Hello"s >> passthrough * repeat(5) >> overwrite(s));                TEST_EQUAL(s, "HelloHelloHelloHelloHello");
    TRY("Hello"s >> repeat(5) * passthrough >> overwrite(s));                TEST_EQUAL(s, "HelloHelloHelloHelloHello");
    TRY("Hello"s >> passthrough * repeat(5) * passthrough >> overwrite(s));  TEST_EQUAL(s, "HelloHelloHelloHelloHello");

    s = "Hello";
    auto r = s >> repeat(3);
    auto i = r.begin(), j = i;

    TEST_EQUAL(r.end() - r.begin(), 15);
    TEST_EQUAL(r.begin() - r.end(), -15);

    /**/       TEST_EQUAL(*i, 'H');  TEST_EQUAL(i - r.begin(), 0);   TEST_EQUAL(i - r.end(), -15);  TRY(j = r.begin());  TRY(j += 0);   TEST(i == j);  TEST_EQUAL(*j, 'H');
    TRY(++i);  TEST_EQUAL(*i, 'e');  TEST_EQUAL(i - r.begin(), 1);   TEST_EQUAL(i - r.end(), -14);  TRY(j = r.begin());  TRY(j += 1);   TEST(i == j);  TEST_EQUAL(*j, 'e');
    TRY(++i);  TEST_EQUAL(*i, 'l');  TEST_EQUAL(i - r.begin(), 2);   TEST_EQUAL(i - r.end(), -13);  TRY(j = r.begin());  TRY(j += 2);   TEST(i == j);  TEST_EQUAL(*j, 'l');
    TRY(++i);  TEST_EQUAL(*i, 'l');  TEST_EQUAL(i - r.begin(), 3);   TEST_EQUAL(i - r.end(), -12);  TRY(j = r.begin());  TRY(j += 3);   TEST(i == j);  TEST_EQUAL(*j, 'l');
    TRY(++i);  TEST_EQUAL(*i, 'o');  TEST_EQUAL(i - r.begin(), 4);   TEST_EQUAL(i - r.end(), -11);  TRY(j = r.begin());  TRY(j += 4);   TEST(i == j);  TEST_EQUAL(*j, 'o');
    TRY(++i);  TEST_EQUAL(*i, 'H');  TEST_EQUAL(i - r.begin(), 5);   TEST_EQUAL(i - r.end(), -10);  TRY(j = r.begin());  TRY(j += 5);   TEST(i == j);  TEST_EQUAL(*j, 'H');
    TRY(++i);  TEST_EQUAL(*i, 'e');  TEST_EQUAL(i - r.begin(), 6);   TEST_EQUAL(i - r.end(), -9);   TRY(j = r.begin());  TRY(j += 6);   TEST(i == j);  TEST_EQUAL(*j, 'e');
    TRY(++i);  TEST_EQUAL(*i, 'l');  TEST_EQUAL(i - r.begin(), 7);   TEST_EQUAL(i - r.end(), -8);   TRY(j = r.begin());  TRY(j += 7);   TEST(i == j);  TEST_EQUAL(*j, 'l');
    TRY(++i);  TEST_EQUAL(*i, 'l');  TEST_EQUAL(i - r.begin(), 8);   TEST_EQUAL(i - r.end(), -7);   TRY(j = r.begin());  TRY(j += 8);   TEST(i == j);  TEST_EQUAL(*j, 'l');
    TRY(++i);  TEST_EQUAL(*i, 'o');  TEST_EQUAL(i - r.begin(), 9);   TEST_EQUAL(i - r.end(), -6);   TRY(j = r.begin());  TRY(j += 9);   TEST(i == j);  TEST_EQUAL(*j, 'o');
    TRY(++i);  TEST_EQUAL(*i, 'H');  TEST_EQUAL(i - r.begin(), 10);  TEST_EQUAL(i - r.end(), -5);   TRY(j = r.begin());  TRY(j += 10);  TEST(i == j);  TEST_EQUAL(*j, 'H');
    TRY(++i);  TEST_EQUAL(*i, 'e');  TEST_EQUAL(i - r.begin(), 11);  TEST_EQUAL(i - r.end(), -4);   TRY(j = r.begin());  TRY(j += 11);  TEST(i == j);  TEST_EQUAL(*j, 'e');
    TRY(++i);  TEST_EQUAL(*i, 'l');  TEST_EQUAL(i - r.begin(), 12);  TEST_EQUAL(i - r.end(), -3);   TRY(j = r.begin());  TRY(j += 12);  TEST(i == j);  TEST_EQUAL(*j, 'l');
    TRY(++i);  TEST_EQUAL(*i, 'l');  TEST_EQUAL(i - r.begin(), 13);  TEST_EQUAL(i - r.end(), -2);   TRY(j = r.begin());  TRY(j += 13);  TEST(i == j);  TEST_EQUAL(*j, 'l');
    TRY(++i);  TEST_EQUAL(*i, 'o');  TEST_EQUAL(i - r.begin(), 14);  TEST_EQUAL(i - r.end(), -1);   TRY(j = r.begin());  TRY(j += 14);  TEST(i == j);  TEST_EQUAL(*j, 'o');
    TRY(++i);  /**/                  TEST_EQUAL(i - r.begin(), 15);  TEST_EQUAL(i - r.end(), 0);    TRY(j = r.begin());  TRY(j += 15);  TEST(i == j);  /**/

}

void test_core_range_expansion_subsets() {

    Ustring s;
    Strings v;

    TRY(""s >> subsets(0) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 1);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[]");

    v.clear();
    TRY("abcd"s >> subsets(0) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 1);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[]");

    v.clear();
    TRY("abcd"s >> subsets(1) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 4);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[a,b,c,d]");

    v.clear();
    TRY("abcd"s >> subsets(2) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 6);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[ab,ac,ad,bc,bd,cd]");

    v.clear();
    TRY("abcd"s >> subsets(3) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 4);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[abc,abd,acd,bcd]");

    v.clear();
    TRY("abcd"s >> subsets(4) >> each([&] (auto& x) { v.push_back(Ustring(x.begin(), x.end())); }));
    TEST_EQUAL(v.size(), 1);
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[abcd]");

    for (size_t n = 0; n <= 10; ++n) {
        for (size_t k = 0; k <= n; ++k) {
            size_t count = 0;
            TRY(Ustring(n, 'a') >> subsets(k) >> each([&] (auto&) { ++count; }));
            TEST_EQUAL(count, binomial(n, k));
        }
    }

}
