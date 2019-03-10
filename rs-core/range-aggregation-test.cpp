#include "rs-core/range-aggregation.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <functional>
#include <map>
#include <vector>

using namespace RS;
using namespace RS::Range;
using namespace std::literals;

void test_core_range_aggregation_adjacent_difference() {

    Ustring s;
    std::vector<int> v;
    auto f = [] (int a, int b) { return 10 * (a - b); };

    TRY("Hello"s >> adjacent_difference(std::minus<int>()) >> overwrite(v));
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[29,7,0,3]");
    TRY("Hello"s >> adjacent_difference(f) >> overwrite(v));
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[290,70,0,30]");

    TRY("Hello"s >> passthrough >> adjacent_difference(std::minus<int>()) >> passthrough >> overwrite(v));
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[29,7,0,3]");
    TRY("Hello"s >> passthrough >> adjacent_difference(f) >> passthrough >> overwrite(v));
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[290,70,0,30]");

    v = {2,3,5,7,11,19,23,29};
    TRY(v << adjacent_difference(std::minus<int>()));
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[1,2,2,4,8,4,6]");
    v = {2,3,5,7,11,19,23,29};
    TRY(v << adjacent_difference(f));
    TRY(s = to_str(v));
    TEST_EQUAL(s, "[10,20,20,40,80,40,60]");

    auto a = adjacent_difference(std::minus<int>());  TRY("Hello"s >> a >> overwrite(v));  TEST_EQUAL(to_str(v), "[29,7,0,3]");
    auto b = passthrough * a;                         TRY("Hello"s >> b >> overwrite(v));  TEST_EQUAL(to_str(v), "[29,7,0,3]");
    auto c = a * passthrough;                         TRY("Hello"s >> c >> overwrite(v));  TEST_EQUAL(to_str(v), "[29,7,0,3]");
    auto d = passthrough * a * passthrough;           TRY("Hello"s >> d >> overwrite(v));  TEST_EQUAL(to_str(v), "[29,7,0,3]");

}

void test_core_range_aggregation_census() {

    Ustring s;
    std::map<char, size_t> m;
    auto f = [] (char a, char b) { return (ascii_islower(a) && ascii_islower(b)) || (ascii_isupper(a) && ascii_isupper(b)); };

    TRY("abbcccddddeeeee"s >> census >> overwrite(m));
    TRY(s = to_str(m));
    TEST_EQUAL(s, "{a:1,b:2,c:3,d:4,e:5}");
    TRY("Hello World"s >> census(f) >> overwrite(m));
    TRY(s = to_str(m));
    TEST_EQUAL(s, "{ :1,H:1,W:1,e:4,o:4}");

    TRY("abbcccddddeeeee"s >> passthrough >> census >> passthrough >> overwrite(m));
    TRY(s = to_str(m));
    TEST_EQUAL(s, "{a:1,b:2,c:3,d:4,e:5}");
    TRY("Hello World"s >> passthrough >> census(f) >> passthrough >> overwrite(m));
    TRY(s = to_str(m));
    TEST_EQUAL(s, "{ :1,H:1,W:1,e:4,o:4}");

    TRY("abbcccddddeeeee"s >> census >> overwrite(m));                              TEST_EQUAL(to_str(m), "{a:1,b:2,c:3,d:4,e:5}");
    TRY("abbcccddddeeeee"s >> passthrough * census >> overwrite(m));                TEST_EQUAL(to_str(m), "{a:1,b:2,c:3,d:4,e:5}");
    TRY("abbcccddddeeeee"s >> census * passthrough >> overwrite(m));                TEST_EQUAL(to_str(m), "{a:1,b:2,c:3,d:4,e:5}");
    TRY("abbcccddddeeeee"s >> passthrough * census * passthrough >> overwrite(m));  TEST_EQUAL(to_str(m), "{a:1,b:2,c:3,d:4,e:5}");

}

void test_core_range_aggregation_collect_groups() {

    std::vector<int> u, v;

    u = {1,2,2,3,3,3,4,4,4,4,5,5,5,5,5};
    TRY(u >> collect_groups(std::equal_to<>()) >> overwrite(v));
    TEST_EQUAL(to_str(v), "[1,4,6,3,8,8,10,10,5]");
    TRY(u << collect_groups(std::equal_to<>()));
    TEST_EQUAL(to_str(u), "[1,4,6,3,8,8,10,10,5]");

    u = {2,3,5,4,6,8,7,9,10};
    auto same_parity = [] (int a, int b) { return (a & 1) == (b & 1); };
    TRY(u >> collect_groups(same_parity, std::multiplies<>()) >> overwrite(v));
    TEST_EQUAL(to_str(v), "[2,15,192,63,10]");
    TRY(u << collect_groups(same_parity, std::multiplies<>()));
    TEST_EQUAL(to_str(u), "[2,15,192,63,10]");

    u = {2,3,5,4,6,8,7,9,10};
    auto parity = [] (int a) { return a & 1; };
    TRY(u >> collect_groups_by(parity, std::multiplies<>()) >> overwrite(v));
    TEST_EQUAL(to_str(v), "[2,15,192,63,10]");
    TRY(u << collect_groups_by(parity, std::multiplies<>()));
    TEST_EQUAL(to_str(u), "[2,15,192,63,10]");

}

void test_core_range_aggregation_group() {

    Ustring s1, s2;
    std::vector<Irange<Ustring::const_iterator>> iv;
    Strings sv;
    auto f = [] (char a, char b) { return (ascii_islower(a) && ascii_islower(b)) || (ascii_isupper(a) && ascii_isupper(b)); };

    auto make_sv = [&] () {
        sv.clear();
        for (auto i: iv)
            sv.push_back(Ustring(i.first, i.second));
    };

    TRY(s1 >> group >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[]");

    s1 = "abbcccddddeeeee";
    TRY(s1 >> group >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[a,bb,ccc,dddd,eeeee]");

    s1 = "Hello World";
    TRY(s1 >> group(f) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[H,ello, ,W,orld]");

    s1 = "";
    TRY(s1 >> group_by(ascii_isupper) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[]");

    s1 = "AbbcCcddddeeEee";
    TRY(s1 >> group_by(ascii_isupper) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[A,bbc,C,cddddee,E,ee]");

    s1 = "";
    TRY(s1 >> group_k(5) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[]");

    s1 = "abcdefghijklmnopqrstuvwxyz";
    TRY(s1 >> group_k(1) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z]");
    TRY(s1 >> group_k(2) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[ab,cd,ef,gh,ij,kl,mn,op,qr,st,uv,wx,yz]");
    TRY(s1 >> group_k(3) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[abc,def,ghi,jkl,mno,pqr,stu,vwx,yz]");
    TRY(s1 >> group_k(4) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[abcd,efgh,ijkl,mnop,qrst,uvwx,yz]");
    TRY(s1 >> group_k(5) >> overwrite(iv));
    make_sv();
    TRY(s2 = to_str(sv));
    TEST_EQUAL(s2, "[abcde,fghij,klmno,pqrst,uvwxy,z]");

    TRY(s1 >> (group_k(5)) >> overwrite(iv));              make_sv();  TEST_EQUAL(to_str(sv), "[abcde,fghij,klmno,pqrst,uvwxy,z]");
    TRY(s1 >> group_k(5) * passthrough >> overwrite(iv));  make_sv();  TEST_EQUAL(to_str(sv), "[abcde,fghij,klmno,pqrst,uvwxy,z]");

}

void test_core_range_aggregation_partial_sum() {

    Ustring s;
    std::vector<int> v1 = {1,2,3,4,5}, v2;
    auto f = [] (int a, int b) { return 2 * a + b; };

    TRY(v1 >> partial_sum >> overwrite(v2));
    TRY(s = to_str(v2));
    TEST_EQUAL(s, "[1,3,6,10,15]");
    TRY(v1 >> partial_sum(f) >> overwrite(v2));
    TRY(s = to_str(v2));
    TEST_EQUAL(s, "[1,4,11,26,57]");

    TRY(v1 >> passthrough >> partial_sum >> passthrough >> overwrite(v2));
    TRY(s = to_str(v2));
    TEST_EQUAL(s, "[1,3,6,10,15]");
    TRY(v1 >> passthrough >> partial_sum(f) >> passthrough >> overwrite(v2));
    TRY(s = to_str(v2));
    TEST_EQUAL(s, "[1,4,11,26,57]");

    v2 = v1;
    TRY(v2 << partial_sum);
    TRY(s = to_str(v2));
    TEST_EQUAL(s, "[1,3,6,10,15]");
    v2 = v1;
    TRY(v2 << partial_sum(f));
    TRY(s = to_str(v2));
    TEST_EQUAL(s, "[1,4,11,26,57]");

    TRY(v1 >> partial_sum >> overwrite(v2));                              TEST_EQUAL(to_str(v2), "[1,3,6,10,15]");
    TRY(v1 >> passthrough * partial_sum >> overwrite(v2));                TEST_EQUAL(to_str(v2), "[1,3,6,10,15]");
    TRY(v1 >> partial_sum * passthrough >> overwrite(v2));                TEST_EQUAL(to_str(v2), "[1,3,6,10,15]");
    TRY(v1 >> passthrough * partial_sum * passthrough >> overwrite(v2));  TEST_EQUAL(to_str(v2), "[1,3,6,10,15]");

}
