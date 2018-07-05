#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include "rs-core/vector.hpp"
#include <chrono>
#include <functional>
#include <iostream>
#include <ostream>
#include <string>

namespace RS {

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

    // Terminal controls

    namespace Xterm {

        constexpr const char* up           = "\x1b[A";    // Cursor up
        constexpr const char* down         = "\x1b[B";    // Cursor down
        constexpr const char* right        = "\x1b[C";    // Cursor right
        constexpr const char* left         = "\x1b[D";    // Cursor left
        constexpr const char* erase_left   = "\x1b[1K";   // Erase left
        constexpr const char* erase_right  = "\x1b[K";    // Erase right
        constexpr const char* erase_above  = "\x1b[1J";   // Erase above
        constexpr const char* erase_below  = "\x1b[J";    // Erase below
        constexpr const char* erase_line   = "\x1b[2K";   // Erase line
        constexpr const char* clear        = "\x1b[2J";   // Clear screen
        constexpr const char* reset        = "\x1b[0m";   // Reset attributes
        constexpr const char* bold         = "\x1b[1m";   // Bold
        constexpr const char* under        = "\x1b[4m";   // Underline
        constexpr const char* bold_off     = "\x1b[22m";  // Bold off
        constexpr const char* under_off    = "\x1b[24m";  // Underline off
        constexpr const char* colour_off   = "\x1b[39m";  // Colour off
        constexpr const char* black        = "\x1b[30m";  // Black fg
        constexpr const char* red          = "\x1b[31m";  // Red fg
        constexpr const char* green        = "\x1b[32m";  // Green fg
        constexpr const char* yellow       = "\x1b[33m";  // Yellow fg
        constexpr const char* blue         = "\x1b[34m";  // Blue fg
        constexpr const char* magenta      = "\x1b[35m";  // Magenta fg
        constexpr const char* cyan         = "\x1b[36m";  // Cyan fg
        constexpr const char* white        = "\x1b[37m";  // White fg
        constexpr const char* black_bg     = "\x1b[40m";  // Black bg
        constexpr const char* red_bg       = "\x1b[41m";  // Red bg
        constexpr const char* green_bg     = "\x1b[42m";  // Green bg
        constexpr const char* yellow_bg    = "\x1b[43m";  // Yellow bg
        constexpr const char* blue_bg      = "\x1b[44m";  // Blue bg
        constexpr const char* magenta_bg   = "\x1b[45m";  // Magenta bg
        constexpr const char* cyan_bg      = "\x1b[46m";  // Cyan bg
        constexpr const char* white_bg     = "\x1b[47m";  // White bg

        inline Ustring move_up(int n) { return "\x1b[" + std::to_string(n) + 'A'; }     // Cursor up n spaces
        inline Ustring move_down(int n) { return "\x1b[" + std::to_string(n) + 'B'; }   // Cursor down n spaces
        inline Ustring move_right(int n) { return "\x1b[" + std::to_string(n) + 'C'; }  // Cursor right n spaces
        inline Ustring move_left(int n) { return "\x1b[" + std::to_string(n) + 'D'; }   // Cursor left n spaces

    };

    // Terminal colours

    class Xcolour:
    public LessThanComparable<Xcolour> {
    public:
        constexpr Xcolour() = default;
        constexpr explicit Xcolour(int grey) noexcept: code(uint8_t(231 + clamp(grey, 1, 24))) {}
        explicit Xcolour(Int3 rgb) noexcept: Xcolour(rgb[0], rgb[1], rgb[2]) {}
        constexpr Xcolour(int r, int g, int b) noexcept: code(uint8_t(36 * cc(r) + 6 * cc(g) + cc(b) + 16)) {}
        constexpr bool is_null() const noexcept { return code == 0; }
        constexpr bool is_grey() const noexcept { return code >= 232; }
        constexpr bool is_rgb() const noexcept { return code >= 16 && code <= 231; }
        constexpr int grey() const noexcept { return is_grey() ? int(code - 231) : 0; }
        constexpr int r() const noexcept { return is_rgb() ? int(code - 16) / 36 % 6 : 0; }
        constexpr int g() const noexcept { return is_rgb() ? int(code - 16) / 6 % 6 : 0; }
        constexpr int b() const noexcept { return is_rgb() ? int(code - 16) % 6 : 0; }
        constexpr int index() const noexcept { return int(code); }
        Int3 as_rgb24() const noexcept { return is_rgb() ? Int3(c8(r()), c8(g()), c8(b())) : is_grey() ? Int3(y8(grey())) : Int3(); }
        Ustring fg() const { return code == 0 ? Ustring() : "\x1b[38;5;" + std::to_string(code) + "m"; }
        Ustring bg() const { return code == 0 ? Ustring() : "\x1b[48;5;" + std::to_string(code) + "m"; }
        Ustring str() const;
        static constexpr Xcolour from_index(int i) noexcept { Xcolour xc; xc.code = uint8_t(clamp(i, 0, 255)); return xc; }
        static Xcolour from_str(Uview str);
        friend bool operator==(Xcolour lhs, Xcolour rhs) noexcept { return lhs.index() == rhs.index(); }
        friend bool operator<(Xcolour lhs, Xcolour rhs) noexcept { return lhs.index() < rhs.index(); }
        friend std::ostream& operator<<(std::ostream& out, Xcolour xc) { return out << xc.str(); }
        friend Ustring to_str(Xcolour xc) { return xc.str(); }
    private:
        uint8_t code = 0;
        static constexpr int cc(int i) noexcept { return clamp(i, 0, 5); }
        static constexpr int c8(int i) noexcept { return i ? 40 * i + 55 : 0; }
        static constexpr int y8(int i) noexcept { return i ? 10 * i - 2 : 0; }
    };

    // Terminal I/O functions

    bool is_stdout_redirected() noexcept;

}

namespace std {

    template <>
    class hash<RS::Xcolour> {
    public:
        size_t operator()(RS::Xcolour xc) const noexcept { return std::hash<int>()(xc.index()); }
    };

}
