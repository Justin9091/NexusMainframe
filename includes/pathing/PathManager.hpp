//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHMANAGER_HPP
#define NEXUSCORE_PATHMANAGER_HPP

#include <filesystem>
#include <string>
#include <unordered_map>
#include <functional>
#include <memory>
#include "resolver/IPathResolver.hpp"

/**
 * @brief Singleton registry for all file-system paths used by the Nexus framework.
 *
 * PathManager resolves a platform-specific base directory (e.g.
 * @c C:\\Users\\<user>\\.nexus on Windows, @c ~/.nexus on Linux) via an
 * IPathResolver, then lets callers register named sub-paths either as
 * relative paths or as lazy resolver lambdas.
 *
 * Core paths are pre-registered in registerCorePaths() (e.g. "modules",
 * "config", "logs").
 *
 * @code
 * auto& pm = PathManager::getInstance();
 * pm.registerPath("plugins", "plugins/extra");
 * auto pluginDir = pm.get("plugins");
 * pm.ensureExists("plugins"); // creates the directory if missing
 * @endcode
 */
class PathManager {
public:
    /// Returns the process-wide singleton instance.
    static PathManager& getInstance();

    /**
     * @brief Resolves a registered path by key.
     * @param pathKey  Name under which the path was registered.
     * @return Absolute filesystem path.
     * @throws PathResolutionException if @p pathKey is not registered.
     */
    std::filesystem::path get(const std::string& pathKey) const;

    /**
     * @brief Registers a named path as a relative sub-path of the base.
     * @param key           Lookup name for the path.
     * @param relativePath  Path relative to the Nexus base directory.
     */
    void registerPath(const std::string& key,
                      const std::filesystem::path& relativePath);

    /**
     * @brief Registers a named path via a lazy resolver lambda.
     * @param key       Lookup name for the path.
     * @param resolver  Callable that returns the absolute path when invoked.
     */
    void registerPath(const std::string& key,
                      std::function<std::filesystem::path()> resolver);

    /**
     * @brief Creates the directory for a registered path if it does not exist.
     * @param pathKey Registered path key.
     * @throws PathResolutionException if the path cannot be created.
     */
    void ensureExists(const std::string& pathKey) const;

    /**
     * @brief Checks whether the directory for a registered path exists on disk.
     * @param pathKey Registered path key.
     * @return @c true if the path exists, @c false otherwise.
     */
    bool exists(const std::string& pathKey) const;

    /** @brief Returns the platform-specific Nexus base directory. */
    std::filesystem::path getBase() const { return _basePath; }

    /**
     * @brief Returns an absolute path by appending @p relativePath to the base.
     * @param relativePath Path relative to the Nexus base directory.
     */
    std::filesystem::path getCustom(const std::string& relativePath) const;

    PathManager(const PathManager&)            = delete;
    PathManager& operator=(const PathManager&) = delete;

private:
    PathManager();

    std::unique_ptr<IPathResolver> _resolver;
    std::filesystem::path _basePath;
    std::unordered_map<std::string, std::function<std::filesystem::path()>> _pathResolvers;

    void initialize();
    void registerCorePaths();
};

#endif //NEXUSCORE_PATHMANAGER_HPP