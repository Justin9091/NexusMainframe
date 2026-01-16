//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_MODULEOPERATIONSFACTORY_HPP
#define NEXUSCORE_MODULEOPERATIONSFACTORY_HPP

#include "IModuleOperations.hpp"
#include <memory>

class ModuleOperationsFactory {
public:
    static std::unique_ptr<IModuleOperations> create();
};

#endif //NEXUSCORE_MODULEOPERATIONSFACTORY_HPP