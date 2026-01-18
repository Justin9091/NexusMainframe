//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHRESOLVERFACTORY_HPP
#define NEXUSCORE_PATHRESOLVERFACTORY_HPP

#include <memory>

#include "IPathResolver.hpp"

class PathResolverFactory {
public:
    static std::unique_ptr<IPathResolver> create();
};

#endif //NEXUSCORE_PATHRESOLVERFACTORY_HPP