#pragma once

#include "rs-core/channel.hpp"
#include "rs-core/string.hpp"
#include <algorithm>
#include <atomic>
#include <cerrno>
#include <chrono>
#include <functional>
#include <memory>
#include <ostream>
#include <set>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <vector>

#ifdef _XOPEN_SOURCE
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <sys/ioctl.h>
    #include <sys/select.h>
    #include <sys/socket.h>
#endif

RS_LDLIB(mingw: ws2_32);

namespace RS {

    // Forward declarations

    class IPv4;
    class IPv6;
    class NetBase;
    class Socket;
    class SocketAddress;
    class SocketSet;
    class TcpClient;
    class TcpServer;
    class UdpClient;

    // Basic definitions

    #ifdef _XOPEN_SOURCE

        using SocketType = int;

        class NetBase {};

        namespace RS_Detail {

            using SocketFionbio = int;
            using SocketFlag = int;
            using SocketSendRecv = ssize_t;

            constexpr auto close_socket = &::close;
            constexpr auto ioctl_socket = &::ioctl;
            constexpr int e_again = EAGAIN;
            constexpr int e_badf = EBADF;

            inline void clear_error() noexcept { errno = 0; }
            inline int get_error() noexcept { return errno; }
            inline auto& net_category() { return std::generic_category(); }

        }

    #else

        using SocketType = SOCKET;

        class NetBase {
        public:
            NetBase() noexcept {
                static const auto init = [] () -> int {
                    WSADATA data;
                    memset(&data, 0, sizeof(data));
                    WSAStartup(0x202, &data);
                    return 0;
                }();
                (void)init;
            }
        };

        namespace RS_Detail {

            using SocketFionbio = unsigned long;
            using SocketFlag = char;
            using SocketSendRecv = int;

            constexpr auto close_socket = &::closesocket;
            constexpr auto ioctl_socket = &::ioctlsocket;
            constexpr int e_again = WSAEWOULDBLOCK;
            constexpr int e_badf = WSAENOTSOCK;

            inline void clear_error() noexcept { WSASetLastError(0); }
            inline int get_error() noexcept { return WSAGetLastError(); }
            inline auto& net_category() { return windows_category(); }

        }

    #endif

    namespace RS_Detail {

        template <typename T>
        struct NetResult {
            T res = 0;
            int err = 0;
            const NetResult& fail_if(T t, const char* f) const {
                if (res == t)
                    throw std::system_error(err, net_category(), f);
                return *this;
            }
        };

        template <typename T>
        NetResult<T> net_call(T t) noexcept {
            auto e = get_error();
            return {t, e};
        };

        inline void control_blocking(SocketType sock, bool state) {
            auto mode = SocketFionbio(! state);
            clear_error();
            net_call(ioctl_socket(sock, FIONBIO, &mode)).fail_if(-1, "ioctl()");
        }

        inline void control_nagle(SocketType sock, bool state) {
            auto mode = SocketFlag(! state);
            clear_error();
            net_call(::setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, &mode, sizeof(mode))).fail_if(-1, "setsockopt()");
        }

        inline uint16_t parse_port(const U8string& s) {
            if (s.empty() || ! ascii_isdigit(s[0]))
                throw std::invalid_argument("Invalid port number: " + quote(s));
            auto n = decnum(s);
            if (n < 0 || n > 65535)
                throw std::invalid_argument("Invalid port number: " + quote(s));
            return uint16_t(n);
        }

    }

    static constexpr auto no_socket = SocketType(-1);

    // IP address classes

    class IPv4:
    public LessThanComparable<IPv4>,
    private NetBase {
    public:
        static constexpr size_t size = 4;
        IPv4() noexcept { udata = 0; }
        explicit IPv4(uint32_t addr) noexcept: udata(addr) {}
        IPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) noexcept: bytes{a,b,c,d} {}
        explicit IPv4(const U8string& s);
        uint8_t operator[](unsigned i) const noexcept { return i < size ? bytes[i] : 0; }
        uint8_t* data() noexcept { return bytes; }
        const uint8_t* data() const noexcept { return bytes; }
        size_t hash() const noexcept { return std::hash<uint32_t>()(value()); }
        uint32_t net() const noexcept { return udata.rep(); }
        uint32_t& net() noexcept { return udata.rep(); }
        U8string str() const;
        uint32_t value() const noexcept { return udata; }
        static IPv4 any() noexcept { return {}; }
        static IPv4 broadcast() noexcept { return IPv4(0xffffffff); }
        static IPv4 localhost() noexcept { return IPv4(0x7f000001); }
        static IPv4 from_sin(const void* ptr) noexcept;
    private:
        union {
            BigEndian<uint32_t> udata;
            uint8_t bytes[size];
        };
    };

        static_assert(sizeof(IPv4) == IPv4::size, "Unexpected size for IPv4 class");

        inline IPv4::IPv4(const U8string& s) {
            using namespace RS_Detail;
            clear_error();
            auto rc = net_call(inet_pton(AF_INET, s.data(), bytes)).fail_if(-1, "inet_pton()");
            if (rc.res == 0)
                throw std::invalid_argument("Invalid IPv4 address: " + quote(s));
        }

        inline U8string IPv4::str() const {
            using namespace RS_Detail;
            auto vbytes = const_cast<uint8_t*>(bytes); // Windows brain damage
            U8string s(16, '\0');
            for (;;) {
                clear_error();
                auto rc = net_call(inet_ntop(AF_INET, vbytes, &s[0], s.size()));
                if (rc.res)
                    break;
                else if (rc.err != ENOSPC)
                    rc.fail_if(nullptr, "inet_ntop()");
                s.resize(s.size() + 16, '\0');
            }
            null_term(s);
            return s;
        }

        inline IPv4 IPv4::from_sin(const void* ptr) noexcept {
            IPv4 ip;
            if (ptr)
                memcpy(ip.bytes, ptr, sizeof(IPv4));
            return ip;
        }

        inline bool operator==(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() == rhs.value(); }
        inline bool operator<(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() < rhs.value(); }
        inline std::ostream& operator<<(std::ostream& out, IPv4 ip) { return out << ip.str(); }

    class IPv6:
    public LessThanComparable<IPv6>,
    private NetBase {
    public:
        static constexpr size_t size = 16;
        IPv6() noexcept { memset(bytes, 0, sizeof(bytes)); }
        IPv6(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h,
            uint8_t i, uint8_t j, uint8_t k, uint8_t l, uint8_t m, uint8_t n, uint8_t o, uint8_t p) noexcept:
            bytes{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p} {}
        explicit IPv6(const U8string& s);
        uint8_t operator[](unsigned i) const noexcept { return i < size ? reinterpret_cast<const uint8_t*>(this)[i] : 0; }
        uint8_t* data() noexcept { return bytes; }
        const uint8_t* data() const noexcept { return bytes; }
        size_t hash() const noexcept { return djb2a(bytes, size); }
        U8string str() const;
        static IPv6 any() noexcept { return {}; }
        static IPv6 localhost() noexcept { return IPv6(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1); }
        static IPv6 from_sin(const void* ptr) noexcept;
    private:
        union {
            uint8_t bytes[size];
            std::aligned_storage_t<size> align;
        };
    };

        static_assert(sizeof(IPv6) == IPv6::size, "Unexpected size for IPv6 class");

        inline IPv6::IPv6(const U8string& s) {
            using namespace RS_Detail;
            clear_error();
            auto rc = net_call(inet_pton(AF_INET6, s.data(), bytes)).fail_if(-1, "inet_pton()");
            if (rc.res == 0)
                throw std::invalid_argument("Invalid IPv6 address: " + quote(s));
        }

        inline U8string IPv6::str() const {
            using namespace RS_Detail;
            auto vbytes = const_cast<uint8_t*>(bytes); // Windows brain damage
            U8string s(40, '\0');
            for (;;) {
                clear_error();
                auto rc = net_call(inet_ntop(AF_INET6, vbytes, &s[0], s.size()));
                if (rc.res)
                    break;
                else if (rc.err != ENOSPC)
                    rc.fail_if(nullptr, "inet_ntop()");
                s.resize(s.size() + 40, '\0');
            }
            null_term(s);
            return s;
        }

        inline IPv6 IPv6::from_sin(const void* ptr) noexcept {
            IPv6 ip;
            if (ptr)
                memcpy(ip.bytes, ptr, sizeof(IPv6));
            return ip;
        }

        inline bool operator==(IPv6 lhs, IPv6 rhs) noexcept { return memcmp(&lhs, &rhs, IPv6::size) == 0; }
        inline bool operator<(IPv6 lhs, IPv6 rhs) noexcept { return memcmp(&lhs, &rhs, IPv6::size) < 0; }
        inline std::ostream& operator<<(std::ostream& out, IPv6 ip) { return out << ip.str(); }

    class SocketAddress:
    public LessThanComparable<SocketAddress>,
    private NetBase {
    private:
        union {
            sockaddr base;
            sockaddr_in inet4;
            sockaddr_in6 inet6;
        } sa_union;
        size_t current_size = 0;
    public:
        static constexpr size_t max_size = sizeof(sa_union);
        SocketAddress() noexcept { memset(&sa_union, 0, sizeof(sa_union)); }
        SocketAddress(IPv4 ip, uint16_t port = 0) noexcept;
        SocketAddress(IPv6 ip, uint16_t port = 0, uint32_t flow = 0, uint32_t scope = 0) noexcept;
        SocketAddress(const void* ptr, size_t n) noexcept;
        explicit SocketAddress(const U8string& s);
        explicit operator bool() const noexcept;
        uint8_t* data() noexcept { return reinterpret_cast<uint8_t*>(&sa_union); }
        const uint8_t* data() const noexcept { return reinterpret_cast<const uint8_t*>(&sa_union); }
        uint16_t family() const noexcept { return current_size < sizeof(sockaddr) ? 0 : sa_union.base.sa_family; }
        uint32_t flow() const noexcept { return family() == AF_INET6 ? ntohl(sa_union.inet6.sin6_flowinfo) : 0; }
        size_t hash() const noexcept { return djb2a(&sa_union, current_size); }
        IPv4 ipv4() const noexcept { return family() == AF_INET ? IPv4(ntohl(sa_union.inet4.sin_addr.s_addr)) : IPv4(); }
        IPv6 ipv6() const noexcept { return family() == AF_INET6 ? IPv6::from_sin(&sa_union.inet6.sin6_addr) : IPv6(); }
        sockaddr* native() noexcept { return &sa_union.base; }
        const sockaddr* native() const noexcept { return &sa_union.base; }
        uint16_t port() const noexcept;
        uint32_t scope() const noexcept { return family() == AF_INET6 ? ntohl(sa_union.inet6.sin6_scope_id) : 0; }
        void set_family(uint16_t f);
        void set_size(size_t n);
        size_t size() const noexcept { return current_size; }
    };

        inline SocketAddress::SocketAddress(IPv4 ip, uint16_t port) noexcept:
        SocketAddress() {
            sa_union.inet4.sin_family = AF_INET;
            sa_union.inet4.sin_port = htons(port);
            sa_union.inet4.sin_addr.s_addr = ip.net();
            current_size = sizeof(sockaddr_in);
        }

        inline SocketAddress::SocketAddress(IPv6 ip, uint16_t port, uint32_t flow, uint32_t scope) noexcept:
        SocketAddress() {
            memset(&sa_union, 0, sizeof(sa_union));
            sa_union.inet6.sin6_family = AF_INET6;
            sa_union.inet6.sin6_port = htons(port);
            sa_union.inet6.sin6_flowinfo = htonl(flow);
            memcpy(sa_union.inet6.sin6_addr.s6_addr, ip.data(), ip.size);
            sa_union.inet6.sin6_scope_id = htonl(scope);
            current_size = sizeof(sockaddr_in6);
        }

        inline SocketAddress::SocketAddress(const void* ptr, size_t n) noexcept:
        SocketAddress() {
            current_size = std::min(n, max_size);
            memcpy(&sa_union, ptr, current_size);
        }

        inline SocketAddress::SocketAddress(const U8string& s):
        SocketAddress() {
            using namespace RS_Detail;
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

        inline SocketAddress::operator bool() const noexcept {
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

        inline uint16_t SocketAddress::port() const noexcept {
            switch (family()) {
                case AF_INET:   return ntohs(sa_union.inet4.sin_port);
                case AF_INET6:  return ntohs(sa_union.inet6.sin6_port);
                default:        return 0;
            }
        }

        inline void SocketAddress::set_family(uint16_t f) {
            if (current_size < sizeof(sockaddr))
                current_size = sizeof(sockaddr);
            sa_union.base.sa_family = f;
        }

        inline void SocketAddress::set_size(size_t n) {
            if (n > max_size)
                throw std::length_error("SocketAddress size is too big");
            current_size = n;
        }

        inline bool operator==(const SocketAddress& lhs, const SocketAddress& rhs) noexcept {
            return mem_compare(lhs.native(), lhs.size(), rhs.native(), rhs.size()) == 0;
        }

        inline bool operator<(const SocketAddress& lhs, const SocketAddress& rhs) noexcept {
            return mem_compare(lhs.native(), lhs.size(), rhs.native(), rhs.size()) < 0;
        }

        inline std::ostream& operator<<(std::ostream& out, const SocketAddress& s) {
            if (s == SocketAddress())
                out << "null";
            else if (s.family() == AF_INET)
                out << s.ipv4() << ":" << s.port();
            else if (s.family() == AF_INET6)
                out << "[" << s.ipv6() << "]:" << s.port();
            else
                out << hexdump(s.native(), s.size());
            return out;
        }

    // DNS query functions

    class Dns {
    public:
        static SocketAddress host_to_ip(const U8string& name, int family = 0);
        static std::vector<SocketAddress> host_to_ips(const U8string& name, int family = 0);
        static U8string ip_to_host(const SocketAddress& addr);
    private:
        #ifdef _XOPEN_SOURCE
            static constexpr int eai_again = EAI_AGAIN;
            static constexpr int eai_noname = EAI_NONAME;
            static constexpr int eai_overflow = EAI_OVERFLOW;
            static constexpr int eai_system = EAI_SYSTEM;
        #else
            static constexpr int eai_again = WSATRY_AGAIN;
            static constexpr int eai_noname = WSAHOST_NOT_FOUND;
            static constexpr int eai_overflow = ERROR_INSUFFICIENT_BUFFER;
            static constexpr int eai_system = -1; // No equivalent on Windows
        #endif
        class addrinfo_error_category:
        public std::error_category {
        public:
            addrinfo_error_category() {}
            virtual U8string message(int ev) const { return uconv<U8string>(cstr(gai_strerror(ev))); }
            virtual const char* name() const noexcept { return "Addrinfo"; }
        };
        static const std::error_category& addrinfo_category() noexcept;
    };

        inline SocketAddress Dns::host_to_ip(const U8string& name, int family) {
            using namespace RS_Detail;
            static NetBase init;
            (void)init;
            addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = family;
            addrinfo* info = nullptr;
            ScopeExit guard([&] { if (info) freeaddrinfo(info); });
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

        inline std::vector<SocketAddress> Dns::host_to_ips(const U8string& name, int family) {
            using namespace RS_Detail;
            static NetBase init;
            (void)init;
            addrinfo hints;
            memset(&hints, 0, sizeof(hints));
            hints.ai_family = family;
            addrinfo* info = nullptr;
            ScopeExit guard([&] { if (info) freeaddrinfo(info); });
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

        inline U8string Dns::ip_to_host(const SocketAddress& addr) {
            using namespace RS_Detail;
            static NetBase init;
            (void)init;
            U8string name(100, '\0');
            for (;;) {
                clear_error();
                int rc = getnameinfo(addr.native(), addr.size(), &name[0], name.size(), nullptr, 0, 0);
                int err = get_error();
                switch (rc) {
                    case 0:             return null_term_str(name);
                    case eai_again:     return {};
                    case eai_noname:    return {};
                    case eai_overflow:  break;
                    case eai_system:    throw std::system_error(err, std::generic_category(), "getnameinfo()");
                    default:            throw std::system_error(rc, addrinfo_category(), "getnameinfo()");
                }
                name.resize(2 * name.size());
            }
        }

        inline const std::error_category& Dns::addrinfo_category() noexcept {
            static const addrinfo_error_category cat;
            return cat;
        }

    // TCP/IP socket classes

    class Socket:
    public StreamChannel,
    private NetBase {
    public:
        RS_MOVE_ONLY(Socket);
        Socket() = default;
        explicit Socket(SocketType s): sock(s) {}
        Socket(int domain, int type, int protocol = 0);
        virtual ~Socket() noexcept { do_close(); }
        virtual void close() noexcept { do_close(); }
        virtual size_t read(void* dst, size_t maxlen) { return do_read(dst, maxlen, nullptr); }
        SocketAddress local() const;
        SocketAddress remote() const;
        SocketType native() const noexcept { return sock; }
        size_t read_from(void* dst, size_t maxlen, SocketAddress& from) { return do_read(dst, maxlen, &from); }
        void set_blocking(bool state);
        bool write(const void* src, size_t len) { return do_write(src, len, nullptr); }
        bool write_str(const std::string& s) { return do_write(s.data(), s.size(), nullptr); }
        bool write_to(const void* src, size_t len, const SocketAddress& to) { return do_write(src, len, &to); }
    protected:
        virtual state do_wait(Interval::time t); // Defined after SocketSet
        void do_close() noexcept;
    private:
        SocketType sock = no_socket;
        size_t do_read(void* dst, size_t maxlen, SocketAddress* from); // Defined after SocketSet
        bool do_write(const void* src, size_t len, const SocketAddress* to);
    };

        inline Socket::Socket(int domain, int type, int protocol) {
            using namespace RS_Detail;
            clear_error();
            auto rc = net_call(::socket(domain, type, protocol)).fail_if(no_socket, "socket()");
            sock = rc.res;
        }

        inline SocketAddress Socket::local() const {
            using namespace RS_Detail;
            if (sock == no_socket)
                return {};
            SocketAddress sa;
            socklen_t len = SocketAddress::max_size;
            clear_error();
            net_call(::getsockname(sock, sa.native(), &len)).fail_if(-1, "getpeername()");
            sa.set_size(len);
            return sa;
        }

        inline SocketAddress Socket::remote() const {
            using namespace RS_Detail;
            if (sock == no_socket)
                return {};
            SocketAddress sa;
            socklen_t len = SocketAddress::max_size;
            clear_error();
            net_call(::getpeername(sock, sa.native(), &len)).fail_if(-1, "getpeername()");
            sa.set_size(len);
            return sa;
        }

        inline void Socket::set_blocking(bool state) {
            using namespace RS_Detail;
            control_blocking(sock, state);
        }

        inline void Socket::do_close() noexcept {
            using namespace RS_Detail;
            if (sock != no_socket) {
                close_socket(sock);
                sock = no_socket;
            }
        }

        inline bool Socket::do_write(const void* src, size_t len, const SocketAddress* to) {
            using namespace RS_Detail;
            using namespace std::chrono;
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
                    rc = net_call(::sendto(native(), csrc, len, flags, to->native(), to->size()));
                else
                    rc = net_call(::send(native(), csrc, len, flags));
                if (rc.res == -1 && rc.err == e_again)
                    sleep_for(microseconds(10));
                else
                    rc.fail_if(-1, to ? "sendto()" : "send()");
                written += rc.res;
            }
            return true;
        }

    class TcpClient:
    public Socket {
    public:
        RS_MOVE_ONLY(TcpClient);
        TcpClient() = default;
        explicit TcpClient(SocketType s) noexcept: Socket(s) {}
        explicit TcpClient(const SocketAddress& remote, const SocketAddress& local = {});
        template <typename... Args> explicit TcpClient(const Args&... args): TcpClient(SocketAddress{args...}) {}
        void set_nagle(bool state);
    };

        inline TcpClient::TcpClient(const SocketAddress& remote, const SocketAddress& local):
        Socket(PF_INET, SOCK_STREAM) {
            using namespace RS_Detail;
            if (local) {
                clear_error();
                net_call(::bind(native(), local.native(), local.size())).fail_if(-1, "bind()");
            }
            clear_error();
            net_call(::connect(native(), remote.native(), remote.size())).fail_if(-1, "connect()");
            control_blocking(native(), false);
            control_nagle(native(), false);
        }

        inline void TcpClient::set_nagle(bool state) {
            using namespace RS_Detail;
            control_nagle(native(), state);
        }

    class TcpServer:
    public MessageChannel<std::unique_ptr<TcpClient>>,
    private NetBase {
    public:
        RS_MOVE_ONLY(TcpServer);
        TcpServer() = default;
        explicit TcpServer(SocketType s) noexcept: sock(s) {}
        explicit TcpServer(const SocketAddress& local);
        template <typename... Args> explicit TcpServer(const Args&... args): TcpServer(SocketAddress{args...}) {}
        virtual void close() noexcept { sock.close(); }
        virtual bool read(std::unique_ptr<TcpClient>& t); // Defined after SocketSet
        SocketAddress local() const { return sock.local(); }
        SocketType native() const noexcept { return sock.native(); }
    protected:
        virtual state do_wait(Interval::time t) { return sock.wait(t); }
    private:
        Socket sock;
    };

        inline TcpServer::TcpServer(const SocketAddress& local):
        sock(PF_INET, SOCK_STREAM) {
            using namespace RS_Detail;
            static constexpr int backlog = 10;
            SocketFlag on = 1;
            ::setsockopt(sock.native(), SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
            if (local) {
                clear_error();
                net_call(::bind(sock.native(), local.native(), local.size())).fail_if(-1, "bind()");
            }
            control_blocking(native(), false);
            clear_error();
            net_call(::listen(sock.native(), backlog)).fail_if(-1, "listen()");
        }

    class UdpClient:
    public Socket {
    public:
        RS_MOVE_ONLY(UdpClient);
        UdpClient() = default;
        explicit UdpClient(SocketType s) noexcept: Socket(s) {}
        explicit UdpClient(const SocketAddress& remote, const SocketAddress& local = {});
        template <typename... Args> explicit UdpClient(const Args&... args): UdpClient(SocketAddress{args...}) {}
    };

        inline UdpClient::UdpClient(const SocketAddress& remote, const SocketAddress& local):
        Socket(PF_INET, SOCK_DGRAM) {
            using namespace RS_Detail;
            if (local) {
                clear_error();
                net_call(::bind(native(), local.native(), local.size())).fail_if(-1, "bind()");
            }
            if (remote) {
                clear_error();
                net_call(::connect(native(), remote.native(), remote.size())).fail_if(-1, "connect()");
            }
            control_blocking(native(), false);
        }

    class SocketSet:
    public MessageChannel<Channel*>,
    private NetBase {
    public:
        RS_NO_COPY_MOVE(SocketSet);
        SocketSet() = default;
        virtual void close() noexcept { open = false; }
        virtual bool read(Channel*& t);
        void clear() noexcept;
        bool empty() const noexcept { return channels.empty(); }
        void erase(Socket& s) noexcept { do_erase(s); }
        void erase(TcpServer& s) noexcept { do_erase(s); }
        void insert(Socket& s) { do_insert(s, s.native()); }
        void insert(TcpServer& s) { do_insert(s, s.native()); }
        size_t size() const noexcept { return channels.size(); }
    protected:
        virtual state do_wait(Interval::time t);
    private:
        friend class Socket;
        friend class TcpServer;
        std::vector<Channel*> channels;
        std::vector<SocketType> natives;
        Channel* current {nullptr};
        std::atomic<bool> open {true};
        void do_erase(Channel& c) noexcept;
        void do_insert(Channel& c, SocketType s);
        static Channel::state do_select(SocketType* sockets, size_t n, Interval::time t = {}, size_t* index = nullptr);
    };

        inline bool SocketSet::read(Channel*& t) {
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

        inline void SocketSet::clear() noexcept {
            channels.clear();
            natives.clear();
            current = nullptr;
        }

        inline Channel::state SocketSet::do_wait(Interval::time t) {
            if (! open)
                return state::closed;
            if (current)
                return state::ready;
            size_t index = npos;
            auto rc = do_select(natives.data(), natives.size(), t, &index);
            if (rc != state::waiting && index < channels.size())
                current = channels[index];
            if (rc == state::closed)
                return state::ready;
            else
                return rc;
        }

        inline void SocketSet::do_erase(Channel& c) noexcept {
            auto it = std::find(channels.begin(), channels.end(), &c);
            if (it != channels.end()) {
                natives.erase(natives.begin() + (it - channels.begin()));
                channels.erase(it);
                if (current == &c)
                    current = nullptr;
            }
        }

        inline void SocketSet::do_insert(Channel& c, SocketType s) {
            channels.push_back(&c);
            natives.push_back(s);
        }

        inline Channel::state SocketSet::do_select(SocketType* sockets, size_t n, Interval::time t, size_t* index) {
            using namespace RS_Detail;
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
            if (t > Interval::time())
                tv = duration_to_timeval(t);
            clear_error();
            auto rc = net_call(::select(last + 1, &rfds, nullptr, &efds, &tv));
            if (rc.res == 0)
                return Channel::state::waiting;
            else if (rc.res == -1 && rc.err == e_badf)
                return Channel::state::closed;
            else
                rc.fail_if(-1, "select()");
            size_t pos = npos;
            for (size_t i = 0; i < n && pos == npos; ++i)
                if (sockets[i] != no_socket && (FD_ISSET(sockets[i], &rfds) || FD_ISSET(sockets[i], &efds)))
                    pos = i;
            if (pos == npos)
                return Channel::state::waiting;
            if (index)
                *index = pos;
            return Channel::state::ready;
        }

        inline Channel::state Socket::do_wait(Interval::time t) {
            if (sock == no_socket)
                return state::closed;
            else
                return SocketSet::do_select(&sock, 1, t);
        }

        inline size_t Socket::do_read(void* dst, size_t maxlen, SocketAddress* from) {
            using namespace RS_Detail;
            if (! dst || ! maxlen || sock == no_socket || SocketSet::do_select(&sock, 1) != state::ready)
                return 0;
            auto cdst = static_cast<char*>(dst);
            NetResult<SocketSendRecv> rc;
            clear_error();
            if (from) {
                socklen_t addrlen = SocketAddress::max_size;
                rc = net_call(::recvfrom(native(), cdst, maxlen, 0, from->native(), &addrlen)).fail_if(-1, "recvfrom()");
                if (rc.res > 0)
                    from->set_size(addrlen);
            } else {
                rc = net_call(::recv(native(), cdst, maxlen, 0)).fail_if(-1, "recv()");
            }
            if (rc.res == 0)
                do_close();
            return size_t(rc.res);
        }

        inline bool TcpServer::read(std::unique_ptr<TcpClient>& t) {
            using namespace RS_Detail;
            if (sock.native() == no_socket)
                return false;
            auto s = sock.native();
            if (SocketSet::do_select(&s, 1) != state::ready)
                return false;
            clear_error();
            auto rc = net_call(::accept(sock.native(), nullptr, nullptr)).fail_if(no_socket, "socket()");
            t = std::make_unique<TcpClient>(rc.res);
            control_blocking(t->native(), false);
            control_nagle(t->native(), false);
            return true;
        }

}

RS_DEFINE_STD_HASH(RS::IPv4);
RS_DEFINE_STD_HASH(RS::IPv6);
RS_DEFINE_STD_HASH(RS::SocketAddress);
