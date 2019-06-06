#include "rs-core/interval.hpp"
#include "rs-core/string.hpp"
#include <regex>
#include <unordered_map>

using namespace std::literals;

namespace RS {

    // Common base class for all intervals

    std::pair<IntervalBound, IntervalBound> IntervalCommonBase::parse_interval_mode(Uview mode) {
        static const std::unordered_map<Uview, std::pair<IB, IB>> map = {
            { "()"sv,  { IB::open,     IB::open     }},
            { "(]"sv,  { IB::open,     IB::closed   }},
            { "[)"sv,  { IB::closed,   IB::open     }},
            { "[]"sv,  { IB::closed,   IB::closed   }},
            { ">"sv,   { IB::open,     IB::unbound  }},
            { "<"sv,   { IB::unbound,  IB::open     }},
            { ">="sv,  { IB::closed,   IB::unbound  }},
            { "<="sv,  { IB::unbound,  IB::closed   }},
        };
        auto it = map.find(mode);
        if (it == map.end())
            throw std::invalid_argument("Invalid interval mode: " + quote(mode));
        return it->second;
    }

    bool IntervalCommonBase::parse_interval_string(Uview str, Uview& min, Uview& max, IntervalBound& left, IntervalBound& right) noexcept {
        static const Ustring lb = "([(\\[])";
        static const Ustring rb = "([)\\]])";
        static const Ustring chars = "[^,.<=>()\\[\\]{}]";
        static const Ustring text = "(" + chars + "+(?:\\." + chars + "+)*)";
        static const std::regex pattern(
            "(\\{\\})"                             // [1]      Empty interval
            "|(\\*)"                               // [2]      Universal interval
            "|([<>]=?)" + text +                   // [3-4]    Single bounded interval
            "|" + lb + text + "," + text + rb +    // [5-8]    Double bounded interval
            "|" + text + "(\\.\\.[.<]?)" + text +  // [9-11]   Alternate double bounded interval
            "|" + text + "([+-])" +                // [12-13]  Alternate single bounded interval
            "|=?" + text +                         // [14]     Single value
            "|",                                   // ...      Empty string
            std::regex::optimize);
        min = max = {};
        left = right = IB::empty;
        auto p = str.data(), q = p + str.size();
        std::cmatch match;
        if (! std::regex_match(p, q, match, pattern)) {
            return false;
        } else if (match[2].matched) {
            left = right = IB::unbound;
        } else if (match[3].matched) {
            Uview op(match[3].first, match[3].length());
            Uview val(match[4].first, match[4].length());
            if (op == "<")        { max = val; left = IB::unbound; right = IB::open; }
            else if (op == "<=")  { max = val; left = IB::unbound; right = IB::closed; }
            else if (op == ">")   { min = val; left = IB::open; right = IB::unbound; }
            else                  { min = val; left = IB::closed; right = IB::unbound; }
        } else if (match[5].matched) {
            min = Uview(match[6].first, match[6].length());
            max = Uview(match[7].first, match[7].length());
            char l = *match[5].first;
            char r = *match[8].first;
            left = l == '[' ? IB::closed : IB::open;
            right = r == ']' ? IB::closed : IB::open;
        } else if (match[9].matched) {
            min = Uview(match[9].first, match[9].length());
            Uview op(match[10].first, match[10].length());
            max = Uview(match[11].first, match[11].length());
            left = IB::closed;
            right = op == "..<" ? IB::open : IB::closed;
        } else if (match[12].matched) {
            Uview val(match[12].first, match[12].length());
            char op = *match[13].first;
            if (op == '-')  { max = val; left = IB::unbound; right = IB::closed; }
            else            { min = val; left = IB::closed; right = IB::unbound; }
        } else if (match[14].matched) {
            min = max = Uview(match[14].first, match[14].length());
            left = right = IB::closed;
        }
        return true;
    }

    // Common base class for associative containers

    void IntervalAssociativeBase::parse_interval_set_string(Uview src, views& dst) {
        dst.clear();
        if (src.empty() || src == "{}")
            return;
        if (src.front() != '{' || src.back() != '}') {
            dst.push_back(src);
            return;
        }
        size_t i = 1, len = src.size() - 1;
        while (i < len) {
            size_t depth = 0, j = i;
            while (j < len && (depth > 0 || src[j] != ',')) {
                if (src[j] == '(' || src[j] == '[')
                    ++depth;
                else if (src[j] == ')' || src[j] == ']')
                    --depth;
                ++j;
            }
            dst.push_back(src.substr(i, j - i));
            i = j + 1;
        }
    }

    bool IntervalAssociativeBase::parse_interval_map_string(Uview src, view_pairs& dst) {
        views pairs;
        parse_interval_set_string(src, pairs);
        for (auto& p: pairs) {
            auto i = p.find(':');
            if (i == npos)
                return false;
            auto a = p.substr(0, i);
            auto b = p.substr(i + 1, npos);
            dst.push_back({a, b});
        }
        return true;
    }

}
