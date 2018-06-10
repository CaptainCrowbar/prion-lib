#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include "rs-core/vector.hpp"
#include <chrono>
#include <iostream>
#include <ostream>
#include <string>

namespace RS {

    // Terminal I/O operations

    bool is_stdout_redirected() noexcept;

    namespace RS_Detail {

        int xt_encode_grey(int n) noexcept;
        int xt_encode_rgb(int r, int g, int b) noexcept;

    }

    constexpr const char* xt_up           = "\x1b[A";    // Cursor up
    constexpr const char* xt_down         = "\x1b[B";    // Cursor down
    constexpr const char* xt_right        = "\x1b[C";    // Cursor right
    constexpr const char* xt_left         = "\x1b[D";    // Cursor left
    constexpr const char* xt_erase_left   = "\x1b[1K";   // Erase left
    constexpr const char* xt_erase_right  = "\x1b[K";    // Erase right
    constexpr const char* xt_erase_above  = "\x1b[1J";   // Erase above
    constexpr const char* xt_erase_below  = "\x1b[J";    // Erase below
    constexpr const char* xt_erase_line   = "\x1b[2K";   // Erase line
    constexpr const char* xt_clear        = "\x1b[2J";   // Clear screen
    constexpr const char* xt_reset        = "\x1b[0m";   // Reset attributes
    constexpr const char* xt_bold         = "\x1b[1m";   // Bold
    constexpr const char* xt_under        = "\x1b[4m";   // Underline
    constexpr const char* xt_bold_off     = "\x1b[22m";  // Bold off
    constexpr const char* xt_under_off    = "\x1b[24m";  // Underline off
    constexpr const char* xt_colour_off   = "\x1b[39m";  // Colour off
    constexpr const char* xt_black        = "\x1b[30m";  // Black fg
    constexpr const char* xt_red          = "\x1b[31m";  // Red fg
    constexpr const char* xt_green        = "\x1b[32m";  // Green fg
    constexpr const char* xt_yellow       = "\x1b[33m";  // Yellow fg
    constexpr const char* xt_blue         = "\x1b[34m";  // Blue fg
    constexpr const char* xt_magenta      = "\x1b[35m";  // Magenta fg
    constexpr const char* xt_cyan         = "\x1b[36m";  // Cyan fg
    constexpr const char* xt_white        = "\x1b[37m";  // White fg
    constexpr const char* xt_black_bg     = "\x1b[40m";  // Black bg
    constexpr const char* xt_red_bg       = "\x1b[41m";  // Red bg
    constexpr const char* xt_green_bg     = "\x1b[42m";  // Green bg
    constexpr const char* xt_yellow_bg    = "\x1b[43m";  // Yellow bg
    constexpr const char* xt_blue_bg      = "\x1b[44m";  // Blue bg
    constexpr const char* xt_magenta_bg   = "\x1b[45m";  // Magenta bg
    constexpr const char* xt_cyan_bg      = "\x1b[46m";  // Cyan bg
    constexpr const char* xt_white_bg     = "\x1b[47m";  // White bg

    inline std::string xt_move_up(int n) { return "\x1b[" + dec(n) + 'A'; }                                                       // Cursor up n spaces
    inline std::string xt_move_down(int n) { return "\x1b[" + dec(n) + 'B'; }                                                     // Cursor down n spaces
    inline std::string xt_move_right(int n) { return "\x1b[" + dec(n) + 'C'; }                                                    // Cursor right n spaces
    inline std::string xt_move_left(int n) { return "\x1b[" + dec(n) + 'D'; }                                                     // Cursor left n spaces
    inline std::string xt_colour(int r, int g, int b) { return "\x1b[38;5;" + dec(RS_Detail::xt_encode_rgb(r, g, b)) + 'm'; }     // Set fg colour to an RGB value (0-5)
    inline std::string xt_colour(Int3 rgb) { return xt_colour(rgb.x(), rgb.y(), rgb.z()); }                                       // Set fg colour to an RGB value (0-5)
    inline std::string xt_colour_bg(int r, int g, int b) { return "\x1b[48;5;" + dec(RS_Detail::xt_encode_rgb(r, g, b)) + 'm'; }  // Set bg colour to an RGB value (0-5)
    inline std::string xt_colour_bg(Int3 rgb) { return xt_colour_bg(rgb.x(), rgb.y(), rgb.z()); }                                 // Set fg colour to an RGB value (0-5)
    inline std::string xt_grey(int grey) { return "\x1b[38;5;" + dec(RS_Detail::xt_encode_grey(grey)) + 'm'; }                    // Set fg colour to a grey level (1-24)
    inline std::string xt_grey_bg(int grey) { return "\x1b[48;5;" + dec(RS_Detail::xt_encode_grey(grey)) + 'm'; }                 // Set bg colour to a grey level (1-24)

    // Progress bar

    class ProgressBar {
    public:
        RS_NO_COPY_MOVE(ProgressBar);
        explicit ProgressBar(Uview label, size_t length = 0, std::ostream& out = std::cout);
        ~ProgressBar() noexcept;
        void operator()(double x);
    private:
        static constexpr size_t tail_length = 10;
        size_t bar_length;
        size_t current_pos;
        size_t bar_offset;
        std::ostream* out_ptr;
        std::chrono::system_clock::time_point start_time;
    };

}
