#include "rs-core/serial.hpp"
#include "rs-core/array-map.hpp"
#include "rs-core/auto-array.hpp"
#include "rs-core/blob.hpp"
#include "rs-core/bounded-array.hpp"
#include "rs-core/compact-array.hpp"
#include "rs-core/file-system.hpp"
#include "rs-core/grid.hpp"
#include "rs-core/mirror-map.hpp"
#include "rs-core/mp-integer.hpp"
#include "rs-core/optional.hpp"
#include "rs-core/ordered-map.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/string.hpp"
#include "rs-core/terminal.hpp"
#include "rs-core/unit-test.hpp"
#include "rs-core/uuid.hpp"
#include "rs-core/vector.hpp"
#include "unicorn/path.hpp"
#include <chrono>
#include <map>
#include <string>
#include <thread>
#include <vector>

using namespace RS;
using namespace RS::Literals;
using namespace RS::Unicorn;
using namespace std::chrono;
using namespace std::literals;

void test_core_serial_standard_types() {

    static const int ci = 42;
    static const float cf = 1234.5;
    static const std::string cs = "Hello world\nGoodbye\n";
    static const std::vector<std::string> cv = {"Alpha", "Bravo", "Charlie"};
    static const std::map<std::string, int> cm = {{"Alpha", 1}, {"Bravo", 2}, {"Charlie", 3}};

    json j;
    int i = 0;
    float f = 0;
    std::string s = {};
    std::vector<std::string> v = {};
    std::map<std::string, int> m = {};

    TRY(j = ci);
    TEST_EQUAL(j.dump(), "42");
    TRY(i = j.get<int>());
    TEST_EQUAL(i, ci);

    TRY(j = cf);
    TEST_EQUAL(j.dump(), "1234.5");
    TRY(f = j.get<float>());
    TEST_EQUAL(f, cf);

    TRY(j = cs);
    TEST_EQUAL(j.dump(), "\"Hello world\\nGoodbye\\n\"");
    TRY(s = j.get<std::string>());
    TEST_EQUAL(s, cs);

    TRY(j = cv);
    TEST_EQUAL(j.dump(), "[\"Alpha\",\"Bravo\",\"Charlie\"]");
    TRY(v = j.get<std::vector<std::string>>());
    TEST(v == cv);

    TRY(j = cm);
    TEST_EQUAL(j.dump(), "{\"Alpha\":1,\"Bravo\":2,\"Charlie\":3}");
    TRY((m = j.get<std::map<std::string, int>>()));
    TEST(m == cm);

}

void test_core_serial_std_chrono() {

    static const auto sys_epoch = system_clock::time_point();
    static const auto sys_now = system_clock::now();
    static const auto sys_epsilon = duration_cast<system_clock::duration>(1ms).count();
    static const auto hrc_epoch = high_resolution_clock::time_point();
    static const auto hrc_now = high_resolution_clock::now();
    static const auto hrc_epsilon = duration_cast<high_resolution_clock::duration>(1ms).count();

    json j;
    microseconds us;
    milliseconds ms;
    seconds s;
    minutes min;
    hours h;
    system_clock::time_point sys;
    high_resolution_clock::time_point hrc;

    TRY(j = 42us);         TRY(us = j);   TEST_EQUAL(us.count(), 42);
    TRY(j = 42ms);         TRY(ms = j);   TEST_EQUAL(ms.count(), 42);
    TRY(j = 42s);          TRY(s = j);    TEST_EQUAL(s.count(), 42);
    TRY(j = 42min);        TRY(min = j);  TEST_EQUAL(min.count(), 42);
    TRY(j = 42h);          TRY(h = j);    TEST_EQUAL(h.count(), 42);
    /**/                   TRY(min = j);  TEST_EQUAL(min.count(), 2520);
    /**/                   TRY(s = j);    TEST_EQUAL(s.count(), 151'200);
    /**/                   TRY(ms = j);   TEST_EQUAL(ms.count(), 151'200'000l);
    /**/                   TRY(us = j);   TEST_EQUAL(us.count(), 151'200'000'000ll);
    TRY(j = 123456789ms);  TRY(ms = j);   TEST_EQUAL(ms.count(), 123'456'789l);
    /**/                   TRY(s = j);    TEST_EQUAL(s.count(), 123'456);
    /**/                   TRY(min = j);  TEST_EQUAL(min.count(), 2057);
    /**/                   TRY(h = j);    TEST_EQUAL(h.count(), 34);
    TRY(j = sys_epoch);    TRY(sys = j);  TEST_EQUAL(sys.time_since_epoch().count(), 0);
    TRY(j = sys_now);      TRY(sys = j);  TEST_NEAR_EPSILON(sys.time_since_epoch().count(), sys_now.time_since_epoch().count(), sys_epsilon);
    TRY(j = hrc_epoch);    TRY(hrc = j);  TEST_NEAR_EPSILON(hrc.time_since_epoch().count(), 0, hrc_epsilon);
    TRY(j = hrc_now);      TRY(hrc = j);  TEST_NEAR_EPSILON(hrc.time_since_epoch().count(), hrc_now.time_since_epoch().count(), hrc_epsilon);

}

void test_core_serial_array_map() {

    using T = ArrayMap<int, Ustring>;

    static const T y = {};
    static const T z = {
        { 1, "Alpha" },
        { 2, "Bravo" },
        { 3, "Charlie" },
        { 4, "Delta" },
        { 5, "Echo" },
    };

    json j;
    T x;

    TRY(j = y);  TRY(x = j.get<T>());  TEST(x == y);  TEST_EQUAL(to_str(x), "{}");
    TRY(j = z);  TRY(x = j.get<T>());  TEST(x == z);  TEST_EQUAL(to_str(x), "{1:Alpha,2:Bravo,3:Charlie,4:Delta,5:Echo}");

}

void test_core_serial_auto_array() {

    using Q = AutoDeque<Ustring>;
    using V = AutoVector<Ustring>;

    json j;
    Q p, q, r;
    V u, v, w;

    r[-2] = "Alpha";
    r[-1] = "Bravo";
    r[0] = "Charlie";
    r[1] = "Delta";
    r[2] = "Echo";
    r[3] = "Foxtrot";
    w = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};

    TRY(j = q);  TRY(p = j.get<Q>());  TEST(p == q);  TEST_EQUAL(to_str(p), "[]");
    TRY(j = r);  TRY(p = j.get<Q>());  TEST(p == r);  TEST_EQUAL(to_str(p), "[Alpha,Bravo,Charlie,Delta,Echo,Foxtrot]");
    TEST_EQUAL(p.min_index(), -2);
    TEST_EQUAL(p.max_index(), 3);
    TRY(j = v);  TRY(u = j.get<V>());  TEST(u == v);  TEST_EQUAL(to_str(u), "[]");
    TRY(j = w);  TRY(u = j.get<V>());  TEST(u == w);  TEST_EQUAL(to_str(u), "[Alpha,Bravo,Charlie,Delta,Echo]");

}

void test_core_serial_blob() {

    json j;
    Blob x, y, z;

    z.reset(10);
    for (size_t i = 0; i < z.size(); ++i)
        z.c_data()[i] = char('a' + i);

    TRY(j = y);  TRY(x = j.get<Blob>());  TEST(x == y);  TEST_EQUAL(x.str(), "");
    TRY(j = z);  TRY(x = j.get<Blob>());  TEST(x == z);  TEST_EQUAL(x.str(), "abcdefghij");

}

void test_core_serial_bounded_array() {

    using T = BoundedArray<Ustring, 5>;

    static const T y = {};
    static const T z = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};

    json j;
    T x;

    TRY(j = y);  TRY(x = j.get<T>());  TEST(x == y);  TEST_EQUAL(to_str(x), "[]");
    TRY(j = z);  TRY(x = j.get<T>());  TEST(x == z);  TEST_EQUAL(to_str(x), "[Alpha,Bravo,Charlie,Delta,Echo]");

}

void test_core_serial_compact_array() {

    using T = CompactArray<Ustring, 2>;

    static const T y = {};
    static const T z = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};

    json j;
    T x;

    TRY(j = y);  TRY(x = j.get<T>());  TEST(x == y);  TEST_EQUAL(to_str(x), "[]");
    TRY(j = z);  TRY(x = j.get<T>());  TEST(x == z);  TEST_EQUAL(to_str(x), "[Alpha,Bravo,Charlie,Delta,Echo]");

}

void test_core_serial_grid() {

    using T = Grid<int, 3>;

    json j;
    T t, u, v;

    v.reset({3,4,5});
    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 4; ++y)
            for (int z = 0; z < 5; ++z)
                v(x,y,z) = 1000 + 100 * x + 10 * y + z;

    TRY(j = u);  TRY(t = j.get<T>());  TEST(t == u);  TEST_EQUAL(t.size(), 0);
    TRY(j = v);  TRY(t = j.get<T>());  TEST(t == v);  TEST_EQUAL(t.size(), 60);

    for (int x = 0; x < 3; ++x)
        for (int y = 0; y < 4; ++y)
            for (int z = 0; z < 5; ++z)
                TEST_EQUAL(t(x,y,z), 1000 + 100 * x + 10 * y + z);

}

void test_core_serial_mirror_map() {

    using T = MirrorMap<int, Ustring>;

    json j;
    T x, y, z;

    z.insert({1, "Alpha"});
    z.insert({2, "Bravo"});
    z.insert({3, "Charlie"});
    z.insert({4, "Delta"});
    z.insert({5, "Echo"});

    TRY(j = y);  TRY(x = j.get<T>());  TEST_EQUAL(x.size(), 0);  TEST_EQUAL(to_str(x.left()), "{}");
    TRY(j = z);  TRY(x = j.get<T>());  TEST_EQUAL(x.size(), 5);  TEST_EQUAL(to_str(x.left()), "{1:Alpha,2:Bravo,3:Charlie,4:Delta,5:Echo}");

}

void test_core_serial_ordered_map() {

    using T = OrderedMap<int, Ustring>;

    json j;
    T x, y, z;

    z.insert({5, "Echo"});
    z.insert({4, "Delta"});
    z.insert({3, "Charlie"});
    z.insert({2, "Bravo"});
    z.insert({1, "Alpha"});

    TRY(j = y);  TRY(x = j.get<T>());  TEST_EQUAL(x.size(), 0);  TEST_EQUAL(to_str(x), "{}");
    TRY(j = z);  TRY(x = j.get<T>());  TEST_EQUAL(x.size(), 5);  TEST_EQUAL(to_str(x), "{5:Echo,4:Delta,3:Charlie,2:Bravo,1:Alpha}");

}

void test_core_serial_vector_matrix_quaternion() {

    static const Float3 cv = {1.0f, 2.0f, 3.0f};
    static const Qfloat cq = {1.0f, 2.0f, 3.0f, 4.0f};
    static const Float3x3r crm = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    static const Float3x3c ccm = crm;

    json j;
    Float3 v;
    Qfloat q;
    Float3x3r rm;
    Float3x3c cm;

    TRY(j = cv);   TRY(v = j.get<Float3>());      TEST_EQUAL(v, cv);
    TRY(j = cq);   TRY(q = j.get<Qfloat>());      TEST_EQUAL(q, cq);
    TRY(j = crm);  TRY(rm = j.get<Float3x3r>());  TEST_EQUAL(rm, crm);
    TRY(j = ccm);  TRY(cm = j.get<Float3x3c>());  TEST_EQUAL(cm, ccm);

    TEST_EQUAL(to_str(crm),
        "[[1,2,3],"
        "[4,5,6],"
        "[7,8,9]]"
    );
    TEST_EQUAL(to_str(ccm),
        "[[1,2,3],"
        "[4,5,6],"
        "[7,8,9]]"
    );
    TEST_EQUAL(to_str(rm),
        "[[1,2,3],"
        "[4,5,6],"
        "[7,8,9]]"
    );
    TEST_EQUAL(to_str(cm),
        "[[1,2,3],"
        "[4,5,6],"
        "[7,8,9]]"
    );

}

void test_core_serial_endian() {

    static const BigEndian<uint32_t> cbe32 = 32;
    static const BigEndian<uint64_t> cbe64 = 64;
    static const LittleEndian<uint32_t> cle32 = 33;
    static const LittleEndian<uint64_t> cle64 = 65;

    json j;
    BigEndian<uint32_t> be32 = 0;
    BigEndian<uint64_t> be64 = 0;
    LittleEndian<uint32_t> le32 = 0;
    LittleEndian<uint64_t> le64 = 0;

    TRY(j = cbe32);  TRY(be32 = j);  TEST_EQUAL(be32, cbe32);
    TRY(j = cbe64);  TRY(be64 = j);  TEST_EQUAL(be64, cbe64);
    TRY(j = cle32);  TRY(le32 = j);  TEST_EQUAL(le32, cle32);
    TRY(j = cle64);  TRY(le64 = j);  TEST_EQUAL(le64, cle64);

}

void test_core_serial_multiprecision_integers() {

    static const Mpint ci = Mpint("-987654321987654321987654321987654321987654321");
    static const Mpuint cn = Mpuint("123456789123456789123456789123456789123456789");

    json j;
    Mpint i;
    Mpuint n;

    TRY(j = ci);  TRY(i = j.get<Mpint>());  TEST_EQUAL(i, ci);  TEST_EQUAL(i.str(), "-987654321987654321987654321987654321987654321");
    TRY(j = cn);  TRY(n = j.get<Mpuint>());  TEST_EQUAL(n, cn);  TEST_EQUAL(n.str(), "123456789123456789123456789123456789123456789");

}

void test_core_serial_optional() {

    using T = Optional<Ustring>;

    static const T y = {};
    static const T z = "Hello world"s;

    json j;
    T x;

    TRY(j = y);  TRY(x = j.get<T>());  TEST_EQUAL(x, y);  TEST(! x.has_value());  TEST_EQUAL(x.value_or("None"), "None");
    TRY(j = z);  TRY(x = j.get<T>());  TEST_EQUAL(x, z);  TEST(x.has_value());    TEST_EQUAL(x.value_or("None"), "Hello world");

}

void test_core_serial_rational() {

    static const Ratio y = {};
    static const Ratio z = {16, 10};

    json j;
    Ratio x;

    TRY(j = y);   TRY(x = j.get<Ratio>());  TEST_EQUAL(x, y);   TEST_EQUAL(x.num(), 0);   TEST_EQUAL(x.den(), 1);
    TRY(j = z);   TRY(x = j.get<Ratio>());  TEST_EQUAL(x, z);   TEST_EQUAL(x.num(), 8);   TEST_EQUAL(x.den(), 5);
    TRY(j = 42);  TRY(x = j.get<Ratio>());  TEST_EQUAL(x, 42);  TEST_EQUAL(x.num(), 42);  TEST_EQUAL(x.den(), 1);

}

void test_core_serial_uuid() {

    static const Uuid y = {};
    static const Uuid z = Uuid("a3fe3c0b-d4fe-fa0e-acd0-b5c5d1ace7d3");

    json j;
    Uuid x;

    TRY(j = y);  TRY(x = j.get<Uuid>());  TEST_EQUAL(x, y);  TEST_EQUAL(x.str(), "00000000-0000-0000-0000-000000000000");
    TRY(j = z);  TRY(x = j.get<Uuid>());  TEST_EQUAL(x, z);  TEST_EQUAL(x.str(), "a3fe3c0b-d4fe-fa0e-acd0-b5c5d1ace7d3");

}

void test_core_serial_version() {

    static const Version u = {};
    static const Version v = {1, 2, 3};
    static const Version w = {1, 2, 3, "alpha"};

    json j;
    Version x;

    TRY(j = u);  TRY(x = j.get<Version>());  TEST_EQUAL(x, u);  TEST_EQUAL(x.str(), "0.0");
    TRY(j = v);  TRY(x = j.get<Version>());  TEST_EQUAL(x, v);  TEST_EQUAL(x.str(), "1.2.3");
    TRY(j = w);  TRY(x = j.get<Version>());  TEST_EQUAL(x, w);  TEST_EQUAL(x.str(), "1.2.3alpha");

}

void test_core_serial_xcolour() {

    static const Xcolour u{};
    static const Xcolour v{12};
    static const Xcolour w{1,2,3};

    json j;
    Xcolour x;

    TRY(j = u);  TRY(x = j.get<Xcolour>());  TEST_EQUAL(x, u);  TEST_EQUAL(x.str(), "");
    TRY(j = v);  TRY(x = j.get<Xcolour>());  TEST_EQUAL(x, v);  TEST_EQUAL(x.str(), "[12]");
    TRY(j = w);  TRY(x = j.get<Xcolour>());  TEST_EQUAL(x, w);  TEST_EQUAL(x.str(), "[1,2,3]");

}

namespace {

    enum Alpha { a_zero, a_one, a_two, a_three };
    enum class Bravo { b_zero, b_one, b_two, b_three };
    RS_ENUM(Charlie, uint32_t, 0, c_zero, c_one, c_two, c_three);
    RS_ENUM_CLASS(Delta, uint32_t, 0, d_zero, d_one, d_two, d_three);
    RS_ENUM(Echo, uint32_t, 0, e_zero, e_one, e_two, e_three);
    RS_ENUM_CLASS(Foxtrot, uint32_t, 0, f_zero, f_one, f_two, f_three);
    RS_ENUM_CLASS(Golf, uint32_t, 100, f_zero, f_one, f_two, f_three);

    RS_SERIALIZE_ENUM_AS_INTEGER(Alpha);
    RS_SERIALIZE_ENUM_AS_INTEGER(Bravo);
    RS_SERIALIZE_ENUM_AS_INTEGER(Charlie);
    RS_SERIALIZE_ENUM_AS_INTEGER(Delta);
    RS_SERIALIZE_ENUM_AS_STRING(Echo);
    RS_SERIALIZE_ENUM_AS_STRING(Foxtrot);
    RS_SERIALIZE_ENUM_AS_STRING(Golf);

}

void test_core_serial_enum_helper_functions() {

    Alpha a1 = {}, a2 = {};
    Bravo b1 = {}, b2 = {};
    Charlie c1 = {}, c2 = {};
    Delta d1 = {}, d2 = {};
    Echo e1 = {}, e2 = {};
    Foxtrot f1 = {}, f2 = {};
    Golf g = {};
    json j;

    a1 = Alpha::a_three;    TRY(j = a1);  TEST_EQUAL(j.dump(), "3");            TRY(a2 = j.get<Alpha>());    TEST(a2 == Alpha::a_three);
    b1 = Bravo::b_three;    TRY(j = b1);  TEST_EQUAL(j.dump(), "3");            TRY(b2 = j.get<Bravo>());    TEST(b2 == Bravo::b_three);
    c1 = Charlie::c_three;  TRY(j = c1);  TEST_EQUAL(j.dump(), "3");            TRY(c2 = j.get<Charlie>());  TEST_EQUAL(c2, Charlie::c_three);
    d1 = Delta::d_three;    TRY(j = d1);  TEST_EQUAL(j.dump(), "3");            TRY(d2 = j.get<Delta>());    TEST_EQUAL(d2, Delta::d_three);
    e1 = Echo::e_three;     TRY(j = e1);  TEST_EQUAL(j.dump(), "\"e_three\"");  TRY(e2 = j.get<Echo>());     TEST_EQUAL(e2, Echo::e_three);
    f1 = Foxtrot::f_three;  TRY(j = f1);  TEST_EQUAL(j.dump(), "\"f_three\"");  TRY(f2 = j.get<Foxtrot>());  TEST_EQUAL(f2, Foxtrot::f_three);
    f1 = Foxtrot::f_three;  TRY(j = f1);  TEST_EQUAL(j.dump(), "\"f_three\"");  TRY(g = j.get<Golf>());      TEST_EQUAL(g, Golf::f_three);

}

namespace {

    struct Thing {
        std::string a, b;
        int c = 0, d = 0;
    };

}

void test_core_serial_struct_helper_functions() {

    Thing t;
    json j;

    t = {"hello", "world", 86, 99};
    TRY(struct_to_json(j, t, "*",
        &Thing::a, "a",
        &Thing::b, "b",
        &Thing::c, "c",
        &Thing::d, "d"
    ));

    TEST_EQUAL(j.dump(), "{\"_type\":\"Thing\",\"a\":\"hello\",\"b\":\"world\",\"c\":86,\"d\":99}");

    t = {};
    TRY(json_to_struct(j, t, "*",
        &Thing::a, "a",
        &Thing::b, "b",
        &Thing::c, "c",
        &Thing::d, "d"
    ));
    TEST_EQUAL(t.a, "hello");
    TEST_EQUAL(t.b, "world");
    TEST_EQUAL(t.c, 86);
    TEST_EQUAL(t.d, 99);

    t = {};
    TEST_THROW(json_to_struct(j, t, "Nothing",
        &Thing::a, "a",
        &Thing::b, "b",
        &Thing::c, "c",
        &Thing::d, "d"
    ), std::invalid_argument);
    TEST_EQUAL(t.a, "");
    TEST_EQUAL(t.b, "");
    TEST_EQUAL(t.c, 0);
    TEST_EQUAL(t.d, 0);

    j.erase("_type");
    t = {};
    TEST_THROW(json_to_struct(j, t, "*",
        &Thing::a, "a",
        &Thing::b, "b",
        &Thing::c, "c",
        &Thing::d, "d"
    ), std::invalid_argument);
    TEST_EQUAL(t.a, "");
    TEST_EQUAL(t.b, "");
    TEST_EQUAL(t.c, 0);
    TEST_EQUAL(t.d, 0);

    t = {};
    TRY(json_to_struct(j, t, "",
        &Thing::a, "a",
        &Thing::b, "b",
        &Thing::c, "c",
        &Thing::d, "d"
    ));
    TEST_EQUAL(t.a, "hello");
    TEST_EQUAL(t.b, "world");
    TEST_EQUAL(t.c, 86);
    TEST_EQUAL(t.d, 99);

}

void test_core_serial_persistent_storage() {

    static constexpr const char* vendor = "com.captaincrowbar";
    static constexpr const char* app = "test-core-serial";

    static const Path archive1 = std_path(UserPath::settings) / "com.captaincrowbar/test-core-serial.settings";
    static const Path archive2 = std_path(UserPath::settings) / "com.captaincrowbar/test-core-serial.new.settings";
    static const Path archive3 = std_path(UserPath::settings) / "com.captaincrowbar/test-core-serial.old.settings";
    static const Ustring vendor_app = "com.captaincrowbar/test-core-serial";

    TRY(archive1.remove());
    TEST(! archive1.exists());

    {

        PersistState store(vendor, app);
        Ustring content, s;
        Strings v;
        int n = 0;

        TEST_EQUAL(store.id(), vendor_app);
        TEST_EQUAL(store.file().name(), archive1);

        Persist<int> pnum;
        Persist<Ustring> pstr;

        TRY((pnum = {store, "number"}));
        TRY((pstr = {store, "string"}));

        TEST_EQUAL(pnum.key(), "number");
        TEST_EQUAL(pstr.key(), "string");

        TRY(n = pnum);
        TEST_EQUAL(n, 0);
        TRY(s = pstr);
        TEST_EQUAL(s, "");

        TRY(store.save());
        TEST(store.file().exists());

        TRY(store.file().load(content));
        TEST_EQUAL(content, R"(
            {
                "number": 0,
                "string": ""
            }
            )"_doc);

        TRY(pnum = 42);
        TRY(pstr = "Hello world");

        TRY(n = pnum);
        TEST_EQUAL(n, 42);
        TRY(s = pstr);
        TEST_EQUAL(s, "Hello world");

        TRY(store.save());
        TEST(store.file().exists());

        TRY(store.file().load(content));
        TEST_EQUAL(content, R"(
            {
                "number": 42,
                "string": "Hello world"
            }
            )"_doc);

        TRY(store.autosave(10ms));
        TRY(pnum = 86);
        std::this_thread::sleep_for(100ms);
        TRY(store.autosave_off());
        TRY(pnum = 99);
        std::this_thread::sleep_for(100ms);
        TRY(store.load());
        TEST_EQUAL(pnum.get(), 86);

    }

    TEST(archive1.exists());
    TRY(archive1.remove());
    TEST(! archive1.exists());
    TEST(! archive2.exists());
    TEST(! archive3.exists());

}
