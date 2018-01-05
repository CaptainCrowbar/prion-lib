#include "rs-core/encoding.hpp"
#include "rs-core/random.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <string>

using namespace RS;

void test_core_encoding_escape() {

    static const std::string
        bin1 = "Hello \"world\"!\r\nGoodbye!...\r\n\xff",
        text1 = "Hello \"world\"!\\r\\nGoodbye!...\\r\\n\\xff",
        wrap1 = "Hello \"wor\nld\"!\\r\\nGo\nodbye!...\n\\r\\n\\xff";

    EscapeEncoding code;
    std::string bin, text;
    size_t n = 0;

    bin.clear();  TRY(text = code.encode(""));        TEST_EQUAL(text, "");     TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 0);   TEST_EQUAL(bin, "");
    bin.clear();  TRY(text = code.encode(bin1));      TEST_EQUAL(text, text1);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 37);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin1, 10));  TEST_EQUAL(text, wrap1);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 40);  TEST_EQUAL(bin, bin1);

}

void test_core_encoding_quote() {

    static const std::string
        bin1 = "Hello \"world\"!\r\nGoodbye!...\r\n\xff",
        text1 = "\"Hello \\\"world\\\"!\\r\\nGoodbye!...\\r\\n\\xff\"",
        wrap1 = "\"Hello \\\"w\norld\\\"!\\r\n\\nGoodbye!\n...\\r\\n\n\\xff\"";

    QuoteEncoding code;
    std::string bin, text;
    size_t n = 0;

    bin.clear();  TRY(text = code.encode(""));        TEST_EQUAL(text, "\"\"");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 2);   TEST_EQUAL(bin, "");
    bin.clear();  TRY(text = code.encode(bin1));      TEST_EQUAL(text, text1);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 41);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin1, 10));  TEST_EQUAL(text, wrap1);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 45);  TEST_EQUAL(bin, bin1);

    bin.clear();  text = "\"\"abcde";      TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 2);   TEST_EQUAL(bin, "");
    bin.clear();  text = text1 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 41);  TEST_EQUAL(bin, bin1);
    bin.clear();  text = wrap1 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 45);  TEST_EQUAL(bin, bin1);

}

void test_core_encoding_hex() {

    static const std::string
        bin1 = "Hello world",
        text1 = "48656c6c6f20776f726c64",
        wrap1 = "48656c6c6f\n20776f726c\n64";

    HexEncoding code;
    std::string bin, text;
    size_t n = 0;

    bin.clear();  TRY(text = code.encode(""));        TEST_EQUAL(text, "");     TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 0);   TEST_EQUAL(bin, "");
    bin.clear();  TRY(text = code.encode(bin1));      TEST_EQUAL(text, text1);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 22);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin1, 10));  TEST_EQUAL(text, wrap1);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 24);  TEST_EQUAL(bin, bin1);

}

void test_core_encoding_base32() {

    static constexpr size_t limit = 1000;
    static constexpr size_t width = 10;
    static constexpr size_t maxreg = 200;

    Base32Encoding code;
    Ustring s1, s2, s3;
    Xoroshiro rng(42);

    for (size_t n = 0; n <= limit; ++n) {
        s1.resize(n, '\0');
        for (char& c: s1)
            c = char(random_integer(rng, 256));
        size_t expanded = (n * 8 + 4) / 5;
        if (width != 0 && expanded > width)
            expanded += (expanded - 1) / width;
        TRY(s2 = code.encode(s1, width));
        TEST_EQUAL(s2.size(), expanded);
        if (n > 0 && n <= maxreg)
            TEST_MATCH(s2, "^([0-9a-z]{10}\\n)*[0-9a-z]{1,10}$");
        TRY(s3 = code.decode(s2));
        TEST_EQUAL(s3, s1);
    }

}

void test_core_encoding_base64() {

    static const std::string
        bin1 = "Man is distinguished, not only by his reason, but by this singular passion from "
               "other animals, which is a lust of the mind, that by a perseverance of delight "
               "in the continued and indefatigable generation of knowledge, exceeds the short "
               "vehemence of any carnal pleasure.",
        text1 = "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlzIHNp"
                "bmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2YgdGhlIG1p"
                "bmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGludWVkIGFuZCBp"
                "bmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRoZSBzaG9ydCB2ZWhl"
                "bWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=",
        wrap1 = "TWFuIGlzIG\nRpc3Rpbmd1\naXNoZWQsIG\n5vdCBvbmx5\nIGJ5IGhpcy\nByZWFzb24s\nIGJ1dCBieS\nB0aGlzIHNp\n"
                "bmd1bGFyIH\nBhc3Npb24g\nZnJvbSBvdG\nhlciBhbmlt\nYWxzLCB3aG\nljaCBpcyBh\nIGx1c3Qgb2\nYgdGhlIG1p\n"
                "bmQsIHRoYX\nQgYnkgYSBw\nZXJzZXZlcm\nFuY2Ugb2Yg\nZGVsaWdodC\nBpbiB0aGUg\nY29udGludW\nVkIGFuZCBp\n"
                "bmRlZmF0aW\ndhYmxlIGdl\nbmVyYXRpb2\n4gb2Yga25v\nd2xlZGdlLC\nBleGNlZWRz\nIHRoZSBzaG\n9ydCB2ZWhl\n"
                "bWVuY2Ugb2\nYgYW55IGNh\ncm5hbCBwbG\nVhc3VyZS4=",
        bin2 = "any carnal pleasure.",  text2 = "YW55IGNhcm5hbCBwbGVhc3VyZS4=",
        bin3 = "any carnal pleasure",   text3 = "YW55IGNhcm5hbCBwbGVhc3VyZQ==",
        bin4 = "any carnal pleasur",    text4 = "YW55IGNhcm5hbCBwbGVhc3Vy",
        bin5 = "any carnal pleasu",     text5 = "YW55IGNhcm5hbCBwbGVhc3U=",
        bin6 = "any carnal pleas",      text6 = "YW55IGNhcm5hbCBwbGVhcw==";

    Base64Encoding code;
    std::string bin, text;
    size_t n = 0;

    bin.clear();  TRY(text = code.encode(""));        TEST_EQUAL(text, "");     TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 0);    TEST_EQUAL(bin, "");
    bin.clear();  TRY(text = code.encode(bin1));      TEST_EQUAL(text, text1);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 360);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin1, 10));  TEST_EQUAL(text, wrap1);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 395);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin2));      TEST_EQUAL(text, text2);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 28);   TEST_EQUAL(bin, bin2);
    bin.clear();  TRY(text = code.encode(bin3));      TEST_EQUAL(text, text3);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 28);   TEST_EQUAL(bin, bin3);
    bin.clear();  TRY(text = code.encode(bin4));      TEST_EQUAL(text, text4);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 24);   TEST_EQUAL(bin, bin4);
    bin.clear();  TRY(text = code.encode(bin5));      TEST_EQUAL(text, text5);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 24);   TEST_EQUAL(bin, bin5);
    bin.clear();  TRY(text = code.encode(bin6));      TEST_EQUAL(text, text6);  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 24);   TEST_EQUAL(bin, bin6);

    bin.clear();  text = trim_right(text2, "=");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 27);  TEST_EQUAL(bin, bin2);
    bin.clear();  text = trim_right(text3, "=");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 26);  TEST_EQUAL(bin, bin3);
    bin.clear();  text = trim_right(text4, "=");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 24);  TEST_EQUAL(bin, bin4);
    bin.clear();  text = trim_right(text5, "=");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 23);  TEST_EQUAL(bin, bin5);
    bin.clear();  text = trim_right(text6, "=");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 22);  TEST_EQUAL(bin, bin6);

}

void test_core_encoding_ascii85() {

    static const std::string
        bin1 = "Man is distinguished, not only by his reason, but by this singular passion from "
               "other animals, which is a lust of the mind, that by a perseverance of delight "
               "in the continued and indefatigable generation of knowledge, exceeds the short "
               "vehemence of any carnal pleasure.",
        text1 = "<~9jqo^BlbD-BleB1DJ+*+F(f,q/0JhKF<GL>Cj@.4Gp$d7F!,L7@<6@)/0JDEF<G%<+EV:2F!,O<DJ+"
                "*.@<*K0@<6L(Df-\\0Ec5e;DffZ(EZee.Bl.9pF\"AGXBPCsi+DGm>@3BB/F*&OCAfu2/AKYi(DIb:@FD,"
                "*)+C]U=@3BN#EcYf8ATD3s@q?d$AftVqCh[NqF<G:8+EV:.+Cf>-FD5W8ARlolDIal(DId<j@<?3r@:F"
                "%a+D58'ATD4$Bl@l3De:,-DJs`8ARoFb/0JMK@qB4^F!,R<AKZ&-DfTqBG%G>uD.RTpAKYo'+CT/5+Ce"
                "i#DII?(E,9)oF*2M7/c~>",
        wrap1 = "<~9jqo^Blb\nD-BleB1DJ+\n*+F(f,q/0J\nhKF<GL>Cj@\n.4Gp$d7F!,\nL7@<6@)/0J\nDEF<G%<+EV\n:2F!,O<DJ+\n"
                "*.@<*K0@<6\nL(Df-\\0Ec5\ne;DffZ(EZe\ne.Bl.9pF\"A\nGXBPCsi+DG\nm>@3BB/F*&\nOCAfu2/AKY\ni(DIb:@FD,\n"
                "*)+C]U=@3B\nN#EcYf8ATD\n3s@q?d$Aft\nVqCh[NqF<G\n:8+EV:.+Cf\n>-FD5W8ARl\nolDIal(DId\n<j@<?3r@:F\n"
                "%a+D58'ATD\n4$Bl@l3De:\n,-DJs`8ARo\nFb/0JMK@qB\n4^F!,R<AKZ\n&-DfTqBG%G\n>uD.RTpAKY\no'+CT/5+Ce\n"
                "i#DII?(E,9\n)oF*2M7/c\n~>";

    Ascii85Encoding code;
    std::string bin, text;
    size_t n = 0;

    bin.clear();  TRY(text = code.encode(""));        TEST_EQUAL(text, "<~~>");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 4);    TEST_EQUAL(bin, "");
    bin.clear();  TRY(text = code.encode(bin1));      TEST_EQUAL(text, text1);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 341);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin1, 10));  TEST_EQUAL(text, wrap1);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 375);  TEST_EQUAL(bin, bin1);

    bin.clear();  text = "<~~>abcde";      TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 4);    TEST_EQUAL(bin, "");
    bin.clear();  text = text1 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 341);  TEST_EQUAL(bin, bin1);
    bin.clear();  text = wrap1 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 375);  TEST_EQUAL(bin, bin1);

}

void test_core_encoding_z85() {

    static const std::string
        bin1 = "\x86\x4f\xd2\x6f\xb5\x59\xf7\x5b",
        text1 = "<~HelloWorld~>",
        wrap1 = "<~HelloWor\nld~>",
        bin2 = "\x8e\x0b\xdd\x69\x76\x28\xb9\x1d"
               "\x8f\x24\x55\x87\xee\x95\xc5\xb0"
               "\x4d\x48\x96\x3f\x79\x25\x98\x77"
               "\xb4\x9c\xd9\x06\x3a\xea\xd3\xb7",
        text2 = "<~JTKVSB%%)wK0E.X)V>+}o?pNmC{O&4W4b!Ni{Lh6~>",
        wrap2 = "<~JTKVSB%%\n)wK0E.X)V>\n+}o?pNmC{O\n&4W4b!Ni{L\nh6~>";

    Z85Encoding code;
    std::string bin, text;
    size_t n = 0;

    bin.clear();  TRY(text = code.encode(""));        TEST_EQUAL(text, "<~~>");  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 4);   TEST_EQUAL(bin, "");
    bin.clear();  TRY(text = code.encode(bin1));      TEST_EQUAL(text, text1);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 14);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin1, 10));  TEST_EQUAL(text, wrap1);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 15);  TEST_EQUAL(bin, bin1);
    bin.clear();  TRY(text = code.encode(bin2));      TEST_EQUAL(text, text2);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 44);  TEST_EQUAL(bin, bin2);
    bin.clear();  TRY(text = code.encode(bin2, 10));  TEST_EQUAL(text, wrap2);   TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 48);  TEST_EQUAL(bin, bin2);

    bin.clear();  text = "<~~>abcde";      TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 4);   TEST_EQUAL(bin, "");
    bin.clear();  text = text1 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 14);  TEST_EQUAL(bin, bin1);
    bin.clear();  text = wrap1 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 15);  TEST_EQUAL(bin, bin1);
    bin.clear();  text = text2 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 44);  TEST_EQUAL(bin, bin2);
    bin.clear();  text = wrap2 + "abcde";  TRY(n = code.decode(text, bin));  TEST_EQUAL(n, 48);  TEST_EQUAL(bin, bin2);

}
