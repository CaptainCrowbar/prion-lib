#include "rs-core/kwargs.hpp"
#include "rs-core/unit-test.hpp"
#include <string>

using namespace RS;
using namespace std::literals;

namespace {

    constexpr Kwarg<bool> flag;
    constexpr Kwarg<int> number;
    constexpr Kwarg<std::string, 1> str1;
    constexpr Kwarg<std::string, 2> str2;

    std::string tf(bool b) { return b ? "T" : "F"; }

    template <typename... Args>
    std::string test_kwtest(Args... args) {
        bool f = kwtest(flag, args...);
        bool n = kwtest(number, args...);
        bool s1 = kwtest(str1, args...);
        bool s2 = kwtest(str2, args...);
        return tf(f) + " " + tf(n) + " " + tf(s1) + " " + tf(s2);
    }

    template <typename... Args>
    std::string test_kwget(Args... args) {
        bool f = kwget(flag, false, args...);
        int n = kwget(number, -1, args...);
        std::string s1 = kwget(str1, "foo"s, args...);
        std::string s2 = kwget(str2, "bar"s, args...);
        return tf(f) + " " + std::to_string(n) + " " + s1 + " " + s2;
    }

}

void test_core_kwargs_keyword_arguments() {

    std::string s;

    TRY(s = test_kwtest());                                    TEST_EQUAL(s, "F F F F");
    TRY(s = test_kwtest(flag=true));                           TEST_EQUAL(s, "T F F F");
    TRY(s = test_kwtest(flag));                                TEST_EQUAL(s, "T F F F");
    TRY(s = test_kwtest(number=42));                           TEST_EQUAL(s, "F T F F");
    TRY(s = test_kwtest(str1="hello"));                        TEST_EQUAL(s, "F F T F");
    TRY(s = test_kwtest(str2="goodbye"));                      TEST_EQUAL(s, "F F F T");
    TRY(s = test_kwtest(flag=true, number=42));                TEST_EQUAL(s, "T T F F");
    TRY(s = test_kwtest(number=42, flag=true));                TEST_EQUAL(s, "T T F F");
    TRY(s = test_kwtest(flag=true, number=42, str1="hello"));  TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(str1="hello", number=42, flag=true));  TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(flag, number=42, str1="hello"));       TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(str1="hello", number=42, flag));       TEST_EQUAL(s, "T T T F");
    TRY(s = test_kwtest(str1="hello", str2="goodbye"));        TEST_EQUAL(s, "F F T T");
    TRY(s = test_kwtest(str2="goodbye", str1="hello"));        TEST_EQUAL(s, "F F T T");

    TRY(s = test_kwget());                                    TEST_EQUAL(s, "F -1 foo bar");
    TRY(s = test_kwget(flag=true));                           TEST_EQUAL(s, "T -1 foo bar");
    TRY(s = test_kwget(flag));                                TEST_EQUAL(s, "T -1 foo bar");
    TRY(s = test_kwget(number=42));                           TEST_EQUAL(s, "F 42 foo bar");
    TRY(s = test_kwget(str1="hello"));                        TEST_EQUAL(s, "F -1 hello bar");
    TRY(s = test_kwget(str2="goodbye"));                      TEST_EQUAL(s, "F -1 foo goodbye");
    TRY(s = test_kwget(flag=true, number=42));                TEST_EQUAL(s, "T 42 foo bar");
    TRY(s = test_kwget(number=42, flag=true));                TEST_EQUAL(s, "T 42 foo bar");
    TRY(s = test_kwget(flag=true, number=42, str1="hello"));  TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(str1="hello", number=42, flag=true));  TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(flag, number=42, str1="hello"));       TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(str1="hello", number=42, flag));       TEST_EQUAL(s, "T 42 hello bar");
    TRY(s = test_kwget(str1="hello", str2="goodbye"));        TEST_EQUAL(s, "F -1 hello goodbye");
    TRY(s = test_kwget(str2="goodbye", str1="hello"));        TEST_EQUAL(s, "F -1 hello goodbye");

}
