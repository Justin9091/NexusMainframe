//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHRESOLVERFACTORY_HPP
#define NEXUSCORE_PATHRESOLVERFACTORY_HPP

#include <memory>

#include "IPathResolver.hpp"

/**
 * @brief Factory that creates the platform-appropriate IPathResolver.
 *
 * Returns WindowsPathResolver on Windows and UnixPathResolver elsewhere.
 */
class PathResolverFactory {
public:
    /**
     * @brief Creates the platform-specific IPathResolver implementation.
     * @return Owning pointer to the resolver.
     */
    static std::unique_ptr<IPathResolver> create();
};

#endif //NEXUSCORE_PATHRESOLVERFACTORY_HPP