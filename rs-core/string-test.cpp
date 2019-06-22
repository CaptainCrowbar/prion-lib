#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <array>
#include <sstream>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

using namespace RS;
using namespace std::literals;

void test_core_string_literals() {

    using namespace RS::Literals;

    Ustring s;
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

    TRY(s = R"(Hello world.)"_doc);
    TEST_EQUAL(s, "Hello world.\n");

    TRY(s = R"(
        Hello world.
        )"_doc);
    TEST_EQUAL(s, "Hello world.\n");

    TRY(s = R"(

        Hello world.

            Hello again.

        Goodbye.

        )"_doc);
    TEST_EQUAL(s,
        "\n"
        "Hello world.\n"
        "\n"
        "    Hello again.\n"
        "\n"
        "Goodbye.\n"
        "\n"
    );

    TRY(s =
        "\t\tHello world.\n"
        "\t\t\tHello again.\n"
        "\t\t\t\tGoodbye.\n"
        "\t\t"_doc);
    TEST_EQUAL(s,
        "Hello world.\n"
        "\tHello again.\n"
        "\t\tGoodbye.\n"
    );
}

void test_core_string_case_conversion() {

    static const Ustring s_camel     = "alphaBravoCharlie12345";
    static const Ustring s_fold      = "alpha bravo charlie 12345";
    static const Ustring s_initial   = "ABC1";
    static const Ustring s_kebab     = "alpha-bravo-charlie-12345";
    static const Ustring s_lower     = "alpha bravo charlie 12345";
    static const Ustring s_macro     = "ALPHA_BRAVO_CHARLIE_12345";
    static const Ustring s_mixed_1   = "... ALPHA ... bravo ... charlie ... 12345 ...";
    static const Ustring s_mixed_2   = "ALPHABravo/Charlie12345";
    static const Ustring s_pascal    = "AlphaBravoCharlie12345";
    static const Ustring s_sentence  = "Alpha bravo charlie 12345";
    static const Ustring s_snake     = "alpha_bravo_charlie_12345";
    static const Ustring s_title     = "Alpha Bravo Charlie 12345";
    static const Ustring s_upper     = "ALPHA BRAVO CHARLIE 12345";

    CaseWords c, c_camel, c_macro, c_mixed_1, c_mixed_2, c_sentence, c_snake, c_title;
    Ustring str;

    TRY(c += "ABCDefGHI");        TRY(str = to_str(c));  TEST_EQUAL(str, "abc def ghi");
    TRY(c.push_back("JklMNO"));   TRY(str = to_str(c));  TEST_EQUAL(str, "abc def ghi jkl mno");
    TRY(c.push_front("UVWXyz"));  TRY(str = to_str(c));  TEST_EQUAL(str, "uvw xyz abc def ghi jkl mno");

    TRY(c_camel = s_camel);
    TRY(c_macro = s_macro);
    TRY(c_mixed_1 = s_mixed_1);
    TRY(c_mixed_2 = s_mixed_2);
    TRY(c_sentence = s_sentence);
    TRY(c_snake = s_snake);
    TRY(c_title = s_title);

    TEST_EQUAL(c_camel.size(), 4);     TRY(str = to_str(c_camel));     TEST_EQUAL(str, s_fold);
    TEST_EQUAL(c_macro.size(), 4);     TRY(str = to_str(c_macro));     TEST_EQUAL(str, s_fold);
    TEST_EQUAL(c_mixed_1.size(), 4);   TRY(str = to_str(c_mixed_1));   TEST_EQUAL(str, s_fold);
    TEST_EQUAL(c_mixed_2.size(), 4);   TRY(str = to_str(c_mixed_2));   TEST_EQUAL(str, s_fold);
    TEST_EQUAL(c_sentence.size(), 4);  TRY(str = to_str(c_sentence));  TEST_EQUAL(str, s_fold);
    TEST_EQUAL(c_snake.size(), 4);     TRY(str = to_str(c_snake));     TEST_EQUAL(str, s_fold);
    TEST_EQUAL(c_title.size(), 4);     TRY(str = to_str(c_title));     TEST_EQUAL(str, s_fold);

    TRY(str = c_camel.camel_case());        TEST_EQUAL(str, s_camel);
    TRY(str = c_macro.camel_case());        TEST_EQUAL(str, s_camel);
    TRY(str = c_mixed_1.camel_case());      TEST_EQUAL(str, s_camel);
    TRY(str = c_mixed_2.camel_case());      TEST_EQUAL(str, s_camel);
    TRY(str = c_sentence.camel_case());     TEST_EQUAL(str, s_camel);
    TRY(str = c_snake.camel_case());        TEST_EQUAL(str, s_camel);
    TRY(str = c_title.camel_case());        TEST_EQUAL(str, s_camel);
    TRY(str = c_camel.fold_case());         TEST_EQUAL(str, s_fold);
    TRY(str = c_macro.fold_case());         TEST_EQUAL(str, s_fold);
    TRY(str = c_mixed_1.fold_case());       TEST_EQUAL(str, s_fold);
    TRY(str = c_mixed_2.fold_case());       TEST_EQUAL(str, s_fold);
    TRY(str = c_sentence.fold_case());      TEST_EQUAL(str, s_fold);
    TRY(str = c_snake.fold_case());         TEST_EQUAL(str, s_fold);
    TRY(str = c_title.fold_case());         TEST_EQUAL(str, s_fold);
    TRY(str = c_camel.initials());          TEST_EQUAL(str, s_initial);
    TRY(str = c_macro.initials());          TEST_EQUAL(str, s_initial);
    TRY(str = c_mixed_1.initials());        TEST_EQUAL(str, s_initial);
    TRY(str = c_mixed_2.initials());        TEST_EQUAL(str, s_initial);
    TRY(str = c_sentence.initials());       TEST_EQUAL(str, s_initial);
    TRY(str = c_snake.initials());          TEST_EQUAL(str, s_initial);
    TRY(str = c_title.initials());          TEST_EQUAL(str, s_initial);
    TRY(str = c_camel.kebab_case());        TEST_EQUAL(str, s_kebab);
    TRY(str = c_macro.kebab_case());        TEST_EQUAL(str, s_kebab);
    TRY(str = c_mixed_1.kebab_case());      TEST_EQUAL(str, s_kebab);
    TRY(str = c_mixed_2.kebab_case());      TEST_EQUAL(str, s_kebab);
    TRY(str = c_sentence.kebab_case());     TEST_EQUAL(str, s_kebab);
    TRY(str = c_snake.kebab_case());        TEST_EQUAL(str, s_kebab);
    TRY(str = c_title.kebab_case());        TEST_EQUAL(str, s_kebab);
    TRY(str = c_camel.lower_case());        TEST_EQUAL(str, s_lower);
    TRY(str = c_macro.lower_case());        TEST_EQUAL(str, s_lower);
    TRY(str = c_mixed_1.lower_case());      TEST_EQUAL(str, s_lower);
    TRY(str = c_mixed_2.lower_case());      TEST_EQUAL(str, s_lower);
    TRY(str = c_sentence.lower_case());     TEST_EQUAL(str, s_lower);
    TRY(str = c_snake.lower_case());        TEST_EQUAL(str, s_lower);
    TRY(str = c_title.lower_case());        TEST_EQUAL(str, s_lower);
    TRY(str = c_camel.macro_case());        TEST_EQUAL(str, s_macro);
    TRY(str = c_macro.macro_case());        TEST_EQUAL(str, s_macro);
    TRY(str = c_mixed_1.macro_case());      TEST_EQUAL(str, s_macro);
    TRY(str = c_mixed_2.macro_case());      TEST_EQUAL(str, s_macro);
    TRY(str = c_sentence.macro_case());     TEST_EQUAL(str, s_macro);
    TRY(str = c_snake.macro_case());        TEST_EQUAL(str, s_macro);
    TRY(str = c_title.macro_case());        TEST_EQUAL(str, s_macro);
    TRY(str = c_camel.pascal_case());       TEST_EQUAL(str, s_pascal);
    TRY(str = c_macro.pascal_case());       TEST_EQUAL(str, s_pascal);
    TRY(str = c_mixed_1.pascal_case());     TEST_EQUAL(str, s_pascal);
    TRY(str = c_mixed_2.pascal_case());     TEST_EQUAL(str, s_pascal);
    TRY(str = c_sentence.pascal_case());    TEST_EQUAL(str, s_pascal);
    TRY(str = c_snake.pascal_case());       TEST_EQUAL(str, s_pascal);
    TRY(str = c_title.pascal_case());       TEST_EQUAL(str, s_pascal);
    TRY(str = c_camel.sentence_case());     TEST_EQUAL(str, s_sentence);
    TRY(str = c_macro.sentence_case());     TEST_EQUAL(str, s_sentence);
    TRY(str = c_mixed_1.sentence_case());   TEST_EQUAL(str, s_sentence);
    TRY(str = c_mixed_2.sentence_case());   TEST_EQUAL(str, s_sentence);
    TRY(str = c_sentence.sentence_case());  TEST_EQUAL(str, s_sentence);
    TRY(str = c_snake.sentence_case());     TEST_EQUAL(str, s_sentence);
    TRY(str = c_title.sentence_case());     TEST_EQUAL(str, s_sentence);
    TRY(str = c_camel.snake_case());        TEST_EQUAL(str, s_snake);
    TRY(str = c_macro.snake_case());        TEST_EQUAL(str, s_snake);
    TRY(str = c_mixed_1.snake_case());      TEST_EQUAL(str, s_snake);
    TRY(str = c_mixed_2.snake_case());      TEST_EQUAL(str, s_snake);
    TRY(str = c_sentence.snake_case());     TEST_EQUAL(str, s_snake);
    TRY(str = c_snake.snake_case());        TEST_EQUAL(str, s_snake);
    TRY(str = c_title.snake_case());        TEST_EQUAL(str, s_snake);
    TRY(str = c_camel.title_case());        TEST_EQUAL(str, s_title);
    TRY(str = c_macro.title_case());        TEST_EQUAL(str, s_title);
    TRY(str = c_mixed_1.title_case());      TEST_EQUAL(str, s_title);
    TRY(str = c_mixed_2.title_case());      TEST_EQUAL(str, s_title);
    TRY(str = c_sentence.title_case());     TEST_EQUAL(str, s_title);
    TRY(str = c_snake.title_case());        TEST_EQUAL(str, s_title);
    TRY(str = c_title.title_case());        TEST_EQUAL(str, s_title);
    TRY(str = c_camel.upper_case());        TEST_EQUAL(str, s_upper);
    TRY(str = c_macro.upper_case());        TEST_EQUAL(str, s_upper);
    TRY(str = c_mixed_1.upper_case());      TEST_EQUAL(str, s_upper);
    TRY(str = c_mixed_2.upper_case());      TEST_EQUAL(str, s_upper);
    TRY(str = c_sentence.upper_case());     TEST_EQUAL(str, s_upper);
    TRY(str = c_snake.upper_case());        TEST_EQUAL(str, s_upper);
    TRY(str = c_title.upper_case());        TEST_EQUAL(str, s_upper);

}

void test_core_string_character() {

    #ifdef _MSC_VER
        #pragma warning(push)
        #pragma warning(disable: 4310) // cast truncates constant value
    #endif

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

    #ifdef _MSC_VER
        #pragma warning(pop)
    #endif

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
    TEST(! string_is_ascii("åß∂"));

}

void test_core_string_manipulation() {

    std::string s, s1, s2;
    std::wstring ws;
    std::string_view sv1, sv2;
    Strings sv;

    s = "";                 add_lf(s);  TEST_EQUAL(s, "");
    s = "\n";               add_lf(s);  TEST_EQUAL(s, "\n");
    s = "Hello world";      add_lf(s);  TEST_EQUAL(s, "Hello world\n");
    s = "Hello world\n";    add_lf(s);  TEST_EQUAL(s, "Hello world\n");
    s = "Hello world\n\n";  add_lf(s);  TEST_EQUAL(s, "Hello world\n\n");

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

    s1 = "Hello";
    s2 = "world";

    TRY(s = catstr());                           TEST_EQUAL(s, "");
    TRY(s = catstr('x'));                        TEST_EQUAL(s, "x");
    TRY(s = catstr("Hello"));                    TEST_EQUAL(s, "Hello");
    TRY(s = catstr(s1));                         TEST_EQUAL(s, "Hello");
    TRY(s = catstr(s1, ' ', s2, "!\n"));         TEST_EQUAL(s, "Hello world!\n");
    TRY(s = catstr(s1, ' ', 42, ' ', s2, 's'));  TEST_EQUAL(s, "Hello 42 worlds");

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

    s = ""s;                TRY(null_term(s));   TEST_EQUAL(s, "");
    s = "Hello world"s;     TRY(null_term(s));   TEST_EQUAL(s, "Hello world");
    s = "Hello\0world"s;    TRY(null_term(s));   TEST_EQUAL(s, "Hello");
    ws = L""s;              TRY(null_term(ws));  TEST_EQUAL(ws, L"");
    ws = L"Hello world"s;   TRY(null_term(ws));  TEST_EQUAL(ws, L"Hello world");
    ws = L"Hello\0world"s;  TRY(null_term(ws));  TEST_EQUAL(ws, L"Hello");

    s1 = "";       TRY(s2 = pad_left(s1, 3));       TEST_EQUAL(s2, "   ");
    s1 = "Hello";  TRY(s2 = pad_left(s1, 3));       TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_left(s1, 5));       TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_left(s1, 7));       TEST_EQUAL(s2, "  Hello");
    s1 = "";       TRY(s2 = pad_left(s1, 3, '*'));  TEST_EQUAL(s2, "***");
    s1 = "Hello";  TRY(s2 = pad_left(s1, 3, '*'));  TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_left(s1, 5, '*'));  TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_left(s1, 7, '*'));  TEST_EQUAL(s2, "**Hello");

    s1 = "";       TRY(s2 = pad_right(s1, 3));       TEST_EQUAL(s2, "   ");
    s1 = "Hello";  TRY(s2 = pad_right(s1, 3));       TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_right(s1, 5));       TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_right(s1, 7));       TEST_EQUAL(s2, "Hello  ");
    s1 = "";       TRY(s2 = pad_right(s1, 3, '*'));  TEST_EQUAL(s2, "***");
    s1 = "Hello";  TRY(s2 = pad_right(s1, 3, '*'));  TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_right(s1, 5, '*'));  TEST_EQUAL(s2, "Hello");
    s1 = "Hello";  TRY(s2 = pad_right(s1, 7, '*'));  TEST_EQUAL(s2, "Hello**");

    s = "";                                   TRY(std::tie(sv1, sv2) = partition_at(s, ""));    TEST_EQUAL(sv1, "");             TEST_EQUAL(sv2, "");
    s = "";                                   TRY(std::tie(sv1, sv2) = partition_at(s, ";;"));  TEST_EQUAL(sv1, "");             TEST_EQUAL(sv2, "");
    s = "Hello world";                        TRY(std::tie(sv1, sv2) = partition_at(s, ""));    TEST_EQUAL(sv1, "Hello world");  TEST_EQUAL(sv2, "");
    s = "Hello world";                        TRY(std::tie(sv1, sv2) = partition_at(s, ";;"));  TEST_EQUAL(sv1, "Hello world");  TEST_EQUAL(sv2, "");
    s = "Hello world;;hello again;;goodbye";  TRY(std::tie(sv1, sv2) = partition_at(s, ";;"));  TEST_EQUAL(sv1, "Hello world");  TEST_EQUAL(sv2, "hello again;;goodbye");
    s = ";;Hello world;;";                    TRY(std::tie(sv1, sv2) = partition_at(s, ";;"));  TEST_EQUAL(sv1, "");             TEST_EQUAL(sv2, "Hello world;;");

    s = "";                                   TRY(std::tie(sv1, sv2) = partition_by(s));          TEST_EQUAL(sv1, "");             TEST_EQUAL(sv2, "");
    s = "";                                   TRY(std::tie(sv1, sv2) = partition_by(s, ""));      TEST_EQUAL(sv1, "");             TEST_EQUAL(sv2, "");
    s = "";                                   TRY(std::tie(sv1, sv2) = partition_by(s, ".,:;"));  TEST_EQUAL(sv1, "");             TEST_EQUAL(sv2, "");
    s = "Hello";                              TRY(std::tie(sv1, sv2) = partition_by(s));          TEST_EQUAL(sv1, "Hello");        TEST_EQUAL(sv2, "");
    s = "Hello world";                        TRY(std::tie(sv1, sv2) = partition_by(s));          TEST_EQUAL(sv1, "Hello");        TEST_EQUAL(sv2, "world");
    s = " Hello world";                       TRY(std::tie(sv1, sv2) = partition_by(s));          TEST_EQUAL(sv1, "");             TEST_EQUAL(sv2, "Hello world");
    s = "The quick brown fox";                TRY(std::tie(sv1, sv2) = partition_by(s));          TEST_EQUAL(sv1, "The");          TEST_EQUAL(sv2, "quick brown fox");
    s = "Hello world";                        TRY(std::tie(sv1, sv2) = partition_by(s, ""));      TEST_EQUAL(sv1, "Hello world");  TEST_EQUAL(sv2, "");
    s = "Hello world";                        TRY(std::tie(sv1, sv2) = partition_by(s, ".,:;"));  TEST_EQUAL(sv1, "Hello world");  TEST_EQUAL(sv2, "");
    s = "Hello world;;hello again;;goodbye";  TRY(std::tie(sv1, sv2) = partition_by(s, ".,:;"));  TEST_EQUAL(sv1, "Hello world");  TEST_EQUAL(sv2, "hello again;;goodbye");

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

}

void test_core_string_formatting() {

    static constexpr std::array<uint8_t, 4> a4 = {{10,20,30,40}};
    static constexpr std::array<uint8_t, 8> a8 = {{10,20,30,40,50,60,70,80}};

    std::string s;
    Strings svec;
    std::vector<int> ivec;

    TEST_EQUAL(expand_integer(0), "0");
    TEST_EQUAL(expand_integer(1), "1");
    TEST_EQUAL(expand_integer(12), "12");
    TEST_EQUAL(expand_integer(123), "123");
    TEST_EQUAL(expand_integer(1234), "1'234");
    TEST_EQUAL(expand_integer(12345), "12'345");
    TEST_EQUAL(expand_integer(123456), "123'456");
    TEST_EQUAL(expand_integer(1234567), "1'234'567");
    TEST_EQUAL(expand_integer(12345678), "12'345'678");
    TEST_EQUAL(expand_integer(123456789), "123'456'789");
    TEST_EQUAL(expand_integer(1234567891ll), "1'234'567'891");
    TEST_EQUAL(expand_integer(12345678912ll), "12'345'678'912");
    TEST_EQUAL(expand_integer(123456789123ll), "123'456'789'123");
    TEST_EQUAL(expand_integer(1234567891234ll), "1'234'567'891'234");
    TEST_EQUAL(expand_integer(12345678912345ll), "12'345'678'912'345");
    TEST_EQUAL(expand_integer(123456789123456ll), "123'456'789'123'456");
    TEST_EQUAL(expand_integer(1234567891234567ll), "1'234'567'891'234'567");
    TEST_EQUAL(expand_integer(12345678912345678ll), "12'345'678'912'345'678");
    TEST_EQUAL(expand_integer(123456789123456789ll), "123'456'789'123'456'789");
    TEST_EQUAL(expand_integer(-1), "-1");
    TEST_EQUAL(expand_integer(-12), "-12");
    TEST_EQUAL(expand_integer(-123), "-123");
    TEST_EQUAL(expand_integer(-1234), "-1'234");
    TEST_EQUAL(expand_integer(-12345), "-12'345");
    TEST_EQUAL(expand_integer(-123456), "-123'456");
    TEST_EQUAL(expand_integer(-1234567), "-1'234'567");

    TEST_EQUAL(expand_hex(uint32_t(0)), "0x0000'0000");
    TEST_EQUAL(expand_hex(uint32_t(1)), "0x0000'0001");
    TEST_EQUAL(expand_hex(uint32_t(0x12)), "0x0000'0012");
    TEST_EQUAL(expand_hex(uint32_t(0x123)), "0x0000'0123");
    TEST_EQUAL(expand_hex(uint32_t(0x1234)), "0x0000'1234");
    TEST_EQUAL(expand_hex(uint32_t(0x12345)), "0x0001'2345");
    TEST_EQUAL(expand_hex(uint32_t(0x123456)), "0x0012'3456");
    TEST_EQUAL(expand_hex(uint32_t(0x1234567)), "0x0123'4567");
    TEST_EQUAL(expand_hex(uint32_t(0x12345678)), "0x1234'5678");
    TEST_EQUAL(expand_hex(int64_t(0x123456789abcdef0)), "0x1234'5678'9abc'def0");
    TEST_EQUAL(expand_hex(int32_t(-1)), "-0x0000'0001");
    TEST_EQUAL(expand_hex(int32_t(-0x12345678)), "-0x1234'5678");
    TEST_EQUAL(expand_hex(int64_t(-0x123456789abcdef0)), "-0x1234'5678'9abc'def0");

    TEST_EQUAL(hex(a4), "0a141e28");
    TEST_EQUAL(hex(a8), "0a141e28323c4650");

    TEST_EQUAL(hexdump(""s), "");
    TEST_EQUAL(hexdump(""s, 5), "");
    TEST_EQUAL(hexdump("Hello world!"s), "48 65 6c 6c 6f 20 77 6f 72 6c 64 21");
    TEST_EQUAL(hexdump("Hello world!"s, 5), "48 65 6c 6c 6f\n20 77 6f 72 6c\n64 21\n");
    TEST_EQUAL(hexdump("Hello world!"s, 6), "48 65 6c 6c 6f 20\n77 6f 72 6c 64 21\n");

    TEST_EQUAL(tf(true), "true");
    TEST_EQUAL(tf(false), "false");
    TEST_EQUAL(yn(true), "yes");
    TEST_EQUAL(yn(false), "no");

    TRY(s = fmt(""));                               TEST_EQUAL(s, "");
    TRY(s = fmt("Hello world"));                    TEST_EQUAL(s, "Hello world");
    TRY(s = fmt("Hello $1"));                       TEST_EQUAL(s, "Hello ");
    TRY(s = fmt("Hello $1", "world"s));             TEST_EQUAL(s, "Hello world");
    TRY(s = fmt("Hello $1", 42));                   TEST_EQUAL(s, "Hello 42");
    TRY(s = fmt("($1) ($2) ($3)", 10, 20, 30));     TEST_EQUAL(s, "(10) (20) (30)");
    TRY(s = fmt("${1} ${2} ${3}", 10, 20, 30));     TEST_EQUAL(s, "10 20 30");
    TRY(s = fmt("$3,$3,$2,$2,$1,$1", 10, 20, 30));  TEST_EQUAL(s, "30,30,20,20,10,10");
    TRY(s = fmt("Hello $1 $$ ${}", 42));            TEST_EQUAL(s, "Hello 42 $ {}");

    ivec = {1,2,3,4,5};
    TRY(svec = to_strings(ivec));
    TEST_EQUAL(svec.size(), 5);
    TRY(s = to_str(svec));
    TEST_EQUAL(s, "[1,2,3,4,5]");

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
