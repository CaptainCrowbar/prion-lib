#include "rs-core/file-system.hpp"
#include "rs-core/unit-test.hpp"
#include "unicorn/path.hpp"
#include <algorithm>
#include <vector>

using namespace RS;
using namespace RS::Unicorn;

namespace {

    Ustring fixup(Ustring s) {
        #ifndef _XOPEN_SOURCE
            std::replace(s.begin(), s.end(), ':', ';');
        #endif
        return s;
    }

}

void test_core_file_system_standard_directories() {

    Path system_apps, system_os, user_apps, user_cache, user_desktop, user_documents, user_downloads, user_home, user_movies, user_music, user_pictures, user_settings;

    TRY(system_apps = std_path(SystemPath::apps));
    TRY(system_os = std_path(SystemPath::os));
    TRY(user_apps = std_path(UserPath::apps));
    TRY(user_cache = std_path(UserPath::cache));
    TRY(user_desktop = std_path(UserPath::desktop));
    TRY(user_documents = std_path(UserPath::documents));
    TRY(user_downloads = std_path(UserPath::downloads));
    TRY(user_home = std_path(UserPath::home));
    TRY(user_movies = std_path(UserPath::movies));
    TRY(user_music = std_path(UserPath::music));
    TRY(user_pictures = std_path(UserPath::pictures));
    TRY(user_settings = std_path(UserPath::settings));

    #if defined(__APPLE__)

        TEST_EQUAL(system_apps.name(),     "/Applications");
        TEST_EQUAL(system_os.name(),       "/");
        TEST_MATCH(user_apps.name(),       "^/Users/[^/]+/Applications$");
        TEST_MATCH(user_cache.name(),      "^/Users/[^/]+/Library/Caches$");
        TEST_MATCH(user_desktop.name(),    "^/Users/[^/]+/Desktop$");
        TEST_MATCH(user_documents.name(),  "^/Users/[^/]+/Documents$");
        TEST_MATCH(user_downloads.name(),  "^/Users/[^/]+/Downloads$");
        TEST_MATCH(user_home.name(),       "^/Users/[^/]+$");
        TEST_MATCH(user_movies.name(),     "^/Users/[^/]+/Movies$");
        TEST_MATCH(user_music.name(),      "^/Users/[^/]+/Music$");
        TEST_MATCH(user_pictures.name(),   "^/Users/[^/]+/Pictures$");
        TEST_MATCH(user_settings.name(),   "^/Users/[^/]+/Library/Application Support$");

    #elif defined(_XOPEN_SOURCE)

        TEST_EQUAL(system_apps.name(),     "/usr/local/bin");
        TEST_EQUAL(system_os.name(),       "/");
        TEST_MATCH(user_apps.name(),       "^/home/[^/]+/bin$");
        TEST_MATCH(user_cache.name(),      "^/home/[^/]+/.cache$");
        TEST_MATCH(user_desktop.name(),    "^/home/[^/]+/Desktop$");
        TEST_MATCH(user_documents.name(),  "^/home/[^/]+/Documents$");
        TEST_MATCH(user_downloads.name(),  "^/home/[^/]+/Downloads$");
        TEST_MATCH(user_home.name(),       "^/home/[^/]+$");
        TEST_MATCH(user_movies.name(),     "^/home/[^/]+/Videos$");
        TEST_MATCH(user_music.name(),      "^/home/[^/]+/Music$");
        TEST_MATCH(user_pictures.name(),   "^/home/[^/]+/Pictures$");
        TEST_MATCH(user_settings.name(),   "^/home/[^/]+/.config$");

    #else

        TEST_EQUAL(system_apps.name(),     R"(C:\Program Files)");
        TEST_EQUAL(system_os.name(),       R"(C:\Windows)");
        TEST_MATCH(user_apps.name(),       R"(^C:\\Users\\[^/\\]+\\AppData\\Local\\Programs$)");
        TEST_MATCH(user_cache.name(),      R"(^C:\\Users\\[^/\\]+\\AppData\\Local$)");
        TEST_MATCH(user_desktop.name(),    R"(^C:\\Users\\[^/\\]+\\Desktop$)");
        TEST_MATCH(user_documents.name(),  R"(^C:\\Users\\[^/\\]+\\Documents$)");
        TEST_MATCH(user_downloads.name(),  R"(^C:\\Users\\[^/\\]+\\Downloads$)");
        TEST_MATCH(user_home.name(),       R"(^C:\\Users\\[^/\\]+$)");
        TEST_MATCH(user_movies.name(),     R"(^C:\\Users\\[^/\\]+\\Videos$)");
        TEST_MATCH(user_music.name(),      R"(^C:\\Users\\[^/\\]+\\Music$)");
        TEST_MATCH(user_pictures.name(),   R"(^C:\\Users\\[^/\\]+\\Pictures$)");
        TEST_MATCH(user_settings.name(),   R"(^C:\\Users\\[^/\\]+\\AppData\\Roaming$)");

    #endif

}

void test_core_file_system_search_path() {

    PathList search;
    Path file;
    std::vector<Path> files;
    Ustring str;

    TEST(search.empty());
    TRY(str = search.str());
    TEST_EQUAL(str, "");

    #ifdef _XOPEN_SOURCE

        TRY(search = PathList("/aaa/bbb/ccc:/ddd/eee/fff:/ggg/hhh/iii:"));
        TEST_EQUAL(search.size(), 3);
        TEST_EQUAL(search.at(0), Path("/aaa/bbb/ccc"));
        TEST_EQUAL(search.at(1), Path("/ddd/eee/fff"));
        TEST_EQUAL(search.at(2), Path("/ggg/hhh/iii"));
        TRY(str = search.str());
        TEST_EQUAL(str, "/aaa/bbb/ccc:/ddd/eee/fff:/ggg/hhh/iii");
        TEST(search.contains("/aaa/bbb/ccc"));
        TEST(search.contains("/ddd/eee/fff"));
        TEST(search.contains("/ggg/hhh/iii"));
        TEST(! search.contains("/jjj/kkk/lll"));

    #else

        TRY(search = PathList("C:\\aaa\\bbb\\ccc;C:\\ddd\\eee\\fff;C:\\ggg\\hhh\\iii;"));
        TEST_EQUAL(search.size(), 3);
        TEST_EQUAL(search.at(0), Path("C:\\aaa\\bbb\\ccc"));
        TEST_EQUAL(search.at(1), Path("C:\\ddd\\eee\\fff"));
        TEST_EQUAL(search.at(2), Path("C:\\ggg\\hhh\\iii"));
        TRY(str = search.str());
        TEST_EQUAL(str, "C:\\aaa\\bbb\\ccc;C:\\ddd\\eee\\fff;C:\\ggg\\hhh\\iii");
        TEST(search.contains("C:\\aaa\\bbb\\ccc"));
        TEST(search.contains("C:\\ddd\\eee\\fff"));
        TEST(search.contains("C:\\ggg\\hhh\\iii"));
        TEST(! search.contains("C:\\jjj\\kkk\\lll"));

    #endif

    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core")));
    TEST_EQUAL(search.size(), 6);
    TEST_EQUAL(search.at(0), Path("foo"));
    TEST_EQUAL(search.at(1), Path("build"));
    TEST_EQUAL(search.at(2), Path("rs-core"));
    TEST_EQUAL(search.at(3), Path("foo"));
    TEST_EQUAL(search.at(4), Path("build"));
    TEST_EQUAL(search.at(5), Path("rs-core"));
    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core"), PathList::no_dups));
    TEST_EQUAL(search.size(), 3);
    TEST_EQUAL(search.at(0), Path("foo"));
    TEST_EQUAL(search.at(1), Path("build"));
    TEST_EQUAL(search.at(2), Path("rs-core"));
    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core"), PathList::only_dirs));
    TEST_EQUAL(search.size(), 4);
    TEST_EQUAL(search.at(0), Path("build"));
    TEST_EQUAL(search.at(1), Path("rs-core"));
    TEST_EQUAL(search.at(2), Path("build"));
    TEST_EQUAL(search.at(3), Path("rs-core"));
    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core"), PathList::no_dups | PathList:: only_dirs));
    TEST_EQUAL(search.size(), 2);
    TEST_EQUAL(search.at(0), Path("build"));
    TEST_EQUAL(search.at(1), Path("rs-core"));

    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core")));
    TRY(search.erase_all("foo"));
    TEST_EQUAL(search.size(), 4);
    TEST_EQUAL(search.at(0), Path("build"));
    TEST_EQUAL(search.at(1), Path("rs-core"));
    TEST_EQUAL(search.at(2), Path("build"));
    TEST_EQUAL(search.at(3), Path("rs-core"));

    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core")));
    TRY(search.prune(PathList::no_dups));
    TEST_EQUAL(search.size(), 3);
    TEST_EQUAL(search.at(0), Path("foo"));
    TEST_EQUAL(search.at(1), Path("build"));
    TEST_EQUAL(search.at(2), Path("rs-core"));
    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core")));
    TRY(search.prune(PathList::only_dirs));
    TEST_EQUAL(search.size(), 4);
    TEST_EQUAL(search.at(0), Path("build"));
    TEST_EQUAL(search.at(1), Path("rs-core"));
    TEST_EQUAL(search.at(2), Path("build"));
    TEST_EQUAL(search.at(3), Path("rs-core"));
    TRY(search = PathList(fixup("foo:build:rs-core:foo:build:rs-core")));
    TRY(search.prune(PathList::no_dups | PathList:: only_dirs));
    TEST_EQUAL(search.size(), 2);
    TEST_EQUAL(search.at(0), Path("build"));
    TEST_EQUAL(search.at(1), Path("rs-core"));

    TRY(search = PathList::path(PathList::no_dups | PathList::only_dirs));
    TEST(! search.empty());

    #ifdef _XOPEN_SOURCE

        #ifdef __CYGWIN__
            Ustring ls_rel = "ls.exe";
            Ustring ls_abs = "/usr/bin/ls.exe";
        #else
            Ustring ls_rel = "ls";
            Ustring ls_abs = "/bin/ls";
        #endif

        TEST(search.contains("/usr/bin"));
        TRY(file = search.find(ls_rel));
        TEST_EQUAL(file, Path(ls_abs));
        TRY(files = search.find_all(ls_rel));
        TEST_EQUAL(files.size(), 1);
        TEST_EQUAL(files.at(0), Path(ls_abs));

    #else

        TEST(search.contains("C:\\Windows"));
        TEST(search.contains("C:\\Windows\\system32"));
        TRY(file = search.find("notepad.exe"));
        TEST_EQUAL(file, Path("C:\\Windows\\system32\\notepad.exe"));
        TRY(files = search.find_all("notepad.exe"));
        TEST_EQUAL(files.size(), 2);
        TEST_EQUAL(files.at(0), Path("C:\\Windows\\system32\\notepad.exe"));
        TEST_EQUAL(files.at(1), Path("C:\\Windows\\notepad.exe"));

    #endif

    TRY(file = search.find("no such thing"));
    TEST(file.empty());
    TRY(files = search.find_all("no such thing"));
    TEST(files.empty());

}
