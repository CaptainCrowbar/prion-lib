#include "rs-core/mp-integer.hpp"
#include "rs-core/string.hpp"
#include <cstring>
#include <stdexcept>

namespace RS {

    // Class Nat

    Nat::Nat(uint64_t x) {
        if (x > 0)
            rep.push_back(uint32_t(x));
        if (x > mask32)
            rep.push_back(uint32_t(x >> 32));
    }

    Nat& Nat::operator+=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(std::max(rep.size(), rhs.rep.size()), 0);
        uint64_t sum = 0;
        for (size_t i = 0; i < common; ++i) {
            sum += uint64_t(rep[i]) + uint64_t(rhs.rep[i]);
            rep[i] = uint32_t(sum);
            sum >>= 32;
        }
        const auto* rptr = &rep;
        if (rhs.rep.size() > common)
            rptr = &rhs.rep;
        for (size_t i = common; i < rep.size(); ++i) {
            sum += uint64_t((*rptr)[i]);
            rep[i] = uint32_t(sum);
            sum >>= 32;
        }
        if (sum)
            rep.push_back(uint32_t(sum));
        trim();
        return *this;
    }

    Nat& Nat::operator-=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        bool carry = false;
        for (size_t i = 0; i < common; ++i) {
            bool c = rep[i] < rhs.rep[i] || (carry && rep[i] == rhs.rep[i]);
            rep[i] -= rhs.rep[i];
            if (carry)
                --rep[i];
            carry = c;
        }
        for (size_t i = common; carry && i < rep.size(); ++i)
            carry = --rep[i] == mask32;
        trim();
        return *this;
    }

    Nat& Nat::operator&=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(common);
        for (size_t i = 0; i < common; ++i)
            rep[i] &= rhs.rep[i];
        trim();
        return *this;
    }

    Nat& Nat::operator|=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(std::max(rep.size(), rhs.rep.size()));
        for (size_t i = 0; i < common; ++i)
            rep[i] |= rhs.rep[i];
        return *this;
    }

    Nat& Nat::operator^=(const Nat& rhs) {
        size_t common = std::min(rep.size(), rhs.rep.size());
        rep.resize(std::max(rep.size(), rhs.rep.size()));
        for (size_t i = 0; i < common; ++i)
            rep[i] ^= rhs.rep[i];
        trim();
        return *this;
    }

    Nat& Nat::operator<<=(ptrdiff_t rhs) {
        if (rhs == 0)
            return *this;
        if (rhs < 0)
            return *this >>= - rhs;
        size_t words = rhs / 32;
        int bits = rhs % 32;
        uint32_t prev = 0;
        if (bits > 0) {
            for (auto& word: rep) {
                uint32_t next = word >> (32 - bits);
                word = (word << bits) | prev;
                prev = next;
            }
        }
        if (prev)
            rep.push_back(prev);
        rep.insert(rep.begin(), words, 0);
        return *this;
    }

    Nat& Nat::operator>>=(ptrdiff_t rhs) {
        if (rhs == 0)
            return *this;
        if (rhs < 0)
            return *this <<= - rhs;
        size_t words = rhs / 32;
        int bits = rhs % 32;
        if (words >= rep.size()) {
            rep.clear();
        } else {
            rep.erase(rep.begin(), rep.begin() + words);
            if (bits > 0) {
                uint32_t prev = 0;
                for (size_t i = rep.size() - 1; i != npos; --i) {
                    uint32_t next = rep[i] << (32 - bits);
                    rep[i] = (rep[i] >> bits) | prev;
                    prev = next;
                }
            }
        }
        trim();
        return *this;
    }

    size_t Nat::bits() const noexcept {
        size_t n = 32 * rep.size();
        if (! rep.empty())
            n -= 32 - ilog2p1(rep.back());
        return n;
    }

    size_t Nat::bits_set() const noexcept {
        size_t n = 0;
        for (auto i: rep)
            n += popcount(i);
        return n;
    }

    size_t Nat::bytes() const noexcept {
        if (rep.empty())
            return 0;
        else
            return 4 * (rep.size() - 1) + size_t(rep.back() > 0) + size_t(rep.back() > size_t(0xff))
                + size_t(rep.back() > size_t(0xffff)) + size_t(rep.back() > size_t(0xffffff));
    }

    int Nat::compare(const Nat& rhs) const noexcept {
        if (rep.size() != rhs.rep.size())
            return rep.size() < rhs.rep.size() ? -1 : 1;
        for (size_t i = rep.size() - 1; i != npos; --i)
            if (rep[i] != rhs.rep[i])
                return rep[i] < rhs.rep[i] ? -1 : 1;
        return 0;
    }

    bool Nat::get_bit(size_t i) const noexcept {
        if (i < 32 * rep.size())
            return (rep[i / 32] >> (i % 32)) & 1;
        else
            return false;
    }

    uint8_t Nat::get_byte(size_t i) const noexcept {
        if (i < 4 * rep.size())
            return (rep[i / 4] >> (i % 4 * 8)) & 0xff;
        else
            return 0;
    }

    void Nat::set_bit(size_t i, bool b) {
        bool in_rep = i < 32 * rep.size();
        if (b) {
            if (! in_rep)
                rep.resize(i / 32 + 1, 0);
            rep[i / 32] |= uint32_t(1) << (i % 32);
        } else if (in_rep) {
            rep[i / 32] &= ~ (uint32_t(1) << (i % 32));
            trim();
        }
    }

    void Nat::set_byte(size_t i, uint8_t b) {
        if (i >= 4 * rep.size())
            rep.resize(i / 4 + 1, 0);
        rep[i / 4] |= uint32_t(b) << (i % 4 * 8);
        trim();
    }

    void Nat::flip_bit(size_t i) {
        if (i >= 32 * rep.size())
            rep.resize(i / 32 + 1, 0);
        rep[i / 32] ^= uint32_t(1) << (i % 32);
        trim();
    }

    Nat Nat::pow(const Nat& n) const {
        Nat x = *this, y = n, z = 1;
        while (y) {
            if (y.is_odd())
                z *= x;
            x *= x;
            y >>= 1;
        }
        return z;
    }

    Ustring Nat::str(int base, size_t digits) const {
        if (base < 2 || base > 36)
            throw std::invalid_argument("Invalid base: " + RS::dec(base));
        if (rep.empty())
            return Ustring(digits, '0');
        Ustring s;
        if (base == 2) {
            s = RS::bin(rep.back(), 1);
            for (size_t i = rep.size() - 2; i != npos; --i)
                s += RS::bin(rep[i], 32);
        } else if (base == 16) {
            s = RS::hex(rep.back(), 1);
            for (size_t i = rep.size() - 2; i != npos; --i)
                s += RS::hex(rep[i], 8);
        } else if (base <= 10) {
            Nat b = base, q, r, t = *this;
            while (t) {
                do_divide(t, b, q, r);
                s += char(int(r) + '0');
                t = std::move(q);
            }
            std::reverse(s.begin(), s.end());
        } else {
            Nat b = base, q, r, t = *this;
            while (t) {
                do_divide(t, b, q, r);
                int d = int(r);
                if (d < 10)
                    s += char(d + '0');
                else
                    s += char(d - 10 + 'a');
                t = std::move(q);
            }
            std::reverse(s.begin(), s.end());
        }
        if (s.size() < digits)
            s.insert(0, digits - s.size(), '0');
        return s;
    }

    void Nat::write_be(void* ptr, size_t n) const noexcept {
        size_t nb = std::min(n, bytes());
        std::memset(ptr, 0, n - nb);
        auto bp = static_cast<uint8_t*>(ptr) + n - nb, end = bp + nb;
        while (bp != end)
            *bp++ = get_byte(--nb);
    }

    void Nat::write_le(void* ptr, size_t n) const noexcept {
        auto bp = static_cast<uint8_t*>(ptr);
        size_t nb = std::min(n, bytes());
        for (size_t i = 0; i < nb; ++i)
            bp[i] = get_byte(i);
        std::memset(bp + nb, 0, n - nb);
    }

    Nat Nat::from_double(double x) {
        int e = 0;
        double m = frexp(fabs(x), &e);
        Nat n = uint64_t(floor(ldexp(m, 64)));
        n <<= e - 64;
        return n;
    }

    Nat Nat::read_be(const void* ptr, size_t n) {
        Nat result;
        result.rep.resize((n + 3) / 4);
        auto bp = static_cast<const uint8_t*>(ptr);
        for (size_t i = 0, j = n - 1; i < n; ++i, --j)
            result.set_byte(j, bp[i]);
        result.trim();
        return result;
    }

    Nat Nat::read_le(const void* ptr, size_t n) {
        Nat result;
        result.rep.resize((n + 3) / 4);
        auto bp = static_cast<const uint8_t*>(ptr);
        for (size_t i = 0; i < n; ++i)
            result.set_byte(i, bp[i]);
        result.trim();
        return result;
    }

    void Nat::do_divide(const Nat& x, const Nat& y, Nat& q, Nat& r) {
        if (! y)
            throw std::domain_error("Division by zero");
        Nat quo, rem = x;
        if (x >= y) {
            size_t shift = x.bits() - y.bits();
            Nat rsub = y;
            rsub <<= shift;
            if (rsub > x) {
                --shift;
                rsub >>= 1;
            }
            Nat qadd = 1;
            qadd <<= shift;
            while (qadd) {
                if (rem >= rsub) {
                    rem -= rsub;
                    quo += qadd;
                }
                rsub >>= 1;
                qadd >>= 1;
            }
        }
        q = std::move(quo);
        r = std::move(rem);
    }

    void Nat::do_multiply(const Nat& x, const Nat& y, Nat& z) {
        if (! x || ! y) {
            z.rep.clear();
        } else {
            size_t m = x.rep.size(), n = y.rep.size();
            z.rep.assign(m + n, 0);
            uint64_t carry = 0;
            for (size_t k = 0; k <= m + n - 2; ++k) {
                carry += uint64_t(z.rep[k]);
                z.rep[k] = uint32_t(carry);
                carry >>= 32;
                size_t i_min = k < n ? 0 : k - n + 1;
                size_t i_max = k < m ? k : m - 1;
                for (size_t i = i_min; i <= i_max; ++i) {
                    uint64_t p = uint64_t(x.rep[i]) * uint64_t(y.rep[k - i]) + uint64_t(z.rep[k]);
                    z.rep[k] = uint32_t(p);
                    carry += uint32_t(p >> 32);
                }
            }
            z.rep[m + n - 1] = uint32_t(carry);
            z.trim();
        }
    }

    void Nat::init(Uview s, int base) {
        if (base != 0 && base != 2 && base != 10 && base != 16)
            throw std::invalid_argument("Invalid base: " + RS::dec(base));
        if (s.empty())
            return;
        auto ptr = s.data(), end = ptr + s.size();
        if (base == 0) {
            if (s[0] != '0' || s.size() < 3)
                base = 10;
            else if (s[1] == 'B' || s[1] == 'b')
                base = 2;
            else if (s[1] == 'X' || s[1] == 'x')
                base = 16;
            else
                base = 10;
            if (base != 10)
                ptr += 2;
        }
        Nat nbase = base;
        int digit = 0;
        auto digit_f = base == 2 ? digit_2 : base == 16 ? digit_16 : digit_10;
        for (; ptr != end; ++ptr) {
            if (*ptr == '\'')
                continue;
            digit = digit_f(*ptr);
            if (digit == -1)
                break;
            *this *= nbase;
            *this += digit;
        }
    }

    void Nat::trim() noexcept {
        size_t i = rep.size() - 1;
        while (i != npos && rep[i] == 0)
            --i;
        rep.resize(i + 1);
    }

    // Class Int

    Int& Int::operator+=(const Int& rhs) {
        if (! rhs.mag) {
            // do nothing
        } else if (! mag) {
            mag = rhs.mag;
            neg = rhs.neg;
        } else if (neg == rhs.neg) {
            mag += rhs.mag;
        } else {
            switch (mag.compare(rhs.mag)) {
                case -1:
                    mag = rhs.mag - mag;
                    neg = ! neg;
                    break;
                case 1:
                    mag -= rhs.mag;
                    break;
                default:
                    mag = {};
                    neg = false;
                    break;
            }
        }
        return *this;
    }

    int Int::compare(const Int& rhs) const noexcept {
        if (neg != rhs.neg)
            return neg ? -1 : 1;
        int c = mag.compare(rhs.mag);
        return neg ? - c : c;
    }

    Int Int::pow(const Int& n) const {
        if (n.neg)
            throw std::domain_error("Negative exponent in integer expression");
        Int z;
        z.mag = mag.pow(n.mag);
        z.neg = neg && n.mag.is_odd();
        return z;
    }

    Ustring Int::str(int base, size_t digits, bool sign) const {
        Ustring s = mag.str(base, digits);
        if (neg)
            s.insert(s.begin(), '-');
        else if (sign)
            s.insert(s.begin(), '+');
        return s;
    }

    Int Int::from_double(double x) {
        Int i = Nat::from_double(x);
        if (x < 0)
            i = - i;
        return i;
    }

    void Int::init(Uview s, int base) {
        if (base != 0 && base != 2 && base != 10 && base != 16)
            throw std::invalid_argument("Invalid base: " + RS::dec(base));
        if (s.empty())
            return;
        auto b = s.data(), e = b + s.size();
        neg = *b == '-';
        if (*b == '+' || *b == '-')
            ++b;
        Uview u(b, e - b);
        mag.init(u, base);
        neg &= bool(mag);
    }

    void Int::do_divide(const Int& x, const Int& y, Int& q, Int& r) {
        Int quo, rem;
        Nat::do_divide(x.mag, y.mag, quo.mag, rem.mag);
        if (rem.mag && (x.neg || y.neg)) {
            if (x.neg) {
                ++quo.mag;
                rem.mag = y.mag - rem.mag;
            }
        }
        quo.neg = bool(quo.mag) && x.neg != y.neg;
        q = std::move(quo);
        r = std::move(rem);
    }

    void Int::do_multiply(const Int& x, const Int& y, Int& z) {
        Nat::do_multiply(x.mag, y.mag, z.mag);
        z.neg = bool(x) && bool(y) && x.neg != y.neg;
    }


}
