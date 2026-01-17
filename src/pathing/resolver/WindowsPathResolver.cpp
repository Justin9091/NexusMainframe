//
// Created by jusra on 17-1-2026.
//

#include "pathing/resolver/WindowsPathResolver.hpp"

#include "pathing/PathValidator.hpp"

std::filesystem::path WindowsPathResolver::getNexusPath() const {
    const char* userProfile = std::getenv("USERPROFILE");
    PathValidator::validateEnvironmentVariable(userProfile, "USERPROFILE");

    std::filesystem::path basePath(userProfile);
    PathValidator::validateAbsolutePath(basePath, "USERPROFILE");

    return basePath / ".nexus";
}
