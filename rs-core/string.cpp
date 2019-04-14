#include "rs-core/string.hpp"
#include "rs-core/range.hpp"
#include "unicorn/regex.hpp"
#include "unicorn/string.hpp"
#include <algorithm>
#include <cstring>
#include <stdexcept>

using namespace RS::Literals;
using namespace RS::Range;
using namespace RS::Unicorn;
using namespace std::literals;

namespace RS {

    // Case conversion functions

    CaseWords::CaseWords(Uview text) {
        static const Regex pattern = [] {
            Ustring digit = "\\p{N}";
            Ustring mark = "(\\p{Lm}|\\p{M})*";
            Ustring ll = "(\\p{Ll}" + mark + ")";
            Ustring lo = "(\\p{Lo}" + mark + ")";
            Ustring lt = "((\\p{Lt}|\\p{Lu})" + mark + ")";
            Ustring lu = "(\\p{Lu}" + mark + ")";
            Ustring expr =
                lt + "?" + ll + "+"             // Words in lower or mixed case
                + "|" + lu + "+(?!" + ll + ")"  // Words in upper case
                + "|" + lo + "+"                // Words in uncased letters
                + "|" + digit + "+";            // Numbers
            return Regex(expr, Regex::extended | Regex::no_capture | Regex::optimize);
        }();
        pattern.grep(text) >> convert<Ustring>() >> map(str_casefold) >> append(list_);
    }

    CaseWords& CaseWords::operator+=(const CaseWords& c) {
        list_.insert(list_.end(), c.list_.begin(), c.list_.end());
        return *this;
    }

    void CaseWords::push_front(Uview text) {
        CaseWords c(text);
        list_.insert(list_.begin(), c.list_.begin(), c.list_.end());
    }

    void CaseWords::push_back(Uview text) {
        CaseWords c(text);
        list_.insert(list_.end(), c.list_.begin(), c.list_.end());
    }

    Ustring CaseWords::str(Uview format) const {
        char which = format.empty() ? 'f' : ascii_tolower(format[0]);
        Ustring delim(format, 1, npos);
        Ustring result;
        switch (which) {
            case 'c':  result = list_ >> map_head_tail(str_lowercase, str_titlecase) >> insert_between(delim) >> sum; break;
            case 'f':  result = list_ >> insert_between(delim) >> sum; break;
            case 'i':  result = list_ >> map(str_first_char<char>) >> map(str_char) >> map(str_uppercase) >> insert_after(delim) >> sum; break;
            case 'l':  result = list_ >> map(str_lowercase) >> insert_between(delim) >> sum; break;
            case 's':  result = list_ >> map_head_tail(str_titlecase, str_lowercase) >> insert_between(delim) >> sum; break;
            case 't':  result = list_ >> map(str_titlecase) >> insert_between(delim) >> sum; break;
            case 'u':  result = list_ >> map(str_uppercase) >> insert_between(delim) >> sum; break;
            default:   throw std::invalid_argument("Unknown format: " + quote(format));
        }
        return result;
    }

    // String property functions

    bool ascii_icase_equal(std::string_view lhs, std::string_view rhs) noexcept {
        if (lhs.size() != rhs.size())
            return false;
        for (size_t i = 0, n = lhs.size(); i < n; ++i)
            if (ascii_toupper(lhs[i]) != ascii_toupper(rhs[i]))
                return false;
        return true;
    }

    bool ascii_icase_less(std::string_view lhs, std::string_view rhs) noexcept {
        for (size_t i = 0, n = std::min(lhs.size(), rhs.size()); i < n; ++i) {
            char a = ascii_toupper(lhs[i]), b = ascii_toupper(rhs[i]);
            if (a != b)
                return uint8_t(a) < uint8_t(b);
        }
        return lhs.size() < rhs.size();
    }

    bool starts_with(std::string_view str, std::string_view prefix) noexcept {
        return str.size() >= prefix.size()
            && memcmp(str.data(), prefix.data(), prefix.size()) == 0;
    }

    bool ends_with(std::string_view str, std::string_view suffix) noexcept {
        return str.size() >= suffix.size()
            && memcmp(str.data() + str.size() - suffix.size(), suffix.data(), suffix.size()) == 0;
    }

    bool string_is_ascii(std::string_view str) noexcept {
        return std::find_if(str.begin(), str.end(), [] (char c) { return c & 0x80; }) == str.end();
    }

    // String manipulation functions

    std::string add_prefix(std::string_view s, std::string_view prefix) {
        if (starts_with(s, prefix))
            return std::string(s);
        std::string r(prefix);
        r += s;
        return r;
    }

    std::string add_suffix(std::string_view s, std::string_view suffix) {
        if (ends_with(s, suffix))
            return std::string(s);
        std::string r(s);
        r += suffix;
        return r;
    }

    std::string drop_prefix(std::string_view s, std::string_view prefix) {
        if (starts_with(s, prefix))
            return std::string(s.data() + prefix.size(), s.size() - prefix.size());
        else
            return std::string(s);
    }

    std::string drop_suffix(std::string_view s, std::string_view suffix) {
        if (ends_with(s, suffix))
            return std::string(s.data(), s.size() - suffix.size());
        else
            return std::string(s);
    }

    Ustring indent(Uview str, size_t depth) {
        Ustring result;
        size_t i = 0, j = 0, size = str.size();
        while (i < size) {
            j = str.find('\n', i);
            if (j == npos)
                j = size;
            if (j > i) {
                result.append(4 * depth, ' ');
                result.append(str.data() + i, j - i);
            }
            result += '\n';
            i = j + 1;
        }
        return result;
    }

    std::string linearize(std::string_view str) {
        std::string result;
        size_t i = 0, j = 0, size = str.size();
        while (j < size) {
            i = str.find_first_not_of(ascii_whitespace, j);
            if (i == npos)
                break;
            j = str.find_first_of(ascii_whitespace, i);
            if (j == npos)
                j = size;
            if (! result.empty())
                result += ' ';
            result.append(str.data() + i, j - i);
        }
        return result;
    }

    std::string pad_left(std::string_view str, size_t len, char pad) {
        std::string s(str);
        if (len > s.size())
            s.insert(0, len - s.size(), pad);
        return s;
    }

    std::string pad_right(std::string_view str, size_t len, char pad) {
        std::string s(str);
        if (len > s.size())
            s.append(len - s.size(), pad);
        return s;
    }

    std::pair<std::string_view, std::string_view> partition_at(std::string_view str, std::string_view delim) {
        if (delim.empty())
            return {str, {}};
        size_t p = str.find(delim);
        if (p == npos)
            return {str, {}};
        else
            return {make_view(str, 0, p), make_view(str, p + delim.size(), npos)};
    }

    std::pair<std::string_view, std::string_view> partition_by(std::string_view str, std::string_view delims) {
        if (delims.empty())
            return {str, {}};
        size_t p = str.find_first_of(delims);
        if (p == npos)
            return {str, {}};
        std::string_view v1 = make_view(str, 0, p), v2;
        size_t q = str.find_first_not_of(delims, p);
        if (q != npos)
            v2 = make_view(str, q, npos);
        return {v1, v2};
    }

    std::string repeat(std::string_view s, size_t n, std::string_view delim) {
        if (n == 0)
            return {};
        std::string r(s);
        if (n == 1)
            return r;
        r.reserve(n * s.size());
        size_t reps = 1, half = n / 2;
        for (; reps <= half; reps *= 2) {
            size_t rlen = r.size();
            r += delim;
            r.append(r, 0, rlen);
        }
        if (reps < n) {
            r += delim;
            r += repeat(s, n - reps, delim);
        }
        return r;
    }

    std::string replace(std::string_view s, std::string_view target, std::string_view subst, size_t n) {
        if (target.empty() || n == 0)
            return std::string(s);
        std::string r;
        size_t i = 0, j = 0, slen = s.size(), tlen = target.size();
        for (size_t k = 0; k < n && i < slen; ++k) {
            j = s.find(target, i);
            if (j == npos)
                break;
            r.append(s.data() + i, j - i);
            r += subst;
            i = j + tlen;
        }
        r.append(s.data() + i, s.size() - i);
        return r;
    }

    Strings splitv(std::string_view src, std::string_view delim) {
        Strings v;
        split(src, append(v), delim);
        return v;
    }

    Strings splitv_lines(std::string_view src) {
        Strings v;
        split_lines(src, append(v));
        return v;
    }

    std::string trim(std::string_view str, std::string_view chars) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return std::string(str.substr(pos, str.find_last_not_of(chars) + 1 - pos));
    }

    std::string trim_left(std::string_view str, std::string_view chars) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return std::string(str.substr(pos, npos));
    }

    std::string trim_right(std::string_view str, std::string_view chars) {
        return std::string(str.substr(0, str.find_last_not_of(chars) + 1));
    }

    // String formatting functions

    Ustring hexdump(const void* ptr, size_t n, size_t block) {
        if (ptr == nullptr || n == 0)
            return {};
        Ustring result;
        result.reserve(3 * n);
        size_t col = 0;
        auto bptr = static_cast<const uint8_t*>(ptr);
        for (size_t i = 0; i < n; ++i) {
            RS_Detail::append_hex_byte(bptr[i], result);
            if (++col == block) {
                result += '\n';
                col = 0;
            } else {
                result += ' ';
            }
        }
        result.pop_back();
        if (block)
            result += '\n';
        return result;
    }

    Ustring hexdump(std::string_view str, size_t block) {
        return hexdump(str.data(), str.size(), block);
    }

    // HTML/XML tags

    Tag::Tag(std::ostream& out, std::string_view element) {
        std::string content = trim_right(element, "\n");
        size_t lfs = element.size() - content.size();
        content = trim(content, "\t\n\f\r <>");
        if (content.empty())
            return;
        std::string start = '<' + content + '>';
        if (content.back() == '/') {
            if (lfs)
                start += '\n';
            out << start;
            return;
        }
        auto cut = std::find_if_not(content.begin(), content.end(), ascii_isalnum_w);
        std::string tag(content.begin(), cut);
        end = "</" + tag + '>';
        if (lfs >= 2)
            start += '\n';
        if (lfs)
            end += '\n';
        out << start;
        os = &out;
    }

    Tag::~Tag() noexcept {
        if (os)
            try { *os << end; }
                catch (...) {}
    }

    Tag::Tag(Tag&& t) noexcept {
        if (&t != this) {
            end = std::move(t.end);
            os = t.os;
            t.os = nullptr;
        }
    }

    Tag& Tag::operator=(Tag&& t) noexcept {
        if (&t != this) {
            if (os)
                *os << end;
            end = std::move(t.end);
            os = t.os;
            t.os = nullptr;
        }
        return *this;
    }

    // String literals

    namespace Literals {

        Strings operator""_csv(const char* p, size_t n) {
            if (n == 0)
                return {};
            Strings v;
            const char* q = nullptr;
            const char* end = p + n;
            for (;;) {
                q = static_cast<const char*>(memchr(p, ',', end - p));
                if (! q)
                    q = end;
                v.push_back(trim(Ustring(p, q)));
                if (q == end)
                    break;
                p = q + 1;
                if (p == end) {
                    v.push_back({});
                    break;
                }
            }
            return v;
        }

        Ustring operator""_doc(const char* p, size_t n) {
            Strings lines;
            size_t i = 0, margin = npos;
            while (i < n) {
                auto lf = static_cast<const char*>(std::memchr(p + i, '\n', n - i));
                size_t len = lf ? lf - p - i : n - i;
                lines.emplace_back(p + i, len);
                auto& line = lines.back();
                if (! line.empty()) {
                    size_t spaces = line.find_first_not_of("\t ");
                    margin = std::min({margin, spaces, line.size()});
                }
                i += len + 1;
            }
            Ustring result;
            for (auto& line: lines) {
                if (! line.empty())
                    result.append(line, margin, npos);
                result += '\n';
            }
            if (result.empty() || result == "\n" || result == "\n\n")
                return {};
            if (result[0] == '\n')
                result.erase(0, 1);
            if (ends_with(result, "\n\n"))
                result.pop_back();
            return result;
        }

        Strings operator""_qw(const char* p, size_t n) {
            return splitv(Ustring(p, n));
        }

    }

}
