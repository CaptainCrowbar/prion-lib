#pragma once

#include "rs-core/common.hpp"
#include "unicorn/path.hpp"
#include "unicorn/regex.hpp"
#include <ostream>
#include <vector>

RS_LDLIB(msvc: ole32 shell32);

namespace RS {

    // Standard directories

    RS_ENUM_CLASS(SystemPath, int, 100,
        apps,
        os
    );

    RS_ENUM_CLASS(UserPath, int, 200,
        apps,
        cache,
        desktop,
        documents,
        downloads,
        home,
        movies,
        music,
        pictures,
        settings
    );

    Unicorn::Path std_path(SystemPath id);
    Unicorn::Path std_path(UserPath id);

    // Search path class

    class PathList:
    public std::vector<Unicorn::Path> {
    public:
        using flag_type = uint32_t;
        #ifdef _XOPEN_SOURCE
            static constexpr char delimiter = ':';
        #else
            static constexpr char delimiter = ';';
        #endif
        static constexpr flag_type env        = 1;  // The string is the name of an environment variable
        static constexpr flag_type no_dups    = 2;  // Remove duplicate entries
        static constexpr flag_type only_dirs  = 4;  // Remove entries that do not exist or are not directories
        PathList() = default;
        explicit PathList(Uview text, flag_type flags = 0);
        bool contains(const Unicorn::Path& dir) const noexcept;
        void erase_all(const Unicorn::Path& dir);
        Unicorn::Path find(Uview name) { return find(name, Unicorn::Path::native_case); }
        Unicorn::Path find(Uview name, bool case_sensitive);
        Unicorn::Path find(const Unicorn::Regex& pattern);
        std::vector<Unicorn::Path> find_all(Uview name) { return find_all(name, Unicorn::Path::native_case); }
        std::vector<Unicorn::Path> find_all(Uview name, bool case_sensitive);
        std::vector<Unicorn::Path> find_all(const Unicorn::Regex& pattern);
        void prune(flag_type flags = no_dups | only_dirs);
        Ustring str() const;
        friend Ustring to_str(const PathList& pl) { return pl.str(); }
        friend std::ostream& operator<<(std::ostream& out, const PathList& pl) { return out << pl.str(); }
        static PathList path(flag_type flags = 0);
    };

}
