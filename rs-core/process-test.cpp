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

    constexpr const char* files = "[LICENSE.txt,Makefile,README.md,build,dependencies.make,doc,rs-core,scripts]";

}

void test_core_process_shell_commands() {

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

void test_core_process_stream() {

    std::unique_ptr<StreamProcess> chan;
    Ustring s;
    Strings v;
    auto cs = Channel::state::closed;
    int st = -1;
    size_t n = 0;

    TRY(chan = std::make_unique<StreamProcess>("ls"));
    TRY(cs = chan->wait_for(100ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(n = chan->read_to(s));
    TEST_COMPARE(n, >, 0);
    TEST_MATCH(s, "([A-Za-z0-9.]+\\n)+");

    TRY(v = splitv(s));
    std::sort(v.begin(), v.end());
    TRY(s = to_str(v));
    TEST_EQUAL(s, files);

    TRY(cs = chan->wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::ready);
    TRY(n = chan->read_to(s));
    TEST_EQUAL(n, 0);
    TRY(cs = chan->wait_for(10ms));
    TEST_EQUAL(cs, Channel::state::closed);
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
    auto cs = Channel::state::closed;
    int st = -1;

    TRY(chan = std::make_unique<TextProcess>("ls"));

    for (int i = 1; i <= 10; ++i) {
        TRY(cs = chan->wait_for(10ms));
        if (cs == Channel::state::closed)
            break;
        if (cs == Channel::state::ready) {
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
