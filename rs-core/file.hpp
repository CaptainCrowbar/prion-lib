#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include "rs-core/time.hpp"
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <new>
#include <ostream>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#ifdef _XOPEN_SOURCE
    #include <dirent.h>
    #include <fcntl.h>
    #include <pwd.h>
    #include <sys/stat.h>
    #include <sys/time.h>
    #include <sys/types.h>
    #include <unistd.h>
#else
    #include <shlobj.h>
#endif

#ifdef __CYGWIN__
    #include <sys/cygwin.h>
#endif

RS_LDLIB(msvc: shell32);

namespace RS {

    class File:
    public LessThanComparable<File> {
    public:
        static constexpr uint32_t append = 1;
        static constexpr uint32_t recurse = 2;
        static constexpr uint32_t require = 4;
        File() = default;
        File(const std::string& name): path(name) { normalize(); }
        File(const char* name): path(name ? name : "") { normalize(); }
        #ifndef _XOPEN_SOURCE
            File(const std::wstring& name): path(uconv<std::string>(name)) { normalize(); }
            File(const wchar_t* name): path(name ? uconv<std::string>(std::wstring(name)) : "") { normalize(); }
        #endif
        // File name operations
        const std::string& name() const { return path; }
        const char* c_name() const noexcept { return path.data(); }
        bool empty() const noexcept { return path.empty(); }
        bool is_absolute() const noexcept;
        File parent() const;
        File leaf() const;
        std::string base() const;
        std::string ext() const;
        #if defined(_XOPEN_SOURCE) && ! defined(__CYGWIN__)
            std::string native() const { return path; }
        #else
            std::wstring native() const;
        #endif
        File& operator/=(const File& rhs) { File f = *this / rhs; path = std::move(f.path); return *this; }
        friend File operator/(const File& lhs, const File& rhs);
        // File system query operations
        bool exists() const noexcept;
        bool is_directory() const noexcept;
        bool is_symlink() const noexcept;
        std::vector<File> list() const;
        uint64_t size() const;
        // File system update operations
        void mkdir(uint32_t flags = 0) const;
        void move_to(const File& dst) const;
        void remove(uint32_t flags = 0) const;
        // I/O operations
        std::string load(size_t limit = npos, uint32_t flags = 0) const;
        void save(const void* ptr, size_t len, uint32_t flags = 0) const;
        void save(const std::string& content, uint32_t flags = 0) const { save(content.data(), content.size(), flags); }
        // Metadata operations
        std::chrono::system_clock::time_point atime() const;
        std::chrono::system_clock::time_point mtime() const;
        void set_atime() const { set_atime(std::chrono::system_clock::now()); }
        void set_atime(std::chrono::system_clock::time_point t) const;
        void set_mtime() const { set_mtime(std::chrono::system_clock::now()); }
        void set_mtime(std::chrono::system_clock::time_point t) const;
        // Standard locations
        void set_cwd() const;
        static File cwd();
        static File user_home();
        static File user_documents();
        static File user_cache();
        static File user_settings();
    private:
        std::string path;
        void do_remove() const;
        void normalize() noexcept;
        #ifndef _XOPEN_SOURCE
            char get_file_type() const noexcept;
            static std::wstring get_known_folder(int id);
        #endif
    };

    // File name operations

    inline File File::parent() const {
        size_t pos = path.find_last_of('/') + 1;
        return pos ? path.substr(0, pos - 1) : File();
    }

    inline File File::leaf() const {
        size_t pos = path.find_last_of('/') + 1;
        return path.substr(pos, npos);
    }

    inline std::string File::base() const {
        size_t start = path.find_last_of('/') + 1;
        size_t dot = path.find_last_of('.');
        return dot > start + 1 && dot != npos ? path.substr(start, dot - start) : path.substr(start, npos);
    }

    inline std::string File::ext() const {
        size_t start = path.find_last_of('/') + 1;
        size_t dot = path.find_last_of('.');
        return dot > start + 1 && dot != npos ? path.substr(dot, npos) : std::string();
    }

    inline File operator/(const File& lhs, const File& rhs) {
        if (lhs.empty() || rhs.is_absolute())
            return rhs;
        else if (rhs.empty())
            return lhs;
        std::string path = lhs.path;
        if (path.back() != '/')
            path += '/';
        path += rhs.path;
        return path;
    }

    inline bool operator==(const File& lhs, const File& rhs) noexcept {
        return lhs.name() == rhs.name();
    }

    inline bool operator<(const File& lhs, const File& rhs) noexcept {
        return lhs.name() < rhs.name();
    }

    inline std::ostream& operator<<(std::ostream& out, const File& f) {
        return out << f.name();
    }

    inline std::string to_str(const File& f) {
        return f.name();
    }

    // File system update operations

    inline void File::move_to(const File& dst) const {
        errno = 0;
        int rc = ::rename(path.data(), dst.path.data());
        int err = errno;
        if (rc)
            throw std::system_error(err, std::generic_category(), path + " => " + dst.path);
    }

    inline void File::remove(uint32_t flags) const {
        if (! exists())
            return;
        bool rm_rf = flags & recurse;
        if (rm_rf && is_directory() && ! is_symlink())
            for (auto& file: list())
                file.remove(recurse);
        do_remove();
    }

    // I/O operations

    inline std::string File::load(size_t limit, uint32_t flags) const {
        static constexpr size_t block = 16384;
        FILE* in = stdin;
        ScopeExit guard([&] { if (in && in != stdin) ::fclose(in); });
        if (! path.empty() && path != "-") {
            errno = 0;
            in = ::fopen(path.data(), "rb");
            int err = errno;
            if (! in) {
                if (flags & require)
                    throw std::system_error(err, std::generic_category(), path);
                else
                    return {};
            }
        }
        std::string buf;
        while (buf.size() < limit) {
            size_t ofs = buf.size(), n = std::min(limit - ofs, block);
            buf.append(n, '\0');
            errno = 0;
            size_t rc = ::fread(&buf[0] + ofs, 1, n, in);
            int err = errno;
            if (err)
                throw std::system_error(err, std::generic_category(), path);
            buf.resize(ofs + rc);
            if (rc < n)
                break;
        }
        return buf;
    }

    inline void File::save(const void* ptr, size_t len, uint32_t flags) const {
        FILE* out = stdout;
        ScopeExit guard([&] { if (out && out != stdout) ::fclose(out); });
        if (! path.empty() && path != "-") {
            errno = 0;
            out = ::fopen(path.data(), flags & append ? "ab" : "wb");
            int err = errno;
            if (! out)
                throw std::system_error(err, std::generic_category(), path);
        }
        auto cptr = static_cast<const char*>(ptr);
        size_t pos = 0;
        while (pos < len) {
            errno = 0;
            pos += ::fwrite(cptr + pos, 1, len - pos, out);
            int err = errno;
            if (err)
                throw std::system_error(err, std::generic_category(), path);
        }
    }

    #ifdef _XOPEN_SOURCE

        // File name operations

        inline bool File::is_absolute() const noexcept {
            return path[0] == '/';
        }

        #ifdef __CYGWIN__

            inline std::wstring File::native() const {
                static constexpr auto flags = CCP_POSIX_TO_WIN_W | CCP_RELATIVE;
                if (path.empty())
                    return {};
                errno = 0;
                auto rc = cygwin_conv_path(flags, path.data(), nullptr, 0);
                int err = errno;
                if (rc < 0)
                    throw std::system_error(err, std::generic_category(), path);
                std::wstring wpath(rc / 2, L'\0');
                errno = 0;
                rc = cygwin_conv_path(flags, path.data(), &wpath[0], 2 * wpath.size());
                err = errno;
                if (rc < 0)
                    throw std::system_error(err, std::generic_category(), path);
                null_term(wpath);
                return wpath;
            }

        #endif

        // File system query operations

        inline bool File::exists() const noexcept {
            struct stat st;
            return ::stat(path.data(), &st) == 0;
        }

        inline bool File::is_directory() const noexcept {
            struct stat st;
            int rc = ::stat(path.data(), &st);
            return rc == 0 && S_ISDIR(st.st_mode);
        }

        inline bool File::is_symlink() const noexcept {
            struct stat st;
            int rc = ::lstat(path.data(), &st);
            return rc == 0 && S_ISLNK(st.st_mode);
        }

        inline std::vector<File> File::list() const {
            auto dirptr = opendir(path.data());
            if (! dirptr)
                return {};
            ScopeExit guard([&] { closedir(dirptr); });
            std::vector<File> files;
            std::string leaf;
            File f;
            dirent entry;
            dirent* entptr = nullptr;
            bool cd = path == ".";
            for (;;) {
                int rc = readdir_r(dirptr, &entry, &entptr);
                if (rc || ! entptr)
                    break;
                leaf = entry.d_name;
                if (leaf == "." || leaf == "..")
                    continue;
                f = leaf;
                if (! cd)
                    f = *this / f;
                files.push_back(f);
            }
            return files;
        }

        inline uint64_t File::size() const {
            struct stat st;
            int rc = ::stat(path.data(), &st);
            return rc == 0 ? st.st_size : 0;
        }

        // File system update operations

        inline void File::mkdir(uint32_t flags) const {
            if (is_directory())
                return;
            bool mkdir_p = flags & recurse;
            errno = 0;
            int rc = ::mkdir(path.data(), 0777);
            int err = errno;
            if (rc == 0)
                return;
            if (err != ENOENT || ! mkdir_p)
                throw std::system_error(err, std::generic_category(), path);
            File p = parent();
            if (p == *this)
                throw std::system_error(err, std::generic_category(), path);
            p.mkdir(recurse);
            errno = 0;
            rc = ::mkdir(path.data(), 0777);
            err = errno;
            if (rc)
                throw std::system_error(err, std::generic_category(), path);
        }

        // Metadata operations

        inline std::chrono::system_clock::time_point File::atime() const {
            struct stat st;
            if (::stat(path.data(), &st) == -1)
                return {};
            #ifdef __APPLE__
                return timespec_to_timepoint(st.st_atimespec);
            #else
                return timespec_to_timepoint(st.st_atim);
            #endif
        }

        inline std::chrono::system_clock::time_point File::mtime() const {
            struct stat st;
            if (::stat(path.data(), &st) == -1)
                return {};
            #ifdef __APPLE__
                return timespec_to_timepoint(st.st_mtimespec);
            #else
                return timespec_to_timepoint(st.st_mtim);
            #endif
        }

        inline void File::set_atime(std::chrono::system_clock::time_point t) const {
            timeval times[2];
            times[0] = timepoint_to_timeval(t);
            times[1] = timepoint_to_timeval(mtime());
            errno = 0;
            int rc = ::utimes(path.data(), times);
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::generic_category(), path);
        }

        inline void File::set_mtime(std::chrono::system_clock::time_point t) const {
            timeval times[2];
            times[0] = timepoint_to_timeval(atime());
            times[1] = timepoint_to_timeval(t);
            errno = 0;
            int rc = ::utimes(path.data(), times);
            int err = errno;
            if (rc == -1)
                throw std::system_error(err, std::generic_category(), path);
        }

        // Standard locations
        // For Linux see https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html

        inline void File::set_cwd() const {
            if (chdir(path.data()) == -1) {
                int err = errno;
                throw std::system_error(err, std::generic_category(), path);
            }
        }

        inline File File::cwd() {
            std::string buf(256, '\0');
            for (;;) {
                errno = 0;
                if (::getcwd(&buf[0], buf.size()))
                    break;
                int err = errno;
                if (err == ENOMEM)
                    throw std::bad_alloc();
                else if (err != ERANGE)
                    throw std::system_error(err, std::generic_category());
                buf.resize(2 * buf.size());
            }
            null_term(buf);
            return buf;
        }

        inline File File::user_home() {
            auto home = ::getenv("HOME");
            if (home && *home)
                return home;
            std::string buf(256, '\0');
            passwd pwdbuf;
            passwd* pwdptr = nullptr;
            for (;;) {
                int rc = getpwuid_r(getuid(), &pwdbuf, &buf[0], buf.size(), &pwdptr);
                if (rc == 0)
                    return pwdptr->pw_dir;
                if (rc != ERANGE)
                    throw std::system_error(rc, std::generic_category());
                buf.resize(2 * buf.size());
            }
        }

        inline File File::user_documents() {
            return user_home() / "Documents";
        }

        inline File File::user_cache() {
            #ifdef __APPLE__
                return user_home() / "Library/Caches";
            #else
                auto env = ::getenv("XDG_CACHE_HOME");
                if (env && *env)
                    return env;
                else
                    return user_home() / ".cache";
            #endif
        }

        inline File File::user_settings() {
            #ifdef __APPLE__
                return user_home() / "Library/Application Support";
            #else
                auto env = ::getenv("XDG_CONFIG_HOME");
                if (env && *env)
                    return env;
                else
                    return user_home() / ".config";
            #endif
        }

        // Implementation details

        inline void File::do_remove() const {
            errno = 0;
            int rc = ::remove(path.data());
            int err = errno;
            if (rc)
                throw std::system_error(err, std::generic_category(), path);
        }

        inline void File::normalize() noexcept {}

    #else

        // File name operations

        inline bool File::is_absolute() const noexcept {
            return path[0] == '/' || (path.size() >= 3 && ascii_isalpha(path[0]) && path[1] == ':' && path[2] == '/');
        }

        inline std::wstring File::native() const {
            std::wstring wpath = uconv<std::wstring>(path);
            std::replace(wpath.begin(), wpath.end(), L'/', L'\\');
            return wpath;
        }

        // File system query operations

        inline bool File::exists() const noexcept {
            return get_file_type() != 'n';
        }

        inline bool File::is_directory() const noexcept {
            return get_file_type() == 'd';
        }

        inline bool File::is_symlink() const noexcept {
            return false;
        }

        inline std::vector<File> File::list() const {
            if (! is_directory())
                return {};
            auto glob = (*this / "*").native();
            WIN32_FIND_DATAW info;
            auto handle = FindFirstFileW(glob.data(), &info);
            if (! handle)
                return {};
            ScopeExit guard([&] { FindClose(handle); });
            std::vector<File> files;
            std::wstring wleaf;
            File f;
            bool cd = path == ".";
            for (;;) {
                wleaf = info.cFileName;
                if (wleaf != L"." && wleaf != L"..") {
                    f = wleaf;
                    if (! cd)
                        f = *this / f;
                    files.push_back(f);
                }
                if (! FindNextFile(handle, &info))
                    break;
            }
            return files;
        }

        inline uint64_t File::size() const {
            auto wpath = native();
            WIN32_FILE_ATTRIBUTE_DATA info;
            int rc = GetFileAttributesExW(wpath.data(), GetFileExInfoStandard, &info);
            if (rc)
                return (uint64_t(info.nFileSizeHigh) << 32) + info.nFileSizeLow;
            else
                return 0;
        }

        // Metadata operations

        inline std::chrono::system_clock::time_point File::atime() const {
            std::wstring wpath = native();
            auto fh = CreateFileW(wpath.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (! fh)
                return {};
            auto res = make_resource(fh, CloseHandle);
            FILETIME ft;
            if (GetFileTime(res, nullptr, &ft, nullptr))
                return filetime_to_timepoint(ft);
            else
                return {};
        }

        inline std::chrono::system_clock::time_point File::mtime() const {
            std::wstring wpath = native();
            auto fh = CreateFileW(wpath.data(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (! fh)
                return {};
            auto res = make_resource(fh, CloseHandle);
            FILETIME ft;
            if (GetFileTime(res, nullptr, nullptr, &ft))
                return filetime_to_timepoint(ft);
            else
                return {};
        }

        inline void File::set_atime(std::chrono::system_clock::time_point t) const {
            std::wstring wpath = native();
            SetLastError(0);
            auto fh = CreateFileW(wpath.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            auto err = GetLastError();
            if (! fh)
                throw std::system_error(err, std::system_category(), path);
            auto res = make_resource(fh, CloseHandle);
            auto ft = timepoint_to_filetime(t);
            auto rc = SetFileTime(res, nullptr, &ft, nullptr);
            err = GetLastError();
            if (! rc)
                throw std::system_error(err, std::system_category(), path);
        }

        inline void File::set_mtime(std::chrono::system_clock::time_point t) const {
            std::wstring wpath = native();
            SetLastError(0);
            auto fh = CreateFileW(wpath.data(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
            auto err = GetLastError();
            if (! fh)
                throw std::system_error(err, std::system_category(), path);
            auto res = make_resource(fh, CloseHandle);
            auto ft = timepoint_to_filetime(t);
            auto rc = SetFileTime(res, nullptr, nullptr, &ft);
            err = GetLastError();
            if (! rc)
                throw std::system_error(err, std::system_category(), path);
        }

        // Standard locations

        inline void File::set_cwd() const {
            auto wpath = native();
            if (! SetCurrentDirectoryW(wpath.data())) {
                auto err = GetLastError();
                throw std::system_error(err, std::system_category(), path);
            }
        }

        inline File File::cwd() {
            std::wstring wpath(256, L'\0');
            uint32_t rc = 0;
            for (;;) {
                rc = GetCurrentDirectoryW(uint32_t(wpath.size()), &wpath[0]);
                auto err = GetLastError();
                if (rc == 0)
                    throw std::system_error(err, std::system_category());
                else if (rc < wpath.size())
                    break;
                wpath.resize(2 * wpath.size());
            }
            wpath.resize(rc);
            return wpath;
        }

        inline File File::user_home() {
            return get_known_folder(CSIDL_PROFILE);
        }

        inline File File::user_documents() {
            return get_known_folder(CSIDL_MYDOCUMENTS);
        }

        inline File File::user_cache() {
            return get_known_folder(CSIDL_LOCAL_APPDATA);
        }

        inline File File::user_settings() {
            return get_known_folder(CSIDL_APPDATA);
        }

        // Implementation details

        inline void File::do_remove() const {
            auto wpath = native();
            SetLastError(0);
            uint32_t rc = 0;
            if (is_directory())
                rc = RemoveDirectoryW(wpath.data());
            else
                rc = DeleteFileW(wpath.data());
            auto err = GetLastError();
            if (! rc)
                throw std::system_error(err, std::system_category(), path);
        }

        inline void File::normalize() noexcept {
            std::replace(path.begin(), path.end(), '\\', '/');
        }

        inline char File::get_file_type() const noexcept {
            if (path.empty())
                return 'n';
            else if (path == "." || path == "..")
                return 'd';
            std::wstring wpath = native();
            if (path.size() == 3 && ascii_isalpha(path[0]) && path[1] == ':' && path[2] == '/') {
                auto rc = GetDriveTypeW(wpath.data());
                if (rc == DRIVE_NO_ROOT_DIR || rc == DRIVE_UNKNOWN)
                    return 'n';
                else
                    return 'd';
            } else {
                auto rc = GetFileAttributesW(wpath.data());
                if (rc == 0 || rc == INVALID_FILE_ATTRIBUTES)
                    return 'n';
                else if (rc == FILE_ATTRIBUTE_DIRECTORY)
                    return 'd';
                else
                    return 'f';
            }
        }

        inline std::wstring File::get_known_folder(int id) {
            std::wstring wpath(MAX_PATH, L'\0');
            auto rc = SHGetFolderPath(nullptr, id, nullptr, 0, &wpath[0]);
            if (rc != S_OK)
                throw std::system_error(rc, std::system_category());
            null_term(wpath);
            return wpath;
        }

        // File system update operations

        inline void File::mkdir(uint32_t flags) const {
            if (is_directory())
                return;
            bool mkdir_p = flags & recurse;
            auto wpath = native();
            SetLastError(0);
            auto rc = CreateDirectoryW(wpath.data(), nullptr);
            auto err = GetLastError();
            if (rc)
                return;
            if (err != ERROR_PATH_NOT_FOUND || ! mkdir_p)
                throw std::system_error(err, std::system_category(), path);
            File p = parent();
            if (p == *this)
                throw std::system_error(err, std::system_category(), path);
            p.mkdir(recurse);
            SetLastError(0);
            rc = CreateDirectoryW(wpath.data(), nullptr);
            err = GetLastError();
            if (! rc)
                throw std::system_error(err, std::system_category(), path);
        }

    #endif

}
