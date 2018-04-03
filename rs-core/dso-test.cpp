#include "rs-core/dso.hpp"
#include "rs-core/file.hpp"
#include "rs-core/unit-test.hpp"
#include <functional>
#include <system_error>
#include <utility>

using namespace RS;

void test_core_dso_dynamic_library() {

    Dso lib1, lib2, lib3, lib4;

    TEST_THROW(Dso("no-such-thing"), std::system_error);
    TRY(lib1 = Dso::self());

    File dir, file;

    #if defined(__APPLE__)
        dir = "/usr/lib";
        file = "libcurl.dylib";
    #elif defined(__CYGWIN__)
        dir = "/usr/bin";
        file = "cygcurl-4.dll";
    #elif defined(__linux__)
        #error Not implemented for Linux
    #elif defined(_XOPEN_SOURCE)
        #error Not implemented for generic Unix
    #else
        dir = "C:/Users/ross/Documents/Source/vcpkg/installed/x64-windows/bin";
        file = "libcurl.dll";
    #endif

    File path = dir / file;

    if (! path.exists()) {
        FAIL("Tests could not be run because the expected library was not found on this system: " + path.name());
        return;
    }

    TRY(lib1 = Dso(path));
    TRY(lib2 = Dso(file));
    TRY(lib3 = Dso::search("curl", "curl-4"));
    TRY(lib4 = Dso::search("curl", "curl-4", Dso::now));
    TEST(lib1);
    TEST(lib2);
    TEST(lib3);
    TEST(lib4);
    REQUIRE(lib1 || lib2 || lib3 || lib4);

    if (! lib1 && ! lib2 && ! lib3)
        lib1 = std::move(lib4);
    else if (! lib1 && ! lib2)
        lib1 = std::move(lib3);
    else if (! lib1)
        lib1 = std::move(lib2);

    using cv_ptr_type = const char* (*)();
    using cv_obj_type = std::function<const char*()>;
    cv_ptr_type cv_ptr = nullptr;
    cv_obj_type cv_obj = nullptr;
    Ustring text;
    static const Ustring cv_pattern = "^libcurl/\\d+\\.\\d+";

    TRY(cv_ptr = lib1.symbol<cv_ptr_type>("curl_version"));
    TEST(cv_ptr);
    if (cv_ptr) {
        TRY(text = cv_ptr());
        TEST_MATCH(text, cv_pattern);
    }

    TRY(cv_obj = lib1.symbol<cv_obj_type>("curl_version"));
    TEST(cv_obj);
    if (cv_obj) {
        TRY(text = cv_obj());
        TEST_MATCH(text, cv_pattern);
    }

    cv_ptr = nullptr;
    TEST(lib1.symbol("curl_version", cv_ptr));
    TEST(cv_ptr);
    if (cv_ptr) {
        TRY(text = cv_ptr());
        TEST_MATCH(text, cv_pattern);
    }

    cv_obj = nullptr;
    TEST(lib1.symbol("curl_version", cv_obj));
    TEST(cv_obj);
    if (cv_obj) {
        TRY(text = cv_obj());
        TEST_MATCH(text, cv_pattern);
    }

}
