//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_UNIXPATHRESOLVER_HPP
#define NEXUSCORE_UNIXPATHRESOLVER_HPP

#include "IPathResolver.hpp"

class UnixPathResolver : public IPathResolver {
public:
    std::filesystem::path getNexusPath() const override;
};

#endif //NEXUSCORE_UNIXPATHRESOLVER_HPP