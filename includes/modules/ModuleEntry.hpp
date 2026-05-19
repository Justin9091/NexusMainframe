#ifndef NEXUSCORE_MODULEENTRY_HPP
#define NEXUSCORE_MODULEENTRY_HPP

#include <string>

/**
 * @brief Lightweight descriptor for a discoverable module.
 *
 * Produced by IModuleScanner and used by the module registry.  Carries only
 * the name and filesystem path — no live instance or OS handle.
 */
struct ModuleEntry {
    std::string name; ///< Module name without extension (e.g. "my-sensor").
    std::string path; ///< Absolute path to the .dll / .so file.
};

#endif // NEXUSCORE_MODULEENTRY_HPP
