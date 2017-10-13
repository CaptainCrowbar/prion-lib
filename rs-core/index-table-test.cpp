#include "rs-core/index-table.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <iterator>
#include <memory>
#include <ostream>
#include <string>

using namespace RS;

namespace {

    class Neddie:
    public EqualityComparable<Neddie> {
    public:
        Neddie(): n(0), s() {}
        Neddie(int num, const std::string& str): n(num), s(str) {}
        int num() const { return n; }
        std::string str() const { return s; }
        bool operator==(const Neddie& rhs) const { return n == rhs.n && s == rhs.s; }
    private:
        int n;
        std::string s;
    };

    std::ostream& operator<<(std::ostream& out, const Neddie& x) { return out << x.num() << ":" << x.str(); }

}

void test_core_index_table_classes() {

    using table_type = IndexTable<Neddie>;
    using int_index = Index<int, Neddie, IndexMode::duplicate>;
    using string_index = Index<std::string, Neddie>;

    table_type t1, t2;
    const table_type& ct1(t1);
    std::unique_ptr<int_index> iip;
    std::unique_ptr<string_index> sip;
    table_type::iterator x1, x2;
    int_index::iterator y1, y2;
    int_index::const_iterator cy1, cy2;
    string_index::iterator z1, z2;
    string_index::const_iterator cz1, cz2;

    TEST(t1.empty());
    TEST_EQUAL(t1.size(), 0);
    TRY(iip = std::make_unique<int_index>(t1, &Neddie::num));
    TRY(sip = std::make_unique<string_index>(t1, &Neddie::str, [] (std::string a, std::string b) { return a > b; }));

    int_index& ii = *iip;
    string_index& si = *sip;
    const int_index& cii = *iip;
    const string_index& csi = *sip;

    TEST(! cii.compare()(2, 1));
    TEST(! cii.compare()(2, 2));
    TEST(cii.compare()(2, 3));
    TEST_EQUAL(cii.extract()(Neddie(1, "zulu")), 1);
    TEST(! csi.compare()("yankee", "zulu"));
    TEST(! csi.compare()("yankee", "yankee"));
    TEST(csi.compare()("yankee", "xray"));
    TEST_EQUAL(csi.extract()(Neddie(1, "zulu")), "zulu");
    TEST(ii.empty());
    TEST(si.empty());
    TEST_EQUAL(ii.size(), 0);
    TEST_EQUAL(si.size(), 0);

    void* a0 = &t1;
    void* a1 = nullptr;
    const void* a2 = nullptr;
    void* a3 = nullptr;
    const void* a4 = nullptr;

    TRY(a1 = &ii.table());
    TRY(a2 = &cii.table());
    TRY(a3 = &si.table());
    TRY(a4 = &csi.table());
    TEST_EQUAL(a1, a0);
    TEST_EQUAL(a2, a0);
    TEST_EQUAL(a3, a0);
    TEST_EQUAL(a4, a0);

    TRY(t1.insert(Neddie(1, "alpha")));
    TRY(t1.insert(Neddie(2, "bravo")));
    TRY(t1.insert(Neddie(3, "charlie")));
    TRY(t1.insert(Neddie(4, "delta")));
    TRY(t1.insert(Neddie(5, "echo")));
    TEST(! t1.empty());
    TEST(! cii.empty());
    TEST(! csi.empty());
    TEST_EQUAL(t1.size(), 5);
    TEST_EQUAL(cii.size(), 5);
    TEST_EQUAL(csi.size(), 5);
    TEST_EQUAL(cii.count(1), 1);
    TEST_EQUAL(cii.count(10), 0);
    TEST_EQUAL(csi.count("alpha"), 1);
    TEST_EQUAL(csi.count("zulu"), 0);
    TEST_EQUAL(*cii.begin(), Neddie(1, "alpha"));
    TEST_EQUAL(*csi.begin(), Neddie(5, "echo"));
    TEST_EQUAL(cii.begin().key(), 1);
    TEST_EQUAL(csi.begin().key(), "echo");
    TEST_EQUAL(ii.begin().key(), 1);
    TEST_EQUAL(si.begin().key(), "echo");

    TRY(z1 = si.find("echo"));
    TEST(z1 == si.begin());
    TEST(z1 != si.end());
    TEST_EQUAL(*z1, Neddie(5, "echo"));
    TEST_EQUAL(z1.key(), "echo");
    TRY(z1 = si.find("zulu"));
    TEST(z1 == si.end());
    TRY(cz1 = csi.find("echo"));
    TEST(cz1 == csi.begin());
    TEST(cz1 != csi.end());
    TEST_EQUAL(*cz1, Neddie(5, "echo"));
    TEST_EQUAL(cz1.key(), "echo");
    TRY(cz1 = csi.find("zulu"));
    TEST(cz1 == csi.end());

    TEST_EQUAL(to_str(t1), "[1:alpha,2:bravo,3:charlie,4:delta,5:echo]");
    TEST_EQUAL(to_str(ct1), "[1:alpha,2:bravo,3:charlie,4:delta,5:echo]");
    TEST_EQUAL(to_str(ii), "[1:alpha,2:bravo,3:charlie,4:delta,5:echo]");
    TEST_EQUAL(to_str(cii), "[1:alpha,2:bravo,3:charlie,4:delta,5:echo]");
    TEST_EQUAL(to_str(si), "[5:echo,4:delta,3:charlie,2:bravo,1:alpha]");
    TEST_EQUAL(to_str(csi), "[5:echo,4:delta,3:charlie,2:bravo,1:alpha]");

    TRY(t1.erase(t1.begin()));
    TEST_EQUAL(t1.size(), 4);
    TEST_EQUAL(cii.size(), 4);
    TEST_EQUAL(csi.size(), 4);
    TEST_EQUAL(to_str(ct1), "[2:bravo,3:charlie,4:delta,5:echo]");
    TEST_EQUAL(to_str(cii), "[2:bravo,3:charlie,4:delta,5:echo]");
    TEST_EQUAL(to_str(csi), "[5:echo,4:delta,3:charlie,2:bravo]");

    TRY(si.erase(si.begin()));
    TEST_EQUAL(t1.size(), 3);
    TEST_EQUAL(cii.size(), 3);
    TEST_EQUAL(csi.size(), 3);
    TEST_EQUAL(to_str(ct1), "[2:bravo,3:charlie,4:delta]");
    TEST_EQUAL(to_str(cii), "[2:bravo,3:charlie,4:delta]");
    TEST_EQUAL(to_str(csi), "[4:delta,3:charlie,2:bravo]");

    TRY(ii.erase(3));
    TEST_EQUAL(t1.size(), 2);
    TEST_EQUAL(cii.size(), 2);
    TEST_EQUAL(csi.size(), 2);
    TEST_EQUAL(to_str(ct1), "[2:bravo,4:delta]");
    TEST_EQUAL(to_str(cii), "[2:bravo,4:delta]");
    TEST_EQUAL(to_str(csi), "[4:delta,2:bravo]");

    TRY(t1.insert(Neddie(2, "zulu")));
    TEST_THROW(t1.insert(Neddie(1, "bravo")), IndexCollision);

    TRY(t1.clear());
    TEST(t1.empty());
    TEST(cii.empty());
    TEST(csi.empty());
    TEST_EQUAL(t1.size(), 0);
    TEST_EQUAL(cii.size(), 0);
    TEST_EQUAL(csi.size(), 0);

    TRY(t1.push_back(Neddie(1, "alpha")));
    TRY(t1.push_back(Neddie(2, "bravo")));
    TRY(t1.push_back(Neddie(3, "charlie")));
    TRY(t1.push_back(Neddie(4, "delta")));
    TRY(t1.push_back(Neddie(5, "echo")));
    TRY(t1.push_back(Neddie(6, "foxtrot")));
    TRY(t1.push_back(Neddie(7, "golf")));
    TRY(t1.push_back(Neddie(8, "hotel")));
    TRY(t1.push_back(Neddie(9, "india")));
    TRY(t1.push_back(Neddie(10, "juliet")));
    TEST_EQUAL(t1.size(), 10);
    TEST_EQUAL(cii.size(), 10);
    TEST_EQUAL(csi.size(), 10);

    TRY(x1 = t1.begin());
    TRY(x2 = t1.begin());
    TRY(std::advance(x1, 1));
    TRY(std::advance(x2, 4));
    TRY(t1.erase(x1, x2));
    TEST_EQUAL(t1.size(), 7);
    TEST_EQUAL(cii.size(), 7);
    TEST_EQUAL(csi.size(), 7);
    TEST_EQUAL(to_str(ct1), "[1:alpha,5:echo,6:foxtrot,7:golf,8:hotel,9:india,10:juliet]");

    TRY(y1 = ii.begin());
    TRY(y2 = ii.begin());
    TRY(std::advance(y1, 1));
    TRY(std::advance(y2, 3));
    TRY(ii.erase(y1, y2));
    TEST_EQUAL(t1.size(), 5);
    TEST_EQUAL(cii.size(), 5);
    TEST_EQUAL(csi.size(), 5);
    TEST_EQUAL(to_str(ct1), "[1:alpha,7:golf,8:hotel,9:india,10:juliet]");

    TRY(z1 = si.begin());
    TRY(z2 = si.begin());
    TRY(std::advance(z1, 1));
    TRY(std::advance(z2, 3));
    TRY(si.erase(z1, z2));
    TEST_EQUAL(t1.size(), 3);
    TEST_EQUAL(cii.size(), 3);
    TEST_EQUAL(csi.size(), 3);
    TEST_EQUAL(to_str(ct1), "[1:alpha,7:golf,10:juliet]");

    TRY(t1.clear());
    TRY(t1.insert(Neddie(1, "zulu")));
    TRY(t1.insert(Neddie(1, "yankee")));
    TRY(t1.insert(Neddie(2, "xray")));
    TRY(t1.insert(Neddie(2, "whiskey")));
    TRY(t1.insert(Neddie(99, "victor")));
    TRY(t1.insert(Neddie(99, "uniform")));
    TEST_EQUAL(t1.size(), 6);
    TEST_EQUAL(cii.size(), 6);
    TEST_EQUAL(csi.size(), 6);

    TRY(y1 = ii.lower_bound(2));               TEST_EQUAL(std::distance(ii.begin(), y1), 2);
    TRY(y1 = ii.upper_bound(2));               TEST_EQUAL(std::distance(ii.begin(), y1), 4);
    TRY(y1 = ii.equal_range(2).first);         TEST_EQUAL(std::distance(ii.begin(), y1), 2);
    TRY(y1 = ii.equal_range(2).second);        TEST_EQUAL(std::distance(ii.begin(), y1), 4);
    TRY(cy1 = cii.lower_bound(20));            TEST_EQUAL(std::distance(cii.begin(), cy1), 4);
    TRY(cy1 = cii.upper_bound(20));            TEST_EQUAL(std::distance(cii.begin(), cy1), 4);
    TRY(cy1 = cii.equal_range(20).first);      TEST_EQUAL(std::distance(cii.begin(), cy1), 4);
    TRY(cy1 = cii.equal_range(20).second);     TEST_EQUAL(std::distance(cii.begin(), cy1), 4);
    TRY(z1 = si.lower_bound("xray"));          TEST_EQUAL(std::distance(si.begin(), z1), 2);
    TRY(z1 = si.upper_bound("xray"));          TEST_EQUAL(std::distance(si.begin(), z1), 3);
    TRY(z1 = si.equal_range("xray").first);    TEST_EQUAL(std::distance(si.begin(), z1), 2);
    TRY(z1 = si.equal_range("xray").second);   TEST_EQUAL(std::distance(si.begin(), z1), 3);
    TRY(cz1 = csi.lower_bound("xyz"));         TEST_EQUAL(std::distance(csi.begin(), cz1), 2);
    TRY(cz1 = csi.upper_bound("xyz"));         TEST_EQUAL(std::distance(csi.begin(), cz1), 2);
    TRY(cz1 = csi.equal_range("xyz").first);   TEST_EQUAL(std::distance(csi.begin(), cz1), 2);
    TRY(cz1 = csi.equal_range("xyz").second);  TEST_EQUAL(std::distance(csi.begin(), cz1), 2);

    TRY(t2.insert(Neddie(1, "zulu")));
    TRY(t2.insert(Neddie(1, "zulu")));
    TEST_EQUAL(t2.size(), 2);
    TRY(int_index(t2, &Neddie::num));
    TEST_THROW(string_index(t2, &Neddie::str), IndexCollision);

}
