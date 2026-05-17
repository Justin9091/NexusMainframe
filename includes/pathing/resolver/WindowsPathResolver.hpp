//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_WINDOWSPATHRESOLVER_HPP
#define NEXUSCORE_WINDOWSPATHRESOLVER_HPP

#include "IPathResolver.hpp"

/**
 * @brief Windows IPathResolver that resolves the Nexus base path under %USERPROFILE%.
 *
 * Returns @c %USERPROFILE%\\.nexus (e.g. @c C:\\Users\\jusra\\.nexus).
 * @throws PathResolutionException if USERPROFILE is not set.
 */
class WindowsPathResolver : public IPathResolver {
public:
    std::filesystem::path getNexusPath() const override;
};

#endif //NEXUSCORE_WINDOWSPATHRESOLVER_HPP