#include "modules/ModuleManager.hpp"

#include <algorithm>
#include <filesystem>
#include <iostream>

#include "modules/operations/ModuleOperationsFactory.hpp"
#include "paths/PathManager.hpp"

ModuleManager::ModuleManager()
    : loader_(ModuleOperationsFactory::create()),
      stateStore_(PathManager::getInstance().get("config") / "modules.state.json") {
    stateStore_.load();
}

void ModuleManager::loadFromDirectory(const std::string& path) {
    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(path)) {
        if (!entry.is_regular_file()) continue;

        const std::string name = entry.path().stem().string();
        if (isLoaded(name)) continue;

        auto mod = loader_.load(name);
        if (!mod) continue;

        (*mod)->initialize(EventBus::getInstance());
        EventBus::getInstance().publish({"event:enable", mod->getName()});

        stateStore_.add(name);
        loaded_.push_back(std::move(*mod));
    }

    stateStore_.save();
}

void ModuleManager::loadFromState() {
    for (const auto& name : stateStore_.getAll()) {
        if (isLoaded(name)) continue;

        auto mod = loader_.load(name);
        if (!mod) {
            std::cout << "[ModuleManager] Failed to restore module: " << name << "\n";
            continue;
        }

        (*mod)->initialize(EventBus::getInstance());
        EventBus::getInstance().publish({"event:enable", mod->getName()});
        loaded_.push_back(std::move(*mod));
    }
}

bool ModuleManager::load(const std::string& name) {
    if (isLoaded(name)) return false;

    auto mod = loader_.load(name);
    if (!mod) {
        std::cout << "[ModuleManager] Failed to load module: " << name << "\n";
        return false;
    }

    (*mod)->initialize(EventBus::getInstance());
    EventBus::getInstance().publish({"event:enable", mod->getName()});

    stateStore_.add(name);
    stateStore_.save();

    loaded_.push_back(std::move(*mod));
    return true;
}

bool ModuleManager::unload(const std::string& name) {
    auto it = std::find_if(loaded_.begin(), loaded_.end(),
        [&](const Module& m) { return m.getName() == name; });

    if (it == loaded_.end()) return false;

    EventBus::getInstance().publish({"event:disable", it->getName()});
    (*it)->shutdown();       // shutdown before destructor drops the instance
    loaded_.erase(it);       // ~Module(): instance freed, then handle freed

    stateStore_.remove(name);
    stateStore_.save();

    return true;
}

bool ModuleManager::isLoaded(const std::string& name) const {
    return std::any_of(loaded_.begin(), loaded_.end(),
        [&](const Module& m) { return m.getName() == name; });
}

const std::vector<Module>& ModuleManager::getModules() const {
    return loaded_;
}
