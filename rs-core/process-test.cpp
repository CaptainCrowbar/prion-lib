#include "rs-core/process.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <chrono>
#include <memory>
#include <set>

using namespace RS;
using namespace std::chrono;
using namespace std::literals;

namespace {

    constexpr const char* files = "[LICENSE.txt,Makefile,README.md,build,dependencies.make,doc,graphics,rs-core,samples,scripts]";

}

void test_core_process_stream() {

    std::unique_ptr<StreamProcess> chan;
    Ustring s;
    Strings v;
    int st = -1;
    size_t n = 0;

    TRY(chan = std::make_unique<StreamProcess>("ls"));
    TEST(chan->wait_for(100ms));
    TEST(! chan->is_closed());
    TRY(n = chan->read_to(s));
    TEST_COMPARE(n, >, 0);
    TEST_MATCH(s, "([A-Za-z0-9.]+\\n)+");

    TRY(v = splitv(s));
    std::sort(v.begin(), v.end());
    TRY(s = to_str(v));
    TEST_EQUAL(s, files);

    TEST(chan->wait_for(10ms));
    TEST(! chan->is_closed());
    TRY(n = chan->read_to(s));
    TEST_EQUAL(n, 0);
    TEST(chan->wait_for(10ms));
    TEST(chan->is_closed());
    TRY(st = chan->status());
    TEST_EQUAL(st, 0);

    chan.reset();
    TRY(chan = std::make_unique<StreamProcess>("ls"));
    TRY(s = chan->read_all());
    TRY(v = splitv(s));
    std::sort(v.begin(), v.end());
    TRY(s = to_str(v));
    TEST_EQUAL(s, files);

}

void test_core_process_text() {

    std::unique_ptr<TextProcess> chan;
    Ustring s;
    Strings v;
    int st = -1;
    bool rc = false;

    TRY(chan = std::make_unique<TextProcess>("ls"));

    for (int i = 1; i <= 10; ++i) {
        TRY(rc = bool(chan->wait_for(10ms)));
        if (chan->is_closed())
            break;
        if (rc) {
            TEST(chan->read(s));
            v.push_back(s);
        }
    }

    std::sort(v.begin(), v.end());
    TRY(s = to_str(v));
    TEST_EQUAL(s, files);

    TRY(chan->close());
    TRY(st = chan->status());
    TEST_EQUAL(st, 0);

    chan.reset();
    TRY(chan = std::make_unique<TextProcess>("ls"));
    TRY(s = chan->read_all());
    TRY(v = splitv(s));
    std::sort(v.begin(), v.end());
    TRY(s = to_str(v));
    TEST_EQUAL(s, files);

}

void test_core_process_shell_command() {

    Ustring s;
    std::set<Ustring> ss;

    #ifdef _XOPEN_SOURCE
        TRY(s = shell("ls"));
    #else
        TRY(s = shell("dir /b"));
    #endif

    TEST(! s.empty());
    TRY(split(s, overwrite(ss), "\r\n"));
    TEST(! ss.empty());
    TEST(ss.count("Makefile"));
    TEST(ss.count("build"));

}

void test_core_process_preview_file() {

    // preview_file("samples/flowchart.png");
    // preview_file("samples/piper.pdf");
    // preview_file("samples/roget.txt");
    // preview_file("samples/tesseract.gif");

}
