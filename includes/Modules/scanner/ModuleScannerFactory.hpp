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

class ModuleScannerFactory {
public:
    static std::unique_ptr<IModuleScanner> create() {
#ifdef _WIN32
        return std::make_unique<WindowsModuleScanner>();
#else
        return std::make_unique<LinuxModuleScanner>();
#endif
    }
};

#endif //NEXUSCORE_MODULESCANNERFACTORY_HPP