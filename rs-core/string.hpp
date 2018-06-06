#pragma once

#include "rs-core/common.hpp"
#include "rs-core/meta.hpp"
#include <algorithm>
#include <array>
#include <memory>
#include <new>
#include <ostream>
#include <regex>
#include <string>
#include <string_view>
#include <typeindex>
#include <typeinfo>
#include <utility>

#ifdef __GNUC__
    #include <cxxabi.h>
#endif

namespace RS {

    // Needed early

    template <typename InputRange>
    std::string join(const InputRange& range, std::string_view delim = {}, bool term = false) {
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
                return UtfConvert<std::u32string_view, S>()(u);
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
            size_t operator()(std::basic_string_view<C> sv) const noexcept {
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
            size_t operator()(std::basic_string_view<C> sv) const noexcept {
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
            size_t operator()(std::basic_string_view<C> sv) const noexcept {
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

    inline bool ascii_icase_equal(std::string_view lhs, std::string_view rhs) noexcept {
        if (lhs.size() != rhs.size())
            return false;
        for (size_t i = 0, n = lhs.size(); i < n; ++i)
            if (ascii_toupper(lhs[i]) != ascii_toupper(rhs[i]))
                return false;
        return true;
    }

    inline bool ascii_icase_less(std::string_view lhs, std::string_view rhs) noexcept {
        for (size_t i = 0, n = std::min(lhs.size(), rhs.size()); i < n; ++i) {
            char a = ascii_toupper(lhs[i]), b = ascii_toupper(rhs[i]);
            if (a != b)
                return uint8_t(a) < uint8_t(b);
        }
        return lhs.size() < rhs.size();
    }

    inline bool starts_with(std::string_view str, std::string_view prefix) noexcept {
        return str.size() >= prefix.size()
            && memcmp(str.data(), prefix.data(), prefix.size()) == 0;
    }

    inline bool ends_with(std::string_view str, std::string_view suffix) noexcept {
        return str.size() >= suffix.size()
            && memcmp(str.data() + str.size() - suffix.size(), suffix.data(), suffix.size()) == 0;
    }

    inline bool string_is_ascii(std::string_view str) noexcept {
        return std::find_if(str.begin(), str.end(), [] (char c) { return c & 0x80; }) == str.end();
    }

    // String manipulation functions

    namespace RS_Detail {

        inline void catstr_helper(std::string&) noexcept {}

        template <typename T, typename... Args>
        void catstr_helper(std::string& dst, const T& t, const Args&... args) {
            dst += t;
            catstr_helper(dst, args...);
        }

    }

    inline std::string add_prefix(std::string_view s, std::string_view prefix) {
        if (starts_with(s, prefix))
            return std::string(s);
        std::string r(prefix);
        r += s;
        return r;
    }

    inline std::string add_suffix(std::string_view s, std::string_view suffix) {
        if (ends_with(s, suffix))
            return std::string(s);
        std::string r(s);
        r += suffix;
        return r;
    }

    template <typename... Args>
    std::string catstr(const Args&... args) {
        std::string s;
        RS_Detail::catstr_helper(s, args...);
        return s;
    }

    inline std::string drop_prefix(std::string_view s, std::string_view prefix) {
        if (starts_with(s, prefix))
            return std::string(s.data() + prefix.size(), s.size() - prefix.size());
        else
            return std::string(s);
    }

    inline std::string drop_suffix(std::string_view s, std::string_view suffix) {
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

    inline std::string linearize(std::string_view str) {
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

    inline std::string pad_left(std::string_view str, size_t len, char pad = ' ') {
        std::string s(str);
        if (len > s.size())
            s.insert(0, len - s.size(), pad);
        return s;
    }

    inline std::string pad_right(std::string_view str, size_t len, char pad = ' ') {
        std::string s(str);
        if (len > s.size())
            s.append(len - s.size(), pad);
        return s;
    }

    inline std::pair<std::string_view, std::string_view> partition_at(std::string_view str, std::string_view delim) {
        if (delim.empty())
            return {str, {}};
        size_t p = str.find(delim);
        if (p == npos)
            return {str, {}};
        else
            return {make_view(str, 0, p), make_view(str, p + delim.size(), npos)};
    }

    inline std::pair<std::string_view, std::string_view> partition_by(std::string_view str, std::string_view delims = ascii_whitespace) {
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

    inline std::string repeat(std::string_view s, size_t n, std::string_view delim = {}) {
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

    inline std::string replace(std::string_view s, std::string_view target, std::string_view subst, size_t n = npos) {
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
    void split(std::string_view src, OutputIterator dst, std::string_view delim = ascii_whitespace) {
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

    inline Strings splitv(std::string_view src, std::string_view delim = ascii_whitespace) {
        Strings v;
        split(src, append(v), delim);
        return v;
    }

    template <typename OutputIterator>
    void split_lines(std::string_view src, OutputIterator dst) {
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

    inline Strings splitv_lines(std::string_view src) {
        Strings v;
        split_lines(src, append(v));
        return v;
    }

    inline std::string trim(std::string_view str, std::string_view chars = ascii_whitespace) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return std::string(str.substr(pos, str.find_last_not_of(chars) + 1 - pos));
    }

    inline std::string trim_left(std::string_view str, std::string_view chars = ascii_whitespace) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return std::string(str.substr(pos, npos));
    }

    inline std::string trim_right(std::string_view str, std::string_view chars = ascii_whitespace) {
        return std::string(str.substr(0, str.find_last_not_of(chars) + 1));
    }

    inline std::string unqualify(std::string_view str, std::string_view delims = ".:") {
        if (delims.empty())
            return std::string(str);
        size_t pos = str.find_last_of(delims);
        if (pos == npos)
            return std::string(str);
        else
            return std::string(str.substr(pos + 1, npos));
    }

    // String formatting functions

    template <size_t N>
    Ustring hex(const std::array<uint8_t, N>& bytes) {
        using namespace RS_Detail;
        Ustring s;
        for (auto b: bytes)
            append_hex_byte(b, s);
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

    inline Ustring hexdump(std::string_view str, size_t block = 0) {
        return hexdump(str.data(), str.size(), block);
    }

    inline Ustring tf(bool b) { return b ? "true" : "false"; }
    inline Ustring yn(bool b) { return b ? "yes" : "no"; }

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

    // HTML/XML tags

    class Tag {
    public:
        Tag() = default;
        Tag(std::ostream& out, std::string_view element) {
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
    void tagged(std::ostream& out, std::string_view element, const T& t) {
        Tag html(out, element);
        out << t;
    }

    template <typename... Args>
    void tagged(std::ostream& out, std::string_view element, const Args&... args) {
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

        inline Strings operator""_qw(const char* p, size_t n) {
            return splitv(Ustring(p, n));
        }

    }

}
