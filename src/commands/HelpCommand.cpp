//
// Created by jusra on 14-1-2026.
//

#include "commands/HelpCommand.hpp"

#ifdef _WIN32
    #include <windows.h>
#endif

#include <iostream>

std::string HelpCommand::getName() {
    return "help";
}

std::string HelpCommand::getDescription() {
    return "Shows available commands";
}

std::string HelpCommand::execute(const std::vector<std::string> &args) {
    std::string result = "       __\n"
                         "    /\\ \\ \\_____  ___   _\n"
                         "   /  \\/ / _ \\ \\/ / | | / __|\n"
                         "  / /\\  /  __/>  <| |_| \\__ \\\n"
                         "  \\_\\ \\/ \\___/_/\\_\\\\__,_|___/\n\r\n\r\n";

#ifdef _WIN32
    // Enable ANSI colors op Windows 10+
    static bool initialized = false;
    if (!initialized) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        DWORD mode;
        if (GetConsoleMode(hConsole, &mode)) {
            SetConsoleMode(hConsole, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
        }
        initialized = true;
    }
#endif

    result += "\nAvailable commands:\n";
    for (auto &pair: CommandRegistry::getInstance().getAllCommands()) {
        result += " - " + pair.first + ": " + pair.second->getDescription() + "\n";
    }

    return result;
}