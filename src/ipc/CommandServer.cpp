#include "../../includes/IPC/CommandServer.hpp"
#include <sstream>
#include <cstring>

#include "commands/Command.hpp"
#include "commands/CommandRegistry.hpp"

#ifdef _WIN32
static bool wsaInitialized = false;
#endif

CommandServer::CommandServer(int port) : _port(port) {
    initializeSockets();
}

CommandServer::~CommandServer() {
    stop();
    cleanupSockets();
}

void CommandServer::initializeSockets() {
#ifdef _WIN32
    if (!wsaInitialized) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            _logger.logError("WSAStartup failed: " + std::to_string(result));
            return;
        }
        wsaInitialized = true;
    }
#endif
}

void CommandServer::cleanupSockets() {
#ifdef _WIN32
    if (wsaInitialized) {
        WSACleanup();
        wsaInitialized = false;
    }
#endif
}

void CommandServer::start() {
    if (_running) {
        _logger.logWarning("Server already running");
        return;
    }

    // Create TCP socket
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_serverSocket == INVALID_SOCKET_VALUE) {
        _logger.logError("Failed to create socket");
        return;
    }

    // SO_REUSEADDR to allow immediate restart
    int opt = 1;
    setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR,
               (const char*)&opt, sizeof(opt));

    // Bind to localhost - FIXED: use inet_pton instead of inet_addr
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;

    // Use inet_pton for proper IPv4 address conversion
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0) {
        _logger.logError("Invalid address / Address not supported");
        CLOSE_SOCKET(_serverSocket);
        _serverSocket = INVALID_SOCKET_VALUE;
        return;
    }

    addr.sin_port = htons(_port);

    if (bind(_serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        _logger.logError("Failed to bind to port " + std::to_string(_port));
        CLOSE_SOCKET(_serverSocket);
        _serverSocket = INVALID_SOCKET_VALUE;
        return;
    }

    // Listen for connections
    if (listen(_serverSocket, 5) < 0) {
        _logger.logError("Failed to listen on socket");
        CLOSE_SOCKET(_serverSocket);
        _serverSocket = INVALID_SOCKET_VALUE;
        return;
    }

    _running = true;
    _acceptThread = std::thread(&CommandServer::acceptLoop, this);

    _logger.logInfo("Command server started on localhost:" + std::to_string(_port));
}

void CommandServer::stop() {
    if (!_running) return;

    _running = false;

    if (_serverSocket != INVALID_SOCKET_VALUE) {
        CLOSE_SOCKET(_serverSocket);
        _serverSocket = INVALID_SOCKET_VALUE;
    }

    if (_acceptThread.joinable()) {
        _acceptThread.join();
    }

    _logger.logInfo("Command server stopped");
}

void CommandServer::acceptLoop() {
    while (_running) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        SocketType clientSocket = accept(_serverSocket,
                                        (struct sockaddr*)&clientAddr,
                                        &clientLen);

        if (clientSocket == INVALID_SOCKET_VALUE) {
            if (_running) {
                _logger.logError("Accept failed");
            }
            continue;
        }

        // Spawn thread for each client
        std::thread([this, clientSocket]() {
            handleClient(clientSocket);
        }).detach();
    }
}

void CommandServer::handleClient(SocketType clientSocket) {
    std::string buffer;
    char temp[1024];

    while (true) {
#ifdef _WIN32
        int bytesRead = recv(clientSocket, temp, sizeof(temp) - 1, 0);
#else
        ssize_t bytesRead = read(clientSocket, temp, sizeof(temp) - 1);
#endif
        if (bytesRead <= 0) break;  // client gesloten of error

        temp[bytesRead] = '\0';
        buffer += temp;

        // Check voor newline
        size_t pos;
        while ((pos = buffer.find_first_of("\r\n")) != std::string::npos) {
            std::string line = buffer.substr(0, pos);
            buffer.erase(0, pos + 1);

            // Verwijder extra whitespace
            while (!line.empty() && (line.back() == '\r' || line.back() == '\n')) line.pop_back();

            if (!line.empty()) {
                std::string response = processCommand(line);

#ifdef _WIN32
                send(clientSocket, response.c_str(), (int)response.length(), 0);
#else
                write(clientSocket, response.c_str(), response.length());
#endif
            }
        }
    }

    CLOSE_SOCKET(clientSocket);
}

std::string CommandServer::processCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string cmd;
    iss >> cmd;

    std::vector<std::string> args;
    std::string arg;
    while (iss >> arg) {
        args.push_back(arg);
    }

    std::lock_guard<std::mutex> lock(_handlersMutex);
    Command* command = CommandRegistry::getInstance().getCommand(cmd);
    if (!command) return "Command not found: " + cmd;

    return command->execute(args);
}
