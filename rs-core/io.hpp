#pragma once

#include "rs-core/common.hpp"
#include "rs-core/file.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>
#include <vector>

#ifdef _XOPEN_SOURCE
    #define RS_IO_FUNCTION(f) ::f
#else
    #define RS_IO_FUNCTION(f) _##f
#endif

namespace RS {

    // I/O abstract base class

    class IO {
    public:
        class line_iterator: public InputIterator<line_iterator, const std::string> {
        public:
            line_iterator() = default;
            explicit line_iterator(IO& io): iop(&io), line() { ++*this; }
            const std::string& operator*() const noexcept { return line; }
            line_iterator& operator++();
            bool operator==(const line_iterator& rhs) const noexcept { return iop == rhs.iop; }
        private:
            IO* iop = nullptr;
            std::string line;
        };
        enum class mode {
            read_only = 1,
            write_only,
            append,
            create_always,
            open_always,
            open_existing,
        };
        virtual ~IO() = default;
        virtual void close() noexcept = 0;
        virtual void flush() noexcept {}
        virtual int getc() noexcept;
        virtual bool is_open() const noexcept = 0;
        virtual bool is_terminal() const noexcept = 0;
        virtual void putc(char c) { write(&c, 1); }
        virtual size_t read(void* ptr, size_t maxlen) noexcept = 0;
        virtual std::string read_line();
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept = 0;
        virtual ptrdiff_t tell() noexcept = 0;
        virtual size_t write(const void* ptr, size_t len) = 0;
        virtual void write_n(size_t n, char c);
        void check(Uview detail = "") const;
        void clear_error() noexcept { set_error(0); }
        std::error_code error() const noexcept { return status; }
        template <typename... Args> void format(Uview pattern, const Args&... args) { write_str(fmt(pattern, args...)); }
        Irange<line_iterator> lines() { return {line_iterator(*this), {}}; }
        bool ok() const noexcept { return ! status && is_open(); }
        template <typename... Args> void print(const Args&... args);
        std::string read_all();
        size_t read_n(std::string& s, size_t maxlen = 1024);
        std::string read_str(size_t maxlen);
        void write_line() { putc('\n'); }
        void write_line(std::string_view str);
        size_t write_str(std::string_view str);
    protected:
        static constexpr const char* null_device =
            #ifdef _XOPEN_SOURCE
                "/dev/null";
            #else
                "NUL:";
            #endif
        IO() = default;
        void set_error(int err, const std::error_category& cat = std::generic_category()) noexcept;
    private:
        std::error_code status;
    };

        inline int IO::getc() noexcept {
            unsigned char b = 0;
            return read(&b, 1) ? int(b) : EOF;
        }

        inline std::string IO::read_line() {
            std::string s;
            for (;;) {
                int c = getc();
                if (c == '\n' || c == EOF)
                    break;
                s += char(c);
            }
            return s;
        }

        inline void IO::write_n(size_t n, char c) {
            static constexpr size_t block = 16384;
            std::string buf(std::min(n, block), c);
            auto qr = divide(n, block);
            do_n(qr.first, [&] { write(buf.data(), block); });
            if (qr.second)
                write(buf.data(), qr.second);
        }

        inline void IO::check(Uview detail) const {
            if (status) {
                if (detail.empty())
                    throw std::system_error(status);
                else
                    throw std::system_error(status, Ustring(detail));
            }
        }

        template <typename... Args>
        void IO::print(const Args&... args) {
            std::vector<Ustring> vec{to_str(args)...};
            write_line(join(vec, " "));
        }

        inline std::string IO::read_all() {
            static constexpr size_t block = 16384;
            std::string s;
            while (read_n(s, block)) {}
            return s;
        }

        inline size_t IO::read_n(std::string& s, size_t maxlen) {
            size_t offset = s.size();
            s.resize(offset + maxlen);
            size_t n = read(&s[0] + offset, maxlen);
            s.resize(offset + n);
            return n;
        }

        inline std::string IO::read_str(size_t maxlen) {
            std::string s(maxlen, 0);
            s.resize(read(&s[0], maxlen));
            return s;
        }

        inline void IO::write_line(std::string_view str) {
            if (str.empty())
                putc('\n');
            else if (str.back() == '\n')
                write_str(str);
            else
                write_str(std::string(str) + '\n');
        }

        inline size_t IO::write_str(std::string_view str) {
            static constexpr char dummy = '\0';
            const char* ptr = str.empty() ? &dummy : str.data();
            size_t ofs = 0, len = str.size();
            do ofs += write(ptr + ofs, len - ofs);
                while (ofs < len && ok());
            return ofs;
        }

        inline void IO::set_error(int err, const std::error_category& cat) noexcept {
            if (err)
                status = {err, cat};
            else
                status.clear();
        }

        inline IO::line_iterator& IO::line_iterator::operator++() {
            if (iop) {
                line = iop->read_line();
                if (line.empty())
                    iop = nullptr;
            }
            return *this;
        }

    // C standard I/O

    class Cstdio:
    public IO {
    public:
        RS_MOVE_ONLY(Cstdio);
        using handle_type = FILE*;
        Cstdio() = default;
        explicit Cstdio(FILE* f, bool owner = true) noexcept;
        explicit Cstdio(const File& f, mode m = mode::read_only);
        Cstdio(const File& f, const Ustring& iomode);
        virtual void close() noexcept override;
        virtual void flush() noexcept override;
        virtual int getc() noexcept override;
        virtual bool is_open() const noexcept override { return bool(fp); }
        virtual bool is_terminal() const noexcept override { return RS_IO_FUNCTION(isatty)(fd()); }
        virtual void putc(char c) override;
        virtual size_t read(void* ptr, size_t maxlen) noexcept override;
        virtual std::string read_line() override;
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
        virtual ptrdiff_t tell() noexcept override;
        virtual size_t write(const void* ptr, size_t len) override;
        int fd() const noexcept { return RS_IO_FUNCTION(fileno)(get()); }
        FILE* get() const noexcept { return fp.get(); }
        FILE* release() noexcept { return fp.release(); }
        void ungetc(char c);
        static Cstdio dev_null() noexcept;
        static Cstdio std_input() noexcept { return Cstdio(stdin, false); }
        static Cstdio std_output() noexcept { return Cstdio(stdout, false); }
        static Cstdio std_error() noexcept { return Cstdio(stderr, false); }
    private:
        Resource<FILE*> fp;
    };

        inline Cstdio::Cstdio(FILE* f, bool owner) noexcept {
            if (owner)
                fp = {f, [] (FILE* p) { if (p) ::fclose(p); }};
            else
                fp = {f, [] (FILE*) {}};
        }

        inline Cstdio::Cstdio(const File& f, mode m) {
            if (f.empty() || f.name() == "-") {
                if (m == mode::read_only) {
                    *this = std_input();
                    return;
                } else if (m == mode::write_only || m == mode::append) {
                    *this = std_output();
                    return;
                }
            } else if (m == mode::open_always) {
                // There is no C stdio mode corresponding directly to the
                // behaviour we want from IO::mode::open_always. Try "rb+"
                // first (open for R/W if the file exists, otherwise fail),
                // and if that fails, try "wb+" (open for R/W regardless,
                // destroying any existing file). There is a race condition
                // here but I don't think there is any way around it.
                Ustring iomode = "rb+";
                *this = Cstdio(f, "rb+");
                if (! fp)
                    *this = Cstdio(f, "wb+");
            } else {
                Ustring cmode;
                switch (m) {
                    case mode::read_only:      cmode = "rb"; break;
                    case mode::write_only:     cmode = "wb"; break;
                    case mode::append:         cmode = "ab"; break;
                    case mode::create_always:  cmode = "wb+"; break;
                    case mode::open_existing:  cmode = "rb+"; break;
                    default:                   break;
                }
                *this = Cstdio(f, cmode);
            }
        }

        inline Cstdio::Cstdio(const File& f, const Ustring& iomode) {
            #ifdef _XOPEN_SOURCE
                errno = 0;
                auto rc = ::fopen(f.c_name(), iomode.data());
            #else
                auto wfile = f.native();
                auto wmode = uconv<std::wstring>(iomode);
                errno = 0;
                auto rc = _wfopen(wfile.data(), wmode.data());
            #endif
            int err = errno;
            *this = Cstdio(rc);
            set_error(err);
        }

        inline void Cstdio::close() noexcept {
            if (fp) {
                FILE* f = fp.release();
                errno = 0;
                ::fclose(f);
                set_error(errno);
            }
        }

        inline void Cstdio::flush() noexcept {
            errno = 0;
            ::fflush(get());
            set_error(errno);
        }

        inline int Cstdio::getc() noexcept {
            errno = 0;
            int c = ::fgetc(get());
            set_error(errno);
            return c;
        }

        inline void Cstdio::putc(char c) {
            errno = 0;
            ::fputc(int(uint8_t(c)), get());
            set_error(errno);
        }

        inline size_t Cstdio::read(void* ptr, size_t maxlen) noexcept {
            errno = 0;
            size_t n = ::fread(ptr, 1, maxlen, get());
            set_error(errno);
            return n;
        }

        inline std::string Cstdio::read_line() {
            static constexpr size_t block = 256;
            std::string buf;
            for (;;) {
                size_t offset = buf.size();
                buf.resize(offset + block, '\0');
                errno = 0;
                auto rc = ::fgets(&buf[0] + offset, block, get());
                set_error(errno);
                if (rc == nullptr)
                    return buf.substr(0, offset);
                size_t lfpos = buf.find('\n', offset);
                if (lfpos != npos)
                    return buf.substr(0, lfpos);
                size_t ntpos = buf.find_last_not_of('\0') + 1;
                if (ntpos < block - 1)
                    return buf.substr(0, ntpos);
                buf.pop_back();
            }
        }

        inline void Cstdio::seek(ptrdiff_t offset, int which) noexcept {
            errno = 0;
            #ifdef _XOPEN_SOURCE
                ::fseeko(get(), offset, which);
            #else
                _fseeki64(get(), offset, which);
            #endif
            set_error(errno);
        }

        inline ptrdiff_t Cstdio::tell() noexcept {
            errno = 0;
            #ifdef _XOPEN_SOURCE
                ptrdiff_t offset = ::ftello(get());
            #else
                ptrdiff_t offset = _ftelli64(get());
            #endif
            set_error(errno);
            return offset;
        }

        inline size_t Cstdio::write(const void* ptr, size_t len) {
            errno = 0;
            size_t n = ::fwrite(ptr, 1, len, get());
            set_error(errno);
            return n;
        }

        inline void Cstdio::ungetc(char c) {
            errno = 0;
            ::ungetc(int(uint8_t(c)), get());
            set_error(errno);
        }

        inline Cstdio Cstdio::dev_null() noexcept {
            Cstdio io(null_device, "r+");
            ::fgetc(io.get()); // Clear bogus ioctl error
            return io;
        }

    // File descriptor I/O

    class Fdio:
    public IO {
    public:
        RS_MOVE_ONLY(Fdio);
        using handle_type = int;
        Fdio() = default;
        explicit Fdio(int f, bool owner = true) noexcept;
        explicit Fdio(const File& f, mode m = mode::read_only);
        Fdio(const File& f, int iomode, int perm = 0666);
        virtual void close() noexcept override;
        virtual void flush() noexcept override;
        virtual bool is_open() const noexcept override { return fd.get() != -1; }
        virtual bool is_terminal() const noexcept override { return RS_IO_FUNCTION(isatty)(get()); }
        virtual size_t read(void* ptr, size_t maxlen) noexcept override;
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
        virtual ptrdiff_t tell() noexcept override;
        virtual size_t write(const void* ptr, size_t len) override;
        Fdio dup() noexcept;
        Fdio dup(int f) noexcept;
        int get() const noexcept { return fd.get(); }
        int release() noexcept { return fd.release(); }
        static Fdio dev_null() noexcept;
        static std::pair<Fdio, Fdio> pipe(size_t winmem = 1024);
        static Fdio std_input() noexcept { return Fdio(0, false); }
        static Fdio std_output() noexcept { return Fdio(1, false); }
        static Fdio std_error() noexcept { return Fdio(2, false); }
    private:
        Resource<int, -1> fd;
        #ifdef _MSC_VER
            using iosize = unsigned;
            using ofsize = long;
        #else
            using iosize = size_t;
            using ofsize = ptrdiff_t;
        #endif
    };

        inline Fdio::Fdio(int f, bool owner) noexcept {
            if (owner)
                fd = {f, [] (int d) { if (d != -1) RS_IO_FUNCTION(close)(d); }};
            else
                fd = {f, [] (int) {}};
        }

        inline Fdio::Fdio(const File& f, mode m) {
            if (f.empty() || f.name() == "-") {
                if (m == mode::read_only) {
                    *this = std_input();
                    return;
                } else if (m == mode::write_only || m == mode::append) {
                    *this = std_output();
                    return;
                }
            }
            int fmode = 0;
            switch (m) {
                case mode::read_only:      fmode = O_RDONLY; break;
                case mode::write_only:     fmode = O_WRONLY|O_CREAT|O_TRUNC; break;
                case mode::append:         fmode = O_WRONLY|O_APPEND|O_CREAT; break;
                case mode::create_always:  fmode = O_RDWR|O_CREAT|O_TRUNC; break;
                case mode::open_always:    fmode = O_RDWR|O_CREAT; break;
                case mode::open_existing:  fmode = O_RDWR; break;
                default:                   break;
            }
            *this = Fdio(f, fmode, 0666);
        }

        inline Fdio::Fdio(const File& f, int iomode, int perm) {
            #ifdef _XOPEN_SOURCE
                #ifdef __CYGWIN__
                    if (! (iomode & (O_BINARY | O_TEXT)))
                        iomode |= O_BINARY;
                #endif
                errno = 0;
                auto rc = ::open(f.c_name(), iomode, perm);
            #else
                if (! (iomode & (_O_BINARY | _O_TEXT | _O_U8TEXT | _O_U16TEXT | _O_WTEXT)))
                    iomode |= _O_BINARY;
                auto wfile = f.native();
                errno = 0;
                auto rc = _wopen(wfile.data(), iomode, perm);
            #endif
            int err = errno;
            *this = Fdio(rc);
            set_error(err);
        }

        inline void Fdio::close() noexcept {
            int f = fd.release();
            if (f != -1) {
                errno = 0;
                RS_IO_FUNCTION(close)(f);
                set_error(errno);
            }
        }

        inline void Fdio::flush() noexcept {
            #ifdef _XOPEN_SOURCE
                errno = 0;
                ::fsync(get());
                set_error(errno);
            #else
                auto h = reinterpret_cast<HANDLE>(_get_osfhandle(get()));
                if (h == INVALID_HANDLE_VALUE)
                    set_error(EBADF);
                else if (! FlushFileBuffers(h))
                    set_error(GetLastError(), std::system_category());
            #endif
        }

        inline size_t Fdio::read(void* ptr, size_t maxlen) noexcept {
            errno = 0;
            auto rc = RS_IO_FUNCTION(read)(get(), ptr, iosize(maxlen));
            set_error(errno);
            return rc;
        }

        inline void Fdio::seek(ptrdiff_t offset, int which) noexcept {
            errno = 0;
            RS_IO_FUNCTION(lseek)(get(), ofsize(offset), which);
            set_error(errno);
        }

        inline ptrdiff_t Fdio::tell() noexcept {
            errno = 0;
            auto offset = RS_IO_FUNCTION(lseek)(get(), 0, SEEK_CUR);
            set_error(errno);
            return offset;
        }

        inline size_t Fdio::write(const void* ptr, size_t len) {
            errno = 0;
            size_t n = RS_IO_FUNCTION(write)(get(), ptr, iosize(len));
            set_error(errno);
            return n;
        }

        inline Fdio Fdio::dup() noexcept {
            errno = 0;
            int rc = RS_IO_FUNCTION(dup)(get());
            set_error(errno);
            return Fdio(rc);
        }

        inline Fdio Fdio::dup(int f) noexcept {
            errno = 0;
            RS_IO_FUNCTION(dup2)(get(), f);
            set_error(errno);
            return Fdio(f);
        }

        inline Fdio Fdio::dev_null() noexcept {
            int iomode = O_RDWR;
            #ifdef O_CLOEXEC
                iomode |= O_CLOEXEC;
            #endif
            return Fdio(null_device, iomode);
        }

        inline std::pair<Fdio, Fdio> Fdio::pipe(size_t winmem) {
            int fds[2];
            errno = 0;
            #ifdef _XOPEN_SOURCE
                (void)winmem;
                ::pipe(fds);
            #else
                _pipe(fds, iosize(winmem), O_BINARY);
            #endif
            int err = errno;
            std::pair<Fdio, Fdio> pair;
            pair.first = Fdio(fds[0], false);
            pair.second = Fdio(fds[1], false);
            pair.first.set_error(err);
            pair.second.set_error(err);
            return pair;
        }

    #ifdef _WIN32

        // Windows file I/O

        class Winio:
        public IO {
        public:
            RS_MOVE_ONLY(Winio);
            using handle_type = void*;
            Winio() = default;
            explicit Winio(void* f, bool owner = true) noexcept;
            explicit Winio(const File& f, mode m = mode::read_only);
            Winio(const File& f, uint32_t desired_access, uint32_t share_mode, LPSECURITY_ATTRIBUTES security_attributes,
                uint32_t creation_disposition, uint32_t flags_and_attributes = 0, HANDLE template_file = nullptr);
            virtual void close() noexcept override;
            virtual void flush() noexcept override;
            virtual bool is_open() const noexcept override { return bool(fh); }
            virtual bool is_terminal() const noexcept override;
            virtual size_t read(void* ptr, size_t maxlen) noexcept override;
            virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
            virtual ptrdiff_t tell() noexcept override;
            virtual size_t write(const void* ptr, size_t len) override;
            void* get() const noexcept { return fh.get(); }
            void* release() noexcept { return fh.release(); }
            static Winio dev_null() noexcept;
            static Winio std_input() noexcept { return Winio(GetStdHandle(STD_INPUT_HANDLE), false); }
            static Winio std_output() noexcept { return Winio(GetStdHandle(STD_OUTPUT_HANDLE), false); }
            static Winio std_error() noexcept { return Winio(GetStdHandle(STD_ERROR_HANDLE), false); }
        private:
            Resource<void*> fh;
        };

            inline Winio::Winio(void* f, bool owner) noexcept {
                if (owner)
                    fh = {f, [] (void* f) { if (f && f != INVALID_HANDLE_VALUE) CloseHandle(f); }};
                else
                    fh = {f, [] (void*) {}};
            }

            inline Winio::Winio(const File& f, mode m) {
                if (f.empty() || f.name() == "-") {
                    if (m == mode::read_only) {
                        *this = std_input();
                        return;
                    } else if (m == mode::write_only || m == mode::append) {
                        *this = std_output();
                        return;
                    }
                }
                uint32_t access = 0, creation = 0;
                switch (m) {
                    case mode::read_only:      access = GENERIC_READ; creation = OPEN_EXISTING; break;
                    case mode::write_only:     access = GENERIC_WRITE; creation = CREATE_ALWAYS; break;
                    case mode::append:         access = GENERIC_WRITE; creation = OPEN_ALWAYS; break;
                    case mode::create_always:  access = GENERIC_READ | GENERIC_WRITE; creation = CREATE_ALWAYS; break;
                    case mode::open_always:    access = GENERIC_READ | GENERIC_WRITE; creation = OPEN_ALWAYS; break;
                    case mode::open_existing:  access = GENERIC_READ | GENERIC_WRITE; creation = OPEN_EXISTING; break;
                    default:                   break;
                }
                *this = Winio(f, access, 0, nullptr, creation);
                if (m == mode::append) {
                    LARGE_INTEGER distance;
                    distance.QuadPart = 0;
                    SetLastError(0);
                    SetFilePointerEx(get(), distance, nullptr, FILE_END);
                    set_error(GetLastError(), std::system_category());
                }
            }

            inline Winio::Winio(const File& f, uint32_t desired_access, uint32_t share_mode, LPSECURITY_ATTRIBUTES security_attributes,
                    uint32_t creation_disposition, uint32_t flags_and_attributes, HANDLE template_file) {
                auto wfile = f.native();
                SetLastError(0);
                auto rc = CreateFileW(wfile.data(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file);
                int err = GetLastError();
                *this = Winio(rc);
                set_error(err, std::system_category());
            }

            inline void Winio::close() noexcept {
                void* h = fh.release();
                if (h && h != INVALID_HANDLE_VALUE) {
                    SetLastError(0);
                    CloseHandle(h);
                    set_error(GetLastError(), std::system_category());
                }
            }

            inline void Winio::flush() noexcept {
                if (! FlushFileBuffers(get()))
                    set_error(GetLastError(), std::system_category());
            }

            inline bool Winio::is_terminal() const noexcept {
                auto h = get();
                if (h == GetStdHandle(STD_INPUT_HANDLE))
                    return RS_IO_FUNCTION(isatty)(0);
                else if (h == GetStdHandle(STD_OUTPUT_HANDLE))
                    return RS_IO_FUNCTION(isatty)(1);
                else if (h == GetStdHandle(STD_ERROR_HANDLE))
                    return RS_IO_FUNCTION(isatty)(2);
                else
                    return false;
            }

            inline size_t Winio::read(void* ptr, size_t maxlen) noexcept {
                DWORD n = 0;
                SetLastError(0);
                ReadFile(get(), ptr, uint32_t(maxlen), &n, nullptr);
                set_error(GetLastError(), std::system_category());
                return n;
            }

            inline void Winio::seek(ptrdiff_t offset, int which) noexcept {
                LARGE_INTEGER distance;
                distance.QuadPart = offset;
                DWORD method = 0;
                switch (which) {
                    case SEEK_SET:  method = FILE_BEGIN; break;
                    case SEEK_CUR:  method = FILE_CURRENT; break;
                    case SEEK_END:  method = FILE_END; break;
                    default:        break;
                }
                SetLastError(0);
                SetFilePointerEx(get(), distance, nullptr, method);
                set_error(GetLastError(), std::system_category());
            }

            inline ptrdiff_t Winio::tell() noexcept {
                LARGE_INTEGER distance, result;
                distance.QuadPart = result.QuadPart = 0;
                SetLastError(0);
                SetFilePointerEx(get(), distance, &result, FILE_CURRENT);
                set_error(GetLastError(), std::system_category());
                return result.QuadPart;
            }

            inline size_t Winio::write(const void* ptr, size_t len) {
                DWORD n = 0;
                SetLastError(0);
                WriteFile(get(), ptr, uint32_t(len), &n, nullptr);
                set_error(GetLastError(), std::system_category());
                return n;
            }

            inline Winio Winio::dev_null() noexcept {
                return Winio(null_device, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING);
            }

    #endif

    // String buffer

    class StringBuffer:
    public IO {
    public:
        StringBuffer() noexcept: sptr(&sbuf) {}
        explicit StringBuffer(std::string& s): sptr(&s), wpos(s.size()) {}
        StringBuffer(const StringBuffer&) = delete;
        StringBuffer(StringBuffer&& src) noexcept;
        StringBuffer& operator=(const StringBuffer&) = delete;
        StringBuffer& operator=(StringBuffer&& src) noexcept;
        virtual void close() noexcept override { clear(); }
        virtual void flush() noexcept override;
        virtual int getc() noexcept override;
        virtual bool is_open() const noexcept override { return true; }
        virtual bool is_terminal() const noexcept override { return false; }
        virtual void putc(char c) override;
        virtual size_t read(void* ptr, size_t maxlen) noexcept override;
        virtual std::string read_line() override;
        virtual void seek(ptrdiff_t offset, int which = SEEK_CUR) noexcept override;
        virtual ptrdiff_t tell() noexcept override { return 0; }
        virtual size_t write(const void* ptr, size_t len) override;
        virtual void write_n(size_t n, char c) override;
        char* begin() noexcept { return c_ptr() + rpos; }
        const char* begin() const noexcept { return sptr->data() + rpos; }
        char* end() noexcept { return c_ptr() + wpos; }
        const char* end() const noexcept { return sptr->data() + wpos; }
        void clear() noexcept;
        bool empty() const noexcept { return rpos == wpos; }
        size_t size() const noexcept { return wpos - rpos; }
        std::string str() const { return std::string(begin(), end()); }
    private:
        std::string sbuf;
        std::string* sptr;
        size_t rpos = 0;
        size_t wpos = 0;
        char* c_ptr() noexcept { return &(*sptr)[0]; }
        void trim_buffer() noexcept;
    };

        inline StringBuffer::StringBuffer(StringBuffer&& src) noexcept:
        rpos(std::exchange(rpos, 0)),
        wpos(std::exchange(wpos, 0)) {
            if (src.sptr == &src.sbuf) {
                sbuf = std::move(src.sbuf);
                src.sbuf.clear();
                sptr = &sbuf;
            } else {
                sptr = std::exchange(src.sptr, &src.sbuf);
            }
        }

        inline StringBuffer& StringBuffer::operator=(StringBuffer&& src) noexcept {
            if (&src != this) {
                rpos = std::exchange(rpos, 0);
                wpos = std::exchange(wpos, 0);
                if (src.sptr == &src.sbuf) {
                    sbuf = std::move(src.sbuf);
                    src.sbuf.clear();
                    sptr = &sbuf;
                } else {
                    sbuf.clear();
                    sptr = std::exchange(src.sptr, &src.sbuf);
                }
            }
            return *this;
        }

        inline void StringBuffer::flush() noexcept {
            if (rpos == wpos) {
                clear();
            } else {
                sptr->erase(0, rpos);
                wpos -= rpos;
                rpos = 0;
                sptr->resize(wpos);
            }
        }

        inline int StringBuffer::getc() noexcept {
            if (rpos == wpos)
                return EOF;
            int c = char_to<int>((*sptr)[rpos]);
            ++rpos;
            trim_buffer();
            return c;
        }

        inline void StringBuffer::putc(char c) {
            if (wpos < sptr->size())
                *end() = c;
            else
                *sptr += c;
            ++wpos;
        }

        inline size_t StringBuffer::read(void* ptr, size_t maxlen) noexcept {
            size_t n = std::min(maxlen, wpos - rpos);
            if (n) {
                memcpy(ptr, begin(), n);
                rpos += n;
                trim_buffer();
            }
            return n;
        }

        inline std::string StringBuffer::read_line() {
            auto lfptr = static_cast<const char*>(memchr(begin(), '\n', size()));
            size_t ofs = rpos, len = 0;
            if (lfptr == nullptr) {
                len = wpos - rpos;
                rpos = wpos;
            } else {
                len = lfptr - begin();
                rpos += len + 1;
            }
            return sptr->substr(ofs, len);
        }

        inline void StringBuffer::seek(ptrdiff_t offset, int which) noexcept {
            if (offset < 0) {
                // do nothing
            } else if (which == SEEK_END) {
                clear();
            } else {
                rpos = std::min(rpos + offset, wpos);
                trim_buffer();
            }
        }

        inline size_t StringBuffer::write(const void* ptr, size_t len) {
            if (sptr->size() < wpos + len)
                sptr->resize(wpos + len);
            memcpy(end(), ptr, len);
            wpos += len;
            return len;
        }

        inline void StringBuffer::write_n(size_t n, char c) {
            if (sptr->size() >= wpos + n) {
                memset(end(), int(uint8_t(c)), n);
            } else {
                memset(end(), int(uint8_t(c)), sptr->size() - wpos);
                sptr->resize(wpos + n, c);
            }
        }

        inline void StringBuffer::clear() noexcept {
            sptr->clear();
            rpos = wpos = 0;
        }

        inline void StringBuffer::trim_buffer() noexcept {
            if (rpos == wpos) {
                clear();
            } else if (wpos >= 1024 && wpos <= 2 * rpos) {
                sptr->erase(0, rpos);
                wpos -= rpos;
                rpos = 0;
                sptr->resize(wpos);
            }
        }

}
