#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <array>
#include <cstring>
#include <string>
#include <type_traits>

#if defined(__APPLE__)
    #include <CommonCrypto/CommonDigest.h>
#elif defined(_XOPEN_SOURCE)
    #include <openssl/md5.h>
    #include <openssl/sha.h>
#else
    #include <windows.h>
    #include <wincrypt.h>
#endif

RS_LDLIB(cygwin: crypto);
RS_LDLIB(linux: crypto);
RS_LDLIB(msvc: advapi32);

namespace RS {

    namespace RS_Detail {

        template <size_t Bits>
        struct HashTraits {
            using result_type =
                std::conditional_t<Bits <= 8, uint8_t,
                std::conditional_t<Bits <= 16, uint16_t,
                std::conditional_t<Bits <= 32, uint32_t,
                std::conditional_t<Bits <= 64, uint64_t,
                std::array<uint8_t, (Bits + 7) / 8>>>>>;
        };

    }

    template <size_t Bits>
    class HashFunction {
    public:
        static_assert(Bits > 0 && Bits % 8 == 0);
        static constexpr size_t bits = Bits;
        static constexpr size_t bytes = Bits / 8;
        using result_type = typename RS_Detail::HashTraits<Bits>::result_type;
        virtual ~HashFunction() noexcept {}
        virtual result_type hash(const void* ptr, size_t len) const noexcept = 0;
        result_type operator()(const void* ptr, size_t len) const noexcept { return hash(ptr, len); }
        result_type operator()(const std::string& str) const noexcept { return hash(str.data(), str.size()); }
    };

    class Crc32:
    public HashFunction<32> {
    public:
        virtual uint32_t hash(const void* ptr, size_t len) const noexcept {
            static const auto table = [] {
                std::array<uint32_t, 256> buf;
                for (uint32_t i = 0; i < 256; i++) {
                    uint32_t x = i;
                    for (int k = 0; k < 8; k++) {
                        if (x & 1)
                            x = 0xedb88320 ^ (x >> 1);
                        else
                            x >>= 1;
                    }
                    buf[i] = x;
                }
                return buf;
            }();
            const auto data = static_cast<const uint8_t*>(ptr);
            uint32_t c = 0xffffffff;
            for (size_t i = 0; i < len; i++)
                c = table[(c ^ data[i]) & 0xff] ^ (c >> 8);
            return c ^ 0xffffffff;
        }
    };

    class Djb2a:
    public HashFunction<32> {
    public:
        virtual uint32_t hash(const void* ptr, size_t len) const noexcept {
            const auto data = static_cast<const uint8_t*>(ptr);
            uint32_t hash = 5381;
            for (size_t i = 0; i < len; ++i)
                hash = ((hash << 5) + hash) ^ data[i];
            return hash;
        }
    };

    class Fnv1a_32:
    public HashFunction<32> {
    public:
        virtual uint32_t hash(const void* ptr, size_t len) const noexcept {
            static constexpr uint32_t offset = 0x811c9dc5;
            static constexpr uint32_t prime = 16777619u;
            const auto data = static_cast<const uint8_t*>(ptr);
            auto hash = offset;
            for (size_t i = 0; i < len; ++i)
                hash = (hash ^ data[i]) * prime;
            return hash;
        }
    };

    class Fnv1a_64:
    public HashFunction<64> {
    public:
        virtual uint64_t hash(const void* ptr, size_t len) const noexcept {
            static constexpr uint64_t offset = 0xcbf29ce484222325ull;
            static constexpr uint64_t prime = 1099511628211ull;
            const auto data = static_cast<const uint8_t*>(ptr);
            auto hash = offset;
            for (size_t i = 0; i < len; ++i)
                hash = (hash ^ data[i]) * prime;
            return hash;
        }
    };

    using Fnv1a_std = std::conditional_t<sizeof(size_t) <= 4, Fnv1a_32, Fnv1a_64>;

    class Murmur3_32:
    public HashFunction<32> {
    public:
        Murmur3_32() = default;
        explicit Murmur3_32(uint32_t seed) noexcept: seedval(seed) {}
        virtual uint32_t hash(const void* ptr, size_t len) const noexcept {
            static constexpr uint32_t c1 = 0xcc9e2d51;
            static constexpr uint32_t c2 = 0x1b873593;
            static constexpr uint32_t c3 = 0xe6546b64;
            static constexpr uint32_t c4 = 0x85ebca6b;
            static constexpr uint32_t c5 = 0xc2b2ae35;
            const auto data = static_cast<const uint8_t*>(ptr);
            uint32_t hash = seedval, k = 0;
            const size_t n_words = len / 4;
            for (size_t i = 0, max = 4 * n_words; i < max; i += 4) {
                std::memcpy(&k, data + i, 4);
                k *= c1;
                k = rotl(k, 15);
                k *= c2;
                hash ^= k;
                hash = rotl(hash, 13);
                hash = hash * 5 + c3;
            }
            const size_t n_tail = len & 3;
            if (n_tail) {
                const auto tail = data + len - n_tail;
                k = 0;
                if (n_tail == 3)
                    k = tail[2] << 16;
                if (n_tail >= 2)
                    k ^= tail[1] << 8;
                k ^= tail[0];
                k *= c1;
                k = rotl(k, 15);
                k *= c2;
                hash ^= k;
            }
            hash ^= len;
            hash ^= hash >> 16;
            hash *= c4;
            hash ^= hash >> 13;
            hash *= c5;
            hash ^= hash >> 16;
            return hash;
        }
    private:
        uint32_t seedval = 0;
    };

    class SipHash_64:
    public HashFunction<64> {
    public:
        static constexpr size_t key_bytes = 16;
        static constexpr size_t key_bits = 8 * key_bytes;
        SipHash_64() = default;
        SipHash_64(const void* key, size_t keylen) noexcept: SipHash_64() {
            uint64_t k[2] = {0,0};
            std::memcpy(k, key, std::min(keylen, size_t(key_bytes)));
            initvec[3] ^= k[1];
            initvec[2] ^= k[0];
            initvec[1] ^= k[1];
            initvec[0] ^= k[0];
        }
        virtual uint64_t hash(const void* ptr, size_t len) const noexcept {
            static constexpr int c_rounds = 2;
            static constexpr int d_rounds = 4;
            const auto data = static_cast<const uint8_t*>(ptr);
            const size_t len64 = len - len % 8;
            uint64_t mask = 0;
            auto v = initvec;
            for (size_t i = 0; i < len64; i += 8) {
                std::memcpy(&mask, data + i, 8);
                sip_rounds(v, c_rounds, mask);
            }
            mask = uint64_t(len) << 56;
            for (int x = int(len & 7) - 1; x >= 0; --x)
                mask |= uint64_t(data[len64 + x]) << (8 * x);
            sip_rounds(v, c_rounds, mask);
            v[2] ^= 0xff;
            sip_rounds(v, d_rounds, 0);
            return v[0] ^ v[1] ^ v[2] ^ v[3];
        }
    private:
        using vec_type = std::array<uint64_t, 4>;
        vec_type initvec = {{
            0x736f6d6570736575ull,
            0x646f72616e646f6dull,
            0x6c7967656e657261ull,
            0x7465646279746573ull,
        }};
        static void sip_rounds(vec_type& v, int n, uint64_t mask) noexcept {
            v[3] ^= mask;
            for (int i = 0; i < n; ++i) {
                v[0] += v[1];
                v[1] = rotl(v[1], 13) ^ v[0];
                v[0] = rotl(v[0], 32);
                v[2] += v[3];
                v[3] = rotl(v[3], 16) ^ v[2];
                v[0] += v[3];
                v[3] = rotl(v[3], 21) ^ v[0];
                v[2] += v[1];
                v[1] = rotl(v[1], 17) ^ v[2];
                v[2] = rotl(v[2], 32);
            }
            v[0] ^= mask;
        };
    };

    class SipHash_32:
    public HashFunction<32> {
    public:
        static constexpr size_t key_bytes = 16;
        static constexpr size_t key_bits = 8 * key_bytes;
        SipHash_32() = default;
        SipHash_32(const void* key, size_t keylen) noexcept: sip64(key, keylen) {}
        virtual uint32_t hash(const void* ptr, size_t len) const noexcept {
            uint64_t hash = sip64(ptr, len);
            return uint32_t(hash) ^ uint32_t(hash >> 32);
        }
    private:
        SipHash_64 sip64;
    };

    using SipHash_std = std::conditional_t<sizeof(size_t) <= 4, SipHash_32, SipHash_64>;

    #if defined(__APPLE__)

        #define RS_HASH_FUNCTION_IMPLEMENTATION(unix_name, unix_ctx_name, win_name, bits) \
            CC_##unix_name##_CTX context; \
            CC_##unix_name##_Init(&context); \
            CC_##unix_name##_Update(&context, ptr, uint32_t(len)); \
            CC_##unix_name##_Final(hash.data(), &context);

    #elif defined(_XOPEN_SOURCE)

        #define RS_HASH_FUNCTION_IMPLEMENTATION(unix_name, unix_ctx_name, win_name, bits) \
            unix_ctx_name##_CTX context; \
            unix_name##_Init(&context); \
            unix_name##_Update(&context, ptr, uint32_t(len)); \
            unix_name##_Final(hash.data(), &context);

    #else

        #define RS_HASH_FUNCTION_IMPLEMENTATION(unix_name, unix_ctx_name, win_name, bits) \
            const auto data = static_cast<const uint8_t*>(ptr); \
            DWORD hashlen = bits / 8; \
            HCRYPTHASH hchash = 0; \
            HCRYPTPROV hcprov = 0; \
            CryptAcquireContextW(&hcprov, nullptr, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT | CRYPT_VERIFYCONTEXT); \
            CryptCreateHash(hcprov, CALG_##win_name, 0, 0, &hchash); \
            CryptHashData(hchash, data, uint32_t(len), 0); \
            CryptGetHashParam(hchash, HP_HASHVAL, hash.data(), &hashlen, 0); \
            CryptDestroyHash(hchash); \
            CryptReleaseContext(hcprov, 0);

    #endif

    #define RS_HASH_FUNCTION_CLASS(class_name, unix_name, unix_ctx_name, win_name, bits) \
        class class_name: \
        public HashFunction<bits> { \
        public: \
            virtual std::array<uint8_t, bits / 8> hash(const void* ptr, size_t len) const noexcept { \
                std::array<uint8_t, bits / 8> hash; \
                    RS_HASH_FUNCTION_IMPLEMENTATION(unix_name, unix_ctx_name, win_name, bits); \
                return hash; \
            } \
        };

    RS_HASH_FUNCTION_CLASS(Md5, MD5, MD5, MD5, 128);
    RS_HASH_FUNCTION_CLASS(Sha1, SHA1, SHA, SHA1, 160);
    RS_HASH_FUNCTION_CLASS(Sha256, SHA256, SHA256, SHA_256, 256);
    RS_HASH_FUNCTION_CLASS(Sha512, SHA512, SHA512, SHA_512, 512);

    #undef RS_HASH_FUNCTION_CLASS
    #undef RS_HASH_FUNCTION_IMPLEMENTATION

}
