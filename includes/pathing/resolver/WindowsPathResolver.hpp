//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_WINDOWSPATHRESOLVER_HPP
#define NEXUSCORE_WINDOWSPATHRESOLVER_HPP

#include "IPathResolver.hpp"

class WindowsPathResolver : public IPathResolver {
public:
    std::filesystem::path getNexusPath() const override;
};

#endif //NEXUSCORE_WINDOWSPATHRESOLVER_HPP