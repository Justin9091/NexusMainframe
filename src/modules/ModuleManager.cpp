//
// Created by jusra on 15-1-2026.
//

#include "Modules/ModuleManager.hpp"

#include <filesystem>
#include <iostream>

#include "Modules/operations/ModuleOperationsFactory.hpp"

ModuleManager::ModuleManager()
    : loader_(ModuleOperationsFactory::create()),
      unloader_(ModuleOperationsFactory::create()) {}

void ModuleManager::loadFromDirectory(const std::string& path) {
    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;

        auto loaded = loader_.load(entry.path().string());
        if (!loaded) continue;

        loaded_.push_back(*loaded);
        EventBus::getInstance().publish({"event:enable", loaded->name});
    }
}

bool ModuleManager::unload(const std::string& name) {
    auto it = std::find_if(loaded_.begin(), loaded_.end(),
        [&](const LoadedModule& m) { return m.name == name; });

    if (it == loaded_.end()) return false;

    EventBus::getInstance().publish({"event:disable", it->name});
    unloader_.unload(*it);
    loaded_.erase(it);

    return true;
}

const std::vector<LoadedModule>& ModuleManager::getModules() const {
    return loaded_;
}

bool ModuleManager::load(const std::string &name) {
    std::string prefix = "J:\\NexusMainFrame\\cmake-build-debug\\Modules";
    std::string path = "J:\\NexusMainFrame\\cmake-build-debug\\Modules\\NexusAutomationModule.dll";

    std::optional<LoadedModule> loaded = loader_.load(path);
    if (!loaded.has_value()) {
        std::cout << "Failed to load\n\r";
        return false;
    }

    loaded.value().instance->initialize(EventBus::getInstance());

    loaded_.push_back(*loaded);

    return true;
}
