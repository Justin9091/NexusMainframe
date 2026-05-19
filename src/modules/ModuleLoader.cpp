
#include "modules/ModuleLoader.hpp"
#include "modules/operations/ModuleOperationsFactory.hpp"
#include "paths/PathManager.hpp"

#include <iostream>

ModuleLoader::ModuleLoader(std::shared_ptr<IModuleOperations> ops)
    : osOps_(std::move(ops)) {}

std::optional<Module> ModuleLoader::load(const std::string& name) {
    const std::string fullPath =
        (PathManager::getInstance().get("modules.downloaded") /
         (name + osOps_->getLibraryExtension())).string();

    void* handle = osOps_->loadLibrary(fullPath);
    if (!handle) {
        std::cout << "[ModuleLoader] loadLibrary failed for: " << fullPath
                  << " — " << osOps_->getLastError() << "\n";
        return std::nullopt;
    }


    auto createFn = reinterpret_cast<CreateModuleFn>(
        osOps_->getFunction(handle, CREATE_MODULE_FUNC)
    );

    if (!createFn) {
        std::cout << "[ModuleLoader] createModule symbol not found in: " << fullPath << "\n";
        osOps_->unloadLibrary(handle);
        return std::nullopt;
    }

    IModule* raw = createFn();
    if (!raw) {
        std::cout << "[ModuleLoader] createModule() returned nullptr for: " << fullPath << "\n";
        osOps_->unloadLibrary(handle);
        return std::nullopt;
    }

    std::shared_ptr<IModule> instance(raw);

    auto ops = osOps_;
    auto handleFree = [handle, ops]() {
        ops->unloadLibrary(handle);
    };

    return Module{instance->getName(), name, std::move(instance), std::move(handleFree)};
}
