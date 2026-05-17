//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_MODULESCANNERFACTORY_HPP
#define NEXUSCORE_MODULESCANNERFACTORY_HPP

#include "IModuleScanner.hpp"
#include <memory>

#ifdef _WIN32
    #include "WindowsModuleScanner.hpp"
#else
    #include "LinuxModuleScanner.hpp"
#endif

/**
 * @brief Factory that creates the platform-appropriate IModuleScanner.
 *
 * Returns WindowsModuleScanner on Windows and LinuxModuleScanner elsewhere.
 */
class ModuleScannerFactory {
public:
    /**
     * @brief Creates the platform-specific IModuleScanner implementation.
     * @return Owning pointer to the scanner.
     */
    static std::unique_ptr<IModuleScanner> create() {
#ifdef _WIN32
        return std::make_unique<WindowsModuleScanner>();
#else
        return std::make_unique<LinuxModuleScanner>();
#endif
    }
};

#endif //NEXUSCORE_MODULESCANNERFACTORY_HPP