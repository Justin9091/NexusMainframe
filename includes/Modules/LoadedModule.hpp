//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_LOADEDMODULE_HPP
#define NEXUSCORE_LOADEDMODULE_HPP

#include <string>
#include <memory>
#include "Modules/IModule.hpp"

struct LoadedModule {
    std::string name;
    std::shared_ptr<IModule> instance;
    void* handle;
};

#endif //NEXUSCORE_LOADEDMODULE_HPP