#pragma once

#include "rs-core/common.hpp"
#include "rs-core/float.hpp"
#include "rs-core/io.hpp"
#include "rs-core/string.hpp"
#include "rs-core/time.hpp"
#include "rs-core/vector.hpp"
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string>

namespace RS {

    // Terminal I/O operations

    inline bool is_stdout_redirected() noexcept { return ! RS_IO_FUNCTION(isatty)(1); }

    namespace RS_Detail {

        inline int xt_encode_grey(int n) noexcept {
            return 231 + clamp(n, 1, 24);
        }

        inline int xt_encode_rgb(int r, int g, int b) noexcept {
            r = clamp(r, 0, 5);
            g = clamp(g, 0, 5);
            b = clamp(b, 0, 5);
            return 36 * r + 6 * g + b + 16;
        }

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

    inline ProgressBar::ProgressBar(Uview label, size_t length, std::ostream& out):
    bar_length(length), current_pos(0), bar_offset(label.empty() ? 1 : label.size() + 1), out_ptr(&out) {
        using namespace std::chrono;
        if (bar_length == 0) {
            auto w = getenv("WIDTH");
            if (w && *w)
                bar_length = std::strtoul(w, nullptr, 10);
            if (bar_length == 0)
                bar_length = 80;
            bar_length -= bar_offset + tail_length + 4;
        }
        if (! label.empty())
            *out_ptr << xt_yellow << label << " ";
        *out_ptr << Ustring(xt_cyan) << "[" << xt_blue << Ustring(bar_length, '-') << xt_cyan << "] " << Ustring(tail_length, ' ') << xt_reset << std::flush;
        start_time = system_clock::now();
    }

    inline ProgressBar::~ProgressBar() noexcept {
        try { *out_ptr << (xt_move_left(tail_length) + xt_erase_right) << std::endl; }
            catch (...) {}
    }

    inline void ProgressBar::operator()(double x) {
        using namespace std::chrono;
        x = clamp(x, 0, 1);
        Ustring message;
        auto now = system_clock::now();
        if (x > 0 && x < 1 && now > start_time) {
            auto elapsed = duration_cast<Dseconds>(now - start_time);
            auto estimate = elapsed * ((1 - x) / x);
            double count = estimate.count();
            char unit = 's';
            if (count >= 86400) {
                count /= 86400;
                unit = 'd';
            } else if (count >= 3600) {
                count /= 3600;
                unit = 'h';
            } else if (count >= 60) {
                count /= 60;
                unit = 'm';
            }
            message = "ETA " + fp_format(count, 'g', 2) + unit;
        }
        message.resize(tail_length, ' ');
        size_t new_pos = std::max(iround<size_t>(x * bar_length), current_pos);
        size_t n_left = bar_length - current_pos + tail_length + 2;
        size_t n_advance = new_pos - current_pos;
        size_t n_right = bar_length - new_pos + 2;
        *out_ptr << xt_move_left(int(n_left)) << xt_green << Ustring(n_advance, '+') << xt_move_right(int(n_right)) << xt_yellow << message << xt_reset << std::flush;
        current_pos = new_pos;
    }

}
