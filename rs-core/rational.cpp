#include "rs-core/rational.hpp"

namespace RS::RS_Detail {

    bool parse_rational(Uview s, std::vector<Uview>& parts, bool& neg) noexcept {
        auto skipws = [] (auto& i, auto end) { while (i != end && ascii_isspace(*i)) ++i; };
        auto getnum = [skipws] (auto& i, auto end, auto& out) {
            skipws(i, end);
            auto j = i;
            while (i != end && ascii_isdigit(*i))
                ++i;
            if (i == j)
                return false;
            Uview part(&*j, i - j);
            out.push_back(part);
            skipws(i, end);
            return true;
        };
        auto i = s.begin(), end = s.end();
        skipws(i, end);
        if (i == end)
            return false;
        neg = false;
        if (*i == '+' || *i == '-')
            neg = *i++ == '-';
        if (! getnum(i, end, parts))
            return false;
        if (i != end) {
            if (*i != '/' && ! getnum(i, end, parts))
                return false;
            if (*i != '/')
                return false;
            ++i;
            if (! getnum(i, end, parts))
                return false;
        }
        return i == end;
    }

}
