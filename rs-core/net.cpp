#include "rs-core/net.hpp"
#include "rs-core/digest.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <cerrno>
#include <chrono>
#include <set>
#include <stdexcept>
#include <string>
#include <system_error>
#include <thread>

#ifdef _XOPEN_SOURCE
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/tcp.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
#endif

using namespace std::chrono;
using namespace std::literals;

namespace RS {

    namespace {

        #ifdef _XOPEN_SOURCE

            using SocketFlag = int;
            using SocketIop = int;
            using SocketSendRecv = ssize_t;

            constexpr int e_again = EAGAIN;
            constexpr int e_badf = EBADF;
            constexpr int eai_again = EAI_AGAIN;
            constexpr int eai_noname = EAI_NONAME;
            constexpr int eai_overflow = EAI_OVERFLOW;
            constexpr int eai_system = EAI_SYSTEM;

            void clear_error() noexcept { errno = 0; }
            int close_socket(SocketType s) noexcept { return ::close(s); }
            int get_error() noexcept { return errno; }
            constexpr auto ioctl_socket = &::ioctl;

        #else

            using SocketFlag = char;
            using SocketIop = unsigned long;
            using SocketSendRecv = int;
            using sa_family_t = unsigned short;

            constexpr int e_again = WSAEWOULDBLOCK;
            constexpr int e_badf = WSAENOTSOCK;
            constexpr int eai_again = WSATRY_AGAIN;
            constexpr int eai_noname = WSAHOST_NOT_FOUND;
            constexpr int eai_overflow = ERROR_INSUFFICIENT_BUFFER;
            constexpr int eai_system = -1; // No equivalent on Windows

            void clear_error() noexcept { WSASetLastError(0); }
            int close_socket(SocketType s) noexcept { return ::closesocket(s); }
            int get_error() noexcept { return WSAGetLastError(); }
            int ioctl_socket(SocketType s, long r, SocketIop* p) noexcept { return ::ioctlsocket(s, r, p); }

        #endif

        #ifdef _MSC_VER

            using dns_name_size = uint32_t;
            using socket_iosize = int;

        #else

            using dns_name_size = socklen_t;
            using socket_iosize = socklen_t;

        #endif

        template <typename T>
        struct NetResult {
            T res = 0;
            int err = 0;
            const NetResult& fail_if(T t, const char* f) const {
                if (res == t)
                    throw std::system_error(err, std::system_category(), f);
                return *this;
            }
        };

        template <typename T>
        NetResult<T> net_call(T t) noexcept {
            auto e = get_error();
            return {t, e};
        };

        void control_blocking(SocketType sock, bool flag) {
            auto mode = SocketIop(! flag);
            clear_error();
            net_call(ioctl_socket(sock, FIONBIO, &mode)).fail_if(-1, "ioctl()");
        }

        void control_nagle(SocketType sock, bool flag) {
            auto mode = SocketFlag(! flag);
            clear_error();
            net_call(::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &mode, sizeof(mode))).fail_if(-1, "setsockopt()");
        }

        uint16_t parse_port(const Ustring& s) {
            if (s.empty() || ! ascii_isdigit(s[0]))
                throw std::invalid_argument("Invalid port number: " + quote(s));
            auto n = decnum(s);
            if (n < 0 || n > 65535)
                throw std::invalid_argument("Invalid port number: " + quote(s));
            return uint16_t(n);
        }

        class AddrinfoCategory:
        public std::error_category {
        public:
            AddrinfoCategory() {}
            virtual Ustring message(int ev) const { return cstr(gai_strerror(ev)); }
            virtual const char* name() const noexcept { return "Addrinfo"; }
        };

        const std::error_category& addrinfo_category() noexcept {
            static const AddrinfoCategory cat;
            return cat;
        }

    }

    // Class NetBase

    #ifndef _XOPEN_SOURCE

        NetBase::NetBase() noexcept {
            static const auto init = [] () -> int {
                WSADATA data;
                memset(&data, 0, sizeof(data));
                WSAStartup(0x202, &data);
                return 0;
            }();
            (void)init;
        }

    #endif

    // Class IPv4

    IPv4::IPv4(const Ustring& s) {
        static const NetBase init;
        (void)init;
        clear_error();
        auto rc = net_call(inet_pton(AF_INET, s.data(), bytes)).fail_if(-1, "inet_pton()");
        if (rc.res == 0)
            throw std::invalid_argument("Invalid IPv4 address: " + quote(s));
    }

    Ustring IPv4::str() const {
        static const NetBase init;
        (void)init;
        auto vbytes = const_cast<uint8_t*>(bytes); // Windows brain damage
        Ustring s(16, '\0');
        for (;;) {
            clear_error();
            auto rc = net_call(inet_ntop(AF_INET, vbytes, &s[0], socklen_t(s.size())));
            if (rc.res)
                break;
            else if (rc.err != ENOSPC)
                rc.fail_if(nullptr, "inet_ntop()");
            s.resize(s.size() + 16, '\0');
        }
        null_term(s);
        return s;
    }

    IPv4 IPv4::from_sin(const void* ptr) noexcept {
        IPv4 ip;
        if (ptr)
            memcpy(ip.bytes, ptr, sizeof(IPv4));
        return ip;
    }

    // Class IPv6

    IPv6::IPv6(const Ustring& s) {
        static const NetBase init;
        (void)init;
        clear_error();
        auto rc = net_call(inet_pton(AF_INET6, s.data(), bytes)).fail_if(-1, "inet_pton()");
        if (rc.res == 0)
            throw std::invalid_argument("Invalid IPv6 address: " + quote(s));
    }

    size_t IPv6::hash() const noexcept {
        return Djb2a()(bytes, size);
    }

    Ustring IPv6::str() const {
        static const NetBase init;
        (void)init;
        auto vbytes = const_cast<uint8_t*>(bytes); // Windows brain damage
        Ustring s(40, '\0');
        for (;;) {
            clear_error();
            auto rc = net_call(inet_ntop(AF_INET6, vbytes, &s[0], socklen_t(s.size())));
            if (rc.res)
                break;
            else if (rc.err != ENOSPC)
                rc.fail_if(nullptr, "inet_ntop()");
            s.resize(s.size() + 40, '\0');
        }
        null_term(s);
        return s;
    }

    IPv6 IPv6::from_sin(const void* ptr) noexcept {
        IPv6 ip;
        if (ptr)
            memcpy(ip.bytes, ptr, sizeof(IPv6));
        return ip;
    }

    // Class SocketAddress

    SocketAddress::SocketAddress(IPv4 ip, uint16_t port) noexcept:
    SocketAddress() {
        sa_union.inet4.sin_family = AF_INET;
        sa_union.inet4.sin_port = htons(port);
        sa_union.inet4.sin_addr.s_addr = ip.net();
        current_size = sizeof(sockaddr_in);
    }

    SocketAddress::SocketAddress(IPv6 ip, uint16_t port, uint32_t flow, uint32_t scope) noexcept:
    SocketAddress() {
        memset(&sa_union, 0, sizeof(sa_union_type));
        sa_union.inet6.sin6_family = AF_INET6;
        sa_union.inet6.sin6_port = htons(port);
        sa_union.inet6.sin6_flowinfo = htonl(flow);
        memcpy(sa_union.inet6.sin6_addr.s6_addr, ip.data(), ip.size);
        sa_union.inet6.sin6_scope_id = htonl(scope);
        current_size = sizeof(sockaddr_in6);
    }

    SocketAddress::SocketAddress(const void* ptr, size_t n) noexcept:
    SocketAddress() {
        current_size = std::min(n, max_size);
        memcpy(&sa_union, ptr, current_size);
    }

    SocketAddress::SocketAddress(const Ustring& s):
    SocketAddress() {
        if (ascii_isdigit(s[0])) {
            size_t colon = s.find(':');
            if (colon != npos && s.find(':', colon + 1) != npos) {
                *this = IPv6(s);
                return;
            }
            IPv4 ip(s.substr(0, colon));
            uint16_t port = 0;
            if (colon != npos)
                port = parse_port(s.substr(colon + 1, npos));
            *this = SocketAddress(ip, port);
            return;
        }
        if (s[0] == '[') {
            size_t close = s.find(']');
            if (close != npos) {
                IPv6 ip(s.substr(1, close - 1));
                uint16_t port = 0;
                if (close + 2 < s.size())
                    port = parse_port(s.substr(close + 2, npos));
                *this = SocketAddress(ip, port);
                return;
            }
        }
        throw std::invalid_argument("Invalid socket address: " + quote(s));
    }

    SocketAddress::operator bool() const noexcept {
        switch (family()) {
            case 0:         return false;
            case AF_INET:   return ipv4() != IPv4() || port() != 0;
            case AF_INET6:  return ipv6() != IPv6() || port() != 0;
            default:        break;
        }
        for (size_t i = sizeof(sa_union.base); i < current_size; ++i)
            if (data()[i])
                return true;
        return false;
    }

    uint32_t SocketAddress::flow() const noexcept {
        return family() == AF_INET6 ? ntohl(sa_union.inet6.sin6_flowinfo) : 0;
    }

    size_t SocketAddress::hash() const noexcept {
        return Djb2a()(&sa_union, current_size);
    }

    IPv4 SocketAddress::ipv4() const noexcept {
        return family() == AF_INET ? IPv4(ntohl(sa_union.inet4.sin_addr.s_addr)) : IPv4();
    }

    IPv6 SocketAddress::ipv6() const noexcept {
        return family() == AF_INET6 ? IPv6::from_sin(&sa_union.inet6.sin6_addr) : IPv6();
    }

    uint16_t SocketAddress::port() const noexcept {
        switch (family()) {
            case AF_INET:   return ntohs(sa_union.inet4.sin_port);
            case AF_INET6:  return ntohs(sa_union.inet6.sin6_port);
            default:        return 0;
        }
    }

    uint32_t SocketAddress::scope() const noexcept {
        return family() == AF_INET6 ? ntohl(sa_union.inet6.sin6_scope_id) : 0;
    }

    void SocketAddress::set_family(uint16_t f) {
        if (current_size < sizeof(sockaddr))
            current_size = sizeof(sockaddr);
        sa_union.base.sa_family = sa_family_t(f);
    }

    void SocketAddress::set_size(size_t n) {
        if (n > max_size)
            throw std::length_error("SocketAddress size is too big");
        current_size = n;
    }

    Ustring SocketAddress::str() const {
        if (*this == SocketAddress())
            return "null";
        else if (family() == AF_INET)
            return ipv4().str() + ':' + dec(port());
        else if (family() == AF_INET6)
            return '[' + ipv6().str() + "]:" + dec(port());
        else
            return hexdump(native(), size());
    }

    // IP address literals

    namespace Literals {

        IPv4 operator""_ip4(const char* p, size_t n) {
            if (! p || ! n)
                return {};
            std::string s(p, n);
            return IPv4(s);
        }

        IPv6 operator""_ip6(const char* p, size_t n) {
            if (! p || ! n)
                return {};
            std::string s(p, n);
            return IPv6(s);
        }

        SocketAddress operator""_sa(const char* p, size_t n) {
            if (! p || ! n)
                return {};
            std::string s(p, n);
            return SocketAddress(s);
        }

    }

    // Class Dns

    SocketAddress Dns::host_to_ip(const Ustring& name, int family) {
        static NetBase init;
        (void)init;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = family;
        addrinfo* info = nullptr;
        auto guard = scope_exit([&] { if (info) freeaddrinfo(info); });
        clear_error();
        int rc = getaddrinfo(name.data(), nullptr, &hints, &info);
        int err = get_error();
        switch (rc) {
            case 0:           return SocketAddress(info->ai_addr, info->ai_addrlen);
            case eai_again:   return {};
            case eai_noname:  return {};
            case eai_system:  throw std::system_error(err, std::generic_category(), "getaddrinfo()");
            default:          throw std::system_error(rc, addrinfo_category(), "getaddrinfo()");
        }
    }

    std::vector<SocketAddress> Dns::host_to_ips(const Ustring& name, int family) {
        static NetBase init;
        (void)init;
        addrinfo hints;
        memset(&hints, 0, sizeof(hints));
        hints.ai_family = family;
        addrinfo* info = nullptr;
        auto guard = scope_exit([&] { if (info) freeaddrinfo(info); });
        clear_error();
        int rc = getaddrinfo(name.data(), nullptr, &hints, &info);
        int err = get_error();
        switch (rc) {
            case 0:           break;
            case eai_again:   return {};
            case eai_noname:  return {};
            case eai_system:  throw std::system_error(err, std::generic_category(), "getaddrinfo()");
            default:          throw std::system_error(rc, addrinfo_category(), "getaddrinfo()");
        }
        std::vector<SocketAddress> addrs;
        std::set<SocketAddress> seen;
        for (auto ai = info; ai; ai = ai->ai_next) {
            SocketAddress sa(ai->ai_addr, ai->ai_addrlen);
            if (sa && seen.insert(sa).second)
                addrs.push_back(sa);
        }
        return addrs;
    }

    Ustring Dns::ip_to_host(const SocketAddress& addr) {
        static NetBase init;
        (void)init;
        Ustring name(100, '\0');
        for (;;) {
            clear_error();
            int rc = getnameinfo(addr.native(), socklen_t(addr.size()), &name[0], dns_name_size(name.size()), nullptr, 0, 0);
            int err = get_error();
            switch (rc) {
                case 0:             null_term(name); return name;
                case eai_again:     return {};
                case eai_noname:    return {};
                case eai_overflow:  break;
                case eai_system:    throw std::system_error(err, std::generic_category(), "getnameinfo()");
                default:            throw std::system_error(rc, addrinfo_category(), "getnameinfo()");
            }
            name.resize(2 * name.size());
        }
    }

    // Class Socket

    Socket::Socket(int domain, int type, int protocol) {
        clear_error();
        auto rc = net_call(::socket(domain, type, protocol)).fail_if(no_socket, "socket()");
        sock = rc.res;
    }

    SocketAddress Socket::local() const {
        if (sock == no_socket)
            return {};
        SocketAddress sa;
        socklen_t len = SocketAddress::max_size;
        clear_error();
        net_call(::getsockname(sock, sa.native(), &len)).fail_if(-1, "getpeername()");
        sa.set_size(len);
        return sa;
    }

    SocketAddress Socket::remote() const {
        if (sock == no_socket)
            return {};
        SocketAddress sa;
        socklen_t len = SocketAddress::max_size;
        clear_error();
        net_call(::getpeername(sock, sa.native(), &len)).fail_if(-1, "getpeername()");
        sa.set_size(len);
        return sa;
    }

    void Socket::set_blocking(bool flag) {
        control_blocking(sock, flag);
    }

    void Socket::do_close() noexcept {
        if (sock != no_socket) {
            close_socket(sock);
            sock = no_socket;
        }
    }

    bool Socket::do_write(const void* src, size_t len, const SocketAddress* to) {
        if (! src || sock == no_socket)
            return false;
        auto csrc = static_cast<const char*>(src);
        int flags = 0;
        #ifdef MSG_NOSIGNAL
            flags |= MSG_NOSIGNAL;
        #endif
        size_t written = 0;
        NetResult<SocketSendRecv> rc;
        clear_error();
        while (written < len) {
            if (to)
                rc = net_call(::sendto(native(), csrc, socket_iosize(len), flags, to->native(), socket_iosize(to->size())));
            else
                rc = net_call(::send(native(), csrc, socket_iosize(len), flags));
            if (rc.res == -1 && rc.err == e_again)
                std::this_thread::sleep_for(10us);
            else
                rc.fail_if(-1, to ? "sendto()" : "send()");
            written += rc.res;
        }
        return true;
    }

    // Class TcpClient

    TcpClient::TcpClient(const SocketAddress& remote, const SocketAddress& local):
    Socket(PF_INET, SOCK_STREAM) {
        if (local) {
            clear_error();
            net_call(::bind(native(), local.native(), socket_iosize(local.size()))).fail_if(-1, "bind()");
        }
        clear_error();
        net_call(::connect(native(), remote.native(), socket_iosize(remote.size()))).fail_if(-1, "connect()");
        control_blocking(native(), false);
        control_nagle(native(), false);
    }

    void TcpClient::set_nagle(bool flag) {
        control_nagle(native(), flag);
    }

    // Class TcpServer

    TcpServer::TcpServer(const SocketAddress& local):
    sock(PF_INET, SOCK_STREAM) {
        static constexpr int backlog = 10;
        SocketFlag on = 1;
        ::setsockopt(sock.native(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
        if (local) {
            clear_error();
            net_call(::bind(sock.native(), local.native(), socket_iosize(local.size()))).fail_if(-1, "bind()");
        }
        control_blocking(native(), false);
        clear_error();
        net_call(::listen(sock.native(), backlog)).fail_if(-1, "listen()");
    }

    // Class UdpClient

    UdpClient::UdpClient(const SocketAddress& remote, const SocketAddress& local):
    Socket(PF_INET, SOCK_DGRAM) {
        if (local) {
            clear_error();
            net_call(::bind(native(), local.native(), socket_iosize(local.size()))).fail_if(-1, "bind()");
        }
        if (remote) {
            clear_error();
            net_call(::connect(native(), remote.native(), socket_iosize(remote.size()))).fail_if(-1, "connect()");
        }
        control_blocking(native(), false);
    }

    // Class SocketSet

    bool SocketSet::read(Channel*& t) {
        if (! open || channels.empty())
            return false;
        if (current) {
            t = current;
            current = nullptr;
            return true;
        }
        size_t index = npos;
        do_select(natives.data(), natives.size(), {}, &index);
        if (index == npos)
            return false;
        t = channels[index];
        return true;
    }

    void SocketSet::clear() noexcept {
        channels.clear();
        natives.clear();
        current = nullptr;
    }

    bool SocketSet::do_wait_for(duration t) {
        if (! open || current)
            return true;
        size_t index = npos;
        int rc = do_select(natives.data(), natives.size(), t, &index);
        if (rc && index < channels.size())
            current = channels[index];
        return rc;
    }

    void SocketSet::do_erase(Channel& c) noexcept {
        auto it = std::find(channels.begin(), channels.end(), &c);
        if (it != channels.end()) {
            natives.erase(natives.begin() + (it - channels.begin()));
            channels.erase(it);
            if (current == &c)
                current = nullptr;
        }
    }

    void SocketSet::do_insert(Channel& c, SocketType s) {
        channels.push_back(&c);
        natives.push_back(s);
    }

    int SocketSet::do_select(SocketType* sockets, size_t n, duration t, size_t* index) {
        if (index)
            *index = npos;
        fd_set rfds;
        FD_ZERO(&rfds);
        int last = -1;
        for (size_t i = 0; i < n; ++i) {
            if (sockets[i] != no_socket) {
                FD_SET(sockets[i], &rfds);
                last = std::max(last, int(sockets[i]));
            }
        }
        fd_set efds = rfds;
        timeval tv = {0, 0};
        if (t > duration())
            tv = duration_to_timeval(t);
        clear_error();
        auto rc = net_call(::select(last + 1, &rfds, nullptr, &efds, &tv));
        if (rc.res == 0)
            return 0;
        else if (rc.res == -1 && rc.err == e_badf)
            return -1;
        rc.fail_if(-1, "select()");
        size_t pos = npos;
        for (size_t i = 0; i < n && pos == npos; ++i)
            if (sockets[i] != no_socket && (FD_ISSET(sockets[i], &rfds) || FD_ISSET(sockets[i], &efds)))
                pos = i;
        if (pos == npos)
            return 0;
        if (index)
            *index = pos;
        return 1;
    }

    bool Socket::do_wait_for(duration t) {
        return sock == no_socket || SocketSet::do_select(&sock, 1, t);
    }

    size_t Socket::do_read(void* dst, size_t maxlen, SocketAddress* from) {
        if (! dst || ! maxlen || sock == no_socket || SocketSet::do_select(&sock, 1) != 1)
            return 0;
        auto cdst = static_cast<char*>(dst);
        NetResult<SocketSendRecv> rc;
        clear_error();
        if (from) {
            socklen_t addrlen = SocketAddress::max_size;
            rc = net_call(::recvfrom(native(), cdst, socket_iosize(maxlen), 0, from->native(), &addrlen)).fail_if(-1, "recvfrom()");
            if (rc.res > 0)
                from->set_size(addrlen);
        } else {
            rc = net_call(::recv(native(), cdst, socket_iosize(maxlen), 0)).fail_if(-1, "recv()");
        }
        if (rc.res == 0)
            do_close();
        return size_t(rc.res);
    }

    bool TcpServer::read(std::unique_ptr<TcpClient>& t) {
        if (sock.native() == no_socket)
            return false;
        auto s = sock.native();
        if (SocketSet::do_select(&s, 1) != 1)
            return false;
        clear_error();
        auto rc = net_call(::accept(sock.native(), nullptr, nullptr)).fail_if(no_socket, "socket()");
        t = std::make_unique<TcpClient>(rc.res);
        control_blocking(t->native(), false);
        control_nagle(t->native(), false);
        return true;
    }

}
