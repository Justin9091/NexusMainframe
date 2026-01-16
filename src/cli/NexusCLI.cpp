//
// Created by kikker234 on 16-01-2026.
//
#include "../../includes/cli/NexusCLI.h"

#include <iostream>
#include <sstream>
#include <cstring>

#ifdef _WIN32
static bool wsaInitialized = false;
#endif

NexusCLI::NexusCLI(const std::string& host, int port)
    : _host(host), _port(port), _socket(INVALID_SOCKET_VALUE), _connected(false) {
    initializeSockets();
}

NexusCLI::~NexusCLI() {
    disconnect();
    cleanupSockets();
}

void NexusCLI::initializeSockets() {
#ifdef _WIN32
    if (!wsaInitialized) {
        WSADATA wsaData;
        int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
        if (result != 0) {
            std::cerr << "WSAStartup failed: " << result << std::endl;
            return;
        }
        wsaInitialized = true;
    }
#endif
}

void NexusCLI::cleanupSockets() {
#ifdef _WIN32
    if (wsaInitialized) {
        WSACleanup();
        wsaInitialized = false;
    }
#endif
}

bool NexusCLI::connect() {
    if (_connected) {
        return true;
    }

    _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_socket == INVALID_SOCKET_VALUE) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(_port);

    if (inet_pton(AF_INET, _host.c_str(), &serverAddr.sin_addr) <= 0) {
        std::cerr << "Invalid address: " << _host << std::endl;
        CLOSE_SOCKET(_socket);
        _socket = INVALID_SOCKET_VALUE;
        return false;
    }

    if (::connect(_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Connection failed to " << _host << ":" << _port << std::endl;
        std::cerr << "Make sure the Nexus server is running." << std::endl;
        CLOSE_SOCKET(_socket);
        _socket = INVALID_SOCKET_VALUE;
        return false;
    }

    _connected = true;
    return true;
}

void NexusCLI::disconnect() {
    if (_socket != INVALID_SOCKET_VALUE) {
        CLOSE_SOCKET(_socket);
        _socket = INVALID_SOCKET_VALUE;
    }
    _connected = false;
}

std::string NexusCLI::sendCommand(const std::string& command) {
    if (!_connected) {
        return "Error: Not connected to server";
    }

    // Send command with newline
    std::string cmdWithNewline = command + "\n";

#ifdef _WIN32
    int bytesSent = send(_socket, cmdWithNewline.c_str(), (int)cmdWithNewline.length(), 0);
#else
    ssize_t bytesSent = write(_socket, cmdWithNewline.c_str(), cmdWithNewline.length());
#endif

    if (bytesSent <= 0) {
        return "Error: Failed to send command";
    }

    // Receive response
    std::string response;
    char buffer[4096];

#ifdef _WIN32
    int bytesRead = recv(_socket, buffer, sizeof(buffer) - 1, 0);
#else
    ssize_t bytesRead = read(_socket, buffer, sizeof(buffer) - 1);
#endif

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        response = buffer;
    } else {
        response = "Error: Failed to receive response";
    }

    return response;
}

std::string NexusCLI::buildCommand(const std::vector<std::string>& args) {
    std::ostringstream oss;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) oss << " ";
        oss << args[i];
    }
    return oss.str();
}

void NexusCLI::printUsage(const std::string& programName) {
    std::cout << "Usage: " << programName << " <command> [arguments...]\n\n";
    std::cout << "Available commands:\n";
    std::cout << "  help                    - Show available commands\n";
    std::cout << "  list                    - List all modules\n";
    std::cout << "  enable <module>         - Enable a module\n";
    std::cout << "  disable <module>        - Disable a module\n";
    std::cout << "  scan                    - Scan for new modules\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " help\n";
    std::cout << "  " << programName << " list\n";
    std::cout << "  " << programName << " enable mymodule\n";
}

int NexusCLI::run(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    // Build command from arguments
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }

    std::string command = buildCommand(args);

    // Connect to server
    if (!connect()) {
        return 1;
    }

    // Send command and get response
    std::string response = sendCommand(command);
    std::cout << response << std::endl;

    // Disconnect
    disconnect();

    return 0;
}