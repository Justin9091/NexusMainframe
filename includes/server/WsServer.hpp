#pragma once
#include <atomic>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
using ws_socket_t = SOCKET;
static constexpr ws_socket_t WS_INVALID_SOCKET = INVALID_SOCKET;
#else
using ws_socket_t = int;
static constexpr ws_socket_t WS_INVALID_SOCKET = -1;
#endif

class WsServer {
public:
    using MessageHandler  = std::function<void(int clientId, const std::string& msg)>;
    using CloseHandler    = std::function<void(int clientId)>;
    using ConnectHandler  = std::function<void(int clientId)>;

    ~WsServer();

    void start(int port);
    void stop();
    void send(int clientId, const std::string& msg);
    void broadcast(const std::string& msg);

    void onMessage(MessageHandler h)  { _onMessage  = std::move(h); }
    void onClose(CloseHandler h)      { _onClose    = std::move(h); }
    void onConnect(ConnectHandler h)  { _onConnect  = std::move(h); }

private:
    void acceptLoop();
    void clientLoop(int clientId, ws_socket_t sock);

    bool        handshake(ws_socket_t sock);
    std::string readFrame(ws_socket_t sock, bool& closed);
    bool        writeFrame(ws_socket_t sock, const std::string& payload);

    std::atomic<bool>                       _running{false};
    ws_socket_t                             _listenSock{WS_INVALID_SOCKET};
    std::thread                             _acceptThread;

    std::mutex                              _mu;
    std::unordered_map<int, ws_socket_t>    _clients;
    std::atomic<int>                        _nextId{0};

    MessageHandler  _onMessage;
    CloseHandler    _onClose;
    ConnectHandler  _onConnect;
};
