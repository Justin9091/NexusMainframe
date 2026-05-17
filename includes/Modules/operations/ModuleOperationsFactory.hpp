//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_MODULEOPERATIONSFACTORY_HPP
#define NEXUSCORE_MODULEOPERATIONSFACTORY_HPP

#include "IModuleOperations.hpp"
#include <memory>

/**
 * @brief Factory that creates the platform-appropriate IModuleOperations.
 *
 * Returns WindowsModuleOperations on Windows and LinuxModuleOperations elsewhere.
 */
class ModuleOperationsFactory {
public:
    /**
     * @brief Creates the platform-specific IModuleOperations implementation.
     * @return Shared pointer to the operations implementation.
     */
    static std::shared_ptr<IModuleOperations> create();
};

#endif //NEXUSCORE_MODULEOPERATIONSFACTORY_HPP