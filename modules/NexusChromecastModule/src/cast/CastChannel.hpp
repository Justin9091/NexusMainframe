#pragma once
#include "TlsSocket.hpp"
#include "CastMessage.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include <mutex>
#include <string>

class CastChannel {
public:
    using MessageCallback    = std::function<void(const CastMessage&)>;
    using DisconnectCallback = std::function<void()>;

    CastChannel(std::string host, int port = 8009);
    ~CastChannel();

    CastChannel(const CastChannel&)            = delete;
    CastChannel& operator=(const CastChannel&) = delete;

    bool connect();
    void disconnect();
    bool isConnected() const;

    bool send(const CastMessage& msg);
    void onMessage(MessageCallback cb)       { messageCallback_    = std::move(cb); }
    void onDisconnect(DisconnectCallback cb) { disconnectCallback_ = std::move(cb); }

    bool sendConnect(const std::string& sourceId = "sender-0",
                     const std::string& destId   = "receiver-0");
    bool sendHeartbeat(const std::string& sourceId = "sender-0",
                       const std::string& destId   = "receiver-0");

private:
    bool readMessage(CastMessage& msg);
    void receiveLoop();
    void heartbeatLoop();

    std::string host_;
    int         port_;
    TlsSocket   socket_;

    MessageCallback    messageCallback_;
    DisconnectCallback disconnectCallback_;

    std::thread     recvThread_;
    std::thread     heartbeatThread_;
    std::atomic<bool> running_{false};
    mutable std::mutex sendMutex_;
};
