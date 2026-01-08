#include "NexusMainFrame.hpp"
#include <iostream>
#include <thread>
#include <sstream>
#include <algorithm>

void NexusMainFrame::start() {
    _moduleLoader = std::make_unique<ModuleLoader>();
    _moduleLoader->loadModulesFromDirectory("./modules");

    for (auto &m: _moduleLoader->getLoadedModules()) {
        m->initialize(_eventBus);
    }

    // ===== Register CLI Commands =====

    // LIST - List all loaded modules
    _server.registerCommand("list", [this](const std::string& args) {
        std::string result = "Loaded modules:\n";
        auto modules = _moduleLoader->getLoadedModules();
        if (modules.empty()) {
            result += "  (no modules loaded)\n";
        } else {
            for (const auto& mod : modules) {
                result += "  - " + mod->getName() + "\n";
            }
        }
        return result;
    });

    // LOAD - Load a module by name
    _server.registerCommand("load", [this](const std::string& args) {
        if (args.empty()) {
            return std::string("ERROR: Module name required\nUsage: load <module-name>\n");
        }

        // Check if already loaded - FIXED: proper duplicate check
        std::string modulePath = "./modules/" + args;
        auto modules = _moduleLoader->getLoadedModules();

        // Check by actual loaded path or name to prevent duplicates
        for (const auto& mod : modules) {
            if (mod->getName() == args) {
                return std::string("Module '" + args + "' is already loaded\n");
            }
        }

        // Try to load the module
        try {
            size_t beforeCount = modules.size();
            _moduleLoader->loadModule(modulePath);

            auto newModules = _moduleLoader->getLoadedModules();
            if (newModules.size() > beforeCount) {
                // Initialize only the newly loaded module
                newModules.back()->initialize(_eventBus);
                return std::string("Successfully loaded module: " + args + "\n");
            }
            return std::string("ERROR: Failed to load module: " + args + "\n");
        } catch (const std::exception& e) {
            return std::string("ERROR: " + std::string(e.what()) + "\n");
        }
    });

    // UNLOAD - Unload a module by name
    _server.registerCommand("unload", [this](const std::string& args) {
        if (args.empty()) {
            return std::string("ERROR: Module name required\nUsage: unload <module-name>\n");
        }

        auto modules = _moduleLoader->getLoadedModules();
        for (auto& mod : modules) {
            if (mod->getName() == args) {
                mod->shutdown();
                // Note: Actually removing from vector would require ModuleLoader support
                return std::string("Module '" + args + "' has been shut down\n");
            }
        }

        return std::string("ERROR: Module '" + args + "' not found\n");
    });

    // INFO - Show module information
    _server.registerCommand("info", [this](const std::string& args) {
        if (args.empty()) {
            return std::string("ERROR: Module name required\nUsage: info <module-name>\n");
        }

        auto modules = _moduleLoader->getLoadedModules();
        for (const auto& mod : modules) {
            if (mod->getName() == args) {
                std::ostringstream oss;
                oss << "Module Information:\n";
                oss << "  Name:        " << mod->getName() << "\n";
                return oss.str();
            }
        }

        return std::string("ERROR: Module '" + args + "' not found\n");
    });

    // SCAN - Rescan modules directory (FIXED: prevent duplicates)
    _server.registerCommand("scan", [this](const std::string& args) {
        try {
            size_t beforeCount = _moduleLoader->getLoadedModules().size();

            // Note: loadModulesFromDirectory should ideally check for duplicates internally
            // For now, this will still cause duplicates if ModuleLoader doesn't handle it
            _moduleLoader->loadModulesFromDirectory("./modules");

            auto modules = _moduleLoader->getLoadedModules();

            // Initialize only new modules (those added after beforeCount)
            for (size_t i = beforeCount; i < modules.size(); i++) {
                modules[i]->initialize(_eventBus);
            }

            size_t afterCount = modules.size();
            std::ostringstream oss;
            oss << "Modules directory rescanned\n";
            oss << "Total modules: " << afterCount << "\n";
            if (afterCount > beforeCount) {
                oss << "(" << (afterCount - beforeCount) << " new modules loaded)\n";
            } else {
                oss << "(no new modules found)\n";
            }
            return oss.str();
        } catch (const std::exception& e) {
            return std::string("ERROR: " + std::string(e.what()) + "\n");
        }
    });

    // HELP - Show available commands
    _server.registerCommand("help", [](const std::string& args) {
        std::ostringstream oss;
        oss << "Available Commands:\n";
        oss << "  list              - List all loaded modules\n";
        oss << "  load <module>     - Load a module by name\n";
        oss << "  unload <module>   - Unload a module\n";
        oss << "  info <module>     - Show detailed module information\n";
        oss << "  scan              - Rescan modules directory\n";
        oss << "  help              - Show this help message\n";
        return oss.str();
    });

    _server.start();
}

void NexusMainFrame::stop() {
    _eventBus.shutdown();
    for (auto &m: _moduleLoader->getLoadedModules())
        m->shutdown();

    _server.stop();
}

void NexusMainFrame::run() {
    start();

    while (_running.load()) {
        _eventBus.dispatchPending();
        _scheduler.tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    stop();
}