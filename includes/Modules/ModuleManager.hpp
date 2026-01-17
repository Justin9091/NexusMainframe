//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_MODULEMANAGER_HPP
#define NEXUSCORE_MODULEMANAGER_HPP

#include "LoadedModule.hpp"
#include "ModuleLoader.hpp"
#include "ModuleUnloader.hpp"

class ModuleManager {
public:
    ModuleManager();

    void loadFromDirectory(const std::string& path);
    bool unload(const std::string& name);
    bool isLoaded(const std::string& name) const;

    const std::vector<LoadedModule>& getModules() const;

    bool load(const std::string &name);

private:
    ModuleLoader loader_;
    ModuleUnloader unloader_;
    std::vector<LoadedModule> loaded_;
};


#endif //NEXUSCORE_MODULEMANAGER_HPP