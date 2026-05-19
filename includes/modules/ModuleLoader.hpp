#ifndef MODULELOADER_HPP
#define MODULELOADER_HPP

#include <memory>
#include <optional>
#include <string>

#include "Module.hpp"
#include "operations/IModuleOperations.hpp"

/**
 * @brief Loads a single module library and constructs a Module from it.
 *
 * Delegates OS-level dlopen/LoadLibrary calls to IModuleOperations, then
 * resolves the mandatory @c createModule() factory symbol inside the library.
 *
 * @code
 * auto ops = ModuleOperationsFactory::create();
 * ModuleLoader loader(ops);
 * auto module = loader.load("my-sensor");
 * if (module) {
 *     module->initialize(bus);
 * }
 * @endcode
 */
class ModuleLoader {
public:
    using CreateModuleFn = IModule* (*)(); ///< Signature of the module factory symbol.

    /// Name of the factory function that every module library must export.
    static constexpr const char* CREATE_MODULE_FUNC = "createModule";

    /**
     * @param ops Platform-specific operations used for library loading.
     */
    explicit ModuleLoader(std::shared_ptr<IModuleOperations> ops);

    /**
     * @brief Loads a module library by name.
     *
     * Constructs the library path from the Nexus modules directory, opens the
     * library, resolves @c createModule, and calls it to obtain the IModule*.
     *
     * @param name  Module name without extension or directory prefix.
     * @return A Module on success, or std::nullopt if loading fails.
     */
    std::optional<Module> load(const std::string& name);

private:
    std::shared_ptr<IModuleOperations> osOps_;
};

#endif
