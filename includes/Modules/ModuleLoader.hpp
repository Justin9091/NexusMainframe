#ifndef MODULELOADER_HPP
#define MODULELOADER_HPP

#include "IModule.hpp"
#include <vector>
#include <map>
#include <memory>
#include <string>

#include "LoadedModule.hpp"
#include "operations/IModuleOperations.hpp"

class ModuleLoader {
public:
    using CreateModuleFn = IModule* (*)();
    static constexpr const char* CREATE_MODULE_FUNC = "createModule";

    explicit ModuleLoader(std::unique_ptr<IModuleOperations> ops);

    std::optional<LoadedModule> load(const std::string& libraryPath);

private:
    std::unique_ptr<IModuleOperations> osOps_;
};


#endif