//
// Created by jusra on 29-12-2025.
//

#include <algorithm>
#include <iostream>
#include <filesystem>

#include "../../includes/Modules/ModuleLoader.hpp"
#include "Modules/operations/ModuleOperationsFactory.hpp"

ModuleLoader::ModuleLoader(std::unique_ptr<IModuleOperations> ops)
    : osOps_(std::move(ops)) {}

std::optional<LoadedModule> ModuleLoader::load(const std::string& libraryPath) {
    void* handle = osOps_->loadLibrary(libraryPath);
    if (!handle) return std::nullopt;

    auto createFn = reinterpret_cast<CreateModuleFn>(
        osOps_->getFunction(handle, CREATE_MODULE_FUNC)
    );

    if (!createFn) {
        osOps_->unloadLibrary(handle);
        return std::nullopt;
    }

    IModule* raw = createFn();
    if (!raw) {
        osOps_->unloadLibrary(handle);
        return std::nullopt;
    }

    std::shared_ptr<IModule> instance(raw);

    return LoadedModule{
        instance->getName(),
        libraryPath,
        instance,
        handle
    };
}
