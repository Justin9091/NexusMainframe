#ifndef NEXUSCORE_MODULEMANAGER_HPP
#define NEXUSCORE_MODULEMANAGER_HPP

#include <vector>

#include "Module.hpp"
#include "ModuleLoader.hpp"
#include "ModuleStateStore.hpp"

class ModuleManager {
public:
    ModuleManager();

    void loadFromDirectory(const std::string& path);
    void loadFromState();
    bool load(const std::string& name);
    bool unload(const std::string& name);
    bool isLoaded(const std::string& name) const;

    const std::vector<Module>& getModules() const;

private:
    ModuleLoader loader_;
    std::vector<Module> loaded_;
    ModuleStateStore stateStore_;
};

#endif // NEXUSCORE_MODULEMANAGER_HPP
