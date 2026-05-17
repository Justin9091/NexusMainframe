#ifndef NEXUSCORE_MODULEENTRY_HPP
#define NEXUSCORE_MODULEENTRY_HPP

#include <string>

// Lightweight descriptor used by the scanner and module registry.
// Not a runtime object — carries no instance or handle.
struct ModuleEntry {
    std::string name;
    std::string path;
};

#endif // NEXUSCORE_MODULEENTRY_HPP
