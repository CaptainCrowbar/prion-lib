#pragma once

#include "rs-core/common.hpp"
#include <array>
#include <cstring>
#include <type_traits>

#ifdef __APPLE__
    #include <CommonCrypto/CommonDigest.h>
    #define RS_CRYPTO_API(name) CC_##name
#elif defined(_XOPEN_SOURCE)
    #include <openssl/md5.h>
    #include <openssl/sha.h>
    #define RS_CRYPTO_API(name) name
    using SHA1_CTX = SHA_CTX;
#else
    #include <windows.h>
    #include <wincrypt.h>
#endif

RS_LDLIB(cygwin: crypto);
RS_LDLIB(linux: crypto);
RS_LDLIB(msvc: advapi32);

namespace RS {

    // Common hash functions

    class Adler32 {
    public:
        using result_type = uint32_t;
        static constexpr size_t result_size = 4;
        Adler32& operator()(const void* ptr, size_t n) noexcept {
            auto bytes = static_cast<const uint8_t*>(ptr);
            for (size_t i = 0; i < n; ++i) {
                s1 = (s1 + bytes[i]) % 65521;
                s2 = (s2 + s1) % 65521;
            }
            return *this;
        }
        operator uint32_t() noexcept { return (s2 << 16) + s1; }
    private:
        uint32_t s1 = 1, s2 = 0;
    };

    class Crc32 {
    public:
        using result_type = uint32_t;
        static constexpr size_t result_size = 4;
        Crc32& operator()(const void* ptr, size_t n) noexcept {
            static const auto table = [] {
                std::array<uint32_t, 256> data;
                for (uint32_t i = 0; i < 256; i++) {
                    uint32_t x = i;
                    for (int k = 0; k < 8; k++) {
                        if (x & 1)
                            x = 0xedb88320 ^ (x >> 1);
                        else
                            x >>= 1;
                    }
                    data[i] = x;
                }
                return data;
            }();
            auto bytes = static_cast<const uint8_t*>(ptr);
            for (size_t i = 0; i < n; i++)
                c = table[(c ^ bytes[i]) & 0xff] ^ (c >> 8);
            return *this;
        }
        operator uint32_t() noexcept { return c ^ 0xffffffff; }
    private:
        uint32_t c = 0xffffffff;
    };

    // SipHash implementation based on the public domain reference implementation by Jean-Philippe Aumasson and Daniel J. Bernstein
    // See https://131002.net/siphash and https://github.com/veorq/SipHash
    // This implementation assumes 8-bit bytes and little-endian integers

    class SipHash24 {
    public:
        using result_type = uint64_t;
        static constexpr size_t key_size = 16;
        static constexpr size_t result_size = 8;
        SipHash24() noexcept { static constexpr uint64_t z[2] = {0,0}; init(z); }
        explicit SipHash24(const void* key) noexcept { init(key); }
        SipHash24& operator()(const void* ptr, size_t n) noexcept {
            size_t over = len % 8;
            if (over + n < 8) {
                memcpy(buf + over, ptr, n);
                len += n;
                return *this;
            }
            auto bp = static_cast<const uint8_t*>(ptr);
            uint64_t x = 0;
            if (over) {
                size_t over2 = 8 - over;
                auto xp = reinterpret_cast<uint8_t*>(&x);
                memcpy(xp, buf, over);
                memcpy(xp + over, bp, over2);
                sip_rounds(v, x, 2);
                bp += over2;
                n -= over2;
                len += over2;
            }
            len += n;
            over = len % 8;
            for (auto end = bp + n - over; bp != end; bp += 8) {
                memcpy(&x, bp, 8);
                sip_rounds(v, x, 2);
            }
            memcpy(buf, bp, over);
            return *this;
        }
        operator result_type() noexcept {
            uint64_t x = uint64_t(len) << 56;
            memcpy(&x, buf, len % 8);
            sip_rounds(v, x, 2);
            v[2] ^= 0xff;
            sip_rounds(v, 0, 4);
            return v[0] ^ v[1] ^ v[2] ^ v[3];
        }
    private:
        uint64_t v[4];
        size_t len = 0;
        uint8_t buf[7];
        void init(const void* key) noexcept {
            memcpy(v, key, 16);
            v[3] = v[1] ^ 0x7465646279746573ull;
            v[2] = v[0] ^ 0x6c7967656e657261ull;
            v[1] ^= 0x646f72616e646f6dull;
            v[0] ^= 0x736f6d6570736575ull;
        }
        static void sip_rounds(uint64_t* v, uint64_t x, int n) noexcept {
            v[3] ^= x;
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
            v[0] ^= x;
        }
    };

    // Cryptographic hash functions

    // Zhihao Yuan, "Message Digest Library for C++"
    // http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2015/n4449.html

    // Algorithm  GNU   FreeBSD  Apple         Windows    OpenSSL
    // ...        libc  libmd    CommonCrypto  CryptoAPI  ...
    // MD5        yes   yes      yes           yes        yes
    // SHA1       yes   yes      yes           yes        yes
    // SHA224     --    --       yes           --         yes
    // SHA256     yes   yes      yes           yes        yes
    // SHA384     --    --       yes           yes        yes
    // SHA512     yes   yes      yes           yes        yes
    // RMD160     --    yes      --            --         --

    #ifdef _XOPEN_SOURCE

        #define RS_DIGEST_CLASS(DigestClass, bytes, x_tag, w_pname, w_ptype, w_algo) \
            class DigestClass { \
            public: \
                RS_NO_COPY_MOVE(DigestClass) \
                using result_type = std::array<uint8_t, bytes>; \
                static constexpr size_t result_size = bytes; \
                DigestClass() noexcept { RS_CRYPTO_API(x_tag##_Init)(&c); } \
                ~DigestClass() noexcept {} \
                DigestClass& operator()(const void* ptr, size_t n) noexcept { RS_CRYPTO_API(x_tag##_Update)(&c, ptr, uint32_t(n)); return *this; } \
                operator result_type() noexcept { result_type r; RS_CRYPTO_API(x_tag##_Final)(r.data(), &c); return r; } \
            private: \
                RS_CRYPTO_API(x_tag##_CTX) c; \
            };

    #else

        #define RS_DIGEST_CLASS(DigestClass, bytes, x_tag, w_pname, w_ptype, w_algo) \
            class DigestClass { \
            public: \
                RS_NO_COPY_MOVE(DigestClass) \
                using result_type = std::array<uint8_t, bytes>; \
                static constexpr size_t result_size = bytes; \
                DigestClass() noexcept { CryptAcquireContextW(&p, nullptr, w_pname, w_ptype, CRYPT_SILENT | CRYPT_VERIFYCONTEXT); CryptCreateHash(p, w_algo, 0, 0, &h); } \
                ~DigestClass() noexcept { CryptDestroyHash(h); CryptReleaseContext(p, 0); } \
                DigestClass& operator()(const void* ptr, size_t n) noexcept { CryptHashData(h, LPCBYTE(ptr), uint32_t(n), 0); return *this; } \
                operator result_type() noexcept { result_type r; DWORD n = result_size; CryptGetHashParam(h, HP_HASHVAL, r.data(), &n, 0); return r; } \
            private: \
                HCRYPTHASH h = 0; \
                HCRYPTPROV p = 0; \
            };

    #endif

    RS_DIGEST_CLASS(Md5, 16, MD5, MS_DEF_PROV, PROV_RSA_FULL, CALG_MD5);
    RS_DIGEST_CLASS(Sha1, 20, SHA1, MS_DEF_PROV, PROV_RSA_FULL, CALG_SHA1);
    RS_DIGEST_CLASS(Sha256, 32, SHA256, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CALG_SHA_256);
    RS_DIGEST_CLASS(Sha512, 64, SHA512, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CALG_SHA_512);

    // Utility functions

    template <typename MD>
    typename MD::result_type digest(const void* ptr, size_t n) {
        MD md;
        md(ptr, n);
        return md;
    }

    template <typename MD>
    typename MD::result_type digest(string_view s) {
        return digest<MD>(s.data(), s.size());
    }

}
