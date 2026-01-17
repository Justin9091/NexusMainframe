//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_IPATHRESOLVER_HPP
#define NEXUSCORE_IPATHRESOLVER_HPP

#include <filesystem>
#include <vector>

class IPathResolver {
public:
    virtual ~IPathResolver() = default;
    virtual std::filesystem::path getNexusPath() const = 0;
};

#endif //NEXUSCORE_IPATHRESOLVER_HPP