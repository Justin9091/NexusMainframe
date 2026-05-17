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

/**
 * @brief Raw WebSocket server (RFC 6455) with per-client integer IDs.
 *
 * Listens on a TCP port, performs the HTTP upgrade handshake, and runs a
 * dedicated thread per client.  Incoming text frames are delivered via the
 * onMessage() callback; connection and disconnection events are reported via
 * onConnect() / onClose().
 *
 * Thread-safety: send() and broadcast() are guarded by an internal mutex and
 * may be called from any thread.
 *
 * @code
 * WsServer ws;
 * ws.onMessage([](int id, const std::string& msg) { ... });
 * ws.onConnect([](int id) { ... });
 * ws.onClose([](int id)   { ... });
 * ws.start(8082);
 * // ... later:
 * ws.broadcast("{\"event\":\"update\"}");
 * ws.stop();
 * @endcode
 */
class WsServer {
public:
    using MessageHandler = std::function<void(int clientId, const std::string& msg)>; ///< Called for each incoming text frame.
    using CloseHandler   = std::function<void(int clientId)>;                          ///< Called when a client disconnects.
    using ConnectHandler = std::function<void(int clientId)>;                          ///< Called when a new client connects.

    ~WsServer();

    /**
     * @brief Starts the server and begins accepting connections.
     * Spawns a background accept thread; returns immediately.
     * @param port TCP port to listen on.
     */
    void start(int port);

    /** @brief Stops the server and disconnects all clients. */
    void stop();

    /**
     * @brief Sends a text frame to a specific client.
     * @param clientId  Client ID as reported by the connect handler.
     * @param msg       UTF-8 payload.
     */
    void send(int clientId, const std::string& msg);

    /**
     * @brief Sends a text frame to every connected client.
     * @param msg UTF-8 payload.
     */
    void broadcast(const std::string& msg);

    /** @brief Registers the incoming-message handler. */
    void onMessage(MessageHandler h) { _onMessage = std::move(h); }

    /** @brief Registers the client-close handler. */
    void onClose(CloseHandler h)     { _onClose   = std::move(h); }

    /** @brief Registers the client-connect handler. */
    void onConnect(ConnectHandler h) { _onConnect = std::move(h); }

private:
    void acceptLoop();
    void clientLoop(int clientId, ws_socket_t sock);

    bool        handshake(ws_socket_t sock);
    std::string readFrame(ws_socket_t sock, bool& closed);
    bool        writeFrame(ws_socket_t sock, const std::string& payload);

    std::atomic<bool>                    _running{false};
    ws_socket_t                          _listenSock{WS_INVALID_SOCKET};
    std::thread                          _acceptThread;

    std::mutex                           _mu;
    std::unordered_map<int, ws_socket_t> _clients;
    std::atomic<int>                     _nextId{0};

    MessageHandler _onMessage;
    CloseHandler   _onClose;
    ConnectHandler _onConnect;
};
