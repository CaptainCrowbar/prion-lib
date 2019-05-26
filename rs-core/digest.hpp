#pragma once

#include "rs-core/common.hpp"
#include "rs-core/int128.hpp"
#include <array>
#include <cstring>
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

    template <typename T, size_t C = 2, size_t D = 4>
    class SipHash {
    public:
        using key_type = SmallerType<T, uint64_t>;
        using result_type = T;
        static constexpr size_t key_size = 2 * sizeof(key_type);
        static constexpr size_t result_size = sizeof(T);
        SipHash() noexcept { clear(0, 0); }
        SipHash(key_type k0, key_type k1) noexcept { clear(k0, k1); }
        explicit SipHash(const uint8_t* keyptr) noexcept { clear(keyptr); }
        void clear() noexcept;
        void clear(key_type k0, key_type k1) noexcept;
        void clear(const uint8_t* keyptr) noexcept;
        void add(const void* ptr, size_t len) noexcept;
        result_type get() noexcept;
    private:
        static constexpr size_t word_size = sizeof(key_type);
        using carry_array = std::array<uint8_t, word_size>;
        using key_array = std::array<key_type, 2>;
        using shift_array = std::array<int, 6>;
        using state_array = std::array<key_type, 4>;
        static constexpr shift_array bit_shift_32 = {{ 5,16,8,7,13,16 }};
        static constexpr shift_array bit_shift_64 = {{ 13,32,16,21,17,32 }};
        static constexpr shift_array bit_shift = result_size == 4 ? bit_shift_32 : bit_shift_64;
        static constexpr state_array init_state_32 = {{
            key_type(0),
            key_type(0),
            key_type(0x6c79'6765ul),
            key_type(0x7465'6462ul),
        }};
        static constexpr state_array init_state_64 = {{
            key_type(0x736f'6d65'7073'6575ull),
            key_type(0x646f'7261'6e64'6f6dull),
            key_type(0x6c79'6765'6e65'7261ull),
            key_type(0x7465'6462'7974'6573ull),
        }};
        static constexpr state_array init_state = result_size == 4 ? init_state_32 : init_state_64;
        carry_array carry_;
        key_array key_;
        state_array state_;
        size_t carry_bytes_ = 0;
        size_t total_bytes_ = 0;
        result_type result_;
        bool finalized_ = false;
        template <size_t N> void rounds(state_array& state) noexcept;
    };

        template <typename T, size_t C, size_t D>
        void SipHash<T, C, D>::clear() noexcept {
            state_ = init_state;
            for (int i = 0; i < 4; ++i)
                state_[i] ^= key_[i & 1];
            if (result_size == 16)
                state_[1] ^= 0xee;
            carry_bytes_ = total_bytes_ = 0;
            finalized_ = false;
        }

        template <typename T, size_t C, size_t D>
        void SipHash<T, C, D>::clear(key_type k0, key_type k1) noexcept {
            key_[0] = k0;
            key_[1] = k1;
            clear();
        }

        template <typename T, size_t C, size_t D>
        void SipHash<T, C, D>::clear(const uint8_t* keyptr) noexcept {
            read_le(keyptr, key_[0]);
            read_le(keyptr + word_size, key_[1]);
            clear();
        }

        template <typename T, size_t C, size_t D>
        void SipHash<T, C, D>::add(const void* ptr, size_t len) noexcept {
            total_bytes_ += len;
            if (carry_bytes_ + len < word_size) {
                std::memcpy(carry_.data() + carry_bytes_, ptr, len);
                carry_bytes_ += len;
                return;
            }
            auto inptr = static_cast<const uint8_t*>(ptr);
            if (carry_bytes_ > 0) {
                size_t offset = word_size - carry_bytes_;
                std::memcpy(carry_.data() + carry_bytes_, inptr, offset);
                key_type x = read_le<key_type>(carry_.data());
                state_[3] ^= x;
                rounds<C>(state_);
                state_[0] ^= x;
                inptr += offset;
                len -= offset;
            }
            carry_bytes_ = len & (word_size - 1);
            auto endptr = inptr + len - carry_bytes_;
            for (; inptr != endptr; inptr += word_size) {
                key_type x = read_le<key_type>(inptr);
                state_[3] ^= x;
                rounds<C>(state_);
                state_[0] ^= x;
            }
            std::memcpy(carry_.data(), inptr, carry_bytes_);
        }

        template <typename T, size_t C, size_t D>
        typename SipHash<T, C, D>::result_type SipHash<T, C, D>::get() noexcept {
            if (finalized_)
                return result_;
            key_type x = key_type(total_bytes_) << (8 * (word_size - 1));
            for (size_t i = 0; i < carry_bytes_; ++i)
                x |= key_type(carry_[i]) << (8 * i);
            auto final = state_;
            final[3] ^= x;
            rounds<C>(final);
            final[0] ^= x;
            final[2] ^= result_size == 16 ? 0xee : 0xff;
            rounds<D>(final);
            if constexpr (result_size == 4)
                x = final[1] ^ final[3];
            else
                x = final[0] ^ final[1] ^ final[2] ^ final[3];
            std::array<uint8_t, result_size> buf;
            write_le(x, buf.data());
            if (result_size == 16) {
                final[1] ^= 0xdd;
                rounds<D>(final);
                x = final[0] ^ final[1] ^ final[2] ^ final[3];
                write_le(x, buf.data() + 8);
            }
            result_ = read_le<T>(buf.data());
            finalized_ = true;
            return result_;
        }

        template <typename T, size_t C, size_t D>
        template <size_t N>
        inline void SipHash<T, C, D>::rounds(state_array& state) noexcept {
            for (size_t i = 0; i < N; ++i) {
                state[0] += state[1];
                state[1] = rotl(state[1], bit_shift[0]);
                state[1] ^= state[0];
                state[0] = rotl(state[0], bit_shift[1]);
                state[2] += state[3];
                state[3] = rotl(state[3], bit_shift[2]);
                state[3] ^= state[2];
                state[0] += state[3];
                state[3] = rotl(state[3], bit_shift[3]);
                state[3] ^= state[0];
                state[2] += state[1];
                state[1] = rotl(state[1], bit_shift[4]);
                state[1] ^= state[2];
                state[2] = rotl(state[2], bit_shift[5]);
            }
        }

    using SipHash32 = SipHash<uint32_t>;
    using SipHash64 = SipHash<uint64_t>;
    using SipHash128 = SipHash<Uint128>;

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
