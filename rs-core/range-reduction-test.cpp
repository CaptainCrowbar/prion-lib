#include "rs-core/range-reduction.hpp"
#include "rs-core/statistics.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <functional>
#include <map>
#include <utility>
#include <vector>

using namespace RS;
using namespace RS::Range;
using namespace std::literals;

void test_core_range_reduction_all_any_none() {

    TEST(""s >> all_of(ascii_isprint));
    TEST("Hello world"s >> all_of(ascii_isprint));
    TEST(! ("Hello world"s >> all_of(ascii_isspace)));
    TEST(! ("Hello world"s >> all_of(ascii_iscntrl)));

    TEST(! (""s >> any_of(ascii_isprint)));
    TEST("Hello world"s >> any_of(ascii_isprint));
    TEST("Hello world"s >> any_of(ascii_isspace));
    TEST(! ("Hello world"s >> any_of(ascii_iscntrl)));

    TEST(""s >> none_of(ascii_isprint));
    TEST(! ("Hello world"s >> none_of(ascii_isprint)));
    TEST(! ("Hello world"s >> none_of(ascii_isspace)));
    TEST("Hello world"s >> none_of(ascii_iscntrl));

}

void test_core_range_reduction_count() {

    size_t n = 0;

    TRY(n = "Hello world"s >> count);                    TEST_EQUAL(n, 11);
    TRY(n = "Hello world"s >> count('l'));               TEST_EQUAL(n, 3);
    TRY(n = "Hello world"s >> count('*'));               TEST_EQUAL(n, 0);
    TRY(n = "Hello world"s >> count_if(ascii_isalpha));  TEST_EQUAL(n, 10);
    TRY(n = "Hello world"s >> count_if(ascii_isspace));  TEST_EQUAL(n, 1);
    TRY(n = "Hello world"s >> count_if(ascii_ispunct));  TEST_EQUAL(n, 0);

    TRY(n = "Hello world"s >> passthrough >> count);                    TEST_EQUAL(n, 11);
    TRY(n = "Hello world"s >> passthrough >> count('l'));               TEST_EQUAL(n, 3);
    TRY(n = "Hello world"s >> passthrough >> count('*'));               TEST_EQUAL(n, 0);
    TRY(n = "Hello world"s >> passthrough >> count_if(ascii_isalpha));  TEST_EQUAL(n, 10);
    TRY(n = "Hello world"s >> passthrough >> count_if(ascii_isspace));  TEST_EQUAL(n, 1);
    TRY(n = "Hello world"s >> passthrough >> count_if(ascii_ispunct));  TEST_EQUAL(n, 0);

    TRY(n = "Hello world"s >> (passthrough * count));                    TEST_EQUAL(n, 11);
    TRY(n = "Hello world"s >> (passthrough * count('l')));               TEST_EQUAL(n, 3);
    TRY(n = "Hello world"s >> (passthrough * count('*')));               TEST_EQUAL(n, 0);
    TRY(n = "Hello world"s >> (passthrough * count_if(ascii_isalpha)));  TEST_EQUAL(n, 10);
    TRY(n = "Hello world"s >> (passthrough * count_if(ascii_isspace)));  TEST_EQUAL(n, 1);
    TRY(n = "Hello world"s >> (passthrough * count_if(ascii_ispunct)));  TEST_EQUAL(n, 0);

}

void test_core_range_reduction_fold() {

    std::vector<int> v = {1,2,3,4,5};
    int n = 0;
    auto f = [] (int x, int y) { return 2 * x + y; };

    TRY(n = v >> fold_left(0, std::plus<>()));                  TEST_EQUAL(n, 15);
    TRY(n = v >> fold_right(0, std::plus<>()));                 TEST_EQUAL(n, 15);
    TRY(n = v >> fold_left(0, f));                              TEST_EQUAL(n, 57);
    TRY(n = v >> fold_right(0, f));                             TEST_EQUAL(n, 30);
    TRY(n = v >> passthrough >> fold_left(0, std::plus<>()));   TEST_EQUAL(n, 15);
    TRY(n = v >> (passthrough * fold_left(0, std::plus<>())));  TEST_EQUAL(n, 15);
    TRY(n = v >> passthrough >> fold_left(0, f));               TEST_EQUAL(n, 57);
    TRY(n = v >> (passthrough * fold_left(0, f)));              TEST_EQUAL(n, 57);

}

void test_core_range_reduction_is_empty() {

    Ustring s;

    s = "";             TEST(s >> is_empty);      TEST(! (s >> is_nonempty));
    s = "x";            TEST(! (s >> is_empty));  TEST(s >> is_nonempty);
    s = "hello world";  TEST(! (s >> is_empty));  TEST(s >> is_nonempty);

}

void test_core_range_reduction_is_sorted() {

    Ustring s;

    s = "";        TEST(s >> is_sorted);      TEST(s >> is_sorted(std::greater<>()));
    s = "a";       TEST(s >> is_sorted);      TEST(s >> is_sorted(std::greater<>()));
    s = "az";      TEST(s >> is_sorted);      TEST(! (s >> is_sorted(std::greater<>())));
    s = "za";      TEST(! (s >> is_sorted));  TEST(s >> is_sorted(std::greater<>()));
    s = "aeiou";   TEST(s >> is_sorted);      TEST(! (s >> is_sorted(std::greater<>())));
    s = "aeioua";  TEST(! (s >> is_sorted));  TEST(! (s >> is_sorted(std::greater<>())));
    s = "uoiea";   TEST(! (s >> is_sorted));  TEST(s >> is_sorted(std::greater<>()));

}

void test_core_range_reduction_min_max() {

    Ustring s0, s1 = "Hello world";
    char c = 0;
    std::pair<char, char> cp;

    TRY(c = s0 >> min);                         TEST_EQUAL(c, '\0');
    TRY(c = s0 >> max);                         TEST_EQUAL(c, '\0');
    TRY(cp = s0 >> min_max);                    TEST_EQUAL(cp.first, '\0'); TEST_EQUAL(cp.second, '\0');
    TRY(c = s1 >> min);                         TEST_EQUAL(c, ' ');
    TRY(c = s1 >> max);                         TEST_EQUAL(c, 'w');
    TRY(cp = s1 >> min_max);                    TEST_EQUAL(cp.first, ' '); TEST_EQUAL(cp.second, 'w');
    TRY(c = s1 >> min(std::greater<>()));       TEST_EQUAL(c, 'w');
    TRY(c = s1 >> max(std::greater<>()));       TEST_EQUAL(c, ' ');
    TRY(cp = s1 >> min_max(std::greater<>()));  TEST_EQUAL(cp.first, 'w'); TEST_EQUAL(cp.second, ' ');

}

void test_core_range_reduction_reduce() {

    std::vector<int> v0, v1 = {42}, v5 = {1,2,3,4,5};
    int n = 0;
    auto f = [] (int x, int y) { return 2 * x + y; };

    TRY(n = v0 >> reduce(std::plus<>()));                  TEST_EQUAL(n, 0);
    TRY(n = v0 >> reduce(f));                              TEST_EQUAL(n, 0);
    TRY(n = v1 >> reduce(std::plus<>()));                  TEST_EQUAL(n, 42);
    TRY(n = v1 >> reduce(f));                              TEST_EQUAL(n, 42);
    TRY(n = v5 >> reduce(std::plus<>()));                  TEST_EQUAL(n, 15);
    TRY(n = v5 >> reduce(f));                              TEST_EQUAL(n, 57);
    TRY(n = v5 >> passthrough >> reduce(std::plus<>()));   TEST_EQUAL(n, 15);
    TRY(n = v5 >> (passthrough * reduce(std::plus<>())));  TEST_EQUAL(n, 15);
    TRY(n = v5 >> passthrough >> reduce(f));               TEST_EQUAL(n, 57);
    TRY(n = v5 >> (passthrough * reduce(f)));              TEST_EQUAL(n, 57);

}

void test_core_range_reduction_statistics() {

    Statistics<double> stat;
    std::vector<int> v = {2,3,5,7,11,13,17,19,23,29};
    TRY(stat = v >> statistics);
    TEST_EQUAL(stat.count(), 10);
    TEST_EQUAL(stat.min(), 2);
    TEST_EQUAL(stat.max(), 29);
    TEST_NEAR(stat.mean(), 12.9);
    TEST_NEAR(stat.sd(), 8.560958);
    TEST_NEAR(stat.sd_bc(), 9.024042);

    TRY(stat = v >> (passthrough * statistics));
    TEST_EQUAL(stat.count(), 10);
    TEST_EQUAL(stat.min(), 2);
    TEST_EQUAL(stat.max(), 29);
    TEST_NEAR(stat.mean(), 12.9);
    TEST_NEAR(stat.sd(), 8.560958);
    TEST_NEAR(stat.sd_bc(), 9.024042);

    std::map<double, double> m = {{1,2},{2,3},{3,5},{4,7},{5,11},{6,13},{7,17},{8,19},{9,23},{10,29}};
    TRY(stat = m >> statistics);
    TEST_EQUAL(stat.count(), 10);
    TEST_EQUAL(stat.min(0), 1);
    TEST_EQUAL(stat.max(0), 10);
    TEST_EQUAL(stat.min(1), 2);
    TEST_EQUAL(stat.max(1), 29);
    TEST_EQUAL(stat.mean(0), 5.5);
    TEST_NEAR(stat.mean(1), 12.9);
    TEST_NEAR(stat.sd(0), 2.872281);
    TEST_NEAR(stat.sd(1), 8.560958);
    TEST_NEAR(stat.sd_bc(0), 3.027650);
    TEST_NEAR(stat.sd_bc(1), 9.024042);
    TEST_NEAR(stat.correlation(), 0.986194);
    TEST_NEAR(stat.linear(0,1).first, 2.939394);
    TEST_NEAR(stat.linear(0,1).second, -3.266667);
    TEST_NEAR(stat.linear(1,0).first, 0.330877);
    TEST_NEAR(stat.linear(1,0).second, 1.231682);

    TRY(stat = m >> (passthrough * statistics));
    TEST_EQUAL(stat.count(), 10);
    TEST_EQUAL(stat.min(0), 1);
    TEST_EQUAL(stat.max(0), 10);
    TEST_EQUAL(stat.min(1), 2);
    TEST_EQUAL(stat.max(1), 29);
    TEST_EQUAL(stat.mean(0), 5.5);
    TEST_NEAR(stat.mean(1), 12.9);
    TEST_NEAR(stat.sd(0), 2.872281);
    TEST_NEAR(stat.sd(1), 8.560958);
    TEST_NEAR(stat.sd_bc(0), 3.027650);
    TEST_NEAR(stat.sd_bc(1), 9.024042);
    TEST_NEAR(stat.correlation(), 0.986194);
    TEST_NEAR(stat.linear(0,1).first, 2.939394);
    TEST_NEAR(stat.linear(0,1).second, -3.266667);
    TEST_NEAR(stat.linear(1,0).first, 0.330877);
    TEST_NEAR(stat.linear(1,0).second, 1.231682);

}

void test_core_range_reduction_sum_product() {

    std::vector<int> v0, v1 = {1,2,3,4,5};
    int x = 0;

    TRY(x = v0 >> sum);                       TEST_EQUAL(x, 0);
    TRY(x = v0 >> sum(100));                  TEST_EQUAL(x, 100);
    TRY(x = v1 >> sum);                       TEST_EQUAL(x, 15);
    TRY(x = v1 >> sum(100));                  TEST_EQUAL(x, 115);
    TRY(x = v0 >> passthrough >> sum);        TEST_EQUAL(x, 0);
    TRY(x = v0 >> passthrough >> sum(100));   TEST_EQUAL(x, 100);
    TRY(x = v1 >> passthrough >> sum);        TEST_EQUAL(x, 15);
    TRY(x = v1 >> passthrough >> sum(100));   TEST_EQUAL(x, 115);
    TRY(x = v0 >> (passthrough * sum));       TEST_EQUAL(x, 0);
    TRY(x = v0 >> (passthrough * sum(100)));  TEST_EQUAL(x, 100);
    TRY(x = v1 >> (passthrough * sum));       TEST_EQUAL(x, 15);
    TRY(x = v1 >> (passthrough * sum(100)));  TEST_EQUAL(x, 115);

    TRY(x = v0 >> product);                       TEST_EQUAL(x, 1);
    TRY(x = v0 >> product(100));                  TEST_EQUAL(x, 100);
    TRY(x = v1 >> product);                       TEST_EQUAL(x, 120);
    TRY(x = v1 >> product(100));                  TEST_EQUAL(x, 12000);
    TRY(x = v0 >> passthrough >> product);        TEST_EQUAL(x, 1);
    TRY(x = v0 >> passthrough >> product(100));   TEST_EQUAL(x, 100);
    TRY(x = v1 >> passthrough >> product);        TEST_EQUAL(x, 120);
    TRY(x = v1 >> passthrough >> product(100));   TEST_EQUAL(x, 12000);
    TRY(x = v0 >> (passthrough * product));       TEST_EQUAL(x, 1);
    TRY(x = v0 >> (passthrough * product(100)));  TEST_EQUAL(x, 100);
    TRY(x = v1 >> (passthrough * product));       TEST_EQUAL(x, 120);
    TRY(x = v1 >> (passthrough * product(100)));  TEST_EQUAL(x, 12000);

}
