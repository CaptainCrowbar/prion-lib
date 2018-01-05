#include "rs-core/stack.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <ostream>

using namespace RS;

namespace {

    class TopTail {
    public:
        TopTail(): sp(nullptr), ch() {}
        TopTail(Ustring& s, char c): sp(&s), ch(c) { s += '+'; s += c; }
        ~TopTail() { term(); }
        TopTail(TopTail&& t): sp(t.sp), ch(t.ch) { t.sp = nullptr; }
        TopTail& operator=(TopTail&& t) { if (&t != this) { term(); sp = t.sp; ch = t.ch; t.sp = nullptr; } return *this; }
        char get() const noexcept { return ch; }
        friend std::ostream& operator<<(std::ostream& out, const TopTail& t) { return out << t.ch; }
    private:
        Ustring* sp;
        char ch;
        TopTail(const TopTail&) = delete;
        TopTail& operator=(const TopTail&) = delete;
        void term() { if (sp) { *sp += '-'; *sp += ch; sp = nullptr; } }
    };

}

void test_core_stack_class() {

    Stack<TopTail> st;
    Ustring s;

    TEST(st.empty());
    TEST_EQUAL(to_str(st), "[]");
    TRY(st.emplace(s, 'a'));
    TEST_EQUAL(st.size(), 1);
    TEST_EQUAL(st.top().get(), 'a');
    TEST_EQUAL(to_str(st), "[a]");
    TEST_EQUAL(s, "+a");
    TRY(st.emplace(s, 'b'));
    TEST_EQUAL(st.size(), 2);
    TEST_EQUAL(st.top().get(), 'b');
    TEST_EQUAL(to_str(st), "[a,b]");
    TEST_EQUAL(s, "+a+b");
    TRY(st.emplace(s, 'c'));
    TEST_EQUAL(st.size(), 3);
    TEST_EQUAL(st.top().get(), 'c');
    TEST_EQUAL(to_str(st), "[a,b,c]");
    TEST_EQUAL(s, "+a+b+c");
    TRY(st.clear());
    TEST(st.empty());
    TEST_EQUAL(to_str(st), "[]");
    TEST_EQUAL(s, "+a+b+c-c-b-a");

}
