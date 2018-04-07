#pragma once

#include "rs-core/common.hpp"
#include "rs-core/digest.hpp"
#include "rs-core/random.hpp"
#include <algorithm>
#include <ostream>
#include <stdexcept>

namespace RS {

    // UUID

    class Uuid:
    public LessThanComparable<Uuid> {
    public:
        Uuid() noexcept { memset(bytes, 0, 16); }
        Uuid(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h,
            uint8_t i, uint8_t j, uint8_t k, uint8_t l, uint8_t m, uint8_t n, uint8_t o, uint8_t p) noexcept:
            bytes{a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p} {}
        Uuid(const void* ptr, size_t n) noexcept;
        explicit Uuid(Uview s);
        uint8_t& operator[](size_t i) noexcept { return bytes[i]; }
        const uint8_t& operator[](size_t i) const noexcept { return bytes[i]; }
        uint8_t* begin() noexcept { return bytes; }
        const uint8_t* begin() const noexcept { return bytes; }
        uint8_t* end() noexcept { return bytes + 16; }
        const uint8_t* end() const noexcept { return bytes + 16; }
        size_t hash() const noexcept { return Djb2a()(bytes, 16); }
        size_t size() const noexcept { return 16; }
        Ustring str() const;
        friend bool operator==(const Uuid& lhs, const Uuid& rhs) noexcept { return memcmp(lhs.bytes, rhs.bytes, 16) == 0; }
        friend bool operator<(const Uuid& lhs, const Uuid& rhs) noexcept { return memcmp(lhs.bytes, rhs.bytes, 16) == -1; }
    private:
        uint8_t bytes[16];
        static bool is_alnum(char c) noexcept { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
        static bool is_xdigit(char c) noexcept { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); }
    };

    inline Uuid::Uuid(const void* ptr, size_t n) noexcept {
        if (! ptr)
            n = 0;
        if (n > 16)
            n = 16;
        if (ptr && n)
            memcpy(bytes, ptr, n);
        if (n < 16)
            memset(bytes + n, 0, 16 - n);
    }

    inline Uuid::Uuid(Uview s) {
        auto i = s.begin(), end_s = s.end();
        auto j = begin(), end_u = end();
        int rc = 0;
        while (i != end_s && j != end_u && rc != -1) {
            i = std::find_if(i, end_s, is_xdigit);
            if (i == end_s)
                break;
            rc = RS_Detail::decode_hex_byte(i, end_s);
            if (rc == -1)
                break;
            *j++ = uint8_t(rc);
        }
        if (rc == -1 || j != end_u || std::find_if(i, end_s, is_alnum) != end_s)
            throw std::invalid_argument("Invalid UUID: " + Ustring(s));
    }

    inline Ustring Uuid::str() const {
        Ustring s;
        s.reserve(36);
        int i = 0;
        for (; i < 4; ++i)
            RS_Detail::append_hex_byte(bytes[i], s);
        s += '-';
        for (; i < 6; ++i)
            RS_Detail::append_hex_byte(bytes[i], s);
        s += '-';
        for (; i < 8; ++i)
            RS_Detail::append_hex_byte(bytes[i], s);
        s += '-';
        for (; i < 10; ++i)
            RS_Detail::append_hex_byte(bytes[i], s);
        s += '-';
        for (; i < 16; ++i)
            RS_Detail::append_hex_byte(bytes[i], s);
        return s;
    }

    inline std::ostream& operator<<(std::ostream& o, const Uuid& u) { return o << u.str(); }
    inline Ustring to_str(const Uuid& u) { return u.str(); }

    struct RandomUuid {
        using result_type = Uuid;
        template <typename RNG> Uuid operator()(RNG& rng) const {
            Uuid u;
            random_bytes(rng, u.begin(), 16);
            u[6] = (u[6] & 0x0f) | 0x40;
            u[8] = (u[8] & 0x3f) | 0x80;
            return u;
        }
    };

}

RS_DEFINE_STD_HASH(RS::Uuid);
