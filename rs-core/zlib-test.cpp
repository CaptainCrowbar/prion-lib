#include "rs-core/zlib.hpp"
#include "rs-core/random.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include "unicorn/path.hpp"
#include <algorithm>
#include <cstdlib>
#include <string>
#include <system_error>

using namespace RS;
using namespace RS::Unicorn;

void test_core_zlib_compression() {

    std::string src, comp, dst;
    Xoshiro rng(42);

    for (size_t i = 1; i <= 10; ++i) {
        size_t bytes = 1000 * i;
        comp.clear();
        dst.clear();
        TRY(src = lorem_ipsum(rng(), bytes));
        TRY(comp = Zlib::compress(src));
        TEST_COMPARE(comp.size(), <, src.size());
        TRY(dst = Zlib::uncompress(comp));
        TEST_EQUAL(src, dst);
    }

}

void test_core_zlib_io() {

    Gzio io;
    Path file = "__gzio_test__", no_file = "__no_such_file__";
    Ustring text;
    Strings vec;
    ptrdiff_t offset = 0;
    size_t n = 0;
    auto cleanup = scope_exit([=] { file.remove(); });

    file.remove();
    TEST(! file.exists());
    TEST(! io.is_open());
    TEST(! io.ok());

    TRY(io = Gzio(file, IO::mode::write_only));
    TEST(io.is_open());
    TEST(io.ok());
    TRY(io.write_str("Hello world\n"));
    TEST(io.ok());
    TRY(io.write_str("Goodbye\n"));
    TEST(io.ok());
    TRY(io = {});
    TEST(! io.is_open());
    TEST(file.exists());

    #ifdef _XOPEN_SOURCE

        Path log = "__gzio_log__";
        Ustring cmd = fmt("file $1 >$2", file, log);
        int rc = 0;
        TRY(rc = std::system(cmd.data()));
        TEST_EQUAL(rc, 0);
        TRY(log.load(text));
        TEST_MATCH(text, "gzip compressed data");
        TRY(log.remove());

    #endif

    text.clear();
    TRY(io = Gzio(file));
    TRY(n = io.read_n(text, 12));  TEST(io.ok());  TEST_EQUAL(n, 12);  TEST_EQUAL(text, "Hello world\n");
    TRY(n = io.read_n(text, 8));   TEST(io.ok());  TEST_EQUAL(n, 8);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(n = io.read_n(text, 1));   TEST(io.ok());  TEST_EQUAL(n, 0);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Gzio(file));
    TRY(text = io.read_str(12));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
    TRY(text = io.read_str(8));   TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
    TRY(text = io.read_str(1));   TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Gzio(file));
    TRY(text = io.read_str(20));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(text = io.read_str(1));   TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Gzio(file));
    TRY(text = io.read_str(100));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Gzio(file));
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Gzio(file));
    TRY(text = io.read_all());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Gzio(file));
    auto lines = io.lines();
    TRY(std::copy(lines.begin(), lines.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL(vec.at(0), "Hello world\n");
    TEST_EQUAL(vec.at(1), "Goodbye\n");
    TRY(io = {});

    TRY(io = Gzio(file));
    TRY(offset = io.tell());     TEST(io.ok());  TEST_EQUAL(offset, 0);
    TRY(io.seek(12));            TEST(io.ok());
    TRY(offset = io.tell());     TEST(io.ok());  TEST_EQUAL(offset, 12);
    TRY(text = io.read_str(7));  TEST(io.ok());  TEST_EQUAL(text, "Goodbye");
    TRY(offset = io.tell());     TEST(io.ok());  TEST_EQUAL(offset, 19);
    TRY(io.seek(-13));           TEST(io.ok());
    TRY(offset = io.tell());     TEST(io.ok());  TEST_EQUAL(offset, 6);
    TRY(text = io.read_str(5));  TEST(io.ok());  TEST_EQUAL(text, "world");
    TRY(offset = io.tell());     TEST(io.ok());  TEST_EQUAL(offset, 11);
    TRY(io = {});

    TRY(io = Gzio(no_file));
    TEST_THROW(io.check(), std::system_error);

}
