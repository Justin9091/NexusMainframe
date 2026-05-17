//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHVALIDATOR_HPP
#define NEXUSCORE_PATHVALIDATOR_HPP

#include <filesystem>

#include "exceptions/PathResolutionException.hpp"

/**
 * @brief Static utility methods for validating file-system paths.
 *
 * All methods throw PathResolutionException on failure so callers can use a
 * single catch clause for path-related errors.
 */
class PathValidator {
public:
    /**
     * @brief Validates that an environment variable value is set and non-empty.
     * @param value  Pointer as returned by std::getenv() (may be null).
     * @param name   Variable name, used in the error message.
     * @throws PathResolutionException if @p value is null or empty.
     */
    static void validateEnvironmentVariable(const char* value, const std::string& name) {
        if (!value) {
            throw PathResolutionException(name + " environment variable is not set");
        }

        std::string strValue(value);
        if (strValue.empty()) {
            throw PathResolutionException(name + " environment variable is empty");
        }
    }

    /**
     * @brief Validates that a path is absolute.
     * @param path     Path to check.
     * @param context  Description used in the error message.
     * @throws PathResolutionException if @p path is relative.
     */
    static void validateAbsolutePath(const std::filesystem::path& path, const std::string& context) {
        if (!path.is_absolute()) {
            throw PathResolutionException(
                context + " must be an absolute path, got: " + path.string()
            );
        }
    }

    /**
     * @brief Validates that a path exists on the filesystem.
     * @param path     Path to check.
     * @param context  Description used in the error message.
     * @throws PathResolutionException if @p path does not exist.
     */
    static void validatePathExists(const std::filesystem::path& path, const std::string& context) {
        if (!std::filesystem::exists(path)) {
            throw PathResolutionException(
                context + " does not exist: " + path.string()
            );
        }
    }

    /**
     * @brief Validates that a path refers to a directory.
     * @param path     Path to check.
     * @param context  Description used in the error message.
     * @throws PathResolutionException if @p path is not a directory.
     */
    static void validateIsDirectory(const std::filesystem::path& path, const std::string& context) {
        if (!std::filesystem::is_directory(path)) {
            throw PathResolutionException(
                context + " is not a directory: " + path.string()
            );
        }
    }
};

#endif //NEXUSCORE_PATHVALIDATOR_HPP