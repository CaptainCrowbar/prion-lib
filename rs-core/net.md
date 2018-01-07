# TCP/IP Networking #

By Ross Smith

* `#include "rs-core/net.hpp"`

## Contents ##

[TOC]

## Basic definitions ##

* `using` **`SocketType`** `= [integer type]`
* `static constexpr auto` **`no_socket`** `= SocketType(-1)`

The native socket handle type, and a value representing a nonexistent socket.

* `class` **`NetBase`**
    * `NetBase::`**`NetBase`**`() noexcept`
    * `NetBase::`**`~NetBase`**`() noexcept`
    * `NetBase::`**`NetBase`**`(const NetBase&) noexcept`
    * `NetBase::`**`NetBase`**`(NetBase&&) noexcept`
    * `NetBase& NetBase::`**`operator=`**`(const NetBase&) noexcept`
    * `NetBase& NetBase::`**`operator=`**`(NetBase&&) noexcept`

This class exists mainly to ensure that the network subsystem is initialized
properly on Windows (on Unix the constructor does nothing). Any class that
needs access to the networking functions should inherit privately from
`NetBase`, or include another class derived from it in its data members; any
function that needs networking should construct a `NetBase` object (normally
static) before doing anything else.

## IP address classes ##

### Class IPv4 ###

* `class` **`IPv4`**
    * `static constexpr size_t IPv4::`**`size`** `= 4`
    * `IPv4::`**`IPv4`**`() noexcept`
    * `explicit IPv4::`**`IPv4`**`(uint32_t addr) noexcept`
    * `IPv4::`**`IPv4`**`(uint8_t a, uint8_t b, uint8_t c, uint8_t d) noexcept`
    * `explicit IPv4::`**`IPv4`**`(const Ustring& s)`
    * `IPv4::`**`~IPv4`**`() noexcept`
    * `IPv4::`**`IPv4`**`(const IPv4&) noexcept`
    * `IPv4::`**`IPv4`**`(IPv4&&) noexcept`
    * `IPv4& IPv4::`**`operator=`**`(const IPv4&) noexcept`
    * `IPv4& IPv4::`**`operator=`**`(IPv4&&) noexcept`
    * `uint8_t IPv4::`**`operator[]`**`(unsigned i) const noexcept`
    * `uint8_t* IPv4::`**`data`**`() noexcept`
    * `const uint8_t* IPv4::`**`data`**`() const noexcept`
    * `size_t IPv4::`**`hash`**`() const noexcept`
    * `uint32_t IPv4::`**`net`**`() const noexcept`
    * `uint32_t& IPv4::`**`net`**`() noexcept`
    * `Ustring IPv4::`**`str`**`() const`
    * `uint32_t IPv4::`**`value`**`() const noexcept`
    * `static IPv4 IPv4::`**`any`**`() noexcept` _= 0.0.0.0_
    * `static IPv4 IPv4::`**`broadcast`**`() noexcept` _= 255.255.255.255_
    * `static IPv4 IPv4::`**`localhost`**`() noexcept` _= 127.0.0.1_
    * `static IPv4 IPv4::`**`from_sin`**`(const void* ptr) noexcept`
* `bool` **`operator==`**`(IPv4 lhs, IPv4 rhs) noexcept`
* `bool` **`operator!=`**`(IPv4 lhs, IPv4 rhs) noexcept`
* `bool` **`operator<`**`(IPv4 lhs, IPv4 rhs) noexcept`
* `bool` **`operator>`**`(IPv4 lhs, IPv4 rhs) noexcept`
* `bool` **`operator<=`**`(IPv4 lhs, IPv4 rhs) noexcept`
* `bool` **`operator>=`**`(IPv4 lhs, IPv4 rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& out, IPv4 ip)`
* `struct` **`std::hash<IPv4>`**

A class holding an IPv4 address. The default constructor sets the address to
zero; the other constructors accept a 32 bit address in host byte order, an
explicit list of bytes in network byte order, or a string containing the
address in conventional form (`"a.b.c.d"`); the string constructor will throw
`invalid_argument` if the string is not a valid IPv4 address. The `data()` and
`operator[]` functions access the address bytes (in network order); `net()`
and `value()` access the full 32 bit address in network or host order. The
`from_sin()` function extracts the IP address from a `sin_addr` or equivalent
structure (which is assumed to contain an IPv4 address).

### Class IPv6 ###

* `class` **`IPv6`**
    * `static constexpr size_t IPv6::`**`size`** `= 16`
    * `IPv6::`**`IPv6`**`() noexcept`
    * `IPv6::`**`IPv6`**`(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h, uint8_t i, uint8_t j, uint8_t k, uint8_t l, uint8_t m, uint8_t n, uint8_t o, uint8_t p) noexcept`
    * `explicit IPv6::`**`IPv6`**`(const Ustring& s)`
    * `IPv6::`**`~IPv6`**`() noexcept`
    * `IPv6::`**`IPv6`**`(const IPv6&) noexcept`
    * `IPv6::`**`IPv6`**`(IPv6&&) noexcept`
    * `IPv6& IPv6::`**`operator=`**`(const IPv6&) noexcept`
    * `IPv6& IPv6::`**`operator=`**`(IPv6&&) noexcept`
    * `uint8_t IPv6::`**`operator[]`**`(unsigned i) const noexcept`
    * `uint8_t* IPv6::`**`data`**`() noexcept`
    * `const uint8_t* IPv6::`**`data`**`() const noexcept`
    * `size_t IPv6::`**`hash`**`() const noexcept`
    * `Ustring IPv6::`**`str`**`() const`
    * `static IPv6 IPv6::`**`any`**`() noexcept` _= [::0]_
    * `static IPv6 IPv6::`**`localhost`**`() noexcept` _= [::1]_
    * `static IPv6 IPv6::`**`from_sin`**`(const void* ptr) noexcept`
* `bool` **`operator==`**`(IPv6 lhs, IPv6 rhs) noexcept`
* `bool` **`operator!=`**`(IPv6 lhs, IPv6 rhs) noexcept`
* `bool` **`operator<`**`(IPv6 lhs, IPv6 rhs) noexcept`
* `bool` **`operator>`**`(IPv6 lhs, IPv6 rhs) noexcept`
* `bool` **`operator<=`**`(IPv6 lhs, IPv6 rhs) noexcept`
* `bool` **`operator>=`**`(IPv6 lhs, IPv6 rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& out, IPv6 ip)`
* `struct` **`std::hash<IPv6>`**

A class holding an IPv6 address. Most of the functions have the obvious
semantics based on the corresponding function in the `IPv4` class.

### Class SocketAddress ###

* `class` **`SocketAddress`**
    * `static constexpr size_t SocketAddress::`**`max_size`** `= sizeof(sockaddr_in6)`
    * `SocketAddress::`**`SocketAddress`**`() noexcept`
    * `SocketAddress::`**`SocketAddress`**`(IPv4 ip, uint16_t port = 0) noexcept`
    * `SocketAddress::`**`SocketAddress`**`(IPv6 ip, uint16_t port = 0, uint32_t flow = 0, uint32_t scope = 0) noexcept`
    * `SocketAddress::`**`SocketAddress`**`(const void* ptr, size_t n) noexcept`
    * `explicit SocketAddress::`**`SocketAddress`**`(const Ustring& s)`
    * `SocketAddress::`**`~SocketAddress`**`() noexcept`
    * `SocketAddress::`**`SocketAddress`**`(const SocketAddress&) noexcept`
    * `SocketAddress::`**`SocketAddress`**`(SocketAddress&&) noexcept`
    * `SocketAddress& SocketAddress::`**`operator=`**`(const SocketAddress&) noexcept`
    * `SocketAddress& SocketAddress::`**`operator=`**`(SocketAddress&&) noexcept`
    * `explicit SocketAddress::`**`operator bool`**`() const noexcept`
    * `uint8_t* SocketAddress::`**`data`**`() noexcept`
    * `const uint8_t* SocketAddress::`**`data`**`() const noexcept`
    * `uint16_t SocketAddress::`**`family`**`() const noexcept`
    * `uint32_t SocketAddress::`**`flow`**`() const noexcept`
    * `size_t SocketAddress::`**`hash`**`() const noexcept`
    * `IPv4 SocketAddress::`**`ipv4`**`() const noexcept`
    * `IPv6 SocketAddress::`**`ipv6`**`() const noexcept`
    * `sockaddr* SocketAddress::`**`native`**`() noexcept`
    * `const sockaddr* SocketAddress::`**`native`**`() const noexcept`
    * `uint16_t SocketAddress::`**`port`**`() const noexcept`
    * `uint32_t SocketAddress::`**`scope`**`() const noexcept`
    * `void SocketAddress::`**`set_family`**`(uint16_t f)`
    * `void SocketAddress::`**`set_size`**`(size_t n)`
    * `size_t SocketAddress::`**`size`**`() const noexcept`
    * `Ustring SocketAddress::`**`str`**`() const`
* `bool` **`operator==`**`(const SocketAddress& lhs, const SocketAddress& rhs) noexcept`
* `bool` **`operator!=`**`(const SocketAddress& lhs, const SocketAddress& rhs) noexcept`
* `bool` **`operator<`**`(const SocketAddress& lhs, const SocketAddress& rhs) noexcept`
* `bool` **`operator>`**`(const SocketAddress& lhs, const SocketAddress& rhs) noexcept`
* `bool` **`operator<=`**`(const SocketAddress& lhs, const SocketAddress& rhs) noexcept`
* `bool` **`operator>=`**`(const SocketAddress& lhs, const SocketAddress& rhs) noexcept`
* `std::ostream&` **`operator<<`**`(std::ostream& out, const SocketAddress& ip)`
* `struct` **`std::hash<SocketAddress>`**

A class that holds a complete endpoint address, wrapping the Posix
`sockaddr_in` and `sockaddr_in6` structures. The default constructor fills the
socket address with zero bytes; the other constructors can take an IP address
and port, a pointer and size for an existing native socket address structure
(behaviour is undefined if the pointer is null or does not point to a valid
socket address), or an address and port in string format (`"a.b.c.d:port"` or
`"[ipv6-address]:port"`; this will throw `invalid_argument` if the string is
invalid).

The `ipv4()`, `ipv6()`, `port()`, `flow()`, and `scope()` functions will
return zero or a default constructed value if the address is not of the
appropriate type. The conversion to bool returns false if the address contains
all zero bytes; for known formats (IPv4 and IPv6), padding bytes are ignored.

### Address literals ###

All of these are in `namespace RS::Literals`.

* `IPv4` **`operator""_ip4`**`(const char* p, size_t n)`
* `IPv6` **`operator""_ip6`**`(const char* p, size_t n)`
* `SocketAddress` **`operator""_sa`**`(const char* p, size_t n)`

Literals for IPv4/6 addresses and endpoints. These call the constructor of the
relevant type that takes a string.

## DNS query functions ##

* `class` **`Dns`**
    * `static SocketAddress Dns::`**`host_to_ip`**`(const Ustring& name, int family = 0)`
    * `static std::vector<SocketAddress> Dns::`**`host_to_ips`**`(const Ustring& name, int family = 0)`
    * `static Ustring Dns::`**`ip_to_host`**`(const SocketAddress& addr)`

The `Dns` class only exists to act as a wrapper for the DNS resolution
functions.

The `host_to_ip[s]()` functions look up the IP addresses for a host name,
returning the principal name or a list of all names (with the principal name
first). The `family` argument can be used to restrict the DNS lookup to a
particular address family (e.g. `AF_INET` or `AF_INET6`), or set to zero to
return all associated addresses. These will return a null address or an empty
vector on a simple lookup failure (`EAI_AGAIN` or `EAI_NONAME`), or throw
`std::system_error` if anything else goes wrong.

The `ip_to_host()` function looks up the host name for an address (the address
family is included in the `SocketAddress` structure). This will return an
empty string on a simple lookup failure (`EAI_AGAIN` or `EAI_NONAME`), or
throw `std::system_error` if anything else goes wrong.

## TCP/IP socket classes ##

### Class Socket ###

* `class` **`Socket`**`: public StreamChannel`
    * `Socket::`**`Socket`**`() noexcept`
    * `explicit Socket::`**`Socket`**`(SocketType s)`
    * `Socket::`**`Socket`**`(int domain, int type, int protocol = 0)`
    * `virtual Socket::`**`~Socket`**`() noexcept`
    * `Socket::`**`Socket`**`(Socket&&) noexcept`
    * `Socket& Socket::`**`operator=`**`(Socket&&) noexcept`
    * `SocketAddress Socket::`**`local`**`() const`
    * `SocketAddress Socket::`**`remote`**`() const`
    * `SocketType Socket::`**`native`**`() const noexcept`
    * `size_t Socket::`**`read_from`**`(void* dst, size_t maxlen, SocketAddress& from)`
    * `void Socket::`**`set_blocking`**`(bool state)`
    * `bool Socket::`**`write`**`(std::string_view s)`
    * `bool Socket::`**`write`**`(const void* src, size_t len)`
    * `bool Socket::`**`write_to`**`(std::string_view s, const SocketAddress& to)`
    * `bool Socket::`**`write_to`**`(const void* src, size_t len, const SocketAddress& to)`

This is a wrapper around the native socket handle type. The constructor can
take a native socket (defaulting to `no_socket`), or a domain, type, and
protocol that will be passed to the operating system's `socket()` function.

The `read()` and `write()` functions call `recv()` and `send()`; `read_from()`
and `write_to()` call `recvfrom()` and `sendto()`. The `local()` and
`remote()` functions return the local and remote addresses from the last
socket operation.

The `set_blocking()` function controls the blocking state, which starts with
its normal default value for the socket type (normally enabled).

Any function that implicitly calls a native socket API function will throw
`system_error` if anything goes wrong.

### Class TcpClient ###

* `class` **`TcpClient`**`: public Socket`
    * `TcpClient::`**`TcpClient`**`() noexcept`
    * `explicit TcpClient::`**`TcpClient`**`(SocketType s) noexcept`
    * `explicit TcpClient::`**`TcpClient`**`(const SocketAddress& remote, const SocketAddress& local = {})`
    * `template <typename... Args> explicit TcpClient::`**`TcpClient`**`(const Args&... args)`
    * `virtual TcpClient::`**`~TcpClient`**`() noexcept`
    * `TcpClient::`**`TcpClient`**`(TcpClient&&) noexcept`
    * `TcpClient& TcpClient::`**`operator=`**`(TcpClient&&) noexcept`
    * `void TcpClient::`**`set_nagle`**`(bool state)`

A TCP client can be constructed from a native socket, a remote and local
address (which are passed to `bind()` and `connect()`), or a set of arguments
that are used to construct a `SocketAddress`, which is then passed to the
previous constructor.

The `set_nagle()` function controls the Nagle algorithm. By default, the Nagle
algorithm is off, and the socket is non-blocking.œ

Any function that implicitly calls a native socket API function will throw
`system_error` if anything goes wrong.

### Class TcpServer ###

* `class` **`TcpServer`**`: public MessageChannel<std::unique_ptr<TcpClient>>`
    * `TcpServer::`**`TcpServer`**`() noexcept`
    * `explicit TcpServer::`**`TcpServer`**`(SocketType s) noexcept`
    * `explicit TcpServer::`**`TcpServer`**`(const SocketAddress& local)`
    * `template <typename... Args> explicit TcpServer::`**`TcpServer`**`(const Args&... args)`
    * `virtual TcpServer::`**`~TcpServer`**`() noexcept`
    * `TcpServer::`**`TcpServer`**`(TcpServer&&) noexcept`
    * `TcpServer& TcpServer::`**`operator=`**`(TcpServer&&) noexcept`
    * `SocketAddress TcpServer::`**`local`**`() const`
    * `SocketType TcpServer::`**`native`**`() const noexcept`

A TCP server can be constructed from a native socket, a local address (passed
to `bind()` and `listen()`), or a set of arguments that are used to construct
a `SocketAddress`, which is then passed to the previous constructor.

Reading from a TCP server calls `accept()`, and wraps the resulting client
socket in a `TcpClient` object.

Any function that implicitly calls a native socket API function will throw
`system_error` if anything goes wrong.

### Class UdpClient ###

* `class` **`UdpClient`**`: public Socket`
    * `UdpClient::`**`UdpClient`**`() noexcept`
    * `explicit UdpClient::`**`UdpClient`**`(SocketType s) noexcept`
    * `explicit UdpClient::`**`UdpClient`**`(const SocketAddress& remote, const SocketAddress& local = {})`
    * `template <typename... Args> explicit UdpClient::`**`UdpClient`**`(const Args&... args)`
    * `virtual UdpClient::`**`~UdpClient`**`() noexcept`
    * `UdpClient::`**`UdpClient`**`(UdpClient&&) noexcept`
    * `UdpClient& UdpClient::`**`operator=`**`(UdpClient&&) noexcept`

A UDP client can be constructed from a native socket, a remote and local
address (which are passed to `bind()` and `connect()`, where relevant), or a
set of arguments that are used to construct a `SocketAddress`, which is then
passed to the previous constructor.

Any function that implicitly calls a native socket API function will throw
`system_error` if anything goes wrong.

### Class SocketSet ###

* `class` **`SocketSet`**`: public MessageChannel<Channel*>`
    * `SocketSet::`**`SocketSet`**`() noexcept`
    * `virtual SocketSet::`**`~SocketSet`**`() noexcept`
    * `void SocketSet::`**`clear`**`() noexcept`
    * `bool SocketSet::`**`empty`**`() const noexcept`
    * `void SocketSet::`**`erase`**`(Socket& s) noexcept`
    * `void SocketSet::`**`erase`**`(TcpServer& s) noexcept`
    * `void SocketSet::`**`insert`**`(Socket& s)`
    * `void SocketSet::`**`insert`**`(TcpServer& s)`
    * `size_t SocketSet::`**`size`**`() const noexcept`

This class holds a set of socket handles; the wait functions call `select()`
or the equivalent. The `insert()`, `erase()`, and related functions have their
usual semantics for a set-like container. The `read()` function yields a
pointer to the first channel that is available for reading or has been closed;
it will be null if no channels are ready. Any wait or read function will throw
`system_error` if anything goes wrong.
