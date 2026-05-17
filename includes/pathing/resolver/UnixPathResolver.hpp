//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_UNIXPATHRESOLVER_HPP
#define NEXUSCORE_UNIXPATHRESOLVER_HPP

#include "IPathResolver.hpp"

/**
 * @brief Unix IPathResolver that resolves the Nexus base path under $HOME.
 *
 * Returns @c $HOME/.nexus.
 * @throws PathResolutionException if HOME is not set.
 */
class UnixPathResolver : public IPathResolver {
public:
    std::filesystem::path getNexusPath() const override;
};

#endif //NEXUSCORE_UNIXPATHRESOLVER_HPP