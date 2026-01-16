//
// Created by jusra on 15-1-2026.
//

#include "Modules/operations/ModuleOperationsFactory.hpp"


#ifdef _WIN32
    #include "Modules/operations/WindowsModuleOperations.hpp"
#else
    #include "Modules/operations/LinuxModuleOperations.hpp"
#endif

std::unique_ptr<IModuleOperations> ModuleOperationsFactory::create() {
#ifdef _WIN32
        return std::make_unique<WindowsModuleOperations>();
#else
        return std::make_unique<LinuxModuleOperations>();
#endif
};
