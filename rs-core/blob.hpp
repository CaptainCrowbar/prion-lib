#pragma once

#include "rs-core/common.hpp"
#include "rs-core/digest.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <new>
#include <stdexcept>
#include <string>
#include <utility>

namespace RS {

    class Blob:
    public LessThanComparable<Blob> {
    public:
        Blob() = default;
        explicit Blob(size_t n) { init(n); }
        Blob(size_t n, uint8_t x) { init(n); memset(ptr, x, len); }
        Blob(void* p, size_t n): Blob(p, n, &std::free) {}
        template <typename F> Blob(void* p, size_t n, F f) { if (p && n) {ptr = p; len = n; del = f; } }
        ~Blob() noexcept { if (ptr && del) try { del(ptr); } catch (...) {} }
        Blob(const Blob& b) { init(b.data(), b.size()); }
        Blob(Blob&& b) noexcept: ptr(b.ptr), len(b.len) { del.swap(b.del); }
        Blob& operator=(const Blob& b) { copy(b.data(), b.size()); return *this; }
        Blob& operator=(Blob&& b) noexcept { Blob b2(std::move(b)); swap(b2); return *this; }
        operator string_view() const noexcept { return {c_data(), len}; }
        void* data() noexcept { return ptr; }
        const void* data() const noexcept { return ptr; }
        uint8_t* b_data() noexcept { return static_cast<uint8_t*>(ptr); }
        const uint8_t* b_data() const noexcept { return static_cast<const uint8_t*>(ptr); }
        char* c_data() noexcept { return static_cast<char*>(ptr); }
        const char* c_data() const noexcept { return static_cast<const char*>(ptr); }
        Irange<uint8_t*> bytes() noexcept { return {b_data(), b_data() + len}; }
        Irange<const uint8_t*> bytes() const noexcept { return {b_data(), b_data() + len}; }
        Irange<char*> chars() noexcept { return {c_data(), c_data() + len}; }
        Irange<const char*> chars() const noexcept { return {c_data(), c_data() + len}; }
        void clear() noexcept { Blob b; swap(b); }
        void copy(const void* p, size_t n) { Blob b; b.init(p, n); swap(b); }
        bool empty() const noexcept { return len == 0; }
        void fill(uint8_t x) noexcept { memset(ptr, x, len); }
        size_t hash() const noexcept { return Djb2a()(ptr, len); }
        Ustring hex(size_t block = 0) const { return hexdump(ptr, len, block); }
        void reset(size_t n) { Blob b(n); swap(b); }
        void reset(size_t n, uint8_t x) { Blob b(n, x); swap(b); }
        void reset(void* p, size_t n) { Blob b(p, n); swap(b); }
        template <typename F> void reset(void* p, size_t n, F f) { Blob b(p, n, f); swap(b); }
        size_t size() const noexcept { return len; }
        std::string str() const { return empty() ? std::string() : std::string(c_data(), len); }
        void swap(Blob& b) noexcept { std::swap(ptr, b.ptr); std::swap(len, b.len); del.swap(b.del); }
        static Blob from_hex(const Ustring& s);
    private:
        void* ptr = nullptr;
        size_t len = 0;
        std::function<void(void*)> del;
        void init(size_t n);
        void init(const void* p, size_t n);
    };

    inline Blob Blob::from_hex(const Ustring& s) {
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

    inline void Blob::init(size_t n) {
        if (n) {
            ptr = std::malloc(n);
            if (! ptr)
                throw std::bad_alloc();
            len = n;
            del = &std::free;
        }
    }

    inline void Blob::init(const void* p, size_t n) {
        if (p && n) {
            init(n);
            memcpy(ptr, p, n);
        }
    }

    inline bool operator==(const Blob& lhs, const Blob& rhs) noexcept {
        return lhs.size() == rhs.size() && memcmp(lhs.data(), rhs.data(), lhs.size()) == 0;
    }

    inline bool operator<(const Blob& lhs, const Blob& rhs) noexcept {
        auto cmp = memcmp(lhs.data(), rhs.data(), std::min(lhs.size(), rhs.size()));
        return cmp < 0 || (cmp == 0 && lhs.size() < rhs.size());
    }

    inline void swap(Blob& b1, Blob& b2) noexcept { b1.swap(b2); }

}

RS_DEFINE_STD_HASH(RS::Blob);
