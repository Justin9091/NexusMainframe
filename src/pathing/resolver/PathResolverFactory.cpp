//
// Created by jusra on 17-1-2026.
//

#include "pathing/resolver/PathResolverFactory.hpp"

#ifdef _WIN32
#include "pathing/resolver/WindowsPathResolver.hpp"
#else
#include "pathing/resolver/UnixPathResolver.hpp"
#endif

std::unique_ptr<IPathResolver> PathResolverFactory::create() {
#ifdef _WIN32
    return std::make_unique<WindowsPathResolver>();
#else
    return std::make_unique<UnixPathResolver>();
#endif
}
