#include "rs-core/grid.hpp"
#include "rs-core/string.hpp"
#include "rs-core/unit-test.hpp"
#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace RS;

namespace {

    template <typename T, size_t N>
    U8string grid_format_helper(const Grid<T, N>& g, Vector<ptrdiff_t, N> index, size_t current) {
        std::ostringstream out;
        out << "[";
        ptrdiff_t n = g.shape()[current];
        ptrdiff_t& i(index[current]);
        for (i = 0; i < n; ++i) {
            if (current == N - 1)
                out << g[index];
            else
                out << grid_format_helper(g, index, current + 1);
            out << ",";
        }
        auto str = out.str();
        if (n > 0)
            str.pop_back();
        str += "]";
        return str;
    }

    template <typename T, size_t N>
    U8string grid_format(const Grid<T, N>& g) {
        Vector<ptrdiff_t, N> index;
        std::fill(index.begin(), index.end(), 0);
        return grid_format_helper(g, index, 0);
    }

}

void test_core_grid_class() {

    using grid_i2 = Grid<int, 2>;
    using grid_i4 = Grid<int, 4>;
    using grid_s2 = Grid<std::string, 2>;

    grid_i2 ig;
    grid_i4 ig4;
    grid_i2::iterator i;
    grid_i2::const_iterator ci;
    grid_s2 sg;

    TEST(sg.empty());
    TEST_EQUAL(sg.size(), 0);
    TEST_EQUAL(to_str(sg.shape()), "[0,0]");
    TEST_EQUAL(grid_format(sg), "[]");

    TRY(sg = grid_s2(3, 4));
    TEST(! sg.empty());
    TEST_EQUAL(sg.size(), 12);
    TEST_EQUAL(to_str(sg.shape()), "[3,4]");

    TRY(sg.fill("@"));
    TEST_EQUAL(grid_format(sg), "[[@,@,@,@],[@,@,@,@],[@,@,@,@]]");

    TRY(sg = grid_s2({4, 3}, "#"));
    TEST_EQUAL(to_str(sg.shape()), "[4,3]");
    TEST_EQUAL(grid_format(sg), "[[#,#,#],[#,#,#],[#,#,#],[#,#,#]]");

    TRY(sg.reset(3, 4));
    TEST_EQUAL(to_str(sg.shape()), "[3,4]");

    TRY(sg.fill("@"));
    TEST_EQUAL(grid_format(sg), "[[@,@,@,@],[@,@,@,@],[@,@,@,@]]");

    TRY(sg.reset({4, 3}, "#"));
    TEST_EQUAL(to_str(sg.shape()), "[4,3]");
    TEST_EQUAL(grid_format(sg), "[[#,#,#],[#,#,#],[#,#,#],[#,#,#]]");

    TRY(sg.clear());
    TEST(sg.empty());
    TEST_EQUAL(sg.size(), 0);
    TEST_EQUAL(to_str(sg.shape()), "[0,0]");

    TRY(ig.reset(4, 3));
    for (int a = 0; a < 4; ++a)
        for (int b = 0; b < 3; ++b)
            TRY(ig.at(a, b) = 100 + 10 * a + b);
    TEST_EQUAL(grid_format(ig), "[[100,101,102],[110,111,112],[120,121,122],[130,131,132]]");
    TEST_EQUAL(to_str(ig), "[100,101,102,110,111,112,120,121,122,130,131,132]");
    TEST_EQUAL(ig.at(0, 0), 100);
    TEST_EQUAL(ig.at(0, 2), 102);
    TEST_EQUAL(ig.at(3, 0), 130);
    TEST_EQUAL(ig.at(3, 2), 132);
    TEST_THROW(ig.at(0, 3), std::out_of_range);
    TEST_THROW(ig.at(4, 0), std::out_of_range);
    TEST(ig.contains(0, 0));
    TEST(ig.contains(3, 2));
    TEST(! ig.contains(3, 3));
    TEST(! ig.contains(4, 2));

    TRY(i = ig.locate(1, 1));
    TEST_EQUAL(*i, 111);
    TEST_EQUAL(to_str(i.pos()), "[1,1]");

    TRY(ci = ig.locate(1, 1));
    TEST_EQUAL(*ci, 111);
    TEST_EQUAL(to_str(ci.pos()), "[1,1]");

    TRY(i.move(0, 1));
    TEST_EQUAL(*i, 121);
    TEST_EQUAL(to_str(i.pos()), "[2,1]");

    TRY(i.move(1, -1));
    TEST_EQUAL(*i, 120);
    TEST_EQUAL(to_str(i.pos()), "[2,0]");

    TRY(ig.reshape(2, 5));
    TEST_EQUAL(ig.size(), 10);
    TEST_EQUAL(to_str(ig.shape()), "[2,5]");
    TEST_EQUAL(grid_format(ig), "[[100,101,102,0,0],[110,111,112,0,0]]");

    TRY(ig4.reset(2, 3, 2, 3));
    TEST_EQUAL(ig4.size(), 36);
    TEST_EQUAL(to_str(ig4.shape()), "[2,3,2,3]");
    for (int a = 0; a < 2; ++a)
        for (int b = 0; b < 3; ++b)
            for (int c = 0; c < 2; ++c)
                for (int d = 0; d < 3; ++d)
                    TRY(ig4(a, b, c, d) = 1111 + 1000 * a + 100 * b + 10 * c + d);
    TEST_EQUAL(grid_format(ig4),
        "[[[[1111,1112,1113],[1121,1122,1123]],"
          "[[1211,1212,1213],[1221,1222,1223]],"
          "[[1311,1312,1313],[1321,1322,1323]]],"
         "[[[2111,2112,2113],[2121,2122,2123]],"
          "[[2211,2212,2213],[2221,2222,2223]],"
          "[[2311,2312,2313],[2321,2322,2323]]]]"
    );

    TRY(ig4.reshape(4, 2, 4, 2));
    TEST_EQUAL(ig4.size(), 64);
    TEST_EQUAL(to_str(ig4.shape()), "[4,2,4,2]");
    TEST_EQUAL(grid_format(ig4),
        "[[[[1111,1112],[1121,1122],[0,0],[0,0]],"
          "[[1211,1212],[1221,1222],[0,0],[0,0]]],"
         "[[[2111,2112],[2121,2122],[0,0],[0,0]],"
          "[[2211,2212],[2221,2222],[0,0],[0,0]]],"
         "[[[0,0],[0,0],[0,0],[0,0]],"
          "[[0,0],[0,0],[0,0],[0,0]]],"
         "[[[0,0],[0,0],[0,0],[0,0]],"
          "[[0,0],[0,0],[0,0],[0,0]]]]"
    );

}
