#include "rs-core/io.hpp"
#include "rs-core/unit-test.hpp"
#include "unicorn/path.hpp"
#include <algorithm>
#include <system_error>
#include <utility>

using namespace RS;
using namespace RS::Unicorn;

void test_core_io_cstdio() {

    Cstdio io;
    Path file = "__cstdio_test__", no_file = "__no_such_file__";
    Ustring text;
    Strings vec;
    ptrdiff_t offset = 0;
    size_t n = 0;
    auto cleanup = scope_exit([=] { file.remove(); });

    file.remove();
    TEST(! file.exists());
    TEST(! io.is_open());
    TEST(! io.ok());

    TRY(io = Cstdio(file, IO::mode::write_only));
    TEST(io.is_open());
    TEST(io.ok());
    TRY(io.write_str("Hello world\n"));
    TEST(io.ok());
    TRY(io.write_str("Goodbye\n"));
    TEST(io.ok());
    TRY(io = {});
    TEST(! io.is_open());
    TEST(file.exists());

    text.clear();
    TRY(io = Cstdio(file));
    TRY(n = io.read_n(text, 12));  TEST(io.ok());  TEST_EQUAL(n, 12);  TEST_EQUAL(text, "Hello world\n");
    TRY(n = io.read_n(text, 8));   TEST(io.ok());  TEST_EQUAL(n, 8);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(n = io.read_n(text, 1));   TEST(io.ok());  TEST_EQUAL(n, 0);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Cstdio(file));
    TRY(text = io.read_str(12));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
    TRY(text = io.read_str(8));   TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
    TRY(text = io.read_str(1));   TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Cstdio(file));
    TRY(text = io.read_str(20));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(text = io.read_str(1));   TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Cstdio(file));
    TRY(text = io.read_str(100));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Cstdio(file));
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Cstdio(file));
    TRY(text = io.read_all());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Cstdio(file));
    auto lines = io.lines();
    TRY(std::copy(lines.begin(), lines.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL(vec.at(0), "Hello world\n");
    TEST_EQUAL(vec.at(1), "Goodbye\n");
    TRY(io = {});

    TRY(io = Cstdio(file));
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

    TRY(io = Cstdio(no_file, IO::mode::read_only));
    TEST_THROW(io.check(), std::system_error);

}

void test_core_io_fdio() {

    Fdio io;
    Path file = "__fdio_test__", no_file = "__no_such_file__";
    Ustring text;
    Strings vec;
    ptrdiff_t offset = 0;
    size_t n = 0;
    auto cleanup = scope_exit([=] { file.remove(); });

    file.remove();
    TEST(! file.exists());
    TEST(! io.is_open());
    TEST(! io.ok());

    TRY(io = Fdio(file, IO::mode::write_only));
    TEST(io.is_open());
    TEST(io.ok());
    TRY(io.write_str("Hello world\n"));
    TEST(io.ok());
    TRY(io.write_str("Goodbye\n"));
    TEST(io.ok());
    TRY(io = {});
    TEST(! io.is_open());
    TEST(file.exists());

    text.clear();
    TRY(io = Fdio(file));
    TRY(n = io.read_n(text, 12));  TEST(io.ok());  TEST_EQUAL(n, 12);  TEST_EQUAL(text, "Hello world\n");
    TRY(n = io.read_n(text, 8));   TEST(io.ok());  TEST_EQUAL(n, 8);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(n = io.read_n(text, 1));   TEST(io.ok());  TEST_EQUAL(n, 0);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Fdio(file));
    TRY(text = io.read_str(12));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
    TRY(text = io.read_str(8));   TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
    TRY(text = io.read_str(1));   TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Fdio(file));
    TRY(text = io.read_str(100));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Fdio(file));
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
    TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "");
    TRY(io = {});

    TRY(io = Fdio(file));
    TRY(text = io.read_all());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
    TRY(io = {});

    TRY(io = Fdio(file));
    auto lines = io.lines();
    TRY(std::copy(lines.begin(), lines.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL(vec.at(0), "Hello world\n");
    TEST_EQUAL(vec.at(1), "Goodbye\n");
    TRY(io = {});

    TRY(io = Fdio(file));
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

    TRY(io = Fdio(no_file, IO::mode::read_only));
    TEST_THROW(io.check(), std::system_error);

}

void test_core_io_pipe() {

    std::pair<Fdio, Fdio> pipe;
    Ustring text;

    TRY(pipe = Fdio::pipe());
    TRY(pipe.second.write_str("Hello world\n"));
    TRY(pipe.second.write_str("Goodbye\n"));
    TRY(text = pipe.first.read_str(100));
    TEST_EQUAL(text, "Hello world\nGoodbye\n");

}

void test_core_io_winio() {

    #ifdef _WIN32

        Winio io;
        Path file = "__winio_test__", no_file = "__no_such_file__";
        Ustring text;
        Strings vec;
        ptrdiff_t offset = 0;
        size_t n = 0;
        auto cleanup = scope_exit([=] { file.remove(); });

        file.remove();
        TEST(! file.exists());
        TEST(! io.is_open());
        TEST(! io.ok());

        TRY(io = Winio(file, IO::mode::write_only));
        TEST(io.is_open());
        TEST(io.ok());
        TRY(io.write_str("Hello world\n"));
        TEST(io.ok());
        TRY(io.write_str("Goodbye\n"));
        TEST(io.ok());
        TRY(io = {});
        TEST(! io.is_open());
        TEST(file.exists());

        text.clear();
        TRY(io = Winio(file));
        TRY(n = io.read_n(text, 12));  TEST(io.ok());  TEST_EQUAL(n, 12);  TEST_EQUAL(text, "Hello world\n");
        TRY(n = io.read_n(text, 8));   TEST(io.ok());  TEST_EQUAL(n, 8);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
        TRY(n = io.read_n(text, 1));   TEST(io.ok());  TEST_EQUAL(n, 0);   TEST_EQUAL(text, "Hello world\nGoodbye\n");
        TRY(io = {});

        TRY(io = Winio(file));
        TRY(text = io.read_str(12));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
        TRY(text = io.read_str(8));   TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
        TRY(text = io.read_str(1));   TEST(io.ok());  TEST_EQUAL(text, "");
        TRY(io = {});

        TRY(io = Winio(file));
        TRY(text = io.read_str(20));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
        TRY(text = io.read_str(1));   TEST(io.ok());  TEST_EQUAL(text, "");
        TRY(io = {});

        TRY(io = Winio(file));
        TRY(text = io.read_str(100));  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
        TRY(io = {});

        TRY(io = Winio(file));
        TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\n");
        TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "Goodbye\n");
        TRY(text = io.read_line());  TEST(io.ok());  TEST_EQUAL(text, "");
        TRY(io = {});

        TRY(io = Winio(file));
        TRY(text = io.read_all());  TEST(io.ok());  TEST_EQUAL(text, "Hello world\nGoodbye\n");
        TRY(io = {});

        TRY(io = Winio(file));
        auto lines = io.lines();
        TRY(std::copy(lines.begin(), lines.end(), overwrite(vec)));
        TRY(text = to_str(vec));
        TEST_EQUAL(text, "[Hello world,Goodbye]");
        TRY(io = {});

        TRY(io = Winio(file));
        TRY(offset = io.tell());     TEST_EQUAL(offset, 0);
        TRY(io.seek(12));
        TRY(offset = io.tell());     TEST_EQUAL(offset, 12);
        TRY(text = io.read_str(7));  TEST_EQUAL(text, "Goodbye");
        TRY(offset = io.tell());     TEST_EQUAL(offset, 19);
        TRY(io.seek(-13));
        TRY(offset = io.tell());     TEST_EQUAL(offset, 6);
        TRY(text = io.read_str(5));  TEST_EQUAL(text, "world");
        TRY(offset = io.tell());     TEST_EQUAL(offset, 11);
        TRY(io = {});

        TRY(io = Winio(no_file, IO::mode::read_only));
        TEST_THROW(io.check(), std::system_error);

    #endif

}

void test_core_io_null_device() {

    Ustring text;

    {
        Cstdio io;
        TRY(io = Cstdio::dev_null());
        TRY(text = io.read_str(20));
        TRY(text = io.read_str(20));
        TEST_EQUAL(text, "");
    }

    {
        Cstdio io;
        TRY(io = Cstdio::dev_null());
        TRY(io.write_str("Hello world\n"));
        TRY(io.write_str("Hello world\n"));
        TRY(text = io.read_str(20));
        TRY(text = io.read_str(20));
        #ifdef _XOPEN_SOURCE
            TEST_EQUAL(text, "");
        #endif
    }

    {
        Fdio io;
        TRY(io = Fdio::dev_null());
        TRY(text = io.read_str(20));
        TRY(text = io.read_str(20));
        TEST_EQUAL(text, "");
    }

    {
        Fdio io;
        TRY(io = Fdio::dev_null());
        TRY(io.write_str("Hello world\n"));
        TRY(io.write_str("Hello world\n"));
        TRY(text = io.read_str(20));
        TRY(text = io.read_str(20));
        TEST_EQUAL(text, "");
    }

    #ifdef _WIN32

        {
            Winio io;
            TRY(io = Winio::dev_null());
            TRY(text = io.read_str(20));
            TRY(text = io.read_str(20));
            TEST_EQUAL(text, "");
        }

        {
            Winio io;
            TRY(io = Winio::dev_null());
            TRY(io.write_str("Hello world\n"));
            TRY(io.write_str("Hello world\n"));
            TRY(text = io.read_str(20));
            TRY(text = io.read_str(20));
            TEST_EQUAL(text, "");
        }

    #endif

}

void test_core_io_external_buffer() {

    StringBuffer buf;
    Ustring s, t;
    Strings vec;

    TRY(buf = StringBuffer(s));
    TEST(buf.empty());
    TEST_EQUAL(buf.size(), 0);
    TEST_EQUAL(buf.str(), "");

    TRY(buf.write_str("Hello world"));
    TEST(! buf.empty());
    TEST_EQUAL(buf.size(), 11);
    TEST_EQUAL(buf.str(), "Hello world");

    TRY(buf.write_str(" goodbye"));
    TEST_EQUAL(buf.size(), 19);
    TEST_EQUAL(buf.str(), "Hello world goodbye");

    TRY(t = buf.read_str(6));
    TEST_EQUAL(t, "Hello ");
    TEST_EQUAL(buf.size(), 13);
    TEST_EQUAL(buf.str(), "world goodbye");

    TRY(t = buf.read_str(100));
    TEST_EQUAL(t, "world goodbye");
    TEST_EQUAL(buf.size(), 0);
    TEST_EQUAL(buf.str(), "");

    TRY(buf.flush());
    TEST_EQUAL(s, "");
    TEST_EQUAL(buf.str(), "");
    TRY(buf.write_str("Hello world"));
    TEST_EQUAL(s, "Hello world");
    TEST_EQUAL(buf.str(), "Hello world");
    TRY(buf.flush());
    TEST_EQUAL(s, "Hello world");
    TEST_EQUAL(buf.str(), "Hello world");
    TRY(buf.write_str(" goodbye"));
    TEST_EQUAL(s, "Hello world goodbye");
    TEST_EQUAL(buf.str(), "Hello world goodbye");
    TRY(buf.flush());
    TEST_EQUAL(s, "Hello world goodbye");
    TEST_EQUAL(buf.str(), "Hello world goodbye");
    TRY(buf.seek(6));
    TEST_EQUAL(s, "Hello world goodbye");
    TEST_EQUAL(buf.str(), "world goodbye");
    TRY(buf.flush());
    TEST_EQUAL(s, "world goodbye");
    TEST_EQUAL(buf.str(), "world goodbye");
    TRY(buf.seek(100));
    TEST_EQUAL(buf.str(), "");
    TRY(buf.flush());
    TEST_EQUAL(s, "");
    TEST_EQUAL(buf.str(), "");

    TRY(buf.write_str("Hello"));
    TEST_EQUAL(buf.str(), "Hello");
    TRY(buf.clear());
    TEST_EQUAL(buf.size(), 0);
    TEST_EQUAL(buf.str(), "");

    TRY(buf.clear());
    TRY(buf.write_str("Hello world\nGoodbye\n"));
    TRY(t = buf.read_line());  TEST(buf.ok());  TEST_EQUAL(t, "Hello world\n");
    TRY(t = buf.read_line());  TEST(buf.ok());  TEST_EQUAL(t, "Goodbye\n");
    TRY(t = buf.read_line());  TEST(buf.ok());  TEST_EQUAL(t, "");

    TRY(buf.clear());
    TRY(buf.write_str("Hello world\nGoodbye\n"));
    auto lines = buf.lines();
    TRY(std::copy(lines.begin(), lines.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL(vec.at(0), "Hello world\n");
    TEST_EQUAL(vec.at(1), "Goodbye\n");

}

void test_core_io_internal_buffer() {

    StringBuffer buf;
    Ustring t;
    Strings vec;

    TEST(buf.empty());
    TEST_EQUAL(buf.size(), 0);
    TEST_EQUAL(buf.str(), "");

    TRY(buf.write_str("Hello world"));
    TEST(! buf.empty());
    TEST_EQUAL(buf.size(), 11);
    TEST_EQUAL(buf.str(), "Hello world");

    TRY(buf.write_str(" goodbye"));
    TEST_EQUAL(buf.size(), 19);
    TEST_EQUAL(buf.str(), "Hello world goodbye");

    TRY(t = buf.read_str(6));
    TEST_EQUAL(t, "Hello ");
    TEST_EQUAL(buf.size(), 13);
    TEST_EQUAL(buf.str(), "world goodbye");

    TRY(t = buf.read_str(100));
    TEST_EQUAL(t, "world goodbye");
    TEST_EQUAL(buf.size(), 0);
    TEST_EQUAL(buf.str(), "");

    TRY(buf.write_str("Hello"));
    TEST_EQUAL(buf.str(), "Hello");
    TRY(buf.clear());
    TEST_EQUAL(buf.size(), 0);
    TEST_EQUAL(buf.str(), "");

    TRY(buf.clear());
    TRY(buf.write_str("Hello world\nGoodbye\n"));
    TRY(t = buf.read_line());  TEST_EQUAL(t, "Hello world\n");
    TRY(t = buf.read_line());  TEST_EQUAL(t, "Goodbye\n");
    TRY(t = buf.read_line());  TEST_EQUAL(t, "");

    TRY(buf.clear());
    TRY(buf.write_str("Hello world\nGoodbye\n"));
    auto lines = buf.lines();
    TRY(std::copy(lines.begin(), lines.end(), overwrite(vec)));
    TEST_EQUAL(vec.size(), 2);
    TEST_EQUAL(vec.at(0), "Hello world\n");
    TEST_EQUAL(vec.at(1), "Goodbye\n");

}

void test_core_io_print_formatting() {

    StringBuffer buf;
    Ustring s;

    TRY(buf.print());
    TRY(s = buf.str());
    TEST_EQUAL(s, "\n");

    TRY(buf.clear());
    TRY(buf.print("Hello", "world", 42));
    TRY(s = buf.str());
    TEST_EQUAL(s, "Hello world 42\n");

    TRY(buf.clear());
    TRY(buf.format("$1,$2,$3", "Hello", "world", 42));
    TRY(s = buf.str());
    TEST_EQUAL(s, "Hello,world,42");

    TRY(buf.clear());
    TRY(buf.format("$1,$2,$3"));
    TRY(s = buf.str());
    TEST_EQUAL(s, ",,");

}
