//
// Created by jusra on 14-1-2026.
//

#include <iostream>

#include "commands/ListCommand.hpp"

ListCommand::ListCommand(ModuleLoader *moduleLoader) {
    _moduleLoader = moduleLoader;
}

std::string ListCommand::getName() {
    return "list";
}

std::string ListCommand::getDescription() {
    return "List all modules";
}

std::string ListCommand::execute(const std::vector<std::string> &args) {
    std::string list = "Enabled modules: \n\r";

    for (const std::shared_ptr<IModule> & loaded_module : _moduleLoader->getLoadedModules()) {
        list += " - " + loaded_module->getName() + "\n\r";
    }

    return list;
}
