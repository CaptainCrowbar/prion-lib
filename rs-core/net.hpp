#pragma once

#include "rs-core/channel.hpp"
#include "unicorn/utf.hpp"
#include <algorithm>
#include <atomic>
#include <functional>
#include <memory>
#include <ostream>
#include <string_view>
#include <type_traits>
#include <vector>

#ifdef _XOPEN_SOURCE
    #include <netinet/in.h>
    #include <sys/socket.h>
#endif

RS_LDLIB(msvc: ws2_32);

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

    #else

        using SocketType = uintptr_t;

        class NetBase {
        public:
            NetBase() noexcept;
        };

    #endif

    static constexpr auto no_socket = SocketType(-1);

    // IP address classes

    class IPv4 {
    public:
        static constexpr size_t size = 4;
        IPv4() noexcept { udata = 0; }
        explicit IPv4(uint32_t addr) noexcept: udata(addr) {}
        IPv4(uint8_t a, uint8_t b, uint8_t c, uint8_t d) noexcept: bytes{a,b,c,d} {}
        explicit IPv4(const Ustring& s);
        uint8_t operator[](unsigned i) const noexcept { return i < size ? bytes[i] : 0; }
        uint8_t* data() noexcept { return bytes; }
        const uint8_t* data() const noexcept { return bytes; }
        size_t hash() const noexcept { return std::hash<uint32_t>()(value()); }
        uint32_t net() const noexcept { return udata.rep(); }
        uint32_t& net() noexcept { return udata.rep(); }
        Ustring str() const;
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

    inline bool operator==(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() == rhs.value(); }
    inline bool operator!=(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() != rhs.value(); }
    inline bool operator<(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() < rhs.value(); }
    inline bool operator>(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() > rhs.value(); }
    inline bool operator<=(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() <= rhs.value(); }
    inline bool operator>=(IPv4 lhs, IPv4 rhs) noexcept { return lhs.value() >= rhs.value(); }
    inline std::ostream& operator<<(std::ostream& out, IPv4 ip) { return out << ip.str(); }
    inline Ustring to_str(IPv4 ip) { return ip.str(); }

    class IPv6 {
    public:
        static constexpr size_t size = 16;
        IPv6() noexcept { memset(bytes, 0, sizeof(bytes)); }
        IPv6(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h,
            uint8_t i, uint8_t j, uint8_t k, uint8_t l, uint8_t m, uint8_t n, uint8_t o, uint8_t p) noexcept:
            bytes{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p} {}
        explicit IPv6(const Ustring& s);
        uint8_t operator[](unsigned i) const noexcept { return i < size ? reinterpret_cast<const uint8_t*>(this)[i] : 0; }
        uint8_t* data() noexcept { return bytes; }
        const uint8_t* data() const noexcept { return bytes; }
        size_t hash() const noexcept;
        Ustring str() const;
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

    inline bool operator==(IPv6 lhs, IPv6 rhs) noexcept { return memcmp(&lhs, &rhs, IPv6::size) == 0; }
    inline bool operator!=(IPv6 lhs, IPv6 rhs) noexcept { return ! (lhs == rhs); }
    inline bool operator<(IPv6 lhs, IPv6 rhs) noexcept { return memcmp(&lhs, &rhs, IPv6::size) < 0; }
    inline bool operator>(IPv6 lhs, IPv6 rhs) noexcept { return rhs < lhs; }
    inline bool operator<=(IPv6 lhs, IPv6 rhs) noexcept { return ! (rhs < lhs); }
    inline bool operator>=(IPv6 lhs, IPv6 rhs) noexcept { return ! (lhs < rhs); }
    inline std::ostream& operator<<(std::ostream& out, IPv6 ip) { return out << ip.str(); }
    inline Ustring to_str(IPv6 ip) { return ip.str(); }

    class SocketAddress {
    private:
        union sa_union_type {
            sockaddr base;
            sockaddr_in inet4;
            sockaddr_in6 inet6;
        };
        sa_union_type sa_union;
        size_t current_size = 0;
    public:
        static constexpr size_t max_size = sizeof(sa_union_type);
        SocketAddress() noexcept { memset(&sa_union, 0, sizeof(sa_union_type)); }
        SocketAddress(IPv4 ip, uint16_t port = 0) noexcept;
        SocketAddress(IPv6 ip, uint16_t port = 0, uint32_t flow = 0, uint32_t scope = 0) noexcept;
        SocketAddress(const void* ptr, size_t n) noexcept;
        explicit SocketAddress(const Ustring& s);
        explicit operator bool() const noexcept;
        uint8_t* data() noexcept { return reinterpret_cast<uint8_t*>(&sa_union); }
        const uint8_t* data() const noexcept { return reinterpret_cast<const uint8_t*>(&sa_union); }
        uint16_t family() const noexcept { return current_size < sizeof(sockaddr) ? 0 : sa_union.base.sa_family; }
        uint32_t flow() const noexcept;
        size_t hash() const noexcept;
        IPv4 ipv4() const noexcept;
        IPv6 ipv6() const noexcept;
        sockaddr* native() noexcept { return &sa_union.base; }
        const sockaddr* native() const noexcept { return &sa_union.base; }
        uint16_t port() const noexcept;
        uint32_t scope() const noexcept;
        void set_family(uint16_t f);
        void set_size(size_t n);
        size_t size() const noexcept { return current_size; }
        Ustring str() const;
    };

    inline bool operator==(const SocketAddress& lhs, const SocketAddress& rhs) noexcept { return mem_compare(lhs.native(), lhs.size(), rhs.native(), rhs.size()) == 0; }
    inline bool operator!=(const SocketAddress& lhs, const SocketAddress& rhs) noexcept { return ! (lhs == rhs); }
    inline bool operator<(const SocketAddress& lhs, const SocketAddress& rhs) noexcept { return mem_compare(lhs.native(), lhs.size(), rhs.native(), rhs.size()) < 0; }
    inline bool operator>(const SocketAddress& lhs, const SocketAddress& rhs) noexcept { return rhs < lhs; }
    inline bool operator<=(const SocketAddress& lhs, const SocketAddress& rhs) noexcept { return ! (rhs < lhs); }
    inline bool operator>=(const SocketAddress& lhs, const SocketAddress& rhs) noexcept { return ! (lhs < rhs); }
    inline std::ostream& operator<<(std::ostream& out, const SocketAddress& sa) { return out << sa.str(); }
    inline Ustring to_str(const SocketAddress& sa) { return sa.str(); }

    // IP address literals

    namespace Literals {

        IPv4 operator""_ip4(const char* p, size_t n);
        IPv6 operator""_ip6(const char* p, size_t n);
        SocketAddress operator""_sa(const char* p, size_t n);

    }

    // DNS query functions

    class Dns {
    public:
        static SocketAddress host_to_ip(const Ustring& name, int family = 0);
        static std::vector<SocketAddress> host_to_ips(const Ustring& name, int family = 0);
        static Ustring ip_to_host(const SocketAddress& addr);
    };

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
        virtual bool is_closed() const noexcept { return sock == no_socket; }
        virtual size_t read(void* dst, size_t maxlen) { return do_read(dst, maxlen, nullptr); }
        SocketAddress local() const;
        SocketAddress remote() const;
        SocketType native() const noexcept { return sock; }
        size_t read_from(void* dst, size_t maxlen, SocketAddress& from) { return do_read(dst, maxlen, &from); }
        void set_blocking(bool flag);
        bool write(std::string_view s) { return do_write(s.data(), s.size(), nullptr); }
        bool write(const void* src, size_t len) { return do_write(src, len, nullptr); }
        bool write_to(std::string_view s, const SocketAddress& to) { return do_write(s.data(), s.size(), &to); }
        bool write_to(const void* src, size_t len, const SocketAddress& to) { return do_write(src, len, &to); }
    protected:
        virtual bool do_wait_for(duration t); // Defined after SocketSet
        void do_close() noexcept;
    private:
        SocketType sock = no_socket;
        size_t do_read(void* dst, size_t maxlen, SocketAddress* from); // Defined after SocketSet
        bool do_write(const void* src, size_t len, const SocketAddress* to);
    };

    class TcpClient:
    public Socket {
    public:
        RS_MOVE_ONLY(TcpClient);
        TcpClient() = default;
        explicit TcpClient(SocketType s) noexcept: Socket(s) {}
        explicit TcpClient(const SocketAddress& remote, const SocketAddress& local = {});
        template <typename... Args> explicit TcpClient(const Args&... args): TcpClient(SocketAddress{args...}) {}
        void set_nagle(bool flag);
    };

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
        virtual bool is_closed() const noexcept { return sock.is_closed(); }
        virtual bool read(std::unique_ptr<TcpClient>& t); // Defined after SocketSet
        SocketAddress local() const { return sock.local(); }
        SocketType native() const noexcept { return sock.native(); }
    protected:
        virtual bool do_wait_for(duration t) { return sock.wait_for(t); }
    private:
        Socket sock;
    };

    class UdpClient:
    public Socket {
    public:
        RS_MOVE_ONLY(UdpClient);
        UdpClient() = default;
        explicit UdpClient(SocketType s) noexcept: Socket(s) {}
        explicit UdpClient(const SocketAddress& remote, const SocketAddress& local = {});
        template <typename... Args> explicit UdpClient(const Args&... args): UdpClient(SocketAddress{args...}) {}
    };

    class SocketSet:
    public MessageChannel<Channel*>,
    private NetBase {
    public:
        RS_NO_COPY_MOVE(SocketSet);
        SocketSet() = default;
        virtual void close() noexcept { open = false; }
        virtual bool is_closed() const noexcept { return ! open; }
        virtual bool read(Channel*& t);
        void clear() noexcept;
        bool empty() const noexcept { return channels.empty(); }
        void erase(Socket& s) noexcept { do_erase(s); }
        void erase(TcpServer& s) noexcept { do_erase(s); }
        void insert(Socket& s) { do_insert(s, s.native()); }
        void insert(TcpServer& s) { do_insert(s, s.native()); }
        size_t size() const noexcept { return channels.size(); }
    protected:
        virtual bool do_wait_for(duration t);
    private:
        friend class Socket;
        friend class TcpServer;
        std::vector<Channel*> channels;
        std::vector<SocketType> natives;
        Channel* current {nullptr};
        std::atomic<bool> open {true};
        void do_erase(Channel& c) noexcept;
        void do_insert(Channel& c, SocketType s);
        static int do_select(SocketType* sockets, size_t n, duration t = {}, size_t* index = nullptr); // +1 = ready, 0 = timeout, -1 = socket closed
    };

}

RS_DEFINE_STD_HASH(RS::IPv4);
RS_DEFINE_STD_HASH(RS::IPv6);
RS_DEFINE_STD_HASH(RS::SocketAddress);
