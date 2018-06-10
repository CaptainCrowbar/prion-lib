#pragma once

#include "rs-core/common.hpp"
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
        friend Ustring to_str(const Table& tab);
        friend std::ostream& operator<<(std::ostream& out, const Table& tab) { return out << to_str(tab); }
    private:
        std::vector<Strings> cells;
        std::vector<size_t> widths;
        bool repeats = false;
        char rules = '=';
        void add(Ustring s);
    };

}
