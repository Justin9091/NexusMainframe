#ifndef MODULELOADER_HPP
#define MODULELOADER_HPP

#include <memory>
#include <optional>
#include <string>

#include "Module.hpp"
#include "operations/IModuleOperations.hpp"

class ModuleLoader {
public:
    using CreateModuleFn = IModule* (*)();
    static constexpr const char* CREATE_MODULE_FUNC = "createModule";

    explicit ModuleLoader(std::shared_ptr<IModuleOperations> ops);

    std::optional<Module> load(const std::string& name);

private:
    std::shared_ptr<IModuleOperations> osOps_;
};

#endif
