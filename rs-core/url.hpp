#pragma once

#include "rs-core/common.hpp"
#include <cstdlib>
#include <ostream>
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

    namespace Literals {

        inline Url operator""_url(const char* p, size_t n) { return Url(Uview(p, n)); }

    }

}
