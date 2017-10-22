#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <atomic>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

using namespace RS;
using namespace std::literals;

namespace {

    class Base {
    public:
        virtual ~Base() noexcept {}
        virtual int get() const = 0;
    };

    class Derived: public Base {
    public:
        virtual int get() const { return 1; }
    };

}

void test_core_string_literals() {

    using namespace RS::Literals;

    U8string s;
    Strings v;

    TRY(v = ""_csv);                           TEST_EQUAL(v.size(), 0);  TRY(s = to_str(v));  TEST_EQUAL(s, "[]");
    TRY(v = " "_csv);                          TEST_EQUAL(v.size(), 1);  TRY(s = to_str(v));  TEST_EQUAL(s, "[]");
    TRY(v = ","_csv);                          TEST_EQUAL(v.size(), 2);  TRY(s = to_str(v));  TEST_EQUAL(s, "[,]");
    TRY(v = ",,"_csv);                         TEST_EQUAL(v.size(), 3);  TRY(s = to_str(v));  TEST_EQUAL(s, "[,,]");
    TRY(v = "alpha"_csv);                      TEST_EQUAL(v.size(), 1);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha]");
    TRY(v = " alpha "_csv);                    TEST_EQUAL(v.size(), 1);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha]");
    TRY(v = ",,alpha,,"_csv);                  TEST_EQUAL(v.size(), 5);  TRY(s = to_str(v));  TEST_EQUAL(s, "[,,alpha,,]");
    TRY(v = "alpha,bravo"_csv);                TEST_EQUAL(v.size(), 2);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha,bravo]");
    TRY(v = "alpha,bravo,charlie"_csv);        TEST_EQUAL(v.size(), 3);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha,bravo,charlie]");
    TRY(v = " alpha , bravo , charlie "_csv);  TEST_EQUAL(v.size(), 3);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha,bravo,charlie]");
    TRY(v = ",alpha,bravo,charlie,"_csv);      TEST_EQUAL(v.size(), 5);  TRY(s = to_str(v));  TEST_EQUAL(s, "[,alpha,bravo,charlie,]");

    TRY(v = ""_qw);                        TEST_EQUAL(v.size(), 0);  TRY(s = to_str(v));  TEST_EQUAL(s, "[]");
    TRY(v = " \n "_qw);                    TEST_EQUAL(v.size(), 0);  TRY(s = to_str(v));  TEST_EQUAL(s, "[]");
    TRY(v = "alpha"_qw);                   TEST_EQUAL(v.size(), 1);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha]");
    TRY(v = "alpha bravo"_qw);             TEST_EQUAL(v.size(), 2);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha,bravo]");
    TRY(v = " \n alpha \n bravo \n "_qw);  TEST_EQUAL(v.size(), 2);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha,bravo]");
    TRY(v = "alpha bravo charlie"_qw);     TEST_EQUAL(v.size(), 3);  TRY(s = to_str(v));  TEST_EQUAL(s, "[alpha,bravo,charlie]");

    TRY(s = ""_doc);
    TEST_EQUAL(s, "");

    TRY(s = R"(

        Hello world.

            Hello again.

        Goodbye.

    )"_doc);
    TEST_EQUAL(s,
        "Hello world.\n"
        "\n"
        "    Hello again.\n"
        "\n"
        "Goodbye.\n"
    );

    TRY(s =
        "\t\tHello world.\n"
        "\t\t\tHello again.\n"
        "\t\t\t\tGoodbye.\n"
        ""_doc);
    TEST_EQUAL(s,
        "Hello world.\n"
        "    Hello again.\n"
        "        Goodbye.\n"
    );
}

void test_core_string_character() {

    TEST(! ascii_isalnum('\0'));
    TEST(! ascii_isalnum('\t'));
    TEST(! ascii_isalnum('\n'));
    TEST(! ascii_isalnum('\x1f'));
    TEST(! ascii_isalnum(' '));
    TEST(! ascii_isalnum('!'));
    TEST(! ascii_isalnum('/'));
    TEST(ascii_isalnum('0'));
    TEST(ascii_isalnum('9'));
    TEST(! ascii_isalnum(':'));
    TEST(! ascii_isalnum('@'));
    TEST(ascii_isalnum('A'));
    TEST(ascii_isalnum('Z'));
    TEST(! ascii_isalnum('['));
    TEST(! ascii_isalnum('_'));
    TEST(! ascii_isalnum('`'));
    TEST(ascii_isalnum('a'));
    TEST(ascii_isalnum('z'));
    TEST(! ascii_isalnum('{'));
    TEST(! ascii_isalnum('~'));
    TEST(! ascii_isalnum('\x7f'));
    TEST(! ascii_isalnum('\x80'));

    TEST(! ascii_isalpha('\0'));
    TEST(! ascii_isalpha('\t'));
    TEST(! ascii_isalpha('\n'));
    TEST(! ascii_isalpha('\x1f'));
    TEST(! ascii_isalpha(' '));
    TEST(! ascii_isalpha('!'));
    TEST(! ascii_isalpha('/'));
    TEST(! ascii_isalpha('0'));
    TEST(! ascii_isalpha('9'));
    TEST(! ascii_isalpha(':'));
    TEST(! ascii_isalpha('@'));
    TEST(ascii_isalpha('A'));
    TEST(ascii_isalpha('Z'));
    TEST(! ascii_isalpha('['));
    TEST(! ascii_isalpha('_'));
    TEST(! ascii_isalpha('`'));
    TEST(ascii_isalpha('a'));
    TEST(ascii_isalpha('z'));
    TEST(! ascii_isalpha('{'));
    TEST(! ascii_isalpha('~'));
    TEST(! ascii_isalpha('\x7f'));
    TEST(! ascii_isalpha('\x80'));

    TEST(ascii_iscntrl('\0'));
    TEST(ascii_iscntrl('\t'));
    TEST(ascii_iscntrl('\n'));
    TEST(ascii_iscntrl('\x1f'));
    TEST(! ascii_iscntrl(' '));
    TEST(! ascii_iscntrl('!'));
    TEST(! ascii_iscntrl('/'));
    TEST(! ascii_iscntrl('0'));
    TEST(! ascii_iscntrl('9'));
    TEST(! ascii_iscntrl(':'));
    TEST(! ascii_iscntrl('@'));
    TEST(! ascii_iscntrl('A'));
    TEST(! ascii_iscntrl('Z'));
    TEST(! ascii_iscntrl('['));
    TEST(! ascii_iscntrl('_'));
    TEST(! ascii_iscntrl('`'));
    TEST(! ascii_iscntrl('a'));
    TEST(! ascii_iscntrl('z'));
    TEST(! ascii_iscntrl('{'));
    TEST(! ascii_iscntrl('~'));
    TEST(ascii_iscntrl('\x7f'));
    TEST(! ascii_iscntrl('\x80'));

    TEST(! ascii_isdigit('\0'));
    TEST(! ascii_isdigit('\t'));
    TEST(! ascii_isdigit('\n'));
    TEST(! ascii_isdigit('\x1f'));
    TEST(! ascii_isdigit(' '));
    TEST(! ascii_isdigit('!'));
    TEST(! ascii_isdigit('/'));
    TEST(ascii_isdigit('0'));
    TEST(ascii_isdigit('9'));
    TEST(! ascii_isdigit(':'));
    TEST(! ascii_isdigit('@'));
    TEST(! ascii_isdigit('A'));
    TEST(! ascii_isdigit('Z'));
    TEST(! ascii_isdigit('['));
    TEST(! ascii_isdigit('_'));
    TEST(! ascii_isdigit('`'));
    TEST(! ascii_isdigit('a'));
    TEST(! ascii_isdigit('z'));
    TEST(! ascii_isdigit('{'));
    TEST(! ascii_isdigit('~'));
    TEST(! ascii_isdigit('\x7f'));
    TEST(! ascii_isdigit('\x80'));

    TEST(! ascii_isgraph('\0'));
    TEST(! ascii_isgraph('\t'));
    TEST(! ascii_isgraph('\n'));
    TEST(! ascii_isgraph('\x1f'));
    TEST(! ascii_isgraph(' '));
    TEST(ascii_isgraph('!'));
    TEST(ascii_isgraph('/'));
    TEST(ascii_isgraph('0'));
    TEST(ascii_isgraph('9'));
    TEST(ascii_isgraph(':'));
    TEST(ascii_isgraph('@'));
    TEST(ascii_isgraph('A'));
    TEST(ascii_isgraph('Z'));
    TEST(ascii_isgraph('['));
    TEST(ascii_isgraph('_'));
    TEST(ascii_isgraph('`'));
    TEST(ascii_isgraph('a'));
    TEST(ascii_isgraph('z'));
    TEST(ascii_isgraph('{'));
    TEST(ascii_isgraph('~'));
    TEST(! ascii_isgraph('\x7f'));
    TEST(! ascii_isgraph('\x80'));

    TEST(! ascii_islower('\0'));
    TEST(! ascii_islower('\t'));
    TEST(! ascii_islower('\n'));
    TEST(! ascii_islower('\x1f'));
    TEST(! ascii_islower(' '));
    TEST(! ascii_islower('!'));
    TEST(! ascii_islower('/'));
    TEST(! ascii_islower('0'));
    TEST(! ascii_islower('9'));
    TEST(! ascii_islower(':'));
    TEST(! ascii_islower('@'));
    TEST(! ascii_islower('A'));
    TEST(! ascii_islower('Z'));
    TEST(! ascii_islower('['));
    TEST(! ascii_islower('_'));
    TEST(! ascii_islower('`'));
    TEST(ascii_islower('a'));
    TEST(ascii_islower('z'));
    TEST(! ascii_islower('{'));
    TEST(! ascii_islower('~'));
    TEST(! ascii_islower('\x7f'));
    TEST(! ascii_islower('\x80'));

    TEST(! ascii_isprint('\0'));
    TEST(! ascii_isprint('\t'));
    TEST(! ascii_isprint('\n'));
    TEST(! ascii_isprint('\x1f'));
    TEST(ascii_isprint(' '));
    TEST(ascii_isprint('!'));
    TEST(ascii_isprint('/'));
    TEST(ascii_isprint('0'));
    TEST(ascii_isprint('9'));
    TEST(ascii_isprint(':'));
    TEST(ascii_isprint('@'));
    TEST(ascii_isprint('A'));
    TEST(ascii_isprint('Z'));
    TEST(ascii_isprint('['));
    TEST(ascii_isprint('_'));
    TEST(ascii_isprint('`'));
    TEST(ascii_isprint('a'));
    TEST(ascii_isprint('z'));
    TEST(ascii_isprint('{'));
    TEST(ascii_isprint('~'));
    TEST(! ascii_isprint('\x7f'));
    TEST(! ascii_isprint('\x80'));

    TEST(! ascii_ispunct('\0'));
    TEST(! ascii_ispunct('\t'));
    TEST(! ascii_ispunct('\n'));
    TEST(! ascii_ispunct('\x1f'));
    TEST(! ascii_ispunct(' '));
    TEST(ascii_ispunct('!'));
    TEST(ascii_ispunct('/'));
    TEST(! ascii_ispunct('0'));
    TEST(! ascii_ispunct('9'));
    TEST(ascii_ispunct(':'));
    TEST(ascii_ispunct('@'));
    TEST(! ascii_ispunct('A'));
    TEST(! ascii_ispunct('Z'));
    TEST(ascii_ispunct('['));
    TEST(ascii_ispunct('_'));
    TEST(ascii_ispunct('`'));
    TEST(! ascii_ispunct('a'));
    TEST(! ascii_ispunct('z'));
    TEST(ascii_ispunct('{'));
    TEST(ascii_ispunct('~'));
    TEST(! ascii_ispunct('\x7f'));
    TEST(! ascii_ispunct('\x80'));

    TEST(! ascii_isspace('\0'));
    TEST(ascii_isspace('\t'));
    TEST(ascii_isspace('\n'));
    TEST(! ascii_isspace('\x1f'));
    TEST(ascii_isspace(' '));
    TEST(! ascii_isspace('!'));
    TEST(! ascii_isspace('/'));
    TEST(! ascii_isspace('0'));
    TEST(! ascii_isspace('9'));
    TEST(! ascii_isspace(':'));
    TEST(! ascii_isspace('@'));
    TEST(! ascii_isspace('A'));
    TEST(! ascii_isspace('Z'));
    TEST(! ascii_isspace('['));
    TEST(! ascii_isspace('_'));
    TEST(! ascii_isspace('`'));
    TEST(! ascii_isspace('a'));
    TEST(! ascii_isspace('z'));
    TEST(! ascii_isspace('{'));
    TEST(! ascii_isspace('~'));
    TEST(! ascii_isspace('\x7f'));
    TEST(! ascii_isspace('\x80'));

    TEST(! ascii_isupper('\0'));
    TEST(! ascii_isupper('\t'));
    TEST(! ascii_isupper('\n'));
    TEST(! ascii_isupper('\x1f'));
    TEST(! ascii_isupper(' '));
    TEST(! ascii_isupper('!'));
    TEST(! ascii_isupper('/'));
    TEST(! ascii_isupper('0'));
    TEST(! ascii_isupper('9'));
    TEST(! ascii_isupper(':'));
    TEST(! ascii_isupper('@'));
    TEST(ascii_isupper('A'));
    TEST(ascii_isupper('Z'));
    TEST(! ascii_isupper('['));
    TEST(! ascii_isupper('_'));
    TEST(! ascii_isupper('`'));
    TEST(! ascii_isupper('a'));
    TEST(! ascii_isupper('z'));
    TEST(! ascii_isupper('{'));
    TEST(! ascii_isupper('~'));
    TEST(! ascii_isupper('\x7f'));
    TEST(! ascii_isupper('\x80'));

    TEST(! ascii_isxdigit('\0'));
    TEST(! ascii_isxdigit('\t'));
    TEST(! ascii_isxdigit('\n'));
    TEST(! ascii_isxdigit('\x1f'));
    TEST(! ascii_isxdigit(' '));
    TEST(! ascii_isxdigit('!'));
    TEST(! ascii_isxdigit('/'));
    TEST(ascii_isxdigit('0'));
    TEST(ascii_isxdigit('9'));
    TEST(! ascii_isxdigit(':'));
    TEST(! ascii_isxdigit('@'));
    TEST(ascii_isxdigit('A'));
    TEST(!ascii_isxdigit('Z'));
    TEST(!ascii_isxdigit('['));
    TEST(!ascii_isxdigit('_'));
    TEST(!ascii_isxdigit('`'));
    TEST(ascii_isxdigit('a'));
    TEST(! ascii_isxdigit('z'));
    TEST(! ascii_isxdigit('{'));
    TEST(! ascii_isxdigit('~'));
    TEST(! ascii_isxdigit('\x7f'));
    TEST(! ascii_isxdigit('\x80'));

    TEST(! ascii_isalnum_w('\0'));
    TEST(! ascii_isalnum_w('\t'));
    TEST(! ascii_isalnum_w('\n'));
    TEST(! ascii_isalnum_w('\x1f'));
    TEST(! ascii_isalnum_w(' '));
    TEST(! ascii_isalnum_w('!'));
    TEST(! ascii_isalnum_w('/'));
    TEST(ascii_isalnum_w('0'));
    TEST(ascii_isalnum_w('9'));
    TEST(! ascii_isalnum_w(':'));
    TEST(! ascii_isalnum_w('@'));
    TEST(ascii_isalnum_w('A'));
    TEST(ascii_isalnum_w('Z'));
    TEST(! ascii_isalnum_w('['));
    TEST(ascii_isalnum_w('_'));
    TEST(! ascii_isalnum_w('`'));
    TEST(ascii_isalnum_w('a'));
    TEST(ascii_isalnum_w('z'));
    TEST(! ascii_isalnum_w('{'));
    TEST(! ascii_isalnum_w('~'));
    TEST(! ascii_isalnum_w('\x7f'));
    TEST(! ascii_isalnum_w('\x80'));

    TEST(! ascii_isalpha_w('\0'));
    TEST(! ascii_isalpha_w('\t'));
    TEST(! ascii_isalpha_w('\n'));
    TEST(! ascii_isalpha_w('\x1f'));
    TEST(! ascii_isalpha_w(' '));
    TEST(! ascii_isalpha_w('!'));
    TEST(! ascii_isalpha_w('/'));
    TEST(! ascii_isalpha_w('0'));
    TEST(! ascii_isalpha_w('9'));
    TEST(! ascii_isalpha_w(':'));
    TEST(! ascii_isalpha_w('@'));
    TEST(ascii_isalpha_w('A'));
    TEST(ascii_isalpha_w('Z'));
    TEST(! ascii_isalpha_w('['));
    TEST(ascii_isalpha_w('_'));
    TEST(! ascii_isalpha_w('`'));
    TEST(ascii_isalpha_w('a'));
    TEST(ascii_isalpha_w('z'));
    TEST(! ascii_isalpha_w('{'));
    TEST(! ascii_isalpha_w('~'));
    TEST(! ascii_isalpha_w('\x7f'));
    TEST(! ascii_isalpha_w('\x80'));

    TEST(! ascii_ispunct_w('\0'));
    TEST(! ascii_ispunct_w('\t'));
    TEST(! ascii_ispunct_w('\n'));
    TEST(! ascii_ispunct_w('\x1f'));
    TEST(! ascii_ispunct_w(' '));
    TEST(ascii_ispunct_w('!'));
    TEST(ascii_ispunct_w('/'));
    TEST(! ascii_ispunct_w('0'));
    TEST(! ascii_ispunct_w('9'));
    TEST(ascii_ispunct_w(':'));
    TEST(ascii_ispunct_w('@'));
    TEST(! ascii_ispunct_w('A'));
    TEST(! ascii_ispunct_w('Z'));
    TEST(ascii_ispunct_w('['));
    TEST(! ascii_ispunct_w('_'));
    TEST(ascii_ispunct_w('`'));
    TEST(! ascii_ispunct_w('a'));
    TEST(! ascii_ispunct_w('z'));
    TEST(ascii_ispunct_w('{'));
    TEST(ascii_ispunct_w('~'));
    TEST(! ascii_ispunct_w('\x7f'));
    TEST(! ascii_ispunct_w('\x80'));

    TEST_EQUAL(ascii_tolower('A'), 'a');
    TEST_EQUAL(ascii_tolower('a'), 'a');
    TEST_EQUAL(ascii_tolower('\0'), '\0');
    TEST_EQUAL(ascii_tolower('@'), '@');
    TEST_EQUAL(ascii_tolower('\x7f'), '\x7f');
    TEST_EQUAL(ascii_tolower('\x80'), '\x80');
    TEST_EQUAL(ascii_tolower('\xff'), '\xff');

    TEST_EQUAL(ascii_toupper('A'), 'A');
    TEST_EQUAL(ascii_toupper('a'), 'A');
    TEST_EQUAL(ascii_toupper('\0'), '\0');
    TEST_EQUAL(ascii_toupper('@'), '@');
    TEST_EQUAL(ascii_toupper('\x7f'), '\x7f');
    TEST_EQUAL(ascii_toupper('\x80'), '\x80');
    TEST_EQUAL(ascii_toupper('\xff'), '\xff');

    TEST_EQUAL(char_to<int>(0), 0);
    TEST_EQUAL(char_to<int>('A'), 65);
    TEST_EQUAL(char_to<int>(char(127)), 127);
    TEST_EQUAL(char_to<int>(char(128)), 128);
    TEST_EQUAL(char_to<int>(char(-128)), 128);
    TEST_EQUAL(char_to<int>(char(255)), 255);
    TEST_EQUAL(char_to<int>(char(-1)), 255);

    TEST_EQUAL(char_to<uint32_t>(0), 0);
    TEST_EQUAL(char_to<uint32_t>('A'), 65);
    TEST_EQUAL(char_to<uint32_t>(char(127)), 127);
    TEST_EQUAL(char_to<uint32_t>(char(128)), 128);
    TEST_EQUAL(char_to<uint32_t>(char(-128)), 128);
    TEST_EQUAL(char_to<uint32_t>(char(255)), 255);
    TEST_EQUAL(char_to<uint32_t>(char(-1)), 255);

}

void test_core_string_property() {

    TEST(ascii_icase_equal("", ""));                          TEST(! ascii_icase_less("", ""));
    TEST(! ascii_icase_equal("", "hello"));                   TEST(ascii_icase_less("", "hello"));
    TEST(! ascii_icase_equal("hello", ""));                   TEST(! ascii_icase_less("hello", ""));
    TEST(ascii_icase_equal("hello", "hello"));                TEST(! ascii_icase_less("hello", "hello"));
    TEST(ascii_icase_equal("hello", "HELLO"));                TEST(! ascii_icase_less("hello", "HELLO"));
    TEST(ascii_icase_equal("HELLO", "hello"));                TEST(! ascii_icase_less("HELLO", "hello"));
    TEST(! ascii_icase_equal("hello", "hello world"));        TEST(ascii_icase_less("hello", "hello world"));
    TEST(! ascii_icase_equal("hello", "HELLO WORLD"));        TEST(ascii_icase_less("hello", "HELLO WORLD"));
    TEST(! ascii_icase_equal("HELLO", "hello world"));        TEST(ascii_icase_less("HELLO", "hello world"));
    TEST(! ascii_icase_equal("hello world", "hello"));        TEST(! ascii_icase_less("hello world", "hello"));
    TEST(! ascii_icase_equal("hello world", "HELLO"));        TEST(! ascii_icase_less("hello world", "HELLO"));
    TEST(! ascii_icase_equal("HELLO WORLD", "hello"));        TEST(! ascii_icase_less("HELLO WORLD", "hello"));
    TEST(! ascii_icase_equal("hello there", "hello world"));  TEST(ascii_icase_less("hello there", "hello world"));
    TEST(! ascii_icase_equal("hello there", "HELLO WORLD"));  TEST(ascii_icase_less("hello there", "HELLO WORLD"));
    TEST(! ascii_icase_equal("HELLO THERE", "hello world"));  TEST(ascii_icase_less("HELLO THERE", "hello world"));
    TEST(! ascii_icase_equal("hello world", "hello there"));  TEST(! ascii_icase_less("hello world", "hello there"));
    TEST(! ascii_icase_equal("hello world", "HELLO THERE"));  TEST(! ascii_icase_less("hello world", "HELLO THERE"));
    TEST(! ascii_icase_equal("HELLO WORLD", "hello there"));  TEST(! ascii_icase_less("HELLO WORLD", "hello there"));

    TEST(starts_with("", ""));
    TEST(starts_with("Hello world", ""));
    TEST(starts_with("Hello world", "Hello"));
    TEST(! starts_with("Hello world", "hello"));
    TEST(! starts_with("Hello world", "world"));
    TEST(! starts_with("Hello", "Hello world"));

    TEST(ends_with("", ""));
    TEST(ends_with("Hello world", ""));
    TEST(ends_with("Hello world", "world"));
    TEST(! ends_with("Hello world", "World"));
    TEST(! ends_with("Hello world", "Hello"));
    TEST(! ends_with("world", "Hello world"));

    TEST(string_is_ascii(""));
    TEST(string_is_ascii("Hello world"));
    TEST(! string_is_ascii("Hello world\xff"));
    TEST(! string_is_ascii(u8"åß∂"));

}

void test_core_string_manipulation() {

    std::string s;
    std::wstring ws;
    Strings sv;

    s = "";            TRY(s = add_prefix(s, ""));       TEST_EQUAL(s, "");
    s = "";            TRY(s = add_prefix(s, "Hello"));  TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = add_prefix(s, ""));       TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = add_prefix(s, "Hello"));  TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = add_prefix(s, "World"));  TEST_EQUAL(s, "WorldHello");
    s = "HelloWorld";  TRY(s = add_prefix(s, "Hello"));  TEST_EQUAL(s, "HelloWorld");
    s = "HelloWorld";  TRY(s = add_prefix(s, "World"));  TEST_EQUAL(s, "WorldHelloWorld");

    s = "";            TRY(s = add_suffix(s, ""));       TEST_EQUAL(s, "");
    s = "";            TRY(s = add_suffix(s, "Hello"));  TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = add_suffix(s, ""));       TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = add_suffix(s, "Hello"));  TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = add_suffix(s, "World"));  TEST_EQUAL(s, "HelloWorld");
    s = "HelloWorld";  TRY(s = add_suffix(s, "Hello"));  TEST_EQUAL(s, "HelloWorldHello");
    s = "HelloWorld";  TRY(s = add_suffix(s, "World"));  TEST_EQUAL(s, "HelloWorld");

    s = "";            TRY(s = drop_prefix(s, ""));       TEST_EQUAL(s, "");
    s = "";            TRY(s = drop_prefix(s, "Hello"));  TEST_EQUAL(s, "");
    s = "Hello";       TRY(s = drop_prefix(s, ""));       TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = drop_prefix(s, "Hello"));  TEST_EQUAL(s, "");
    s = "Hello";       TRY(s = drop_prefix(s, "World"));  TEST_EQUAL(s, "Hello");
    s = "HelloWorld";  TRY(s = drop_prefix(s, "Hello"));  TEST_EQUAL(s, "World");
    s = "HelloWorld";  TRY(s = drop_prefix(s, "World"));  TEST_EQUAL(s, "HelloWorld");

    s = "";            TRY(s = drop_suffix(s, ""));       TEST_EQUAL(s, "");
    s = "";            TRY(s = drop_suffix(s, "Hello"));  TEST_EQUAL(s, "");
    s = "Hello";       TRY(s = drop_suffix(s, ""));       TEST_EQUAL(s, "Hello");
    s = "Hello";       TRY(s = drop_suffix(s, "Hello"));  TEST_EQUAL(s, "");
    s = "Hello";       TRY(s = drop_suffix(s, "World"));  TEST_EQUAL(s, "Hello");
    s = "HelloWorld";  TRY(s = drop_suffix(s, "Hello"));  TEST_EQUAL(s, "HelloWorld");
    s = "HelloWorld";  TRY(s = drop_suffix(s, "World"));  TEST_EQUAL(s, "Hello");

    TEST_EQUAL(ascii_lowercase("Hello World"s), "hello world");
    TEST_EQUAL(ascii_uppercase("Hello World"s), "HELLO WORLD");
    TEST_EQUAL(ascii_titlecase("hello world"s), "Hello World");
    TEST_EQUAL(ascii_titlecase("HELLO WORLD"s), "Hello World");

    TEST_EQUAL(ascii_sentencecase(""), "");
    TEST_EQUAL(ascii_sentencecase("hello world"), "Hello world");
    TEST_EQUAL(ascii_sentencecase("hello world. goodbye. hello again."), "Hello world. Goodbye. Hello again.");
    TEST_EQUAL(ascii_sentencecase("hello world\ngoodbye\nhello again"), "Hello world\ngoodbye\nhello again");
    TEST_EQUAL(ascii_sentencecase("hello world\n\ngoodbye\n\nhello again"), "Hello world\n\nGoodbye\n\nHello again");
    TEST_EQUAL(ascii_sentencecase("hello world\r\n\r\ngoodbye\r\n\r\nhello again"), "Hello world\r\n\r\nGoodbye\r\n\r\nHello again");

    TRY(s = indent("", 2));
    TEST_EQUAL(s, "");
    TRY(s = indent("\n\n", 2));
    TEST_EQUAL(s, "\n\n");
    TRY(s = indent("Hello world\n    Hello again\nGoodbye", 2));
    TEST_EQUAL(s, "        Hello world\n            Hello again\n        Goodbye\n");

    sv.clear();                      TEST_EQUAL(join(sv), "");
    sv = {"Hello"};                  TEST_EQUAL(join(sv), "Hello");
    sv = {"Hello","world"};          TEST_EQUAL(join(sv), "Helloworld");
    sv = {"Hello","world","again"};  TEST_EQUAL(join(sv), "Helloworldagain");
    sv.clear();                      TEST_EQUAL(join(sv, " "), "");
    sv = {"Hello"};                  TEST_EQUAL(join(sv, " "), "Hello");
    sv = {"Hello","world"};          TEST_EQUAL(join(sv, " "), "Hello world");
    sv = {"Hello","world","again"};  TEST_EQUAL(join(sv, " "), "Hello world again");
    sv.clear();                      TEST_EQUAL(join(sv, "<*>"s), "");
    sv = {"Hello"};                  TEST_EQUAL(join(sv, "<*>"s), "Hello");
    sv = {"Hello","world"};          TEST_EQUAL(join(sv, "<*>"s), "Hello<*>world");
    sv = {"Hello","world","again"};  TEST_EQUAL(join(sv, "<*>"s), "Hello<*>world<*>again");
    sv.clear();                      TEST_EQUAL(join(sv, "\n", true), "");
    sv = {"Hello"};                  TEST_EQUAL(join(sv, "\n", true), "Hello\n");
    sv = {"Hello","world"};          TEST_EQUAL(join(sv, "\n", true), "Hello\nworld\n");
    sv = {"Hello","world","again"};  TEST_EQUAL(join(sv, "\n", true), "Hello\nworld\nagain\n");

    TRY(s = linearize(""));                                  TEST_EQUAL(s, "");
    TRY(s = linearize("\r\n\r\n"));                          TEST_EQUAL(s, "");
    TRY(s = linearize("Hello world."));                      TEST_EQUAL(s, "Hello world.");
    TRY(s = linearize("\r\nHello world.\r\nGoodbye.\r\n"));  TEST_EQUAL(s, "Hello world. Goodbye.");

    s = ""s;                TEST_EQUAL(null_term_str(s), "");
    s = "Hello world"s;     TEST_EQUAL(null_term_str(s), "Hello world");
    s = "Hello\0world"s;    TEST_EQUAL(null_term_str(s), "Hello");
    ws = L""s;              TEST_EQUAL(null_term_str(ws), L"");
    ws = L"Hello world"s;   TEST_EQUAL(null_term_str(ws), L"Hello world");
    ws = L"Hello\0world"s;  TEST_EQUAL(null_term_str(ws), L"Hello");

    TRY(s = quote(""s));                      TEST_EQUAL(s, "\"\""s);
    TRY(s = quote("\"\""s));                  TEST_EQUAL(s, "\"\\\"\\\"\""s);
    TRY(s = quote("Hello world"s));           TEST_EQUAL(s, "\"Hello world\""s);
    TRY(s = quote("\\Hello\\world\\"s));      TEST_EQUAL(s, "\"\\\\Hello\\\\world\\\\\""s);
    TRY(s = quote("\"Hello\" \"world\""s));   TEST_EQUAL(s, "\"\\\"Hello\\\" \\\"world\\\"\""s);
    TRY(s = quote("\t\n\f\r"s));              TEST_EQUAL(s, "\"\\t\\n\\f\\r\""s);
    TRY(s = quote(u8"åß∂ƒ"s));                TEST_EQUAL(s, u8"\"åß∂ƒ\""s);
    TRY(s = quote("\x00\x01\x7f\x80\xff"s));  TEST_EQUAL(s, "\"\\0\\x01\\x7f\\x80\\xff\""s);

    TRY(s = bquote(""s));                      TEST_EQUAL(s, "\"\""s);
    TRY(s = bquote("\"\""s));                  TEST_EQUAL(s, "\"\\\"\\\"\""s);
    TRY(s = bquote("Hello world"s));           TEST_EQUAL(s, "\"Hello world\""s);
    TRY(s = bquote("\\Hello\\world\\"s));      TEST_EQUAL(s, "\"\\\\Hello\\\\world\\\\\""s);
    TRY(s = bquote("\"Hello\" \"world\""s));   TEST_EQUAL(s, "\"\\\"Hello\\\" \\\"world\\\"\""s);
    TRY(s = bquote("\t\n\f\r"s));              TEST_EQUAL(s, "\"\\t\\n\\f\\r\""s);
    TRY(s = bquote(u8"åß∂ƒ"s));                TEST_EQUAL(s, "\"\\xc3\\xa5\\xc3\\x9f\\xe2\\x88\\x82\\xc6\\x92\""s);
    TRY(s = bquote("\x00\x01\x7f\x80\xff"s));  TEST_EQUAL(s, "\"\\0\\x01\\x7f\\x80\\xff\""s);

    TRY(s = repeat("", 0));              TEST_EQUAL(s, "");
    TRY(s = repeat("", 1));              TEST_EQUAL(s, "");
    TRY(s = repeat("", 2));              TEST_EQUAL(s, "");
    TRY(s = repeat("Hello", 0));         TEST_EQUAL(s, "");
    TRY(s = repeat("Hello", 1));         TEST_EQUAL(s, "Hello");
    TRY(s = repeat("Hello", 2));         TEST_EQUAL(s, "HelloHello");
    TRY(s = repeat("Hello", 3));         TEST_EQUAL(s, "HelloHelloHello");
    TRY(s = repeat("Hello", 4));         TEST_EQUAL(s, "HelloHelloHelloHello");
    TRY(s = repeat("Hello", 5));         TEST_EQUAL(s, "HelloHelloHelloHelloHello");
    TRY(s = repeat("Hello", 6));         TEST_EQUAL(s, "HelloHelloHelloHelloHelloHello");
    TRY(s = repeat("Hello", 7));         TEST_EQUAL(s, "HelloHelloHelloHelloHelloHelloHello");
    TRY(s = repeat("Hello", 8));         TEST_EQUAL(s, "HelloHelloHelloHelloHelloHelloHelloHello");
    TRY(s = repeat("Hello", 9));         TEST_EQUAL(s, "HelloHelloHelloHelloHelloHelloHelloHelloHello");
    TRY(s = repeat("Hello", 10));        TEST_EQUAL(s, "HelloHelloHelloHelloHelloHelloHelloHelloHelloHello");
    TRY(s = repeat("", 0, "<>"));        TEST_EQUAL(s, "");
    TRY(s = repeat("", 1, "<>"));        TEST_EQUAL(s, "");
    TRY(s = repeat("", 2, "<>"));        TEST_EQUAL(s, "<>");
    TRY(s = repeat("Hello", 0, "<>"));   TEST_EQUAL(s, "");
    TRY(s = repeat("Hello", 1, "<>"));   TEST_EQUAL(s, "Hello");
    TRY(s = repeat("Hello", 2, "<>"));   TEST_EQUAL(s, "Hello<>Hello");
    TRY(s = repeat("Hello", 3, "<>"));   TEST_EQUAL(s, "Hello<>Hello<>Hello");
    TRY(s = repeat("Hello", 4, "<>"));   TEST_EQUAL(s, "Hello<>Hello<>Hello<>Hello");
    TRY(s = repeat("Hello", 5, "<>"));   TEST_EQUAL(s, "Hello<>Hello<>Hello<>Hello<>Hello");
    TRY(s = repeat("Hello", 6, "<>"));   TEST_EQUAL(s, "Hello<>Hello<>Hello<>Hello<>Hello<>Hello");
    TRY(s = repeat("Hello", 7, "<>"));   TEST_EQUAL(s, "Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello");
    TRY(s = repeat("Hello", 8, "<>"));   TEST_EQUAL(s, "Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello");
    TRY(s = repeat("Hello", 9, "<>"));   TEST_EQUAL(s, "Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello");
    TRY(s = repeat("Hello", 10, "<>"));  TEST_EQUAL(s, "Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello<>Hello");

    TRY(s = replace(""s, "", ""));                             TEST_EQUAL(s, "");
    TRY(s = replace(""s, "a", "b"));                           TEST_EQUAL(s, "");
    TRY(s = replace("abcdefabcdefabcdef"s, "abc", "xyz"));     TEST_EQUAL(s, "xyzdefxyzdefxyzdef");
    TRY(s = replace("abcdefabcdefabcdef"s, "def", "xyz"));     TEST_EQUAL(s, "abcxyzabcxyzabcxyz");
    TRY(s = replace("abcdefabcdefabcdef"s, "abc", ""));        TEST_EQUAL(s, "defdefdef");
    TRY(s = replace("abcdefabcdefabcdef"s, "def", ""));        TEST_EQUAL(s, "abcabcabc");
    TRY(s = replace("abcdefabcdefabcdef"s, "abc", "uvwxyz"));  TEST_EQUAL(s, "uvwxyzdefuvwxyzdefuvwxyzdef");
    TRY(s = replace("abcdefabcdefabcdef"s, "def", "uvwxyz"));  TEST_EQUAL(s, "abcuvwxyzabcuvwxyzabcuvwxyz");
    TRY(s = replace("abc,abc,abc"s, "abc", "*", 0));           TEST_EQUAL(s, "abc,abc,abc");
    TRY(s = replace("abc,abc,abc"s, "abc", "*", 1));           TEST_EQUAL(s, "*,abc,abc");
    TRY(s = replace("abc,abc,abc"s, "abc", "*", 2));           TEST_EQUAL(s, "*,*,abc");
    TRY(s = replace("abc,abc,abc"s, "abc", "*", 3));           TEST_EQUAL(s, "*,*,*");
    TRY(s = replace("abc,abc,abc"s, "abc", "*", 4));           TEST_EQUAL(s, "*,*,*");
    TRY(s = replace("abc,abc,abc"s, "abc", "*", 5));           TEST_EQUAL(s, "*,*,*");

    TRY(split("", overwrite(sv)));                       TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split(" ", overwrite(sv)));                      TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split("\n\n", overwrite(sv)));                   TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split("Hello", overwrite(sv)));                  TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello]");
    TRY(split("Hello world", overwrite(sv)));            TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,world]");
    TRY(split("\t Hello \t world \t", overwrite(sv)));   TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,world]");
    TRY(split("", overwrite(sv), "*"));                  TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split(" ", overwrite(sv), "*"));                 TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[ ]");
    TRY(split("**", overwrite(sv), "*"));                TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split("**Hello**world**", overwrite(sv), "*"));  TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,world]");
    TRY(split("*****", overwrite(sv), "@*"));            TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");

    TRY(sv = splitv(""));                       TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv(" "));                      TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv("\n\n"));                   TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv("Hello"));                  TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello]");
    TRY(sv = splitv("Hello world"));            TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,world]");
    TRY(sv = splitv("\t Hello \t world \t"));   TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,world]");
    TRY(sv = splitv("", "*"));                  TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv(" ", "*"));                 TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[ ]");
    TRY(sv = splitv("**", "*"));                TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv("**Hello**world**", "*"));  TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,world]");
    TRY(sv = splitv("*****", "@*"));            TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");

    TRY(split_lines("", overwrite(sv)));                      TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split_lines("\n", overwrite(sv)));                    TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split_lines("\r\n", overwrite(sv)));                  TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(split_lines("\n\n", overwrite(sv)));                  TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[,]");
    TRY(split_lines("\r\n\r\n", overwrite(sv)));              TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[,]");
    TRY(split_lines("Hello\nGoodbye", overwrite(sv)));        TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,Goodbye]");
    TRY(split_lines("Hello\nGoodbye\n", overwrite(sv)));      TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,Goodbye]");
    TRY(split_lines("Hello\r\nGoodbye\r\n", overwrite(sv)));  TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,Goodbye]");
    TRY(split_lines("Hello\n\nGoodbye\n\n", overwrite(sv)));  TEST_EQUAL(sv.size(), 4);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,,Goodbye,]");

    TRY(sv = splitv_lines(""));                      TEST_EQUAL(sv.size(), 0);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv_lines("\n"));                    TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv_lines("\r\n"));                  TEST_EQUAL(sv.size(), 1);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[]");
    TRY(sv = splitv_lines("\n\n"));                  TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[,]");
    TRY(sv = splitv_lines("\r\n\r\n"));              TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[,]");
    TRY(sv = splitv_lines("Hello\nGoodbye"));        TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,Goodbye]");
    TRY(sv = splitv_lines("Hello\nGoodbye\n"));      TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,Goodbye]");
    TRY(sv = splitv_lines("Hello\r\nGoodbye\r\n"));  TEST_EQUAL(sv.size(), 2);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,Goodbye]");
    TRY(sv = splitv_lines("Hello\n\nGoodbye\n\n"));  TEST_EQUAL(sv.size(), 4);  TRY(s = to_str(sv));  TEST_EQUAL(s, "[Hello,,Goodbye,]");

    TEST_EQUAL(trim(""), "");
    TEST_EQUAL(trim("Hello"), "Hello");
    TEST_EQUAL(trim("Hello world"), "Hello world");
    TEST_EQUAL(trim("", ""), "");
    TEST_EQUAL(trim("Hello", ""), "Hello");
    TEST_EQUAL(trim("<<<>>>", "<>"), "");
    TEST_EQUAL(trim("<<<Hello>>>", "<>"), "Hello");
    TEST_EQUAL(trim("<<<Hello>>> <<<world>>>", "<>"), "Hello>>> <<<world");

    TEST_EQUAL(trim_left(""), "");
    TEST_EQUAL(trim_left("Hello"), "Hello");
    TEST_EQUAL(trim_left("Hello world"), "Hello world");
    TEST_EQUAL(trim_left("", ""), "");
    TEST_EQUAL(trim_left("Hello", ""), "Hello");
    TEST_EQUAL(trim_left("<<<>>>", "<>"), "");
    TEST_EQUAL(trim_left("<<<Hello>>>", "<>"), "Hello>>>");
    TEST_EQUAL(trim_left("<<<Hello>>> <<<world>>>", "<>"), "Hello>>> <<<world>>>");

    TEST_EQUAL(trim_right(""), "");
    TEST_EQUAL(trim_right("Hello"), "Hello");
    TEST_EQUAL(trim_right("Hello world"), "Hello world");
    TEST_EQUAL(trim_right("", ""), "");
    TEST_EQUAL(trim_right("Hello", ""), "Hello");
    TEST_EQUAL(trim_right("<<<>>>", "<>"), "");
    TEST_EQUAL(trim_right("<<<Hello>>>", "<>"), "<<<Hello");
    TEST_EQUAL(trim_right("<<<Hello>>> <<<world>>>", "<>"), "<<<Hello>>> <<<world");

    TEST_EQUAL(unqualify(""), "");
    TEST_EQUAL(unqualify("alpha"), "alpha");
    TEST_EQUAL(unqualify("alpha.bravo"), "bravo");
    TEST_EQUAL(unqualify("alpha.bravo.charlie"), "charlie");
    TEST_EQUAL(unqualify("alpha::bravo"), "bravo");
    TEST_EQUAL(unqualify("alpha::bravo::charlie"), "charlie");
    TEST_EQUAL(unqualify("alpha-bravo"), "alpha-bravo");
    TEST_EQUAL(unqualify("alpha-bravo-charlie"), "alpha-bravo-charlie");

}

void test_core_string_formatting() {

    TEST_EQUAL(bin(0, 1), "0");
    TEST_EQUAL(bin(0, 10), "0000000000");
    TEST_EQUAL(bin(42, 1), "101010");
    TEST_EQUAL(bin(42, 5), "101010");
    TEST_EQUAL(bin(42, 10), "0000101010");
    TEST_EQUAL(bin(0xabcdef, 20), "101010111100110111101111");
    TEST_EQUAL(bin(0xabcdef, 24), "101010111100110111101111");
    TEST_EQUAL(bin(0xabcdef, 28), "0000101010111100110111101111");
    TEST_EQUAL(bin(int8_t(0)), "00000000");
    TEST_EQUAL(bin(int16_t(0)), "0000000000000000");
    TEST_EQUAL(bin(int32_t(0)), "00000000000000000000000000000000");
    TEST_EQUAL(bin(int8_t(42)), "00101010");
    TEST_EQUAL(bin(int16_t(42)), "0000000000101010");
    TEST_EQUAL(bin(int32_t(42)), "00000000000000000000000000101010");
    TEST_EQUAL(bin(int8_t(-42)), "-00101010");
    TEST_EQUAL(bin(int16_t(-42)), "-0000000000101010");
    TEST_EQUAL(bin(int32_t(-42)), "-00000000000000000000000000101010");
    TEST_EQUAL(bin(uint32_t(0xabcdef)), "00000000101010111100110111101111");
    TEST_EQUAL(bin(uint64_t(0x123456789abcdefull)), "0000000100100011010001010110011110001001101010111100110111101111");

    TEST_EQUAL(dec(0), "0");
    TEST_EQUAL(dec(42), "42");
    TEST_EQUAL(dec(-42), "-42");
    TEST_EQUAL(dec(0, 4), "0000");
    TEST_EQUAL(dec(42, 4), "0042");
    TEST_EQUAL(dec(-42, 4), "-0042");

    TEST_EQUAL(hex(0, 1), "0");
    TEST_EQUAL(hex(0, 2), "00");
    TEST_EQUAL(hex(42, 1), "2a");
    TEST_EQUAL(hex(42, 2), "2a");
    TEST_EQUAL(hex(42, 3), "02a");
    TEST_EQUAL(hex(0xabcdef, 3), "abcdef");
    TEST_EQUAL(hex(0xabcdef, 6), "abcdef");
    TEST_EQUAL(hex(0xabcdef, 9), "000abcdef");
    TEST_EQUAL(hex(int8_t(0)), "00");
    TEST_EQUAL(hex(int16_t(0)), "0000");
    TEST_EQUAL(hex(int32_t(0)), "00000000");
    TEST_EQUAL(hex(int8_t(42)), "2a");
    TEST_EQUAL(hex(int16_t(42)), "002a");
    TEST_EQUAL(hex(int32_t(42)), "0000002a");
    TEST_EQUAL(hex(int8_t(-42)), "-2a");
    TEST_EQUAL(hex(int16_t(-42)), "-002a");
    TEST_EQUAL(hex(int32_t(-42)), "-0000002a");
    TEST_EQUAL(hex(uint32_t(0xabcdef)), "00abcdef");
    TEST_EQUAL(hex(uint64_t(0x123456789abcdefull)), "0123456789abcdef");

    TEST_EQUAL(fp_format(0), "0");
    TEST_EQUAL(fp_format(0, 'e', 3), "0.000e0");
    TEST_EQUAL(fp_format(42, 'e', 3), "4.200e1");
    TEST_EQUAL(fp_format(-42, 'e', 3), "-4.200e1");
    TEST_EQUAL(fp_format(1.23456e8, 'e', 3), "1.235e8");
    TEST_EQUAL(fp_format(1.23456e-6, 'e', 3), "1.235e-6");
    TEST_EQUAL(fp_format(0, 'f', 3), "0.000");
    TEST_EQUAL(fp_format(42, 'f', 3), "42.000");
    TEST_EQUAL(fp_format(-42, 'f', 3), "-42.000");
    TEST_EQUAL(fp_format(1.23456e8, 'f', 3), "123456000.000");
    TEST_EQUAL(fp_format(1.23456e-6, 'f', 3), "0.000");
    TEST_EQUAL(fp_format(0, 'g', 3), "0");
    TEST_EQUAL(fp_format(42, 'g', 3), "42");
    TEST_EQUAL(fp_format(-42, 'g', 3), "-42");
    TEST_EQUAL(fp_format(1.23456e8, 'g', 3), "1.23e8");
    TEST_EQUAL(fp_format(1.23456e-6, 'g', 3), "1.23e-6");
    TEST_EQUAL(fp_format(0, 'z', 3), "0.00");
    TEST_EQUAL(fp_format(42, 'z', 3), "42.0");
    TEST_EQUAL(fp_format(-42, 'z', 3), "-42.0");
    TEST_EQUAL(fp_format(1.23456e8, 'z', 3), "1.23e8");
    TEST_EQUAL(fp_format(1.23456e-6, 'z', 3), "1.23e-6");
    TEST_EQUAL(fp_format(0.123, 'z', 3), "0.123");
    TEST_EQUAL(fp_format(1.23, 'z', 3), "1.23");
    TEST_EQUAL(fp_format(12.3, 'z', 3), "12.3");
    TEST_EQUAL(fp_format(123, 'z', 3), "123");

    TEST_EQUAL(roman(0), "0");
    TEST_EQUAL(roman(1), "I");
    TEST_EQUAL(roman(42), "XLII");
    TEST_EQUAL(roman(1111), "MCXI");
    TEST_EQUAL(roman(2222), "MMCCXXII");
    TEST_EQUAL(roman(3333), "MMMCCCXXXIII");
    TEST_EQUAL(roman(4444), "MMMMCDXLIV");
    TEST_EQUAL(roman(5555), "MMMMMDLV");
    TEST_EQUAL(roman(6666), "MMMMMMDCLXVI");
    TEST_EQUAL(roman(7777), "MMMMMMMDCCLXXVII");
    TEST_EQUAL(roman(8888), "MMMMMMMMDCCCLXXXVIII");
    TEST_EQUAL(roman(9999), "MMMMMMMMMCMXCIX");

    TEST_EQUAL(hexdump(""s), "");
    TEST_EQUAL(hexdump(""s, 5), "");
    TEST_EQUAL(hexdump("Hello world!"s), "48 65 6c 6c 6f 20 77 6f 72 6c 64 21");
    TEST_EQUAL(hexdump("Hello world!"s, 5), "48 65 6c 6c 6f\n20 77 6f 72 6c\n64 21\n");
    TEST_EQUAL(hexdump("Hello world!"s, 6), "48 65 6c 6c 6f 20\n77 6f 72 6c 64 21\n");

    TEST_EQUAL(tf(true), "true");
    TEST_EQUAL(tf(false), "false");
    TEST_EQUAL(yn(true), "yes");
    TEST_EQUAL(yn(false), "no");

    std::atomic<int> ai(42);

    TEST_EQUAL(to_str(0), "0");
    TEST_EQUAL(to_str(42), "42");
    TEST_EQUAL(to_str(-42), "-42");
    TEST_EQUAL(to_str(123.456), "123.456");
    TEST_EQUAL(to_str(true), "true");
    TEST_EQUAL(to_str(false), "false");
    TEST_EQUAL(to_str(std::make_pair(10,20)), "{10,20}");
    TEST_EQUAL(to_str(std::make_pair("hello"s,"world"s)), "{hello,world}");
    TEST_EQUAL(to_str(ai), "42");

    std::string s = "Hello";
    std::u16string s16 = u"Hello";
    std::u32string s32 = U"Hello";
    std::wstring ws = L"Hello";

    TEST_EQUAL(to_str(s), "Hello");
    TEST_EQUAL(to_str(s.data()), "Hello");
    TEST_EQUAL(to_str(""s), "");
    TEST_EQUAL(to_str("Hello"s), "Hello");
    TEST_EQUAL(to_str('X'), "X");

    TEST_EQUAL(to_str(s16), "Hello");
    TEST_EQUAL(to_str(s16.data()), "Hello");
    TEST_EQUAL(to_str(u""s), "");
    TEST_EQUAL(to_str(u"Hello"s), "Hello");
    TEST_EQUAL(to_str(u'X'), "X");

    TEST_EQUAL(to_str(s32), "Hello");
    TEST_EQUAL(to_str(s32.data()), "Hello");
    TEST_EQUAL(to_str(U""s), "");
    TEST_EQUAL(to_str(U"Hello"s), "Hello");
    TEST_EQUAL(to_str(U'X'), "X");

    TEST_EQUAL(to_str(ws), "Hello");
    TEST_EQUAL(to_str(ws.data()), "Hello");
    TEST_EQUAL(to_str(L""s), "");
    TEST_EQUAL(to_str(L"Hello"s), "Hello");
    TEST_EQUAL(to_str(L'X'), "X");

    std::vector<bool> bv;
    std::vector<int> iv;
    Strings sv;
    std::map<int, std::string> ism;

    TEST_EQUAL(to_str(bv), "[]");
    TEST_EQUAL(to_str(iv), "[]");
    TEST_EQUAL(to_str(sv), "[]");
    TEST_EQUAL(to_str(ism), "{}");

    bv = {true,false};
    iv = {1,2,3};
    sv = {"hello","world","goodbye"};
    ism = {{1,"hello"},{2,"world"},{3,"goodbye"}};

    TEST_EQUAL(to_str(bv), "[true,false]");
    TEST_EQUAL(to_str(iv), "[1,2,3]");
    TEST_EQUAL(to_str(sv), "[hello,world,goodbye]");
    TEST_EQUAL(to_str(ism), "{1:hello,2:world,3:goodbye}");

    TRY(s = fmt(""));                               TEST_EQUAL(s, "");
    TRY(s = fmt("Hello world"));                    TEST_EQUAL(s, "Hello world");
    TRY(s = fmt("Hello $1"));                       TEST_EQUAL(s, "Hello ");
    TRY(s = fmt("Hello $1", "world"s));             TEST_EQUAL(s, "Hello world");
    TRY(s = fmt("Hello $1", 42));                   TEST_EQUAL(s, "Hello 42");
    TRY(s = fmt("($1) ($2) ($3)", 10, 20, 30));     TEST_EQUAL(s, "(10) (20) (30)");
    TRY(s = fmt("${1} ${2} ${3}", 10, 20, 30));     TEST_EQUAL(s, "10 20 30");
    TRY(s = fmt("$3,$3,$2,$2,$1,$1", 10, 20, 30));  TEST_EQUAL(s, "30,30,20,20,10,10");
    TRY(s = fmt("Hello $1 $$ ${}", 42));            TEST_EQUAL(s, "Hello 42 $ {}");

}

void test_core_string_parsing() {

    TEST_EQUAL(binnum(""), 0);
    TEST_EQUAL(binnum("0"), 0);
    TEST_EQUAL(binnum("101010"), 42);
    TEST_EQUAL(binnum("000000101010"), 42);
    TEST_EQUAL(binnum("1111111111111111111111111111111111111111111111111111111111111110"), 0xfffffffffffffffeull);
    TEST_EQUAL(binnum("1111111111111111111111111111111111111111111111111111111111111111"), 0xffffffffffffffffull);
    TEST_EQUAL(binnum("10000000000000000000000000000000000000000000000000000000000000000"), 0xffffffffffffffffull);

    TEST_EQUAL(decnum(""), 0);
    TEST_EQUAL(decnum("0"), 0);
    TEST_EQUAL(decnum("42"), 42);
    TEST_EQUAL(decnum("0042"), 42);
    TEST_EQUAL(decnum("+42"), 42);
    TEST_EQUAL(decnum("-42"), -42);
    TEST_EQUAL(decnum("9223372036854775806"), 9'223'372'036'854'775'806ll); // 2^63-2
    TEST_EQUAL(decnum("9223372036854775807"), 9'223'372'036'854'775'807ll); // 2^63-1
    TEST_EQUAL(decnum("9223372036854775808"), 9'223'372'036'854'775'807ll); // 2^63
    TEST_EQUAL(decnum("9223372036854775809"), 9'223'372'036'854'775'807ll); // 2^63+1
    TEST_EQUAL(decnum("-9223372036854775806"), -9'223'372'036'854'775'806ll); // -(2^63-2)
    TEST_EQUAL(decnum("-9223372036854775807"), -9'223'372'036'854'775'807ll); // -(2^63-1)
    TEST_EQUAL(decnum("-9223372036854775808"), -9'223'372'036'854'775'807ll-1); // -2^63
    TEST_EQUAL(decnum("-9223372036854775809"), -9'223'372'036'854'775'807ll-1); // -(2^63+1)

    TEST_EQUAL(hexnum(""), 0);
    TEST_EQUAL(hexnum("0"), 0);
    TEST_EQUAL(hexnum("42"), 66);
    TEST_EQUAL(hexnum("0042"), 66);
    TEST_EQUAL(hexnum("fffffffffffffffe"), 0xfffffffffffffffeull);
    TEST_EQUAL(hexnum("ffffffffffffffff"), 0xffffffffffffffffull);
    TEST_EQUAL(hexnum("10000000000000000"), 0xffffffffffffffffull);

    TEST_EQUAL(fpnum(""), 0);
    TEST_EQUAL(fpnum("42"), 42);
    TEST_EQUAL(fpnum("-42"), -42);
    TEST_EQUAL(fpnum("1.234e5"), 123400);
    TEST_EQUAL(fpnum("-1.234e5"), -123400);
    TEST_NEAR(fpnum("1.23e-4"), 0.000123);
    TEST_NEAR(fpnum("-1.23e-4"), -0.000123);
    TEST_EQUAL(fpnum("1e9999"), HUGE_VAL);
    TEST_EQUAL(fpnum("-1e9999"), - HUGE_VAL);

    TEST_EQUAL(si_to_int("0"), 0);
    TEST_EQUAL(si_to_int("42"), 42);
    TEST_EQUAL(si_to_int("-42"), -42);
    TEST_EQUAL(si_to_int("0k"), 0);
    TEST_EQUAL(si_to_int("123k"), 123'000);
    TEST_EQUAL(si_to_int("123K"), 123'000);
    TEST_EQUAL(si_to_int("123 k"), 123'000);
    TEST_EQUAL(si_to_int("123 K"), 123'000);
    TEST_EQUAL(si_to_int("123 M"), 123'000'000ll);
    TEST_EQUAL(si_to_int("123 G"), 123'000'000'000ll);
    TEST_EQUAL(si_to_int("123 T"), 123'000'000'000'000ll);
    TEST_EQUAL(si_to_int("123 P"), 123'000'000'000'000'000ll);
    TEST_THROW(si_to_int("123 E"), std::range_error);
    TEST_THROW(si_to_int("123 Z"), std::range_error);
    TEST_THROW(si_to_int("123 Y"), std::range_error);
    TEST_EQUAL(si_to_int("-123k"), -123'000);
    TEST_EQUAL(si_to_int("-123K"), -123'000);
    TEST_EQUAL(si_to_int("-123 k"), -123'000);
    TEST_EQUAL(si_to_int("-123 K"), -123'000);
    TEST_THROW(si_to_int(""), std::invalid_argument);
    TEST_THROW(si_to_int("k9"), std::invalid_argument);

    TEST_EQUAL(si_to_float("0"), 0);
    TEST_EQUAL(si_to_float("42"), 42);
    TEST_EQUAL(si_to_float("-42"), -42);
    TEST_EQUAL(si_to_float("1234.5"), 1234.5);
    TEST_EQUAL(si_to_float("-1234.5"), -1234.5);
    TEST_EQUAL(si_to_float("0k"), 0);
    TEST_EQUAL(si_to_float("12.34k"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34K"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34 k"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34 K"), 12.34e3);
    TEST_EQUAL(si_to_float("12.34 M"), 12.34e6);
    TEST_EQUAL(si_to_float("12.34 G"), 12.34e9);
    TEST_EQUAL(si_to_float("12.34 T"), 12.34e12);
    TEST_EQUAL(si_to_float("12.34 P"), 12.34e15);
    TEST_EQUAL(si_to_float("12.34 E"), 12.34e18);
    TEST_EQUAL(si_to_float("12.34 Z"), 12.34e21);
    TEST_EQUAL(si_to_float("12.34 Y"), 12.34e24);
    TEST_NEAR_EPSILON(si_to_float("12.34 m"), 12.34e-3, 1e-9);
    TEST_NEAR_EPSILON(si_to_float("12.34 u"), 12.34e-6, 1e-12);
    TEST_NEAR_EPSILON(si_to_float("12.34 n"), 12.34e-9, 1e-15);
    TEST_NEAR_EPSILON(si_to_float("12.34 p"), 12.34e-12, 1e-18);
    TEST_NEAR_EPSILON(si_to_float("12.34 f"), 12.34e-15, 1e-21);
    TEST_NEAR_EPSILON(si_to_float("12.34 a"), 12.34e-18, 1e-24);
    TEST_NEAR_EPSILON(si_to_float("12.34 z"), 12.34e-21, 1e-27);
    TEST_NEAR_EPSILON(si_to_float("12.34 y"), 12.34e-24, 1e-30);
    TEST_EQUAL(si_to_float("-12.34k"), -12.34e3);
    TEST_EQUAL(si_to_float("-12.34K"), -12.34e3);
    TEST_EQUAL(si_to_float("-12.34 k"), -12.34e3);
    TEST_EQUAL(si_to_float("-12.34 K"), -12.34e3);
    TEST_THROW(si_to_float("1e999999"), std::range_error);
    TEST_THROW(si_to_float(""), std::invalid_argument);
    TEST_THROW(si_to_float("k9"), std::invalid_argument);

}

void test_core_string_html_xml_tags() {

    const std::string expected =
        "<h1>Header</h1>\n"
        "<br/>\n"
        "<ul>\n"
        "<li><code>alpha</code></li>\n"
        "<li><code>bravo</code></li>\n"
        "<li><code>charlie</code></li>\n"
        "</ul>\n";

    {
        std::ostringstream out;
        {
            Tag h1;
            TRY(h1 = Tag(out, "<h1>\n"));
            out << "Header";
        }
        Tag br;
        TRY(br = Tag(out, "<br/>\n"));
        {
            Tag ul;
            TRY(ul = Tag(out, "<ul>\n\n"));
            for (auto item: {"alpha", "bravo", "charlie"}) {
                Tag li, code;
                TRY(li = Tag(out, "<li>\n"));
                TRY(code = Tag(out, "<code>"));
                out << item;
            }
        }
        std::string s = out.str();
        TEST_EQUAL(s, expected);
    }

    {
        std::ostringstream out;
        {
            Tag h1(out, "h1\n");
            out << "Header";
        }
        Tag br(out, "br/\n");
        {
            Tag ul(out, "ul\n\n");
            for (auto item: {"alpha", "bravo", "charlie"}) {
                Tag li(out, "li\n");
                Tag code(out, "code");
                out << item;
            }
        }
        std::string s = out.str();
        TEST_EQUAL(s, expected);
    }

    {
        std::ostringstream out;
        tagged(out, "h1\n", "Header");
        Tag br(out, "br/\n");
        {
            Tag ul(out, "ul\n\n");
            for (auto item: {"alpha", "bravo", "charlie"})
                tagged(out, "li\n", "code", item);
        }
        std::string s = out.str();
        TEST_EQUAL(s, expected);
    }

}

void test_core_string_type_names() {

    U8string s;

    const std::type_info& v_info = typeid(void);
    const std::type_info& i_info = typeid(int);
    const std::type_info& s_info = typeid(std::string);
    auto v_index = std::type_index(typeid(void));
    auto i_index = std::type_index(typeid(int));
    auto s_index = std::type_index(typeid(std::string));

    TEST_EQUAL(type_name(v_info), "void");
    TEST_MATCH(type_name(i_info), "^(signed )?int$");
    TEST_MATCH(type_name(s_info), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");
    TEST_EQUAL(type_name(v_index), "void");
    TEST_MATCH(type_name(i_index), "^(signed )?int$");
    TEST_MATCH(type_name(s_index), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");
    TEST_EQUAL(type_name<void>(), "void");
    TEST_MATCH(type_name<int>(), "^(signed )?int$");
    TEST_MATCH(type_name<std::string>(), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");
    TEST_MATCH(type_name(42), "^(signed )?int$");
    TEST_MATCH(type_name(s), "^(class )?std::([^:]+::)*(string|basic_string ?<.+>)$");

    Derived d;
    Base& b(d);
    const std::type_info& d_info = typeid(d);
    const std::type_info& b_info = typeid(b);
    auto d_index = std::type_index(typeid(d));
    auto b_index = std::type_index(typeid(b));

    TEST_MATCH(type_name(d), "::Derived$");
    TEST_MATCH(type_name(b), "::Derived$");
    TEST_MATCH(type_name(d_info), "::Derived$");
    TEST_MATCH(type_name(b_info), "::Derived$");
    TEST_MATCH(type_name(d_index), "::Derived$");
    TEST_MATCH(type_name(b_index), "::Derived$");

}

void test_core_string_unicode() {

    // UTF-32    UTF-16     UTF-8
    // 0000004d  004d       4d
    // 00000430  0430       d0 b0
    // 00004e8c  4e8c       e4 ba 8c
    // 00010302  d800 df02  f0 90 8c 82
    // 0010fffd  dbff dffd  f4 8f bf bd

    U8string s8, t8 = "\x4d\xd0\xb0\xe4\xba\x8c\xf0\x90\x8c\x82\xf4\x8f\xbf\xbd";
    std::u16string s16, t16 = {0x4d,0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
    std::u32string s32, t32 = {0x4d,0x430,0x4e8c,0x10302,0x10fffd};
    std::wstring sw, tw =
        #if WCHAR_MAX > 0xffff
            {0x4d,0x430,0x4e8c,0x10302,0x10fffd};
        #else
            {0x4d,0x430,0x4e8c,0xd800,0xdf02,0xdbff,0xdffd};
        #endif
    size_t n = 0;

    TRY(s8 = uconv<U8string>(""s));          TEST_EQUAL(s8, ""s);
    TRY(s8 = uconv<U8string>(u""s));         TEST_EQUAL(s8, ""s);
    TRY(s8 = uconv<U8string>(U""s));         TEST_EQUAL(s8, ""s);
    TRY(s8 = uconv<U8string>(L""s));         TEST_EQUAL(s8, ""s);
    TRY(s16 = uconv<std::u16string>(""s));   TEST_EQUAL(s16, u""s);
    TRY(s16 = uconv<std::u16string>(u""s));  TEST_EQUAL(s16, u""s);
    TRY(s16 = uconv<std::u16string>(U""s));  TEST_EQUAL(s16, u""s);
    TRY(s16 = uconv<std::u16string>(L""s));  TEST_EQUAL(s16, u""s);
    TRY(s32 = uconv<std::u32string>(""s));   TEST_EQUAL(s32, U""s);
    TRY(s32 = uconv<std::u32string>(u""s));  TEST_EQUAL(s32, U""s);
    TRY(s32 = uconv<std::u32string>(U""s));  TEST_EQUAL(s32, U""s);
    TRY(s32 = uconv<std::u32string>(L""s));  TEST_EQUAL(s32, U""s);
    TRY(sw = uconv<std::wstring>(""s));      TEST_EQUAL(sw, L""s);
    TRY(sw = uconv<std::wstring>(u""s));     TEST_EQUAL(sw, L""s);
    TRY(sw = uconv<std::wstring>(U""s));     TEST_EQUAL(sw, L""s);
    TRY(sw = uconv<std::wstring>(L""s));     TEST_EQUAL(sw, L""s);
    TRY(s8 = uconv<U8string>(t8));           TEST_EQUAL(s8, t8);
    TRY(s8 = uconv<U8string>(t16));          TEST_EQUAL(s8, t8);
    TRY(s8 = uconv<U8string>(t32));          TEST_EQUAL(s8, t8);
    TRY(s8 = uconv<U8string>(tw));           TEST_EQUAL(s8, t8);
    TRY(s16 = uconv<std::u16string>(t8));    TEST_EQUAL(s16, t16);
    TRY(s16 = uconv<std::u16string>(t16));   TEST_EQUAL(s16, t16);
    TRY(s16 = uconv<std::u16string>(t32));   TEST_EQUAL(s16, t16);
    TRY(s16 = uconv<std::u16string>(tw));    TEST_EQUAL(s16, t16);
    TRY(s32 = uconv<std::u32string>(t8));    TEST_EQUAL(s32, t32);
    TRY(s32 = uconv<std::u32string>(t16));   TEST_EQUAL(s32, t32);
    TRY(s32 = uconv<std::u32string>(t32));   TEST_EQUAL(s32, t32);
    TRY(s32 = uconv<std::u32string>(tw));    TEST_EQUAL(s32, t32);
    TRY(sw = uconv<std::wstring>(t8));       TEST_EQUAL(sw, tw);
    TRY(sw = uconv<std::wstring>(t16));      TEST_EQUAL(sw, tw);
    TRY(sw = uconv<std::wstring>(t32));      TEST_EQUAL(sw, tw);
    TRY(sw = uconv<std::wstring>(tw));       TEST_EQUAL(sw, tw);

    TRY(n = ulength(u8""s));       TEST_EQUAL(n, 0);
    TRY(n = ulength(u8"hello"s));  TEST_EQUAL(n, 5);
    TRY(n = ulength(u8"αβγδε"s));  TEST_EQUAL(n, 5);
    TRY(n = ulength(u""s));        TEST_EQUAL(n, 0);
    TRY(n = ulength(u"hello"s));   TEST_EQUAL(n, 5);
    TRY(n = ulength(u"αβγδε"s));   TEST_EQUAL(n, 5);
    TRY(n = ulength(U""s));        TEST_EQUAL(n, 0);
    TRY(n = ulength(U"hello"s));   TEST_EQUAL(n, 5);
    TRY(n = ulength(U"αβγδε"s));   TEST_EQUAL(n, 5);
    TRY(n = ulength(L""s));        TEST_EQUAL(n, 0);
    TRY(n = ulength(L"hello"s));   TEST_EQUAL(n, 5);
    TRY(n = ulength(L"αβγδε"s));   TEST_EQUAL(n, 5);

    TEST(uvalid(""s, n));                       TEST_EQUAL(n, 0);
    TEST(uvalid(u""s, n));                      TEST_EQUAL(n, 0);
    TEST(uvalid(U""s, n));                      TEST_EQUAL(n, 0);
    TEST(uvalid(L""s, n));                      TEST_EQUAL(n, 0);
    TEST(uvalid(t8, n));                        TEST_EQUAL(n, t8.size());
    TEST(uvalid(t16, n));                       TEST_EQUAL(n, t16.size());
    TEST(uvalid(t32, n));                       TEST_EQUAL(n, t32.size());
    TEST(uvalid(tw, n));                        TEST_EQUAL(n, tw.size());
    TEST(! uvalid(t8 + '\xff', n));             TEST_EQUAL(n, t8.size());
    TEST(! uvalid(t16 + char16_t(0xd800), n));  TEST_EQUAL(n, t16.size());
    TEST(! uvalid(t32 + char32_t(0xd800), n));  TEST_EQUAL(n, t32.size());
    TEST(! uvalid(tw + wchar_t(0xd800), n));    TEST_EQUAL(n, tw.size());

}
