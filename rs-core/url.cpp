#include "rs-core/url.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <regex>
#include <stdexcept>
#include <string>

namespace RS {

    // Class Url

    Url::Url(Uview s) {
        if (! try_parse(s))
            throw std::invalid_argument("Invalid URL: " + quote(s));
    }

    Url::Url(Uview scheme, Uview user, Uview password, Uview host, uint16_t port, Uview path, Uview query, Uview fragment) {
        set_scheme(scheme);
        set_host(host);
        set_user(user);
        set_password(password);
        set_port(port);
        set_path(path);
        set_query(query);
        set_fragment(fragment);
    }

    void Url::set_scheme(Uview new_scheme) {
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

    void Url::set_user(Uview new_user) {
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

    void Url::set_password(Uview new_password) {
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

    void Url::set_host(Uview new_host) {
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

    void Url::set_port(uint16_t new_port) {
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

    void Url::set_path(Uview new_path) {
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

    void Url::set_query(Uview new_query) {
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

    void Url::set_fragment(Uview new_fragment) {
        if (new_fragment.empty()) {
            clear_fragment();
            return;
        }
        check_nonempty();
        text.replace(fragment_pos, npos, '#' + encode(new_fragment));
    }

    void Url::clear_user() noexcept {
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

    void Url::clear_password() noexcept {
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

    void Url::clear_host() noexcept {
        if (! has_host())
            return;
        int delta = port_pos - host_pos;
        text.erase(host_pos, delta);
        port_pos -= delta;
        path_pos -= delta;
        query_pos -= delta;
        fragment_pos -= delta;
    }

    void Url::clear_port() noexcept {
        if (! has_port())
            return;
        int delta = path_pos - port_pos;
        text.erase(port_pos, delta);
        path_pos -= delta;
        query_pos -= delta;
        fragment_pos -= delta;
    }

    void Url::clear_path() noexcept {
        if (! has_path())
            return;
        int delta = query_pos - path_pos;
        text.erase(path_pos, delta);
        query_pos -= delta;
        fragment_pos -= delta;
    }

    void Url::clear_query() noexcept {
        if (! has_query())
            return;
        text.erase(query_pos, fragment_pos - query_pos);
        fragment_pos = query_pos;
    }

    void Url::clear_fragment() noexcept {
        if (has_fragment())
            text.resize(fragment_pos);
    }

    void Url::append_path(Uview new_path) {
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

    void Url::clear() noexcept {
        text.clear();
        user_pos = password_pos = host_pos = port_pos = path_pos = query_pos = fragment_pos = 0;
    }

    Url Url::doc() const {
        Url u = *this;
        u.text.resize(fragment_pos);
        return u;
    }

    Url Url::base() const {
        Url u = *this;
        u.text.resize(query_pos);
        u.fragment_pos = u.query_pos;
        return u;
    }

    Url Url::parent() const {
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

    Url Url::root() const {
        Url u = *this;
        u.text.resize(path_pos);
        u.path_pos = int(u.text.size());
        if (query_pos > path_pos)
            u.text += '/';
        u.query_pos = u.fragment_pos = int(u.text.size());
        return u;
    }

    Ustring Url::path_dir() const {
        int cut = find_path_cut();
        return decode(text.substr(path_pos, cut - path_pos));
    }

    Ustring Url::path_leaf() const {
        int cut = find_path_cut();
        return decode(text.substr(cut, query_pos - cut));
    }

    bool Url::try_parse(Uview s) {
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

    Ustring Url::encode(Uview s, Uview exempt) {
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

    Ustring Url::decode(Uview s) {
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

    std::vector<std::pair<Ustring, Ustring>> Url::parse_query(Uview query, char delimiter) {
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

    void Url::check_nonempty() const {
        if (empty())
            throw std::invalid_argument("Individual URL elements can't be set on an empty URL");
    }

    int Url::find_path_cut() const noexcept {
        int i = query_pos - 1;
        while (i >= path_pos && text[i] != '/')
            --i;
        return i + 1;
    }

}
