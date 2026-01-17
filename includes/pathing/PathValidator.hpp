//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHVALIDATOR_HPP
#define NEXUSCORE_PATHVALIDATOR_HPP

#include <filesystem>

#include "exceptions/PathResolutionException.hpp"

class PathValidator {
public:
    // Valideer environment variable
    static void validateEnvironmentVariable(const char* value, const std::string& name) {
        if (!value) {
            throw PathResolutionException(name + " environment variable is not set");
        }

        std::string strValue(value);
        if (strValue.empty()) {
            throw PathResolutionException(name + " environment variable is empty");
        }
    }

    // Valideer dat path absoluut is
    static void validateAbsolutePath(const std::filesystem::path& path, const std::string& context) {
        if (!path.is_absolute()) {
            throw PathResolutionException(
                context + " must be an absolute path, got: " + path.string()
            );
        }
    }

    // Valideer dat path bestaat (optioneel, voor runtime checks)
    static void validatePathExists(const std::filesystem::path& path, const std::string& context) {
        if (!std::filesystem::exists(path)) {
            throw PathResolutionException(
                context + " does not exist: " + path.string()
            );
        }
    }

    // Valideer dat het een directory is
    static void validateIsDirectory(const std::filesystem::path& path, const std::string& context) {
        if (!std::filesystem::is_directory(path)) {
            throw PathResolutionException(
                context + " is not a directory: " + path.string()
            );
        }
    }
};

#endif //NEXUSCORE_PATHVALIDATOR_HPP