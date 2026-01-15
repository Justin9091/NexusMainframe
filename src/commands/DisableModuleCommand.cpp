//
// Created by jusra on 14-1-2026.
//

#include "commands/DisableModuleCommand.hpp"

DisableModuleCommand::DisableModuleCommand(ModuleLoader *event) {
    _moduleLoader = event;
}

std::string DisableModuleCommand::getName() {
    return "disable-module";
}

std::string DisableModuleCommand::getDescription() {
    return "Disables a module";
}

std::string DisableModuleCommand::execute(const std::vector<std::string> &args) {
    std::string moduleName = args[0];
    _moduleLoader->unloadModule(moduleName);

    return "Disabled " + moduleName;
}
