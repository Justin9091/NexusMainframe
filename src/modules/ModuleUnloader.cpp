//
// Created by jusra on 15-1-2026.
//

#include "Modules/ModuleUnloader.hpp"

ModuleUnloader::ModuleUnloader(std::unique_ptr<IModuleOperations> ops)
    : osOps_(std::move(ops)) {}

bool ModuleUnloader::unload(const LoadedModule& module) {
    return osOps_->unloadLibrary(module.handle);
}
