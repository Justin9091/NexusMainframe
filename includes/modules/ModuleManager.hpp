#ifndef NEXUSCORE_MODULEMANAGER_HPP
#define NEXUSCORE_MODULEMANAGER_HPP

#include <vector>

#include "Module.hpp"
#include "ModuleLoader.hpp"
#include "ModuleStateStore.hpp"

/**
 * @brief Manages the runtime lifecycle of dynamically loaded Nexus modules.
 *
 * Modules are platform-specific shared libraries (.dll / .so) that implement
 * IModule.  ModuleManager coordinates loading through ModuleLoader and persists
 * the set of active modules via ModuleStateStore so they survive restarts.
 *
 * @code
 * ModuleManager mgr;
 * mgr.loadFromState();           // restore previously enabled modules
 * mgr.load("my-sensor-module");  // load an additional module by name
 * for (auto& m : mgr.getModules())
 *     m->initialize(EventBus::getInstance());
 * @endcode
 */
class ModuleManager {
public:
    ModuleManager();

    /**
     * @brief Scans a directory and loads every valid module found there.
     * @param path Absolute path to the directory containing .dll / .so files.
     */
    void loadFromDirectory(const std::string& path);

    /**
     * @brief Loads all modules that were previously persisted by ModuleStateStore.
     * Call on start-up to restore the last known enabled set.
     */
    void loadFromState();

    /**
     * @brief Loads a single module by name.
     * @param name Module name without extension or path prefix.
     * @return @c true if the module was found and loaded successfully.
     * @return @c false if the module is already loaded or cannot be found.
     */
    bool load(const std::string& name);

    /**
     * @brief Unloads a currently loaded module by name.
     * @param name Module name as returned by IModule::getName().
     * @return @c true if the module was found and unloaded.
     * @return @c false if the module was not loaded.
     */
    bool unload(const std::string& name);

    /**
     * @brief Checks whether a module is currently loaded.
     * @param name Module name to look up.
     * @return @c true if loaded, @c false otherwise.
     */
    bool isLoaded(const std::string& name) const;

    /** @brief Returns a read-only view of all currently loaded modules. */
    const std::vector<Module>& getModules() const;

private:
    ModuleLoader loader_;
    std::vector<Module> loaded_;
    ModuleStateStore stateStore_;
};

#endif // NEXUSCORE_MODULEMANAGER_HPP
