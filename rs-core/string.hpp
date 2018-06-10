#pragma once

#include "rs-core/common.hpp"
#include <algorithm>
#include <array>
#include <ostream>
#include <string>
#include <string_view>
#include <typeindex>
#include <typeinfo>
#include <utility>

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

    Strings name_breakdown(Uview name);
    Ustring name_to_lower_case(Uview name, char delim = '_');
    Ustring name_to_upper_case(Uview name, char delim = '_');
    Ustring name_to_title_case(Uview name, char delim = '\0');
    Ustring name_to_camel_case(Uview name, char delim = '\0');
    Ustring name_to_sentence_case(Uview name, char delim = ' ');

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

    bool ascii_icase_equal(std::string_view lhs, std::string_view rhs) noexcept;
    bool ascii_icase_less(std::string_view lhs, std::string_view rhs) noexcept;
    bool starts_with(std::string_view str, std::string_view prefix) noexcept;
    bool ends_with(std::string_view str, std::string_view suffix) noexcept;
    bool string_is_ascii(std::string_view str) noexcept;

    // String manipulation functions

    namespace RS_Detail {

        inline void catstr_helper(std::string&) noexcept {}

        template <typename T, typename... Args>
        void catstr_helper(std::string& dst, const T& t, const Args&... args) {
            dst += t;
            catstr_helper(dst, args...);
        }

    }

    std::string add_prefix(std::string_view s, std::string_view prefix);
    std::string add_suffix(std::string_view s, std::string_view suffix);
    std::string drop_prefix(std::string_view s, std::string_view prefix);
    std::string drop_suffix(std::string_view s, std::string_view suffix);
    Ustring indent(Uview str, size_t depth);
    std::string linearize(std::string_view str);
    std::string pad_left(std::string_view str, size_t len, char pad = ' ');
    std::string pad_right(std::string_view str, size_t len, char pad = ' ');
    std::pair<std::string_view, std::string_view> partition_at(std::string_view str, std::string_view delim);
    std::pair<std::string_view, std::string_view> partition_by(std::string_view str, std::string_view delims = ascii_whitespace);
    std::string repeat(std::string_view s, size_t n, std::string_view delim = {});
    std::string replace(std::string_view s, std::string_view target, std::string_view subst, size_t n = npos);
    Strings splitv(std::string_view src, std::string_view delim = ascii_whitespace);
    Strings splitv_lines(std::string_view src);
    std::string trim(std::string_view str, std::string_view chars = ascii_whitespace);
    std::string trim_left(std::string_view str, std::string_view chars = ascii_whitespace);
    std::string trim_right(std::string_view str, std::string_view chars = ascii_whitespace);
    std::string unqualify(std::string_view str, std::string_view delims = ".:");

    template <typename... Args>
    std::string catstr(const Args&... args) {
        std::string s;
        RS_Detail::catstr_helper(s, args...);
        return s;
    }

    template <typename C>
    void null_term(std::basic_string<C>& str) noexcept {
        auto p = str.find(C(0));
        if (p != npos)
            str.resize(p);
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

    // String formatting functions

    Ustring hexdump(const void* ptr, size_t n, size_t block = 0);
    Ustring hexdump(std::string_view str, size_t block = 0);
    inline Ustring tf(bool b) { return b ? "true" : "false"; }
    inline Ustring yn(bool b) { return b ? "yes" : "no"; }

    template <size_t N>
    Ustring hex(const std::array<uint8_t, N>& bytes) {
        using namespace RS_Detail;
        Ustring s;
        for (auto b: bytes)
            append_hex_byte(b, s);
        return s;
    }

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
        Tag(std::ostream& out, std::string_view element);
        ~Tag() noexcept;
        Tag(const Tag&) = delete;
        Tag(Tag&& t) noexcept;
        Tag& operator=(const Tag&) = delete;
        Tag& operator=(Tag&& t) noexcept;
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

    std::string demangle(const std::string& name);
    inline std::string type_name(const std::type_info& t) { return demangle(t.name()); }
    inline std::string type_name(const std::type_index& t) { return demangle(t.name()); }
    template <typename T> std::string type_name() { return type_name(typeid(T)); }
    template <typename T> std::string type_name(const T& t) { return type_name(typeid(t)); }

    // String literals

    namespace Literals {

        Strings operator""_csv(const char* p, size_t n);
        Ustring operator""_doc(const char* p, size_t n);
        Strings operator""_qw(const char* p, size_t n);

    }

}
