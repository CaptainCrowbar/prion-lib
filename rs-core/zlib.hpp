#pragma once

#include "rs-core/common.hpp"
#include "rs-core/io.hpp"
#include "unicorn/path.hpp"
#include <string>
#include <string_view>
#include <system_error>
#include <zlib.h>

RS_LDLIB(msvc: zlib);

namespace RS {

    class ZlibCategory:
    public std::error_category {
    public:
        virtual const char* name() const noexcept;
        virtual Ustring message(int value) const;
    };

    const std::error_category& zlib_category() noexcept;

    struct Zlib {
        static std::string compress(std::string_view src, int level = Z_DEFAULT_COMPRESSION);
        static std::string uncompress(std::string_view src);
    };

    class Gzio:
    public IO {
    public:
        RS_MOVE_ONLY(Gzio);
        using handle_type = gzFile;
        Gzio() = default;
        explicit Gzio(const Unicorn::Path& f, mode m = mode::read_only);
        Gzio(const Unicorn::Path& f, Uview iomode);
        virtual void close() noexcept override;
        virtual void flush() noexcept override;
        virtual int getc() noexcept override;
        virtual bool is_open() const noexcept override { return bool(gz); }
        virtual bool is_terminal() const noexcept override { return false; }
        virtual void putc(char c) override;
        virtual size_t read(void* ptr, size_t maxlen) noexcept override;
        virtual std::string read_line() override;
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
        virtual ptrdiff_t tell() noexcept override;
        virtual size_t write(const void* ptr, size_t len) override;
        gzFile get() const noexcept { return gz.get(); }
        void ungetc(char c);
    private:
        Resource<gzFile> gz;
        void check_status(bool ok) noexcept;
    };

}
