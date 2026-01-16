#pragma once

#include <string>
#include <vector>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
#define INVALID_SOCKET_VALUE INVALID_SOCKET
#define CLOSE_SOCKET closesocket
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
typedef int SocketType;
#define INVALID_SOCKET_VALUE -1
#define CLOSE_SOCKET close
#endif

class NexusCLI {
public:
    NexusCLI(const std::string& host = "127.0.0.1", int port = 9090);
    ~NexusCLI();

    bool connect();
    void disconnect();
    std::string sendCommand(const std::string& command);
    int run(int argc, char* argv[]);

private:
    std::string _host;
    int _port;
    SocketType _socket;
    bool _connected;

    void initializeSockets();
    void cleanupSockets();
    std::string buildCommand(const std::vector<std::string>& args);
    void printUsage(const std::string& programName);
};