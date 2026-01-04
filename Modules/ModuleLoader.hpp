#ifndef MODULELOADER_HPP
#define MODULELOADER_HPP

#include <memory>
#include <vector>
#include <string>
#include <map>

#include "IModule.hpp"

class ModuleLoader {
private:
    std::vector<std::shared_ptr<IModule>> modules_;
    std::map<std::string, void*> moduleHandles_;

public:
    using CreateModuleFn = IModule* (*)();
    static constexpr const char* CREATE_MODULE_FUNC = "createModule";

    ModuleLoader() = default;
    ~ModuleLoader();

    std::shared_ptr<IModule> loadModule(const std::string& libraryPath);
    std::vector<std::shared_ptr<IModule>> loadModulesFromDirectory(const std::string& directory);
    bool unloadModule(const std::string& moduleName);
    const std::vector<std::shared_ptr<IModule>>& getLoadedModules() const;
};

#endif