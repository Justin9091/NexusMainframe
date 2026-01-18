//
// Created by jusra on 16-1-2026.
//

#include "commands/MonitorCommand.hpp"
#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <thread>

#include "commands/CommandRegistry.hpp"

#ifdef _WIN32
    #include <windows.h>
    #include <psapi.h>
    #include <conio.h>
#else
    #include <sys/resource.h>
    #include <unistd.h>
    #include <fstream>
    #include <termios.h>
    #include <fcntl.h>
#endif

std::string MonitorCommand::getName() {
    return "monitor";
}

std::string MonitorCommand::getDescription() {
    return "Real-time system monitoring";
}

std::string MonitorCommand::getMemoryUsage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        double memMB = pmc.WorkingSetSize / (1024.0 * 1024.0);
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << memMB << " MB";
        return oss.str();
    }
#else
    struct rusage usage;
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        double memMB = usage.ru_maxrss / 1024.0;
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2) << memMB << " MB";
        return oss.str();
    }
#endif
    return "N/A";
}

std::string MonitorCommand::getCPUUsage() {
#ifdef _WIN32
    FILETIME idleTime, kernelTime, userTime;
    if (GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return "Available";
    }
#else
    std::ifstream stat("/proc/stat");
    if (stat.is_open()) {
        std::string line;
        std::getline(stat, line);
        stat.close();
        return "Available";
    }
#endif
    return "N/A";
}

std::string MonitorCommand::execute(const std::vector<std::string> &args) {
#ifdef _WIN32
    // Enable ANSI escape sequences (Windows 10+)
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut != INVALID_HANDLE_VALUE) {
        DWORD dwMode = 0;
        if (GetConsoleMode(hOut, &dwMode)) {
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hOut, dwMode);
        }
    }
#endif

    // Check if continuous mode is requested
    bool continuous = false;
    int refreshInterval = 2; // Default 2 seconds

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--continuous" || args[i] == "-c") {
            continuous = true;
        } else if (args[i] == "--interval" || args[i] == "-i") {
            if (i + 1 < args.size()) {
                refreshInterval = std::stoi(args[i + 1]);
            }
        }
    }

    if (!continuous) {
        return generateMonitorOutput();
    } else {
        return "MONITOR_CONTINUOUS:" + std::to_string(refreshInterval);
    }
}

std::string MonitorCommand::generateMonitorOutput() {
    std::ostringstream result;

    result << "\033[1;36m"; // Cyan color
    result << "+========================================+\n";
    result << "|         NEXUS SYSTEM MONITOR          |\n";
    result << "+========================================+\033[0m\n\n";

    result << "\033[1;33m[System Information]\033[0m\n";
    result << "  Memory Usage    : \033[1;32m" << getMemoryUsage() << "\033[0m\n";
    result << "  CPU Status      : \033[1;32m" << getCPUUsage() << "\033[0m\n";

    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::string timeStr = std::ctime(&time);
    timeStr.pop_back(); // Remove newline
    result << "  Current Time    : \033[1;32m" << timeStr << "\033[0m\n\n";

    result << "\033[1;33m[Application Status]\033[0m\n";
    result << "  Status          : \033[1;32mRunning\033[0m\n";
    result << "  Commands Loaded : \033[1;32m"
           << CommandRegistry::getInstance().getAllCommands().size()
           << "\033[0m\n";

    return result.str();
}