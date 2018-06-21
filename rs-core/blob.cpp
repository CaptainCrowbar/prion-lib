#include "rs-core/blob.hpp"
#include "rs-core/digest.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <new>
#include <stdexcept>

namespace RS {

    size_t Blob::hash() const noexcept {
        std::string_view view(*this);
        return std_hash(view);
    }

    Ustring Blob::hex(size_t block) const {
        return hexdump(ptr, len, block);
    }

    std::string Blob::str() const {
        return empty() ? std::string() : std::string(c_data(), len);
    }

    void Blob::swap(Blob& b) noexcept {
        std::swap(ptr, b.ptr);
        std::swap(len, b.len);
        del.swap(b.del);
    }

    Blob Blob::from_hex(const Ustring& s) {
        std::string buf;
        buf.reserve(s.size() / 2);
        auto i = s.begin(), end = s.end();
        while (i != end) {
            auto j = std::find_if(i, end, ascii_isalnum);
            if (j == end)
                break;
            i = std::find_if_not(j, end, ascii_isxdigit);
            if ((i != end && ascii_isalnum(*i)) || (j - i) % 2)
                throw std::invalid_argument("Invalid hex string");
            while (j != i)
                buf += char(RS_Detail::decode_hex_byte(j, i));
        }
        Blob b(buf.size());
        memcpy(b.data(), buf.data(), buf.size());
        return b;
    }

    void Blob::init(size_t n) {
        if (n) {
            ptr = std::malloc(n);
            if (! ptr)
                throw std::bad_alloc();
            len = n;
            del = &std::free;
        }
    }

    void Blob::init(const void* p, size_t n) {
        if (p && n) {
            init(n);
            memcpy(ptr, p, n);
        }
    }

    bool operator==(const Blob& lhs, const Blob& rhs) noexcept {
        return lhs.size() == rhs.size() && memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
    }

    bool operator<(const Blob& lhs, const Blob& rhs) noexcept {
        auto cmp = memcmp(lhs.data(), rhs.data(), std::min(lhs.size(), rhs.size()));
        return cmp < 0 || (cmp == 0 && lhs.size() < rhs.size());
    }

}
