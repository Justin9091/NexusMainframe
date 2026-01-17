//
// Created by kikker234 on 16-01-2026.
//
#include "../../includes/cli/NexusCLI.h"

#include <iostream>
#include <sstream>
#include <cstring>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
static bool wsaInitialized = false;
#else
#include <termios.h>
#include <fcntl.h>
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
    std::cout << "  scan                    - Scan for new modules\n";
    std::cout << "  monitor [-c] [-i N]     - System monitoring\n";
    std::cout << "                            -c, --continuous  Continuous mode\n";
    std::cout << "                            -i, --interval N  Refresh interval (seconds)\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " help\n";
    std::cout << "  " << programName << " list\n";
    std::cout << "  " << programName << " enable mymodule\n";
    std::cout << "  " << programName << " monitor\n";
    std::cout << "  " << programName << " monitor -c\n";
    std::cout << "  " << programName << " monitor -c -i 5\n";
}

#ifdef _WIN32
bool kbhit() {
    return _kbhit() != 0;
}
#else
bool kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if(ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }

    return false;
}
#endif

void NexusCLI::runContinuousMonitor(int refreshInterval) {
#ifdef _WIN32
    // Enable ANSI escape sequences
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
#endif

    std::cout << "\033[?25l"; // Hide cursor
    std::cout << "\033[1;33mReal-time monitoring started. Press 'q' to quit...\033[0m\n\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));

    bool running = true;
    while (running) {
        // Clear screen and move to top
        std::cout << "\033[2J\033[H";

        // Reconnect for each request to avoid timeout
        if (!_connected) {
            if (!connect()) {
                std::cerr << "Lost connection to server" << std::endl;
                break;
            }
        }

        // Send monitor command (without -c flag to get single snapshot)
        std::string response = sendCommand("monitor");

        // Close connection after each request
        disconnect();

        std::cout << response << std::endl;
        std::cout << "\n\033[1;33m[Refresh: " << refreshInterval << "s | Press 'q' to quit]\033[0m" << std::endl;

        // Sleep and check for 'q' keypress
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::steady_clock::now() - start).count() < refreshInterval) {

            if (kbhit()) {
#ifdef _WIN32
                char ch = _getch();
#else
                char ch = getchar();
#endif
                if (ch == 'q' || ch == 'Q') {
                    running = false;
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    std::cout << "\033[?25h"; // Show cursor
    std::cout << "\n\033[1;32mMonitoring stopped.\033[0m\n";
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

    // Check for continuous monitor mode
    if (args[0] == "monitor") {
        bool continuous = false;
        int refreshInterval = 2;

        for (size_t i = 1; i < args.size(); ++i) {
            if (args[i] == "--continuous" || args[i] == "-c") {
                continuous = true;
            } else if (args[i] == "--interval" || args[i] == "-i") {
                if (i + 1 < args.size()) {
                    try {
                        refreshInterval = std::stoi(args[i + 1]);
                        if (refreshInterval < 1) refreshInterval = 1;
                        ++i;
                    } catch (...) {
                        std::cerr << "Invalid interval value" << std::endl;
                        return 1;
                    }
                }
            }
        }

        if (continuous) {
            runContinuousMonitor(refreshInterval);
            return 0;
        }
    }

    // Regular command execution
    if (!connect()) {
        return 1;
    }

    std::string command = buildCommand(args);
    std::string response = sendCommand(command);
    std::cout << response << std::endl;

    disconnect();
    return 0;
}