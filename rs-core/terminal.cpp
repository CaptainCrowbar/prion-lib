#include "rs-core/terminal.hpp"
#include "rs-core/float.hpp"
#include "rs-core/time.hpp"
#include <algorithm>
#include <cstdlib>
#include <stdexcept>

using namespace std::chrono;

#ifdef _XOPEN_SOURCE
    #define IO_FUNCTION(f) ::f
#else
    #define IO_FUNCTION(f) _##f
#endif

namespace RS {

    // Class ProgressBar

    ProgressBar::ProgressBar(Uview label, size_t length, std::ostream& out):
    bar_length(length), current_pos(0), bar_offset(label.empty() ? 1 : label.size() + 1), out_ptr(&out) {
        if (bar_length == 0) {
            auto w = getenv("WIDTH");
            if (w && *w)
                bar_length = std::strtoul(w, nullptr, 10);
            if (bar_length == 0)
                bar_length = 80;
            bar_length -= bar_offset + tail_length + 4;
        }
        if (! label.empty())
            *out_ptr << Xterm::yellow << label << " ";
        *out_ptr << Ustring(Xterm::cyan) << "[" << Xterm::blue << Ustring(bar_length, '-') << Xterm::cyan << "] "
            << Ustring(tail_length, ' ') << Xterm::reset << std::flush;
        start_time = system_clock::now();
    }

    ProgressBar::~ProgressBar() noexcept {
        try { *out_ptr << (Xterm::move_left(tail_length) + Xterm::erase_right) << std::endl; }
            catch (...) {}
    }

    void ProgressBar::operator()(double x) {
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
        *out_ptr << Xterm::move_left(int(n_left)) << Xterm::green << Ustring(n_advance, '+')
            << Xterm::move_right(int(n_right)) << Xterm::yellow << message << Xterm::reset << std::flush;
        current_pos = new_pos;
    }

    // Terminal colours

    Ustring Xcolour::str() const {
        if (is_grey())
            return '[' + std::to_string(grey()) + ']';
        else if (is_rgb())
            return '[' + std::to_string(r()) + ',' + std::to_string(g()) + ',' + std::to_string(b()) + ']';
        else
            return {};
    }

    Xcolour Xcolour::from_str(Uview str) {
        int val[3], nval = 0;
        auto p = str.data(), end = p + str.size();
        while (p != end) {
            p = std::find_if_not(p, end, [] (char c) { return ascii_ispunct(c) || ascii_isspace(c); });
            if (p == end)
                break;
            if (! ascii_isdigit(*p) || nval == 3)
                throw std::invalid_argument("Invalid Xcolour: " + quote(str));
            auto q = std::find_if_not(p, end, ascii_isdigit);
            Uview v(p, q - p);
            val[nval++] = int(decnum(v));
            p = q;
        }
        switch (nval) {
            case 0:   return Xcolour();
            case 1:   return Xcolour(val[0]);
            case 3:   return Xcolour(val[0], val[1], val[2]);
            default:  throw std::invalid_argument("Invalid Xcolour: " + quote(str));
        }
    }

    // Terminal I/O functions

    bool is_stdout_redirected() noexcept {
        return ! IO_FUNCTION(isatty)(1);
    }

}
