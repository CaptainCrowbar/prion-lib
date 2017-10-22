#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <ostream>
#include <vector>

namespace RS {

    class Table {
    public:
        Table() { cells.push_back({}); }
        template <typename T> Table& operator<<(const T& t) { add(to_str(t)); return *this; }
        void clear() noexcept;
        bool empty() const noexcept { return cells.size() == 1 && cells.back().empty(); }
        void rule(char c = '\0');
        void show_repeats(bool flag = true) noexcept { repeats = flag; }
        friend U8string to_str(const Table& tab);
        friend std::ostream& operator<<(std::ostream& out, const Table& tab) { return out << to_str(tab); }
    private:
        std::vector<Strings> cells;
        std::vector<size_t> widths;
        bool repeats = false;
        char rules = '=';
        void add(U8string s);
    };

    inline void Table::clear() noexcept {
        cells.resize(1);
        cells.back().clear();
        widths.clear();
        repeats = false;
    }

    inline void Table::rule(char c) {
        if (c == '\0')
            c = rules;
        if (! cells.back().empty())
            cells.push_back({});
        cells.back().push_back(U8string{'\0', c});
        cells.push_back({});
        rules = '-';
    }

    inline U8string to_str(const Table& tab) {
        U8string text;
        size_t columns = tab.widths.size(), rows = tab.cells.size(), width = 0;
        if (tab.cells.back().empty())
            --rows;
        const Strings* prev = nullptr;
        for (size_t r = 0; r < rows; ++r) {
            auto& row = tab.cells[r];
            char ruler = ! row.empty() && row[0][0] == '\0' ? row[0][1] : '\0';
            for (size_t c = 0; c < columns; ++c) {
                if (ruler) {
                    width = tab.widths[c];
                    text.append(width, ruler);
                } else if (c >= row.size() || row[c].empty()) {
                    text += "--";
                    width = 2;
                } else if (tab.repeats && prev && c < prev->size() && row[c] == (*prev)[c]) {
                    text += "''";
                    width = 2;
                } else {
                    text += row[c];
                    width = ulength(row[c]);
                }
                if (c != columns - 1)
                    text.append(tab.widths[c] - width + 2, ' ');
            }
            text += '\n';
            prev = &row;
        }
        return text;
    }

    inline void Table::add(U8string s) {
        if (s == "\n") {
            cells.push_back({});
            return;
        }
        auto& row = cells.back();
        if (s.empty())
            s = "--";
        row.push_back(s);
        size_t len = ulength(s);
        if (widths.size() < row.size()) {
            widths.push_back(len);
        } else {
            auto& width = widths[row.size() - 1];
            width = std::max(width, len);
        }
    }

}
