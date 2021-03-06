#include "rs-core/zlib.hpp"
#include <algorithm>
#include <cerrno>

namespace RS {

    // Class ZlibCategory

    const char* ZlibCategory::name() const noexcept {
        return "Zlib";
    }

    #define ZLIB_MESSAGE(Z) if (value == Z) msg += " " #Z

    Ustring ZlibCategory::message(int value) const {
        Ustring msg = "Zlib error";
        ZLIB_MESSAGE(Z_OK);
        ZLIB_MESSAGE(Z_STREAM_END);
        ZLIB_MESSAGE(Z_NEED_DICT);
        ZLIB_MESSAGE(Z_ERRNO);
        ZLIB_MESSAGE(Z_STREAM_ERROR);
        ZLIB_MESSAGE(Z_DATA_ERROR);
        ZLIB_MESSAGE(Z_MEM_ERROR);
        ZLIB_MESSAGE(Z_BUF_ERROR);
        ZLIB_MESSAGE(Z_VERSION_ERROR);
        return msg;
    }

    const std::error_category& zlib_category() noexcept {
        static const ZlibCategory cat;
        return cat;
    }

    // Struct Zlib

    std::string Zlib::compress(std::string_view src, int level) {
        if (src.empty())
            return {};
        auto dstlen = ::compressBound(uLong(src.size()));
        std::string dst(size_t(dstlen), '\0');
        errno = 0;
        int rc = ::compress2(reinterpret_cast<unsigned char*>(&dst[0]), &dstlen, reinterpret_cast<const unsigned char*>(src.data()), uLong(src.size()), level);
        int err = errno;
        if (rc == Z_ERRNO)
            throw std::system_error(err, std::generic_category());
        else if (rc != 0)
            throw std::system_error(rc, zlib_category());
        dst.resize(size_t(dstlen));
        return dst;
    }

    std::string Zlib::uncompress(std::string_view src) {
        if (src.empty())
            return {};
        auto block = uLong(5 * src.size());
        auto dstlen = block;
        std::string dst(size_t(block), '\0');
        for (;;) {
            errno = 0;
            int rc = ::uncompress(reinterpret_cast<unsigned char*>(&dst[0]), &dstlen, reinterpret_cast<const unsigned char*>(src.data()), uLong(src.size()));
            int err = errno;
            if (rc == 0)
                break;
            else if (rc == Z_ERRNO)
                throw std::system_error(err, std::generic_category());
            else if (rc != Z_BUF_ERROR)
                throw std::system_error(rc, zlib_category());
            dstlen += block;
            dst.resize(size_t(dstlen), '\0');
        }
        dst.resize(size_t(dstlen));
        return dst;
    }

    // Class Gzio

    Gzio::Gzio(const Unicorn::Path& f, mode m) {
        Ustring gzmode;
        switch (m) {
            case IO::mode::read_only:   gzmode = "rb"; break;
            case IO::mode::write_only:  gzmode = "wb"; break;
            case IO::mode::append:      gzmode = "ab"; break;
            default:                    break;
        }
        *this = Gzio(f, gzmode);
    }

    Gzio::Gzio(const Unicorn::Path& f, Uview iomode) {
        std::string u_name;
        #ifdef _XOPEN_SOURCE
            u_name = f.os_name();
        #else
            u_name = f.name();
        #endif
        errno = 0;
        auto rc = gzopen(u_name.data(), iomode.data());
        int err = errno;
        if (rc) {
            gzbuffer(rc, 65536);
            gz = {rc, [] (gzFile g) { if (g) gzclose(g); }};
        } else if (err) {
            set_error(err);
        } else {
            set_error(EIO);
        }
    }

    void Gzio::close() noexcept {
        if (gz)
            set_error(gzclose(gz.release()), zlib_category());
    }

    void Gzio::flush() noexcept {
        check_status(gzflush(get(), Z_FULL_FLUSH) == 0);
    }

    int Gzio::getc() noexcept {
        errno = 0;
        int rc = gzgetc(get());
        if (rc == -1 && ! gzeof(get()))
            check_status(false);
        return rc;
    }

    void Gzio::putc(char c) {
        errno = 0;
        check_status(gzputc(get(), int(uint8_t(c))) != -1);
    }

    size_t Gzio::read(void* ptr, size_t maxlen) noexcept {
        errno = 0;
        int n = gzread(get(), ptr, unsigned(maxlen));
        check_status(n < 0);
        return size_t(std::max(n, 0));
    }

    std::string Gzio::read_line() {
        static constexpr size_t block = 256;
        std::string buf;
        for (;;) {
            size_t offset = buf.size();
            buf.resize(offset + block, '\0');
            errno = 0;
            auto rc = gzgets(get(), &buf[0] + offset, block);
            if (rc == nullptr) {
                check_status(false);
                return buf.substr(0, offset);
            }
            size_t lfpos = buf.find('\n', offset);
            if (lfpos != npos)
                return buf.substr(0, lfpos + 1);
            size_t ntpos = buf.find_last_not_of('\0') + 1;
            if (ntpos < block - 1)
                return buf.substr(0, ntpos);
            buf.pop_back();
        }
    }

    void Gzio::seek(ptrdiff_t offset, int which) noexcept {
        errno = 0;
        check_status(gzseek(get(), z_off_t(offset), which) != -1);
    }

    ptrdiff_t Gzio::tell() noexcept {
        errno = 0;
        ptrdiff_t offset = gztell(get());
        check_status(offset != -1);
        return offset;
    }

    size_t Gzio::write(const void* ptr, size_t len) {
        errno = 0;
        size_t n = gzwrite(get(), ptr, unsigned(len));
        check_status(n > 0);
        return n;
    }

    void Gzio::ungetc(char c) {
        errno = 0;
        int rc = gzungetc(int(uint8_t(c)), get());
        check_status(rc != -1);
    }

    void Gzio::check_status(bool ok) noexcept {
        int err = errno;
        if (ok) {
            set_error(0);
        } else {
            int zerr = 0;
            gzerror(get(), &zerr);
            if (zerr == Z_ERRNO)
                set_error(err);
            else
                set_error(zerr, zlib_category());
        }
    }

}
