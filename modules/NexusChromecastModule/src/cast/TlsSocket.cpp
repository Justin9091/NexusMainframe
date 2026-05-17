#include "TlsSocket.hpp"

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
using socket_t = SOCKET;
static constexpr socket_t SOCK_INVALID_VAL = INVALID_SOCKET;
static void closeSocket(socket_t s) { closesocket(s); }
#else
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
using socket_t = int;
static constexpr socket_t SOCK_INVALID_VAL = -1;
static void closeSocket(socket_t s) { ::close(s); }
#endif

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <string>

struct TlsSocketImpl {
    socket_t sock = SOCK_INVALID_VAL;
    SSL_CTX* ctx  = nullptr;
    SSL*     ssl  = nullptr;
};

static void initOpenSSL() {
    static bool done = false;
    if (!done) {
        SSL_library_init();
        SSL_load_error_strings();
        OpenSSL_add_all_algorithms();
        done = true;
    }
}

TlsSocket::TlsSocket() : impl_(new TlsSocketImpl) {
#ifdef _WIN32
    static bool wsaReady = false;
    if (!wsaReady) {
        WSADATA d;
        WSAStartup(MAKEWORD(2, 2), &d);
        wsaReady = true;
    }
#endif
}

TlsSocket::~TlsSocket() {
    disconnect();
    delete impl_;
}

bool TlsSocket::connect(const std::string& host, int port) {
    disconnect();

    // ── TCP connect ──────────────────────────────────────────────────────────
    struct addrinfo hints{}, *res = nullptr;
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &res) != 0 || !res)
        return false;

    socket_t sock = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == SOCK_INVALID_VAL) { freeaddrinfo(res); return false; }

    if (::connect(sock, res->ai_addr, static_cast<int>(res->ai_addrlen)) != 0) {
        closeSocket(sock);
        freeaddrinfo(res);
        return false;
    }
    freeaddrinfo(res);

    // ── TLS handshake ─────────────────────────────────────────────────────────
    initOpenSSL();

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (!ctx) { closeSocket(sock); return false; }

    // Chromecast devices use self-signed certificates
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);

    SSL* ssl = SSL_new(ctx);
    if (!ssl) { SSL_CTX_free(ctx); closeSocket(sock); return false; }

    SSL_set_fd(ssl, static_cast<int>(sock));
    SSL_set_tlsext_host_name(ssl, host.c_str());

    if (SSL_connect(ssl) != 1) {
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        closeSocket(sock);
        return false;
    }

    impl_->sock = sock;
    impl_->ctx  = ctx;
    impl_->ssl  = ssl;
    connected_  = true;
    return true;
}

void TlsSocket::disconnect() {
    if (impl_->ssl) {
        SSL_shutdown(impl_->ssl);
        SSL_free(impl_->ssl);
        impl_->ssl = nullptr;
    }
    if (impl_->ctx) {
        SSL_CTX_free(impl_->ctx);
        impl_->ctx = nullptr;
    }
    if (impl_->sock != SOCK_INVALID_VAL) {
        closeSocket(impl_->sock);
        impl_->sock = SOCK_INVALID_VAL;
    }
    connected_ = false;
}

bool TlsSocket::sendAll(const std::vector<uint8_t>& data) {
    if (!connected_ || !impl_->ssl) return false;

    size_t sent = 0;
    while (sent < data.size()) {
        int n = SSL_write(impl_->ssl,
                          data.data() + sent,
                          static_cast<int>(data.size() - sent));
        if (n <= 0) { connected_ = false; return false; }
        sent += static_cast<size_t>(n);
    }
    return true;
}

bool TlsSocket::recvAll(uint8_t* buf, size_t size) {
    if (!connected_ || !impl_->ssl) return false;

    size_t got = 0;
    while (got < size) {
        int n = SSL_read(impl_->ssl,
                         buf + got,
                         static_cast<int>(size - got));
        if (n <= 0) { connected_ = false; return false; }
        got += static_cast<size_t>(n);
    }
    return true;
}
