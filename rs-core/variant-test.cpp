#include "rs-core/variant.hpp"
#include "rs-core/unit-test.hpp"
#include <ostream>
#include <string>
#include <utility>

using namespace RS;
using namespace std::literals;

namespace {

    template <char C>
    class Thing:
    public LessThanComparable<Thing<C>> {
    public:
        Thing(): n(0) { count(1); }
        explicit Thing(int i): n(i) { count(1); }
        ~Thing() { count(-1); }
        Thing(const Thing& t): n(t.n) { count(1); }
        Thing(Thing&& t): n(t.n) { t.n = 0; count(1); }
        Thing& operator=(const Thing& t) { n = t.n; return *this; }
        Thing& operator=(Thing&& t) { n = t.n; t.n = 0; return *this; }
        int num() const { return n; }
        bool operator==(const Thing& rhs) const { return n == rhs.n; }
        bool operator<(const Thing& rhs) const { return n < rhs.n; }
        static int count(int add = 0) { static int c = 0; return c += add; }
        friend std::ostream& operator<<(std::ostream& o, const Thing& t) { return o << C << t.n; }
    private:
        int n;
    };

    using XA = Thing<'A'>;
    using XB = Thing<'B'>;
    using XC = Thing<'C'>;

    using VT = Variant<XA, XB, XC>;
    using WT = Variant<Nil, XA, XB, XC>;

    using V1 = Variant<int>;
    using V2 = Variant<int, float>;
    using V3 = Variant<int, float, std::string>;
    using W1 = Variant<Nil, int>;
    using W2 = Variant<Nil, int, float>;
    using W3 = Variant<Nil, int, float, std::string>;

}

void test_core_variant_types() {

    TEST(! V1::is_nullable);
    TEST(! V2::is_nullable);
    TEST(! V3::is_nullable);
    TEST(W1::is_nullable);
    TEST(W2::is_nullable);
    TEST(W3::is_nullable);

    {  using T = VariantTypeAt<V1, 0>;  TEST_TYPE(T, int);          }
    {  using T = VariantTypeAt<V1, 1>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<V2, 0>;  TEST_TYPE(T, int);          }
    {  using T = VariantTypeAt<V2, 1>;  TEST_TYPE(T, float);        }
    {  using T = VariantTypeAt<V2, 2>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<V3, 0>;  TEST_TYPE(T, int);          }
    {  using T = VariantTypeAt<V3, 1>;  TEST_TYPE(T, float);        }
    {  using T = VariantTypeAt<V3, 2>;  TEST_TYPE(T, std::string);  }
    {  using T = VariantTypeAt<V3, 3>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<W1, 0>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<W1, 1>;  TEST_TYPE(T, int);          }
    {  using T = VariantTypeAt<W1, 2>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<W2, 0>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<W2, 1>;  TEST_TYPE(T, int);          }
    {  using T = VariantTypeAt<W2, 2>;  TEST_TYPE(T, float);        }
    {  using T = VariantTypeAt<W2, 3>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<W3, 0>;  TEST_TYPE(T, Nil);          }
    {  using T = VariantTypeAt<W3, 1>;  TEST_TYPE(T, int);          }
    {  using T = VariantTypeAt<W3, 2>;  TEST_TYPE(T, float);        }
    {  using T = VariantTypeAt<W3, 3>;  TEST_TYPE(T, std::string);  }
    {  using T = VariantTypeAt<W3, 4>;  TEST_TYPE(T, Nil);          }

    TEST(is_variant<V1>);
    TEST(is_variant<V2>);
    TEST(is_variant<V3>);
    TEST(! is_variant<void>);
    TEST(! is_variant<int>);
    TEST(! is_variant<Nil>);
    TEST(! is_variant<XA>);

    TEST_EQUAL(variant_length<V1>, 1);
    TEST_EQUAL(variant_length<V2>, 2);
    TEST_EQUAL(variant_length<V3>, 3);
    TEST_EQUAL(variant_length<W1>, 2);
    TEST_EQUAL(variant_length<W2>, 3);
    TEST_EQUAL(variant_length<W3>, 4);

    TEST((variant_has_type<V1, int>));
    TEST((! variant_has_type<V1, float>));
    TEST((! variant_has_type<V1, std::string>));
    TEST((! variant_has_type<V1, void*>));
    TEST((variant_has_type<V2, int>));
    TEST((variant_has_type<V2, float>));
    TEST((! variant_has_type<V2, std::string>));
    TEST((! variant_has_type<V2, void*>));
    TEST((variant_has_type<V3, int>));
    TEST((variant_has_type<V3, float>));
    TEST((variant_has_type<V3, std::string>));
    TEST((! variant_has_type<V3, void*>));
    TEST((variant_has_type<W1, Nil>));
    TEST((variant_has_type<W1, int>));
    TEST((! variant_has_type<W1, float>));
    TEST((! variant_has_type<W1, std::string>));
    TEST((! variant_has_type<W1, void*>));
    TEST((variant_has_type<W2, Nil>));
    TEST((variant_has_type<W2, int>));
    TEST((variant_has_type<W2, float>));
    TEST((! variant_has_type<W2, std::string>));
    TEST((! variant_has_type<W2, void*>));
    TEST((variant_has_type<W3, Nil>));
    TEST((variant_has_type<W3, int>));
    TEST((variant_has_type<W3, float>));
    TEST((variant_has_type<W3, std::string>));
    TEST((! variant_has_type<W3, void*>));

    TEST_EQUAL((variant_index_of<V1, int>), 0);
    TEST_EQUAL((variant_index_of<V1, float>), -1);
    TEST_EQUAL((variant_index_of<V1, std::string>), -1);
    TEST_EQUAL((variant_index_of<V1, void*>), -1);
    TEST_EQUAL((variant_index_of<V2, int>), 0);
    TEST_EQUAL((variant_index_of<V2, float>), 1);
    TEST_EQUAL((variant_index_of<V2, std::string>), -1);
    TEST_EQUAL((variant_index_of<V2, void*>), -1);
    TEST_EQUAL((variant_index_of<V3, int>), 0);
    TEST_EQUAL((variant_index_of<V3, float>), 1);
    TEST_EQUAL((variant_index_of<V3, std::string>), 2);
    TEST_EQUAL((variant_index_of<V3, void*>), -1);
    TEST_EQUAL((variant_index_of<W1, Nil>), 0);
    TEST_EQUAL((variant_index_of<W1, int>), 1);
    TEST_EQUAL((variant_index_of<W1, float>), -1);
    TEST_EQUAL((variant_index_of<W1, std::string>), -1);
    TEST_EQUAL((variant_index_of<W1, void*>), -1);
    TEST_EQUAL((variant_index_of<W2, Nil>), 0);
    TEST_EQUAL((variant_index_of<W2, int>), 1);
    TEST_EQUAL((variant_index_of<W2, float>), 2);
    TEST_EQUAL((variant_index_of<W2, std::string>), -1);
    TEST_EQUAL((variant_index_of<W2, void*>), -1);
    TEST_EQUAL((variant_index_of<W3, Nil>), 0);
    TEST_EQUAL((variant_index_of<W3, int>), 1);
    TEST_EQUAL((variant_index_of<W3, float>), 2);
    TEST_EQUAL((variant_index_of<W3, std::string>), 3);
    TEST_EQUAL((variant_index_of<W3, void*>), -1);

    V1 v1;
    V2 v2;
    V3 v3;
    W1 w1;
    W2 w2;
    W3 w3;

    TEST_EQUAL(v1.index(), 0);
    TEST(! v1.empty());
    TEST(v1.is<int>());
    TEST(! v1.is<float>());
    TEST(! v1.is<void*>());

    TEST_EQUAL(v2.index(), 0);
    TEST(! v2.empty());
    TEST(v2.is<int>());
    TEST(! v2.is<float>());
    TEST(! v2.is<void*>());

    TEST_EQUAL(v3.index(), 0);
    TEST(! v3.empty());
    TEST(v3.is<int>());
    TEST(! v3.is<float>());
    TEST(! v3.is<void*>());

    TEST_EQUAL(w1.index(), 0);
    TEST(w1.empty());
    TEST(w1.is<Nil>());
    TEST(! w1.is<int>());
    TEST(! w1.is<float>());
    TEST(! w1.is<void*>());

    TEST_EQUAL(w2.index(), 0);
    TEST(w2.empty());
    TEST(w2.is<Nil>());
    TEST(! w2.is<int>());
    TEST(! w2.is<float>());
    TEST(! w2.is<void*>());

    TEST_EQUAL(w3.index(), 0);
    TEST(w3.empty());
    TEST(w3.is<Nil>());
    TEST(! w3.is<int>());
    TEST(! w3.is<float>());
    TEST(! w3.is<void*>());

}

void test_core_variant_behaviour() {

    TEST_EQUAL(XA::count(), 0);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 0);

    VT v1;
    WT w1;

    TEST_EQUAL(XA::count(), 1);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 0);

    TEST_EQUAL(v1.index(), 0);
    TEST_EQUAL(w1.index(), 0);

    TEST_EQUAL(v1.as<XA>().num(), 0);
    TEST_THROW(v1.as<XB>(), VariantError);
    TEST_THROW(v1.as<XC>(), VariantError);

    TEST_THROW(w1.as<XA>(), VariantError);
    TEST_THROW(w1.as<XB>(), VariantError);
    TEST_THROW(w1.as<XC>(), VariantError);

    TEST_EQUAL(v1.at<0>().num(), 0);
    TEST_THROW(v1.at<1>(), VariantError);
    TEST_THROW(v1.at<2>(), VariantError);

    TRY(w1.at<0>());
    TEST_THROW(w1.at<1>(), VariantError);
    TEST_THROW(w1.at<2>(), VariantError);
    TEST_THROW(w1.at<3>(), VariantError);

    TEST_EQUAL(v1.get_as<XA>(XA(-1)).num(), 0);
    TEST_EQUAL(v1.get_as<XB>(XB(-1)).num(), -1);
    TEST_EQUAL(v1.get_as<XC>(XC(-1)).num(), -1);

    TEST_EQUAL(w1.get_as<XA>(XA(-1)).num(), -1);
    TEST_EQUAL(w1.get_as<XB>(XB(-1)).num(), -1);
    TEST_EQUAL(w1.get_as<XC>(XC(-1)).num(), -1);

    TEST_EQUAL(v1.get_at<0>(XA(-1)).num(), 0);
    TEST_EQUAL(v1.get_at<1>(XB(-1)).num(), -1);
    TEST_EQUAL(v1.get_at<2>(XC(-1)).num(), -1);

    TEST_EQUAL(w1.get_at<1>(XA(-1)).num(), -1);
    TEST_EQUAL(w1.get_at<2>(XB(-1)).num(), -1);
    TEST_EQUAL(w1.get_at<3>(XC(-1)).num(), -1);

    std::string s;

    TRY(s = to_str(v1));  TEST_EQUAL(s, "A0");
    TRY(s = to_str(w1));  TEST_EQUAL(s, "nil");

    TRY(v1 = XA(10));
    TEST(! v1.empty());
    TEST_EQUAL(v1.index(), 0);
    TEST_EQUAL(v1.as<XA>().num(), 10);
    TEST_THROW(v1.as<XB>(), VariantError);
    TEST_THROW(v1.as<XC>(), VariantError);
    TEST_EQUAL(v1.at<0>().num(), 10);
    TEST_THROW(v1.at<1>(), VariantError);
    TEST_THROW(v1.at<2>(), VariantError);
    TEST_EQUAL(XA::count(), 1);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 0);
    TRY(s = to_str(v1));
    TEST_EQUAL(s, "A10");

    TRY(v1 = XB(20));
    TEST(! v1.empty());
    TEST_EQUAL(v1.index(), 1);
    TEST_THROW(v1.as<XA>(), VariantError);
    TEST_EQUAL(v1.as<XB>().num(), 20);
    TEST_THROW(v1.as<XC>(), VariantError);
    TEST_THROW(v1.at<0>(), VariantError);
    TEST_EQUAL(v1.at<1>().num(), 20);
    TEST_THROW(v1.at<2>(), VariantError);
    TEST_EQUAL(XA::count(), 0);
    TEST_EQUAL(XB::count(), 1);
    TEST_EQUAL(XC::count(), 0);
    TRY(s = to_str(v1));
    TEST_EQUAL(s, "B20");

    TRY(v1 = XC(30));
    TEST(! v1.empty());
    TEST_EQUAL(v1.index(), 2);
    TEST_THROW(v1.as<XA>(), VariantError);
    TEST_THROW(v1.as<XB>(), VariantError);
    TEST_EQUAL(v1.as<XC>().num(), 30);
    TEST_THROW(v1.at<0>(), VariantError);
    TEST_THROW(v1.at<1>(), VariantError);
    TEST_EQUAL(v1.at<2>().num(), 30);
    TEST_EQUAL(XA::count(), 0);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 1);
    TRY(s = to_str(v1));
    TEST_EQUAL(s, "C30");

    TRY(v1.reset());
    TEST(! v1.empty());
    TEST_EQUAL(v1.index(), 0);
    TEST_EQUAL(v1.as<XA>().num(), 0);
    TEST_THROW(v1.as<XB>(), VariantError);
    TEST_THROW(v1.as<XC>(), VariantError);
    TEST_EQUAL(v1.at<0>().num(), 0);
    TEST_THROW(v1.at<1>(), VariantError);
    TEST_THROW(v1.at<2>(), VariantError);
    TEST_EQUAL(XA::count(), 1);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 0);
    TRY(s = to_str(v1));
    TEST_EQUAL(s, "A0");

    TRY(w1 = XA(10));
    TEST(! w1.empty());
    TEST_EQUAL(w1.index(), 1);
    TEST_EQUAL(w1.as<XA>().num(), 10);
    TEST_THROW(w1.as<XB>(), VariantError);
    TEST_THROW(w1.as<XC>(), VariantError);
    TEST_THROW(w1.at<0>(), VariantError);
    TEST_EQUAL(w1.at<1>().num(), 10);
    TEST_THROW(w1.at<2>(), VariantError);
    TEST_THROW(w1.at<3>(), VariantError);
    TEST_EQUAL(XA::count(), 2);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 0);
    TRY(s = to_str(w1));
    TEST_EQUAL(s, "A10");

    TRY(w1 = XB(20));
    TEST(! w1.empty());
    TEST_EQUAL(w1.index(), 2);
    TEST_THROW(w1.as<XA>(), VariantError);
    TEST_EQUAL(w1.as<XB>().num(), 20);
    TEST_THROW(w1.as<XC>(), VariantError);
    TEST_THROW(w1.at<0>(), VariantError);
    TEST_THROW(w1.at<1>(), VariantError);
    TEST_EQUAL(w1.at<2>().num(), 20);
    TEST_THROW(w1.at<3>(), VariantError);
    TEST_EQUAL(XA::count(), 1);
    TEST_EQUAL(XB::count(), 1);
    TEST_EQUAL(XC::count(), 0);
    TRY(s = to_str(w1));
    TEST_EQUAL(s, "B20");

    TRY(w1 = XC(30));
    TEST(! w1.empty());
    TEST_EQUAL(w1.index(), 3);
    TEST_THROW(w1.as<XA>(), VariantError);
    TEST_THROW(w1.as<XB>(), VariantError);
    TEST_EQUAL(w1.as<XC>().num(), 30);
    TEST_THROW(w1.at<0>(), VariantError);
    TEST_THROW(w1.at<1>(), VariantError);
    TEST_THROW(w1.at<2>(), VariantError);
    TEST_EQUAL(w1.at<3>().num(), 30);
    TEST_EQUAL(XA::count(), 1);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 1);
    TRY(s = to_str(w1));
    TEST_EQUAL(s, "C30");

    TRY(w1.reset());
    TEST(w1.empty());
    TEST_EQUAL(w1.index(), 0);
    TEST_THROW(w1.as<XA>(), VariantError);
    TEST_THROW(w1.as<XB>(), VariantError);
    TEST_THROW(w1.as<XC>(), VariantError);
    TEST_THROW(w1.at<1>(), VariantError);
    TEST_THROW(w1.at<2>(), VariantError);
    TEST_THROW(w1.at<3>(), VariantError);
    TEST_EQUAL(XA::count(), 1);
    TEST_EQUAL(XB::count(), 0);
    TEST_EQUAL(XC::count(), 0);
    TRY(s = to_str(w1));
    TEST_EQUAL(s, "nil");

    VT v2;
    WT w2;

    TRY(v1 = XA(10));
    TRY(v2 = XA(11));
    TEST_EQUAL(v1.index(), 0);
    TEST_EQUAL(v2.index(), 0);
    TEST_EQUAL(XA::count(), 2);

    TRY(v1 = v2);
    TEST_EQUAL(v1.index(), 0);
    TEST_EQUAL(v2.index(), 0);
    TEST_EQUAL(v1.as<XA>().num(), 11);
    TEST_EQUAL(v2.as<XA>().num(), 11);
    TEST_EQUAL(XA::count(), 2);

    TRY(v2 = XA(12));
    TRY(v1 = std::move(v2));
    TEST_EQUAL(v1.index(), 0);
    TEST_EQUAL(v2.index(), 0);
    TEST_EQUAL(v1.as<XA>().num(), 12);
    TEST_EQUAL(v2.as<XA>().num(), 0);
    TEST_EQUAL(XA::count(), 2);

    TRY(v2 = XB(20));
    TRY(v1 = std::move(v2));
    TEST_EQUAL(v1.index(), 1);
    TEST_EQUAL(v2.index(), 1);
    TEST_EQUAL(v1.as<XB>().num(), 20);
    TEST_EQUAL(v2.as<XB>().num(), 0);
    TEST_EQUAL(XA::count(), 0);
    TEST_EQUAL(XB::count(), 2);

    TRY(w1 = XA(10));
    TRY(w2 = XA(11));
    TEST_EQUAL(w1.index(), 1);
    TEST_EQUAL(w2.index(), 1);
    TEST_EQUAL(XA::count(), 2);
    TEST_EQUAL(XB::count(), 2);

    TRY(w1 = w2);
    TEST_EQUAL(w1.index(), 1);
    TEST_EQUAL(w2.index(), 1);
    TEST_EQUAL(w1.as<XA>().num(), 11);
    TEST_EQUAL(w2.as<XA>().num(), 11);
    TEST_EQUAL(XA::count(), 2);
    TEST_EQUAL(XB::count(), 2);

    TRY(w2 = XA(12));
    TRY(w1 = std::move(w2));
    TEST_EQUAL(w1.index(), 1);
    TEST_EQUAL(w2.index(), 0);
    TEST_EQUAL(w1.as<XA>().num(), 12);
    TEST_EQUAL(XA::count(), 1);
    TEST_EQUAL(XB::count(), 2);

    TRY(w2 = XB(20));
    TRY(w1 = std::move(w2));
    TEST_EQUAL(w1.index(), 2);
    TEST_EQUAL(w2.index(), 0);
    TEST_EQUAL(w1.as<XB>().num(), 20);
    TEST_EQUAL(XA::count(), 0);
    TEST_EQUAL(XB::count(), 3);

    TRY(w2.reset());
    TRY(w1 = std::move(w2));
    TEST_EQUAL(w1.index(), 0);
    TEST_EQUAL(w2.index(), 0);
    TEST_EQUAL(XA::count(), 0);
    TEST_EQUAL(XB::count(), 2);

    V3 v3;
    W3 w3;

    TEST_EQUAL(v3.index(), 0);  TEST(! v3);  TEST_EQUAL(v3.as<int>(), 0);
    TEST_EQUAL(w3.index(), 0);  TEST(! w3);

    TRY(v3 = 42);  TEST_EQUAL(v3.index(), 0);  TEST(v3);  TEST_EQUAL(v3.as<int>(), 42);
    TRY(w3 = 42);  TEST_EQUAL(w3.index(), 1);  TEST(w3);  TEST_EQUAL(w3.as<int>(), 42);

    TRY(v3 = 1234.5f);  TEST_EQUAL(v3.index(), 1);  TEST(v3);  TEST_EQUAL(v3.as<float>(), 1234.5f);
    TRY(w3 = 1234.5f);  TEST_EQUAL(w3.index(), 2);  TEST(w3);  TEST_EQUAL(w3.as<float>(), 1234.5f);

    TRY(v3 = "Hello"s);  TEST_EQUAL(v3.index(), 2);  TEST(v3);  TEST_EQUAL(v3.as<std::string>(), "Hello");
    TRY(w3 = "Hello"s);  TEST_EQUAL(w3.index(), 3);  TEST(w3);  TEST_EQUAL(w3.as<std::string>(), "Hello");

    TRY(v3 = 0);  TEST_EQUAL(v3.index(), 0);  TEST(! v3);  TEST_EQUAL(v3.as<int>(), 0);
    TRY(w3 = 0);  TEST_EQUAL(w3.index(), 1);  TEST(! w3);  TEST_EQUAL(w3.as<int>(), 0);

    TRY(v3 = 0.0f);  TEST_EQUAL(v3.index(), 1);  TEST(! v3);  TEST_EQUAL(v3.as<float>(), 0);
    TRY(w3 = 0.0f);  TEST_EQUAL(w3.index(), 2);  TEST(! w3);  TEST_EQUAL(w3.as<float>(), 0);

    TRY(v3 = {});  TEST_EQUAL(v3.index(), 0);  TEST(! v3);  TEST_EQUAL(v3.as<int>(), 0);
    TRY(w3 = {});  TEST_EQUAL(w3.index(), 0);  TEST(! w3);

}

void test_core_variant_comparison() {

    VT v1{XA(10)}, v2{XA(20)}, v3{XB(5)}, v4{XB(5)};
    WT w0, w1{XA(10)}, w2{XA(20)}, w3{XB(5)}, w4{XB(5)};

    TEST_COMPARE(v1, <, v2);   TEST_COMPARE(v1, <=, v2);  TEST_COMPARE(v1, !=, v2);
    TEST_COMPARE(v2, <, v3);   TEST_COMPARE(v2, <=, v3);  TEST_COMPARE(v2, !=, v3);
    TEST_COMPARE(v3, ==, v4);  TEST_COMPARE(v3, <=, v4);  TEST_COMPARE(v3, >=, v4);

    TEST_COMPARE(v2, >, v1);   TEST_COMPARE(v2, >=, v1);  TEST_COMPARE(v2, !=, v1);
    TEST_COMPARE(v3, >, v2);   TEST_COMPARE(v3, >=, v2);  TEST_COMPARE(v3, !=, v2);
    TEST_COMPARE(v4, ==, v3);  TEST_COMPARE(v4, <=, v3);  TEST_COMPARE(v4, >=, v3);

    TEST_COMPARE(w0, <, w1);   TEST_COMPARE(w0, <=, w1);  TEST_COMPARE(w0, !=, w1);
    TEST_COMPARE(w1, <, w2);   TEST_COMPARE(w1, <=, w2);  TEST_COMPARE(w1, !=, w2);
    TEST_COMPARE(w2, <, w3);   TEST_COMPARE(w2, <=, w3);  TEST_COMPARE(w2, !=, w3);
    TEST_COMPARE(w3, ==, w4);  TEST_COMPARE(w3, <=, w4);  TEST_COMPARE(w3, >=, w4);

    TEST_COMPARE(w1, >, w0);   TEST_COMPARE(w1, >=, w0);  TEST_COMPARE(w1, !=, w0);
    TEST_COMPARE(w2, >, w1);   TEST_COMPARE(w2, >=, w1);  TEST_COMPARE(w2, !=, w1);
    TEST_COMPARE(w3, >, w2);   TEST_COMPARE(w3, >=, w2);  TEST_COMPARE(w3, !=, w2);
    TEST_COMPARE(w4, ==, w3);  TEST_COMPARE(w4, <=, w3);  TEST_COMPARE(w4, >=, w3);

    TEST_COMPARE(v1, >, Nil());    TEST_COMPARE(v1, >=, Nil());   TEST_COMPARE(v1, !=, Nil());
    TEST_COMPARE(v1, ==, XA(10));  TEST_COMPARE(v1, <=, XA(10));  TEST_COMPARE(v1, >=, XA(10));
    TEST_COMPARE(v1, <, XA(20));   TEST_COMPARE(v1, <=, XA(20));  TEST_COMPARE(v1, !=, XA(20));
    TEST_COMPARE(v1, <, XB(5));    TEST_COMPARE(v1, <=, XB(5));   TEST_COMPARE(v1, !=, XB(5));

    TEST_COMPARE(Nil(), <, v1);    TEST_COMPARE(Nil(), <=, v1);   TEST_COMPARE(Nil(), !=, v1);
    TEST_COMPARE(XA(10), ==, v1);  TEST_COMPARE(XA(10), <=, v1);  TEST_COMPARE(XA(10), >=, v1);
    TEST_COMPARE(XA(20), >, v1);   TEST_COMPARE(XA(20), >=, v1);  TEST_COMPARE(XA(20), !=, v1);
    TEST_COMPARE(XB(5), >, v1);    TEST_COMPARE(XB(5), >=, v1);   TEST_COMPARE(XB(5), !=, v1);

    TEST_COMPARE(w0, ==, Nil());  TEST_COMPARE(w0, <=, Nil());   TEST_COMPARE(w0, >=, Nil());
    TEST_COMPARE(w0, <, XA(10));  TEST_COMPARE(w0, <=, XA(10));  TEST_COMPARE(w0, !=, XA(10));

    TEST_COMPARE(Nil(), ==, w0);  TEST_COMPARE(Nil(), <=, w0);   TEST_COMPARE(Nil(), >=, w0);
    TEST_COMPARE(XA(10), >, w0);  TEST_COMPARE(XA(10), >=, w0);  TEST_COMPARE(XA(10), !=, w0);

    TEST_COMPARE(w1, >, Nil());    TEST_COMPARE(w1, >=, Nil());   TEST_COMPARE(w1, !=, Nil());
    TEST_COMPARE(w1, ==, XA(10));  TEST_COMPARE(w1, <=, XA(10));  TEST_COMPARE(w1, >=, XA(10));
    TEST_COMPARE(w1, <, XA(20));   TEST_COMPARE(w1, <=, XA(20));  TEST_COMPARE(w1, !=, XA(20));
    TEST_COMPARE(w1, <, XB(5));    TEST_COMPARE(w1, <=, XB(5));   TEST_COMPARE(w1, !=, XB(5));

    TEST_COMPARE(Nil(), <, w1);    TEST_COMPARE(Nil(), <=, w1);   TEST_COMPARE(Nil(), !=, w1);
    TEST_COMPARE(XA(10), ==, w1);  TEST_COMPARE(XA(10), <=, w1);  TEST_COMPARE(XA(10), >=, w1);
    TEST_COMPARE(XA(20), >, w1);   TEST_COMPARE(XA(20), >=, w1);  TEST_COMPARE(XA(20), !=, w1);
    TEST_COMPARE(XB(5), >, w1);    TEST_COMPARE(XB(5), >=, w1);   TEST_COMPARE(XB(5), !=, w1);

}
