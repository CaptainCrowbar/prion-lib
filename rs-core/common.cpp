#include "rs-core/common.hpp"
#include <chrono>
#include <mutex>
#include <random>

namespace RS {

    // Implementation details

    namespace RS_Detail {

        void append_hex_byte(uint8_t b, std::string& s) {
            static constexpr const char* digits = "0123456789abcdef";
            s += digits[b / 16];
            s += digits[b % 16];
        }

    }

    // Memory algorithms

    int mem_compare(const void* lhs, size_t n1, const void* rhs, size_t n2) noexcept {
        if (! lhs || ! rhs)
            return rhs ? -1 : lhs ? 1 : 0;
        int rc = memcmp(lhs, rhs, std::min(n1, n2));
        return rc < 0 ? -1 : rc > 0 ? 1 : n1 < n2 ? -1 : n1 > n2 ? 1 : 0;
    }

    size_t mem_match(const void* lhs, const void* rhs, size_t n) noexcept {
        if (! (lhs && rhs))
            return 0;
        auto cp1 = static_cast<const char*>(lhs);
        auto cp2 = static_cast<const char*>(rhs);
        size_t i = 0;
        while (i < n && cp1[i] == cp2[i])
            ++i;
        return i;
    }

    void mem_swap(void* ptr1, void* ptr2, size_t n) noexcept {
        if (ptr1 == ptr2)
            return;
        uint8_t b;
        auto p = static_cast<uint8_t*>(ptr1), endp = p + n, q = static_cast<uint8_t*>(ptr2);
        while (p != endp) {
            b = *p;
            *p++ = *q;
            *q++ = b;
        }
    }

    // Integer arithmetic functions

    double xbinomial(int a, int b) noexcept {
        if (b < 0 || b > a)
            return 0;
        if (b == 0 || b == a)
            return 1;
        if (b > a / 2)
            b = a - b;
        double n = 1, d = 1;
        while (b > 0) {
            n *= a--;
            d *= b--;
        }
        return n / d;
    }

    // Logging

    namespace {

        int random_text_colour() noexcept {
            using namespace std::chrono;
            static const auto t1 = duration_cast<microseconds>(system_clock::now().time_since_epoch()).count();
            static const auto h1 = uint32_t(std_hash(t1));
            auto t2 = std::this_thread::get_id();
            auto h2 = uint32_t(std_hash(t2));
            std::minstd_rand rng(h1 ^ h2);
            std::uniform_int_distribution<> colours(17, 230);
            return colours(rng);
        }

    }

    namespace RS_Detail {

        void log_message(const Ustring& msg) {
            static std::mutex mtx;
            auto lock = make_lock(mtx);
            Ustring text = "\x1b[38;5;" + std::to_string(random_text_colour()) + "m# " + msg + "\x1b[0m\n";
            fwrite(text.data(), 1, text.size(), stdout);
            fflush(stdout);
        }

    }

    void logx(const Ustring& msg) noexcept {
        try { RS_Detail::log_message(msg); }
        catch (...) {}
    }

    void logx(const char* msg) noexcept {
        try { RS_Detail::log_message(cstr(msg)); }
        catch (...) {}
    }

}
