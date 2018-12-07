#include "rs-core/file-system.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <cstdlib>
#include <stdexcept>
#include <system_error>
#include <unordered_set>
#include <utility>

#ifdef _XOPEN_SOURCE
    #include <pwd.h>
#else
    #include <windows.h>
    #include <shlobj.h>
#endif

using namespace RS::Unicorn;

namespace RS {

    // Standard directories

    namespace {

        #ifdef _XOPEN_SOURCE

            Path get_user_home() {
                auto envptr = std::getenv("HOME");
                if (envptr && *envptr)
                    return Path(envptr);
                std::string workbuf(1024, '\0');
                passwd pwdbuf;
                passwd* pwdptr = nullptr;
                for (;;) {
                    int rc = getpwuid_r(geteuid(), &pwdbuf, workbuf.data(), workbuf.size(), &pwdptr);
                    if (rc != ERANGE)
                        break;
                    workbuf.resize(2 * workbuf.size());
                }
                if (pwdptr && pwdptr->pw_dir && *pwdptr->pw_dir)
                    return Path(pwdptr->pw_dir);
                envptr = std::getenv("USER");
                std::string user;
                if (envptr && *envptr)
                    user = envptr;
                else if (pwdptr && pwdptr->pw_name && *pwdptr->pw_name)
                    user = pwdptr->pw_name;
                else
                    throw std::system_error(std::make_error_code(std::errc::no_such_file_or_directory));
                #ifdef __APPLE__
                    static const Path dir("/Users");
                #else
                    static const Path dir("/home");
                #endif
                return dir / user;
            }

            #ifndef __APPLE__

                Path get_xdg_path(const char* varname, const char* fallback) {
                    auto env = std::getenv(varname);
                    return env && *env ? Path(env) : get_user_home() / fallback;
                }

            #endif

        #else

            Path get_known_folder(REFKNOWNFOLDERID id) {
                wchar_t* wpath = nullptr;
                auto rc = SHGetKnownFolderPath(id, KF_FLAG_DONT_VERIFY, nullptr, &wpath);
                if (rc != S_OK)
                    throw std::system_error(rc, std::system_category());
                auto guard = scope_exit([&] { CoTaskMemFree(wpath); });
                return Path(wpath);
            }

        #endif

    }

    Path std_path(SystemPath id) {
        switch (id) {
            #if defined(__APPLE__)
                case SystemPath::apps:  return "/Applications";
                case SystemPath::os:    return "/";
            #elif defined(_XOPEN_SOURCE)
                case SystemPath::apps:  return "/usr/local/bin";
                case SystemPath::os:    return "/";
            #else
                case SystemPath::apps:  return get_known_folder(FOLDERID_ProgramFiles);
                case SystemPath::os:    return get_known_folder(FOLDERID_Windows);
            #endif
            default:
                throw std::invalid_argument("Invalid system path selector");
        }
    }

    Path std_path(UserPath id) {
        switch (id) {
            #if defined(__APPLE__)
                case UserPath::apps:       return get_user_home() / "Applications";
                case UserPath::cache:      return get_user_home() / "Library/Caches";
                case UserPath::desktop:    return get_user_home() / "Desktop";
                case UserPath::documents:  return get_user_home() / "Documents";
                case UserPath::downloads:  return get_user_home() / "Downloads";
                case UserPath::home:       return get_user_home();
                case UserPath::movies:     return get_user_home() / "Movies";
                case UserPath::music:      return get_user_home() / "Music";
                case UserPath::pictures:   return get_user_home() / "Pictures";
                case UserPath::settings:   return get_user_home() / "Library/Application Support";
            #elif defined(_XOPEN_SOURCE)
                case UserPath::apps:       return get_user_home() / "bin";
                case UserPath::cache:      return get_xdg_path("XDG_CACHE_HOME", ".cache");
                case UserPath::desktop:    return get_xdg_path("XDG_DESKTOP_DIR", "Desktop");
                case UserPath::documents:  return get_xdg_path("XDG_DOCUMENTS_DIR", "Documents");
                case UserPath::downloads:  return get_xdg_path("XDG_DOWNLOAD_DIR", "Downloads");
                case UserPath::home:       return get_user_home();
                case UserPath::movies:     return get_xdg_path("XDG_VIDEOS_DIR", "Videos");
                case UserPath::music:      return get_xdg_path("XDG_MUSIC_DIR", "Music");
                case UserPath::pictures:   return get_xdg_path("XDG_PICTURES_DIR", "Pictures");
                case UserPath::settings:   return get_xdg_path("XDG_CONFIG_HOME", ".config");
            #else
                case UserPath::apps:       return get_known_folder(FOLDERID_UserProgramFiles);
                case UserPath::cache:      return get_known_folder(FOLDERID_LocalAppData);
                case UserPath::desktop:    return get_known_folder(FOLDERID_Desktop);
                case UserPath::documents:  return get_known_folder(FOLDERID_Documents);
                case UserPath::downloads:  return get_known_folder(FOLDERID_Downloads);
                case UserPath::home:       return get_known_folder(FOLDERID_Profile);
                case UserPath::movies:     return get_known_folder(FOLDERID_Videos);
                case UserPath::music:      return get_known_folder(FOLDERID_Music);
                case UserPath::pictures:   return get_known_folder(FOLDERID_Pictures);
                case UserPath::settings:   return get_known_folder(FOLDERID_RoamingAppData);
            #endif
            default:
                throw std::invalid_argument("Invalid user path selector");
        }
    }

    // Search path class

    namespace {

        using PathSet = std::unordered_set<Path>;

        void search_by_name(const std::vector<Path>& dirs, std::vector<Path>& results, Uview name, size_t limit) {
            for (auto& dir: dirs) {
                Path file = dir / name;
                if (file.exists()) {
                    results.push_back(file);
                    if (results.size() == limit)
                        break;
                }
            }
        }

        void search_by_name_icase(const std::vector<Path>& dirs, std::vector<Path>& results, Uview name, size_t limit) {
            auto cfname = str_casefold(Ustring(name));
            for (auto& dir: dirs) {
                for (auto& file: dir.directory()) {
                    auto leaf = file.split_path().second.name();
                    if (str_casefold(leaf) == cfname) {
                        results.push_back(file);
                        if (results.size() == limit)
                            return;
                    }
                }
            }
        }

        void search_by_regex(const std::vector<Path>& dirs, std::vector<Path>& results, const Regex& pattern, size_t limit) {
            for (auto& dir: dirs) {
                for (auto& file: dir.directory()) {
                    auto leaf = file.split_path().second.name();
                    if (pattern(leaf)) {
                        results.push_back(file);
                        if (results.size() == limit)
                            return;
                    }
                }
            }
        }

        void checked_append(PathList& list, PathSet& seen, const Path& file, PathList::flag_type flags) {
            if (file.empty()) return;
            if ((flags & PathList::no_dups) && ! seen.insert(file).second) return;
            if ((flags & PathList::only_dirs) && ! file.is_directory()) return;
            list.push_back(file);
        }

    }

    PathList::PathList(Uview text, flag_type flags) {
        Ustring utext(text);
        if (flags & env)
            utext = cstr(std::getenv(utext.data()));
        PathSet seen;
        size_t i = 0, size = utext.size();
        while (i < size) {
            size_t j = utext.find(delimiter, i);
            if (j == npos)
                j = size;
            auto view = make_view(utext, i, j - i);
            checked_append(*this, seen, view, flags);
            i = j + 1;
        }
    }

    bool PathList::contains(const Path& dir) const noexcept {
        return std::find(begin(), end(), dir) != end();
    }

    void PathList::erase_all(const Path& dir) {
        erase(std::remove(begin(), end(), dir), end());
    }

    Path PathList::find(Uview name, bool case_sensitive) {
        std::vector<Path> results;
        if (case_sensitive)
            search_by_name(*this, results, name, 1);
        else
            search_by_name_icase(*this, results, name, 1);
        return results.empty() ? Path() : results.front();
    }

    Path PathList::find(const Regex& pattern) {
        std::vector<Path> results;
        search_by_regex(*this, results, pattern, 1);
        return results.empty() ? Path() : results.front();
    }

    std::vector<Path> PathList::find_all(Uview name, bool case_sensitive) {
        std::vector<Path> results;
        if (case_sensitive)
            search_by_name(*this, results, name, npos);
        else
            search_by_name_icase(*this, results, name, npos);
        return results;
    }

    std::vector<Path> PathList::find_all(const Regex& pattern) {
        std::vector<Path> results;
        search_by_regex(*this, results, pattern, npos);
        return results;
    }

    void PathList::prune(flag_type flags) {
        PathList list;
        PathSet seen;
        for (auto& file: *this)
            checked_append(list, seen, file, flags);
        *this = std::move(list);
    }

    Ustring PathList::str() const {
        if (empty())
            return {};
        Ustring text;
        for (auto& file: *this)
            text += file.name() + delimiter;
        text.pop_back();
        return text;
    }

    PathList PathList::path(flag_type flags) {
        return PathList("PATH", env | flags);
    }

}
