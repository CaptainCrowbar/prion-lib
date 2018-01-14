#pragma once

#include "rs-core/common.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <string>

namespace RS {

    class Encoding {
    public:
        virtual ~Encoding() = default;
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const = 0;
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const = 0;
        void encode(string_view src, Ustring& dst, size_t width = npos) const { encode_bytes(src.data(), src.size(), dst, width); }
        Ustring encode(string_view src, size_t width = npos) const { Ustring dst; encode_bytes(src.data(), src.size(), dst, width); return dst; }
        size_t decode(Uview src, std::string& dst) const { return decode_bytes(src.data(), src.size(), dst); }
        std::string decode(Uview src) const { std::string dst; decode_bytes(src.data(), src.size(), dst); return dst; }
    protected:
        Encoding() = default;
    };

    namespace RS_Detail {

        class EscapeQuoteBase:
        public Encoding {
        protected:
            static void do_encode(const void* src, size_t len, Ustring& dst, size_t width, bool quote) {
                if (quote)
                    dst += '\"';
                auto begin = static_cast<const char*>(src), end = begin + len, i = begin;
                size_t margin = 0;
                while (i != end) {
                    auto j = std::find_if(i, end, is_escaped);
                    if (j != i) {
                        dst.append(i, j);
                        while (dst.size() > margin + width) {
                            dst.insert(margin + width, 1, '\n');
                            margin += width + 1;
                        }
                    }
                    i = std::find_if_not(j, end, is_escaped);
                    for (; j != i; ++j) {
                        size_t start = dst.size();
                        switch (*j) {
                            case '\0':  dst += "\\0"; break;
                            case '\a':  dst += "\\a"; break;
                            case '\b':  dst += "\\b"; break;
                            case '\t':  dst += "\\t"; break;
                            case '\n':  dst += "\\n"; break;
                            case '\v':  dst += "\\v"; break;
                            case '\f':  dst += "\\f"; break;
                            case '\r':  dst += "\\r"; break;
                            case '\"':  dst += quote ? "\\\"" : "\""; break;
                            case '\\':  dst += "\\\\"; break;
                            default:    dst += "\\x"; RS_Detail::append_hex_byte(uint8_t(*j), dst); break;
                        }
                        if (dst.size() > margin + width) {
                            dst.insert(start, 1, '\n');
                            margin = start + 1;
                        }
                    }
                }
                if (quote)
                    dst += '\"';
            }
            static size_t do_decode(const char* src, size_t len, std::string& dst, char quote) {
                if (len == 0 || (quote && *src != '\"'))
                    return 0;
                auto i = src + int(quote), end = src + len;
                while (i != end) {
                    if (*i < 32 || *i >= 126) {
                        ++i;
                    } else if (quote && *i == '\"') {
                        return i - src + 1;
                    } else if (*i == '\\' && end - i >= 2) {
                        switch (i[1]) {
                            case '0':   dst += '\0'; i += 2; break;
                            case 'a':   dst += '\a'; i += 2; break;
                            case 'b':   dst += '\b'; i += 2; break;
                            case 't':   dst += '\t'; i += 2; break;
                            case 'n':   dst += '\n'; i += 2; break;
                            case 'v':   dst += '\v'; i += 2; break;
                            case 'f':   dst += '\f'; i += 2; break;
                            case 'r':   dst += '\r'; i += 2; break;
                            case '\"':  dst += '\"'; i += 2; break;
                            case '\\':  dst += '\\'; i += 2; break;
                            case 'x':
                                if (end - i >= 3 && ascii_isxdigit(i[2])) {
                                    dst += char(hexnum(Ustring(i + 2, 2)));
                                    i += 3;
                                    if (ascii_isxdigit(*i))
                                        ++i;
                                } else {
                                    dst += 'x';
                                    i += 2;
                                }
                                break;
                            default:
                                dst += i[1];
                                i += 2;
                                break;
                        }
                    } else {
                        dst += *i++;
                    }
                }
                return len;
            }
            static constexpr bool is_escaped(char c) noexcept { return c < 32 || c > 126 || c == '\\' || c == '\"'; }
        };

    }

    class EscapeEncoding:
    public RS_Detail::EscapeQuoteBase {
    public:
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const { do_encode(src, len, dst, width, false); }
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const { return do_decode(src, len, dst, false); }
    };

    class QuoteEncoding:
    public RS_Detail::EscapeQuoteBase {
    public:
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const { do_encode(src, len, dst, width, true); }
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const { return do_decode(src, len, dst, true); }
    };

    class HexEncoding:
    public Encoding {
    public:
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const {
            static constexpr const char* digits = "0123456789abcdef";
            auto bp = static_cast<const uint8_t*>(src);
            size_t col = 0;
            for (size_t i = 0; i < len; ++i) {
                dst += digits[bp[i] / 16];
                dst += digits[bp[i] % 16];
                col += 2;
                if (col > width) {
                    dst.insert(dst.size() - col + width, 1, '\n');
                    col -= width;
                }
            }
        }
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const {
            uint8_t byte = 0, digit = 0;
            size_t nhex = 0;
            for (size_t i = 0; i < len; ++i) {
                if (src[i] >= '0' && src[i] <= '9')
                    digit = uint8_t(src[i] - '0');
                else if (src[i] >= 'A' && src[i] <= 'F')
                    digit = uint8_t(src[i] - 'A' + 10);
                else if (src[i] >= 'a' && src[i] <= 'f')
                    digit = uint8_t(src[i] - 'a' + 10);
                else
                    continue;
                byte = uint8_t(16 * byte + digit);
                if (++nhex == 2) {
                    dst += char(byte);
                    byte = 0;
                    nhex = 0;
                }
            }
            if (nhex)
                dst += char(byte);
            return len;
        }
    };

    class Base32Encoding:
    public Encoding {
    public:
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const {
            auto bytes = static_cast<const uint8_t*>(src);
            size_t column = 0, i = 0, j = 0;
            uint64_t group = 0;
            for (;;) {
                if (i == len || j == 5) {
                    size_t n_out = (8 * j + 4) / 5;
                    for (size_t k = 0; k < n_out; ++k) {
                        if (width > 0 && column++ == width) {
                            dst += '\n';
                            column = 1;
                        }
                        size_t digit = group & 0x1f;
                        if (digit < 10)
                            dst += char('0' + digit);
                        else
                            dst += char('a' + (digit - 10));
                        group >>= 5;
                    }
                    if (i == len)
                        break;
                    j = 0;
                }
                group |= uint64_t(bytes[i++]) << (8 * j++);
            }
        }
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const {
            size_t i = 0, j = 0;
            uint64_t digit = 0, group = 0;
            for (;;) {
                if (i == len || j == 8) {
                    size_t n_out = 5 * j / 8;
                    for (size_t k = 0; k < n_out; ++k) {
                        dst += char(group & 0xff);
                        group >>= 8;
                    }
                    if (i == len)
                        break;
                    j = 0;
                }
                char c = src[i++];
                if (c >= '0' && c <= '9')
                    digit = uint64_t(c - '0');
                else if (c >= 'A' && c <= 'V')
                    digit = uint64_t(c - 'A') + 10;
                else if (c >= 'a' && c <= 'v')
                    digit = uint64_t(c - 'a') + 10;
                else
                    continue;
                group |= digit << (5 * j++);
            }
            return len;
        }
    };

    class Base64Encoding:
    public Encoding {
    public:
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const {
            static constexpr const char* n2c_base64_table =
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                "0123456789+/";
            auto bp = static_cast<const uint8_t*>(src);
            uint32_t group = 0;
            size_t col = 0, ngroup = 0;
            for (size_t i = 0; i < len; ++i) {
                group = group * 256 + bp[i];
                if (++ngroup == 3) {
                    for (int j = 18; j >= 0; j -= 6)
                        dst += n2c_base64_table[(group >> j) & 0x3f];
                    group = 0;
                    ngroup = 0;
                    col += 4;
                    if (col > width) {
                        dst.insert(dst.size() - col + width, 1, '\n');
                        col -= width;
                    }
                }
            }
            if (ngroup) {
                if (ngroup == 1) {
                    group <<= 16;
                    dst += n2c_base64_table[(group >> 18) & 0x3f];
                    dst += n2c_base64_table[(group >> 12) & 0x3f];
                    dst += "==";
                } else if (ngroup == 2) {
                    group <<= 8;
                    dst += n2c_base64_table[(group >> 18) & 0x3f];
                    dst += n2c_base64_table[(group >> 12) & 0x3f];
                    dst += n2c_base64_table[(group >> 6) & 0x3f];
                    dst += "=";
                }
                col += 4;
                if (col > width) {
                    dst.insert(dst.size() - col + width, 1, '\n');
                    col -= width;
                }
            }
        }
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const {
            static constexpr uint8_t c2n_base64_table[96] = {
                99,  99,  99,  99,  99,  99,  99,  99,  99,  99,  99,  62,  99,  62,  99,  63,  //  !"#$%&'()*+,-./
                52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  99,  99,  99,  0,   99,  99,  // 0123456789:;<=>?
                99,  0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,  // @ABCDEFGHIJKLMNO
                15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  99,  99,  99,  99,  63,  // PQRSTUVWXYZ[\]^_
                99,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  // `abcdefghijklmno
                41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  99,  99,  99,  99,  99,  // pqrstuvwxyz{|}~
            };
            uint32_t group = 0;
            int neq = 0, ngroup = 0;
            for (size_t i = 0; i < len; ++i) {
                if (src[i] < 33 || src[i] > 126)
                    continue;
                auto digit = c2n_base64_table[src[i] - 32];
                if (digit == 99)
                    continue;
                group = group * 64 + digit;
                neq += src[i] == '=';
                if (++ngroup == 4) {
                    for (int j = 16; j >= 8 * neq; j -= 8)
                        dst += char((group >> j) & 0xff);
                    group = 0;
                    neq = ngroup = 0;
                }
            }
            if (ngroup) {
                group <<= 6 * (4 - ngroup);
                neq += 4 - ngroup;
                for (int j = 16; j >= 8 * neq; j -= 8)
                    dst += char((group >> j) & 0xff);
            }
            return len;
        }
    };

    namespace RS_Detail {

        class Base85EncodingBase:
        public Encoding {
        protected:
            using n2c_function = char (*)(uint32_t);
            using c2n_function = uint32_t (*)(char);
            static void do_encode(const void* src, size_t len, Ustring& dst, size_t width, n2c_function n2c, bool y) {
                auto bp = static_cast<const uint8_t*>(src);
                uint32_t group = 0;
                size_t col = 2, dcol = 0, ngroup = 0;
                dst += "<~";
                for (size_t i = 0; i < len; ++i) {
                    group = group * 256 + bp[i];
                    if (++ngroup == 4) {
                        if (group == 0) {
                            dst += 'z';
                            dcol = 1;
                        } else if (y && group == 0x20202020) {
                            dst += 'y';
                            dcol = 1;
                        } else {
                            dst.append(5, '\0');
                            for (size_t j = 1; j <= 5; ++j, group /= 85)
                                dst[dst.size() - j] = n2c(group % 85);
                            dcol = 5;
                        }
                        ngroup = 0;
                        col += dcol;
                        if (col > width) {
                            dst.insert(dst.size() - col + width, 1, '\n');
                            col -= width;
                        }
                    }
                }
                if (ngroup) {
                    group <<= (4 - ngroup) * 8;
                    dst.append(5, '\0');
                    for (size_t j = 1; j <= 5; ++j, group /= 85)
                        dst[dst.size() - j] = n2c(group % 85);
                    dst.resize(dst.size() + ngroup - 4);
                    col += ngroup + 1;
                    if (col > width) {
                        dst.insert(dst.size() - col + width, 1, '\n');
                        col -= width;
                    }
                }
                if (col >= width - 1)
                    dst += '\n';
                dst += "~>";
            }
            static size_t do_decode(const char* src, size_t len, std::string& dst, c2n_function c2n) {
                uint32_t group = 0;
                int ngroup = 0;
                size_t i = 0;
                if (len >= 2 && src[0] == '<' && src[1] == '~')
                    i = 2;
                for (; i < len; ++i) {
                    if (src[i] == '~' && len - i >= 2 && src[i + 1] == '>') {
                        i += 2;
                        break;
                    } else if (src[i] == 'y') {
                        dst.append(4, ' ');
                    } else if (src[i] == 'z') {
                        dst.append(4, '\0');
                    } else {
                        uint32_t n = c2n(src[i]);
                        if (n != 99) {
                            group = group * 85 + n;
                            if (++ngroup == 5) {
                                for (int j = 24; j >= 0; j -= 8)
                                    dst += char((group >> j) & 0xff);
                                group = 0;
                                ngroup = 0;
                            }
                        }
                    }
                }
                if (ngroup) {
                    for (size_t j = ngroup; j < 5; ++j)
                        group = group * 85 + 84;
                    int minshift = 8 * (5 - ngroup);
                    for (int j = 24; j >= minshift; j -= 8)
                        dst += char((group >> j) & 0xff);
                }
                return i;
            }
        };

    }

    class Ascii85Encoding:
    public RS_Detail::Base85EncodingBase {
    public:
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const { do_encode(src, len, dst, width, n2c_ascii85, true); }
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const { return do_decode(src, len, dst, c2n_ascii85); }
    private:
        static constexpr char n2c_ascii85(uint32_t n) noexcept { return char(n + 33); }
        static constexpr uint32_t c2n_ascii85(char c) noexcept { return c < '!' || c > 'u' ? 99 : uint32_t(c - '!'); }
    };

    class Z85Encoding:
    public RS_Detail::Base85EncodingBase {
    public:
        virtual void encode_bytes(const void* src, size_t len, Ustring& dst, size_t width = npos) const { do_encode(src, len, dst, width, n2c_z85, false); }
        virtual size_t decode_bytes(const char* src, size_t len, std::string& dst) const { return do_decode(src, len, dst, c2n_z85); }
    private:
        static char n2c_z85(uint32_t n) noexcept {
            static constexpr const char* n2c_z85_table =
                "0123456789"
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                ".-:+=^!/*?&<>()[]{}@%$#";
            return n2c_z85_table[n];
        }
        static uint32_t c2n_z85(char c) noexcept {
            constexpr uint8_t c2n_z85_table[96] = {
                98,  68,  85,  84,  83,  82,  72,  98,  75,  76,  70,  65,  98,  63,  62,  69,  //  !"#$%&'()*+,-./
                0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   64,  98,  73,  66,  74,  71,  // 0123456789:;<=>?
                81,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  // @ABCDEFGHIJKLMNO
                51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  77,  98,  78,  67,  98,  // PQRSTUVWXYZ[\]^_
                98,  10,  11,  12,  13,  14,  15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  // `abcdefghijklmno
                25,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  79,  98,  80,  98,  98,  // pqrstuvwxyz{|}~
            };
            return c < 33 || c > 126 ? 99 : c2n_z85_table[c - 32];
        }
    };

}
