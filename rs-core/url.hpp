#pragma once

#include "rs-core/common.hpp"
#include "rs-core/file.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cstdlib>
#include <ostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace RS {

    class Url:
    public LessThanComparable<Url> {
    public:
        static constexpr uint32_t lone_keys = 1;
        Url() = default;
        explicit Url(Uview s);
        Url(Uview scheme, Uview user, Uview password, Uview host, uint16_t port = 0, Uview path = {}, Uview query = {}, Uview fragment = {});
        bool has_scheme() const noexcept { return ! empty(); }
        bool has_slashes() const noexcept { return ! empty() && text[user_pos - 1] == '/'; }
        bool has_user() const noexcept { return ! empty() && password_pos > user_pos; }
        bool has_password() const noexcept { return ! empty() && host_pos - password_pos > 1; }
        bool has_host() const noexcept { return ! empty() && port_pos > host_pos; }
        bool has_port() const noexcept { return ! empty() && path_pos > port_pos; }
        bool has_path() const noexcept { return ! empty() && query_pos - path_pos > 1; }
        bool has_query() const noexcept { return ! empty() && fragment_pos > query_pos; }
        bool has_fragment() const noexcept { return ! empty() && text.size() > size_t(fragment_pos); }
        Ustring scheme() const { return empty() ? Ustring() : ascii_lowercase(text.substr(0, user_pos - (has_slashes() ? 3 : 1))); }
        Ustring user() const { return decode(text.substr(user_pos, password_pos - user_pos)); }
        Ustring password() const { return has_password() ? decode(text.substr(password_pos + 1, host_pos - password_pos - 2)) : Ustring(); }
        Ustring host() const { return decode(text.substr(host_pos, port_pos - host_pos)); }
        uint16_t port() const noexcept { return has_port() ? uint16_t(std::strtoul(text.data() + port_pos + 1, nullptr, 10)) : 0; }
        Ustring path() const { return decode(text.substr(path_pos, query_pos - path_pos)); }
        Ustring query() const { return has_query() ? decode(text.substr(query_pos + 1, fragment_pos - query_pos - 1)) : Ustring(); }
        Ustring fragment() const { return has_fragment() ? decode(text.substr(fragment_pos + 1, npos)) : Ustring(); }
        void set_scheme(Uview new_scheme);
        void set_user(Uview new_user);
        void set_password(Uview new_password);
        void set_host(Uview new_host);
        void set_port(uint16_t new_port);
        void set_path(Uview new_path);
        void set_query(Uview new_query);
        void set_fragment(Uview new_fragment);
        void clear_user() noexcept;
        void clear_password() noexcept;
        void clear_host() noexcept;
        void clear_port() noexcept;
        void clear_path() noexcept;
        void clear_query() noexcept;
        void clear_fragment() noexcept;
        void append_path(Uview new_path);
        File as_file() const;
        void clear() noexcept;
        Url doc() const;
        Url base() const;
        Url parent() const;
        Url root() const;
        bool empty() const noexcept { return text.empty(); }
        bool is_root() const noexcept { return ! empty() && ! has_path() && ! has_query() && ! has_fragment(); }
        Ustring path_dir() const;
        Ustring path_leaf() const;
        Ustring str() const { return text; }
        bool try_parse(Uview s);
        Url& operator/=(Uview rhs) { append_path(rhs); return *this; }
        static Ustring encode(Uview s, Uview exempt = {});
        static Ustring decode(Uview s);
        static Url from_file(const File& f);
        template <typename R> static Ustring make_query(const R& range, char delimiter = '&', uint32_t flags = 0);
        static std::vector<std::pair<Ustring, Ustring>> parse_query(Uview query, char delimiter = '\0');
        friend Url operator/(const Url& lhs, Uview rhs) { Url u = lhs; u.append_path(rhs); return u; }
        friend bool operator==(const Url& lhs, const Url& rhs) noexcept { return lhs.text == rhs.text; }
        friend bool operator<(const Url& lhs, const Url& rhs) noexcept { return lhs.text < rhs.text; }
        friend std::ostream& operator<<(std::ostream& out, const Url& u) { return out << u.text; }
        friend Ustring to_str(const Url& u) { return u.text; }
    private:
        // scheme:[//][[user[:password]@]host[:port]][/path][?query][#fragment]
        Ustring text;
        int user_pos = 0;      // user
        int password_pos = 0;  // :password@
        int host_pos = 0;      // host
        int port_pos = 0;      // :port
        int path_pos = 0;      // /path
        int query_pos = 0;     // ?query
        int fragment_pos = 0;  // #fragment
        void check_nonempty() const;
        int find_path_cut() const noexcept;
    };

        inline Url::Url(Uview s) {
            if (! try_parse(s))
                throw std::invalid_argument("Invalid URL: " + quote(s));
        }

        inline Url::Url(Uview scheme, Uview user, Uview password, Uview host, uint16_t port, Uview path, Uview query, Uview fragment) {
            set_scheme(scheme);
            set_host(host);
            set_user(user);
            set_password(password);
            set_port(port);
            set_path(path);
            set_query(query);
            set_fragment(fragment);
        }

        inline void Url::set_scheme(Uview new_scheme) {
            static const std::regex pattern("[A-Za-z][A-Za-z0-9+.-]*(:(?://)?)?", std::regex_constants::optimize);
            auto begin = new_scheme.data(), end = begin + new_scheme.size();
            std::cmatch match;
            if (! std::regex_match(begin, end, match, pattern))
                throw std::invalid_argument("Invalid URL scheme: " + quote(new_scheme));
            Ustring scheme_text = ascii_lowercase(new_scheme);
            if (! match[1].matched) {
                scheme_text += ':';
                if (empty() || has_slashes())
                    scheme_text += "//";
            }
            int delta = int(scheme_text.size()) - user_pos;
            text.replace(0, user_pos, scheme_text);
            user_pos += delta;
            password_pos += delta;
            host_pos += delta;
            port_pos += delta;
            path_pos += delta;
            query_pos += delta;
            fragment_pos += delta;
        }

        inline void Url::set_user(Uview new_user) {
            if (new_user.empty()) {
                clear_user();
                return;
            }
            check_nonempty();
            if (! has_host())
                throw std::invalid_argument("Can't set user on URL with no host");
            Ustring user_text = encode(new_user);
            int delta = int(user_text.size()) - (password_pos - user_pos);
            if (has_user()) {
                text.replace(user_pos, password_pos - user_pos, user_text);
                password_pos += delta;
                host_pos += delta;
            } else {
                text.replace(user_pos, password_pos - user_pos, user_text + '@');
                ++delta;
                host_pos += delta;
                password_pos = host_pos - 1;
            }
            port_pos += delta;
            path_pos += delta;
            query_pos += delta;
            fragment_pos += delta;
        }

        inline void Url::set_password(Uview new_password) {
            if (new_password.empty()) {
                clear_password();
                return;
            }
            check_nonempty();
            if (! has_user())
                throw std::invalid_argument("Can't set password on URL with no user");
            Ustring password_text = ':' + encode(new_password) + '@';
            int delta = int(password_text.size()) - (host_pos - password_pos);
            text.replace(password_pos, host_pos - password_pos, password_text);
            host_pos += delta;
            port_pos += delta;
            path_pos += delta;
            query_pos += delta;
            fragment_pos += delta;
        }

        inline void Url::set_host(Uview new_host) {
            if (new_host.empty()) {
                clear_host();
                return;
            }
            check_nonempty();
            Ustring host_text = encode(new_host);
            int delta = int(host_text.size()) - (port_pos - host_pos);
            text.replace(host_pos, port_pos - host_pos, host_text);
            port_pos += delta;
            path_pos += delta;
            query_pos += delta;
            fragment_pos += delta;
        }

        inline void Url::set_port(uint16_t new_port) {
            if (new_port == 0) {
                clear_port();
                return;
            }
            check_nonempty();
            if (! has_host())
                throw std::invalid_argument("Can't set port on URL with no host");
            Ustring port_text = ':' + std::to_string(new_port);
            text.replace(port_pos, path_pos - port_pos, port_text);
            int delta = int(port_text.size()) - (path_pos - port_pos);
            path_pos += delta;
            query_pos += delta;
            fragment_pos += delta;
        }

        inline void Url::set_path(Uview new_path) {
            if (new_path.empty()) {
                clear_path();
                return;
            }
            check_nonempty();
            if (new_path.find("//") != npos)
                throw std::invalid_argument("Invalid URL path: " + quote(new_path));
            Ustring path_text = encode(new_path, ":/");
            if (! path_text.empty() && path_text[0] != '/')
                path_text.insert(0, 1, '/');
            int delta = int(path_text.size()) - (query_pos - path_pos);
            text.replace(path_pos, query_pos - path_pos, path_text);
            query_pos += delta;
            fragment_pos += delta;
        }

        inline void Url::set_query(Uview new_query) {
            if (new_query.empty()) {
                clear_query();
                return;
            }
            check_nonempty();
            int delta = int(new_query.size() + 1) - (fragment_pos - query_pos);
            Ustring q = "?";
            q += new_query;
            text.replace(query_pos, fragment_pos - query_pos, q);
            fragment_pos += delta;
        }

        inline void Url::set_fragment(Uview new_fragment) {
            if (new_fragment.empty()) {
                clear_fragment();
                return;
            }
            check_nonempty();
            text.replace(fragment_pos, npos, '#' + encode(new_fragment));
        }

        inline void Url::clear_user() noexcept {
            if (! has_user())
                return;
            int delta = host_pos - user_pos;
            text.erase(user_pos, delta);
            host_pos = password_pos = user_pos;
            port_pos -= delta;
            path_pos -= delta;
            query_pos -= delta;
            fragment_pos -= delta;
        }

        inline void Url::clear_password() noexcept {
            if (! has_password())
                return;
            text.replace(password_pos, host_pos - password_pos, "@");
            int delta = host_pos - password_pos - 1;
            host_pos -= delta;
            port_pos -= delta;
            path_pos -= delta;
            query_pos -= delta;
            fragment_pos -= delta;
        }

        inline void Url::clear_host() noexcept {
            if (! has_host())
                return;
            int delta = port_pos - host_pos;
            text.erase(host_pos, delta);
            port_pos -= delta;
            path_pos -= delta;
            query_pos -= delta;
            fragment_pos -= delta;
        }

        inline void Url::clear_port() noexcept {
            if (! has_port())
                return;
            int delta = path_pos - port_pos;
            text.erase(port_pos, delta);
            path_pos -= delta;
            query_pos -= delta;
            fragment_pos -= delta;
        }

        inline void Url::clear_path() noexcept {
            if (! has_path())
                return;
            int delta = query_pos - path_pos;
            text.erase(path_pos, delta);
            query_pos -= delta;
            fragment_pos -= delta;
        }

        inline void Url::clear_query() noexcept {
            if (! has_query())
                return;
            text.erase(query_pos, fragment_pos - query_pos);
            fragment_pos = query_pos;
        }

        inline void Url::clear_fragment() noexcept {
            if (has_fragment())
                text.resize(fragment_pos);
        }

        inline void Url::append_path(Uview new_path) {
            if (new_path.empty())
                return;
            check_nonempty();
            if (new_path[0] == '/' || ! has_path()) {
                set_path(new_path);
                return;
            }
            if (new_path.find("//") != npos)
                throw std::invalid_argument("Invalid URL path: " + quote(new_path));
            Ustring path_text = encode(new_path, ":/");
            if (text[query_pos - 1] != '/')
                path_text.insert(0, 1, '/');
            int delta = int(path_text.size());
            text.insert(query_pos, path_text);
            query_pos += delta;
            fragment_pos += delta;
        }

        inline File Url::as_file() const {
            static const std::regex windows_root("^/[A-Za-z]:", std::regex_constants::optimize);
            if (text.empty())
                return {};
            if (scheme() != "file")
                throw std::invalid_argument("Not a file URL: " + quote(text));
            Ustring f, h = host(), p = path();
            if (! h.empty()) {
                f = "//" + h;
                if (p[0] != '/')
                    f += '/';
                f += p;
            } else if (std::regex_search(p, windows_root)) {
                f.assign(p, 1, npos);
            } else {
                f = p;
            }
            return File(f);
        }

        inline void Url::clear() noexcept {
            text.clear();
            user_pos = password_pos = host_pos = port_pos = path_pos = query_pos = fragment_pos = 0;
        }

        inline Url Url::doc() const {
            Url u = *this;
            u.text.resize(fragment_pos);
            return u;
        }

        inline Url Url::base() const {
            Url u = *this;
            u.text.resize(query_pos);
            u.fragment_pos = u.query_pos;
            return u;
        }

        inline Url Url::parent() const {
            Url u = base();
            if (u.query_pos - u.path_pos > 1) {
                int p = u.query_pos - 1;
                if (u.text[p] == '/')
                    --p;
                while (p > u.path_pos && u.text[p] != '/')
                    --p;
                if (u.text[p] == '/')
                    ++p;
                u.text.resize(p);
                u.query_pos = u.fragment_pos = p;
            }
            return u;
        }

        inline Url Url::root() const {
            Url u = *this;
            u.text.resize(path_pos);
            u.path_pos = int(u.text.size());
            if (query_pos > path_pos)
                u.text += '/';
            u.query_pos = u.fragment_pos = int(u.text.size());
            return u;
        }

        inline Ustring Url::path_dir() const {
            int cut = find_path_cut();
            return decode(text.substr(path_pos, cut - path_pos));
        }

        inline Ustring Url::path_leaf() const {
            int cut = find_path_cut();
            return decode(text.substr(cut, query_pos - cut));
        }

        inline bool Url::try_parse(Uview s) {
            // scheme:[//][[user[:password]@]host[:port]][/path][?query][#fragment]
            static const std::regex pattern(
                "([A-Za-z][A-Za-z0-9+.-]*:(?://)?)"  // scheme
                "(?:"
                    "(?:"
                        "([^/:@]+)"  // user
                        "([^/@]*@)"  // password
                    ")?"
                    "([^/:@]+)"  // host
                    "(:\\d+)?"   // port
                ")?"
                "(/[^#?]*)?"   // path
                "(\\?[^#]*)?"  // query
                "(#.*)?",      // fragment
                std::regex_constants::optimize);
            auto begin = s.data(), end = begin + s.size();
            if (std::find_if(begin, end, ascii_iscntrl) != end)
                return false;
            std::cmatch match;
            if (! std::regex_match(begin, end, match, pattern))
                return false;
            text = Ustring(s);
            user_pos = int(match[1].length());
            password_pos = user_pos + int(match[2].length());
            host_pos = password_pos + int(match[3].length());
            port_pos = host_pos + int(match[4].length());
            path_pos = port_pos + int(match[5].length());
            query_pos = path_pos + int(match[6].length());
            fragment_pos = query_pos + int(match[7].length());
            return true;
        }

        inline Ustring Url::encode(Uview s, Uview exempt) {
            auto is_safe = [exempt] (char c) { return ascii_isalnum_w(c) || c == '-' || c == '.' || c == '~' || exempt.find(c) != npos; };
            Ustring out;
            auto i = s.begin(), end = s.end();
            while (i != end) {
                auto j = std::find_if_not(i, end, is_safe);
                out.append(i, j);
                if (j == end)
                    break;
                out += '%' + hex(uint8_t(*j));
                i = j + 1;
            }
            return out;
        }

        inline Ustring Url::decode(Uview s) {
            size_t i = 0, j = 0, len = s.size();
            if (len < 3)
                return Ustring(s);
            Ustring out;
            while (i < len) {
                j = s.find('%', i);
                if (j > len - 3) {
                    out += make_view(s, i, npos);
                    break;
                }
                out += make_view(s, i, j - i);
                if (ascii_isxdigit(s[j + 1]) && ascii_isxdigit(s[j + 2])) {
                    out += char(hexnum(s.substr(j + 1, 2)));
                    i = j + 3;
                } else {
                    out += '%';
                    i = j + 1;
                }
            }
            return out;
        }

        inline Url Url::from_file(const File& f) {
            Ustring name = f.name(), u = "file:";
            if (starts_with(name, "//")) {
                u += name;
            } else {
                u += "///";
                u.append(name, size_t(name[0] == '/'), npos);
            }
            return Url(u);
        }

        template <typename R>
        Ustring Url::make_query(const R& range, char delimiter, uint32_t flags) {
            bool lone = flags & lone_keys;
            Ustring q;
            for (auto& kv: range) {
                Uview k(kv.first), v(kv.second);
                q += encode(k);
                if (! lone || ! v.empty())
                    q += '=';
                q += encode(v);
                q += delimiter;
            }
            if (! q.empty())
                q.pop_back();
            return q;
        }

        inline std::vector<std::pair<Ustring, Ustring>> Url::parse_query(Uview query, char delimiter) {
            std::vector<std::pair<Ustring, Ustring>> v;
            if (! delimiter) {
                delimiter = '&';
                size_t pos = query.find_first_of("&;");
                if (pos != npos)
                    delimiter = query[pos];
            }
            auto entries = splitv(query, Ustring{delimiter});
            for (auto& entry: entries) {
                auto kv = partition_at(entry, "=");
                v.push_back({decode(kv.first), decode(kv.second)});
            }
            return v;
        }

        inline void Url::check_nonempty() const {
            if (empty())
                throw std::invalid_argument("Individual URL elements can't be set on an empty URL");
        }

        inline int Url::find_path_cut() const noexcept {
            int i = query_pos - 1;
            while (i >= path_pos && text[i] != '/')
                --i;
            return i + 1;
        }

    namespace Literals {

        inline Url operator""_url(const char* p, size_t n) { return Url(Uview(p, n)); }

    }

}
