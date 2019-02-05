#include "rs-core/system.hpp"
#include "unicorn/utf.hpp"
#include <cerrno>
#include <cstdlib>
#include <string>
#include <system_error>

#ifdef _XOPEN_SOURCE
    #include <sys/stat.h>
    #include <sys/wait.h>
    #ifdef __CYGWIN__
        #include <sys/cygwin.h>
    #endif
#else
    #include <windows.h>
#endif

using namespace RS::Unicorn;

namespace RS {

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
