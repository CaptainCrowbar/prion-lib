#include "rs-core/scale-map.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;

void test_core_scale_map_class() {

    ScaleMap<double> sm;

    TEST(sm.empty());
    TEST_EQUAL(sm.min(), 0);
    TEST_EQUAL(sm.max(), 0);
    TEST_EQUAL(sm(0), 0);
    TEST_EQUAL(sm(10), 0);

    TRY(sm.insert(5, 10));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 5);
    TEST_EQUAL(sm.max(), 5);
    TEST_EQUAL(sm(0), 10);
    TEST_EQUAL(sm(5), 10);
    TEST_EQUAL(sm(10), 10);

    TRY(sm.insert(10, 20));
    TEST_EQUAL(sm.min(), 5);
    TEST_EQUAL(sm.max(), 10);
    TEST_EQUAL(sm(4), 10);
    TEST_EQUAL(sm(5), 10);
    TEST_EQUAL(sm(6), 12);
    TEST_EQUAL(sm(7), 14);
    TEST_EQUAL(sm(8), 16);
    TEST_EQUAL(sm(9), 18);
    TEST_EQUAL(sm(10), 20);
    TEST_EQUAL(sm(11), 20);

    TRY(sm.insert(15, 100, 10));
    TRY(sm.insert(20, -10));
    TEST_EQUAL(sm.min(), 5);
    TEST_EQUAL(sm.max(), 20);
    TEST_EQUAL(sm(4), 10);
    TEST_EQUAL(sm(5), 10);
    TEST_EQUAL(sm(6), 12);
    TEST_EQUAL(sm(7), 14);
    TEST_EQUAL(sm(8), 16);
    TEST_EQUAL(sm(9), 18);
    TEST_EQUAL(sm(10), 20);
    TEST_EQUAL(sm(11), 36);
    TEST_EQUAL(sm(12), 52);
    TEST_EQUAL(sm(13), 68);
    TEST_EQUAL(sm(14), 84);
    TEST_EQUAL(sm(15), 55);
    TEST_EQUAL(sm(16), 6);
    TEST_EQUAL(sm(17), 2);
    TEST_EQUAL(sm(18), -2);
    TEST_EQUAL(sm(19), -6);
    TEST_EQUAL(sm(20), -10);
    TEST_EQUAL(sm(21), -10);

    TRY(sm.erase(10));
    TEST_EQUAL(sm.min(), 5);
    TEST_EQUAL(sm.max(), 20);
    TEST_EQUAL(sm(4), 10);
    TEST_EQUAL(sm(5), 10);
    TEST_EQUAL(sm(6), 19);
    TEST_EQUAL(sm(7), 28);
    TEST_EQUAL(sm(8), 37);
    TEST_EQUAL(sm(9), 46);
    TEST_EQUAL(sm(10), 55);
    TEST_EQUAL(sm(11), 64);
    TEST_EQUAL(sm(12), 73);
    TEST_EQUAL(sm(13), 82);
    TEST_EQUAL(sm(14), 91);
    TEST_EQUAL(sm(15), 55);
    TEST_EQUAL(sm(16), 6);
    TEST_EQUAL(sm(17), 2);
    TEST_EQUAL(sm(18), -2);
    TEST_EQUAL(sm(19), -6);
    TEST_EQUAL(sm(20), -10);
    TEST_EQUAL(sm(21), -10);

    TRY(sm.erase(5, 15));
    TEST_EQUAL(sm.min(), 20);
    TEST_EQUAL(sm.max(), 20);
    TEST_EQUAL(sm(4), -10);
    TEST_EQUAL(sm(5), -10);
    TEST_EQUAL(sm(6), -10);
    TEST_EQUAL(sm(7), -10);
    TEST_EQUAL(sm(8), -10);
    TEST_EQUAL(sm(9), -10);
    TEST_EQUAL(sm(10), -10);
    TEST_EQUAL(sm(11), -10);
    TEST_EQUAL(sm(12), -10);
    TEST_EQUAL(sm(13), -10);
    TEST_EQUAL(sm(14), -10);
    TEST_EQUAL(sm(15), -10);
    TEST_EQUAL(sm(16), -10);
    TEST_EQUAL(sm(17), -10);
    TEST_EQUAL(sm(18), -10);
    TEST_EQUAL(sm(19), -10);
    TEST_EQUAL(sm(20), -10);
    TEST_EQUAL(sm(21), -10);

    TRY(sm.erase(0, 100));
    TEST(sm.empty());
    TEST_EQUAL(sm.min(), 0);
    TEST_EQUAL(sm.max(), 0);
    TEST_EQUAL(sm(0), 0);
    TEST_EQUAL(sm(10), 0);

    TRY((sm = {
        {10, 100},
        {20, 200, 300, 400},
        {30, 500, 600},
    }));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 10);
    TEST_EQUAL(sm.max(), 30);
    TEST_EQUAL(sm(5), 100);
    TEST_EQUAL(sm(10), 100);
    TEST_EQUAL(sm(15), 150);
    TEST_EQUAL(sm(20), 300);
    TEST_EQUAL(sm(25), 450);
    TEST_EQUAL(sm(30), 550);
    TEST_EQUAL(sm(35), 600);

    TRY(sm.scale_x(2, 100));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 120);
    TEST_EQUAL(sm.max(), 160);
    TEST_EQUAL(sm(110), 100);
    TEST_EQUAL(sm(120), 100);
    TEST_EQUAL(sm(130), 150);
    TEST_EQUAL(sm(140), 300);
    TEST_EQUAL(sm(150), 450);
    TEST_EQUAL(sm(160), 550);
    TEST_EQUAL(sm(170), 600);

    TRY(sm.scale_x(-0.5, 100));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 20);
    TEST_EQUAL(sm.max(), 40);
    TEST_EQUAL(sm(15), 600);
    TEST_EQUAL(sm(20), 550);
    TEST_EQUAL(sm(25), 450);
    TEST_EQUAL(sm(30), 300);
    TEST_EQUAL(sm(35), 150);
    TEST_EQUAL(sm(40), 100);
    TEST_EQUAL(sm(45), 100);

    TRY(sm.scale_y(0.5, 1000));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 20);
    TEST_EQUAL(sm.max(), 40);
    TEST_EQUAL(sm(15), 1300);
    TEST_EQUAL(sm(20), 1275);
    TEST_EQUAL(sm(25), 1225);
    TEST_EQUAL(sm(30), 1150);
    TEST_EQUAL(sm(35), 1075);
    TEST_EQUAL(sm(40), 1050);
    TEST_EQUAL(sm(45), 1050);

    TRY(sm.scale_x(0));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 0);
    TEST_EQUAL(sm.max(), 0);
    TEST_EQUAL(sm(-5), 1300);
    TEST_EQUAL(sm(0), 1175);
    TEST_EQUAL(sm(5), 1050);

    TRY(sm.clear());
    TEST(sm.empty());
    TEST_EQUAL(sm.min(), 0);
    TEST_EQUAL(sm.max(), 0);
    TEST_EQUAL(sm(0), 0);
    TEST_EQUAL(sm(10), 0);

    TRY(sm.insert(20, 25, 75, 100));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 20);
    TEST_EQUAL(sm.max(), 20);
    TEST_EQUAL(sm(15), 25);
    TEST_EQUAL(sm(20), 75);
    TEST_EQUAL(sm(25), 100);

    TRY(sm.scale_x(2));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), 40);
    TEST_EQUAL(sm.max(), 40);
    TEST_EQUAL(sm(35), 25);
    TEST_EQUAL(sm(40), 75);
    TEST_EQUAL(sm(45), 100);

    TRY(sm.scale_x(-2));
    TEST(! sm.empty());
    TEST_EQUAL(sm.min(), -80);
    TEST_EQUAL(sm.max(), -80);
    TEST_EQUAL(sm(-85), 100);
    TEST_EQUAL(sm(-80), 75);
    TEST_EQUAL(sm(-75), 25);

}
