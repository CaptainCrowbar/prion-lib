#include "rs-core/astronomy.hpp"
#include "rs-core/unit-test.hpp"
#include <stdexcept>

using namespace RS;
using namespace RS::Astro;

void test_core_astronomy_interpolation_map() {

    InterpolationTable<false, false> lin_lin;
    InterpolationTable<false, true> lin_log;
    InterpolationTable<true, false> log_lin;
    InterpolationTable<true, true> log_log;
    double y = 0;

    TRY(y = lin_lin(42));
    TEST_EQUAL(y, 0);

    TRY((lin_lin = {
        {1, 10},
    }));

    TRY(y = lin_lin(0));  TEST_EQUAL(y, 10);
    TRY(y = lin_lin(1));  TEST_EQUAL(y, 10);
    TRY(y = lin_lin(2));  TEST_EQUAL(y, 10);

    TRY((lin_lin = {
        {4, 20},
        {8, 40},
        {12, 80},
        {16, 200},
        {20, 0},
    }));

    TRY(y = lin_lin(0));   TEST_EQUAL(y, 0);
    TRY(y = lin_lin(1));   TEST_EQUAL(y, 5);
    TRY(y = lin_lin(2));   TEST_EQUAL(y, 10);
    TRY(y = lin_lin(3));   TEST_EQUAL(y, 15);
    TRY(y = lin_lin(4));   TEST_EQUAL(y, 20);
    TRY(y = lin_lin(5));   TEST_EQUAL(y, 25);
    TRY(y = lin_lin(6));   TEST_EQUAL(y, 30);
    TRY(y = lin_lin(7));   TEST_EQUAL(y, 35);
    TRY(y = lin_lin(8));   TEST_EQUAL(y, 40);
    TRY(y = lin_lin(9));   TEST_EQUAL(y, 50);
    TRY(y = lin_lin(10));  TEST_EQUAL(y, 60);
    TRY(y = lin_lin(11));  TEST_EQUAL(y, 70);
    TRY(y = lin_lin(12));  TEST_EQUAL(y, 80);
    TRY(y = lin_lin(13));  TEST_EQUAL(y, 110);
    TRY(y = lin_lin(14));  TEST_EQUAL(y, 140);
    TRY(y = lin_lin(15));  TEST_EQUAL(y, 170);
    TRY(y = lin_lin(16));  TEST_EQUAL(y, 200);
    TRY(y = lin_lin(17));  TEST_EQUAL(y, 150);
    TRY(y = lin_lin(18));  TEST_EQUAL(y, 100);
    TRY(y = lin_lin(19));  TEST_EQUAL(y, 50);
    TRY(y = lin_lin(20));  TEST_EQUAL(y, 0);
    TRY(y = lin_lin(21));  TEST_EQUAL(y, -50);
    TRY(y = lin_lin(22));  TEST_EQUAL(y, -100);
    TRY(y = lin_lin(23));  TEST_EQUAL(y, -150);
    TRY(y = lin_lin(24));  TEST_EQUAL(y, -200);

    TRY((lin_log = {
        {4, 20},
        {8, 30},
        {12, 50},
    }));

    TRY(y = lin_log(1));   TEST_NEAR(y, 14.755759);
    TRY(y = lin_log(2));   TEST_NEAR(y, 16.329932);
    TRY(y = lin_log(3));   TEST_NEAR(y, 18.072040);
    TRY(y = lin_log(4));   TEST_NEAR(y, 20.000000);
    TRY(y = lin_log(5));   TEST_NEAR(y, 22.133638);
    TRY(y = lin_log(6));   TEST_NEAR(y, 24.494897);
    TRY(y = lin_log(7));   TEST_NEAR(y, 27.108060);
    TRY(y = lin_log(8));   TEST_NEAR(y, 30.000000);
    TRY(y = lin_log(9));   TEST_NEAR(y, 34.086581);
    TRY(y = lin_log(10));  TEST_NEAR(y, 38.729833);
    TRY(y = lin_log(11));  TEST_NEAR(y, 44.005587);
    TRY(y = lin_log(12));  TEST_NEAR(y, 50.000000);
    TRY(y = lin_log(13));  TEST_NEAR(y, 56.810968);
    TRY(y = lin_log(14));  TEST_NEAR(y, 64.549722);
    TRY(y = lin_log(15));  TEST_NEAR(y, 73.342645);
    TRY(y = lin_log(16));  TEST_NEAR(y, 83.333333);

    TRY((log_lin = {
        {4, 20},
        {8, 30},
        {12, 50},
    }));

    TRY(y = log_lin(1));   TEST_NEAR(y, 0.000000);
    TRY(y = log_lin(2));   TEST_NEAR(y, 10.000000);
    TRY(y = log_lin(3));   TEST_NEAR(y, 15.849625);
    TRY(y = log_lin(4));   TEST_NEAR(y, 20.000000);
    TRY(y = log_lin(5));   TEST_NEAR(y, 23.219281);
    TRY(y = log_lin(6));   TEST_NEAR(y, 25.849625);
    TRY(y = log_lin(7));   TEST_NEAR(y, 28.073549);
    TRY(y = log_lin(8));   TEST_NEAR(y, 30.000000);
    TRY(y = log_lin(9));   TEST_NEAR(y, 35.809774);
    TRY(y = log_lin(10));  TEST_NEAR(y, 41.006794);
    TRY(y = log_lin(11));  TEST_NEAR(y, 45.708071);
    TRY(y = log_lin(12));  TEST_NEAR(y, 50.000000);
    TRY(y = log_lin(13));  TEST_NEAR(y, 53.948192);
    TRY(y = log_lin(14));  TEST_NEAR(y, 57.603647);
    TRY(y = log_lin(15));  TEST_NEAR(y, 61.006794);
    TRY(y = log_lin(16));  TEST_NEAR(y, 64.190226);

    TRY((log_log = {
        {4, 20},
        {8, 30},
        {12, 50},
    }));

    TRY(y = log_log(1));   TEST_NEAR(y, 8.888889);
    TRY(y = log_log(2));   TEST_NEAR(y, 13.333333);
    TRY(y = log_log(3));   TEST_NEAR(y, 16.902289);
    TRY(y = log_log(4));   TEST_NEAR(y, 20.000000);
    TRY(y = log_log(5));   TEST_NEAR(y, 22.788656);
    TRY(y = log_log(6));   TEST_NEAR(y, 25.353433);
    TRY(y = log_log(7));   TEST_NEAR(y, 27.745857);
    TRY(y = log_log(8));   TEST_NEAR(y, 30.000000);
    TRY(y = log_log(9));   TEST_NEAR(y, 34.798924);
    TRY(y = log_log(10));  TEST_NEAR(y, 39.738680);
    TRY(y = log_log(11));  TEST_NEAR(y, 44.808669);
    TRY(y = log_log(12));  TEST_NEAR(y, 50.000000);
    TRY(y = log_log(13));  TEST_NEAR(y, 55.305087);
    TRY(y = log_log(14));  TEST_NEAR(y, 60.717375);
    TRY(y = log_log(15));  TEST_NEAR(y, 66.231133);
    TRY(y = log_log(16));  TEST_NEAR(y, 71.841302);

}

void test_core_astronomy_physical_constants() {

    TEST_NEAR_RELATIVE  (atomic_mass_unit,           1.660'539'040e-27,   1e-10);
    TEST_NEAR_RELATIVE  (avogadro_constant,          6.022'140'76e23,     1e-10);
    TEST_NEAR_RELATIVE  (boltzmann_constant,         1.380'649e-23,       1e-10);
    TEST_NEAR_RELATIVE  (electron_mass,              9.109'383'56e-31,    1e-9);
    TEST_NEAR_RELATIVE  (elementary_charge,          1.602'176'634e-19,   1e-10);
    TEST_NEAR_RELATIVE  (faraday_constant,           96'485.332'89,       1e-6);
    TEST_NEAR_RELATIVE  (gravitational_constant,     6.674'08e-11,        1e-6);
    TEST_NEAR_RELATIVE  (josephson_constant,         4.835'978'525e14,    1e-6);
    TEST_NEAR_RELATIVE  (magnetic_constant,          1.256'637'061e-6,    1e-10);
    TEST_NEAR_RELATIVE  (molar_gas_constant,         8.314'459'8,         1e-6);
    TEST_NEAR_RELATIVE  (molar_mass_constant,        0.001,               1e-7);
    TEST_NEAR_RELATIVE  (planck_constant,            6.626'070'15e-34,    1e-10);
    TEST_NEAR_RELATIVE  (rydberg_constant,           10'973'731.568'508,  1e-10);
    TEST_EQUAL          (speed_of_light,             299'792'458);        // integer
    TEST_NEAR_RELATIVE  (stefan_boltzmann_constant,  5.670'367e-8,        1e-5);
    TEST_NEAR_RELATIVE  (vacuum_impedance,           376.730'313'461'77,  1e-10);
    TEST_NEAR_RELATIVE  (vacuum_permittivity,        8.854'187'817e-12,   1e-10);
    TEST_NEAR_RELATIVE  (von_klitzing_constant,      25'812.807'455'5,    1e-9);

}

void test_core_astronomy_astronomical_constants() {

    TEST_NEAR_RELATIVE  (sidereal_day,                    86'164.090'53,        1e-10);
    TEST_EQUAL          (julian_day,                      86'400);              // integer
    TEST_NEAR_RELATIVE  (tropical_year,                   31'556'925.2,         1e-9);
    TEST_EQUAL          (julian_year,                     31'557'600);          // integer
    TEST_NEAR_RELATIVE  (sidereal_year,                   31'558'149.8,         1e-9);
    TEST_EQUAL          (earth_radius,                    6'371'000);           // integer
    TEST_EQUAL          (jupiter_radius,                  69'911'000);          // integer
    TEST_EQUAL          (solar_radius,                    695'700'000);         // integer
    TEST_NEAR_RELATIVE  (astronomical_unit,               1.495'978'707e11,     1e-10);
    TEST_NEAR_RELATIVE  (light_year,                      9.460'730'473e15,     1e-10);
    TEST_NEAR_RELATIVE  (parsec,                          3.085'677'581'49e16,  1e-10);
    TEST_NEAR_RELATIVE  (earth_mass,                      5.972'4e24,           1e-5);
    TEST_NEAR_RELATIVE  (jupiter_mass,                    1.898'2e27,           1e-5);
    TEST_NEAR_RELATIVE  (solar_mass,                      1.988'48e30,          1e-6);
    TEST_NEAR_RELATIVE  (earth_gravity,                   9.806'65,             1e-6);
    TEST_NEAR_RELATIVE  (bolometric_luminosity_constant,  3.012'8e28,           1e-5);
    TEST_NEAR_RELATIVE  (solar_luminosity,                3.828e26,             1e-4);
    TEST_EQUAL          (solar_temperature,               5772);                // integer

}

void test_core_astronomy_coordinate_systems() {

    int ideg = 0, ihrs = 0, imin = 0;
    double rad = 0, deg = 0, hrs = 0, min = 0, sec = 0;
    Ustring text;

    TRY(rad = degrees_to_radians(10));             TEST_NEAR(rad, 0.174533);
    TRY(rad = degrees_to_radians(-10));            TEST_NEAR(rad, -0.174533);
    TRY(rad = degrees_to_radians(10, 20));         TEST_NEAR(rad, 0.180351);
    TRY(rad = degrees_to_radians(-10, -20));       TEST_NEAR(rad, -0.180351);
    TRY(rad = degrees_to_radians(10, 20, 30));     TEST_NEAR(rad, 0.180496);
    TRY(rad = degrees_to_radians(-10, -20, -30));  TEST_NEAR(rad, -0.180496);
    TRY(rad = hours_to_radians(10));               TEST_NEAR(rad, 2.617994);
    TRY(rad = hours_to_radians(-10));              TEST_NEAR(rad, -2.617994);
    TRY(rad = hours_to_radians(10, 20));           TEST_NEAR(rad, 2.705260);
    TRY(rad = hours_to_radians(-10, -20));         TEST_NEAR(rad, -2.705260);
    TRY(rad = hours_to_radians(10, 20, 30));       TEST_NEAR(rad, 2.707442);
    TRY(rad = hours_to_radians(-10, -20, -30));    TEST_NEAR(rad, -2.707442);
    TRY(deg = radians_to_degrees(1));              TEST_NEAR(deg, 57.295780);
    TRY(deg = radians_to_degrees(-1));             TEST_NEAR(deg, -57.295780);
    TRY(radians_to_degrees(1, ideg, min));         TEST_EQUAL(ideg, 57); TEST_NEAR(min, 17.746771);
    TRY(radians_to_degrees(-1, ideg, min));        TEST_EQUAL(ideg, -57); TEST_NEAR(min, -17.746771);
    TRY(radians_to_degrees(1, ideg, imin, sec));   TEST_EQUAL(ideg, 57); TEST_EQUAL(imin, 17); TEST_NEAR(sec, 44.806247);
    TRY(radians_to_degrees(-1, ideg, imin, sec));  TEST_EQUAL(ideg, -57); TEST_EQUAL(imin, -17); TEST_NEAR(sec, -44.806247);
    TRY(hrs = radians_to_hours(1));                TEST_NEAR(hrs, 3.819719);
    TRY(hrs = radians_to_hours(-1));               TEST_NEAR(hrs, -3.819719);
    TRY(radians_to_hours(1, ihrs, min));           TEST_EQUAL(ihrs, 3); TEST_NEAR(min, 49.183118);
    TRY(radians_to_hours(-1, ihrs, min));          TEST_EQUAL(ihrs, -3); TEST_NEAR(min, -49.183118);
    TRY(radians_to_hours(1, ihrs, imin, sec));     TEST_EQUAL(ihrs, 3); TEST_EQUAL(imin, 49); TEST_NEAR(sec, 10.987083);
    TRY(radians_to_hours(-1, ihrs, imin, sec));    TEST_EQUAL(ihrs, -3); TEST_EQUAL(imin, -49); TEST_NEAR(sec, -10.987083);

    rad = 0;                                        TRY(text = format_angle(rad));                    TEST_EQUAL(text, "0 00 00");
    rad = 0;                                        TRY(text = format_angle(rad, "dms2"));            TEST_EQUAL(text, "0 00 00.00");
    rad = 0;                                        TRY(text = format_angle(rad, "dms2p"));           TEST_EQUAL(text, "000 00 00.00");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad));                    TEST_EQUAL(text, "123 45 12");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "dms2"));            TEST_EQUAL(text, "123 45 12.34");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "dms2p"));           TEST_EQUAL(text, "123 45 12.34");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, "dms2"));            TEST_EQUAL(text, "1 23 45.67");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, "dms2p"));           TEST_EQUAL(text, "001 23 45.67");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, "dm4"));             TEST_EQUAL(text, "1 23.7612");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, "d6"));              TEST_EQUAL(text, "1.396019");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dms2,˚,\',\""));  TEST_EQUAL(text, u8"1˚23\'45.67\"");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dms2s"));         TEST_EQUAL(text, u8"1˚23\'45.67\"");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dms2is"));        TEST_EQUAL(text, u8"1˚23\'45\".67");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dms2ips"));       TEST_EQUAL(text, u8"001˚23\'45\".67");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dm2s"));          TEST_EQUAL(text, u8"1˚23.76\'");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dm2ips"));        TEST_EQUAL(text, u8"001˚23\'.76");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"d2s"));           TEST_EQUAL(text, u8"1.40˚");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"d2ips"));         TEST_EQUAL(text, u8"001˚.40");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "hms2"));            TEST_EQUAL(text, "8 15 00.82");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "hms2p"));           TEST_EQUAL(text, "08 15 00.82");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "hms2,h,m,s"));      TEST_EQUAL(text, "8h15m00.82s");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "hms2s"));           TEST_EQUAL(text, "8h15m00.82s");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "hms2ps"));          TEST_EQUAL(text, "08h15m00.82s");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "hms2is"));          TEST_EQUAL(text, "8h15m00s.82");
    TRY(rad = degrees_to_radians(123, 45, 12.34));  TRY(text = format_angle(rad, "hms2ips"));         TEST_EQUAL(text, "08h15m00s.82");
    TRY(rad = degrees_to_radians(0, 0, 0.9));       TRY(text = format_angle(rad));                    TEST_EQUAL(text, "0 00 01");
    TRY(rad = degrees_to_radians(0, 0, 59.9));      TRY(text = format_angle(rad));                    TEST_EQUAL(text, "0 01 00");
    TRY(rad = degrees_to_radians(0, 59, 59.9));     TRY(text = format_angle(rad));                    TEST_EQUAL(text, "1 00 00");
    TRY(rad = degrees_to_radians(0, 0, 31));        TRY(text = format_angle(rad, "dm"));              TEST_EQUAL(text, "0 01");
    TRY(rad = degrees_to_radians(0, 59, 31));       TRY(text = format_angle(rad, "dm"));              TEST_EQUAL(text, "1 00");
    TRY(rad = degrees_to_radians(0, 31));           TRY(text = format_angle(rad, "d"));               TEST_EQUAL(text, "1");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, "dms+2"));           TEST_EQUAL(text, "+1 23 45.67");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dms+2s"));        TEST_EQUAL(text, u8"+1˚23\'45.67\"");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dms+2is"));       TEST_EQUAL(text, u8"+1˚23\'45\".67");
    TRY(rad = degrees_to_radians(1, 23, 45.67));    TRY(text = format_angle(rad, u8"dms+2ips"));      TEST_EQUAL(text, u8"+001˚23\'45\".67");
    TRY(rad = - degrees_to_radians(1, 23, 45.67));  TRY(text = format_angle(rad, "dms2"));            TEST_EQUAL(text, "-1 23 45.67");
    TRY(rad = - degrees_to_radians(1, 23, 45.67));  TRY(text = format_angle(rad, u8"dms2s"));         TEST_EQUAL(text, u8"-1˚23\'45.67\"");
    TRY(rad = - degrees_to_radians(1, 23, 45.67));  TRY(text = format_angle(rad, u8"dms2is"));        TEST_EQUAL(text, u8"-1˚23\'45\".67");
    TRY(rad = - degrees_to_radians(1, 23, 45.67));  TRY(text = format_angle(rad, u8"dms2ips"));       TEST_EQUAL(text, u8"-001˚23\'45\".67");

    TEST_THROW(format_angle(0, "2"), std::invalid_argument);
    TEST_THROW(format_angle(0, "dms2+"), std::invalid_argument);
    TEST_THROW(format_angle(0, "DMS"), std::invalid_argument);

    TRY(rad = parse_angle("1 23 45.67"));           TEST_NEAR(rad, 0.024365);
    TRY(rad = parse_angle("1 23.45"));              TEST_NEAR(rad, 0.024275);
    TRY(rad = parse_angle("1.23"));                 TEST_NEAR(rad, 0.021468);
    TRY(rad = parse_angle("1 23 45.67", "dms"));    TEST_NEAR(rad, 0.024365);
    TRY(rad = parse_angle("1 23.45", "dms"));       TEST_NEAR(rad, 0.024275);
    TRY(rad = parse_angle("1.23", "dms"));          TEST_NEAR(rad, 0.021468);
    TRY(rad = parse_angle("1 23 45.67", "hms"));    TEST_NEAR(rad, 0.365477);
    TRY(rad = parse_angle("1 23.45", "hms"));       TEST_NEAR(rad, 0.364119);
    TRY(rad = parse_angle("1.23", "hms"));          TEST_NEAR(rad, 0.322013);
    TRY(rad = parse_angle(u8"1˚ 23\' 45\".67"));    TEST_NEAR(rad, 0.024365);
    TRY(rad = parse_angle(u8"1˚ 23\'.45"));         TEST_NEAR(rad, 0.024275);
    TRY(rad = parse_angle(u8"1˚.23"));              TEST_NEAR(rad, 0.021468);
    TRY(rad = parse_angle(u8"1˚ 23\' 45.67\""));    TEST_NEAR(rad, 0.024365);
    TRY(rad = parse_angle(u8"1˚ 23.45\'"));         TEST_NEAR(rad, 0.024275);
    TRY(rad = parse_angle(u8"1.23˚"));              TEST_NEAR(rad, 0.021468);
    TRY(rad = parse_angle("1h 23m 45s.67"));        TEST_NEAR(rad, 0.365477);
    TRY(rad = parse_angle("1h 23m.45"));            TEST_NEAR(rad, 0.364119);
    TRY(rad = parse_angle("1h.23"));                TEST_NEAR(rad, 0.322013);
    TRY(rad = parse_angle(u8"- 1˚ 23\' 45\".67"));  TEST_NEAR(rad, -0.024365);
    TRY(rad = parse_angle(u8"- 1˚ 23\'.45"));       TEST_NEAR(rad, -0.024275);
    TRY(rad = parse_angle(u8"- 1˚.23"));            TEST_NEAR(rad, -0.021468);

    TEST_THROW(parse_angle(""), std::invalid_argument);
    TEST_THROW(parse_angle("12 34 56 78"), std::invalid_argument);
    TEST_THROW(parse_angle("12h 34m 56s", "dms"), std::invalid_argument);
    TEST_THROW(parse_angle("12d 34m 56s", "hms"), std::invalid_argument);

}

void test_core_astronomy_solar_system_data() {

    TEST_NEAR_RELATIVE(Sun.mass,         solar_mass,         1e-10);
    TEST_NEAR_RELATIVE(Sun.luminosity,   solar_luminosity,   1e-10);
    TEST_NEAR_RELATIVE(Sun.radius,       solar_radius,       1e-10);
    TEST_NEAR_RELATIVE(Sun.temperature,  solar_temperature,  1e-10);
    TEST_NEAR_EPSILON(Sun.V,             -26.75,             0.005);
    TEST_NEAR_EPSILON(Sun.M_U,           5.66,               0.005);
    TEST_NEAR_EPSILON(Sun.M_B,           5.47,               0.005);
    TEST_NEAR_EPSILON(Sun.M_V,           4.82,               0.005);
    TEST_NEAR_EPSILON(Sun.M_R,           4.28,               0.005);
    TEST_NEAR_EPSILON(Sun.M_I,           3.94,               0.005);
    TEST_NEAR_EPSILON(Sun.M_bol,         4.74,               0.005);
    TEST_NEAR_EPSILON(Sun.BC,            -0.08,              0.005);
    TEST_NEAR_EPSILON(Sun.B_V,           0.65,               0.005);

    TEST_EQUAL(planets[Mercury].name,  "Mercury");
    TEST_EQUAL(planets[Venus].name,    "Venus");
    TEST_EQUAL(planets[Earth].name,    "Earth");
    TEST_EQUAL(planets[Mars].name,     "Mars");
    TEST_EQUAL(planets[Jupiter].name,  "Jupiter");
    TEST_EQUAL(planets[Saturn].name,   "Saturn");
    TEST_EQUAL(planets[Uranus].name,   "Uranus");
    TEST_EQUAL(planets[Neptune].name,  "Neptune");

    TEST_NEAR_RELATIVE(planets[Mercury].distance,  5.79090e10,  1e-5);
    TEST_NEAR_RELATIVE(planets[Venus].distance,    1.08209e11,  1e-5);
    TEST_NEAR_RELATIVE(planets[Earth].distance,    1.49598e11,  1e-5);
    TEST_NEAR_RELATIVE(planets[Mars].distance,     2.27939e11,  1e-5);
    TEST_NEAR_RELATIVE(planets[Jupiter].distance,  7.78567e11,  1e-5);
    TEST_NEAR_RELATIVE(planets[Saturn].distance,   1.43354e12,  1e-5);
    TEST_NEAR_RELATIVE(planets[Uranus].distance,   2.87503e12,  1e-5);
    TEST_NEAR_RELATIVE(planets[Neptune].distance,  4.504e12,    1e-3);

    TEST_NEAR_RELATIVE(planets[Earth].distance,           1.49598e11,  1e-5);
    TEST_NEAR_RELATIVE(planets[Earth].period,             3.15581e7,   1e-5);
    TEST_NEAR_RELATIVE(planets[Earth].eccentriciy,        0.0167086,   1e-5);
    TEST_NEAR_RELATIVE(planets[Earth].inclination,        8.7e-7,      0.1);
    TEST_NEAR_RELATIVE(planets[Earth].mass,               5.97237e24,  1e-5);
    TEST_NEAR_RELATIVE(planets[Earth].radius,             6.3710e6,    1e-4);
    TEST_NEAR_RELATIVE(planets[Earth].equatorial_radius,  6.3781e6,    1e-4);
    TEST_NEAR_RELATIVE(planets[Earth].polar_radius,       6.3568e6,    1e-4);
    TEST_NEAR_RELATIVE(planets[Earth].density,            5514,        1e-3);
    TEST_NEAR_RELATIVE(planets[Earth].rotation,           86164.1,     1e-5);
    TEST_NEAR_RELATIVE(planets[Earth].axial_inclination,  0.409093,    1e-5);

    for (auto& sat: satellites) {

        if (sat.name == "Moon") {

            TEST_EQUAL(sat.planet,      3);
            TEST_EQUAL(sat.satellite,   1);
            TEST_EQUAL(sat.label,       "I");
            TEST_EQUAL(sat.name,        "Moon");
            TEST_NEAR(sat.distance,     384399000);
            TEST_NEAR(sat.period,       2360591.5104);
            TEST_NEAR(sat.ecc,          0.0549);
            TEST_NEAR(sat.incl,         0.089797);
            TEST_NEAR(sat.mass / 1e18,  73420);
            TEST_NEAR(sat.radius,       1737100);
            TEST_NEAR(sat.density,      3344);

        }

        if (sat.name == "Io") {

            TEST_EQUAL(sat.planet,      5);
            TEST_EQUAL(sat.satellite,   5);
            TEST_EQUAL(sat.label,       "I");
            TEST_EQUAL(sat.name,        "Io");
            TEST_NEAR(sat.distance,     421700000);
            TEST_NEAR(sat.period,       152850.24);
            TEST_NEAR(sat.ecc,          0.0041);
            TEST_NEAR(sat.incl,         0.000873);
            TEST_NEAR(sat.mass / 1e18,  89319);
            TEST_NEAR(sat.radius,       1821300);
            TEST_NEAR(sat.density,      3529);

        }

        if (sat.name == "Europa") {

            TEST_EQUAL(sat.planet,      5);
            TEST_EQUAL(sat.satellite,   6);
            TEST_EQUAL(sat.label,       "II");
            TEST_EQUAL(sat.name,        "Europa");
            TEST_NEAR(sat.distance,     671034000);
            TEST_NEAR(sat.period,       306823.68);
            TEST_NEAR(sat.ecc,          0.0094);
            TEST_NEAR(sat.incl,         0.008221);
            TEST_NEAR(sat.mass / 1e18,  48000);
            TEST_NEAR(sat.radius,       1560800);
            TEST_NEAR(sat.density,      3014);

        }

        if (sat.name == "Ganymede") {

            TEST_EQUAL(sat.planet,      5);
            TEST_EQUAL(sat.satellite,   7);
            TEST_EQUAL(sat.label,       "III");
            TEST_EQUAL(sat.name,        "Ganymede");
            TEST_NEAR(sat.distance,     1070412000);
            TEST_NEAR(sat.period,       618157.44);
            TEST_NEAR(sat.ecc,          0.0011);
            TEST_NEAR(sat.incl,         0.003560);
            TEST_NEAR(sat.mass / 1e18,  148190);
            TEST_NEAR(sat.radius,       2631200);
            TEST_NEAR(sat.density,      1942);

        }

        if (sat.name == "Callisto") {

            TEST_EQUAL(sat.planet,      5);
            TEST_EQUAL(sat.satellite,   8);
            TEST_EQUAL(sat.label,       "IV");
            TEST_EQUAL(sat.name,        "Callisto");
            TEST_NEAR(sat.distance,     1882709000);
            TEST_NEAR(sat.period,       1441929.6);
            TEST_NEAR(sat.ecc,          0.0074);
            TEST_NEAR(sat.incl,         0.003578);
            TEST_NEAR(sat.mass / 1e18,  107590);
            TEST_NEAR(sat.radius,       2410300);
            TEST_NEAR(sat.density,      1834);

        }

        if (sat.name == "Titan") {

            TEST_EQUAL(sat.planet,      6);
            TEST_EQUAL(sat.satellite,   22);
            TEST_EQUAL(sat.label,       "VI");
            TEST_EQUAL(sat.name,        "Titan");
            TEST_NEAR(sat.distance,     1221930000);
            TEST_NEAR(sat.period,       1377684.288);
            TEST_NEAR(sat.ecc,          0.0288);
            TEST_NEAR(sat.incl,         0.006082);
            TEST_NEAR(sat.mass / 1e18,  134520);
            TEST_NEAR(sat.radius,       2574730);
            TEST_NEAR(sat.density,      1881);

        }

        if (sat.name == "Triton") {

            TEST_EQUAL(sat.planet,      8);
            TEST_EQUAL(sat.satellite,   8);
            TEST_EQUAL(sat.label,       "I");
            TEST_EQUAL(sat.name,        "Triton");
            TEST_NEAR(sat.distance,     354759000);
            TEST_NEAR(sat.period,       507772.8);
            TEST_NEAR(sat.ecc,          0.0000);
            TEST_NEAR(sat.incl,         2.737811);
            TEST_NEAR(sat.mass / 1e18,  21408);
            TEST_NEAR(sat.radius,       1352600);
            TEST_NEAR(sat.density,      2065);

        }
    }

}

void test_core_astronomy_stellar_property_relations() {

    double x = 0;

    TRY(x = magnitude_to_ratio(-5.0));                          TEST_NEAR_RELATIVE(x, 100, 1e-10);
    TRY(x = magnitude_to_ratio(-2.5));                          TEST_NEAR_RELATIVE(x, 10, 1e-10);
    TRY(x = magnitude_to_ratio(0.0));                           TEST_NEAR_RELATIVE(x, 1, 1e-10);
    TRY(x = magnitude_to_ratio(2.5));                           TEST_NEAR_RELATIVE(x, 0.1, 1e-10);
    TRY(x = magnitude_to_ratio(5.0));                           TEST_NEAR_RELATIVE(x, 0.01, 1e-10);
    TRY(x = ratio_to_magnitude(100));                           TEST_NEAR(x, -5.0);
    TRY(x = ratio_to_magnitude(10));                            TEST_NEAR(x, -2.5);
    TRY(x = ratio_to_magnitude(1));                             TEST_NEAR(x, 0.0);
    TRY(x = ratio_to_magnitude(0.1));                           TEST_NEAR(x, 2.5);
    TRY(x = ratio_to_magnitude(0.01));                          TEST_NEAR(x, 5.0);
    TRY(x = magnitude_to_luminosity(-0.26));                    TEST_NEAR_RELATIVE(x, 100, 1e-10);
    TRY(x = magnitude_to_luminosity(2.24));                     TEST_NEAR_RELATIVE(x, 10, 1e-10);
    TRY(x = magnitude_to_luminosity(4.74));                     TEST_NEAR_RELATIVE(x, 1, 1e-10);
    TRY(x = magnitude_to_luminosity(7.24));                     TEST_NEAR_RELATIVE(x, 0.1, 1e-10);
    TRY(x = magnitude_to_luminosity(9.74));                     TEST_NEAR_RELATIVE(x, 0.01, 1e-10);
    TRY(x = luminosity_to_magnitude(100));                      TEST_NEAR(x, -0.26);
    TRY(x = luminosity_to_magnitude(10));                       TEST_NEAR(x, 2.24);
    TRY(x = luminosity_to_magnitude(1));                        TEST_NEAR(x, 4.74);
    TRY(x = luminosity_to_magnitude(0.1));                      TEST_NEAR(x, 7.24);
    TRY(x = luminosity_to_magnitude(0.01));                     TEST_NEAR(x, 9.74);
    TRY(x = magnitude_to_visual_luminosity(-0.18));             TEST_NEAR_RELATIVE(x, 100, 1e-10);
    TRY(x = magnitude_to_visual_luminosity(2.32));              TEST_NEAR_RELATIVE(x, 10, 1e-10);
    TRY(x = magnitude_to_visual_luminosity(4.82));              TEST_NEAR_RELATIVE(x, 1, 1e-10);
    TRY(x = magnitude_to_visual_luminosity(7.32));              TEST_NEAR_RELATIVE(x, 0.1, 1e-10);
    TRY(x = magnitude_to_visual_luminosity(9.82));              TEST_NEAR_RELATIVE(x, 0.01, 1e-10);
    TRY(x = visual_luminosity_to_magnitude(100));               TEST_NEAR(x, -0.18);
    TRY(x = visual_luminosity_to_magnitude(10));                TEST_NEAR(x, 2.32);
    TRY(x = visual_luminosity_to_magnitude(1));                 TEST_NEAR(x, 4.82);
    TRY(x = visual_luminosity_to_magnitude(0.1));               TEST_NEAR(x, 7.32);
    TRY(x = visual_luminosity_to_magnitude(0.01));              TEST_NEAR(x, 9.82);
    TRY(x = solar_luminosity_radius_to_temperature(1, 1));      TEST_NEAR_RELATIVE(x, 5772, 1e-4);
    TRY(x = solar_luminosity_radius_to_temperature(2, 1));      TEST_NEAR_RELATIVE(x, 6864.10, 1e-4);
    TRY(x = solar_luminosity_radius_to_temperature(1, 2));      TEST_NEAR_RELATIVE(x, 4081.42, 1e-4);
    TRY(x = solar_luminosity_temperature_to_radius(1, 5772));   TEST_NEAR_RELATIVE(x, 1, 1e-4);
    TRY(x = solar_luminosity_temperature_to_radius(2, 5772));   TEST_NEAR_RELATIVE(x, 1.41421, 1e-4);
    TRY(x = solar_luminosity_temperature_to_radius(1, 11544));  TEST_NEAR_RELATIVE(x, 0.25, 1e-4);
    TRY(x = solar_radius_temperature_to_luminosity(1, 5772));   TEST_NEAR_RELATIVE(x, 1, 1e-4);
    TRY(x = solar_radius_temperature_to_luminosity(2, 5772));   TEST_NEAR_RELATIVE(x, 4, 1e-4);
    TRY(x = solar_radius_temperature_to_luminosity(1, 11544));  TEST_NEAR_RELATIVE(x, 16, 1e-4);

}

void test_core_astronomy_spectral_classification() {

    Spectrum spc;
    Ustring str;

    TEST_EQUAL(sizeof(Spectrum), 3);

    TRY((spc = {LC::V, Sp::O, 1}));    TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::O);  TEST_EQUAL(spc.sub, 1);  TEST_EQUAL(spc.index(), 1);   TRY(str = to_str(spc));  TEST_EQUAL(str, "O1 V");
    TRY((spc = {LC::V, Sp::B, 2}));    TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::B);  TEST_EQUAL(spc.sub, 2);  TEST_EQUAL(spc.index(), 12);  TRY(str = to_str(spc));  TEST_EQUAL(str, "B2 V");
    TRY((spc = {LC::V, Sp::A, 3}));    TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::A);  TEST_EQUAL(spc.sub, 3);  TEST_EQUAL(spc.index(), 23);  TRY(str = to_str(spc));  TEST_EQUAL(str, "A3 V");
    TRY((spc = {LC::V, Sp::F, 4}));    TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::F);  TEST_EQUAL(spc.sub, 4);  TEST_EQUAL(spc.index(), 34);  TRY(str = to_str(spc));  TEST_EQUAL(str, "F4 V");
    TRY((spc = {LC::V, Sp::G, 5}));    TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);  TEST_EQUAL(spc.index(), 45);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G5 V");
    TRY((spc = {LC::V, Sp::K, 6}));    TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 6);  TEST_EQUAL(spc.index(), 56);  TRY(str = to_str(spc));  TEST_EQUAL(str, "K6 V");
    TRY((spc = {LC::V, Sp::M, 7}));    TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::M);  TEST_EQUAL(spc.sub, 7);  TEST_EQUAL(spc.index(), 67);  TRY(str = to_str(spc));  TEST_EQUAL(str, "M7 V");
    TRY((spc = {LC::Ia, Sp::G, 1}));   TEST_EQUAL(spc.lc, LC::Ia);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 1);  TEST_EQUAL(spc.index(), 41);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G1 Ia");
    TRY((spc = {LC::Ib, Sp::G, 2}));   TEST_EQUAL(spc.lc, LC::Ib);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 2);  TEST_EQUAL(spc.index(), 42);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G2 Ib");
    TRY((spc = {LC::II, Sp::G, 3}));   TEST_EQUAL(spc.lc, LC::II);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 3);  TEST_EQUAL(spc.index(), 43);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G3 II");
    TRY((spc = {LC::III, Sp::G, 4}));  TEST_EQUAL(spc.lc, LC::III);  TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 4);  TEST_EQUAL(spc.index(), 44);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G4 III");
    TRY((spc = {LC::IV, Sp::G, 5}));   TEST_EQUAL(spc.lc, LC::IV);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);  TEST_EQUAL(spc.index(), 45);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G5 IV");
    TRY((spc = {LC::VI, Sp::G, 6}));   TEST_EQUAL(spc.lc, LC::VI);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 6);  TEST_EQUAL(spc.index(), 46);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G6 VI");
    TRY((spc = {LC::V, 1}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::O);  TEST_EQUAL(spc.sub, 1);  TEST_EQUAL(spc.index(), 1);   TRY(str = to_str(spc));  TEST_EQUAL(str, "O1 V");
    TRY((spc = {LC::V, 12}));          TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::B);  TEST_EQUAL(spc.sub, 2);  TEST_EQUAL(spc.index(), 12);  TRY(str = to_str(spc));  TEST_EQUAL(str, "B2 V");
    TRY((spc = {LC::V, 23}));          TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::A);  TEST_EQUAL(spc.sub, 3);  TEST_EQUAL(spc.index(), 23);  TRY(str = to_str(spc));  TEST_EQUAL(str, "A3 V");
    TRY((spc = {LC::V, 34}));          TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::F);  TEST_EQUAL(spc.sub, 4);  TEST_EQUAL(spc.index(), 34);  TRY(str = to_str(spc));  TEST_EQUAL(str, "F4 V");
    TRY((spc = {LC::V, 45}));          TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);  TEST_EQUAL(spc.index(), 45);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G5 V");
    TRY((spc = {LC::V, 56}));          TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 6);  TEST_EQUAL(spc.index(), 56);  TRY(str = to_str(spc));  TEST_EQUAL(str, "K6 V");
    TRY((spc = {LC::V, 67}));          TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::M);  TEST_EQUAL(spc.sub, 7);  TEST_EQUAL(spc.index(), 67);  TRY(str = to_str(spc));  TEST_EQUAL(str, "M7 V");
    TRY((spc = {LC::Ia, 41}));         TEST_EQUAL(spc.lc, LC::Ia);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 1);  TEST_EQUAL(spc.index(), 41);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G1 Ia");
    TRY((spc = {LC::Ib, 42}));         TEST_EQUAL(spc.lc, LC::Ib);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 2);  TEST_EQUAL(spc.index(), 42);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G2 Ib");
    TRY((spc = {LC::II, 43}));         TEST_EQUAL(spc.lc, LC::II);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 3);  TEST_EQUAL(spc.index(), 43);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G3 II");
    TRY((spc = {LC::III, 44}));        TEST_EQUAL(spc.lc, LC::III);  TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 4);  TEST_EQUAL(spc.index(), 44);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G4 III");
    TRY((spc = {LC::IV, 45}));         TEST_EQUAL(spc.lc, LC::IV);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);  TEST_EQUAL(spc.index(), 45);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G5 IV");
    TRY((spc = {LC::VI, 46}));         TEST_EQUAL(spc.lc, LC::VI);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 6);  TEST_EQUAL(spc.index(), 46);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G6 VI");
    TRY((spc = {Sp::O, 1}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::O);  TEST_EQUAL(spc.sub, 1);  TEST_EQUAL(spc.index(), 1);   TRY(str = to_str(spc));  TEST_EQUAL(str, "O1 V");
    TRY((spc = {Sp::B, 2}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::B);  TEST_EQUAL(spc.sub, 2);  TEST_EQUAL(spc.index(), 12);  TRY(str = to_str(spc));  TEST_EQUAL(str, "B2 V");
    TRY((spc = {Sp::A, 3}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::A);  TEST_EQUAL(spc.sub, 3);  TEST_EQUAL(spc.index(), 23);  TRY(str = to_str(spc));  TEST_EQUAL(str, "A3 V");
    TRY((spc = {Sp::F, 4}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::F);  TEST_EQUAL(spc.sub, 4);  TEST_EQUAL(spc.index(), 34);  TRY(str = to_str(spc));  TEST_EQUAL(str, "F4 V");
    TRY((spc = {Sp::G, 5}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);  TEST_EQUAL(spc.index(), 45);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G5 V");
    TRY((spc = {Sp::K, 6}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 6);  TEST_EQUAL(spc.index(), 56);  TRY(str = to_str(spc));  TEST_EQUAL(str, "K6 V");
    TRY((spc = {Sp::M, 7}));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::M);  TEST_EQUAL(spc.sub, 7);  TEST_EQUAL(spc.index(), 67);  TRY(str = to_str(spc));  TEST_EQUAL(str, "M7 V");
    TRY(spc = Spectrum(1));            TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::O);  TEST_EQUAL(spc.sub, 1);  TEST_EQUAL(spc.index(), 1);   TRY(str = to_str(spc));  TEST_EQUAL(str, "O1 V");
    TRY(spc = Spectrum(12));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::B);  TEST_EQUAL(spc.sub, 2);  TEST_EQUAL(spc.index(), 12);  TRY(str = to_str(spc));  TEST_EQUAL(str, "B2 V");
    TRY(spc = Spectrum(23));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::A);  TEST_EQUAL(spc.sub, 3);  TEST_EQUAL(spc.index(), 23);  TRY(str = to_str(spc));  TEST_EQUAL(str, "A3 V");
    TRY(spc = Spectrum(34));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::F);  TEST_EQUAL(spc.sub, 4);  TEST_EQUAL(spc.index(), 34);  TRY(str = to_str(spc));  TEST_EQUAL(str, "F4 V");
    TRY(spc = Spectrum(45));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);  TEST_EQUAL(spc.index(), 45);  TRY(str = to_str(spc));  TEST_EQUAL(str, "G5 V");
    TRY(spc = Spectrum(56));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 6);  TEST_EQUAL(spc.index(), 56);  TRY(str = to_str(spc));  TEST_EQUAL(str, "K6 V");
    TRY(spc = Spectrum(67));           TEST_EQUAL(spc.lc, LC::V);    TEST_EQUAL(spc.sp, Sp::M);  TEST_EQUAL(spc.sub, 7);  TEST_EQUAL(spc.index(), 67);  TRY(str = to_str(spc));  TEST_EQUAL(str, "M7 V");

    TEST(spc.try_parse("O1"));      TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::O);  TEST_EQUAL(spc.sub, 1);
    TEST(spc.try_parse("B2"));      TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::B);  TEST_EQUAL(spc.sub, 2);
    TEST(spc.try_parse("A3"));      TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::A);  TEST_EQUAL(spc.sub, 3);
    TEST(spc.try_parse("F4"));      TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::F);  TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("G5"));      TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);
    TEST(spc.try_parse("K6"));      TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 6);
    TEST(spc.try_parse("M7"));      TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::M);  TEST_EQUAL(spc.sub, 7);
    TEST(spc.try_parse("O1 V"));    TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::O);  TEST_EQUAL(spc.sub, 1);
    TEST(spc.try_parse("B2 V"));    TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::B);  TEST_EQUAL(spc.sub, 2);
    TEST(spc.try_parse("A3 V"));    TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::A);  TEST_EQUAL(spc.sub, 3);
    TEST(spc.try_parse("F4 V"));    TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::F);  TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("G5 V"));    TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);
    TEST(spc.try_parse("K6 V"));    TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 6);
    TEST(spc.try_parse("M7 V"));    TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::M);  TEST_EQUAL(spc.sub, 7);
    TEST(spc.try_parse("G1 Ia"));   TEST_EQUAL(spc.lc, LC::Ia);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 1);
    TEST(spc.try_parse("G2 Ib"));   TEST_EQUAL(spc.lc, LC::Ib);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 2);
    TEST(spc.try_parse("G3 II"));   TEST_EQUAL(spc.lc, LC::II);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 3);
    TEST(spc.try_parse("G4 III"));  TEST_EQUAL(spc.lc, LC::III);   TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("G5 IV"));   TEST_EQUAL(spc.lc, LC::IV);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 5);
    TEST(spc.try_parse("G6 VI"));   TEST_EQUAL(spc.lc, LC::VI);    TEST_EQUAL(spc.sp, Sp::G);  TEST_EQUAL(spc.sub, 6);
    TEST(spc.try_parse("cK1"));     TEST_EQUAL(spc.lc, LC::Ia);    TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 1);
    TEST(spc.try_parse("gK2"));     TEST_EQUAL(spc.lc, LC::III);   TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 2);
    TEST(spc.try_parse("L8"));      TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::L);  TEST_EQUAL(spc.sub, 8);
    TEST(spc.try_parse("sgK3"));    TEST_EQUAL(spc.lc, LC::IV);    TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 3);
    TEST(spc.try_parse("dK4"));     TEST_EQUAL(spc.lc, LC::V);     TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("sdK5"));    TEST_EQUAL(spc.lc, LC::VI);    TEST_EQUAL(spc.sp, Sp::K);  TEST_EQUAL(spc.sub, 5);

    TRY((spc = {LC::none, Sp::L, 8}));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::L);  TEST_EQUAL(spc.sub, 8);  TEST_EQUAL(spc.index(), 78);  TRY(str = to_str(spc));  TEST_EQUAL(str, "L8");
    TRY((spc = {LC::none, Sp::T, 9}));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::T);  TEST_EQUAL(spc.sub, 9);  TEST_EQUAL(spc.index(), 89);  TRY(str = to_str(spc));  TEST_EQUAL(str, "T9");
    TRY((spc = {LC::none, Sp::Y, 0}));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::Y);  TEST_EQUAL(spc.sub, 0);  TEST_EQUAL(spc.index(), 90);  TRY(str = to_str(spc));  TEST_EQUAL(str, "Y0");
    TRY((spc = {LC::none, 78}));        TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::L);  TEST_EQUAL(spc.sub, 8);  TEST_EQUAL(spc.index(), 78);  TRY(str = to_str(spc));  TEST_EQUAL(str, "L8");
    TRY((spc = {LC::none, 89}));        TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::T);  TEST_EQUAL(spc.sub, 9);  TEST_EQUAL(spc.index(), 89);  TRY(str = to_str(spc));  TEST_EQUAL(str, "T9");
    TRY((spc = {LC::none, 90}));        TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::Y);  TEST_EQUAL(spc.sub, 0);  TEST_EQUAL(spc.index(), 90);  TRY(str = to_str(spc));  TEST_EQUAL(str, "Y0");
    TRY((spc = {Sp::L, 8}));            TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::L);  TEST_EQUAL(spc.sub, 8);  TEST_EQUAL(spc.index(), 78);  TRY(str = to_str(spc));  TEST_EQUAL(str, "L8");
    TRY((spc = {Sp::T, 9}));            TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::T);  TEST_EQUAL(spc.sub, 9);  TEST_EQUAL(spc.index(), 89);  TRY(str = to_str(spc));  TEST_EQUAL(str, "T9");
    TRY((spc = {Sp::Y, 0}));            TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::Y);  TEST_EQUAL(spc.sub, 0);  TEST_EQUAL(spc.index(), 90);  TRY(str = to_str(spc));  TEST_EQUAL(str, "Y0");
    TRY(spc = Spectrum(78));            TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::L);  TEST_EQUAL(spc.sub, 8);  TEST_EQUAL(spc.index(), 78);  TRY(str = to_str(spc));  TEST_EQUAL(str, "L8");
    TRY(spc = Spectrum(89));            TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::T);  TEST_EQUAL(spc.sub, 9);  TEST_EQUAL(spc.index(), 89);  TRY(str = to_str(spc));  TEST_EQUAL(str, "T9");
    TRY(spc = Spectrum(90));            TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::Y);  TEST_EQUAL(spc.sub, 0);  TEST_EQUAL(spc.index(), 90);  TRY(str = to_str(spc));  TEST_EQUAL(str, "Y0");

    TEST(spc.try_parse("T9"));    TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::T);  TEST_EQUAL(spc.sub, 9);
    TEST(spc.try_parse("Y0"));    TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::Y);  TEST_EQUAL(spc.sub, 0);
    TEST(spc.try_parse("L8 V"));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::L);  TEST_EQUAL(spc.sub, 8);
    TEST(spc.try_parse("T9 V"));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::T);  TEST_EQUAL(spc.sub, 9);
    TEST(spc.try_parse("Y0 V"));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::Y);  TEST_EQUAL(spc.sub, 0);

    TRY((spc = {Sp::DA, 1}));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DA);  TEST_EQUAL(spc.sub, 1);  TRY(str = to_str(spc));  TEST_EQUAL(str, "DA1");
    TRY((spc = {Sp::DB, 2}));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DB);  TEST_EQUAL(spc.sub, 2);  TRY(str = to_str(spc));  TEST_EQUAL(str, "DB2");
    TRY((spc = {Sp::DC, 3}));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DC);  TEST_EQUAL(spc.sub, 3);  TRY(str = to_str(spc));  TEST_EQUAL(str, "DC3");
    TRY((spc = {Sp::DO, 4}));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DO);  TEST_EQUAL(spc.sub, 4);  TRY(str = to_str(spc));  TEST_EQUAL(str, "DO4");
    TRY((spc = {Sp::DQ, 5}));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DQ);  TEST_EQUAL(spc.sub, 5);  TRY(str = to_str(spc));  TEST_EQUAL(str, "DQ5");
    TRY((spc = {Sp::DX, 6}));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DX);  TEST_EQUAL(spc.sub, 6);  TRY(str = to_str(spc));  TEST_EQUAL(str, "DX6");
    TRY((spc = {Sp::DZ, 7}));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DZ);  TEST_EQUAL(spc.sub, 7);  TRY(str = to_str(spc));  TEST_EQUAL(str, "DZ7");

    TEST(spc.try_parse("DA1"));    TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DA);  TEST_EQUAL(spc.sub, 1);
    TEST(spc.try_parse("DB2"));    TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DB);  TEST_EQUAL(spc.sub, 2);
    TEST(spc.try_parse("DC3"));    TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DC);  TEST_EQUAL(spc.sub, 3);
    TEST(spc.try_parse("DO4"));    TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DO);  TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("DQ5"));    TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DQ);  TEST_EQUAL(spc.sub, 5);
    TEST(spc.try_parse("DX6"));    TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DX);  TEST_EQUAL(spc.sub, 6);
    TEST(spc.try_parse("DZ7"));    TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DZ);  TEST_EQUAL(spc.sub, 7);
    TEST(spc.try_parse("DZPV8"));  TEST_EQUAL(spc.lc, LC::VII);  TEST_EQUAL(spc.sp, Sp::DZ);  TEST_EQUAL(spc.sub, 8);

    TRY((spc = {Sp::WC, 4}));   TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WC);   TEST_EQUAL(spc.sub, 4);  TRY(str = to_str(spc));  TEST_EQUAL(str, "WC4");
    TRY((spc = {Sp::WN, 4}));   TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WN);   TEST_EQUAL(spc.sub, 4);  TRY(str = to_str(spc));  TEST_EQUAL(str, "WN4");
    TRY((spc = {Sp::WNh, 4}));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WNh);  TEST_EQUAL(spc.sub, 4);  TRY(str = to_str(spc));  TEST_EQUAL(str, "WN4h");
    TRY((spc = {Sp::WO, 4}));   TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WO);   TEST_EQUAL(spc.sub, 4);  TRY(str = to_str(spc));  TEST_EQUAL(str, "WO4");
    TRY((spc = {Sp::C, 4}));    TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::C);    TEST_EQUAL(spc.sub, 4);  TRY(str = to_str(spc));  TEST_EQUAL(str, "C4");
    TRY((spc = {Sp::S, 4}));    TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::S);    TEST_EQUAL(spc.sub, 4);  TRY(str = to_str(spc));  TEST_EQUAL(str, "S4");

    TEST(spc.try_parse("WC4"));   TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WC);   TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("WN4"));   TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WN);   TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("WN4h"));  TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WNh);  TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("WO4"));   TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::WO);   TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("C4"));    TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::C);    TEST_EQUAL(spc.sub, 4);
    TEST(spc.try_parse("S4"));    TEST_EQUAL(spc.lc, LC::none);  TEST_EQUAL(spc.sp, Sp::S);    TEST_EQUAL(spc.sub, 4);

    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), >, Spectrum(LC::V, Sp::B, 5));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), ==, Spectrum(LC::V, Sp::G, 2));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(LC::V, Sp::M, 0));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), >, Spectrum(LC::III, Sp::B, 5));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), >, Spectrum(LC::III, Sp::G, 2));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), >, Spectrum(LC::III, Sp::M, 0));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(LC::VI, Sp::B, 5));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(LC::VI, Sp::G, 2));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(LC::VI, Sp::M, 0));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(Sp::L, 5));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(Sp::DA, 5));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(Sp::WC, 5));
    TEST_COMPARE(Spectrum(LC::V, Sp::G, 2), <, Spectrum(Sp::S, 5));

}

void test_core_astronomy_hertzsprung_russell_diagram() {

    Spectrum spc;
    ClassData cld;

    TRY(spc = Spectrum("O5 V"));    TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "O5 V");    TEST_NEAR(cld.mass, 60);    TEST_NEAR(cld.M_V, -5.7);  TEST_NEAR(cld.temperature, 42000);  TEST_NEAR(cld.B_V, -0.33);  TEST_NEAR(cld.BC, -4.40);
    TRY(spc = Spectrum("M5 V"));    TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "M5 V");    TEST_NEAR(cld.mass, 0.21);  TEST_NEAR(cld.M_V, 12.3);  TEST_NEAR(cld.temperature, 3170);   TEST_NEAR(cld.B_V, 1.64);   TEST_NEAR(cld.BC, -2.73);
    TRY(spc = Spectrum("M0 III"));  TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "M0 III");  TEST_NEAR(cld.mass, 1.2);   TEST_NEAR(cld.M_V, -0.4);
    TRY(spc = Spectrum("M0 Ia"));   TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "M0 Ia");   TEST_NEAR(cld.mass, 13);    TEST_NEAR(cld.M_V, -5.6);

    TRY(spc = Spectrum("L0"));    TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "L0");    TEST_NEAR_EPSILON(cld.mass, 0.0869, 1e-4);  TEST_NEAR_EPSILON(cld.luminosity, 2.19e-4, 1e-6);  TEST_NEAR_EPSILON(cld.temperature, 2220, 10);
    TRY(spc = Spectrum("T0"));    TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "T0");    TEST_NEAR_EPSILON(cld.mass, 0.0424, 1e-4);  TEST_NEAR_EPSILON(cld.luminosity, 2.00e-5, 1e-7);  TEST_NEAR_EPSILON(cld.temperature, 1220, 10);
    TRY(spc = Spectrum("Y0"));    TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "Y0");    TEST_NEAR_EPSILON(cld.mass, 0.0225, 1e-4);  TEST_NEAR_EPSILON(cld.luminosity, 2.42e-6, 1e-8);  TEST_NEAR_EPSILON(cld.temperature, 720, 1);
    TRY(spc = Spectrum("DA1"));   TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "DA1");   TEST_NEAR_EPSILON(cld.mass, 0.6, 1e-3);     TEST_NEAR_EPSILON(cld.luminosity, 0.982, 0.001);   TEST_NEAR_EPSILON(cld.temperature, 50400, 100);
    TRY(spc = Spectrum("DA9"));   TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "DA9");   TEST_NEAR_EPSILON(cld.mass, 0.6, 1e-3);     TEST_NEAR_EPSILON(cld.luminosity, 1.50e-4, 1e-6);  TEST_NEAR_EPSILON(cld.temperature, 5600, 10);
    TRY(spc = Spectrum("WC4"));   TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "WC4");   TEST_EQUAL(cld.mass, 10);                   TEST_EQUAL(cld.luminosity, 158'000);               TEST_EQUAL(cld.temperature, 117'000);
    TRY(spc = Spectrum("WN4"));   TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "WN4");   TEST_EQUAL(cld.mass, 15);                   TEST_EQUAL(cld.luminosity, 200'000);               TEST_EQUAL(cld.temperature, 70'000);
    TRY(spc = Spectrum("WN4h"));  TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "WN4h");  TEST_EQUAL(cld.mass, 200);                  TEST_EQUAL(cld.luminosity, 5'000'000);             TEST_EQUAL(cld.temperature, 50'000);
    TRY(spc = Spectrum("WO4"));   TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "WO4");   TEST_EQUAL(cld.mass, 19);                   TEST_EQUAL(cld.luminosity, 630'000);               TEST_EQUAL(cld.temperature, 200'000);
    TRY(spc = Spectrum("C5"));    TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "C5");    TEST_NEAR_EPSILON(cld.mass, 2.66, 0.01);    TEST_NEAR_EPSILON(cld.luminosity, 1340, 10);       TEST_NEAR_EPSILON(cld.temperature, 4170, 10);
    TRY(spc = Spectrum("S5"));    TRY(cld = ClassData(spc));  TEST_EQUAL(cld.spectrum.str(), "S5");    TEST_NEAR_EPSILON(cld.mass, 3.64, 0.01);    TEST_NEAR_EPSILON(cld.luminosity, 6530, 10);       TEST_NEAR_EPSILON(cld.temperature, 3170, 10);

}
