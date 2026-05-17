//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_IPATHRESOLVER_HPP
#define NEXUSCORE_IPATHRESOLVER_HPP

#include <filesystem>
#include <vector>

/**
 * @brief Platform abstraction for resolving the Nexus base directory.
 *
 * Returns the root directory under which all Nexus data is stored
 * (e.g. @c C:\\Users\\<user>\\.nexus on Windows, @c ~/.nexus on Linux).
 * Use PathResolverFactory::create() to obtain the correct implementation.
 */
class IPathResolver {
public:
    virtual ~IPathResolver() = default;

    /**
     * @brief Returns the absolute path to the Nexus base directory.
     * @throws PathResolutionException if the path cannot be determined.
     */
    virtual std::filesystem::path getNexusPath() const = 0;
};

#endif //NEXUSCORE_IPATHRESOLVER_HPP