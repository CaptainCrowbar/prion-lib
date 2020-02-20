#include "rs-core/random.hpp"
#include <cstring>

using namespace RS::Literals;

namespace RS {

    // Class Isaac32

    namespace {

        void mix32(uint32_t* a) noexcept {
            a[0] ^= a[1] << 11;  a[3] += a[0];  a[1] += a[2];
            a[1] ^= a[2] >> 2;   a[4] += a[1];  a[2] += a[3];
            a[2] ^= a[3] << 8;   a[5] += a[2];  a[3] += a[4];
            a[3] ^= a[4] >> 16;  a[6] += a[3];  a[4] += a[5];
            a[4] ^= a[5] << 10;  a[7] += a[4];  a[5] += a[6];
            a[5] ^= a[6] >> 4;   a[0] += a[5];  a[6] += a[7];
            a[6] ^= a[7] << 8;   a[1] += a[6];  a[7] += a[0];
            a[7] ^= a[0] >> 9;   a[2] += a[7];  a[0] += a[1];
        }

        void step32(const uint32_t* mem, uint32_t*& m1, uint32_t*& m2, uint32_t*& r,
                uint32_t& a, uint32_t& b, uint32_t& x, uint32_t& y, uint32_t mix) noexcept {
            x = *m1;
            a = (a ^ mix) + *m2++;
            *m1++ = y = mem[(x >> 2) & 255] + a + b;
            *r++ = b = mem[(y >> 10) & 255] + x;
        }

    }

    uint32_t Isaac32::operator()() noexcept {
        if (index == 256)
            next_block();
        return res[index++];
    }

    void Isaac32::seed(const uint32_t* sptr, size_t len) noexcept {
        std::memcpy(res, sptr, 4 * std::min(len, 256_sz));
        if (len < 256)
            std::memset(res + len, 0, 4 * (256 - len));
        for (size_t i = 256; i < len; ++i)
            res[i % 256] ^= sptr[i];
        std::memset(mem, 0, sizeof(mem));
        a = b = c = 0;
        uint32_t array[8];
        std::fill_n(array, 8, 0x9e3779b9_u32);
        for (int i = 0; i < 4; ++i)
            mix32(array);
        for (int i = 0; i < 256; i += 8) {
            for (int j = 0; j < 8; ++j)
                array[j] += res[i + j];
            mix32(array);
            for (int j = 0; j < 8; ++j)
                mem[i + j] = array[j];
        }
        for (int i = 0; i < 256; i += 8) {
            for (int j = 0; j < 8; ++j)
                array[j] += mem[i + j];
            mix32(array);
            for (int j = 0; j < 8; ++j)
                mem[i + j] = array[j];
        }
        next_block();
    }

    void Isaac32::seed(std::initializer_list<uint32_t> s) noexcept {
        std::vector<uint32_t> v{s};
        seed(v.data(), v.size());
    }

    void Isaac32::next_block() noexcept {
        b += ++c;
        uint32_t* end = mem + 128;
        uint32_t* m1 = mem;
        uint32_t* m2 = end;
        uint32_t* r = res;
        uint32_t x = 0, y = 0;
        while (m1 < end) {
            step32(mem, m1, m2, r, a, b, x, y, a << 13);
            step32(mem, m1, m2, r, a, b, x, y, a >> 6);
            step32(mem, m1, m2, r, a, b, x, y, a << 2);
            step32(mem, m1, m2, r, a, b, x, y, a >> 16);
        }
        m2 = mem;
        while (m2 < end) {
            step32(mem, m1, m2, r, a, b, x, y, a << 13);
            step32(mem, m1, m2, r, a, b, x, y, a >> 6);
            step32(mem, m1, m2, r, a, b, x, y, a << 2);
            step32(mem, m1, m2, r, a, b, x, y, a >> 16);
        }
        index = 0;
    }

    // Class Isaac64

    namespace {

        void mix64(uint64_t* a) noexcept {
            a[0] -= a[4];  a[5] ^= a[7] >> 9;   a[7] += a[0];
            a[1] -= a[5];  a[6] ^= a[0] << 9;   a[0] += a[1];
            a[2] -= a[6];  a[7] ^= a[1] >> 23;  a[1] += a[2];
            a[3] -= a[7];  a[0] ^= a[2] << 15;  a[2] += a[3];
            a[4] -= a[0];  a[1] ^= a[3] >> 14;  a[3] += a[4];
            a[5] -= a[1];  a[2] ^= a[4] << 20;  a[4] += a[5];
            a[6] -= a[2];  a[3] ^= a[5] >> 17;  a[5] += a[6];
            a[7] -= a[3];  a[4] ^= a[6] << 14;  a[6] += a[7];
        }

        void step64(const uint64_t* mem, uint64_t*& m1, uint64_t*& m2, uint64_t*& r,
                uint64_t& a, uint64_t& b, uint64_t& x, uint64_t& y, uint64_t mix) noexcept {
            x = *m1;
            a = mix + *m2++;
            *m1++ = y = mem[(x >> 3) & 255] + a + b;
            *r++ = b = mem[(y >> 11) & 255] + x;
        }

    }

    uint64_t Isaac64::operator()() noexcept {
        if (index == 256)
            next_block();
        return res[index++];
    }

    void Isaac64::seed(const uint64_t* sptr, size_t len) noexcept {
        std::memcpy(res, sptr, 8 * std::min(len, 256_sz));
        if (len < 256)
            std::memset(res + len, 0, 8 * (256 - len));
        for (size_t i = 256; i < len; ++i)
            res[i % 256] ^= sptr[i];
        std::memset(mem, 0, sizeof(mem));
        a = b = c = 0;
        uint64_t array[8];
        std::fill_n(array, 8, 0x9e3779b97f4a7c13_u64);
        for (int i = 0; i < 4; ++i)
            mix64(array);
        for (int i = 0; i < 256; i += 8) {
            for (int j = 0; j < 8; ++j)
                array[j] += res[i + j];
            mix64(array);
            for (int j = 0; j < 8; ++j)
                mem[i + j] = array[j];
        }
        for (int i = 0; i < 256; i += 8) {
            for (int j = 0; j < 8; ++j)
                array[j] += mem[i + j];
            mix64(array);
            for (int j = 0; j < 8; ++j)
                mem[i + j] = array[j];
        }
        next_block();
    }

    void Isaac64::seed(std::initializer_list<uint64_t> s) noexcept {
        std::vector<uint64_t> v{s};
        seed(v.data(), v.size());
    }

    void Isaac64::next_block() noexcept {
        b += ++c;
        uint64_t* end = mem + 128;
        uint64_t* m1 = mem;
        uint64_t* m2 = end;
        uint64_t* r = res;
        uint64_t x = 0, y = 0;
        while (m1 < end) {
            step64(mem, m1, m2, r, a, b, x, y, ~ (a ^ (a << 21)));
            step64(mem, m1, m2, r, a, b, x, y, a ^ (a >> 5));
            step64(mem, m1, m2, r, a, b, x, y, a ^ (a << 12));
            step64(mem, m1, m2, r, a, b, x, y, a ^ (a >> 33));
        }
        m2 = mem;
        while (m2 < end) {
            step64(mem, m1, m2, r, a, b, x, y, ~ (a ^ (a << 21)));
            step64(mem, m1, m2, r, a, b, x, y, a ^ (a >> 5));
            step64(mem, m1, m2, r, a, b, x, y, a ^ (a << 12));
            step64(mem, m1, m2, r, a, b, x, y, a ^ (a >> 33));
        }
        index = 0;
    }

    // Random device engines

    uint32_t Urandom32::operator()() {
        if constexpr (sizeof(unsigned) >= 4) {
            return uint32_t(dev_());
        } else {
            static const auto gen = random_integer(min(), max());
            return gen(dev_);
        }
    }

    uint64_t Urandom64::operator()() {
        if constexpr (sizeof(unsigned) >= 8) {
            return uint64_t(dev_());
        } else if constexpr (sizeof(unsigned) >= 4) {
            uint64_t x = uint32_t(dev_());
            uint64_t y = uint32_t(dev_());
            return (x << 32) + y;
        } else {
            static const auto gen = random_integer(min(), max());
            return gen(dev_);
        }
    }

    // Text generators

    Ustring lorem_ipsum(uint64_t seed, size_t bytes, bool paras) {
        static constexpr const char* classic[] = {
            "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. ",
            "Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. ",
            "Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. ",
            "Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum. ",
        };
        static constexpr size_t n_lines = sizeof(classic) / sizeof(classic[0]);
        if (bytes == 0)
            return {};
        Xoshiro rng(seed);
        Ustring text;
        text.reserve(bytes + 20);
        for (size_t i = 0; i < n_lines && text.size() <= bytes; ++i)
            text += classic[i];
        if (paras)
            text.replace(text.size() - 1, 1, "\n\n");
        while (text.size() <= bytes) {
            size_t n_para = paras ? rng() % 7 + 1 : npos;
            for (size_t i = 0; i < n_para && text.size() <= bytes; ++i)
                text += classic[rng() % n_lines];
            if (paras)
                text.replace(text.size() - 1, 1, "\n\n");
        }
        size_t cut = text.find_first_of("\n .,", bytes);
        if (cut != npos)
            text.resize(cut);
        while (! ascii_isalpha(text.back()))
            text.pop_back();
        text += '.';
        if (paras)
            text += '\n';
        return text;
    }

}
