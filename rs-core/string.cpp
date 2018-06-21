#include "rs-core/string.hpp"
#include <algorithm>
#include <memory>
#include <new>

#ifdef __GNUC__
    #include <cxxabi.h>
#endif

namespace RS {

    // Case conversion functions

    namespace {

        char name_char_type(char c) noexcept {
            return ascii_isdigit(c) ? 'd' :
                ascii_islower(c) ? 'l' :
                ascii_isupper(c) ? 'u' : 'x';
        }

    }

    Strings name_breakdown(Uview name) {
        Strings vec;
        auto i = name.begin(), end = name.end();
        while (i != end) {
            while (i != end && name_char_type(*i) == 'x')
                ++i;
            if (i == end)
                break;
            char ctype = name_char_type(*i);
            auto j = i + 1;
            if (ctype == 'u') {
                while (j != end && name_char_type(*j) == 'u')
                    ++j;
                if (name_char_type(*j) == 'l') {
                    if (j - i == 1) {
                        ++j;
                        while (j != end && name_char_type(*j) == 'l')
                            ++j;
                    } else {
                        --j;
                    }
                }
            } else {
                while (j != end && name_char_type(*j) == ctype)
                    ++j;
            }
            vec.push_back(Ustring(i, j));
            i = j;
        }
        return vec;
    }

    Ustring name_to_lower_case(Uview name, char delim) {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        std::transform(vec.begin(), vec.end(), vec.begin(), ascii_lowercase);
        return join(vec, dchars);
    }

    Ustring name_to_upper_case(Uview name, char delim) {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        std::transform(vec.begin(), vec.end(), vec.begin(), ascii_uppercase);
        return join(vec, dchars);
    }

    Ustring name_to_title_case(Uview name, char delim) {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        std::transform(vec.begin(), vec.end(), vec.begin(), ascii_titlecase);
        return join(vec, dchars);
    }

    Ustring name_to_camel_case(Uview name, char delim) {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        if (! vec.empty()) {
            vec[0] = ascii_lowercase(vec[0]);
            std::transform(vec.begin() + 1, vec.end(), vec.begin() + 1, ascii_titlecase);
        }
        return join(vec, dchars);
    }

    Ustring name_to_sentence_case(Uview name, char delim) {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        if (! vec.empty()) {
            vec[0] = ascii_titlecase(vec[0]);
            std::transform(vec.begin() + 1, vec.end(), vec.begin() + 1, ascii_lowercase);
        }
        return join(vec, dchars);
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

    std::string unqualify(std::string_view str, std::string_view delims) {
        if (delims.empty())
            return std::string(str);
        size_t pos = str.find_last_of(delims);
        if (pos == npos)
            return std::string(str);
        else
            return std::string(str.substr(pos + 1, npos));
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

    // Type names

    std::string demangle(const std::string& name) {
        #ifdef __GNUC__
            auto mangled = name;
            std::shared_ptr<char> demangled;
            int status = 0;
            for (;;) {
                if (mangled.empty())
                    return name;
                demangled.reset(abi::__cxa_demangle(mangled.data(), nullptr, nullptr, &status), free);
                if (status == -1)
                    throw std::bad_alloc();
                if (status == 0 && demangled)
                    return demangled.get();
                if (mangled[0] != '_')
                    return name;
                mangled.erase(0, 1);
            }
        #else
            return name;
        #endif
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
            using namespace RS_Detail;
            Uview text(p, n);
            size_t cut = text.find_last_of('\n') + 1, indent = 0;
            if (text.find_first_not_of("\t ", cut) == npos) {
                for (size_t i = cut; i < n; ++i)
                    indent += text[i] == '\t' ? 4 : 1;
                text = Uview(p, cut);
            }
            auto lines = splitv_lines(text);
            for (auto& line: lines) {
                line = trim_right(line);
                size_t start = line.find_first_not_of("\t ");
                if (start == npos) {
                    line.clear();
                } else {
                    size_t leading = 0;
                    for (size_t i = 0; i < start; ++i)
                        leading += line[i] == '\t' ? 4 : 1;
                    leading = leading > indent ? leading - indent : 0;
                    line.replace(0, start, leading, ' ');
                }
            }
            if (! lines.empty() && lines.front().empty())
                lines.erase(lines.begin());
            return join(lines, "\n", true);
        }

        Strings operator""_qw(const char* p, size_t n) {
            return splitv(Ustring(p, n));
        }

    }

}
