#include "rs-core/digest.hpp"
#include <algorithm>

#if defined(__APPLE__)
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    #include <CommonCrypto/CommonDigest.h>
#elif defined(_XOPEN_SOURCE)
    #include <openssl/md5.h>
    #include <openssl/sha.h>
#else
    #include <windows.h>
    #include <wincrypt.h>
#endif

namespace RS {

    // Class Crc32

    void Crc32::add(const void* ptr, size_t len) noexcept {
        auto bptr = static_cast<const uint8_t*>(ptr);
        static const auto table = [] {
            std::array<uint32_t, 256> buf;
            for (uint32_t i = 0; i < 256; i++) {
                uint32_t x = i;
                for (int j = 0; j < 8; j++) {
                    if (x & 1)
                        x = 0xedb88320 ^ (x >> 1);
                    else
                        x >>= 1;
                }
                buf[i] = x;
            }
            return buf;
        }();
        for (size_t i = 0; i < len; i++)
            hash = table[(hash ^ bptr[i]) & 0xff] ^ (hash >> 8);
    }

    // Class Murmur3_32

    namespace {

        template <typename T>
        inline void add_bytes(T& t, const uint8_t* ptr, size_t len = sizeof(T), size_t ofs = 0) noexcept {
            size_t maxbits = 8 * std::min(ofs + len, sizeof(T));
            for (size_t i = 8 * ofs; i < maxbits; i += 8)
                t += T(*ptr++) << i;
        }

    }

    void Murmur3_32::add(const void* ptr, size_t len) noexcept {
        auto bptr = static_cast<const uint8_t*>(ptr);
        total += uint32_t(len);
        if (carried + len < 4) {
            add_bytes(word, bptr, len, carried);
            carried += uint32_t(len);
            return;
        }
        if (carried) {
            uint32_t leading = 4 - carried;
            add_bytes(word, bptr, leading, carried);
            word = rotl(word * c1, 15) * c2;
            hash = rotl(hash ^ word, 13) * 5 + c3;
            bptr += leading;
            len -= leading;
        }
        carried = len & 3;
        len -= carried;
        for (size_t ofs = 0; ofs < len; ofs += 4) {
            word = 0;
            add_bytes(word, bptr + ofs);
            word = rotl(word * c1, 15) * c2;
            hash = rotl(hash ^ word, 13) * 5 + c3;
        }
        word = 0;
        add_bytes(word, bptr + len, carried);
    }

    uint32_t Murmur3_32::get() noexcept {
        uint32_t result = hash;
        if (carried)
            result ^= rotl(word * c1, 15) * c2;
        result ^= total;
        result ^= result >> 16;
        result *= c4;
        result ^= result >> 13;
        result *= c5;
        result ^= result >> 16;
        return result;
    }

    // Cryptographic hash functions

    #if defined(__APPLE__)

        #define HASH_CONTEXT(unix_name, unix_ctx_name)  CC_##unix_name##_CTX context;
        #define HASH_INIT(unix_name, win_name)          CC_##unix_name##_Init(&impl->context);
        #define HASH_UPDATE(unix_name)                  CC_##unix_name##_Update(&impl->context, ptr, uint32_t(len));
        #define HASH_FINAL(unix_name)                   CC_##unix_name##_Final(hash.data(), &impl->context);

    #elif defined(_XOPEN_SOURCE)

        #define HASH_CONTEXT(unix_name, unix_ctx_name)  unix_ctx_name##_CTX context;
        #define HASH_INIT(unix_name, win_name)          unix_name##_Init(&impl->context);
        #define HASH_UPDATE(unix_name)                  unix_name##_Update(&impl->context, ptr, uint32_t(len));
        #define HASH_FINAL(unix_name)                   unix_name##_Final(hash.data(), &impl->context);

    #else

        #define HASH_CONTEXT(unix_name, unix_ctx_name) \
            HCRYPTPROV hcprov = 0; \
            HCRYPTHASH hchash = 0; \
            DWORD hashlen = sizeof(result_type);
        #define HASH_INIT(unix_name, win_name) \
            CryptAcquireContextW(&impl->hcprov, nullptr, MS_ENH_RSA_AES_PROV, PROV_RSA_AES, CRYPT_SILENT | CRYPT_VERIFYCONTEXT); \
            CryptCreateHash(impl->hcprov, CALG_##win_name, 0, 0, &impl->hchash);
        #define HASH_UPDATE(unix_name) \
            CryptHashData(impl->hchash, static_cast<const uint8_t*>(ptr), uint32_t(len), 0);
        #define HASH_FINAL(unix_name) \
            CryptGetHashParam(impl->hchash, HP_HASHVAL, hash.data(), &impl->hashlen, 0); \
            CryptDestroyHash(impl->hchash); \
            CryptReleaseContext(impl->hcprov, 0);

    #endif

    #define IMPLEMENT_HASH_CLASS(ClassName, unix_name, unix_ctx_name, win_name) \
        struct ClassName::impl_type { \
            HASH_CONTEXT(unix_name, unix_ctx_name); \
        }; \
        void ClassName::add(const void* ptr, size_t len) { \
            if (! impl) { \
                impl = new impl_type; \
                HASH_INIT(unix_name, win_name); \
            } \
            HASH_UPDATE(unix_name); \
        } \
        ClassName::result_type ClassName::get() { \
            if (impl) { \
                HASH_FINAL(unix_name); \
                delete impl; \
                impl = nullptr; \
            } \
            return hash; \
        }

    IMPLEMENT_HASH_CLASS(Md5, MD5, MD5, MD5);
    IMPLEMENT_HASH_CLASS(Sha1, SHA1, SHA, SHA1);
    IMPLEMENT_HASH_CLASS(Sha256, SHA256, SHA256, SHA_256);
    IMPLEMENT_HASH_CLASS(Sha512, SHA512, SHA512, SHA_512);

}
