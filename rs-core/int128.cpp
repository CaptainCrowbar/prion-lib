#include "rs-core/int128.hpp"
#include <functional>
#include <stdexcept>
#include <string>

namespace RS {

    // Class Uint128

    Uint128::Uint128(Uview str, int base):
    Uint128() {
        if (base < 2 || base > 35)
            throw std::invalid_argument("Invalid base: " + std::to_string(base));
        Uint128 digit, scale = base;
        bool any = false;
        for (auto c: str) {
            if (c == ' ' || c == '\'' || c == ',' || c == '_')
                continue;
            digit.low() = ~ uint64_t(0);
            if (ascii_isdigit(c))
                digit.low() = uint64_t(c - '0');
            else if (ascii_isupper(c))
                digit.low() = uint64_t(c - 'A' + 10);
            else if (ascii_islower(c))
                digit.low() = uint64_t(c - 'a' + 10);
            if (digit.low() >= uint64_t(base))
                throw std::invalid_argument("Invalid integer: " + quote(str));
            *this *= scale;
            *this += digit;
            any = true;
        }
        if (! any)
            throw std::invalid_argument("Invalid integer: " + quote(str));
    }

    size_t Uint128::hash() const noexcept {
        using h64 = std::hash<uint64_t>;
        size_t h = 0;
        hash_combine(h, h64()(low()), h64()(high()));
        return h;
    }

    Ustring Uint128::str(int base, int digits) const {
        if (base < 2 || base > 35)
            throw std::invalid_argument("Invalid base: " + std::to_string(base));
        bool shortcut = has_single_bit(base);
        size_t width = std::max(digits, 0);
        Uint128 scale = base, x = *this;
        Ustring s;
        if (shortcut) {
            int bits = bit_width(base) - 1;
            Uint128 mask = scale - 1;
            while (x || s.size() < width) {
                Uint128 r = x & mask;
                x >>= bits;
                if (r < 10)
                    s += char('0' + r);
                else
                    s += char('a' + r - 10);
            }
        } else {
            while (x || s.size() < width) {
                auto qr = divide(x, scale);
                x = qr.first;
                if (qr.second < 10)
                    s += char('0' + qr.second);
                else
                    s += char('a' + qr.second - 10);
            }
        }
        std::reverse(s.begin(), s.end());
        return s;
    }

}
