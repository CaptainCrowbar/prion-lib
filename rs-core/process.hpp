#pragma once

#include "rs-core/channel.hpp"
#include <algorithm>
#include <atomic>
#include <chrono>
#include <string>
#include <system_error>

#ifdef _XOPEN_SOURCE
    #include <sys/select.h>
#else
    #include <io.h>
#endif

namespace RS {

    // Spawned process channels

    class StreamProcess:
    public StreamChannel {
    public:
        RS_NO_COPY_MOVE(StreamProcess);
        explicit StreamProcess(const U8string& cmd);
        virtual ~StreamProcess() noexcept { do_close(); }
        virtual void close() noexcept { do_close(); }
        virtual size_t read(void* dst, size_t maxlen);
        int status() const noexcept { return st; }
    protected:
        virtual state do_wait(Interval::time t);
    private:
        std::atomic<FILE*> fp;
        int st = -1;
        void do_close() noexcept;
    };

        inline StreamProcess::StreamProcess(const U8string& cmd) {
            #ifdef _XOPEN_SOURCE
                errno = 0;
                fp = ::popen(cmd.data(), "r");
            #else
                auto wcmd = uconv<std::wstring>(cmd);
                errno = 0;
                fp = ::_wpopen(wcmd.data(), L"rb");
            #endif
            int err = errno;
            if (! fp)
                throw std::system_error(err, std::generic_category());
        }

        inline size_t StreamProcess::read(void* dst, size_t maxlen) {
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

        inline void StreamProcess::do_close() noexcept {
            if (! fp)
                return;
            int rc = ::pclose(fp);
            if (st == -1)
                st = rc;
            fp = nullptr;
        }

        inline Channel::state StreamProcess::do_wait(Interval::time t) {
            using namespace std::chrono;
            if (! fp)
                return state::closed;
            if (t < Interval::time())
                t = {};
            int fd = fileno(fp);
            auto cs = state::closed;
            #ifdef _XOPEN_SOURCE
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(fd, &fds);
                auto tv = duration_to_timeval(t);
                errno = 0;
                int rc = ::select(fd + 1, &fds, nullptr, &fds, &tv);
                int err = errno;
                if (rc == -1 && err == EBADF)
                    cs = state::closed;
                else if (rc == -1)
                    throw std::system_error(err, std::generic_category());
                else if (rc == 0)
                    cs = state::waiting;
                else
                    cs = state::ready;
            #else
                auto fh = reinterpret_cast<HANDLE>(_get_osfhandle(fd));
                auto ms = duration_cast<milliseconds>(t).count();
                SetLastError(0);
                auto rc = WaitForSingleObject(fh, ms);
                auto err = GetLastError();
                if (rc == WAIT_OBJECT_0)
                    cs = state::ready;
                else if (rc == WAIT_TIMEOUT)
                    cs = state::waiting;
                else if (err == ERROR_INVALID_HANDLE)
                    throw std::system_error(err, windows_category());
                else
                    cs = state::closed;
            #endif
            return cs;
        }

    class TextProcess:
    public MessageChannel<U8string> {
    public:
        RS_NO_COPY_MOVE(TextProcess);
        explicit TextProcess(const U8string& cmd): ps(cmd), buf() {}
        virtual void close() noexcept;
        virtual bool read(U8string& t);
        std::string read_all() { return buf + ps.read_all(); }
        int status() const noexcept { return ps.status(); }
    protected:
        virtual state do_wait(Interval::time t);
    private:
        StreamProcess ps;
        U8string buf;
    };

        inline void TextProcess::close() noexcept {
            ps.close();
            buf.clear();
        }

        inline bool TextProcess::read(U8string& t) {
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

        inline Channel::state TextProcess::do_wait(Interval::time t) {
            using namespace std::chrono;
            t = std::max(t, Interval::time());
            auto deadline = ReliableClock::now() + t;
            Interval::time delta;
            for (;;) {
                auto rc = ps.wait(delta);
                if (rc == state::closed || ! ps.read_to(buf))
                    return buf.empty() ? state::closed : state::ready;
                size_t lf = buf.find('\n');
                if (lf != npos)
                    return state::ready;
                auto now = ReliableClock::now();
                if (now > deadline)
                    return state::waiting;
                delta = duration_cast<Interval::time>(deadline - now);
            }
        }

    // Shell commands

    inline std::string shell(const U8string& cmd) {
        StreamProcess proc(cmd);
        return proc.read_all();
    }

}
