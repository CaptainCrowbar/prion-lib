#include "rs-core/serial.hpp"
#include "rs-core/blob.hpp"
#include "rs-core/compact-array.hpp"
#include "rs-core/file-system.hpp"
#include "rs-core/mp-integer.hpp"
#include "rs-core/optional.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/string.hpp"
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

void test_core_serial_blob() {

    json j;
    Blob b, b1, b2;

    b2.reset(10);
    for (size_t i = 0; i < b2.size(); ++i)
        b2.c_data()[i] = char('a' + i);

    TRY(j = b1);  TRY(b = j.get<Blob>());  TEST(b == b1);  TEST_EQUAL(b.str(), "");
    TRY(j = b2);  TRY(b = j.get<Blob>());  TEST(b == b2);  TEST_EQUAL(b.str(), "abcdefghij");

}

void test_core_serial_compact_array() {

    using CA = CompactArray<Ustring, 2>;

    static const CA ca1 = {};
    static const CA ca2 = {"Alpha", "Bravo", "Charlie", "Delta", "Echo"};

    json j;
    CA ca;

    TRY(j = ca1);  TRY(ca = j.get<CA>());  TEST(ca == ca1);  TEST_EQUAL(to_str(ca), "[]");
    TRY(j = ca2);  TRY(ca = j.get<CA>());  TEST(ca == ca2);  TEST_EQUAL(to_str(ca), "[Alpha,Bravo,Charlie,Delta,Echo]");

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

    static const Nat cn = Nat("123456789123456789123456789123456789123456789");
    static const Int ci = Int("-987654321987654321987654321987654321987654321");

    json j;
    Nat n;
    Int i;

    TRY(j = cn);  TRY(n = j.get<Nat>());  TEST_EQUAL(n, cn);  TEST_EQUAL(n.str(), "123456789123456789123456789123456789123456789");
    TRY(j = ci);  TRY(i = j.get<Int>());  TEST_EQUAL(i, ci);  TEST_EQUAL(i.str(), "-987654321987654321987654321987654321987654321");

}

void test_core_serial_optional() {

    using OS = Optional<Ustring>;

    static const OS cos1 = {};
    static const OS cos2 = "Hello world"s;

    json j;
    OS os;

    TRY(j = cos1);  TRY(os = j.get<OS>());  TEST_EQUAL(os, cos1);  TEST(! os.has_value());  TEST_EQUAL(os.value_or("None"), "None");
    TRY(j = cos2);  TRY(os = j.get<OS>());  TEST_EQUAL(os, cos2);  TEST(os.has_value());    TEST_EQUAL(os.value_or("None"), "Hello world");

}

void test_core_serial_rational() {

    static const Rat cr1 = {};
    static const Rat cr2 = {16, 10};

    json j;
    Rat r;

    TRY(j = cr1);  TRY(r = j.get<Rat>());  TEST_EQUAL(r, cr1);  TEST_EQUAL(r.num(), 0);   TEST_EQUAL(r.den(), 1);
    TRY(j = cr2);  TRY(r = j.get<Rat>());  TEST_EQUAL(r, cr2);  TEST_EQUAL(r.num(), 8);   TEST_EQUAL(r.den(), 5);
    TRY(j = 42);   TRY(r = j.get<Rat>());  TEST_EQUAL(r, 42);   TEST_EQUAL(r.num(), 42);  TEST_EQUAL(r.den(), 1);

}

void test_core_serial_uuid() {

    static const Uuid cu1 = {};
    static const Uuid cu2 = Uuid("a3fe3c0b-d4fe-fa0e-acd0-b5c5d1ace7d3");

    json j;
    Uuid u;

    TRY(j = cu1);  TRY(u = j.get<Uuid>());  TEST_EQUAL(u, cu1);  TEST_EQUAL(u.str(), "00000000-0000-0000-0000-000000000000");
    TRY(j = cu2);  TRY(u = j.get<Uuid>());  TEST_EQUAL(u, cu2);  TEST_EQUAL(u.str(), "a3fe3c0b-d4fe-fa0e-acd0-b5c5d1ace7d3");

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

void test_core_serial_version() {

    static const Version cv1 = {};
    static const Version cv2 = {1, 2, 3};
    static const Version cv3 = {1, 2, 3, "alpha"};

    json j;
    Version v;

    TRY(j = cv1);  TRY(v = j.get<Version>());  TEST_EQUAL(v, cv1);  TEST_EQUAL(v.str(), "0.0");
    TRY(j = cv2);  TRY(v = j.get<Version>());  TEST_EQUAL(v, cv2);  TEST_EQUAL(v.str(), "1.2.3");
    TRY(j = cv3);  TRY(v = j.get<Version>());  TEST_EQUAL(v, cv3);  TEST_EQUAL(v.str(), "1.2.3alpha");

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
