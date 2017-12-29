#include "rs-core/terminal.hpp"
#include "rs-core/time.hpp"
#include "rs-core/unit-test.hpp"
#include "rs-core/vector.hpp"
#include <chrono>
#include <iostream>
#include <string>

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

void test_core_terminal_progress_bar() {

    // double scale = 100;
    // auto runtime = 2s;
    // auto interval = runtime / scale;
    // ProgressBar bar("Hello");
    // for (int i = 0; i <= 100; ++i) {
    //     bar(i / scale);
    //     sleep_for(interval);
    // }

}

void test_core_terminal_io_operations() {

    U8string s;

    TRY(s = xt_up);                        TEST_EQUAL(s, "\x1b[A");
    TRY(s = xt_down);                      TEST_EQUAL(s, "\x1b[B");
    TRY(s = xt_right);                     TEST_EQUAL(s, "\x1b[C");
    TRY(s = xt_left);                      TEST_EQUAL(s, "\x1b[D");
    TRY(s = xt_erase_left);                TEST_EQUAL(s, "\x1b[1K");
    TRY(s = xt_erase_right);               TEST_EQUAL(s, "\x1b[K");
    TRY(s = xt_erase_above);               TEST_EQUAL(s, "\x1b[1J");
    TRY(s = xt_erase_below);               TEST_EQUAL(s, "\x1b[J");
    TRY(s = xt_erase_line);                TEST_EQUAL(s, "\x1b[2K");
    TRY(s = xt_clear);                     TEST_EQUAL(s, "\x1b[2J");
    TRY(s = xt_reset);                     TEST_EQUAL(s, "\x1b[0m");
    TRY(s = xt_bold);                      TEST_EQUAL(s, "\x1b[1m");
    TRY(s = xt_under);                     TEST_EQUAL(s, "\x1b[4m");
    TRY(s = xt_black);                     TEST_EQUAL(s, "\x1b[30m");
    TRY(s = xt_red);                       TEST_EQUAL(s, "\x1b[31m");
    TRY(s = xt_green);                     TEST_EQUAL(s, "\x1b[32m");
    TRY(s = xt_yellow);                    TEST_EQUAL(s, "\x1b[33m");
    TRY(s = xt_blue);                      TEST_EQUAL(s, "\x1b[34m");
    TRY(s = xt_magenta);                   TEST_EQUAL(s, "\x1b[35m");
    TRY(s = xt_cyan);                      TEST_EQUAL(s, "\x1b[36m");
    TRY(s = xt_white);                     TEST_EQUAL(s, "\x1b[37m");
    TRY(s = xt_black_bg);                  TEST_EQUAL(s, "\x1b[40m");
    TRY(s = xt_red_bg);                    TEST_EQUAL(s, "\x1b[41m");
    TRY(s = xt_green_bg);                  TEST_EQUAL(s, "\x1b[42m");
    TRY(s = xt_yellow_bg);                 TEST_EQUAL(s, "\x1b[43m");
    TRY(s = xt_blue_bg);                   TEST_EQUAL(s, "\x1b[44m");
    TRY(s = xt_magenta_bg);                TEST_EQUAL(s, "\x1b[45m");
    TRY(s = xt_cyan_bg);                   TEST_EQUAL(s, "\x1b[46m");
    TRY(s = xt_white_bg);                  TEST_EQUAL(s, "\x1b[47m");
    TRY(s = xt_move_up(42));               TEST_EQUAL(s, "\x1b[42A");
    TRY(s = xt_move_down(42));             TEST_EQUAL(s, "\x1b[42B");
    TRY(s = xt_move_right(42));            TEST_EQUAL(s, "\x1b[42C");
    TRY(s = xt_move_left(42));             TEST_EQUAL(s, "\x1b[42D");
    TRY(s = xt_colour(0, 0, 0));           TEST_EQUAL(s, "\x1b[38;5;16m");
    TRY(s = xt_colour(1, 2, 3));           TEST_EQUAL(s, "\x1b[38;5;67m");
    TRY(s = xt_colour(5, 4, 3));           TEST_EQUAL(s, "\x1b[38;5;223m");
    TRY(s = xt_colour(5, 5, 5));           TEST_EQUAL(s, "\x1b[38;5;231m");
    TRY(s = xt_colour_bg(0, 0, 0));        TEST_EQUAL(s, "\x1b[48;5;16m");
    TRY(s = xt_colour_bg(1, 2, 3));        TEST_EQUAL(s, "\x1b[48;5;67m");
    TRY(s = xt_colour_bg(5, 4, 3));        TEST_EQUAL(s, "\x1b[48;5;223m");
    TRY(s = xt_colour_bg(5, 5, 5));        TEST_EQUAL(s, "\x1b[48;5;231m");
    TRY(s = xt_colour(Int3(0, 0, 0)));     TEST_EQUAL(s, "\x1b[38;5;16m");
    TRY(s = xt_colour(Int3(1, 2, 3)));     TEST_EQUAL(s, "\x1b[38;5;67m");
    TRY(s = xt_colour(Int3(5, 4, 3)));     TEST_EQUAL(s, "\x1b[38;5;223m");
    TRY(s = xt_colour(Int3(5, 5, 5)));     TEST_EQUAL(s, "\x1b[38;5;231m");
    TRY(s = xt_colour_bg(Int3(0, 0, 0)));  TEST_EQUAL(s, "\x1b[48;5;16m");
    TRY(s = xt_colour_bg(Int3(1, 2, 3)));  TEST_EQUAL(s, "\x1b[48;5;67m");
    TRY(s = xt_colour_bg(Int3(5, 4, 3)));  TEST_EQUAL(s, "\x1b[48;5;223m");
    TRY(s = xt_colour_bg(Int3(5, 5, 5)));  TEST_EQUAL(s, "\x1b[48;5;231m");
    TRY(s = xt_grey(1));                   TEST_EQUAL(s, "\x1b[38;5;232m");
    TRY(s = xt_grey(10));                  TEST_EQUAL(s, "\x1b[38;5;241m");
    TRY(s = xt_grey(24));                  TEST_EQUAL(s, "\x1b[38;5;255m");
    TRY(s = xt_grey_bg(1));                TEST_EQUAL(s, "\x1b[48;5;232m");
    TRY(s = xt_grey_bg(10));               TEST_EQUAL(s, "\x1b[48;5;241m");
    TRY(s = xt_grey_bg(24));               TEST_EQUAL(s, "\x1b[48;5;255m");

}
