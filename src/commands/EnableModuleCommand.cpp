//
// Created by jusra on 14-1-2026.
//

#include "commands/EnableModuleCommand.hpp"

EnableModuleCommand::EnableModuleCommand(ModuleManager& moduleManager): _moduleManager(moduleManager) {
}

std::string EnableModuleCommand::getName() {
    return "enable-module";
}

std::string EnableModuleCommand::getDescription() {
    return "Enable an module";
}

std::string EnableModuleCommand::execute(const std::vector<std::string> &args) {
    std::string moduleName = args[0];
    if (moduleName.length() <= 0) return "Usage: enable-module <module name>";

    _moduleManager.load(moduleName);

    return "Module enabled!";
}
