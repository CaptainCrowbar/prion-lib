#include "rs-core/table.hpp"
#include "unicorn/string.hpp"
#include <algorithm>

using namespace RS::Unicorn;

namespace RS {

    namespace {

        constexpr uint32_t length_flags = Length::graphemes | Length::narrow;

    }

    // Class Table

    void Table::clear() noexcept {
        cells.resize(1);
        cells.back().clear();
        widths.clear();
        repeats = false;
    }

    void Table::rule(char c) {
        if (c == '\0')
            c = rules;
        if (! cells.back().empty())
            cells.push_back({});
        cells.back().push_back(Ustring{'\0', c});
        cells.push_back({});
        rules = '-';
    }

    Ustring to_str(const Table& tab) {
        Ustring text;
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
                    width = str_length(row[c], length_flags);
                }
                if (c != columns - 1)
                    text.append(tab.widths[c] - width + 2, ' ');
            }
            text += '\n';
            prev = &row;
        }
        return text;
    }

    void Table::add(Ustring s) {
        if (s == "\n") {
            cells.push_back({});
            return;
        }
        auto& row = cells.back();
        if (s.empty())
            s = "--";
        row.push_back(s);
        size_t len = str_length(s, length_flags);
        if (widths.size() < row.size()) {
            widths.push_back(len);
        } else {
            auto& width = widths[row.size() - 1];
            width = std::max(width, len);
        }
    }

}
