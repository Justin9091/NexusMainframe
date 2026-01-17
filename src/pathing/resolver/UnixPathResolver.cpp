//
// Created by jusra on 17-1-2026.
//

#include "pathing/resolver/UnixPathResolver.hpp"

#include "pathing/PathValidator.hpp"


std::filesystem::path UnixPathResolver::getNexusPath() const {
    const char* home = std::getenv("HOME");
    PathValidator::validateEnvironmentVariable(home, "HOME");

    std::filesystem::path basePath(home);
    PathValidator::validateAbsolutePath(basePath, "HOME");

    return basePath / ".nexus";
}
