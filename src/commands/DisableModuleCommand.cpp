//
// Created by jusra on 14-1-2026.
//

#include "commands/DisableModuleCommand.hpp"

#include "Modules/ModuleManager.hpp"

DisableModuleCommand::DisableModuleCommand(ModuleManager& moduleManager): _moduleManager(moduleManager) {
}

std::string DisableModuleCommand::getName() {
    return "disable-module";
}

std::string DisableModuleCommand::getDescription() {
    return "Disables a module";
}

std::string DisableModuleCommand::execute(const std::vector<std::string> &args) {
    std::string moduleName = args[0];
    for (const LoadedModule & module : _moduleManager.getModules()) {
        if (module.name == moduleName) {
            _moduleManager.unload(moduleName);
            break;
        }
    }

    return "Disabled " + moduleName;
}
