#pragma once
#include <memory>
#include <string>
#include <vector>
#include "IService.hpp"
#include "modules/ModuleManager.hpp"
#include "result/Result.hpp"

/**
 * @brief IService facade over ModuleManager, exposing module lifecycle via the service API.
 *
 * Owns the ModuleManager and bridges it to the REST API (via ModuleController)
 * and the HTTP health/metrics endpoints.
 */
class ModuleService : public IService {
public:
    /** @brief Lightweight descriptor returned by list(). */
    struct ModuleInfo {
        std::string name; ///< Module name.
        std::string path; ///< Absolute path to the library file.
    };

    std::string_view getName() const override { return "ModuleService"; }

    /** @brief Initialises the ModuleManager and restores previously enabled modules. */
    void start() override;

    /** @brief Shuts down all loaded modules. */
    void stop() override;

    /** @brief Returns descriptors for all currently loaded modules. */
    std::vector<ModuleInfo> list() const;

    /**
     * @brief Checks whether a module is currently loaded.
     * @param name Module name.
     * @return @c true if loaded.
     */
    bool isLoaded(const std::string& name) const;

    /**
     * @brief Loads and starts a module by name.
     * @param name Module name (without path or extension).
     * @return Result::ok() on success; Result::conflict() if already loaded;
     *         Result::failed() if loading fails.
     */
    Result enable(const std::string& name);

    /**
     * @brief Unloads a module by name.
     * @param name Module name.
     * @return Result::ok() on success; Result::notFound() if not loaded.
     */
    Result disable(const std::string& name);

    /** @brief Direct access to the underlying ModuleManager (for health/metrics). */
    ModuleManager& getManager() { return *_manager; }

private:
    std::unique_ptr<ModuleManager> _manager;
};
