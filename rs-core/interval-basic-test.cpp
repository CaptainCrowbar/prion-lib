#include "rs-core/interval.hpp"
#include "rs-core/fixed-binary.hpp"
#include "rs-core/mp-integer.hpp"
#include "rs-core/rational.hpp"
#include "rs-core/unit-test.hpp"
#include "rs-core/vector.hpp"
#include "unicorn/path.hpp"
#include <chrono>
#include <complex>
#include <string>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;

using IC = IntervalCategory;

void test_core_interval_basic_type_traits() {

    TEST_EQUAL(interval_category<bool>, IC::none);
    TEST_EQUAL(interval_category<const bool>, IC::none);
    TEST_EQUAL(interval_category<volatile bool>, IC::none);
    TEST_EQUAL(interval_category<const volatile bool>, IC::none);
    TEST_EQUAL(interval_category<void>, IC::none);
    TEST_EQUAL(interval_category<std::complex<float>>, IC::none);

    TEST_EQUAL(interval_category<std::string>, IC::ordered);
    TEST_EQUAL(interval_category<const std::string>, IC::ordered);
    TEST_EQUAL(interval_category<volatile std::string>, IC::ordered);
    TEST_EQUAL(interval_category<const volatile std::string>, IC::ordered);
    TEST_EQUAL(interval_category<std::vector<int>>, IC::ordered);
    TEST_EQUAL(interval_category<void*>, IC::ordered);
    TEST_EQUAL(interval_category<const void*>, IC::ordered);
    TEST_EQUAL(interval_category<Float3>, IC::ordered);
    TEST_EQUAL(interval_category<Path>, IC::ordered);

    TEST_EQUAL(interval_category<int>, IC::integral);
    TEST_EQUAL(interval_category<const int>, IC::integral);
    TEST_EQUAL(interval_category<volatile int>, IC::integral);
    TEST_EQUAL(interval_category<const volatile int>, IC::integral);
    TEST_EQUAL(interval_category<uint8_t>, IC::integral);
    TEST_EQUAL(interval_category<int64_t>, IC::integral);
    TEST_EQUAL(interval_category<char*>, IC::integral);
    TEST_EQUAL(interval_category<const char*>, IC::integral);
    TEST_EQUAL(interval_category<std::string::const_iterator>, IC::integral);
    TEST_EQUAL(interval_category<std::string::iterator>, IC::integral);
    TEST_EQUAL(interval_category<std::vector<int>::const_iterator>, IC::integral);
    TEST_EQUAL(interval_category<std::vector<int>::iterator>, IC::integral);
    TEST_EQUAL(interval_category<std::chrono::microseconds>, IC::integral);
    TEST_EQUAL(interval_category<Binary<128>>, IC::integral);
    TEST_EQUAL(interval_category<Mpint>, IC::integral);

    TEST_EQUAL(interval_category<float>, IC::continuous);
    TEST_EQUAL(interval_category<const float>, IC::continuous);
    TEST_EQUAL(interval_category<volatile float>, IC::continuous);
    TEST_EQUAL(interval_category<const volatile float>, IC::continuous);
    TEST_EQUAL(interval_category<double>, IC::continuous);
    TEST_EQUAL(interval_category<Ratio>, IC::continuous);

}
