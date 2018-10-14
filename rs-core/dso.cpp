#include "rs-core/dso.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cerrno>
#include <system_error>

namespace RS {

    // Class Dso

    bool Dso::load_library(Unicorn::Path file, flag_type flags, bool check) {
        handle.reset();
        libfile.value = {};
        #ifdef _XOPEN_SOURCE
            const char* c_file = file.empty() ? nullptr : file.c_name();
            auto libptr = dlopen(c_file, flags);
            if (libptr) {
                handle = make_resource(libptr, dlclose);
            } else if (check) {
                Ustring msg = cstr(dlerror());
                if (! file.empty())
                    msg = quote(file.name()) + ": " + msg;
                throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), msg);
            }
        #else
            HMODULE libptr = nullptr;
            if (file.empty()) {
                libptr = GetModuleHandle(nullptr);
            } else {
                auto wname = file.os_name();
                libptr = LoadLibraryExW(wname.data(), nullptr, flags);
            }
            if (libptr) {
                handle = make_resource(libptr, FreeLibrary);
            } else if (check) {
                auto err = GetLastError();
                throw std::system_error(err, std::system_category(), file.name());
            }
        #endif
        std::swap(libfile.value, file);
        return bool(handle);
    }

    Dso::symbol_type Dso::load_symbol(const Ustring& name, bool check) {
        #ifdef _XOPEN_SOURCE
            auto sym = dlsym(handle.get(), name.data());
            if (check && ! sym) {
                Ustring msg = cstr(dlerror());
                throw std::system_error(std::make_error_code(std::errc::function_not_supported), quote(name) + ": " + msg);
            }
        #else
            auto sym = GetProcAddress(handle.get(), name.data());
            if (check && ! sym) {
                int err = GetLastError();
                throw std::system_error(err, std::system_category(), name);
            }
        #endif
        return sym;
    }

    Dso Dso::do_search(const Strings& names, flag_type flags) {
        #ifdef _XOPEN_SOURCE
            static constexpr const char* delimiters = "/";
            #if defined(__APPLE__)
                static constexpr const char* suffix = ".dylib";
            #elif defined(__CYGWIN__)
                static constexpr const char* suffix = ".dll";
            #else
                static constexpr const char* suffix = ".so";
            #endif
        #else
            static constexpr const char* delimiters = "/\\";
            static constexpr const char* suffix = ".dll";
        #endif
        Dso dso;
        for (auto& name: names) {
            if (dso.load_library(name, flags, false))
                return dso;
            if (name.find_first_of(delimiters) != npos)
                continue;
            Ustring libname = name + suffix;
            if (dso.load_library(libname, flags, false))
                return dso;
            #ifdef __CYGWIN__
                if (dso.load_library("cyg" + libname, flags, false))
                    return dso;
            #endif
            if (dso.load_library("lib" + libname, flags, false))
                return dso;
        }
        throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory), join(names, "|"));
    }

}
