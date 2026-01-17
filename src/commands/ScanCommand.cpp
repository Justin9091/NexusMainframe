#include "commands/ScanCommand.hpp"
#include "Modules/scanner/ModuleScannerFactory.hpp"
#include <sstream>

#include "config/ConfigProviderFactory.hpp"
#include "config/Adapter/LoadedModuleListAdapter.hpp"
#include "Modules/LoadedModule.hpp"
#include "pathing/PathManager.hpp"

std::string ScanCommand::getName() {
    return "scan";
}

std::string ScanCommand::getDescription() {
    return "Scans a directory for modules";
}

std::string ScanCommand::execute(const std::vector<std::string>& args) {
    if (args.empty()) {
        return "Usage: scan <directory>";
    }

    const std::string& directory = args[0];
    const auto start = std::chrono::steady_clock::now();

    auto scanner = ModuleScannerFactory::create();
    const auto modules = scanner->scanDirectory(directory);

    const auto end = std::chrono::steady_clock::now();
    const auto durationMs =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (modules.empty()) {
        return "No modules found in directory: " + directory +
               " (" + std::to_string(durationMs) + " ms)";
    }

    std::ostringstream output;
    output << "Found " << modules.size()
           << " module(s) in " << directory
           << " (" << durationMs << " ms)"
           << ":\r\n\r\n";

    PathManager& pathManager = PathManager::getInstance();
    std::string configPath = pathManager.get("modules.registry").string();

    auto config = ConfigProviderFactory::create(configPath);

    config.registerAdapter<std::vector<LoadedModule>>(
        std::make_shared<LoadedModuleListAdapter>()
    );

    std::vector<LoadedModule> loadedModules;

    for (const auto& mod : modules) {
        if (!mod.isValid) continue;

        output << " - " << mod.name << "\r\n"
               << "    File: " << mod.filename << "\r\n"
               << "    Path: " << mod.path << "\r\n\r\n";

        loadedModules.push_back({
            mod.name,
            mod.path,
            nullptr,
            nullptr
        });
    }

    config.setStruct("modules", loadedModules);
    config.save(configPath);

    return output.str();
}
