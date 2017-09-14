#include "rs-core/file.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <cstdio>
#include <fstream>
#include <string>
#include <system_error>
#include <vector>

using namespace RS;
using namespace std::literals;

namespace {

    void check_file_names() {

        File f, f1, f2;

        TRY(f = "");                  TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), "");             TEST_EQUAL(f.base(), "");         TEST_EQUAL(f.ext(), "");
        TRY(f = "abc");               TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), "abc");          TEST_EQUAL(f.base(), "abc");      TEST_EQUAL(f.ext(), "");
        TRY(f = "abc.def");           TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), "abc.def");      TEST_EQUAL(f.base(), "abc");      TEST_EQUAL(f.ext(), ".def");
        TRY(f = "abc.def.ghi");       TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), "abc.def.ghi");  TEST_EQUAL(f.base(), "abc.def");  TEST_EQUAL(f.ext(), ".ghi");
        TRY(f = ".abc");              TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), ".abc");         TEST_EQUAL(f.base(), ".abc");     TEST_EQUAL(f.ext(), "");
        TRY(f = ".abc.def");          TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), ".abc.def");     TEST_EQUAL(f.base(), ".abc");     TEST_EQUAL(f.ext(), ".def");
        TRY(f = "abc/def");           TEST_EQUAL(f.parent().name(), "abc");       TEST_EQUAL(f.leaf().name(), "def");          TEST_EQUAL(f.base(), "def");      TEST_EQUAL(f.ext(), "");
        TRY(f = "abc/def.ghi");       TEST_EQUAL(f.parent().name(), "abc");       TEST_EQUAL(f.leaf().name(), "def.ghi");      TEST_EQUAL(f.base(), "def");      TEST_EQUAL(f.ext(), ".ghi");
        TRY(f = "abc/def.ghi.jkl");   TEST_EQUAL(f.parent().name(), "abc");       TEST_EQUAL(f.leaf().name(), "def.ghi.jkl");  TEST_EQUAL(f.base(), "def.ghi");  TEST_EQUAL(f.ext(), ".jkl");
        TRY(f = "abc/def/ghi");       TEST_EQUAL(f.parent().name(), "abc/def");   TEST_EQUAL(f.leaf().name(), "ghi");          TEST_EQUAL(f.base(), "ghi");      TEST_EQUAL(f.ext(), "");
        TRY(f = "abc/def/ghi.jkl");   TEST_EQUAL(f.parent().name(), "abc/def");   TEST_EQUAL(f.leaf().name(), "ghi.jkl");      TEST_EQUAL(f.base(), "ghi");      TEST_EQUAL(f.ext(), ".jkl");
        TRY(f = "/");                 TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), "");             TEST_EQUAL(f.base(), "");         TEST_EQUAL(f.ext(), "");
        TRY(f = "/abc");              TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), "abc");          TEST_EQUAL(f.base(), "abc");      TEST_EQUAL(f.ext(), "");
        TRY(f = "/abc.def");          TEST_EQUAL(f.parent().name(), "");          TEST_EQUAL(f.leaf().name(), "abc.def");      TEST_EQUAL(f.base(), "abc");      TEST_EQUAL(f.ext(), ".def");
        TRY(f = "/abc/def");          TEST_EQUAL(f.parent().name(), "/abc");      TEST_EQUAL(f.leaf().name(), "def");          TEST_EQUAL(f.base(), "def");      TEST_EQUAL(f.ext(), "");
        TRY(f = "/abc/def.ghi");      TEST_EQUAL(f.parent().name(), "/abc");      TEST_EQUAL(f.leaf().name(), "def.ghi");      TEST_EQUAL(f.base(), "def");      TEST_EQUAL(f.ext(), ".ghi");
        TRY(f = "/abc/def/ghi");      TEST_EQUAL(f.parent().name(), "/abc/def");  TEST_EQUAL(f.leaf().name(), "ghi");          TEST_EQUAL(f.base(), "ghi");      TEST_EQUAL(f.ext(), "");
        TRY(f = "/abc/def/ghi.jkl");  TEST_EQUAL(f.parent().name(), "/abc/def");  TEST_EQUAL(f.leaf().name(), "ghi.jkl");      TEST_EQUAL(f.base(), "ghi");      TEST_EQUAL(f.ext(), ".jkl");

        TRY(f = "");                    TEST(! f.is_absolute());
        TRY(f = "hello");               TEST(! f.is_absolute());
        TRY(f = "hello.txt");           TEST(! f.is_absolute());
        TRY(f = ".hello");              TEST(! f.is_absolute());
        TRY(f = "foo/hello.txt");       TEST(! f.is_absolute());
        TRY(f = "foo/bar/hello.txt");   TEST(! f.is_absolute());
        TRY(f = "/");                   TEST(f.is_absolute());
        TRY(f = "/foo");                TEST(f.is_absolute());
        TRY(f = "/foo/hello.txt");      TEST(f.is_absolute());
        TRY(f = "/foo/bar/hello.txt");  TEST(f.is_absolute());

        #ifndef _XOPEN_SOURCE
            TRY(f = "C:/");                   TEST(f.is_absolute());
            TRY(f = "C:/foo");                TEST(f.is_absolute());
            TRY(f = "C:/foo/hello.txt");      TEST(f.is_absolute());
            TRY(f = "C:/foo/bar/hello.txt");  TEST(f.is_absolute());
        #endif

        TRY(f1 = "");          TRY(f2 = "");          TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "");
        TRY(f1 = "");          TRY(f2 = "xyz");       TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "xyz");
        TRY(f1 = "");          TRY(f2 = "uvw/xyz");   TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "uvw/xyz");
        TRY(f1 = "");          TRY(f2 = "/");         TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/");
        TRY(f1 = "");          TRY(f2 = "/xyz");      TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/xyz");
        TRY(f1 = "");          TRY(f2 = "/uvw/xyz");  TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/uvw/xyz");
        TRY(f1 = "abc");       TRY(f2 = "");          TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "abc");
        TRY(f1 = "abc");       TRY(f2 = "xyz");       TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "abc/xyz");
        TRY(f1 = "abc");       TRY(f2 = "uvw/xyz");   TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "abc/uvw/xyz");
        TRY(f1 = "abc");       TRY(f2 = "/");         TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/");
        TRY(f1 = "abc");       TRY(f2 = "/xyz");      TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/xyz");
        TRY(f1 = "abc");       TRY(f2 = "/uvw/xyz");  TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/uvw/xyz");
        TRY(f1 = "abc/def");   TRY(f2 = "");          TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "abc/def");
        TRY(f1 = "abc/def");   TRY(f2 = "xyz");       TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "abc/def/xyz");
        TRY(f1 = "abc/def");   TRY(f2 = "uvw/xyz");   TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "abc/def/uvw/xyz");
        TRY(f1 = "abc/def");   TRY(f2 = "/");         TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/");
        TRY(f1 = "abc/def");   TRY(f2 = "/xyz");      TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/xyz");
        TRY(f1 = "abc/def");   TRY(f2 = "/uvw/xyz");  TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/uvw/xyz");
        TRY(f1 = "/");         TRY(f2 = "");          TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/");
        TRY(f1 = "/");         TRY(f2 = "xyz");       TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/xyz");
        TRY(f1 = "/");         TRY(f2 = "uvw/xyz");   TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/uvw/xyz");
        TRY(f1 = "/");         TRY(f2 = "/");         TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/");
        TRY(f1 = "/");         TRY(f2 = "/xyz");      TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/xyz");
        TRY(f1 = "/");         TRY(f2 = "/uvw/xyz");  TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/uvw/xyz");
        TRY(f1 = "/abc");      TRY(f2 = "");          TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/abc");
        TRY(f1 = "/abc");      TRY(f2 = "xyz");       TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/abc/xyz");
        TRY(f1 = "/abc");      TRY(f2 = "uvw/xyz");   TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/abc/uvw/xyz");
        TRY(f1 = "/abc");      TRY(f2 = "/");         TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/");
        TRY(f1 = "/abc");      TRY(f2 = "/xyz");      TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/xyz");
        TRY(f1 = "/abc");      TRY(f2 = "/uvw/xyz");  TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/uvw/xyz");
        TRY(f1 = "/abc/def");  TRY(f2 = "");          TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/abc/def");
        TRY(f1 = "/abc/def");  TRY(f2 = "xyz");       TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/abc/def/xyz");
        TRY(f1 = "/abc/def");  TRY(f2 = "uvw/xyz");   TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/abc/def/uvw/xyz");
        TRY(f1 = "/abc/def");  TRY(f2 = "/");         TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/");
        TRY(f1 = "/abc/def");  TRY(f2 = "/xyz");      TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/xyz");
        TRY(f1 = "/abc/def");  TRY(f2 = "/uvw/xyz");  TRY(f = f1 / f2);  TEST_EQUAL(f.name(), "/uvw/xyz");

        #ifdef __CYGWIN__

            U8string s;
            std::wstring ws;

            TRY(f = "");                              TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "");
            TRY(f = ".");                             TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, ".");
            TRY(f = "Makefile");                      TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "Makefile");
            TRY(f = "/usr/bin/bash");                 TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "C:\\cygwin64\\bin\\bash.exe");
            TRY(f = "/cygdrive/c");                   TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "C:\\");
            TRY(f = "/cygdrive/c/Windows/System32");  TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "C:\\Windows\\System32");

        #elif defined(_XOPEN_SOURCE)

            U8string s;

            TRY(f = "");               TRY(s = f.native());  TEST_EQUAL(s, "");
            TRY(f = ".");              TRY(s = f.native());  TEST_EQUAL(s, ".");
            TRY(f = "Makefile");       TRY(s = f.native());  TEST_EQUAL(s, "Makefile");
            TRY(f = "/usr/bin/bash");  TRY(s = f.native());  TEST_EQUAL(s, "/usr/bin/bash");

        #else

            U8string s;
            std::wstring ws;

            TRY(f = "");                     TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "");
            TRY(f = ".");                    TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, ".");
            TRY(f = "Makefile");             TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "Makefile");
            TRY(f = "C:/Windows/System32");  TRY(ws = f.native());  TRY(s = uconv<U8string>(ws));  TEST_EQUAL(s, "C:\\Windows\\System32");

        #endif

    }

    void check_file_system_queries() {

        File f;
        std::vector<File> v;
        U8string s;
        uint64_t n = 0;

        TRY(f = "");              TEST(! f.exists());  TEST(! f.is_directory());
        TRY(f = ".");             TEST(f.exists());    TEST(f.is_directory());
        TRY(f = "Makefile");      TEST(f.exists());    TEST(! f.is_directory());
        TRY(f = "no such file");  TEST(! f.exists());  TEST(! f.is_directory());

        #ifdef _XOPEN_SOURCE
            TRY(f = "/");  TEST(f.exists());  TEST(f.is_directory());
        #else
            TRY(f = "C:/");  TEST(f.exists());  TEST(f.is_directory());
        #endif

        #ifdef __APPLE__
            TRY(f = "");      TEST(! f.is_symlink());
            TRY(f = ".");     TEST(! f.is_symlink());
            TRY(f = "/");     TEST(! f.is_symlink());
            TRY(f = "/bin");  TEST(! f.is_symlink());
            TRY(f = "/etc");  TEST(f.is_symlink());
        #endif

        #ifdef __CYGWIN__
            TRY(f = "");                  TEST(! f.is_symlink());
            TRY(f = ".");                 TEST(! f.is_symlink());
            TRY(f = "/");                 TEST(! f.is_symlink());
            TRY(f = "/usr/bin/cc");       TEST(f.is_symlink());
            TRY(f = "/usr/bin/gcc.exe");  TEST(! f.is_symlink());
        #endif

        TRY(f = "");          TRY(n = f.size());  TEST_EQUAL(n, 0);
        TRY(f = "Makefile");  TRY(n = f.size());  TEST_COMPARE(n, >, 9000);

        TRY(v = File("").list());   std::sort(v.begin(), v.end());  TRY(s = to_str(v));  TEST_EQUAL(s, "[]");
        TRY(v = File(".").list());  std::sort(v.begin(), v.end());  TRY(s = to_str(v));  TEST_MATCH(s, "^\\[(.+,)?Makefile,README\\.md,build,.+\\]$");

        #ifdef _XOPEN_SOURCE
            TRY(v = File("/").list());  std::sort(v.begin(), v.end());  TRY(s = to_str(v));  TEST_MATCH(s, "^\\[.+,/bin,.+,/etc,.+,/usr,.+\\]$");
        #else
            TRY(v = File("C:/").list());  std::sort(v.begin(), v.end());  TRY(s = to_str(v));  TEST_MATCH(s, "^\\[.+,C:/Program Files,C:/Program Files \\(x86\\),.+,C:/Windows,.+\\]$");
        #endif

    }

    void check_file_system_update() {

        const File d1 = "__test__";
        const File d2 = "__test__/foo";
        const File d3 = "__test__/foo/bar";
        const File f1 = "__test__/alpha";
        const File f2 = "__test__/bravo";

        ScopeExit guard([=] {
            ::remove(f1.c_name());
            ::remove(f2.c_name());
            ::remove(d3.c_name());
            ::remove(d2.c_name());
            ::remove(d1.c_name());
        });

        TEST(! d1.exists());
        TEST(! d2.exists());
        TEST(! d3.exists());
        TEST(! f1.exists());
        TEST(! f2.exists());

        TRY(d1.mkdir());
        TEST(d1.exists());
        TEST(d1.is_directory());

        TEST_THROW(d3.mkdir(), std::system_error);
        TEST(! d2.exists());
        TEST(! d3.exists());

        TRY(d3.mkdir(File::recurse));
        TEST(d2.exists());
        TEST(d2.is_directory());
        TEST(d3.exists());
        TEST(d3.is_directory());

        {
            std::ofstream out(f1.name());
            out << "Hello\n";
        }

        TEST(f1.exists());
        TRY(f1.move_to(f2));
        TEST(! f1.exists());
        TEST(f2.exists());

        TRY(d3.remove());
        TEST(d1.exists());
        TEST(d2.exists());
        TEST(! d3.exists());

        TEST_THROW(d1.remove(), std::system_error);
        TEST(d1.exists());
        TEST(d2.exists());
        TEST(! d3.exists());
        TEST(f2.exists());

        TRY(d1.remove(File::recurse));
        TEST(! d1.exists());
        TEST(! d2.exists());
        TEST(! d3.exists());
        TEST(! f1.exists());
        TEST(! f2.exists());

    }

    void check_file_io() {

        const File makefile = "Makefile";
        const File testfile = "__test__";
        const File nofile = "__no_such_file__";

        std::string s;
        ScopeExit guard([=] { ::remove(testfile.c_name()); });

        TRY(s = makefile.load());
        TEST_EQUAL(s.substr(0, 25), "# Grand Unified Makefile\n");
        TRY(s = makefile.load(100'000));
        TEST_COMPARE(s.size(), >, 9000);
        TEST_EQUAL(s.substr(0, 25), "# Grand Unified Makefile\n");
        TRY(s = makefile.load(10));
        TEST_EQUAL(s.size(), 10);
        TEST_EQUAL(s.substr(0, 25), "# Grand Un");

        TRY(testfile.save("Hello world\n"s));
        TRY(s = testfile.load());
        TEST_EQUAL(s, "Hello world\n");
        TRY(testfile.save("Goodbye\n"s, File::append));
        TRY(s = testfile.load());
        TEST_EQUAL(s, "Hello world\nGoodbye\n");

        TRY(s = nofile.load());
        TEST(s.empty());
        TEST_THROW(nofile.load(npos, File::require), std::system_error);

    }

    void check_file_standard_locations() {

        File cd, f;

        TRY(cd = File::cwd());
        TEST_MATCH(cd.name(), "/Code/core-lib$");

        TRY(f = "..");
        TRY(f.set_cwd());
        TRY(f = File::cwd());
        TEST_MATCH(f.name(), "/Code$");
        TRY(cd.set_cwd());

        TRY(f = File::user_home());
        #ifdef __APPLE__
            TEST_MATCH(f.name(), "^/Users/[\\w.]+$");
        #elif defined(_XOPEN_SOURCE)
            TEST_MATCH(f.name(), "^/home/[\\w.]+$");
        #else
            TEST_MATCH(f.name(), "^C:/Users/[\\w.]+$");
        #endif

        TRY(f = File::user_documents());
        TEST_MATCH(f.name(), "/Documents$");

        TRY(f = File::user_settings());
        #ifdef __APPLE__
            TEST_MATCH(f.name(), "/Library/Application Support$");
        #elif defined(_XOPEN_SOURCE)
            TEST_MATCH(f.name(), "/.config$");
        #else
            TEST_MATCH(f.name(), "/AppData/Roaming$");
        #endif

    }

}

TEST_MODULE(core, file) {

    check_file_names();
    check_file_system_queries();
    check_file_system_update();
    check_file_io();
    check_file_standard_locations();

}
