//
// Created by jusra on 14-1-2026.
//

#include <iostream>

#include "commands/ListCommand.hpp"
#include "Modules/ModuleManager.hpp"

ListCommand::ListCommand(const ModuleManager& moduleManager)
    : _moduleManager(moduleManager) {}

std::string ListCommand::getName() {
    return "list";
}

std::string ListCommand::getDescription() {
    return "List all modules";
}

std::string ListCommand::execute(const std::vector<std::string> &args) {
    std::string list = "Enabled modules: \n\r";

    for (const LoadedModule& loaded_module : _moduleManager.getModules()) {
        list += " - " + loaded_module.instance->getName() + "\n\r";
    }

    return list;
}
