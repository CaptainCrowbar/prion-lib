#pragma once

#include "rs-core/common.hpp"
#include "unicorn/path.hpp"
#include <functional>
#include <type_traits>

#ifdef _XOPEN_SOURCE

    #include <dlfcn.h>

    #ifdef __APPLE__
        #define RS_DSO_APPLE_FLAG(name) name
    #else
        #define RS_DSO_APPLE_FLAG(name) 0
    #endif

    #define RS_DSO_POSIX_FLAG(name) name
    #define RS_DSO_WIN32_FLAG(name) 0

#else

    #define RS_DSO_APPLE_FLAG(name) 0
    #define RS_DSO_POSIX_FLAG(name) 0
    #define RS_DSO_WIN32_FLAG(name) name

#endif

RS_LDLIB(linux: dl);

namespace RS {

    class Dso {
    public:
        RS_MOVE_ONLY(Dso);
        #ifdef _XOPEN_SOURCE
            using flag_type = int;
            using handle_type = void*;
        #else
            using flag_type = uint32_t;
            using handle_type = HMODULE;
        #endif
        static constexpr flag_type lazy                = RS_DSO_POSIX_FLAG(RTLD_LAZY);                            // Relocations may be delayed
        static constexpr flag_type now                 = RS_DSO_POSIX_FLAG(RTLD_NOW);                             // Relocations are performed immediately
        static constexpr flag_type global              = RS_DSO_POSIX_FLAG(RTLD_GLOBAL);                          // Symbols are available when relocating other libraries
        static constexpr flag_type local               = RS_DSO_POSIX_FLAG(RTLD_LOCAL);                           // Symbols are not visible
        static constexpr flag_type noload              = RS_DSO_APPLE_FLAG(RTLD_NOLOAD);                          // Do not load, succeed only if already loaded
        static constexpr flag_type nodelete            = RS_DSO_APPLE_FLAG(RTLD_NODELETE);                        // Never unload
        static constexpr flag_type first               = RS_DSO_APPLE_FLAG(RTLD_FIRST);                           // Search only this library for symbols
        static constexpr flag_type ignore_authz        = RS_DSO_WIN32_FLAG(LOAD_IGNORE_CODE_AUTHZ_LEVEL);         // Do not check software restriction policies
        static constexpr flag_type datafile            = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_AS_DATAFILE);             // Map address space as data, do not execute
        static constexpr flag_type datafile_exclusive  = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_AS_DATAFILE_EXCLUSIVE);   // Map address space as data, with exclusive write access
        static constexpr flag_type image_resource      = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_AS_IMAGE_RESOURCE);       // Map address space as image, do not execute
        static constexpr flag_type search_application  = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_SEARCH_APPLICATION_DIR);  // Search application directory only
        static constexpr flag_type search_default      = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);     // Search application, system, and user-added directories
        static constexpr flag_type search_dll          = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR);     // Search DLL directory for its dependencies
        static constexpr flag_type search_system32     = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_SEARCH_SYSTEM32);         // Search system directory only
        static constexpr flag_type search_user         = RS_DSO_WIN32_FLAG(LOAD_LIBRARY_SEARCH_USER_DIRS);        // Search user-added directories only
        static constexpr flag_type altered_search      = RS_DSO_WIN32_FLAG(LOAD_WITH_ALTERED_SEARCH_PATH);        // Use alternative standard search path
        static constexpr flag_type default_flags = now | global;
        Dso() = default;
        explicit Dso(const Unicorn::Path& file, flag_type flags = default_flags) { load_library(file, flags, true); }
        ~Dso() = default;
        explicit operator bool() const noexcept { return bool(handle); }
        handle_type get() const noexcept { return handle.get(); }
        Unicorn::Path file() const { return libfile.value; }
        template <typename Sym> Sym symbol(const Ustring& name) { using SP = typename sym_type<Sym>::ptr; return Sym(SP(load_symbol(name, true))); }
        template <typename Sym> bool symbol(const Ustring& name, Sym& sym) noexcept;
        static Dso search(const Strings& names, flag_type flags = default_flags) { return do_search(names, flags); }
        template <typename... Args> static Dso search(Args... args) { Strings names; return do_search(names, args...); }
        static Dso self(flag_type flags = default_flags) { return Dso(Unicorn::Path(), flags); }
    private:
        #ifdef _XOPEN_SOURCE
            using symbol_type = void*;
        #else
            using symbol_type = FARPROC;
        #endif
        template <typename T, bool = std::is_function<T>::value> struct sym_type;
        template <typename T> struct sym_type<T, true> { using ptr = T*; };
        template <typename T> struct sym_type<T*, false> { using ptr = T*; };
        template <typename R, typename... Args> struct sym_type<std::function<R(Args...)>, false>: sym_type<R(Args...)> {};
        Resource<handle_type> handle;
        AutoMove<Unicorn::Path> libfile;
        bool load_library(Unicorn::Path file, flag_type flags, bool check);
        symbol_type load_symbol(const Ustring& name, bool check);
        static Dso do_search(const Strings& names, flag_type flags = default_flags);
        template <typename... Args> static Dso do_search(Strings& names, Uview next, Args... args) { names.emplace_back(next); return do_search(names, args...); }
    };

        template <typename Sym>
        bool Dso::symbol(const Ustring& name, Sym& sym) noexcept {
            using SP = typename sym_type<Sym>::ptr;
            auto symptr = load_symbol(name, false);
            if (! symptr)
                return false;
            sym = Sym(SP(symptr));
            return true;
        }

}

#undef RS_DSO_APPLE_FLAG
#undef RS_DSO_POSIX_FLAG
#undef RS_DSO_WIN32_FLAG
