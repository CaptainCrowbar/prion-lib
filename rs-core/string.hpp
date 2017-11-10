#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <atomic>
#include <cmath>
#include <cstddef>
#include <iterator>
#include <limits>
#include <memory>
#include <new>
#include <ostream>
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

    // Character functions

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
    constexpr char ascii_tolower(char c) noexcept { return ascii_isupper(c) ? c + 32 : c; }
    constexpr char ascii_toupper(char c) noexcept { return ascii_islower(c) ? c - 32 : c; }
    template <typename T> constexpr T char_to(char c) noexcept { return T(uint8_t(c)); }

    // Unicode functions

    namespace RS_Detail {

        template <typename Src, typename Dst, size_t N1 = sizeof(typename Src::value_type), size_t N2 = sizeof(typename Dst::value_type)>
        struct UtfConvert {
            Dst operator()(const Src& s) const {
                return UtfConvert<std::u32string, Dst>()(UtfConvert<Src, std::u32string>()(s));
            }
        };

        template <typename Src, typename Dst, size_t N>
        struct UtfConvert<Src, Dst, N, N> {
            Dst operator()(const Src& s) const {
                using dc = typename Dst::value_type;
                return Dst(reinterpret_cast<const dc*>(s.data()), s.size());
            }
        };

        template <typename Src, typename Dst>
        struct UtfConvert<Src, Dst, 1, 4> {
            Dst operator()(const Src& s) const {
                using dc = typename Dst::value_type;
                auto p = reinterpret_cast<const uint8_t*>(s.data());
                auto q = p + s.size();
                Dst d;
                while (p < q) {
                    if (*p <= 0x7f) {
                        d += dc(*p++);
                    } else if (p[0] <= 0xc1 || p[0] >= 0xf5 || q - p < 2 || p[1] < 0x80 || p[1] > 0xbf
                            || (p[0] == 0xe0 && p[1] < 0xa0) || (p[0] == 0xed && p[1] > 0x9f)
                            || (p[0] == 0xf0 && p[1] < 0x90) || (p[0] == 0xf4 && p[1] > 0x8f)) {
                        d += dc(0xfffd);
                        ++p;
                    } else if (p[0] <= 0xdf) {
                        d += (dc(p[0] & 0x1f) << 6) | dc(p[1] & 0x3f);
                        p += 2;
                    } else if (p[0] <= 0xef) {
                        if (q - p < 3 || p[2] < 0x80 || p[2] > 0xbf) {
                            d += dc(0xfffd);
                            p += 2;
                        } else {
                            d += (dc(p[0] & 0x0f) << 12) | (dc(p[1] & 0x3f) << 6) | dc(p[2] & 0x3f);
                            p += 3;
                        }
                    } else {
                        if (q - p < 3 || p[2] < 0x80 || p[2] > 0xbf) {
                            d += dc(0xfffd);
                            p += 2;
                        } else if (q - p < 4 || p[3] < 0x80 || p[3] > 0xbf) {
                            d += dc(0xfffd);
                            p += 3;
                        } else {
                            d += (dc(p[0] & 0x07) << 18) | (dc(p[1] & 0x3f) << 12)
                                | (dc(p[2] & 0x3f) << 6) | dc(p[3] & 0x3f);
                            p += 4;
                        }
                    }
                }
                return d;
            }
        };

        template <typename Src, typename Dst>
        struct UtfConvert<Src, Dst, 2, 4> {
            Dst operator()(const Src& s) const {
                using dc = typename Dst::value_type;
                auto p = reinterpret_cast<const uint16_t*>(s.data());
                auto q = p + s.size();
                Dst d;
                while (p < q) {
                    if (*p <= 0xd7ff || *p >= 0xe000) {
                        d += dc(*p++);
                    } else if (q - p >= 2 && p[0] >= 0xd800 && p[0] <= 0xdbff && p[1] >= 0xdc00 && p[1] <= 0xdfff) {
                        d += dc(0x10000) + ((dc(p[0]) & 0x3ff) << 10) + (dc(p[1]) & 0x3ff);
                        p += 2;
                    } else {
                        d += dc(0xfffd);
                        ++p;
                    }
                }
                return d;
            }
        };

        template <typename Src, typename Dst>
        struct UtfConvert<Src, Dst, 4, 1> {
            Dst operator()(const Src& s) const {
                using dc = typename Dst::value_type;
                Dst d;
                for (auto c: s) {
                    auto u = uint32_t(c);
                    if (u <= 0x7f) {
                        d += dc(u);
                    } else if (u <= 0x7ff) {
                        d += dc(0xc0 | (u >> 6));
                        d += dc(0x80 | (u & 0x3f));
                    } else if (u <= 0xd7ff || (u >= 0xe000 && u <= 0xffff)) {
                        d += dc(0xe0 | (u >> 12));
                        d += dc(0x80 | ((u >> 6) & 0x3f));
                        d += dc(0x80 | (u & 0x3f));
                    } else if (u >= 0x10000 && u <= 0x10ffff) {
                        d += dc(0xf0 | (u >> 18));
                        d += dc(0x80 | ((u >> 12) & 0x3f));
                        d += dc(0x80 | ((u >> 6) & 0x3f));
                        d += dc(0x80 | (u & 0x3f));
                    } else {
                        d += dc(0xef);
                        d += dc(0xbf);
                        d += dc(0xbf);
                    }
                }
                return d;
            }
        };

        template <typename Src, typename Dst>
        struct UtfConvert<Src, Dst, 4, 2> {
            Dst operator()(const Src& s) const {
                using dc = typename Dst::value_type;
                Dst d;
                for (auto c: s) {
                    auto u = uint32_t(c);
                    if (u <= 0xd7ff || (u >= 0xe000 && u <= 0xffff)) {
                        d += dc(u);
                    } else if (u >= 0x10000 && u <= 0x10ffff) {
                        d += dc(0xd800 + ((u >> 10) - 0x40));
                        d += dc(0xdc00 + (u & 0x3ff));
                    } else {
                        d += dc(0xfffd);
                    }
                }
                return d;
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

        template <typename C>
        struct UtfLength {
            size_t operator()(const std::basic_string<C>& s) const noexcept {
                switch (sizeof(C)) {
                    case 1:
                        return std::count_if(s.begin(), s.end(), [] (C c) {
                            auto b = uint8_t(c);
                            return b <= 0x7f || (b >= 0xc2 && b <= 0xf4);
                        });
                    case 2:
                        return std::count_if(s.begin(), s.end(), [] (C c) {
                            auto u = uint16_t(c);
                            return u <= 0xdbff || u >= 0xe000;
                        });
                    default: {
                        return s.size();
                    }
                }
            }
        };

        template <typename C, size_t N = sizeof(C)>
        struct UtfValidate;

        template <typename C>
        struct UtfValidate<C, 1> {
            size_t operator()(const std::basic_string<C>& s) const noexcept {
                auto ptr = reinterpret_cast<const uint8_t*>(s.data());
                size_t units = s.size(), i = 0;
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
            size_t operator()(const std::basic_string<C>& s) const noexcept {
                auto ptr = reinterpret_cast<const uint16_t*>(s.data());
                size_t units = s.size(), i = 0;
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
            size_t operator()(const std::basic_string<C>& s) const noexcept {
                auto ptr = reinterpret_cast<const uint32_t*>(s.data());
                size_t units = s.size(), i = 0;
                for (; i < units; ++i)
                    if ((ptr[i] >= 0xd800 && ptr[i] <= 0xdfff) || ptr[i] >= 0x110000)
                        break;
                return i;
            }
        };

    }

    template <typename Dst, typename Src>
    Dst uconv(const Src& s) {
        return RS_Detail::UtfConvert<Src, Dst>()(s);
    }

    template <typename C>
    size_t ulength(const std::basic_string<C>& s) noexcept {
        return RS_Detail::UtfLength<C>()(s);
    }

    template <typename C>
    bool uvalid(const std::basic_string<C>& s, size_t& n) noexcept {
        n = RS_Detail::UtfValidate<C>()(s);
        return n == s.size();
    }

    template <typename C>
    bool uvalid(const std::basic_string<C>& s) noexcept {
        size_t n = 0;
        return uvalid(s, n);
    }

    // String property functions

    inline bool ascii_icase_equal(const std::string& lhs, const std::string& rhs) noexcept {
        if (lhs.size() != rhs.size())
            return false;
        for (size_t i = 0, n = lhs.size(); i < n; ++i)
            if (ascii_toupper(lhs[i]) != ascii_toupper(rhs[i]))
                return false;
        return true;
    }

    inline bool ascii_icase_less(const std::string& lhs, const std::string& rhs) noexcept {
        for (size_t i = 0, n = std::min(lhs.size(), rhs.size()); i < n; ++i) {
            char a = ascii_toupper(lhs[i]), b = ascii_toupper(rhs[i]);
            if (a != b)
                return uint8_t(a) < uint8_t(b);
        }
        return lhs.size() < rhs.size();
    }

    inline bool starts_with(const std::string& str, const std::string& prefix) noexcept {
        return str.size() >= prefix.size()
            && memcmp(str.data(), prefix.data(), prefix.size()) == 0;
    }

    inline bool ends_with(const std::string& str, const std::string& suffix) noexcept {
        return str.size() >= suffix.size()
            && memcmp(str.data() + str.size() - suffix.size(), suffix.data(), suffix.size()) == 0;
    }

    inline bool string_is_ascii(const std::string& str) noexcept {
        return std::find_if(str.begin(), str.end(), [] (char c) { return c & 0x80; }) == str.end();
    }

    // String manipulation functions

    namespace RS_Detail {

        inline U8string quote_string(const std::string& str, bool check_utf8) {
            bool allow_8bit = check_utf8 && uvalid(str);
            U8string result = "\"";
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

    inline std::string add_prefix(const std::string& s, const std::string& prefix) {
        if (starts_with(s, prefix))
            return s;
        else
            return prefix + s;
    }

    inline std::string add_suffix(const std::string& s, const std::string& suffix) {
        if (ends_with(s, suffix))
            return s;
        else
            return s + suffix;
    }

    inline std::string drop_prefix(const std::string& s, const std::string& prefix) {
        if (starts_with(s, prefix))
            return s.substr(prefix.size(), npos);
        else
            return s;
    }

    inline std::string drop_suffix(const std::string& s, const std::string& suffix) {
        if (ends_with(s, suffix))
            return s.substr(0, s.size() - suffix.size());
        else
            return s;
    }

    inline std::string ascii_lowercase(const std::string& s) {
        auto r = s;
        std::transform(r.begin(), r.end(), r.begin(), ascii_tolower);
        return r;
    }

    inline std::string ascii_uppercase(const std::string& s) {
        auto r = s;
        std::transform(r.begin(), r.end(), r.begin(), ascii_toupper);
        return r;
    }

    inline std::string ascii_titlecase(const std::string& s) {
        auto r = s;
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

    inline std::string ascii_sentencecase(const std::string& s) {
        auto r = s;
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

    inline U8string dent(size_t depth) { return U8string(4 * depth, ' '); }

    inline U8string indent(const U8string& str, size_t depth) {
        U8string result;
        size_t i = 0, j = 0, size = str.size();
        while (i < size) {
            j = str.find('\n', i);
            if (j == npos)
                j = size;
            if (j > i) {
                result.append(4 * depth, ' ');
                result.append(str, i, j - i);
            }
            result += '\n';
            i = j + 1;
        }
        return result;
    }

    template <typename InputRange>
    std::string join(const InputRange& range, const std::string& delim = {}, bool term = false) {
        std::string result;
        for (auto& s: range) {
            result += s;
            result += delim;
        }
        if (! term && ! result.empty() && ! delim.empty())
            result.resize(result.size() - delim.size());
        return result;
    }

    inline std::string linearize(const std::string& str) {
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
            result.append(str, i, j - i);
        }
        return result;
    }

    template <typename C>
    void null_term(std::basic_string<C>& str) noexcept {
        auto p = str.find(C(0));
        if (p != npos)
            str.resize(p);
    }

    template <typename C>
    std::basic_string<C> null_term_str(const std::basic_string<C>& str) {
        auto result = str;
        null_term(result);
        return result;
    }

    inline std::pair<std::string, std::string> partition_at(const std::string& str, const std::string& delim) {
        if (delim.empty())
            return {str, {}};
        size_t p = str.find(delim);
        if (p == npos)
            return {str, {}};
        else
            return {str.substr(0, p), str.substr(p + delim.size(), npos)};
    }

    inline std::pair<std::string, std::string> partition_by(const std::string& str, const std::string& delims = ascii_whitespace) {
        if (delims.empty())
            return {str, {}};
        size_t p = str.find_first_of(delims);
        if (p == npos)
            return {str, {}};
        size_t q = str.find_first_not_of(delims, p);
        if (q == npos)
            return {str.substr(0, p), {}};
        else
            return {str.substr(0, p), str.substr(q, npos)};
    }

    inline U8string quote(const std::string& str) { return RS_Detail::quote_string(str, true); }
    inline U8string bquote(const std::string& str) { return RS_Detail::quote_string(str, false); }

    inline std::string repeat(const std::string& s, size_t n, const std::string& delim = {}) {
        if (n == 0)
            return {};
        else if (n == 1)
            return s;
        std::string r = s;
        r.reserve(n * s.size());
        size_t reps = 1, half = n / 2;
        for (; reps <= half; reps *= 2)
            r += delim + r;
        if (reps < n)
            r += delim + repeat(s, n - reps, delim);
        return r;
    }

    inline std::string replace(const std::string& s, const std::string& target, const std::string& subst, size_t n = npos) {
        if (target.empty() || n == 0)
            return s;
        std::string r;
        size_t i = 0, j = 0, slen = s.size(), tlen = target.size();
        for (size_t k = 0; k < n && i < slen; ++k) {
            j = s.find(target, i);
            if (j == npos)
                break;
            r.append(s, i, j - i);
            r += subst;
            i = j + tlen;
        }
        r.append(s, i, npos);
        return r;
    }

    template <typename OutputIterator>
    void split(const std::string& src, OutputIterator dst, const std::string& delim = ascii_whitespace) {
        if (src.empty())
            return;
        if (delim.empty()) {
            *dst = src;
            return;
        }
        size_t i = 0, j = 0, size = src.size();
        while (i < size) {
            j = src.find_first_of(delim, i);
            if (j == npos) {
                *dst = src.substr(i, npos);
                break;
            }
            if (j > i) {
                *dst = src.substr(i, j - i);
                ++dst;
            }
            i = src.find_first_not_of(delim, j);
        }
    }

    inline Strings splitv(const std::string& src, const std::string& delim = ascii_whitespace) {
        Strings v;
        split(src, append(v), delim);
        return v;
    }

    template <typename OutputIterator>
    void split_lines(const std::string& src, OutputIterator dst) {
        if (src.empty())
            return;
        size_t i = 0, j = 0, size = src.size();
        while (i < size) {
            j = src.find('\n', i);
            if (j == npos) {
                *dst = src.substr(i, npos);
                break;
            } else if (j - i > 0 && src[j - 1] == '\r') {
                *dst = src.substr(i, j - i - 1);
            } else {
                *dst = src.substr(i, j - i);
            }
            ++dst;
            i = j + 1;
        }
    }

    inline Strings splitv_lines(const std::string& src) {
        Strings v;
        split_lines(src, append(v));
        return v;
    }

    inline std::string trim(const std::string& str, const std::string& chars = ascii_whitespace) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return str.substr(pos, str.find_last_not_of(chars) + 1 - pos);
    }

    inline std::string trim_left(const std::string& str, const std::string& chars = ascii_whitespace) {
        size_t pos = str.find_first_not_of(chars);
        if (pos == npos)
            return {};
        else
            return str.substr(pos, npos);
    }

    inline std::string trim_right(const std::string& str, const std::string& chars = ascii_whitespace) {
        return str.substr(0, str.find_last_not_of(chars) + 1);
    }

    inline std::string unqualify(const std::string& str, const std::string& delims = ".:") {
        if (delims.empty())
            return str;
        size_t pos = str.find_last_of(delims);
        if (pos == npos)
            return str;
        else
            return str.substr(pos + 1, npos);
    }

    // String formatting functions

    template <typename T> U8string bin(T x, size_t digits = 8 * sizeof(T)) { return RS_Detail::int_to_string(x, 2, digits); }
    template <typename T> U8string dec(T x, size_t digits = 1) { return RS_Detail::int_to_string(x, 10, digits); }
    template <typename T> U8string hex(T x, size_t digits = 2 * sizeof(T)) { return RS_Detail::int_to_string(x, 16, digits); }

    template <typename T>
    U8string fp_format(T t, char mode = 'g', int prec = 6) {
        using namespace std::literals;
        static const U8string modes = "EFGZefgz";
        if (modes.find(mode) == npos)
            throw std::invalid_argument("Invalid floating point mode: " + quote(U8string{mode}));
        if (t == 0) {
            switch (mode) {
                case 'E': case 'e':  return prec < 1 ? "0"s + mode + '0' : "0."s + U8string(prec, '0') + mode + "0";
                case 'F': case 'f':  return prec < 1 ? "0"s : "0."s + U8string(prec, '0');
                case 'G': case 'g':  return "0";
                case 'Z': case 'z':  return prec < 2 ? "0"s : "0."s + U8string(prec - 1, '0');
                default:             break;
            }
        }
        U8string buf(20, '\0'), fmt;
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

    inline U8string roman(unsigned n) {
        struct char_value { const char* str; unsigned num; };
        static constexpr char_value table[] {
            { "CM", 900 }, { "D", 500 }, { "CD", 400 }, { "C", 100 },
            { "XC", 90 }, { "L", 50 }, { "XL", 40 }, { "X", 10 },
            { "IX", 9 }, { "V", 5 }, { "IV", 4 }, { "I", 1 },
        };
        if (n == 0)
            return "0";
        U8string s(n / 1000, 'M');
        n %= 1000;
        for (auto& t: table) {
            unsigned q = n / t.num;
            n %= t.num;
            for (unsigned i = 0; i < q; ++i)
                s += t.str;
        }
        return s;
    }

    inline U8string hexdump(const void* ptr, size_t n, size_t block = 0) {
        if (ptr == nullptr || n == 0)
            return {};
        U8string result;
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

    inline U8string hexdump(const std::string& str, size_t block = 0) { return hexdump(str.data(), str.size(), block); }
    inline U8string tf(bool b) { return b ? "true" : "false"; }
    inline U8string yn(bool b) { return b ? "yes" : "no"; }

    template <typename T> U8string to_str(const T& t);

    namespace RS_Detail {

        template <typename R, typename V = RangeValue<R>>
        struct RangeToString {
            U8string operator()(const R& r) const {
                U8string s = "[";
                for (const V& v: r) {
                    s += to_str(v);
                    s += ',';
                }
                if (s.size() > 1)
                    s.pop_back();
                s += ']';
                return s;
            }
        };

        template <typename R, typename K, typename V>
        struct RangeToString<R, std::pair<K, V>> {
            U8string operator()(const R& r) const {
                U8string s = "{";
                for (const auto& kv: r) {
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
            U8string operator()(const R& r) const {
                using std::begin;
                using std::end;
                return U8string(begin(r), end(r));
            }
        };

        template <typename T>
        struct ObjectToStringCategory {
            static constexpr char value =
                std::is_integral<T>::value ? 'I' :
                std::is_floating_point<T>::value ? 'F' :
                std::is_convertible<T, std::string>::value ? 'S' :
                CommonRangeType<T>::value ? 'R' : 'X';
        };

        template <typename T, char C = ObjectToStringCategory<T>::value>
        struct ObjectToString {
            U8string operator()(const T& t) const {
                std::ostringstream out;
                out << t;
                return out.str();
            }
        };

        template <> struct ObjectToString<U8string> { U8string operator()(const U8string& t) const { return t; } };
        template <> struct ObjectToString<char*> { U8string operator()(char* t) const { return t ? U8string(t) : U8string(); } };
        template <> struct ObjectToString<const char*> { U8string operator()(const char* t) const { return t ? U8string(t) : U8string(); } };
        template <size_t N> struct ObjectToString<char[N], 'S'> { U8string operator()(const char* t) const { return U8string(t); } };
        template <> struct ObjectToString<char> { U8string operator()(char t) const { return {t}; } };

        template <> struct ObjectToString<std::u16string> { U8string operator()(const std::u16string& t) const { return uconv<U8string>(t); } };
        template <> struct ObjectToString<char16_t*> { U8string operator()(char16_t* t) const { return t ? uconv<U8string>(std::u16string(t)) : U8string(); } };
        template <> struct ObjectToString<const char16_t*> { U8string operator()(const char16_t* t) const { return t ? uconv<U8string>(std::u16string(t)) : U8string(); } };
        template <size_t N> struct ObjectToString<char16_t[N], 'X'> { U8string operator()(const char16_t* t) const { return uconv<U8string>(std::u16string(t)); } };
        template <> struct ObjectToString<char16_t> { U8string operator()(char16_t t) const { return uconv<U8string>(std::u16string{t}); } };

        template <> struct ObjectToString<std::u32string> { U8string operator()(const std::u32string& t) const { return uconv<U8string>(t); } };
        template <> struct ObjectToString<char32_t*> { U8string operator()(char32_t* t) const { return t ? uconv<U8string>(std::u32string(t)) : U8string(); } };
        template <> struct ObjectToString<const char32_t*> { U8string operator()(const char32_t* t) const { return t ? uconv<U8string>(std::u32string(t)) : U8string(); } };
        template <size_t N> struct ObjectToString<char32_t[N], 'X'> { U8string operator()(const char32_t* t) const { return uconv<U8string>(std::u32string(t)); } };
        template <> struct ObjectToString<char32_t> { U8string operator()(char32_t t) const { return uconv<U8string>(std::u32string{t}); } };

        template <> struct ObjectToString<std::wstring> { U8string operator()(const std::wstring& t) const { return uconv<U8string>(t); } };
        template <> struct ObjectToString<wchar_t*> { U8string operator()(wchar_t* t) const { return t ? uconv<U8string>(std::wstring(t)) : U8string(); } };
        template <> struct ObjectToString<const wchar_t*> { U8string operator()(const wchar_t* t) const { return t ? uconv<U8string>(std::wstring(t)) : U8string(); } };
        template <size_t N> struct ObjectToString<wchar_t[N], 'X'> { U8string operator()(const wchar_t* t) const { return uconv<U8string>(std::wstring(t)); } };
        template <> struct ObjectToString<wchar_t> { U8string operator()(wchar_t t) const { return uconv<U8string>(std::wstring{t}); } };

        template <> struct ObjectToString<bool> { U8string operator()(bool t) const { return t ? "true" : "false"; } };
        template <> struct ObjectToString<std::nullptr_t> { U8string operator()(std::nullptr_t) const { return "null"; } };
        template <typename T> struct ObjectToString<T, 'I'> { U8string operator()(T t) const { return dec(t); } };
        template <typename T> struct ObjectToString<T, 'F'> { U8string operator()(T t) const { return fp_format(t); } };
        template <typename T> struct ObjectToString<T, 'S'> { U8string operator()(T t) const { return static_cast<std::string>(*&t); } };
        template <typename T> struct ObjectToString<T, 'R'>: RangeToString<T> {};
        template <typename T> struct ObjectToString<std::atomic<T>, 'X'> { U8string operator()(const std::atomic<T>& t) const { return ObjectToString<T>()(t); } };
        template <typename T1, typename T2> struct ObjectToString<std::pair<T1, T2>, 'X'>
            { U8string operator()(const std::pair<T1, T2>& t) const { return '{' + ObjectToString<T1>()(t.first) + ',' + ObjectToString<T2>()(t.second) + '}'; } };

    }

    template <typename T> inline U8string to_str(const T& t) { return RS_Detail::ObjectToString<T>()(t); }

    template <typename... Args>
    U8string fmt(const U8string& pattern, const Args&... args) {
        Strings argstr{to_str(args)...};
        U8string result;
        size_t i = 0, psize = pattern.size();
        while (i < psize) {
            size_t j = pattern.find('$', i);
            if (j == npos)
                j = psize;
            result.append(pattern, i, j - i);
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
                size_t a = std::stoul(pattern.substr(j, k - j));
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

    inline unsigned long long binnum(const std::string& str) noexcept { return strtoull(str.data(), nullptr, 2); }
    inline long long decnum(const std::string& str) noexcept { return strtoll(str.data(), nullptr, 10); }
    inline unsigned long long hexnum(const std::string& str) noexcept { return strtoull(str.data(), nullptr, 16); }
    inline double fpnum(const std::string& str) noexcept { return strtod(str.data(), nullptr); }

    inline int64_t si_to_int(const U8string& s) {
        using limits = std::numeric_limits<int64_t>;
        static constexpr const char* prefixes = "KMGTPEZY";
        char* endp = nullptr;
        errno = 0;
        int64_t n = strtoll(s.data(), &endp, 10);
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
                if (steps > limit)
                    throw std::range_error("Out of range: " + quote(s));
                n *= int_power(int64_t(1000), steps);
            }
        }
        return n;
    }

    inline double si_to_float(const U8string& s) {
        using limits = std::numeric_limits<double>;
        static constexpr const char* prefixes = "yzafpnum kMGTPEZY";
        char* endp = nullptr;
        errno = 0;
        double x = strtod(s.data(), &endp);
        if (errno == ERANGE)
            throw std::range_error("Out of range: " + quote(s));
        if (errno || endp == s.data())
            throw std::invalid_argument("Invalid number: " + quote(s));
        if (ascii_isspace(*endp))
            ++endp;
        char c = *endp;
        if (x && ascii_isalpha(c)) {
            if (c == 'K')
                c = 'k';
            auto pp = strchr(prefixes, c);
            if (pp) {
                auto steps = pp - prefixes - 8;
                double limit = log10(limits::max() / fabs(x)) / 3;
                if (steps > limit)
                    throw std::range_error("Out of range: " + quote(s));
                x *= pow(1000.0, steps);
            }
        }
        return x;
    }

    // HTML/XML tags

    class Tag {
    public:
        Tag() = default;
        Tag(std::ostream& out, const std::string& element) {
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
    void tagged(std::ostream& out, const std::string& element, const T& t) {
        Tag html(out, element);
        out << t;
    }

    template <typename... Args>
    void tagged(std::ostream& out, const std::string& element, const Args&... args) {
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

        inline auto find_leading_spaces(const U8string& s, size_t limit = npos) noexcept {
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
                v.push_back(trim(U8string(p, q)));
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

        inline U8string operator""_doc(const char* p, size_t n) {
            using namespace RS_Detail;
            auto lines = splitv_lines(U8string(p, n));
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
            return splitv(U8string(p, n));
        }

    }

}
