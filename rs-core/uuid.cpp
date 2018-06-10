#include "rs-core/uuid.hpp"
#include <algorithm>
#include <stdexcept>

namespace RS {

    // Class Uuid

    Uuid::Uuid(const void* ptr, size_t n) noexcept {
        if (! ptr)
            n = 0;
        if (n > 16)
            n = 16;
        if (ptr && n)
            memcpy(bytes, ptr, n);
        if (n < 16)
            memset(bytes + n, 0, 16 - n);
    }

    Uuid::Uuid(Uview s) {
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

    Ustring Uuid::str() const {
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

}
