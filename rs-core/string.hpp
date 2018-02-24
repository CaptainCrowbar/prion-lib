#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <ostream>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

#ifdef __GNUC__
    #include <cxxabi.h>
#endif

namespace RS {

    // These are defined out of order so other functions can use them

    template <typename C> basic_string_view<C> make_view(const std::basic_string<C>& s) noexcept { return s; }
    template <typename C> basic_string_view<C> make_view(basic_string_view<C> s) noexcept { return s; }
    template <typename C> basic_string_view<C> make_view(const C* s) noexcept { if (s) return s; else return {}; }

    template <typename S>
    auto make_view(const S& s, size_t pos, size_t len) noexcept {
        auto v = make_view(s);
        using SV = decltype(v);
        pos = std::min(pos, v.size());
        len = std::min(len, v.size() - pos);
        return SV(v.data() + pos, len);
    }

    template <typename InputRange>
    std::string join(const InputRange& range, string_view delim = {}, bool term = false) {
        std::string result;
        for (auto& s: range) {
            result += make_view(s);
            result += delim;
        }
        if (! term && ! result.empty() && ! delim.empty())
            result.resize(result.size() - delim.size());
        return result;
    }

    // Case conversion functions

    constexpr bool ascii_iscntrl(char c) noexcept { return uint8_t(c) <= 31 || c == 127; }
    constexpr bool ascii_isdigit(char c) noexcept { return c >= '0' && c <= '9'; }
    constexpr bool ascii_isgraph(char c) noexcept { return c >= '!' && c <= '~'; }
    constexpr bool ascii_islower(char c) noexcept { return c >= 'a' && c <= 'z'; }
    constexpr bool ascii_isprint(char c) noexcept { return c >= ' ' && c <= '~'; }
    constexpr bool ascii_ispunct(char c) noexcept { return (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~'); }
    constexpr bool ascii_isspace(char c) noexcept { return (c >= '\t' && c <= '\r') || c == ' '; }
    constexpr bool ascii_isupper(char c) noexcept { return c >= 'A' && c <= 'Z'; }
    constexpr bool ascii_isalpha(char c) noexcept { return ascii_islower(c) || ascii_isupper(c); }
    constexpr bool ascii_isalnum(char c) noexcept { return ascii_isalpha(c) || ascii_isdigit(c); }
    constexpr bool ascii_isxdigit(char c) noexcept { return ascii_isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'); }
    constexpr bool ascii_isalnum_w(char c) noexcept { return ascii_isalnum(c) || c == '_'; }
    constexpr bool ascii_isalpha_w(char c) noexcept { return ascii_isalpha(c) || c == '_'; }
    constexpr bool ascii_ispunct_w(char c) noexcept { return ascii_ispunct(c) && c != '_'; }
    constexpr char ascii_tolower(char c) noexcept { return ascii_isupper(c) ? char(c + 32) : c; }
    constexpr char ascii_toupper(char c) noexcept { return ascii_islower(c) ? char(c - 32) : c; }

    inline std::string ascii_lowercase(string_view s) {
        Ustring r(s);
        std::transform(r.begin(), r.end(), r.begin(), ascii_tolower);
        return r;
    }

    inline std::string ascii_uppercase(string_view s) {
        Ustring r(s);
        std::transform(r.begin(), r.end(), r.begin(), ascii_toupper);
        return r;
    }

    inline std::string ascii_titlecase(string_view s) {
        Ustring r(s);
        bool was_alpha = false;
        for (char& c: r) {
            if (was_alpha)
                c = ascii_tolower(c);
            else
                c = ascii_toupper(c);
            was_alpha = ascii_isalpha(c);
        }
        return r;
    }

    inline std::string ascii_sentencecase(string_view s) {
        Ustring r(s);
        bool new_sentence = true, was_break = false;
        for (char& c: r) {
            if (c == '\n' || c == '\f' || c == '\r') {
                if (was_break)
                    new_sentence = true;
                was_break = true;
            } else {
                was_break = false;
                if (c == '.') {
                    new_sentence = true;
                } else if (new_sentence && ascii_isalpha(c)) {
                    c = ascii_toupper(c);
                    new_sentence = false;
                }
            }
        }
        return r;
    }

    inline Strings name_breakdown(Uview name) {
        static const std::regex word_pattern(
            "[[:digit:]]+"
            "|[[:lower:]]+"
            "|[[:upper:]]+(?![[:lower:]])"
            "|[[:upper:]][[:lower:]]*"
        );
        const char* cbegin = name.data();
        const char* cend = cbegin + name.size();
        std::cregex_iterator rbegin(cbegin, cend, word_pattern), rend;
        Strings vec;
        std::transform(rbegin, rend, append(vec), [] (auto& m) { return m.str(); });
        return vec;
    }

    inline Ustring name_to_lower_case(Uview name, char delim = '_') {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        std::transform(vec.begin(), vec.end(), vec.begin(), ascii_lowercase);
        return join(vec, dchars);
    }

    inline Ustring name_to_upper_case(Uview name, char delim = '_') {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        std::transform(vec.begin(), vec.end(), vec.begin(), ascii_uppercase);
        return join(vec, dchars);
    }

    inline Ustring name_to_title_case(Uview name, char delim = '\0') {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        std::transform(vec.begin(), vec.end(), vec.begin(), ascii_titlecase);
        return join(vec, dchars);
    }

    inline Ustring name_to_camel_case(Uview name, char delim = '\0') {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        if (! vec.empty()) {
            vec[0] = ascii_lowercase(vec[0]);
            std::transform(vec.begin() + 1, vec.end(), vec.begin() + 1, ascii_titlecase);
        }
        return join(vec, dchars);
    }

    inline Ustring name_to_sentence_case(Uview name, char delim = ' ') {
        const char dchars[] = {delim, '\0'};
        Strings vec = name_breakdown(name);
        if (! vec.empty()) {
            vec[0] = ascii_titlecase(vec[0]);
            std::transform(vec.begin() + 1, vec.end(), vec.begin() + 1, ascii_lowercase);
        }
        return join(vec, dchars);
    }

    // Character functions

    template <typename T> constexpr T char_to(char c) noexcept { return T(uint8_t(c)); }

    // Construction functions

    inline Ustring dent(size_t depth) { return Ustring(4 * depth, ' '); }

    // Unicode functions

    namespace RS_Detail {

        template <typename SV, typename S, size_t N1 = sizeof(typename SV::value_type), size_t N2 = sizeof(typename S::value_type)>
        struct UtfConvert {
            S operator()(SV sv) const {
                auto u = UtfConvert<SV, std::u32string>()(sv);
                return UtfConvert<u32string_view, S>()(u);
            }
        };

        template <typename SV, typename S, size_t N>
        struct UtfConvert<SV, S, N, N> {
            S operator()(SV sv) const {
                using C = typename S::value_type;
                return S(reinterpret_cast<const C*>(sv.data()), sv.size());
            }
        };

        template <typename SV, typename S>
        struct UtfConvert<SV, S, 1, 4> {
            S operator()(SV sv) const {
                using C = typename S::value_type;
                auto p = reinterpret_cast<const uint8_t*>(sv.data());
                auto q = p + sv.size();
                S s;
                while (p < q) {
                    if (*p <= 0x7f) {
                        s += C(*p++);
                    } else if (p[0] <= 0xc1 || p[0] >= 0xf5 || q - p < 2 || p[1] < 0x80 || p[1] > 0xbf
                            || (p[0] == 0xe0 && p[1] < 0xa0) || (p[0] == 0xed && p[1] > 0x9f)
                            || (p[0] == 0xf0 && p[1] < 0x90) || (p[0] == 0xf4 && p[1] > 0x8f)) {
                        s += C(0xfffd);
                        ++p;
                    } else if (p[0] <= 0xdf) {
                        s += (C(p[0] & 0x1f) << 6) | C(p[1] & 0x3f);
                        p += 2;
                    } else if (p[0] <= 0xef) {
                        if (q - p < 3 || p[2] < 0x80 || p[2] > 0xbf) {
                            s += C(0xfffd);
                            p += 2;
                        } else {
                            s += (C(p[0] & 0x0f) << 12) | (C(p[1] & 0x3f) << 6) | C(p[2] & 0x3f);
                            p += 3;
                        }
                    } else {
                        if (q - p < 3 || p[2] < 0x80 || p[2] > 0xbf) {
                            s += C(0xfffd);
                            p += 2;
                        } else if (q - p < 4 || p[3] < 0x80 || p[3] > 0xbf) {
                            s += C(0xfffd);
                            p += 3;
                        } else {
                            s += (C(p[0] & 0x07) << 18) | (C(p[1] & 0x3f) << 12)
                                | (C(p[2] & 0x3f) << 6) | C(p[3] & 0x3f);
                            p += 4;
                        }
                    }
                }
                return s;
            }
        };

        template <typename SV, typename S>
        struct UtfConvert<SV, S, 2, 4> {
            S operator()(SV sv) const {
                using C = typename S::value_type;
                auto p = reinterpret_cast<const uint16_t*>(sv.data());
                auto q = p + sv.size();
                S s;
                while (p < q) {
                    if (*p <= 0xd7ff || *p >= 0xe000) {
                        s += C(*p++);
                    } else if (q - p >= 2 && p[0] >= 0xd800 && p[0] <= 0xdbff && p[1] >= 0xdc00 && p[1] <= 0xdfff) {
                        s += C(0x10000) + ((C(p[0]) & 0x3ff) << 10) + (C(p[1]) & 0x3ff);
                        p += 2;
                    } else {
                        s += C(0xfffd);
                        ++p;
                    }
                }
                return s;
            }
        };

        template <typename SV, typename S>
        struct UtfConvert<SV, S, 4, 1> {
            S operator()(SV sv) const {
                using C = typename S::value_type;
                S s;
                for (auto c: sv) {
                    auto u = uint32_t(c);
                    if (u <= 0x7f) {
                        s += C(u);
                    } else if (u <= 0x7ff) {
                        s += C(0xc0 | (u >> 6));
                        s += C(0x80 | (u & 0x3f));
                    } else if (u <= 0xd7ff || (u >= 0xe000 && u <= 0xffff)) {
                        s += C(0xe0 | (u >> 12));
                        s += C(0x80 | ((u >> 6) & 0x3f));
                        s += C(0x80 | (u & 0x3f));
                    } else if (u >= 0x10000 && u <= 0x10ffff) {
                        s += C(0xf0 | (u >> 18));
                        s += C(0x80 | ((u >> 12) & 0x3f));
                        s += C(0x80 | ((u >> 6) & 0x3f));
                        s += C(0x80 | (u & 0x3f));
                    } else {
                        s += C(0xef);
                        s += C(0xbf);
                        s += C(0xbf);
                    }
                }
                return s;
            }
        };

        template <typename SV, typename S>
        struct UtfConvert<SV, S, 4, 2> {
            S operator()(SV sv) const {
                using C = typename S::value_type;
                S s;
                for (auto c: sv) {
                    auto u = uint32_t(c);
                    if (u <= 0xd7ff || (u >= 0xe000 && u <= 0xffff)) {
                        s += C(u);
                    } else if (u >= 0x10000 && u <= 0x10ffff) {
                        s += C(0xd800 + ((u >> 10) - 0x40));
                        s += C(0xdc00 + (u & 0x3ff));
                    } else {
                        s += C(0xfffd);
                    }
                }
                return s;
            }
        };

        inline size_t utf8_check_ahead(const uint8_t* ptr, size_t len, size_t pos, size_t n, uint8_t a1, uint8_t b1) {
            if (pos + n > len)
                return 0;
            if (ptr[pos + 1] < a1 || ptr[pos + 1] > b1)
                return 0;
            for (size_t i = 2; i < n; ++i)
                if (ptr[pos + i] < 0x80 || ptr[pos + i] > 0xbf)
                    return 0;
            return n;
        }

        template <typename C, size_t N = sizeof(C)>
        struct UtfValidate;

        template <typename C>
        struct UtfValidate<C, 1> {
            size_t operator()(basic_string_view<C> sv) const noexcept {
                auto ptr = reinterpret_cast<const uint8_t*>(sv.data());
                size_t units = sv.size(), i = 0;
                while (i < units) {
                    size_t n = 0;
                    if (ptr[i] <= 0x7f)
                        n = 1;
                    else if (ptr[i] <= 0xc1)
                        break;
                    else if (ptr[i] <= 0xdf)
                        n = utf8_check_ahead(ptr, units, i, 2, 0x80, 0xbf);
                    else if (ptr[i] <= 0xe0)
                        n = utf8_check_ahead(ptr, units, i, 3, 0xa0, 0xbf);
                    else if (ptr[i] <= 0xec)
                        n = utf8_check_ahead(ptr, units, i, 3, 0x80, 0xbf);
                    else if (ptr[i] <= 0xed)
                        n = utf8_check_ahead(ptr, units, i, 3, 0x80, 0x9f);
                    else if (ptr[i] <= 0xef)
                        n = utf8_check_ahead(ptr, units, i, 3, 0x80, 0xbf);
                    else if (ptr[i] <= 0xf0)
                        n = utf8_check_ahead(ptr, units, i, 4, 0x90, 0xbf);
                    else if (ptr[i] <= 0xf3)
                        n = utf8_check_ahead(ptr, units, i, 4, 0x80, 0xbf);
                    else if (ptr[i] <= 0xf4)
                        n = utf8_check_ahead(ptr, units, i, 4, 0x80, 0x8f);
                    if (n == 0)
                        break;
                    i += n;
                }
                return i;
            }
        };

        template <typename C>
        struct UtfValidate<C, 2> {
            size_t operator()(basic_string_view<C> sv) const noexcept {
                auto ptr = reinterpret_cast<const uint16_t*>(sv.data());
                size_t units = sv.size(), i = 0;
                while (i < units) {
                    if (ptr[i] <= 0xd7ff || ptr[i] >= 0xe000) {
                        ++i;
                    } else if (ptr[i] <= 0xdbff) {
                        if (units - i < 2 || ptr[i + 1] <= 0xdbff || ptr[i + 1] >= 0xe000)
                            break;
                        i += 2;
                    } else {
                        break;
                    }
                }
                return i;
            }
        };

        template <typename C>
        struct UtfValidate<C, 4> {
            size_t operator()(basic_string_view<C> sv) const noexcept {
                auto ptr = reinterpret_cast<const uint32_t*>(sv.data());
                size_t units = sv.size(), i = 0;
                for (; i < units; ++i)
                    if ((ptr[i] >= 0xd800 && ptr[i] <= 0xdfff) || ptr[i] >= 0x110000)
                        break;
                return i;
            }
        };

    }

    template <typename S2, typename S>
    S2 uconv(const S& s) {
        auto sv = make_view(s);
        return RS_Detail::UtfConvert<decltype(sv), S2>()(sv);
    }

    template <typename S>
    size_t ulength(const S& s) noexcept {
        auto sv = make_view(s);
        using C = typename decltype(sv)::value_type;
        switch (sizeof(C)) {
            case 1:
                return std::count_if(sv.begin(), sv.end(), [] (C c) {
                    auto b = uint8_t(c);
                    return b <= 0x7f || (b >= 0xc2 && b <= 0xf4);
                });
            case 2:
                return std::count_if(sv.begin(), sv.end(), [] (C c) {
                    auto u = uint16_t(c);
                    return u <= 0xdbff || u >= 0xe000;
                });
            default: {
                return sv.size();
            }
        }
    }

    template <typename S>
    bool uvalid(const S& s, size_t& n) noexcept {
        auto sv = make_view(s);
        using C = typename decltype(sv)::value_type;
        n = RS_Detail::UtfValidate<C>()(sv);
        return n == sv.size();
    }

    template <typename S>
    bool uvalid(const S& s) noexcept {
        size_t n = 0;
        return uvalid(s, n);
    }

    // String property functions

    inline bool ascii_icase_equal(string_view lhs, string_view rhs) noexcept {
        if (lhs.size() != rhs.size())
            return false;
        for (size_t i = 0, n = lhs.size(); i < n; ++i)
            if (ascii_toupper(lhs[i]) != ascii_toupper(rhs[i]))
                return false;
        return true;
    }

    inline bool ascii_icase_less(string_view lhs, string_view rhs) noexcept {
        for (size_t i = 0, n = std::min(lhs.size(), rhs.size()); i < n; ++i) {
            char a = ascii_toupper(lhs[i]), b = ascii_toupper(rhs[i]);
            if (a != b)
                return uint8_t(a) < uint8_t(b);
        }
        return lhs.size() < rhs.size();
    }

    inline bool starts_with(string_view str, string_view prefix) noexcept {
        return str.size() >= prefix.size()
            && memcmp(str.data(), prefix.data(), prefix.size()) == 0;
    }

    inline bool ends_with(string_view str, string_view suffix) noexcept {
        return str.size() >= suffix.size()
            && memcmp(str.data() + str.size() - suffix.size(), suffix.data(), suffix.size()) == 0;
    }

    inline bool string_is_ascii(string_view str) noexcept {
        return std::find_if(str.begin(), str.end(), [] (char c) { return c & 0x80; }) == str.end();
    }

    // String manipulation functions

    namespace RS_Detail {

        inline Ustring quote_string(string_view str, bool check_utf8) {
            bool allow_8bit = check_utf8 && uvalid(str);
            Ustring result = "\"";
            for (auto c: str) {
                switch (c) {
                    case 0:     result += "\\0"; break;
                    case '\t':  result += "\\t"; break;
                    case '\n':  result += "\\n"; break;
                    case '\f':  result += "\\f"; break;
                    case '\r':  result += "\\r"; break;
                    case '\"':  result += "\\\""; break;
                    case '\\':  result += "\\\\"; break;
                    default: {
                        auto b = uint8_t(c);
                        if ((b >= 0x20 && b <= 0x7e) || (allow_8bit && b >= 0x80)) {
                            result += c;
                        } else {
                            result += "\\x";
                            RS_Detail::append_hex_byte(b, result);
                        }
                        break;
                    }
                }
            }
            result += '\"';
            return result;
        }

    }

    inline std::string add_prefix(string_view s, string_view prefix) {
        if (starts_with(s, prefix))
            return std::string(s);
        std::string r(prefix);
        r += s;
        return r;
    }

    inline std::string add_suffix(string_view s, string_view suffix) {
        if (ends_with(s, suffix))
            return std::string(s);
        std::string r(s);
        r += suffix;
        return r;
    }

    inline std::string drop_prefix(string_view s, string_view prefix) {
        if (starts_with(s, prefix))
            return std::string(s.data() + prefix.size(), s.size() - prefix.size());
        else
            return std::string(s);
    }

    inline std::string drop_suffix(string_view s, string_view suffix) {
        if (ends_with(s, suffix))
            return std::string(s.data(), s.size() - suffix.size());
        else
            return std::string(s);
    }

    inline Ustring indent(Uview str, size_t depth) {
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

    inline std::string linearize(string_view str) {
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

    template <typename C>
    void null_term(std::basic_string<C>& str) noexcept {
        auto p = str.find(C(0));
        if (p != npos)
            str.resize(p);
    }

    inline std::string pad_left(string_view str, size_t len, char pad = ' ') {
        std::string s(str);
        if (len > s.size())
            s.insert(0, len - s.size(), pad);
        return s;
    }

    inline std::string pad_right(string_view str, size_t len, char pad = ' ') {
        std::string s(str);
        if (len > s.size())
            s.append(len - s.size(), pad);
        return s;
    }

    inline std::pair<string_view, string_view> partition_at(string_view str, string_view delim) {
        if (delim.empty())
            return {str, {}};
        size_t p = str.find(delim);
        if (p == npos)
            return {str, {}};
        else
            return {make_view(str, 0, p), make_view(str, p + delim.size(), npos)};
    }

    inline std::pair<string_view, string_view> partition_by(string_view str, string_view delims = ascii_whitespace) {
        if (delims.empty())
            return {str, {}};
        size_t p = str.find_first_of(delims);
        if (p == npos)
            return {str, {}};
        string_view v1 = make_view(str, 0, p), v2;
        size_t q = str.find_first_not_of(delims, p);
        if (q != npos)
            v2 = make_view(str, q, npos);
        return {v1, v2};
    }

    inline Ustring quote(string_view str) { return RS_Detail::quote_string(str, true); }
    inline Ustring bquote(string_view str) { return RS_Detail::quote_string(str, false); }

    inline std::string repeat(string_view s, size_t n, string_view delim = {}) {
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

    inline std::string replace(string_view s, string_view target, string_view subst, size_t n = npos) {
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

    template <typename OutputIterator>
    void split(string_view src, OutputIterator dst, string_view delim = ascii_whitespace) {
        if (src.empty())
            return;
        if (delim.empty()) {
            *dst = std::string(src);
            return;
        }
        size_t i = 0, j = 0, size = src.size();
        while (i < size) {
            j = src.find_first_of(delim, i);
            if (j == npos) {
                *dst = std::string(src.substr(i, npos));
                break;
            }
            if (j > i) {
                *dst = std::string(src.substr(i, j - i));
                ++dst;
            }
            i = src.find_first_not_of(delim, j);
        }
    }

    inline Strings splitv(string_view src, string_view delim = ascii_whitespace) {
        Strings v;
        split(src, append(v), delim);
        return v;
    }

    template <typename OutputIterator>
    void split_lines(string_view src, OutputIterator dst) {
        if (src.empty())
            return;
        size_t i = 0, j = 0, size = src.size();
        while (i < size) {
            j = src.find('\n', i);
            if (j == npos) {
                *dst = std::string(src.substr(i, npos));
                break;
            } else if (j - i > 0 && src[j - 1] == '\r') {
                *dst = std::string(src.substr(i, j - i - 1));
            } else {
                *dst = std::string(src.substr(i, j - i));
            }
            ++dst;
            i = j + 1;
        }
    }

    inline Strings splitv_lines(string_view src) {
        Strings v;
        split_lines(src, append(v));
        return v;
    }

    inline std::string trim(string_view str, string_view chars = ascii_whitespace) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return std::string(str.substr(pos, str.find_last_not_of(chars) + 1 - pos));
    }

    inline std::string trim_left(string_view str, string_view chars = ascii_whitespace) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return std::string(str.substr(pos, npos));
    }

    inline std::string trim_right(string_view str, string_view chars = ascii_whitespace) {
        return std::string(str.substr(0, str.find_last_not_of(chars) + 1));
    }

    inline std::string unqualify(string_view str, string_view delims = ".:") {
        if (delims.empty())
            return std::string(str);
        size_t pos = str.find_last_of(delims);
        if (pos == npos)
            return std::string(str);
        else
            return std::string(str.substr(pos + 1, npos));
    }

    // String formatting functions

    template <typename T> Ustring bin(T x, size_t digits = 8 * sizeof(T)) { return RS_Detail::int_to_string(x, 2, digits); }
    template <typename T> Ustring dec(T x, size_t digits = 1) { return RS_Detail::int_to_string(x, 10, digits); }
    template <typename T> Ustring hex(T x, size_t digits = 2 * sizeof(T)) { return RS_Detail::int_to_string(x, 16, digits); }

    template <typename T>
    Ustring fp_format(T t, char mode = 'g', int prec = 6) {
        using namespace std::literals;
        static const Ustring modes = "EFGZefgz";
        if (modes.find(mode) == npos)
            throw std::invalid_argument("Invalid floating point mode: " + quote(Ustring{mode}));
        if (t == 0) {
            switch (mode) {
                case 'E': case 'e':  return prec < 1 ? "0"s + mode + '0' : "0."s + Ustring(prec, '0') + mode + "0";
                case 'F': case 'f':  return prec < 1 ? "0"s : "0."s + Ustring(prec, '0');
                case 'G': case 'g':  return "0";
                case 'Z': case 'z':  return prec < 2 ? "0"s : "0."s + Ustring(prec - 1, '0');
                default:             break;
            }
        }
        Ustring buf(20, '\0'), fmt;
        switch (mode) {
            case 'Z':  fmt = "%#.*G"; break;
            case 'z':  fmt = "%#.*g"; break;
            default:   fmt = "%.*_"; fmt[3] = mode; break;
        }
        auto x = double(t);
        int rc = 0;
        for (;;) {
            rc = snprintf(&buf[0], buf.size(), fmt.data(), prec, x);
            if (rc < 0)
                throw std::system_error(errno, std::generic_category(), "snprintf()");
            if (size_t(rc) < buf.size())
                break;
            buf.resize(2 * buf.size());
        }
        buf.resize(rc);
        if (mode != 'F' && mode != 'f') {
            size_t p = buf.find_first_of("Ee");
            if (p == npos)
                p = buf.size();
            if (buf[p - 1] == '.') {
                buf.erase(p - 1, 1);
                --p;
            }
            if (p < buf.size()) {
                ++p;
                if (buf[p] == '+')
                    buf.erase(p, 1);
                else if (buf[p] == '-')
                    ++p;
                size_t q = std::min(buf.find_first_not_of('0', p), buf.size() - 1);
                if (q > p)
                    buf.erase(p, q - p);
            }
        }
        return buf;
    }

    inline Ustring roman(int n) {
        static constexpr std::pair<int, const char*> table[] = {
            { 900, "CM" }, { 500, "D" }, { 400, "CD" }, { 100, "C" },
            { 90, "XC" }, { 50, "L" }, { 40, "XL" }, { 10, "X" },
            { 9, "IX" }, { 5, "V" }, { 4, "IV" }, { 1, "I" },
        };
        if (n < 1)
            return {};
        Ustring s(size_t(n / 1000), 'M');
        n %= 1000;
        for (auto& t: table) {
            for (int q = n / t.first; q > 0; --q)
                s += t.second;
            n %= t.first;
        }
        return s;
    }

    inline Ustring hexdump(const void* ptr, size_t n, size_t block = 0) {
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

    inline Ustring hexdump(string_view str, size_t block = 0) { return hexdump(str.data(), str.size(), block); }
    inline Ustring tf(bool b) { return b ? "true" : "false"; }
    inline Ustring yn(bool b) { return b ? "yes" : "no"; }

    template <typename T> Ustring to_str(const T& t);

    namespace RS_Detail {

        template <typename R, typename T = RangeValue<R>>
        struct RangeToString {
            Ustring operator()(const R& r) const {
                Ustring s = "[";
                for (auto&& t: r) {
                    s += to_str(t);
                    s += ',';
                }
                if (s.size() > 1)
                    s.pop_back();
                s += ']';
                return s;
            }
        };

        template <typename R, typename K, typename T>
        struct RangeToString<R, std::pair<K, T>> {
            Ustring operator()(const R& r) const {
                Ustring s = "{";
                for (auto&& kv: r) {
                    s += to_str(kv.first);
                    s += ':';
                    s += to_str(kv.second);
                    s += ',';
                }
                if (s.size() > 1)
                    s.pop_back();
                s += '}';
                return s;
            }
        };

        template <typename R>
        struct RangeToString<R, char> {
            Ustring operator()(const R& r) const {
                using std::begin;
                using std::end;
                return Ustring(begin(r), end(r));
            }
        };

        template <typename T>
        struct ObjectToStringCategory {
            static constexpr char value =
                std::is_integral<T>::value ? 'I' :
                std::is_floating_point<T>::value ? 'F' :
                std::is_convertible<T, std::string>::value ? 'S' :
                is_range<T> ? 'R' : 'X';
        };

        template <typename T, char C = ObjectToStringCategory<T>::value>
        struct ObjectToString {
            Ustring operator()(const T& t) const {
                std::ostringstream out;
                out << t;
                return out.str();
            }
        };

        template <> struct ObjectToString<Ustring> { Ustring operator()(const Ustring& t) const { return t; } };
        template <> struct ObjectToString<Uview> { Ustring operator()(Uview t) const { return Ustring(t); } };
        template <> struct ObjectToString<char*> { Ustring operator()(char* t) const { return t ? Ustring(t) : Ustring(); } };
        template <> struct ObjectToString<const char*> { Ustring operator()(const char* t) const { return t ? Ustring(t) : Ustring(); } };
        template <size_t N> struct ObjectToString<char[N], 'S'> { Ustring operator()(const char* t) const { return Ustring(t); } };
        template <> struct ObjectToString<char> { Ustring operator()(char t) const { return {t}; } };

        template <> struct ObjectToString<std::u16string> { Ustring operator()(const std::u16string& t) const { return uconv<Ustring>(t); } };
        template <> struct ObjectToString<u16string_view> { Ustring operator()(u16string_view t) const { return uconv<Ustring>(t); } };
        template <> struct ObjectToString<char16_t*> { Ustring operator()(char16_t* t) const { return t ? uconv<Ustring>(std::u16string(t)) : Ustring(); } };
        template <> struct ObjectToString<const char16_t*> { Ustring operator()(const char16_t* t) const { return t ? uconv<Ustring>(std::u16string(t)) : Ustring(); } };
        template <size_t N> struct ObjectToString<char16_t[N], 'X'> { Ustring operator()(const char16_t* t) const { return uconv<Ustring>(std::u16string(t)); } };
        template <> struct ObjectToString<char16_t> { Ustring operator()(char16_t t) const { return uconv<Ustring>(std::u16string{t}); } };

        template <> struct ObjectToString<std::u32string> { Ustring operator()(const std::u32string& t) const { return uconv<Ustring>(t); } };
        template <> struct ObjectToString<u32string_view> { Ustring operator()(u32string_view t) const { return uconv<Ustring>(t); } };
        template <> struct ObjectToString<char32_t*> { Ustring operator()(char32_t* t) const { return t ? uconv<Ustring>(std::u32string(t)) : Ustring(); } };
        template <> struct ObjectToString<const char32_t*> { Ustring operator()(const char32_t* t) const { return t ? uconv<Ustring>(std::u32string(t)) : Ustring(); } };
        template <size_t N> struct ObjectToString<char32_t[N], 'X'> { Ustring operator()(const char32_t* t) const { return uconv<Ustring>(std::u32string(t)); } };
        template <> struct ObjectToString<char32_t> { Ustring operator()(char32_t t) const { return uconv<Ustring>(std::u32string{t}); } };

        template <> struct ObjectToString<std::wstring> { Ustring operator()(const std::wstring& t) const { return uconv<Ustring>(t); } };
        template <> struct ObjectToString<wstring_view> { Ustring operator()(wstring_view t) const { return uconv<Ustring>(t); } };
        template <> struct ObjectToString<wchar_t*> { Ustring operator()(wchar_t* t) const { return t ? uconv<Ustring>(std::wstring(t)) : Ustring(); } };
        template <> struct ObjectToString<const wchar_t*> { Ustring operator()(const wchar_t* t) const { return t ? uconv<Ustring>(std::wstring(t)) : Ustring(); } };
        template <size_t N> struct ObjectToString<wchar_t[N], 'X'> { Ustring operator()(const wchar_t* t) const { return uconv<Ustring>(std::wstring(t)); } };
        template <> struct ObjectToString<wchar_t> { Ustring operator()(wchar_t t) const { return uconv<Ustring>(std::wstring{t}); } };

        template <> struct ObjectToString<bool> { Ustring operator()(bool t) const { return t ? "true" : "false"; } };
        template <> struct ObjectToString<std::nullptr_t> { Ustring operator()(std::nullptr_t) const { return "null"; } };
        template <typename T> struct ObjectToString<T, 'I'> { Ustring operator()(T t) const { return dec(t); } };
        template <typename T> struct ObjectToString<T, 'F'> { Ustring operator()(T t) const { return fp_format(t); } };
        template <typename T> struct ObjectToString<T, 'S'> { Ustring operator()(T t) const { return static_cast<std::string>(*&t); } };
        template <typename T> struct ObjectToString<T, 'R'>: RangeToString<T> {};
        template <typename T> struct ObjectToString<std::atomic<T>, 'X'> { Ustring operator()(const std::atomic<T>& t) const { return ObjectToString<T>()(t); } };
        template <typename T1, typename T2> struct ObjectToString<std::pair<T1, T2>, 'X'>
            { Ustring operator()(const std::pair<T1, T2>& t) const { return '{' + ObjectToString<T1>()(t.first) + ',' + ObjectToString<T2>()(t.second) + '}'; } };

    }

    template <typename T> inline Ustring to_str(const T& t) { return RS_Detail::ObjectToString<T>()(t); }

    template <typename... Args>
    Ustring fmt(Uview pattern, const Args&... args) {
        Strings argstr{to_str(args)...};
        Ustring result;
        size_t i = 0, psize = pattern.size();
        while (i < psize) {
            size_t j = pattern.find('$', i);
            if (j == npos)
                j = psize;
            result.append(pattern.data() + i, j - i);
            i = j;
            while (pattern[i] == '$') {
                ++i;
                j = i;
                if (pattern[j] == '{')
                    ++j;
                size_t k = j;
                while (ascii_isdigit(pattern[k]))
                    ++k;
                if (k == j || (pattern[i] == '{' && pattern[k] != '}')) {
                    result += pattern[i++];
                    continue;
                }
                size_t a = std::stoul(Ustring(pattern.substr(j, k - j)));
                if (a > 0 && a <= argstr.size())
                    result += argstr[a - 1];
                if (pattern[k] == '}')
                    ++k;
                i = k;
            }
        }
        return result;
    }

    // String parsing functions

    inline unsigned long long binnum(string_view str) noexcept {
        std::string s(str);
        return std::strtoull(s.data(), nullptr, 2);
    }

    inline long long decnum(string_view str) noexcept {
        std::string s(str);
        return std::strtoll(s.data(), nullptr, 10);
    }

    inline unsigned long long hexnum(string_view str) noexcept {
        std::string s(str);
        return std::strtoull(s.data(), nullptr, 16);
    }

    inline double fpnum(string_view str) noexcept {
        std::string s(str);
        return std::strtod(s.data(), nullptr);
    }

    inline int64_t si_to_int(Uview str) {
        using limits = std::numeric_limits<int64_t>;
        static constexpr const char* prefixes = "KMGTPEZY";
        Ustring s(str);
        char* endp = nullptr;
        errno = 0;
        int64_t n = std::strtoll(s.data(), &endp, 10);
        if (errno == ERANGE)
            throw std::range_error("Out of range: " + quote(s));
        if (errno || endp == s.data())
            throw std::invalid_argument("Invalid number: " + quote(s));
        if (ascii_isspace(*endp))
            ++endp;
        if (n && ascii_isalpha(*endp)) {
            auto pp = strchr(prefixes, ascii_toupper(*endp));
            if (pp) {
                int64_t steps = pp - prefixes + 1;
                double limit = log10(double(limits::max()) / double(std::abs(n))) / 3;
                if (double(steps) > limit)
                    throw std::range_error("Out of range: " + quote(s));
                n *= int_power(int64_t(1000), steps);
            }
        }
        return n;
    }

    inline double si_to_float(Uview str) {
        using limits = std::numeric_limits<double>;
        static constexpr const char* prefixes = "yzafpnum kMGTPEZY";
        Ustring s(str);
        char* endp = nullptr;
        errno = 0;
        double x = std::strtod(s.data(), &endp);
        if (errno == ERANGE)
            throw std::range_error("Out of range: " + quote(s));
        if (errno || endp == s.data())
            throw std::invalid_argument("Invalid number: " + quote(s));
        if (ascii_isspace(*endp))
            ++endp;
        char c = *endp;
        if (x != 0 && ascii_isalpha(c)) {
            if (c == 'K')
                c = 'k';
            auto pp = strchr(prefixes, c);
            if (pp) {
                auto steps = pp - prefixes - 8;
                double limit = log10(limits::max() / fabs(x)) / 3;
                if (double(steps) > limit)
                    throw std::range_error("Out of range: " + quote(s));
                x *= std::pow(1000.0, steps);
            }
        }
        return x;
    }

    // HTML/XML tags

    class Tag {
    public:
        Tag() = default;
        Tag(std::ostream& out, string_view element) {
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
        ~Tag() noexcept {
            if (os)
                try { *os << end; }
                    catch (...) {}
        }
        Tag(const Tag&) = delete;
        Tag(Tag&& t) noexcept {
            if (&t != this) {
                end = std::move(t.end);
                os = t.os;
                t.os = nullptr;
            }
        }
        Tag& operator=(const Tag&) = delete;
        Tag& operator=(Tag&& t) noexcept {
            if (&t != this) {
                if (os)
                    *os << end;
                end = std::move(t.end);
                os = t.os;
                t.os = nullptr;
            }
            return *this;
        }
    private:
        std::string end;
        std::ostream* os = nullptr;
    };

    template <typename T>
    void tagged(std::ostream& out, string_view element, const T& t) {
        Tag html(out, element);
        out << t;
    }

    template <typename... Args>
    void tagged(std::ostream& out, string_view element, const Args&... args) {
        Tag html(out, element);
        tagged(out, args...);
    }

    // Type names

    inline std::string demangle(const std::string& name) {
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

    inline std::string type_name(const std::type_info& t) { return demangle(t.name()); }
    inline std::string type_name(const std::type_index& t) { return demangle(t.name()); }
    template <typename T> std::string type_name() { return type_name(typeid(T)); }
    template <typename T> std::string type_name(const T& t) { return type_name(typeid(t)); }

    // String literals

    namespace RS_Detail {

        inline auto find_leading_spaces(const Ustring& s, size_t limit = npos) noexcept {
            size_t n = 0;
            auto i = s.begin(), e = s.end();
            while (i != e && n < limit) {
                if (*i == ' ')
                    ++n;
                else if (*i == '\t')
                    n += 4;
                else
                    break;
                ++i;
            }
            return std::make_pair(i, n);
        }

    }

    namespace Literals {

        inline Strings operator""_csv(const char* p, size_t n) {
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

        inline Ustring operator""_doc(const char* p, size_t n) {
            using namespace RS_Detail;
            auto lines = splitv_lines(Ustring(p, n));
            for (auto& line: lines)
                line = trim_right(line);
            if (! lines.empty() && lines.front().empty())
                lines.erase(lines.begin());
            if (! lines.empty() && lines.back().empty())
                lines.pop_back();
            if (lines.empty())
                return {};
            size_t margin = npos;
            for (auto& line: lines) {
                if (! line.empty()) {
                    size_t spaces = find_leading_spaces(line).second;
                    margin = std::min(margin, spaces);
                }
            }
            for (auto& line: lines) {
                auto i = find_leading_spaces(line, margin).first;
                line.erase(0, i - line.begin());
                auto ls = find_leading_spaces(line);
                line.replace(0, ls.first - line.begin(), ls.second, ' ');
            }
            return join(lines, "\n", true);
        }

        inline Strings operator""_qw(const char* p, size_t n) {
            return splitv(Ustring(p, n));
        }

    }

}
