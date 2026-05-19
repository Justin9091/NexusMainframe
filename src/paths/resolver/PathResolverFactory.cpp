//
// Created by jusra on 17-1-2026.
//

#include "paths/resolver/PathResolverFactory.hpp"

#ifdef _WIN32
#include "paths/resolver/WindowsPathResolver.hpp"
#else
#include "paths/resolver/UnixPathResolver.hpp"
#endif

std::unique_ptr<IPathResolver> PathResolverFactory::create() {
#ifdef _WIN32
    return std::make_unique<WindowsPathResolver>();
#else
    return std::make_unique<UnixPathResolver>();
#endif
}
