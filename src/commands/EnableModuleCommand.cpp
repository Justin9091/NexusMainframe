//
// Created by jusra on 14-1-2026.
//

#include "commands/EnableModuleCommand.hpp"

#include <iostream>

#include "config/ConfigProviderFactory.hpp"
#include "config/Adapter/LoadedModuleAdapter.hpp"
#include "config/Adapter/LoadedModuleListAdapter.hpp"

EnableModuleCommand::EnableModuleCommand(ModuleManager& moduleManager): _moduleManager(moduleManager) {
}

std::string EnableModuleCommand::getName() {
    return "enable-module";
}

std::string EnableModuleCommand::getDescription() {
    return "Enable an module";
}

std::string EnableModuleCommand::execute(const std::vector<std::string>& args) {
    if (args.empty())
        return "Usage: enable-module <module name>";

    const std::string& moduleName = args[0];

    ConfigProvider config =
        ConfigProviderFactory::create(
            "J:\\NexusMainFrame\\cmake-build-debug\\AvailableModules.json"
        );

    config.registerAdapter<std::vector<LoadedModule>>(
        std::make_shared<LoadedModuleListAdapter>()
    );

    try {
        config.load("J:\\NexusMainFrame\\cmake-build-debug\\AvailableModules.json");
    } catch (const std::exception& e) {
        return std::string("Failed to load config: ") + e.what();
    }

    auto modulesOpt = config.getStruct<std::vector<LoadedModule>>("modules");

    if (!modulesOpt)
        return "Couldn't get struct!";

    std::string path;

    for (const auto& m : *modulesOpt) {
        std::cout << "Comparing " << m.name
                  << " with " << moduleName << std::endl;

        if (m.name == moduleName) {
            path = m.path;
            break;
        }
    }

    if (path.empty())
        return "Module not found!";

    if (!_moduleManager.load(path))
        return "Failed to load module!";

    return "Module enabled!";
}