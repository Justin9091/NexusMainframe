#pragma once
#include <string>
#include <vector>
#include <cstdint>

struct TlsSocketImpl;

class TlsSocket {
public:
    TlsSocket();
    ~TlsSocket();

    TlsSocket(const TlsSocket&)            = delete;
    TlsSocket& operator=(const TlsSocket&) = delete;

    bool connect(const std::string& host, int port);
    void disconnect();
    bool isConnected() const { return connected_; }

    bool sendAll(const std::vector<uint8_t>& data);
    bool recvAll(uint8_t* buf, size_t size);

private:
    bool            connected_ = false;
    TlsSocketImpl*  impl_      = nullptr;
};
