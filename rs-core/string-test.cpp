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

    static const Ustring n1 = "... ALPHA ... bravo ... charlie ... 12345 ...";
    static const Ustring n2 = "ALPHABravo/Charlie12345";
    static const Ustring in = "ABC1";
    static const Ustring lc = "alpha_bravo_charlie_12345";
    static const Ustring uc = "ALPHA_BRAVO_CHARLIE_12345";
    static const Ustring tc = "AlphaBravoCharlie12345";
    static const Ustring cc = "alphaBravoCharlie12345";
    static const Ustring sc = "Alpha bravo charlie 12345";

    Strings vec;
    Ustring str;

    TRY(vec = name_breakdown(n1));  TEST_EQUAL(vec.size(), 4);  TRY(str = to_str(vec));  TEST_EQUAL(str, "[ALPHA,bravo,charlie,12345]");
    TRY(vec = name_breakdown(n2));  TEST_EQUAL(vec.size(), 4);  TRY(str = to_str(vec));  TEST_EQUAL(str, "[ALPHA,Bravo,Charlie,12345]");
    TRY(vec = name_breakdown(lc));  TEST_EQUAL(vec.size(), 4);  TRY(str = to_str(vec));  TEST_EQUAL(str, "[alpha,bravo,charlie,12345]");
    TRY(vec = name_breakdown(uc));  TEST_EQUAL(vec.size(), 4);  TRY(str = to_str(vec));  TEST_EQUAL(str, "[ALPHA,BRAVO,CHARLIE,12345]");
    TRY(vec = name_breakdown(tc));  TEST_EQUAL(vec.size(), 4);  TRY(str = to_str(vec));  TEST_EQUAL(str, "[Alpha,Bravo,Charlie,12345]");
    TRY(vec = name_breakdown(cc));  TEST_EQUAL(vec.size(), 4);  TRY(str = to_str(vec));  TEST_EQUAL(str, "[alpha,Bravo,Charlie,12345]");
    TRY(vec = name_breakdown(sc));  TEST_EQUAL(vec.size(), 4);  TRY(str = to_str(vec));  TEST_EQUAL(str, "[Alpha,bravo,charlie,12345]");

    TRY(str = name_to_initials(n1));  TEST_EQUAL(str, in);
    TRY(str = name_to_initials(n2));  TEST_EQUAL(str, in);
    TRY(str = name_to_initials(lc));  TEST_EQUAL(str, in);
    TRY(str = name_to_initials(uc));  TEST_EQUAL(str, in);
    TRY(str = name_to_initials(tc));  TEST_EQUAL(str, in);
    TRY(str = name_to_initials(cc));  TEST_EQUAL(str, in);
    TRY(str = name_to_initials(sc));  TEST_EQUAL(str, in);

    TRY(str = name_to_lower_case(n1));  TEST_EQUAL(str, lc);
    TRY(str = name_to_lower_case(n2));  TEST_EQUAL(str, lc);
    TRY(str = name_to_lower_case(lc));  TEST_EQUAL(str, lc);
    TRY(str = name_to_lower_case(uc));  TEST_EQUAL(str, lc);
    TRY(str = name_to_lower_case(tc));  TEST_EQUAL(str, lc);
    TRY(str = name_to_lower_case(cc));  TEST_EQUAL(str, lc);
    TRY(str = name_to_lower_case(sc));  TEST_EQUAL(str, lc);

    TRY(str = name_to_upper_case(n1));  TEST_EQUAL(str, uc);
    TRY(str = name_to_upper_case(n2));  TEST_EQUAL(str, uc);
    TRY(str = name_to_upper_case(lc));  TEST_EQUAL(str, uc);
    TRY(str = name_to_upper_case(uc));  TEST_EQUAL(str, uc);
    TRY(str = name_to_upper_case(tc));  TEST_EQUAL(str, uc);
    TRY(str = name_to_upper_case(cc));  TEST_EQUAL(str, uc);
    TRY(str = name_to_upper_case(sc));  TEST_EQUAL(str, uc);

    TRY(str = name_to_title_case(n1));  TEST_EQUAL(str, tc);
    TRY(str = name_to_title_case(n2));  TEST_EQUAL(str, tc);
    TRY(str = name_to_title_case(lc));  TEST_EQUAL(str, tc);
    TRY(str = name_to_title_case(uc));  TEST_EQUAL(str, tc);
    TRY(str = name_to_title_case(tc));  TEST_EQUAL(str, tc);
    TRY(str = name_to_title_case(cc));  TEST_EQUAL(str, tc);
    TRY(str = name_to_title_case(sc));  TEST_EQUAL(str, tc);

    TRY(str = name_to_camel_case(n1));  TEST_EQUAL(str, cc);
    TRY(str = name_to_camel_case(n2));  TEST_EQUAL(str, cc);
    TRY(str = name_to_camel_case(lc));  TEST_EQUAL(str, cc);
    TRY(str = name_to_camel_case(uc));  TEST_EQUAL(str, cc);
    TRY(str = name_to_camel_case(tc));  TEST_EQUAL(str, cc);
    TRY(str = name_to_camel_case(cc));  TEST_EQUAL(str, cc);
    TRY(str = name_to_camel_case(sc));  TEST_EQUAL(str, cc);

    TRY(str = name_to_sentence_case(n1));  TEST_EQUAL(str, sc);
    TRY(str = name_to_sentence_case(n2));  TEST_EQUAL(str, sc);
    TRY(str = name_to_sentence_case(lc));  TEST_EQUAL(str, sc);
    TRY(str = name_to_sentence_case(uc));  TEST_EQUAL(str, sc);
    TRY(str = name_to_sentence_case(tc));  TEST_EQUAL(str, sc);
    TRY(str = name_to_sentence_case(cc));  TEST_EQUAL(str, sc);
    TRY(str = name_to_sentence_case(sc));  TEST_EQUAL(str, sc);

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
    TEST(! string_is_ascii(u8"åß∂"));

}

void test_core_string_manipulation() {

    std::string s, s1, s2;
    std::wstring ws;
    std::string_view sv1, sv2;
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

    static constexpr std::array<uint8_t, 4> a4 = {{10,20,30,40}};
    static constexpr std::array<uint8_t, 8> a8 = {{10,20,30,40,50,60,70,80}};

    std::string s;
    Strings svec;
    std::vector<int> ivec;

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
