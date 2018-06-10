#include "rs-core/terminal.hpp"
#include "rs-core/float.hpp"
#include "rs-core/time.hpp"
#include <algorithm>
#include <cstdlib>

using namespace std::chrono;

#ifdef _XOPEN_SOURCE
    #define IO_FUNCTION(f) ::f
#else
    #define IO_FUNCTION(f) _##f
#endif

namespace RS {

    // Terminal I/O operations

    bool is_stdout_redirected() noexcept {
        return ! IO_FUNCTION(isatty)(1);
    }

    namespace RS_Detail {

        int xt_encode_grey(int n) noexcept {
            return 231 + clamp(n, 1, 24);
        }

        int xt_encode_rgb(int r, int g, int b) noexcept {
            r = clamp(r, 0, 5);
            g = clamp(g, 0, 5);
            b = clamp(b, 0, 5);
            return 36 * r + 6 * g + b + 16;
        }

    }

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
            *out_ptr << xt_yellow << label << " ";
        *out_ptr << Ustring(xt_cyan) << "[" << xt_blue << Ustring(bar_length, '-') << xt_cyan << "] " << Ustring(tail_length, ' ') << xt_reset << std::flush;
        start_time = system_clock::now();
    }

    ProgressBar::~ProgressBar() noexcept {
        try { *out_ptr << (xt_move_left(tail_length) + xt_erase_right) << std::endl; }
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
        *out_ptr << xt_move_left(int(n_left)) << xt_green << Ustring(n_advance, '+') << xt_move_right(int(n_right)) << xt_yellow << message << xt_reset << std::flush;
        current_pos = new_pos;
    }


}
