#include "rs-core/statistics.hpp"
#include "rs-core/unit-test.hpp"
#include <map>
#include <vector>

using namespace RS;

void test_core_statistics_class() {

    Statistics<> stats;
    std::vector<int> v;
    std::map<int, int> m;

    TEST_EQUAL(stats.num(), 0);
    TEST_EQUAL(stats.min_x(), 0);
    TEST_EQUAL(stats.min_y(), 0);
    TEST_EQUAL(stats.max_x(), 0);
    TEST_EQUAL(stats.max_y(), 0);
    TEST_EQUAL(stats.mean_x(), 0);
    TEST_EQUAL(stats.mean_y(), 0);
    TEST_EQUAL(stats.stdevp_x(), 0);
    TEST_EQUAL(stats.stdevp_y(), 0);
    TEST_EQUAL(stats.stdevs_x(), 0);
    TEST_EQUAL(stats.stdevs_y(), 0);
    TEST_EQUAL(stats.correlation(), 0);
    TEST_EQUAL(stats.linear().first, 0);
    TEST_EQUAL(stats.linear().second, 0);

    TRY(stats.clear());
    for (int i = 1; i <= 100; ++i)
        TRY(stats.add(i));

    TEST_EQUAL(stats.num(), 100);
    TEST_EQUAL(stats.min(), 1);
    TEST_EQUAL(stats.max(), 100);
    TEST_EQUAL(stats.mean(), 50.5);
    TEST_NEAR(stats.stdevp(), 28.866070);
    TEST_NEAR(stats.stdevs(), 29.011492);

    TRY(stats.clear());
    for (int i = 1; i <= 100; ++i)
        TRY(stats.add(i, i * i));

    TEST_EQUAL(stats.num(), 100);
    TEST_EQUAL(stats.min_x(), 1);
    TEST_EQUAL(stats.min_y(), 1);
    TEST_EQUAL(stats.max_x(), 100);
    TEST_EQUAL(stats.max_y(), 10000);
    TEST_EQUAL(stats.mean_x(), 50.5);
    TEST_EQUAL(stats.mean_y(), 3383.5);
    TEST_NEAR(stats.stdevp_x(), 28.866070);
    TEST_NEAR(stats.stdevp_y(), 3009.196080);
    TEST_NEAR(stats.stdevs_x(), 29.011492);
    TEST_NEAR(stats.stdevs_y(), 3024.355854);
    TEST_NEAR(stats.correlation(), 0.968854);
    TEST_NEAR(stats.linear().first, 101);
    TEST_NEAR(stats.linear().second, -1717);

    for (int i = 1; i <= 100; ++i)
        v.push_back(i);
    TRY(stats.clear());
    TRY(stats.append(v));

    TEST_EQUAL(stats.num(), 100);
    TEST_EQUAL(stats.min(), 1);
    TEST_EQUAL(stats.max(), 100);
    TEST_EQUAL(stats.mean(), 50.5);
    TEST_NEAR(stats.stdevp(), 28.866070);
    TEST_NEAR(stats.stdevs(), 29.011492);

    for (int i = 1; i <= 100; ++i)
        m[i] = i * i;
    TRY(stats.clear());
    TRY(stats.append(m));

    TEST_EQUAL(stats.num(), 100);
    TEST_EQUAL(stats.min_x(), 1);
    TEST_EQUAL(stats.min_y(), 1);
    TEST_EQUAL(stats.max_x(), 100);
    TEST_EQUAL(stats.max_y(), 10000);
    TEST_EQUAL(stats.mean_x(), 50.5);
    TEST_EQUAL(stats.mean_y(), 3383.5);
    TEST_NEAR(stats.stdevp_x(), 28.866070);
    TEST_NEAR(stats.stdevp_y(), 3009.196080);
    TEST_NEAR(stats.stdevs_x(), 29.011492);
    TEST_NEAR(stats.stdevs_y(), 3024.355854);
    TEST_NEAR(stats.correlation(), 0.968854);
    TEST_NEAR(stats.linear().first, 101);
    TEST_NEAR(stats.linear().second, -1717);

}
