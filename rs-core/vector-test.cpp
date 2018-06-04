#include "rs-core/vector.hpp"
#include "rs-core/float.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace RS;
using namespace RS::Literals;

namespace {

    template <typename R>
    void fuzz(R& r) {
        for (auto& x: r)
            if (std::abs(x) < 1e-6)
                x = 0;
    }

    void check_2d_helper(double x, double y, double r, double theta) {

        Double2 car1 = {x, y};
        Double2 pol1 = {r, theta};
        Double2 car2 = polar_to_cartesian(pol1);
        Double2 pol2 = cartesian_to_polar(car1);

        TEST_NEAR_RANGE(car2, car1);
        TEST_NEAR_RANGE(pol2, pol1);

    }

    void check_3d_helper(double x, double y, double z, double r, double phi, double theta, double rho) {

        Double3 car1 = {x, y, z};
        Double3 cyl1 = {rho, phi, z};
        Double3 sph1 = {r, phi, theta};
        Double3 car2 = cylindrical_to_cartesian(cyl1);
        Double3 car3 = spherical_to_cartesian(sph1);
        Double3 cyl2 = cartesian_to_cylindrical(car1);
        Double3 cyl3 = spherical_to_cylindrical(sph1);
        Double3 sph2 = cartesian_to_spherical(car1);
        Double3 sph3 = cylindrical_to_spherical(cyl1);

        TEST_NEAR_RANGE(car2, car1);
        TEST_NEAR_RANGE(car3, car1);
        TEST_NEAR_RANGE(cyl2, cyl1);
        TEST_NEAR_RANGE(cyl3, cyl1);
        TEST_NEAR_RANGE(sph2, sph1);
        TEST_NEAR_RANGE(sph3, sph1);

    }

}

void test_core_vector_integer() {

    int x = 0;
    Int3 v1(1), v2, v3, v4;
    const Int3& cv1 = v1;
    const Int3& cv2 = v2;
    const Int3& cv3 = v3;

    TEST_EQUAL(sizeof(Int3), 3 * sizeof(int));

    TEST(! v1.is_null());
    TEST(v2.is_null());

    TEST_EQUAL(to_str(v1), "[1,1,1]");
    TEST_EQUAL(to_str(cv1), "[1,1,1]");

    TEST_EQUAL(v1[0], 1);  TEST_EQUAL(cv1[0], 1);
    TEST_EQUAL(v1[1], 1);  TEST_EQUAL(cv1[1], 1);
    TEST_EQUAL(v1[2], 1);  TEST_EQUAL(cv1[2], 1);

    TRY(v2[0] = 99);
    TRY(v2[1] = 98);
    TRY(v2[2] = 97);
    TEST_EQUAL(to_str(v2), "[99,98,97]");
    TEST_EQUAL(to_str(cv2), "[99,98,97]");

    TEST_EQUAL(v2[0], 99);     TEST_EQUAL(cv2[0], 99);
    TEST_EQUAL(v2[1], 98);     TEST_EQUAL(cv2[1], 98);
    TEST_EQUAL(v2[2], 97);     TEST_EQUAL(cv2[2], 97);
    TEST_EQUAL(v2.at(0), 99);  TEST_EQUAL(cv2.at(0), 99);
    TEST_EQUAL(v2.at(1), 98);  TEST_EQUAL(cv2.at(1), 98);
    TEST_EQUAL(v2.at(2), 97);  TEST_EQUAL(cv2.at(2), 97);
    TEST_EQUAL(v2.x(), 99);    TEST_EQUAL(cv2.x(), 99);
    TEST_EQUAL(v2.y(), 98);    TEST_EQUAL(cv2.y(), 98);
    TEST_EQUAL(v2.z(), 97);    TEST_EQUAL(cv2.z(), 97);

    TEST_THROW(v2.at(3), std::out_of_range);
    TEST_THROW(cv2.at(3), std::out_of_range);

    TRY((v3 = Int3{1,2,3}));
    TEST_EQUAL(to_str(v3), "[1,2,3]");
    TEST_EQUAL(to_str(cv3), "[1,2,3]");

    TRY((v3 = Int3(2,4,6)));
    TEST_EQUAL(to_str(v3), "[2,4,6]");
    TEST_EQUAL(to_str(cv3), "[2,4,6]");

    TRY((v3 = {10,20,30}));
    TEST_EQUAL(to_str(v3), "[10,20,30]");
    TEST_EQUAL(to_str(cv3), "[10,20,30]");

    TEST_EQUAL(v3[0], 10);          TEST_EQUAL(cv3[0], 10);
    TEST_EQUAL(v3[1], 20);          TEST_EQUAL(cv3[1], 20);
    TEST_EQUAL(v3[2], 30);          TEST_EQUAL(cv3[2], 30);
    TEST_EQUAL(v3.begin()[0], 10);  TEST_EQUAL(cv3.begin()[0], 10);
    TEST_EQUAL(v3.begin()[1], 20);  TEST_EQUAL(cv3.begin()[1], 20);
    TEST_EQUAL(v3.begin()[2], 30);  TEST_EQUAL(cv3.begin()[2], 30);

    TEST(v1 == v1);      TEST(! (v1 == v2));  TEST(! (v1 == v3));  TEST(! (v2 == v3));
    TEST(! (v1 != v1));  TEST(v1 != v2);      TEST(v1 != v3);      TEST(v2 != v3);

    TRY((v1 = {2,3,5}));
    TRY((v2 = {7,11,13}));

    TRY(v3 = + v1);     TEST_EQUAL(to_str(v3), "[2,3,5]");
    TRY(v3 = - v1);     TEST_EQUAL(to_str(v3), "[-2,-3,-5]");
    TRY(v3 = v1 + v2);  TEST_EQUAL(to_str(v3), "[9,14,18]");
    TRY(v3 = v1 - v2);  TEST_EQUAL(to_str(v3), "[-5,-8,-8]");
    TRY(v3 = 17 * v1);  TEST_EQUAL(to_str(v3), "[34,51,85]");
    TRY(v3 = v1 * 19);  TEST_EQUAL(to_str(v3), "[38,57,95]");
    TRY(x = v1 % v2);   TEST_EQUAL(x, 112);
    TRY(v3 = v1 ^ v2);  TEST_EQUAL(to_str(v3), "[-16,9,1]");
    TRY(v3 = v1 * v2);  TEST_EQUAL(to_str(v3), "[14,33,65]");
    TRY(v3 = v2 / v1);  TEST_EQUAL(to_str(v3), "[3,3,2]");

    TRY(v3 = Int3::unit(0));  TEST_EQUAL(to_str(v3), "[1,0,0]");
    TRY(v3 = Int3::unit(1));  TEST_EQUAL(to_str(v3), "[0,1,0]");
    TRY(v3 = Int3::unit(2));  TEST_EQUAL(to_str(v3), "[0,0,1]");

    Int2 s2;
    Int3 s3;
    Int4 s4;

    TRY((v1 = {1,2,3}));

    TRY(s2 = v1["xy"]);    TEST_EQUAL(to_str(s2), "[1,2]");
    TRY(s2 = v1["yz"]);    TEST_EQUAL(to_str(s2), "[2,3]");
    TRY(s2 = v1["zx"]);    TEST_EQUAL(to_str(s2), "[3,1]");
    TRY(s2 = v1["xx"]);    TEST_EQUAL(to_str(s2), "[1,1]");
    TRY(s2 = v1["yy"]);    TEST_EQUAL(to_str(s2), "[2,2]");
    TRY(s2 = v1["zz"]);    TEST_EQUAL(to_str(s2), "[3,3]");
    TRY(s3 = v1["xyz"]);   TEST_EQUAL(to_str(s3), "[1,2,3]");
    TRY(s3 = v1["zyx"]);   TEST_EQUAL(to_str(s3), "[3,2,1]");
    TRY(s3 = v1["xxx"]);   TEST_EQUAL(to_str(s3), "[1,1,1]");
    TRY(s3 = v1["yyy"]);   TEST_EQUAL(to_str(s3), "[2,2,2]");
    TRY(s3 = v1["zzz"]);   TEST_EQUAL(to_str(s3), "[3,3,3]");
    TRY(s4 = v1["xyzx"]);  TEST_EQUAL(to_str(s4), "[1,2,3,1]");
    TRY(s4 = v1["yzxy"]);  TEST_EQUAL(to_str(s4), "[2,3,1,2]");
    TRY(s4 = v1["zxyz"]);  TEST_EQUAL(to_str(s4), "[3,1,2,3]");
    TRY(s4 = v1["xxxx"]);  TEST_EQUAL(to_str(s4), "[1,1,1,1]");
    TRY(s4 = v1["yyyy"]);  TEST_EQUAL(to_str(s4), "[2,2,2,2]");
    TRY(s4 = v1["zzzz"]);  TEST_EQUAL(to_str(s4), "[3,3,3,3]");

    TRY((v1 = {1,5,9}));
    TRY((v2 = {2,3,4}));
    TRY((v3 = {4,6,8}));
    TRY(v1 = clamp(v1, v2, v3));
    TEST_EQUAL(to_str(v1), "[2,5,8]");

    TRY((v1 = {1,3,5}));
    TRY((v2 = {2,3,4}));
    TRY(v3 = min(v1, v2));
    TEST_EQUAL(to_str(v3), "[1,3,4]");
    TRY(v3 = max(v1, v2));
    TEST_EQUAL(to_str(v3), "[2,3,5]");
    TRY(std::tie(v3, v4) = minmax(v1, v2));
    TEST_EQUAL(to_str(v3), "[1,3,4]");
    TEST_EQUAL(to_str(v4), "[2,3,5]");

}

void test_core_vector_floating() {

    double x = 0;
    Double3 v1(1), v2, v3, v4;
    const Double3& cv1(v1);
    const Double3& cv2(v2);
    const Double3& cv3(v3);

    TEST_EQUAL(sizeof(Double3), 3 * sizeof(double));

    TEST(! v1.is_null());
    TEST(v2.is_null());

    TEST_EQUAL(to_str(v1), "[1,1,1]");
    TEST_EQUAL(to_str(cv1), "[1,1,1]");

    TEST_EQUAL(v1[0], 1);  TEST_EQUAL(cv1[0], 1);
    TEST_EQUAL(v1[1], 1);  TEST_EQUAL(cv1[1], 1);
    TEST_EQUAL(v1[2], 1);  TEST_EQUAL(cv1[2], 1);

    TRY(v2[0] = 99);
    TRY(v2[1] = 98);
    TRY(v2[2] = 97);
    TEST_EQUAL(to_str(v2), "[99,98,97]");
    TEST_EQUAL(to_str(cv2), "[99,98,97]");

    TEST_EQUAL(v2[0], 99);     TEST_EQUAL(cv2[0], 99);
    TEST_EQUAL(v2[1], 98);     TEST_EQUAL(cv2[1], 98);
    TEST_EQUAL(v2[2], 97);     TEST_EQUAL(cv2[2], 97);
    TEST_EQUAL(v2.at(0), 99);  TEST_EQUAL(cv2.at(0), 99);
    TEST_EQUAL(v2.at(1), 98);  TEST_EQUAL(cv2.at(1), 98);
    TEST_EQUAL(v2.at(2), 97);  TEST_EQUAL(cv2.at(2), 97);
    TEST_EQUAL(v2.x(), 99);    TEST_EQUAL(cv2.x(), 99);
    TEST_EQUAL(v2.y(), 98);    TEST_EQUAL(cv2.y(), 98);
    TEST_EQUAL(v2.z(), 97);    TEST_EQUAL(cv2.z(), 97);

    TEST_THROW(v2.at(3), std::out_of_range);
    TEST_THROW(cv2.at(3), std::out_of_range);

    TRY((v3 = Double3{1,2,3}));
    TEST_EQUAL(to_str(v3), "[1,2,3]");
    TEST_EQUAL(to_str(cv3), "[1,2,3]");

    TRY((v3 = Double3(2,4,6)));
    TEST_EQUAL(to_str(v3), "[2,4,6]");
    TEST_EQUAL(to_str(cv3), "[2,4,6]");

    TRY((v3 = {10,20,30}));
    TEST_EQUAL(to_str(v3), "[10,20,30]");
    TEST_EQUAL(to_str(cv3), "[10,20,30]");

    TEST_EQUAL(v3[0], 10);          TEST_EQUAL(cv3[0], 10);
    TEST_EQUAL(v3[1], 20);          TEST_EQUAL(cv3[1], 20);
    TEST_EQUAL(v3[2], 30);          TEST_EQUAL(cv3[2], 30);
    TEST_EQUAL(v3.begin()[0], 10);  TEST_EQUAL(cv3.begin()[0], 10);
    TEST_EQUAL(v3.begin()[1], 20);  TEST_EQUAL(cv3.begin()[1], 20);
    TEST_EQUAL(v3.begin()[2], 30);  TEST_EQUAL(cv3.begin()[2], 30);

    TEST(v1 == v1);      TEST(! (v1 == v2));  TEST(! (v1 == v3));  TEST(! (v2 == v3));
    TEST(! (v1 != v1));  TEST(v1 != v2);      TEST(v1 != v3);      TEST(v2 != v3);

    TRY((v1 = {2,3,5}));
    TRY((v2 = {7,11,13}));

    TRY(v3 = + v1);          TEST_EQUAL(to_str(v3), "[2,3,5]");
    TRY(v3 = - v1);          TEST_EQUAL(to_str(v3), "[-2,-3,-5]");
    TRY(v3 = v1 + v2);       TEST_EQUAL(to_str(v3), "[9,14,18]");
    TRY(v3 = v1 - v2);       TEST_EQUAL(to_str(v3), "[-5,-8,-8]");
    TRY(v3 = 17 * v1);       TEST_EQUAL(to_str(v3), "[34,51,85]");
    TRY(v3 = v1 * 19);       TEST_EQUAL(to_str(v3), "[38,57,95]");
    TRY(v3 = v1 / 23);       TEST_EQUAL(to_str(v3), "[0.0869565,0.130435,0.217391]");
    TRY(x = v1 % v2);        TEST_EQUAL(x, 112);
    TRY(v3 = v1 ^ v2);       TEST_EQUAL(to_str(v3), "[-16,9,1]");
    TRY(v3 = v1 * v2);  TEST_EQUAL(to_str(v3), "[14,33,65]");
    TRY(v3 = v1 / v2);  TEST_EQUAL(to_str(v3), "[0.285714,0.272727,0.384615]");

    TEST_EQUAL(v1.r2(), 38);   TEST_NEAR(v1.r(), 6.164414);
    TEST_EQUAL(v2.r2(), 339);  TEST_NEAR(v2.r(), 18.411953);

    TRY(v3 = Double3::unit(0));  TEST_EQUAL(to_str(v3), "[1,0,0]");
    TRY(v3 = Double3::unit(1));  TEST_EQUAL(to_str(v3), "[0,1,0]");
    TRY(v3 = Double3::unit(2));  TEST_EQUAL(to_str(v3), "[0,0,1]");

    TRY(v3 = Double3().dir());  TEST_EQUAL(to_str(v3), "[0,0,0]");
    TRY(v3 = v1.dir());         TEST_EQUAL(to_str(v3), "[0.324443,0.486664,0.811107]");
    TRY(v3 = v2.dir());         TEST_EQUAL(to_str(v3), "[0.380188,0.597438,0.706063]");

    TRY((v1 = {1,2,3}));
    TRY((v2 = {}));          TRY(x = v1.angle(v2));  TEST_EQUAL(x, 0);
    TRY((v2 = {2,4,6}));     TRY(x = v1.angle(v2));  TEST_EQUAL(x, 0);
    TRY((v2 = {-2,-4,-6}));  TRY(x = v1.angle(v2));  TEST_NEAR(x, pi_d);
    TRY((v2 = {3,2,1}));     TRY(x = v1.angle(v2));  TEST_NEAR(x, 0.775193);
    TRY((v2 = {-3,-2,-1}));  TRY(x = v1.angle(v2));  TEST_NEAR(x, 2.366399);

    TRY((v1 = {2,3,5}));
    TRY((v2 = {7,11,13}));
    TRY(v3 = v1.project(v2));
    TRY(v4 = v1.reject(v2));
    TEST_NEAR_RANGE(v3 + v4, v1);
    TEST_NEAR_RANGE(v2 ^ v3, Double3());
    TEST_NEAR(v2 % v4, 0);
    TEST_NEAR(v3 % v4, 0);

    TRY((v1 = {1,5,9}));
    TRY((v2 = {2,3,4}));
    TRY((v3 = {4,6,8}));
    TRY(v1 = clamp(v1, v2, v3));
    TEST_EQUAL(to_str(v1), "[2,5,8]");

    TRY((v1 = {1,3,5}));
    TRY((v2 = {2,3,4}));
    TRY(v3 = min(v1, v2));
    TEST_EQUAL(to_str(v3), "[1,3,4]");
    TRY(v3 = max(v1, v2));
    TEST_EQUAL(to_str(v3), "[2,3,5]");
    TRY(std::tie(v3, v4) = minmax(v1, v2));
    TEST_EQUAL(to_str(v3), "[1,3,4]");
    TEST_EQUAL(to_str(v4), "[2,3,5]");

}

void test_core_vector_matrix() {

    Double3 v1, v2;
    Double3x3r rm1(2, 1), rm2, rm3;
    Double3x3c cm1(2, 1), cm2, cm3;
    std::vector<double> v;

    TEST_EQUAL(sizeof(Double3x3r), 9 * sizeof(double));
    TEST_EQUAL(sizeof(Double3x3c), 9 * sizeof(double));

    TEST_EQUAL(to_str(rm1),
        "[[2,1,1],"
        "[1,2,1],"
        "[1,1,2]]"
    );
    TEST_EQUAL(to_str(cm1),
        "[[2,1,1],"
        "[1,2,1],"
        "[1,1,2]]"
    );

    TRY((rm1 = Double3x3r{1,2,3,4,5,6,7,8,9}));
    TRY((cm1 = Double3x3c{1,2,3,4,5,6,7,8,9}));
    TEST_EQUAL(to_str(irange(rm1.begin(), rm1.end())), "[1,2,3,4,5,6,7,8,9]");
    TEST_EQUAL(to_str(irange(cm1.begin(), cm1.end())), "[1,2,3,4,5,6,7,8,9]");

    TRY((rm1 = {2,3,5,7,11,13,17,19,23}));
    TRY((cm1 = {2,3,5,7,11,13,17,19,23}));
    TEST_EQUAL(to_str(irange(rm1.begin(), rm1.end())), "[2,3,5,7,11,13,17,19,23]");
    TEST_EQUAL(to_str(irange(cm1.begin(), cm1.end())), "[2,3,5,7,11,13,17,19,23]");

    TEST_EQUAL(to_str(rm1),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TEST_EQUAL(to_str(cm1),
        "[[2,7,17],"
        "[3,11,19],"
        "[5,13,23]]"
    );

    TRY(rm2 = Double3x3r(cm1));
    TEST_EQUAL(to_str(rm2),
        "[[2,7,17],"
        "[3,11,19],"
        "[5,13,23]]"
    );
    TRY(cm2 = Double3x3c(rm1));
    TEST_EQUAL(to_str(cm2),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );

    TRY(rm2 = cm1);
    TEST_EQUAL(to_str(rm2),
        "[[2,7,17],"
        "[3,11,19],"
        "[5,13,23]]"
    );
    TRY(cm2 = rm1);
    TEST_EQUAL(to_str(cm2),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );

    TEST_EQUAL(rm1(0, 0), 2);
    TEST_EQUAL(rm1(0, 1), 3);
    TEST_EQUAL(rm1(0, 2), 5);
    TEST_EQUAL(rm1(1, 0), 7);
    TEST_EQUAL(rm1(1, 1), 11);
    TEST_EQUAL(rm1(1, 2), 13);
    TEST_EQUAL(rm1(2, 0), 17);
    TEST_EQUAL(rm1(2, 1), 19);
    TEST_EQUAL(rm1(2, 2), 23);
    TEST_EQUAL(cm1(0, 0), 2);
    TEST_EQUAL(cm1(0, 1), 7);
    TEST_EQUAL(cm1(0, 2), 17);
    TEST_EQUAL(cm1(1, 0), 3);
    TEST_EQUAL(cm1(1, 1), 11);
    TEST_EQUAL(cm1(1, 2), 19);
    TEST_EQUAL(cm1(2, 0), 5);
    TEST_EQUAL(cm1(2, 1), 13);
    TEST_EQUAL(cm1(2, 2), 23);

    TEST_EQUAL(rm1.at(0, 0), 2);
    TEST_EQUAL(rm1.at(0, 1), 3);
    TEST_EQUAL(rm1.at(0, 2), 5);
    TEST_EQUAL(rm1.at(1, 0), 7);
    TEST_EQUAL(rm1.at(1, 1), 11);
    TEST_EQUAL(rm1.at(1, 2), 13);
    TEST_EQUAL(rm1.at(2, 0), 17);
    TEST_EQUAL(rm1.at(2, 1), 19);
    TEST_EQUAL(rm1.at(2, 2), 23);
    TEST_EQUAL(cm1.at(0, 0), 2);
    TEST_EQUAL(cm1.at(0, 1), 7);
    TEST_EQUAL(cm1.at(0, 2), 17);
    TEST_EQUAL(cm1.at(1, 0), 3);
    TEST_EQUAL(cm1.at(1, 1), 11);
    TEST_EQUAL(cm1.at(1, 2), 19);
    TEST_EQUAL(cm1.at(2, 0), 5);
    TEST_EQUAL(cm1.at(2, 1), 13);
    TEST_EQUAL(cm1.at(2, 2), 23);

    TEST_THROW(rm1.at(0, 3), std::out_of_range);
    TEST_THROW(rm1.at(3, 0), std::out_of_range);
    TEST_THROW(rm1.at(3, 3), std::out_of_range);
    TEST_THROW(cm1.at(0, 3), std::out_of_range);
    TEST_THROW(cm1.at(3, 0), std::out_of_range);
    TEST_THROW(cm1.at(3, 3), std::out_of_range);

    TRY((rm2 = {29,31,37,41,43,47,53,59,61}));
    TRY(cm1 = rm1);
    TRY(cm2 = rm2);

    TEST_EQUAL(to_str(rm1),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TEST_EQUAL(to_str(cm1),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TEST_EQUAL(to_str(rm2),
        "[[29,31,37],"
        "[41,43,47],"
        "[53,59,61]]"
    );
    TEST_EQUAL(to_str(cm2),
        "[[29,31,37],"
        "[41,43,47],"
        "[53,59,61]]"
    );

    TEST(rm1 == rm1);
    TEST(rm1 != rm2);
    TEST(rm1 == cm1);
    TEST(rm1 != cm2);
    TEST(cm1 == rm1);
    TEST(cm1 != rm2);
    TEST(cm1 == cm1);
    TEST(cm1 != cm2);

    TRY(rm3 = rm1 * 10);
    TEST_EQUAL(to_str(rm3),
        "[[20,30,50],"
        "[70,110,130],"
        "[170,190,230]]"
    );
    TRY(cm3 = cm1 * 10);
    TEST_EQUAL(to_str(cm3),
        "[[20,30,50],"
        "[70,110,130],"
        "[170,190,230]]"
    );

    TRY(rm3 = 10 * rm1);
    TEST_EQUAL(to_str(rm3),
        "[[20,30,50],"
        "[70,110,130],"
        "[170,190,230]]"
    );
    TRY(cm3 = 10 * cm1);
    TEST_EQUAL(to_str(cm3),
        "[[20,30,50],"
        "[70,110,130],"
        "[170,190,230]]"
    );

    TRY(rm3 = rm1 / 10);
    TEST_EQUAL(to_str(rm3),
        "[[0.2,0.3,0.5],"
        "[0.7,1.1,1.3],"
        "[1.7,1.9,2.3]]"
    );
    TRY(cm3 = cm1 / 10);
    TEST_EQUAL(to_str(cm3),
        "[[0.2,0.3,0.5],"
        "[0.7,1.1,1.3],"
        "[1.7,1.9,2.3]]"
    );

    TRY(rm3 = rm1 + rm2);
    TEST_EQUAL(to_str(rm3),
        "[[31,34,42],"
        "[48,54,60],"
        "[70,78,84]]"
    );
    TRY(cm3 = cm1 + cm2);
    TEST_EQUAL(to_str(cm3),
        "[[31,34,42],"
        "[48,54,60],"
        "[70,78,84]]"
    );
    TRY(rm3 = rm1 + cm2);
    TEST_EQUAL(to_str(rm3),
        "[[31,34,42],"
        "[48,54,60],"
        "[70,78,84]]"
    );
    TRY(cm3 = cm1 + rm2);
    TEST_EQUAL(to_str(cm3),
        "[[31,34,42],"
        "[48,54,60],"
        "[70,78,84]]"
    );

    TRY(rm3 = rm1 - rm2);
    TEST_EQUAL(to_str(rm3),
        "[[-27,-28,-32],"
        "[-34,-32,-34],"
        "[-36,-40,-38]]"
    );
    TRY(cm3 = cm1 - cm2);
    TEST_EQUAL(to_str(cm3),
        "[[-27,-28,-32],"
        "[-34,-32,-34],"
        "[-36,-40,-38]]"
    );
    TRY(rm3 = rm1 - cm2);
    TEST_EQUAL(to_str(rm3),
        "[[-27,-28,-32],"
        "[-34,-32,-34],"
        "[-36,-40,-38]]"
    );
    TRY(cm3 = cm1 - rm2);
    TEST_EQUAL(to_str(cm3),
        "[[-27,-28,-32],"
        "[-34,-32,-34],"
        "[-36,-40,-38]]"
    );

    TRY(rm3 = rm1 * rm2);
    TEST_EQUAL(to_str(rm3),
        "[[446,486,520],"
        "[1343,1457,1569],"
        "[2491,2701,2925]]"
    );
    TRY(cm3 = cm1 * cm2);
    TEST_EQUAL(to_str(cm3),
        "[[446,486,520],"
        "[1343,1457,1569],"
        "[2491,2701,2925]]"
    );
    TRY(rm3 = rm1 * cm2);
    TEST_EQUAL(to_str(rm3),
        "[[446,486,520],"
        "[1343,1457,1569],"
        "[2491,2701,2925]]"
    );
    TRY(cm3 = cm1 * rm2);
    TEST_EQUAL(to_str(cm3),
        "[[446,486,520],"
        "[1343,1457,1569],"
        "[2491,2701,2925]]"
    );

    TRY((v1 = {67,71,73}));

    TRY(v2 = rm1 * v1);  TEST_EQUAL(to_str(v2), "[712,2199,4167]");
    TRY(v2 = v1 * rm1);  TEST_EQUAL(to_str(v2), "[1872,2369,2937]");
    TRY(v2 = cm1 * v1);  TEST_EQUAL(to_str(v2), "[712,2199,4167]");
    TRY(v2 = v1 * cm1);  TEST_EQUAL(to_str(v2), "[1872,2369,2937]");

    TRY(rm3 = Double3x3r::identity());
    TEST_EQUAL(to_str(rm3),
        "[[1,0,0],"
        "[0,1,0],"
        "[0,0,1]]"
    );
    TRY(cm3 = Double3x3r::identity());
    TEST_EQUAL(to_str(cm3),
        "[[1,0,0],"
        "[0,1,0],"
        "[0,0,1]]"
    );

    TRY(rm3 = rm1);
    TRY(v1 = rm3.column(0));  TEST_EQUAL(to_str(v1), "[2,7,17]");
    TRY(v1 = rm3.column(1));  TEST_EQUAL(to_str(v1), "[3,11,19]");
    TRY(v1 = rm3.column(2));  TEST_EQUAL(to_str(v1), "[5,13,23]");
    TRY(v1 = rm3.row(0));     TEST_EQUAL(to_str(v1), "[2,3,5]");
    TRY(v1 = rm3.row(1));     TEST_EQUAL(to_str(v1), "[7,11,13]");
    TRY(v1 = rm3.row(2));     TEST_EQUAL(to_str(v1), "[17,19,23]");

    TRY(cm3 = cm1);
    TRY(v1 = cm3.column(0));  TEST_EQUAL(to_str(v1), "[2,7,17]");
    TRY(v1 = cm3.column(1));  TEST_EQUAL(to_str(v1), "[3,11,19]");
    TRY(v1 = cm3.column(2));  TEST_EQUAL(to_str(v1), "[5,13,23]");
    TRY(v1 = cm3.row(0));     TEST_EQUAL(to_str(v1), "[2,3,5]");
    TRY(v1 = cm3.row(1));     TEST_EQUAL(to_str(v1), "[7,11,13]");
    TRY(v1 = cm3.row(2));     TEST_EQUAL(to_str(v1), "[17,19,23]");

    TRY((v1 = {1,2,3}));
    TRY(rm3 = rm1);
    TRY(rm3.set_column(0, v1));
    TEST_EQUAL(to_str(rm3),
        "[[1,3,5],"
        "[2,11,13],"
        "[3,19,23]]"
    );
    TRY(rm3.set_column(1, v1));
    TEST_EQUAL(to_str(rm3),
        "[[1,1,5],"
        "[2,2,13],"
        "[3,3,23]]"
    );
    TRY(rm3.set_column(2, v1));
    TEST_EQUAL(to_str(rm3),
        "[[1,1,1],"
        "[2,2,2],"
        "[3,3,3]]"
    );
    TRY(rm3 = rm1);
    TRY(rm3.set_row(0, v1));
    TEST_EQUAL(to_str(rm3),
        "[[1,2,3],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TRY(rm3.set_row(1, v1));
    TEST_EQUAL(to_str(rm3),
        "[[1,2,3],"
        "[1,2,3],"
        "[17,19,23]]"
    );
    TRY(rm3.set_row(2, v1));
    TEST_EQUAL(to_str(rm3),
        "[[1,2,3],"
        "[1,2,3],"
        "[1,2,3]]"
    );

    TRY(cm3 = cm1);
    TRY(cm3.set_column(0, v1));
    TEST_EQUAL(to_str(cm3),
        "[[1,3,5],"
        "[2,11,13],"
        "[3,19,23]]"
    );
    TRY(cm3.set_column(1, v1));
    TEST_EQUAL(to_str(cm3),
        "[[1,1,5],"
        "[2,2,13],"
        "[3,3,23]]"
    );
    TRY(cm3.set_column(2, v1));
    TEST_EQUAL(to_str(cm3),
        "[[1,1,1],"
        "[2,2,2],"
        "[3,3,3]]"
    );
    TRY(cm3 = cm1);
    TRY(cm3.set_row(0, v1));
    TEST_EQUAL(to_str(cm3),
        "[[1,2,3],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TRY(cm3.set_row(1, v1));
    TEST_EQUAL(to_str(cm3),
        "[[1,2,3],"
        "[1,2,3],"
        "[17,19,23]]"
    );
    TRY(cm3.set_row(2, v1));
    TEST_EQUAL(to_str(cm3),
        "[[1,2,3],"
        "[1,2,3],"
        "[1,2,3]]"
    );

    TRY(rm3 = rm1.swap_rows(0, 1));
    TEST_EQUAL(to_str(rm3),
        "[[7,11,13],"
        "[2,3,5],"
        "[17,19,23]]"
    );
    TRY(rm3 = rm3.swap_rows(1, 2));
    TEST_EQUAL(to_str(rm3),
        "[[7,11,13],"
        "[17,19,23],"
        "[2,3,5]]"
    );
    TRY(rm3 = rm3.swap_columns(0, 1));
    TEST_EQUAL(to_str(rm3),
        "[[11,7,13],"
        "[19,17,23],"
        "[3,2,5]]"
    );
    TRY(rm3 = rm3.swap_columns(1, 2));
    TEST_EQUAL(to_str(rm3),
        "[[11,13,7],"
        "[19,23,17],"
        "[3,5,2]]"
    );

    TRY(cm3 = rm1.swap_rows(0, 1));
    TEST_EQUAL(to_str(cm3),
        "[[7,11,13],"
        "[2,3,5],"
        "[17,19,23]]"
    );
    TRY(cm3 = cm3.swap_rows(1, 2));
    TEST_EQUAL(to_str(cm3),
        "[[7,11,13],"
        "[17,19,23],"
        "[2,3,5]]"
    );
    TRY(cm3 = cm3.swap_columns(0, 1));
    TEST_EQUAL(to_str(cm3),
        "[[11,7,13],"
        "[19,17,23],"
        "[3,2,5]]"
    );
    TRY(cm3 = cm3.swap_columns(1, 2));
    TEST_EQUAL(to_str(cm3),
        "[[11,13,7],"
        "[19,23,17],"
        "[3,5,2]]"
    );

    TRY(rm3 = rm1.transpose());
    TEST_EQUAL(to_str(rm3),
        "[[2,7,17],"
        "[3,11,19],"
        "[5,13,23]]"
    );
    TRY(rm3 = rm3.transpose());
    TEST_EQUAL(to_str(rm3),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TRY(cm3 = cm1.transpose());
    TEST_EQUAL(to_str(cm3),
        "[[2,7,17],"
        "[3,11,19],"
        "[5,13,23]]"
    );
    TRY(cm3 = cm3.transpose());
    TEST_EQUAL(to_str(cm3),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );

    v = {1,2,3,4,5,6,7,8,9};
    TRY(rm1 = Double3x3r::from_array(v.data()));
    TEST_EQUAL(to_str(rm1),
        "[[1,2,3],"
        "[4,5,6],"
        "[7,8,9]]"
    );
    TRY(cm1 = Double3x3c::from_array(v.data()));
    TEST_EQUAL(to_str(cm1),
        "[[1,4,7],"
        "[2,5,8],"
        "[3,6,9]]"
    );

    TRY(rm1 = Double3x3r::from_columns(1,2,3,4,5,6,7,8,9));
    TEST_EQUAL(to_str(rm1),
        "[[1,4,7],"
        "[2,5,8],"
        "[3,6,9]]"
    );
    TRY(rm1 = Double3x3r::from_rows(1,2,3,4,5,6,7,8,9));
    TEST_EQUAL(to_str(rm1),
        "[[1,2,3],"
        "[4,5,6],"
        "[7,8,9]]"
    );
    TRY(cm1 = Double3x3r::from_columns(1,2,3,4,5,6,7,8,9));
    TEST_EQUAL(to_str(cm1),
        "[[1,4,7],"
        "[2,5,8],"
        "[3,6,9]]"
    );
    TRY(cm1 = Double3x3r::from_rows(1,2,3,4,5,6,7,8,9));
    TEST_EQUAL(to_str(cm1),
        "[[1,2,3],"
        "[4,5,6],"
        "[7,8,9]]"
    );

}

void test_core_vector_matrix_inversion() {

    double x = 0;

    Double2x2r r2, s2, t2;
    Double3x3r r3, s3, t3;
    Double4x4r r4, s4, t4;
    Double2x2c c2, d2, e2;
    Double3x3c c3, d3, e3;
    Double4x4c c4, d4, e4;

    TRY((r2 = {2,3,5,7}));
    TEST_EQUAL(to_str(r2),
        "[[2,3],"
        "[5,7]]"
    );
    TRY(c2 = r2);
    TEST_EQUAL(to_str(c2),
        "[[2,3],"
        "[5,7]]"
    );
    TRY((r3 = {2,3,5,7,11,13,17,19,23}));
    TEST_EQUAL(to_str(r3),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TRY(c3 = r3);
    TEST_EQUAL(to_str(c3),
        "[[2,3,5],"
        "[7,11,13],"
        "[17,19,23]]"
    );
    TRY((r4 = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53}));
    TEST_EQUAL(to_str(r4),
        "[[2,3,5,7],"
        "[11,13,17,19],"
        "[23,29,31,37],"
        "[41,43,47,53]]"
    );
    TRY(c4 = r4);
    TEST_EQUAL(to_str(c4),
        "[[2,3,5,7],"
        "[11,13,17,19],"
        "[23,29,31,37],"
        "[41,43,47,53]]"
    );

    TRY(x = r2.det());  TEST_EQUAL(x, -1);
    TRY(x = c2.det());  TEST_EQUAL(x, -1);
    TRY(x = r3.det());  TEST_EQUAL(x, -78);
    TRY(x = c3.det());  TEST_EQUAL(x, -78);
    TRY(x = r4.det());  TEST_EQUAL(x, 880);
    TRY(x = c4.det());  TEST_EQUAL(x, 880);

    Ustring id_str_2 = to_str(Double2x2r::identity());
    Ustring id_str_3 = to_str(Double3x3r::identity());
    Ustring id_str_4 = to_str(Double4x4r::identity());
    Ustring inv_str_2 =
        "[[-7,3],"
        "[5,-2]]";
    Ustring inv_str_3 =
        "[[-0.0769231,-0.333333,0.205128],"
        "[-0.769231,0.5,-0.115385],"
        "[0.692308,-0.166667,-0.0128205]]";
    Ustring inv_str_4 =
        "[[0.272727,-0.218182,-0.2,0.181818],"
        "[-0.454545,-0.0363636,0.3,-0.136364],"
        "[-0.590909,0.697727,-0.1,-0.102273],"
        "[0.681818,-0.420455,0,0.0795455]]";

    TRY(s2 = r2.inverse());  TEST_EQUAL(to_str(s2), inv_str_2);  TRY(t2 = r2 * s2);  TRY(fuzz(t2));  TEST_EQUAL(to_str(t2), id_str_2);
    TRY(d2 = c2.inverse());  TEST_EQUAL(to_str(d2), inv_str_2);  TRY(e2 = c2 * d2);  TRY(fuzz(e2));  TEST_EQUAL(to_str(e2), id_str_2);
    TRY(s3 = r3.inverse());  TEST_EQUAL(to_str(s3), inv_str_3);  TRY(t3 = r3 * s3);  TRY(fuzz(t3));  TEST_EQUAL(to_str(t3), id_str_3);
    TRY(d3 = c3.inverse());  TEST_EQUAL(to_str(d3), inv_str_3);  TRY(e3 = c3 * d3);  TRY(fuzz(e3));  TEST_EQUAL(to_str(e3), id_str_3);
    TRY(s4 = r4.inverse());  TEST_EQUAL(to_str(s4), inv_str_4);  TRY(t4 = r4 * s4);  TRY(fuzz(t4));  TEST_EQUAL(to_str(t4), id_str_4);
    TRY(d4 = c4.inverse());  TEST_EQUAL(to_str(d4), inv_str_4);  TRY(e4 = c4 * d4);  TRY(fuzz(e4));  TEST_EQUAL(to_str(e4), id_str_4);

}

void test_core_vector_quaternion() {

    Qdouble q, r, s;

    TEST_EQUAL(to_str(q), "[0,0,0,0]");
    TRY(q = 42);
    TEST_EQUAL(to_str(q), "[42,0,0,0]");
    TRY((q = {10,20,30,40}));
    TEST_EQUAL(to_str(q), "[10,20,30,40]");
    TEST_EQUAL(q.a(), 10);
    TEST_EQUAL(q.b(), 20);
    TEST_EQUAL(q.c(), 30);
    TEST_EQUAL(q.d(), 40);
    TEST_EQUAL(q[0], 10);
    TEST_EQUAL(q[1], 20);
    TEST_EQUAL(q[2], 30);
    TEST_EQUAL(q[3], 40);

    TEST_EQUAL(q.s_part(), 10);
    TEST_EQUAL(to_str(q.v_part()), "[20,30,40]");
    TEST_EQUAL(q.norm2(), 3000);
    TEST_NEAR(q.norm(), 54.772256);
    TEST_EQUAL(to_str(q.versor()), "[0.182574,0.365148,0.547723,0.730297]");
    TEST_EQUAL(to_str(q.recip()), "[0.00333333,-0.00666667,-0.01,-0.0133333]");

    TRY(q *= 2);
    TEST_EQUAL(to_str(q), "[20,40,60,80]");
    TRY(q /= 4);
    TEST_EQUAL(to_str(q), "[5,10,15,20]");

    TRY((q = {10,20,30,40}));
    TRY((r = {100,200,300,400}));
    TRY(s = q + r);
    TEST_EQUAL(to_str(s), "[110,220,330,440]");
    TRY(s = q - r);
    TEST_EQUAL(to_str(s), "[-90,-180,-270,-360]");

    TRY((q = {2,3,5,7}));
    TRY((r = {11,13,17,19}));
    TRY(s = q * r);
    TEST_EQUAL(to_str(s), "[-235,35,123,101]");

    TRY((q = {10,20,30,40}));
    TRY(r = q * q.recip());
    TRY(fuzz(r));
    TEST_EQUAL(to_str(r), "[1,0,0,0]");
    TRY(r = q.recip() * q);
    TRY(fuzz(r));
    TEST_EQUAL(to_str(r), "[1,0,0,0]");

}

void test_core_vector_2d_coordinate_transformations() {

    check_2d_helper(0,          0,          0,  0);
    check_2d_helper(1,          0,          1,  0);
    check_2d_helper(0.866025,   0.5,        1,  30_deg);
    check_2d_helper(0.5,        0.866025,   1,  60_deg);
    check_2d_helper(0,          1,          1,  90_deg);
    check_2d_helper(-0.5,       0.866025,   1,  120_deg);
    check_2d_helper(-0.866025,  0.5,        1,  150_deg);
    check_2d_helper(-1,         0,          1,  180_deg);
    check_2d_helper(-0.866025,  -0.5,       1,  -150_deg);
    check_2d_helper(-0.5,       -0.866025,  1,  -120_deg);
    check_2d_helper(0,          -1,         1,  -90_deg);
    check_2d_helper(0.5,        -0.866025,  1,  -60_deg);
    check_2d_helper(0.866025,   -0.5,       1,  -30_deg);

}

void test_core_vector_3d_coordinate_transformations() {

    check_3d_helper(0,          0,          0,          0,  0,         0,        0);
    check_3d_helper(1,          0,          0,          1,  0,         90_deg,   1);
    check_3d_helper(0.866025,   0.5,        0,          1,  30_deg,    90_deg,   1);
    check_3d_helper(0.5,        0.866025,   0,          1,  60_deg,    90_deg,   1);
    check_3d_helper(0,          1,          0,          1,  90_deg,    90_deg,   1);
    check_3d_helper(-0.5,       0.866025,   0,          1,  120_deg,   90_deg,   1);
    check_3d_helper(-0.866025,  0.5,        0,          1,  150_deg,   90_deg,   1);
    check_3d_helper(-1,         0,          0,          1,  180_deg,   90_deg,   1);
    check_3d_helper(-0.866025,  -0.5,       0,          1,  -150_deg,  90_deg,   1);
    check_3d_helper(-0.5,       -0.866025,  0,          1,  -120_deg,  90_deg,   1);
    check_3d_helper(0,          -1,         0,          1,  -90_deg,   90_deg,   1);
    check_3d_helper(0.5,        -0.866025,  0,          1,  -60_deg,   90_deg,   1);
    check_3d_helper(0.866025,   -0.5,       0,          1,  -30_deg,   90_deg,   1);
    check_3d_helper(0.707107,   0,          0.707107,   1,  0,         45_deg,   0.707107);
    check_3d_helper(0.612372,   0.353553,   0.707107,   1,  30_deg,    45_deg,   0.707107);
    check_3d_helper(0.353553,   0.612372,   0.707107,   1,  60_deg,    45_deg,   0.707107);
    check_3d_helper(0,          0.707107,   0.707107,   1,  90_deg,    45_deg,   0.707107);
    check_3d_helper(-0.353553,  0.612372,   0.707107,   1,  120_deg,   45_deg,   0.707107);
    check_3d_helper(-0.612372,  0.353553,   0.707107,   1,  150_deg,   45_deg,   0.707107);
    check_3d_helper(-0.707107,  0,          0.707107,   1,  180_deg,   45_deg,   0.707107);
    check_3d_helper(-0.612372,  -0.353553,  0.707107,   1,  -150_deg,  45_deg,   0.707107);
    check_3d_helper(-0.353553,  -0.612372,  0.707107,   1,  -120_deg,  45_deg,   0.707107);
    check_3d_helper(0,          -0.707107,  0.707107,   1,  -90_deg,   45_deg,   0.707107);
    check_3d_helper(0.353553,   -0.612372,  0.707107,   1,  -60_deg,   45_deg,   0.707107);
    check_3d_helper(0.612372,   -0.353553,  0.707107,   1,  -30_deg,   45_deg,   0.707107);
    check_3d_helper(0.707107,   0,          -0.707107,  1,  0,         135_deg,  0.707107);
    check_3d_helper(0.612372,   0.353553,   -0.707107,  1,  30_deg,    135_deg,  0.707107);
    check_3d_helper(0.353553,   0.612372,   -0.707107,  1,  60_deg,    135_deg,  0.707107);
    check_3d_helper(0,          0.707107,   -0.707107,  1,  90_deg,    135_deg,  0.707107);
    check_3d_helper(-0.353553,  0.612372,   -0.707107,  1,  120_deg,   135_deg,  0.707107);
    check_3d_helper(-0.612372,  0.353553,   -0.707107,  1,  150_deg,   135_deg,  0.707107);
    check_3d_helper(-0.707107,  0,          -0.707107,  1,  180_deg,   135_deg,  0.707107);
    check_3d_helper(-0.612372,  -0.353553,  -0.707107,  1,  -150_deg,  135_deg,  0.707107);
    check_3d_helper(-0.353553,  -0.612372,  -0.707107,  1,  -120_deg,  135_deg,  0.707107);
    check_3d_helper(0,          -0.707107,  -0.707107,  1,  -90_deg,   135_deg,  0.707107);
    check_3d_helper(0.353553,   -0.612372,  -0.707107,  1,  -60_deg,   135_deg,  0.707107);
    check_3d_helper(0.612372,   -0.353553,  -0.707107,  1,  -30_deg,   135_deg,  0.707107);
    check_3d_helper(0,          0,          1,          1,  0,         0,        0);
    check_3d_helper(0,          0,          -1,         1,  0,         180_deg,  0);

}

void test_core_vector_projective() {

    Double3 v3;
    Double4 v4;

    TRY((v3 = {10,20,30}));     TRY(v4 = point4(v3));  TEST_EQUAL(to_str(v4), "[10,20,30,1]");
    TRY((v3 = {10,20,30}));     TRY(v4 = norm4(v3));   TEST_EQUAL(to_str(v4), "[10,20,30,0]");
    TRY((v4 = {10,20,30,40}));  TRY(v3 = point3(v4));  TEST_EQUAL(to_str(v3), "[0.25,0.5,0.75]");
    TRY((v4 = {10,20,30,40}));  TRY(v3 = norm3(v4));   TEST_EQUAL(to_str(v3), "[10,20,30]");

    Double3x3c cm3;
    Double4x4c cm4;
    Double3x3r rm3;
    Double4x4r rm4;

    TRY(cm3(0, 0) = rm3(0, 0) = 2);
    TRY(cm3(0, 1) = rm3(0, 1) = 3);
    TRY(cm3(0, 2) = rm3(0, 2) = 5);
    TRY(cm3(1, 0) = rm3(1, 0) = 7);
    TRY(cm3(1, 1) = rm3(1, 1) = 11);
    TRY(cm3(1, 2) = rm3(1, 2) = 13);
    TRY(cm3(2, 0) = rm3(2, 0) = 17);
    TRY(cm3(2, 1) = rm3(2, 1) = 19);
    TRY(cm3(2, 2) = rm3(2, 2) = 23);
    TRY((v3 = {100,200,300}));

    TRY(cm4 = make_transform(cm3, v3));
    TEST_EQUAL(to_str(cm4),
        "[[2,3,5,100],"
        "[7,11,13,200],"
        "[17,19,23,300],"
        "[0,0,0,1]]"
    );
    TRY(cm4 = normal_transform(cm4));
    TRY(rm4 = make_transform(rm3, v3));
    TEST_EQUAL(to_str(rm4),
        "[[2,3,5,100],"
        "[7,11,13,200],"
        "[17,19,23,300],"
        "[0,0,0,1]]"
    );
    TRY(rm4 = normal_transform(rm4));

}

void test_core_vector_primitives() {

    const Double3 zero, u = {1,0,0}, v = {0,1,0}, w = {0,0,1}, p = {2,3,5};

    Double3 r;
    Double3x3 m3;
    Double4x4 m4;

    TRY(m3 = rotate3(30_deg, 0));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[1,0,0],"
        "[0,0.866025,-0.5],"
        "[0,0.5,0.866025]]"
    );
    TRY(r = m3 * u);  TEST_EQUAL(to_str(r), "[1,0,0]");
    TRY(r = m3 * v);  TEST_EQUAL(to_str(r), "[0,0.866025,0.5]");
    TRY(r = m3 * w);  TEST_EQUAL(to_str(r), "[0,-0.5,0.866025]");

    TRY(m3 = rotate3(30_deg, 1));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[0.866025,0,0.5],"
        "[0,1,0],"
        "[-0.5,0,0.866025]]"
    );
    TRY(r = m3 * u);  TEST_EQUAL(to_str(r), "[0.866025,0,-0.5]");
    TRY(r = m3 * v);  TEST_EQUAL(to_str(r), "[0,1,0]");
    TRY(r = m3 * w);  TEST_EQUAL(to_str(r), "[0.5,0,0.866025]");

    TRY(m3 = rotate3(30_deg, 2));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[0.866025,-0.5,0],"
        "[0.5,0.866025,0],"
        "[0,0,1]]"
    );
    TRY(r = m3 * u);  TEST_EQUAL(to_str(r), "[0.866025,0.5,0]");
    TRY(r = m3 * v);  TEST_EQUAL(to_str(r), "[-0.5,0.866025,0]");
    TRY(r = m3 * w);  TEST_EQUAL(to_str(r), "[0,0,1]");

    TRY(m4 = rotate4(30_deg, 0));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[1,0,0,0],"
        "[0,0.866025,-0.5,0],"
        "[0,0.5,0.866025,0],"
        "[0,0,0,1]]"
    );
    TRY(r = point3(m4 * point4(u)));  TEST_EQUAL(to_str(r), "[1,0,0]");
    TRY(r = point3(m4 * point4(v)));  TEST_EQUAL(to_str(r), "[0,0.866025,0.5]");
    TRY(r = point3(m4 * point4(w)));  TEST_EQUAL(to_str(r), "[0,-0.5,0.866025]");

    TRY(m4 = rotate4(30_deg, 1));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[0.866025,0,0.5,0],"
        "[0,1,0,0],"
        "[-0.5,0,0.866025,0],"
        "[0,0,0,1]]"
    );
    TRY(r = point3(m4 * point4(u)));  TEST_EQUAL(to_str(r), "[0.866025,0,-0.5]");
    TRY(r = point3(m4 * point4(v)));  TEST_EQUAL(to_str(r), "[0,1,0]");
    TRY(r = point3(m4 * point4(w)));  TEST_EQUAL(to_str(r), "[0.5,0,0.866025]");

    TRY(m4 = rotate4(30_deg, 2));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[0.866025,-0.5,0,0],"
        "[0.5,0.866025,0,0],"
        "[0,0,1,0],"
        "[0,0,0,1]]"
    );
    TRY(r = point3(m4 * point4(u)));  TEST_EQUAL(to_str(r), "[0.866025,0.5,0]");
    TRY(r = point3(m4 * point4(v)));  TEST_EQUAL(to_str(r), "[-0.5,0.866025,0]");
    TRY(r = point3(m4 * point4(w)));  TEST_EQUAL(to_str(r), "[0,0,1]");

    TRY(m3 = rotate3(30_deg, zero));
    TEST_EQUAL(to_str(m3),
        "[[1,0,0],"
        "[0,1,0],"
        "[0,0,1]]"
    );
    TRY(m3 = rotate3(30_deg, u));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[1,0,0],"
        "[0,0.866025,-0.5],"
        "[0,0.5,0.866025]]"
    );
    TRY(m3 = rotate3(30_deg, v));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[0.866025,0,0.5],"
        "[0,1,0],"
        "[-0.5,0,0.866025]]"
    );
    TRY(m3 = rotate3(30_deg, w));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[0.866025,-0.5,0],"
        "[0.5,0.866025,0],"
        "[0,0,1]]"
    );

    TRY(m4 = rotate4(30_deg, zero));
    TEST_EQUAL(to_str(m4),
        "[[1,0,0,0],"
        "[0,1,0,0],"
        "[0,0,1,0],"
        "[0,0,0,1]]"
    );
    TRY(m4 = rotate4(30_deg, u));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[1,0,0,0],"
        "[0,0.866025,-0.5,0],"
        "[0,0.5,0.866025,0],"
        "[0,0,0,1]]"
    );
    TRY(m4 = rotate4(30_deg, v));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[0.866025,0,0.5,0],"
        "[0,1,0,0],"
        "[-0.5,0,0.866025,0],"
        "[0,0,0,1]]"
    );
    TRY(m4 = rotate4(30_deg, w));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[0.866025,-0.5,0,0],"
        "[0.5,0.866025,0,0],"
        "[0,0,1,0],"
        "[0,0,0,1]]"
    );

    TRY(m3 = scale3(42.0));
    TEST_EQUAL(to_str(m3),
        "[[42,0,0],"
        "[0,42,0],"
        "[0,0,42]]"
    );
    TRY(r = m3 * p);
    TEST_EQUAL(to_str(r), "[84,126,210]");

    TRY(m3 = scale3(Double3{10,20,30}));
    TEST_EQUAL(to_str(m3),
        "[[10,0,0],"
        "[0,20,0],"
        "[0,0,30]]"
    );
    TRY(r = m3 * p);
    TEST_EQUAL(to_str(r), "[20,60,150]");

    TRY(m4 = scale4(42.0));
    TEST_EQUAL(to_str(m4),
        "[[42,0,0,0],"
        "[0,42,0,0],"
        "[0,0,42,0],"
        "[0,0,0,1]]"
    );
    TRY(r = point3(m4 * point4(p)));
    TEST_EQUAL(to_str(r), "[84,126,210]");

    TRY(m4 = scale4(Double3{10,20,30}));
    TEST_EQUAL(to_str(m4),
        "[[10,0,0,0],"
        "[0,20,0,0],"
        "[0,0,30,0],"
        "[0,0,0,1]]"
    );
    TRY(r = point3(m4 * point4(p)));
    TEST_EQUAL(to_str(r), "[20,60,150]");

    TRY(m4 = translate4(Double3{10,20,30}));
    TEST_EQUAL(to_str(m4),
        "[[1,0,0,10],"
        "[0,1,0,20],"
        "[0,0,1,30],"
        "[0,0,0,1]]"
    );
    TRY(r = point3(m4 * point4(p)));
    TEST_EQUAL(to_str(r), "[12,23,35]");

}

void test_core_vector_quaternions() {

    const Double3 zero, u = {1,0,0}, v = {0,1,0}, w = {0,0,1}, unit = {1,1,1};

    Double3 r;
    Double3x3 m3;
    Double4x4 m4;
    Qdouble q;

    TRY(q = rotateq(120_deg, unit));
    TEST_EQUAL(to_str(q), "[0.5,0.5,0.5,0.5]");

    TRY(q = rotateq(120_deg, zero));
    TEST_EQUAL(to_str(q), "[1,0,0,0]");
    TRY(m3 = rotate3(q));
    TEST_EQUAL(to_str(m3),
        "[[1,0,0],"
        "[0,1,0],"
        "[0,0,1]]"
    );
    TRY(m4 = rotate4(q));
    TEST_EQUAL(to_str(m4),
        "[[1,0,0,0],"
        "[0,1,0,0],"
        "[0,0,1,0],"
        "[0,0,0,1]]"
    );

    TRY(q = rotateq(30_deg, u));
    TRY(m3 = rotate3(q));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[1,0,0],"
        "[0,0.866025,-0.5],"
        "[0,0.5,0.866025]]"
    );
    TRY(m4 = rotate4(q));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[1,0,0,0],"
        "[0,0.866025,-0.5,0],"
        "[0,0.5,0.866025,0],"
        "[0,0,0,1]]"
    );
    TRY(r = rotate(q, u));  TEST_EQUAL(to_str(r), "[1,0,0]");
    TRY(r = rotate(q, v));  TEST_EQUAL(to_str(r), "[0,0.866025,0.5]");
    TRY(r = rotate(q, w));  TEST_EQUAL(to_str(r), "[0,-0.5,0.866025]");

    TRY(q = rotateq(30_deg, v));
    TRY(m3 = rotate3(q));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[0.866025,0,0.5],"
        "[0,1,0],"
        "[-0.5,0,0.866025]]"
    );
    TRY(m4 = rotate4(q));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[0.866025,0,0.5,0],"
        "[0,1,0,0],"
        "[-0.5,0,0.866025,0],"
        "[0,0,0,1]]"
    );
    TRY(r = rotate(q, u));  TEST_EQUAL(to_str(r), "[0.866025,0,-0.5]");
    TRY(r = rotate(q, v));  TEST_EQUAL(to_str(r), "[0,1,0]");
    TRY(r = rotate(q, w));  TEST_EQUAL(to_str(r), "[0.5,0,0.866025]");

    TRY(q = rotateq(30_deg, w));
    TRY(m3 = rotate3(q));
    TRY(fuzz(m3));
    TEST_EQUAL(to_str(m3),
        "[[0.866025,-0.5,0],"
        "[0.5,0.866025,0],"
        "[0,0,1]]"
    );
    TRY(m4 = rotate4(q));
    TRY(fuzz(m4));
    TEST_EQUAL(to_str(m4),
        "[[0.866025,-0.5,0,0],"
        "[0.5,0.866025,0,0],"
        "[0,0,1,0],"
        "[0,0,0,1]]"
    );
    TRY(r = rotate(q, u));  TEST_EQUAL(to_str(r), "[0.866025,0.5,0]");
    TRY(r = rotate(q, v));  TEST_EQUAL(to_str(r), "[-0.5,0.866025,0]");
    TRY(r = rotate(q, w));  TEST_EQUAL(to_str(r), "[0,0,1]");

}
