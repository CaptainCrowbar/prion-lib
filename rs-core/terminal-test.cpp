#include "rs-core/terminal.hpp"
#include "rs-core/time.hpp"
#include "rs-core/unit-test.hpp"
#include "rs-core/vector.hpp"
#include <chrono>
#include <iostream>
#include <thread>

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

// void test_core_terminal_progress_bar() {

//     double scale = 100;
//     auto runtime = 2s;
//     auto interval = runtime / scale;
//     ProgressBar bar("Hello");
//     for (int i = 0; i <= 100; ++i) {
//         bar(i / scale);
//         std::this_thread::sleep_for(interval);
//     }

// }

// void test_core_terminal_xterm_24bit() {

//     for (int x = 0; x <= 255; x += 17)
//         std::cout << hex(x, 2)
//             << "  \x1b[48;2;" << x << ";0;0m        \x1b[0m"
//             << "  \x1b[48;2;0;" << x << ";0m        \x1b[0m"
//             << "  \x1b[48;2;0;0;" << x << "m        \x1b[0m"
//             << "\n";

// }

void test_core_terminal_xterm_control_codes() {

    Ustring s;

    TRY(s = Xterm::up);              TEST_EQUAL(s, "\x1b[A");
    TRY(s = Xterm::down);            TEST_EQUAL(s, "\x1b[B");
    TRY(s = Xterm::right);           TEST_EQUAL(s, "\x1b[C");
    TRY(s = Xterm::left);            TEST_EQUAL(s, "\x1b[D");
    TRY(s = Xterm::erase_left);      TEST_EQUAL(s, "\x1b[1K");
    TRY(s = Xterm::erase_right);     TEST_EQUAL(s, "\x1b[K");
    TRY(s = Xterm::erase_above);     TEST_EQUAL(s, "\x1b[1J");
    TRY(s = Xterm::erase_below);     TEST_EQUAL(s, "\x1b[J");
    TRY(s = Xterm::erase_line);      TEST_EQUAL(s, "\x1b[2K");
    TRY(s = Xterm::clear);           TEST_EQUAL(s, "\x1b[2J");
    TRY(s = Xterm::reset);           TEST_EQUAL(s, "\x1b[0m");
    TRY(s = Xterm::bold);            TEST_EQUAL(s, "\x1b[1m");
    TRY(s = Xterm::under);           TEST_EQUAL(s, "\x1b[4m");
    TRY(s = Xterm::black);           TEST_EQUAL(s, "\x1b[30m");
    TRY(s = Xterm::red);             TEST_EQUAL(s, "\x1b[31m");
    TRY(s = Xterm::green);           TEST_EQUAL(s, "\x1b[32m");
    TRY(s = Xterm::yellow);          TEST_EQUAL(s, "\x1b[33m");
    TRY(s = Xterm::blue);            TEST_EQUAL(s, "\x1b[34m");
    TRY(s = Xterm::magenta);         TEST_EQUAL(s, "\x1b[35m");
    TRY(s = Xterm::cyan);            TEST_EQUAL(s, "\x1b[36m");
    TRY(s = Xterm::white);           TEST_EQUAL(s, "\x1b[37m");
    TRY(s = Xterm::black_bg);        TEST_EQUAL(s, "\x1b[40m");
    TRY(s = Xterm::red_bg);          TEST_EQUAL(s, "\x1b[41m");
    TRY(s = Xterm::green_bg);        TEST_EQUAL(s, "\x1b[42m");
    TRY(s = Xterm::yellow_bg);       TEST_EQUAL(s, "\x1b[43m");
    TRY(s = Xterm::blue_bg);         TEST_EQUAL(s, "\x1b[44m");
    TRY(s = Xterm::magenta_bg);      TEST_EQUAL(s, "\x1b[45m");
    TRY(s = Xterm::cyan_bg);         TEST_EQUAL(s, "\x1b[46m");
    TRY(s = Xterm::white_bg);        TEST_EQUAL(s, "\x1b[47m");
    TRY(s = Xterm::move_up(42));     TEST_EQUAL(s, "\x1b[42A");
    TRY(s = Xterm::move_down(42));   TEST_EQUAL(s, "\x1b[42B");
    TRY(s = Xterm::move_right(42));  TEST_EQUAL(s, "\x1b[42C");
    TRY(s = Xterm::move_left(42));   TEST_EQUAL(s, "\x1b[42D");

}

void test_core_terminal_xterm_colour_class() {

    Xcolour xc;
    Ustring s;
    Int3 v;

    TEST_EQUAL(sizeof(Xcolour), 1);

    TRY(xc = {});                         TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 0);
    TRY(xc = Xcolour(-1));                TEST_EQUAL(xc.grey(), 1);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 232);
    TRY(xc = Xcolour(1));                 TEST_EQUAL(xc.grey(), 1);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 232);
    TRY(xc = Xcolour(10));                TEST_EQUAL(xc.grey(), 10);  TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 241);
    TRY(xc = Xcolour(24));                TEST_EQUAL(xc.grey(), 24);  TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 255);
    TRY(xc = Xcolour(99));                TEST_EQUAL(xc.grey(), 24);  TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 255);
    TRY(xc = Xcolour(-1, -1, -1));        TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 16);
    TRY(xc = Xcolour(0, 0, 0));           TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 16);
    TRY(xc = Xcolour(1, 2, 3));           TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 1);  TEST_EQUAL(xc.g(), 2);  TEST_EQUAL(xc.b(), 3);  TEST_EQUAL(xc.index(), 67);
    TRY(xc = Xcolour(5, 4, 3));           TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 4);  TEST_EQUAL(xc.b(), 3);  TEST_EQUAL(xc.index(), 223);
    TRY(xc = Xcolour(5, 5, 5));           TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 5);  TEST_EQUAL(xc.b(), 5);  TEST_EQUAL(xc.index(), 231);
    TRY(xc = Xcolour(99, 99, 99));        TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 5);  TEST_EQUAL(xc.b(), 5);  TEST_EQUAL(xc.index(), 231);
    TRY(xc = Xcolour(Int3(-1, -1, -1)));  TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 16);
    TRY(xc = Xcolour(Int3(0, 0, 0)));     TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);  TEST_EQUAL(xc.index(), 16);
    TRY(xc = Xcolour(Int3(1, 2, 3)));     TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 1);  TEST_EQUAL(xc.g(), 2);  TEST_EQUAL(xc.b(), 3);  TEST_EQUAL(xc.index(), 67);
    TRY(xc = Xcolour(Int3(5, 4, 3)));     TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 4);  TEST_EQUAL(xc.b(), 3);  TEST_EQUAL(xc.index(), 223);
    TRY(xc = Xcolour(Int3(5, 5, 5)));     TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 5);  TEST_EQUAL(xc.b(), 5);  TEST_EQUAL(xc.index(), 231);
    TRY(xc = Xcolour(Int3(99, 99, 99)));  TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 5);  TEST_EQUAL(xc.b(), 5);  TEST_EQUAL(xc.index(), 231);

    TRY(xc = {});                TEST(xc.is_null());    TEST(! xc.is_grey());  TEST(! xc.is_rgb());
    TRY(xc = Xcolour(1));        TEST(! xc.is_null());  TEST(xc.is_grey());    TEST(! xc.is_rgb());
    TRY(xc = Xcolour(10));       TEST(! xc.is_null());  TEST(xc.is_grey());    TEST(! xc.is_rgb());
    TRY(xc = Xcolour(24));       TEST(! xc.is_null());  TEST(xc.is_grey());    TEST(! xc.is_rgb());
    TRY(xc = Xcolour(0, 0, 0));  TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());
    TRY(xc = Xcolour(1, 2, 3));  TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());
    TRY(xc = Xcolour(5, 4, 3));  TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());
    TRY(xc = Xcolour(5, 5, 5));  TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());

    TRY(xc = {});                TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[0,0,0]");
    TRY(xc = Xcolour(0, 0, 0));  TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[0,0,0]");
    TRY(xc = Xcolour(1, 2, 3));  TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[95,135,175]");
    TRY(xc = Xcolour(5, 4, 3));  TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[255,215,175]");
    TRY(xc = Xcolour(5, 5, 5));  TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[255,255,255]");
    TRY(xc = Xcolour(1));        TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[8,8,8]");
    TRY(xc = Xcolour(10));       TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[98,98,98]");
    TRY(xc = Xcolour(24));       TRY(v = xc.as_rgb24());  TRY(s = to_str(v));  TEST_EQUAL(s, "[238,238,238]");

    TRY(xc = {});                TRY(s = xc.fg());  TEST_EQUAL(s, "");                TRY(s = xc.bg());  TEST_EQUAL(s, "");
    TRY(xc = Xcolour(1));        TRY(s = xc.fg());  TEST_EQUAL(s, "\x1b[38;5;232m");  TRY(s = xc.bg());  TEST_EQUAL(s, "\x1b[48;5;232m");
    TRY(xc = Xcolour(10));       TRY(s = xc.fg());  TEST_EQUAL(s, "\x1b[38;5;241m");  TRY(s = xc.bg());  TEST_EQUAL(s, "\x1b[48;5;241m");
    TRY(xc = Xcolour(24));       TRY(s = xc.fg());  TEST_EQUAL(s, "\x1b[38;5;255m");  TRY(s = xc.bg());  TEST_EQUAL(s, "\x1b[48;5;255m");
    TRY(xc = Xcolour(0, 0, 0));  TRY(s = xc.fg());  TEST_EQUAL(s, "\x1b[38;5;16m");   TRY(s = xc.bg());  TEST_EQUAL(s, "\x1b[48;5;16m");
    TRY(xc = Xcolour(1, 2, 3));  TRY(s = xc.fg());  TEST_EQUAL(s, "\x1b[38;5;67m");   TRY(s = xc.bg());  TEST_EQUAL(s, "\x1b[48;5;67m");
    TRY(xc = Xcolour(5, 4, 3));  TRY(s = xc.fg());  TEST_EQUAL(s, "\x1b[38;5;223m");  TRY(s = xc.bg());  TEST_EQUAL(s, "\x1b[48;5;223m");
    TRY(xc = Xcolour(5, 5, 5));  TRY(s = xc.fg());  TEST_EQUAL(s, "\x1b[38;5;231m");  TRY(s = xc.bg());  TEST_EQUAL(s, "\x1b[48;5;231m");

    TRY(xc = {});                      TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour()");
    TRY(xc = Xcolour(1));              TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour(1)");
    TRY(xc = Xcolour(10));             TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour(10)");
    TRY(xc = Xcolour(24));             TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour(24)");
    TRY(xc = Xcolour(0, 0, 0));        TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour(0,0,0)");
    TRY(xc = Xcolour(1, 2, 3));        TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour(1,2,3)");
    TRY(xc = Xcolour(5, 4, 3));        TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour(5,4,3)");
    TRY(xc = Xcolour(5, 5, 5));        TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour(5,5,5)");
    TRY(xc = Xcolour::from_index(1));  TRY(s = xc.repr());  TEST_EQUAL(s, "Xcolour::from_index(1)");

    TRY(xc = Xcolour::from_index(0));    TEST_EQUAL(xc.index(), 0);    TEST(xc.is_null());    TEST(! xc.is_grey());  TEST(! xc.is_rgb());
    TRY(xc = Xcolour::from_index(16));   TEST_EQUAL(xc.index(), 16);   TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());
    TRY(xc = Xcolour::from_index(67));   TEST_EQUAL(xc.index(), 67);   TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());
    TRY(xc = Xcolour::from_index(223));  TEST_EQUAL(xc.index(), 223);  TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());
    TRY(xc = Xcolour::from_index(231));  TEST_EQUAL(xc.index(), 231);  TEST(! xc.is_null());  TEST(! xc.is_grey());  TEST(xc.is_rgb());
    TRY(xc = Xcolour::from_index(232));  TEST_EQUAL(xc.index(), 232);  TEST(! xc.is_null());  TEST(xc.is_grey());    TEST(! xc.is_rgb());
    TRY(xc = Xcolour::from_index(241));  TEST_EQUAL(xc.index(), 241);  TEST(! xc.is_null());  TEST(xc.is_grey());    TEST(! xc.is_rgb());
    TRY(xc = Xcolour::from_index(255));  TEST_EQUAL(xc.index(), 255);  TEST(! xc.is_null());  TEST(xc.is_grey());    TEST(! xc.is_rgb());
    TRY(xc = Xcolour::from_index(999));  TEST_EQUAL(xc.index(), 255);  TEST(! xc.is_null());  TEST(xc.is_grey());    TEST(! xc.is_rgb());

    TRY(xc = Xcolour::from_index(0));    TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);
    TRY(xc = Xcolour::from_index(16));   TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);
    TRY(xc = Xcolour::from_index(67));   TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 1);  TEST_EQUAL(xc.g(), 2);  TEST_EQUAL(xc.b(), 3);
    TRY(xc = Xcolour::from_index(223));  TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 4);  TEST_EQUAL(xc.b(), 3);
    TRY(xc = Xcolour::from_index(231));  TEST_EQUAL(xc.grey(), 0);   TEST_EQUAL(xc.r(), 5);  TEST_EQUAL(xc.g(), 5);  TEST_EQUAL(xc.b(), 5);
    TRY(xc = Xcolour::from_index(232));  TEST_EQUAL(xc.grey(), 1);   TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);
    TRY(xc = Xcolour::from_index(241));  TEST_EQUAL(xc.grey(), 10);  TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);
    TRY(xc = Xcolour::from_index(255));  TEST_EQUAL(xc.grey(), 24);  TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);
    TRY(xc = Xcolour::from_index(999));  TEST_EQUAL(xc.grey(), 24);  TEST_EQUAL(xc.r(), 0);  TEST_EQUAL(xc.g(), 0);  TEST_EQUAL(xc.b(), 0);

}
