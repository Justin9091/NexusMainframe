//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_MODULEUNLOADER_HPP
#define NEXUSCORE_MODULEUNLOADER_HPP
#include <memory>

#include "LoadedModule.hpp"
#include "operations/IModuleOperations.hpp"

class ModuleUnloader {
public:
    explicit ModuleUnloader(std::unique_ptr<IModuleOperations> ops);

    bool unload(const LoadedModule& module);

private:
    std::unique_ptr<IModuleOperations> osOps_;
};


#endif //NEXUSCORE_MODULEUNLOADER_HPP