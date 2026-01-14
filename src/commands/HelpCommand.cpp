//
// Created by jusra on 14-1-2026.
//

#include "commands/HelpCommand.hpp"

std::string HelpCommand::getName() {
    return "help";
}

std::string HelpCommand::getDescription() {
    return "Shows available commands";
}

std::string HelpCommand::execute(const std::vector<std::string> &args) {
    std::string result = "Available commands:\n\r";
    for (auto &pair: CommandRegistry::getInstance().getAllCommands()) {
        result += " - " + pair.first + ": " + pair.second->getDescription() + "\n\r";
    }

    return result;
}
