//
// Created by jusra on 15-1-2026.
//

#ifndef NEXUSCORE_IMODULEOPERATIONS_HPP
#define NEXUSCORE_IMODULEOPERATIONS_HPP

#include <string>

/**
 * @brief Platform abstraction for dynamic library operations.
 *
 * Decouples ModuleLoader from OS-specific APIs (LoadLibrary/dlopen).
 * Concrete implementations: WindowsModuleOperations, LinuxModuleOperations.
 * Use ModuleOperationsFactory::create() to obtain the correct instance.
 */
class IModuleOperations {
public:
    virtual ~IModuleOperations() = default;

    /**
     * @brief Opens a shared library file.
     * @param libraryPath  Absolute path to the .dll / .so file.
     * @return Opaque library handle, or @c nullptr on failure.
     */
    virtual void* loadLibrary(const std::string& libraryPath) = 0;

    /**
     * @brief Closes a previously opened library.
     * @param handle  Handle returned by loadLibrary().
     * @return @c true on success.
     */
    virtual bool unloadLibrary(void* handle) = 0;

    /**
     * @brief Resolves an exported symbol from a loaded library.
     * @param handle        Handle returned by loadLibrary().
     * @param functionName  Exported symbol name.
     * @return Function pointer, or @c nullptr if the symbol was not found.
     */
    virtual void* getFunction(void* handle, const std::string& functionName) = 0;

    /** @brief Returns a human-readable description of the last OS error. */
    virtual std::string getLastError() const = 0;

    /** @brief Returns the file extension used for modules on this platform (e.g. ".dll"). */
    virtual std::string getLibraryExtension() const = 0;
};
#endif //NEXUSCORE_IMODULEOPERATIONS_HPP