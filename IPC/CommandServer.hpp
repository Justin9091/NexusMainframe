#ifndef COMMAND_SERVER_HPP
#define COMMAND_SERVER_HPP

#include <string>
#include <functional>
#include <map>
#include <thread>
#include <atomic>
#include <mutex>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define CLOSE_SOCKET closesocket
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int SocketType;
#define INVALID_SOCKET_VALUE -1
#define CLOSE_SOCKET close
#endif

#include "Logger/Logger.hpp"

class CommandServer {
public:
    using CommandHandler = std::function<std::string(const std::string& args)>;

    CommandServer(int port = 9999);
    ~CommandServer();

    void start();
    void stop();
    void registerCommand(const std::string& cmd, CommandHandler handler);
    int getPort() const { return _port; }

private:
    void initializeSockets();
    void cleanupSockets();
    void acceptLoop();
    void handleClient(SocketType clientSocket);
    std::string processCommand(const std::string& input);

    int _port;
    SocketType _serverSocket = INVALID_SOCKET_VALUE;
    std::atomic<bool> _running{false};
    std::thread _acceptThread;

    std::map<std::string, CommandHandler> _handlers;
    std::mutex _handlersMutex;
    Logger _logger{"CommandServer"};
};

#endif