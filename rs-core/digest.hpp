#pragma once

#include "rs-core/common.hpp"
#include <array>
#include <string_view>
#include <type_traits>

RS_LDLIB(cygwin: crypto);
RS_LDLIB(linux: crypto);
RS_LDLIB(msvc: advapi32);

namespace RS {

    template <typename Hash> auto digest(const void* ptr, size_t len) { Hash h; h.add(ptr, len); return h.get(); }
    template <typename Hash> auto digest(std::string_view view) { Hash h; h.add(view.data(), view.size()); return h.get(); }

    template <uint32_t Seed, uint32_t Mod>
    class Multiplicative {
    public:
        using result_type = uint32_t;
        void add(const void* ptr, size_t len) noexcept {
            auto bptr = static_cast<const uint8_t*>(ptr);
            for (size_t i = 0; i < len; ++i)
                hash = Mod * hash + bptr[i];
        }
        void clear() noexcept { hash = Seed; }
        uint32_t get() noexcept { return hash; }
    private:
        uint32_t hash = Seed;
    };

    using Bernstein = Multiplicative<5381, 33>;
    using Kernighan = Multiplicative<0, 31>;

    class Crc32 {
    public:
        using result_type = uint32_t;
        void add(const void* ptr, size_t len) noexcept;
        void clear() noexcept { hash = mask; }
        uint32_t get() noexcept { return hash ^ mask; }
    private:
        static constexpr auto mask = ~ uint32_t(0);
        uint32_t hash = mask;
    };

    class Djb2a {
    public:
        using result_type = uint32_t;
        void add(const void* ptr, size_t len) noexcept {
            auto bptr = static_cast<const uint8_t*>(ptr);
            for (size_t i = 0; i < len; ++i)
                hash = ((hash << 5) + hash) ^ bptr[i];
        }
        void clear() noexcept { hash = seed; }
        uint32_t get() noexcept { return hash; }
    private:
        static constexpr uint32_t seed = 5381;
        uint32_t hash = seed;
    };

    class Fnv1a_32 {
    public:
        using result_type = uint32_t;
        void add(const void* ptr, size_t len) noexcept {
            auto bptr = static_cast<const uint8_t*>(ptr);
            for (size_t i = 0; i < len; ++i)
                hash = (hash ^ bptr[i]) * prime;
        }
        void clear() noexcept { hash = offset; }
        uint32_t get() noexcept { return hash; }
    private:
        static constexpr uint32_t offset = 0x811c9dc5;
        static constexpr uint32_t prime = 16777619u;
        uint32_t hash = offset;
    };

    class Fnv1a_64 {
    public:
        using result_type = uint64_t;
        void add(const void* ptr, size_t len) noexcept {
            auto bptr = static_cast<const uint8_t*>(ptr);
            for (size_t i = 0; i < len; ++i)
                hash = (hash ^ bptr[i]) * prime;
        }
        void clear() noexcept { hash = offset; }
        uint64_t get() noexcept { return hash; }
    private:
        static constexpr uint64_t offset = 0xcbf29ce484222325ull;
        static constexpr uint64_t prime = 1099511628211ull;
        uint64_t hash = offset;
    };

    using Fnv1a_std = std::conditional_t<sizeof(size_t) <= 4, Fnv1a_32, Fnv1a_64>;

    class Murmur3_32 {
    public:
        using result_type = uint32_t;
        Murmur3_32() = default;
        explicit Murmur3_32(uint32_t seed) noexcept { clear(seed); }
        void add(const void* ptr, size_t len) noexcept;
        void clear() noexcept { hash = seedval; total = word = carried = 0; }
        void clear(uint32_t seed) noexcept { seedval = seed; clear(); }
        uint32_t get() noexcept;
    private:
        static constexpr uint32_t c1 = 0xcc9e2d51;
        static constexpr uint32_t c2 = 0x1b873593;
        static constexpr uint32_t c3 = 0xe6546b64;
        static constexpr uint32_t c4 = 0x85ebca6b;
        static constexpr uint32_t c5 = 0xc2b2ae35;
        uint32_t hash = 0;
        uint32_t seedval = 0;
        uint32_t total = 0;
        uint32_t word = 0;
        uint32_t carried = 0;
    };

    #define RS_DEFINE_HASH_CLASS(ClassName, bits) \
        class ClassName { \
        public: \
            RS_NO_COPY_MOVE(ClassName); \
            using result_type = std::array<uint8_t, bits / 8>; \
            ClassName() = default; \
            ~ClassName() { get(); } \
            void add(const void* ptr, size_t len); \
            void clear() { get(); hash = {}; } \
            result_type get(); \
        private: \
            struct impl_type; \
            result_type hash = {}; \
            impl_type* impl = nullptr; \
        };

    RS_DEFINE_HASH_CLASS(Md5, 128);
    RS_DEFINE_HASH_CLASS(Sha1, 160);
    RS_DEFINE_HASH_CLASS(Sha256, 256);
    RS_DEFINE_HASH_CLASS(Sha512, 512);

    #undef RS_DEFINE_HASH_CLASS

}
