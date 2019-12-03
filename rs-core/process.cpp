#include "rs-core/process.hpp"
#include "rs-core/io.hpp"
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <system_error>

#ifdef _XOPEN_SOURCE
    #include <sys/select.h>
    #include <sys/stat.h>
    #include <sys/wait.h>
    #ifdef __CYGWIN__
        #include <sys/cygwin.h>
    #endif
    #define IO_FUNCTION(f) ::f
#else
    #include <windows.h>
    #include <io.h>
    #define IO_FUNCTION(f) _##f
#endif

using namespace RS::Unicorn;

namespace RS {

    // Class StreamProcess

    StreamProcess::StreamProcess(const Ustring& cmd) {
        #ifdef _XOPEN_SOURCE
            errno = 0;
            fp = ::popen(cmd.data(), "r");
        #else
            auto wcmd = to_wstring(cmd, Utf::throws);
            errno = 0;
            fp = ::_wpopen(wcmd.data(), L"rb");
        #endif
        int err = errno;
        if (! fp)
            throw std::system_error(err, std::generic_category());
    }

    size_t StreamProcess::read(void* dst, size_t maxlen) {
        if (! dst || ! maxlen || ! fp)
            return 0;
        errno = 0;
        size_t rc = fread(dst, 1, maxlen, fp);
        int err = errno;
        if (rc > 0)
            return rc;
        if (err != 0 && err != EBADF)
            throw std::system_error(err, std::generic_category());
        do_close();
        return 0;
    }

    void StreamProcess::do_close() noexcept {
        if (! fp)
            return;
        int rc = IO_FUNCTION(pclose)(fp);
        if (st == -1)
            st = rc;
        fp = nullptr;
    }

    bool StreamProcess::do_wait_for(duration t) {
        using namespace std::chrono;
        if (! fp)
            return true;
        if (t < duration())
            t = {};
        int fd = IO_FUNCTION(fileno)(fp);
        #ifdef _XOPEN_SOURCE
            fd_set fds;
            FD_ZERO(&fds);
            FD_SET(fd, &fds);
            auto tv = duration_to_timeval(t);
            errno = 0;
            int rc = ::select(fd + 1, &fds, nullptr, nullptr, &tv);
            int err = errno;
            if (rc == -1 && err == EBADF)
                return true;
            else if (rc == -1)
                throw std::system_error(err, std::generic_category());
            else
                return rc > 0;
        #else
            auto fh = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
            auto ms = duration_cast<milliseconds>(t).count();
            SetLastError(0);
            auto rc = WaitForSingleObject(fh, uint32_t(ms));
            auto err = GetLastError();
            if (rc == WAIT_OBJECT_0)
                return true;
            else if (rc == WAIT_TIMEOUT)
                return false;
            else if (err == ERROR_INVALID_HANDLE)
                throw std::system_error(err, std::system_category());
            else
                return true;
        #endif
    }

    // Class TextProcess

    void TextProcess::close() noexcept {
        sp.close();
        buf.clear();
    }

    bool TextProcess::read(Ustring& t) {
        if (buf.empty())
            return false;
        size_t lf = buf.find('\n');
        if (lf == npos) {
            t = move(buf);
            buf.clear();
        } else {
            if (lf > 0 && buf[lf - 1] == '\r')
                t.assign(buf, 0, lf - 1);
            else
                t.assign(buf, 0, lf);
            buf.erase(0, lf + 1);
        }
        return true;
    }

    bool TextProcess::do_wait_for(duration t) {
        using namespace std::chrono;
        t = std::max(t, duration());
        auto deadline = ReliableClock::now() + t;
        duration delta = {};
        for (;;) {
            sp.wait_for(delta);
            if (sp.is_closed() || ! sp.read_to(buf))
                return true;
            size_t lf = buf.find('\n');
            if (lf != npos)
                return true;
            auto now = ReliableClock::now();
            if (now > deadline)
                return false;
            delta = duration_cast<duration>(deadline - now);
        }
    }

    // Shell commands

    std::string shell(const Ustring& cmd) {
        StreamProcess proc(cmd);
        return proc.read_all();
    }

    void preview_file(const Path& file) {
        #ifdef __CYGWIN__
            struct stat st;
            if (::stat(file.c_name(), &st) != 0)
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), file.name());
            if ((st.st_mode & 0111) == 0)
                chmod(file.c_name(), 0775);
            auto name = to_utf8(file.native_name(), Utf::replace);
            Ustring cmd = "cmd /c \"" + name + "\"";
            int rc = std::system(cmd.data());
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::generic_category(), file.name());
            rc = WEXITSTATUS(rc);
            if (rc != 0)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument), file.name() + " returned " + std::to_string(rc));
        #elif defined(_WIN32)
            if (! file.exists())
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), file.name());
            std::wstring cmd = L"cmd /c \"" + file.os_name() + L"\"";
            int rc = _wsystem(cmd.data());
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::generic_category(), file.name());
            if (rc != 0)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument), file.name() + " returned " + std::to_string(rc));
        #else
            if (! file.exists())
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), file.name());
            Ustring cmd;
            #ifdef __APPLE__
                cmd = "open";
            #elif defined(__linux__)
                cmd = "gnome-open";
            #else
                #error No file open command is configured for this operating system
            #endif
            cmd += " \"" + file.name() + "\"";
            int rc = std::system(cmd.data());
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::generic_category(), file.name());
            rc = WEXITSTATUS(rc);
            if (rc != 0)
                throw std::system_error(std::make_error_code(std::errc::invalid_argument), file.name() + " returned " + std::to_string(rc));
        #endif
    }

}
