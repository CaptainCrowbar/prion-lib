#include "rs-core/statistics.hpp"
#include "rs-core/random.hpp"
#include "rs-core/unit-test.hpp"
#include <cmath>
#include <cstdlib>

using namespace RS;

void test_core_statistics_basic_parameters() {

    Statistics<double> stats;

    for (int i = 1; i <= 1000; ++i) {
        double z = i;
        double x = std::pow(z, 2) / 1e3;
        double y = x - std::pow(z, 3) / 1e6;
        TRY(stats(x, y));
    }

    TEST_EQUAL(stats.columns(), 2);
    TEST_EQUAL(stats.count(), 1000);
    TEST_EQUAL(stats.weight(), 1000);
    TEST_EQUAL(stats.min(0), 0.001);
    TEST_EQUAL(stats.min(1), 0);
    TEST_EQUAL(stats.max(0), 1000);
    TEST_EQUAL(stats.max(1), 148.148037);
    TEST_NEAR(stats.mean(0), 333.833500);
    TEST_NEAR(stats.mean(1), 83.333250);
    TEST_NEAR(stats.sd(0), 298.421728);
    TEST_NEAR(stats.sd(1), 50.787587);
    TEST_NEAR(stats.sd_bc(0), 298.571051);
    TEST_NEAR(stats.sd_bc(1), 50.813000);
    TEST_NEAR(stats.sd_bc_rel(0), 298.571051);
    TEST_NEAR(stats.sd_bc_rel(1), 50.813000);
    TEST_NEAR(stats.covariance(), 5513.819486);
    TEST_NEAR(stats.covariance_bc(), 5519.338825);
    TEST_NEAR(stats.covariance_bc_rel(), 5519.338825);
    TEST_NEAR(stats.correlation(), 0.363802);
    TEST_NEAR(stats.linear(0,1).first, 0.061914);
    TEST_NEAR(stats.linear(0,1).second, 62.664149);
    TEST_NEAR(stats.linear(1,0).first, 2.137654);
    TEST_NEAR(stats.linear(1,0).second, 155.695854);

    TRY(stats.clear());

    for (int i = 1; i <= 1000; ++i) {
        double z = i;
        double x = std::pow(z, 2) / 1e3;
        double y = x - std::pow(z, 3) / 1e6;
        double w = z / 1000;
        TRY(stats.w(w, x, y));
    }

    TEST_EQUAL(stats.columns(), 2);
    TEST_EQUAL(stats.count(), 1000);
    TEST_EQUAL(stats.weight(), 500.5);
    TEST_EQUAL(stats.min(0), 0.001);
    TEST_EQUAL(stats.min(1), 0);
    TEST_EQUAL(stats.max(0), 1000);
    TEST_EQUAL(stats.max(1), 148.148037);
    TEST_NEAR(stats.mean(0), 500.500000);
    TEST_NEAR(stats.mean(1), 99.899933);
    TEST_NEAR(stats.sd(0), 288.963521);
    TEST_NEAR(stats.sd(1), 43.736396);
    TEST_NEAR(stats.sd_bc(0), 289.252629);
    TEST_NEAR(stats.sd_bc(1), 43.780154);
    TEST_NEAR(stats.sd_bc_rel(0), 289.156260);
    TEST_NEAR(stats.sd_bc_rel(1), 43.765568);
    TEST_NEAR(stats.covariance(), -2428.607024);
    TEST_NEAR(stats.covariance_bc(), -2433.469100);
    TEST_NEAR(stats.covariance_bc_rel(), -2431.847868);
    TEST_NEAR(stats.correlation(), -0.192164);
    TEST_NEAR(stats.linear(0,1).first, -0.029085);
    TEST_NEAR(stats.linear(0,1).second, 114.457048);
    TEST_NEAR(stats.linear(1,0).first, -1.269613);
    TEST_NEAR(stats.linear(1,0).second, 627.334224);

}

void test_core_statistics_frequency_weighting() {

    Statistics<double> stats1, stats2;

    for (int i = 1; i <= 100; ++i) {
        double w = i;
        double x = std::pow(w, 2) - w;
        double y = std::pow(w, 3) - w;
        for (int j = 0; j < i; ++j)
            TRY(stats1(x, y));
        TRY(stats2.w(w, x, y));
    }

    TEST_EQUAL(stats1.columns(), 2);    TEST_EQUAL(stats2.columns(), 2);
    TEST_EQUAL(stats1.count(), 5050);   TEST_EQUAL(stats2.count(), 100);
    TEST_EQUAL(stats1.weight(), 5050);  TEST_EQUAL(stats2.weight(), 5050);
    TEST_EQUAL(stats1.min(0), 0);       TEST_EQUAL(stats2.min(0), 0);
    TEST_EQUAL(stats1.min(1), 0);       TEST_EQUAL(stats2.min(1), 0);
    TEST_EQUAL(stats1.max(0), 9900);    TEST_EQUAL(stats2.max(0), 9900);
    TEST_EQUAL(stats1.max(1), 999900);  TEST_EQUAL(stats2.max(1), 999900);

    TEST_NEAR(stats1.mean(0), stats2.mean(0));
    TEST_NEAR(stats1.mean(1), stats2.mean(1));
    TEST_NEAR(stats1.sd(0), stats2.sd(0));
    TEST_NEAR(stats1.sd(1), stats2.sd(1));
    TEST_NEAR(stats1.sd_bc(0), stats2.sd_bc(0));
    TEST_NEAR(stats1.sd_bc(1), stats2.sd_bc(1));
    TEST_NEAR(stats1.covariance(), stats2.covariance());
    TEST_NEAR(stats1.covariance_bc(), stats2.covariance_bc());
    TEST_NEAR(stats1.correlation(), stats2.correlation());
    TEST_NEAR(stats1.linear(0,1).first, stats2.linear(0,1).first);
    TEST_NEAR(stats1.linear(0,1).second, stats2.linear(0,1).second);
    TEST_NEAR(stats1.linear(1,0).first, stats2.linear(1,0).first);
    TEST_NEAR(stats1.linear(1,0).second, stats2.linear(1,0).second);

}

void test_core_statistics_fisher_cumulant_test() {

    Xoshiro rng(42);
    UniformReal<double> unit;
    Normal<double> norm;
    FisherTest<double> fish;

    for (int i = 0; i < 1000; ++i) {
        double f = unit(rng);
        double x = norm(rng);
        fish.add(x, f);
    }

    TEST_COMPARE(std::abs(fish.u1()), <, 3);
    TEST_COMPARE(std::abs(fish.u2()), <, 3);
    TEST_COMPARE(fish.p_chi2(), >, 0.001);
    TEST_COMPARE(fish.z_chi2(), <, 3);

}
