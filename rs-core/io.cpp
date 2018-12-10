#include "rs-core/io.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <random>

#ifdef _XOPEN_SOURCE

    #include <sys/stat.h>
    #include <unistd.h>

    #ifdef __APPLE__
        #include <sys/syslimits.h>
    #endif

    #define IO_FUNCTION(f) ::f

#else

    #include <io.h>
    #include <windows.h>

    #define IO_FUNCTION(f) _##f

#endif

using namespace RS::Unicorn;

namespace RS {

    namespace {

        #if defined(__APPLE__)

            std::string path_from_fd(int fd) {
                if (fd == -1)
                    return {};
                std::string path(PATH_MAX, '\0');
                auto rc = ::fcntl(fd, F_GETPATH, path.data());
                if (rc < 0)
                    return {};
                null_term(path);
                return path;
            }

        #elif defined(__CYGWIN__) || defined(__linux__)

            std::string path_from_fd(int fd) {
                if (fd == -1)
                    return {};
                std::string link = "/proc/self/fd/" + std::to_string(fd);
                std::string path(256, '\0');
                bool ok = false;
                while (! ok) {
                    auto rc = ::readlink(link.data(), path.data(), path.size());
                    if (rc < 0)
                        return {};
                    ok = size_t(rc) < path.size();
                    path.resize(rc);
                }
                return path;
            }

        #elif defined(_WIN32)

            std::wstring path_from_handle(HANDLE handle) {
                if (! handle || handle == INVALID_HANDLE_VALUE)
                    return {};
                std::wstring path(256, L'\0');
                bool ok = false;
                while (! ok) {
                    auto rc = GetFinalPathNameByHandleW(handle, path.data(), DWORD(path.size()), 0);
                    if (rc == 0)
                        return {};
                    ok = rc < path.size();
                    path.resize(rc);
                }
                return path;
            }

        #else

            std::string path_from_fd(int /*fd*/) {
                return {};
            }

        #endif

    }

    // Class IO

    int IO::getc() noexcept {
        unsigned char b = 0;
        return read(&b, 1) ? int(b) : EOF;
    }

    std::string IO::read_line() {
        std::string s;
        for (;;) {
            int c = getc();
            if (c == EOF)
                break;
            s += char(c);
            if (c == '\n')
                break;
        }
        return s;
    }

    void IO::write_n(size_t n, char c) {
        static constexpr size_t block = 16384;
        std::string buf(std::min(n, block), c);
        auto qr = divide(n, block);
        do_n(qr.first, [&] { write(buf.data(), block); });
        if (qr.second)
            write(buf.data(), qr.second);
    }

    void IO::check(Uview detail) const {
        if (status) {
            if (detail.empty())
                throw std::system_error(status);
            else
                throw std::system_error(status, Ustring(detail));
        }
    }

    std::string IO::read_all() {
        static constexpr size_t block = 16384;
        std::string s;
        while (read_n(s, block)) {}
        return s;
    }

    size_t IO::read_n(std::string& s, size_t maxlen) {
        size_t offset = s.size();
        s.resize(offset + maxlen);
        size_t n = read(&s[0] + offset, maxlen);
        s.resize(offset + n);
        return n;
    }

    std::string IO::read_str(size_t maxlen) {
        std::string s(maxlen, 0);
        s.resize(read(&s[0], maxlen));
        return s;
    }

    void IO::write_line(std::string_view str) {
        if (str.empty())
            putc('\n');
        else if (str.back() == '\n')
            write_str(str);
        else
            write_str(std::string(str) + '\n');
    }

    size_t IO::write_str(std::string_view str) {
        static constexpr char dummy = '\0';
        const char* ptr = str.empty() ? &dummy : str.data();
        size_t ofs = 0, len = str.size();
        do ofs += write(ptr + ofs, len - ofs);
            while (ofs < len && ok());
        return ofs;
    }

    void IO::set_error(int err, const std::error_category& cat) noexcept {
        if (err)
            status = {err, cat};
        else
            status.clear();
    }

    IO::line_iterator& IO::line_iterator::operator++() {
        if (iop) {
            line = iop->read_line();
            if (line.empty())
                iop = nullptr;
        }
        return *this;
    }

    // Class Cstdio

    Cstdio::Cstdio(FILE* f, bool owner) noexcept {
        if (owner)
            fp = {f, [] (FILE* p) { if (p) ::fclose(p); }};
        else
            fp = {f, [] (FILE*) {}};
    }

    Cstdio::Cstdio(const Unicorn::Path& f, mode m) {
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
            // behaviour we want from IO::mode::open_always. Try "rb+" first
            // (open for R/W if the file exists, otherwise fail), and if that
            // fails, try "wb+" (open for R/W regardless, destroying any
            // existing file). There is a race condition here but I don't
            // think there is any way around it.
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

    Cstdio::Cstdio(const Unicorn::Path& f, const Ustring& iomode) {
        #ifdef _XOPEN_SOURCE
            errno = 0;
            auto rc = ::fopen(f.c_name(), iomode.data());
        #else
            auto wmode = to_wstring(iomode);
            errno = 0;
            auto rc = _wfopen(f.c_name(), wmode.data());
        #endif
        int err = errno;
        *this = Cstdio(rc);
        set_error(err);
    }

    void Cstdio::close() noexcept {
        if (fp) {
            FILE* f = fp.release();
            errno = 0;
            ::fclose(f);
            set_error(errno);
        }
    }

    void Cstdio::flush() noexcept {
        errno = 0;
        ::fflush(get());
        set_error(errno);
    }

    int Cstdio::getc() noexcept {
        errno = 0;
        int c = ::fgetc(get());
        set_error(errno);
        return c;
    }

    Path Cstdio::get_path() const {
        #ifdef _XOPEN_SOURCE
            return path_from_fd(fd());
        #else
            return path_from_handle(HANDLE(_get_osfhandle(fd())));
        #endif
    }

    bool Cstdio::is_terminal() const noexcept {
        auto desc = fd();
        return desc != -1 && IO_FUNCTION(isatty)(desc);
    }

    void Cstdio::putc(char c) {
        errno = 0;
        ::fputc(int(uint8_t(c)), get());
        set_error(errno);
    }

    size_t Cstdio::read(void* ptr, size_t maxlen) noexcept {
        errno = 0;
        size_t n = ::fread(ptr, 1, maxlen, get());
        set_error(errno);
        return n;
    }

    std::string Cstdio::read_line() {
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
                return buf.substr(0, lfpos + 1);
            size_t ntpos = buf.find_last_not_of('\0') + 1;
            if (ntpos < block - 1)
                return buf.substr(0, ntpos);
            buf.pop_back();
        }
    }

    void Cstdio::seek(ptrdiff_t offset, int which) noexcept {
        errno = 0;
        #ifdef _XOPEN_SOURCE
            ::fseeko(get(), offset, which);
        #else
            _fseeki64(get(), offset, which);
        #endif
        set_error(errno);
    }

    ptrdiff_t Cstdio::tell() noexcept {
        errno = 0;
        #ifdef _XOPEN_SOURCE
            ptrdiff_t offset = ::ftello(get());
        #else
            ptrdiff_t offset = _ftelli64(get());
        #endif
        set_error(errno);
        return offset;
    }

    size_t Cstdio::write(const void* ptr, size_t len) {
        errno = 0;
        size_t n = ::fwrite(ptr, 1, len, get());
        set_error(errno);
        return n;
    }

    int Cstdio::fd() const noexcept {
        auto ptr = get();
        return ptr ? IO_FUNCTION(fileno)(ptr) : -1;
    }

    void Cstdio::ungetc(char c) {
        errno = 0;
        ::ungetc(int(uint8_t(c)), get());
        set_error(errno);
    }

    Cstdio Cstdio::dev_null() noexcept {
        Cstdio io(null_device, "r+");
        ::fgetc(io.get()); // Clear bogus ioctl error
        return io;
    }

    // Class Fdio

    Fdio::Fdio(int f, bool owner) noexcept {
        if (owner)
            fd = {f, [] (int d) { if (d != -1) IO_FUNCTION(close)(d); }};
        else
            fd = {f, [] (int) {}};
    }

    Fdio::Fdio(const Unicorn::Path& f, mode m) {
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

    Fdio::Fdio(const Unicorn::Path& f, int iomode, int perm) {
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
            errno = 0;
            auto rc = _wopen(f.c_name(), iomode, perm);
        #endif
        int err = errno;
        *this = Fdio(rc);
        set_error(err);
    }

    void Fdio::close() noexcept {
        int f = fd.release();
        if (f != -1) {
            errno = 0;
            IO_FUNCTION(close)(f);
            set_error(errno);
        }
    }

    void Fdio::flush() noexcept {
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

    Path Fdio::get_path() const {
        #ifdef _XOPEN_SOURCE
            return path_from_fd(get());
        #else
            return path_from_handle(HANDLE(_get_osfhandle(get())));
        #endif
    }

    bool Fdio::is_terminal() const noexcept {
        return IO_FUNCTION(isatty)(get());
    }

    size_t Fdio::read(void* ptr, size_t maxlen) noexcept {
        errno = 0;
        auto rc = IO_FUNCTION(read)(get(), ptr, iosize(maxlen));
        set_error(errno);
        return rc;
    }

    void Fdio::seek(ptrdiff_t offset, int which) noexcept {
        errno = 0;
        IO_FUNCTION(lseek)(get(), ofsize(offset), which);
        set_error(errno);
    }

    ptrdiff_t Fdio::tell() noexcept {
        errno = 0;
        auto offset = IO_FUNCTION(lseek)(get(), 0, SEEK_CUR);
        set_error(errno);
        return offset;
    }

    size_t Fdio::write(const void* ptr, size_t len) {
        errno = 0;
        size_t n = IO_FUNCTION(write)(get(), ptr, iosize(len));
        set_error(errno);
        return n;
    }

    Fdio Fdio::dup() noexcept {
        errno = 0;
        int rc = IO_FUNCTION(dup)(get());
        set_error(errno);
        return Fdio(rc);
    }

    Fdio Fdio::dup(int f) noexcept {
        errno = 0;
        IO_FUNCTION(dup2)(get(), f);
        set_error(errno);
        return Fdio(f);
    }

    Fdio Fdio::dev_null() noexcept {
        int iomode = O_RDWR;
        #ifdef O_CLOEXEC
            iomode |= O_CLOEXEC;
        #endif
        return Fdio(null_device, iomode);
    }

    std::pair<Fdio, Fdio> Fdio::pipe(size_t winmem) {
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

        // Class Winio

        Winio::Winio(void* f, bool owner) noexcept {
            if (owner)
                fh = {f, [] (void* f) { if (f && f != INVALID_HANDLE_VALUE) CloseHandle(f); }};
            else
                fh = {f, [] (void*) {}};
        }

        Winio::Winio(const Unicorn::Path& f, mode m) {
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

        Winio::Winio(const Unicorn::Path& f, uint32_t desired_access, uint32_t share_mode, LPSECURITY_ATTRIBUTES security_attributes,
                uint32_t creation_disposition, uint32_t flags_and_attributes, HANDLE template_file) {
            SetLastError(0);
            auto rc = CreateFileW(f.c_name(), desired_access, share_mode, security_attributes, creation_disposition, flags_and_attributes, template_file);
            int err = GetLastError();
            *this = Winio(rc);
            set_error(err, std::system_category());
        }

        void Winio::close() noexcept {
            void* h = fh.release();
            if (h && h != INVALID_HANDLE_VALUE) {
                SetLastError(0);
                CloseHandle(h);
                set_error(GetLastError(), std::system_category());
            }
        }

        void Winio::flush() noexcept {
            if (! FlushFileBuffers(get()))
                set_error(GetLastError(), std::system_category());
        }

        Path Winio::get_path() const {
            return path_from_handle(get());
        }

        bool Winio::is_terminal() const noexcept {
            auto h = get();
            if (h == GetStdHandle(STD_INPUT_HANDLE))
                return IO_FUNCTION(isatty)(0);
            else if (h == GetStdHandle(STD_OUTPUT_HANDLE))
                return IO_FUNCTION(isatty)(1);
            else if (h == GetStdHandle(STD_ERROR_HANDLE))
                return IO_FUNCTION(isatty)(2);
            else
                return false;
        }

        size_t Winio::read(void* ptr, size_t maxlen) noexcept {
            DWORD n = 0;
            SetLastError(0);
            ReadFile(get(), ptr, uint32_t(maxlen), &n, nullptr);
            set_error(GetLastError(), std::system_category());
            return n;
        }

        void Winio::seek(ptrdiff_t offset, int which) noexcept {
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

        ptrdiff_t Winio::tell() noexcept {
            LARGE_INTEGER distance, result;
            distance.QuadPart = result.QuadPart = 0;
            SetLastError(0);
            SetFilePointerEx(get(), distance, &result, FILE_CURRENT);
            set_error(GetLastError(), std::system_category());
            return result.QuadPart;
        }

        size_t Winio::write(const void* ptr, size_t len) {
            DWORD n = 0;
            SetLastError(0);
            WriteFile(get(), ptr, uint32_t(len), &n, nullptr);
            set_error(GetLastError(), std::system_category());
            return n;
        }

        Winio Winio::dev_null() noexcept {
            return Winio(null_device, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING);
        }

    #endif

    // Class StringBuffer

    StringBuffer::StringBuffer(StringBuffer&& src) noexcept:
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

    StringBuffer& StringBuffer::operator=(StringBuffer&& src) noexcept {
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

    void StringBuffer::flush() noexcept {
        if (rpos == wpos) {
            clear();
        } else {
            sptr->erase(0, rpos);
            wpos -= rpos;
            rpos = 0;
            sptr->resize(wpos);
        }
    }

    int StringBuffer::getc() noexcept {
        if (rpos == wpos)
            return EOF;
        int c = char_to<int>((*sptr)[rpos]);
        ++rpos;
        trim_buffer();
        return c;
    }

    void StringBuffer::putc(char c) {
        if (wpos < sptr->size())
            *end() = c;
        else
            *sptr += c;
        ++wpos;
    }

    size_t StringBuffer::read(void* ptr, size_t maxlen) noexcept {
        size_t n = std::min(maxlen, wpos - rpos);
        if (n) {
            memcpy(ptr, begin(), n);
            rpos += n;
            trim_buffer();
        }
        return n;
    }

    std::string StringBuffer::read_line() {
        auto lfptr = static_cast<const char*>(memchr(begin(), '\n', size()));
        size_t ofs = rpos, len = 0;
        if (lfptr == nullptr) {
            len = wpos - rpos;
            rpos = wpos;
        } else {
            len = lfptr - begin() + 1;
            rpos += len;
        }
        return sptr->substr(ofs, len);
    }

    void StringBuffer::seek(ptrdiff_t offset, int which) noexcept {
        if (offset < 0) {
            // do nothing
        } else if (which == SEEK_END) {
            clear();
        } else {
            rpos = std::min(rpos + offset, wpos);
            trim_buffer();
        }
    }

    size_t StringBuffer::write(const void* ptr, size_t len) {
        if (sptr->size() < wpos + len)
            sptr->resize(wpos + len);
        memcpy(end(), ptr, len);
        wpos += len;
        return len;
    }

    void StringBuffer::write_n(size_t n, char c) {
        if (sptr->size() >= wpos + n) {
            memset(end(), int(uint8_t(c)), n);
        } else {
            memset(end(), int(uint8_t(c)), sptr->size() - wpos);
            sptr->resize(wpos + n, c);
        }
    }

    void StringBuffer::clear() noexcept {
        sptr->clear();
        rpos = wpos = 0;
    }

    void StringBuffer::trim_buffer() noexcept {
        if (rpos == wpos) {
            clear();
        } else if (wpos >= 1024 && wpos <= 2 * rpos) {
            sptr->erase(0, rpos);
            wpos -= rpos;
            rpos = 0;
            sptr->resize(wpos);
        }
    }

    // Class TempFile

    TempFile::TempFile() {
        errno = 0;
        auto file = std::tmpfile();
        auto err = errno;
        if (! file)
            throw std::system_error(err, std::generic_category());
        Cstdio io(file);
        Cstdio::operator=(std::move(io));
    }

    TempFile::TempFile(const Unicorn::Path& dir, Uview prefix) {
        static constexpr int max_tries = 10;
        if (! dir.empty() && ! dir.is_directory())
            throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
        std::random_device dev;
        Fdio fdio;
        for (int i = 0; i < max_tries && ! fdio.ok(); ++i) {
            uint64_t x = dev();
            uint64_t y = dev();
            uint64_t z = (x << 32) + y;
            where = dir / (Ustring(prefix) + hex(z));
            where = where.resolve();
            fdio = Fdio(where, O_RDWR | O_CREAT | O_EXCL);
        }
        fdio.check();
        FILE* file = nullptr;
        #if defined(_WIN32) && ! defined(__CYGWIN__)
            file = _wfdopen(fdio.release(), L"rb+");
        #else
            file = fdopen(fdio.release(), "rb+");
        #endif
        Cstdio io(file);
        Cstdio::operator=(std::move(io));
    }

    TempFile::~TempFile() noexcept {
        if (! where.empty()) {
            try { close(); } catch (...) {}
            try { where.remove(); } catch (...) {}
        }
    }

    Path TempFile::get_path() const {
        if (where.empty())
            return Cstdio::get_path();
        else
            return where;
    }

}
