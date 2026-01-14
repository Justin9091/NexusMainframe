//
// Created by jusra on 14-1-2026.
//

#include "commands/EnableModuleCommand.hpp"

EnableModuleCommand::EnableModuleCommand(ModuleLoader *moduleLoader) {
    _moduleLoader = moduleLoader;
}

std::string EnableModuleCommand::getName() {
    return "enable-module";
}

std::string EnableModuleCommand::getDescription() {
    return "Enable an module";
}

std::string EnableModuleCommand::execute(const std::vector<std::string> &args) {
    return "WIP";
}
