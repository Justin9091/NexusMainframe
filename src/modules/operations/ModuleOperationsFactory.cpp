//
// Created by jusra on 15-1-2026.
//

#include "modules/operations/ModuleOperationsFactory.hpp"


#ifdef _WIN32
    #include "modules/operations/WindowsModuleOperations.hpp"
#else
    #include "modules/operations/LinuxModuleOperations.hpp"
#endif

std::shared_ptr<IModuleOperations> ModuleOperationsFactory::create() {
#ifdef _WIN32
        return std::make_unique<WindowsModuleOperations>();
#else
        return std::make_unique<LinuxModuleOperations>();
#endif
};
