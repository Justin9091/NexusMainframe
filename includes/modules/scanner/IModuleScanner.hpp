//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_IMODULESCANNER_HPP
#define NEXUSCORE_IMODULESCANNER_HPP

#include <string>
#include <vector>

/**
 * @brief Metadata about a module candidate found during a directory scan.
 */
struct ModuleInfo {
    std::string name;      ///< Derived module name (filename without extension).
    std::string filename;  ///< Bare filename including extension.
    std::string path;      ///< Absolute path to the library file.
    bool        isValid;   ///< @c true if the file is a recognised module library.
};

/**
 * @brief Platform abstraction for discovering module libraries in a directory.
 *
 * Concrete implementations: WindowsModuleScanner, LinuxModuleScanner.
 * Use ModuleScannerFactory::create() to obtain the correct instance.
 */
class IModuleScanner {
public:
    virtual ~IModuleScanner() = default;

    /**
     * @brief Scans a directory for module library files.
     * @param directory  Absolute path to the directory to search.
     * @return List of ModuleInfo descriptors for every file found.
     */
    virtual std::vector<ModuleInfo> scanDirectory(const std::string& directory) = 0;

    /**
     * @brief Checks whether a module file exists at the given path.
     * @param modulePath  Absolute path to the .dll / .so file.
     * @return @c true if the file exists and is accessible.
     */
    virtual bool moduleExists(const std::string& modulePath) = 0;
};

#endif
