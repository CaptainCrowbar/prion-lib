#include "rs-core/table.hpp"
#include "rs-core/unit-test.hpp"

using namespace RS;

void test_core_table_class() {

    Table tab;
    U8string s;

    TEST(tab.empty());
    TRY(s = to_str(tab));
    TEST_EQUAL(s, "");

    TRY(tab << "alpha" << "bravo" << "charlie" << '\n');
    TEST(! tab.empty());
    TRY(tab << 1 << 2 << 3 << '\n');
    TRY(s = to_str(tab));
    TEST_EQUAL(s,
        "alpha  bravo  charlie\n"
        "1      2      3\n"
    );

    TRY(tab.clear());
    TEST(tab.empty());
    TRY(tab << "alpha" << "bravo" << "charlie" << '\n');
    TRY(tab.rule());
    TRY(tab << 10 << 20 << 30 << '\n');
    TRY(tab << 40 << '\n');
    TRY(tab << "" << 50 << '\n');
    TRY(tab << "" << "" << 60 << '\n');
    TRY(tab << "" << "" << "" << 70 << '\n');
    TRY(s = to_str(tab));
    TEST_EQUAL(s,
        "alpha  bravo  charlie  --\n"
        "=====  =====  =======  ==\n"
        "10     20     30       --\n"
        "40     --     --       --\n"
        "--     50     --       --\n"
        "--     --     60       --\n"
        "--     --     --       70\n"
    );

    TRY(tab.clear());
    TRY(tab << "alpha" << "bravo" << "charlie" << '\n');
    TRY(tab.rule('*'));
    TRY(tab << 10 << 20 << 30 << '\n');
    TRY(tab << 10 << 9 << 8 << '\n');
    TRY(tab << 10 << 9 << 7 << '\n');
    TRY(tab << 10 << 9 << 7 << '\n');
    TRY(s = to_str(tab));
    TEST_EQUAL(s,
        "alpha  bravo  charlie\n"
        "*****  *****  *******\n"
        "10     20     30\n"
        "10     9      8\n"
        "10     9      7\n"
        "10     9      7\n"
    );
    TRY(tab.show_repeats());
    TRY(s = to_str(tab));
    TEST_EQUAL(s,
        "alpha  bravo  charlie\n"
        "*****  *****  *******\n"
        "10     20     30\n"
        "''     9      8\n"
        "''     ''     7\n"
        "''     ''     ''\n"
    );

}
