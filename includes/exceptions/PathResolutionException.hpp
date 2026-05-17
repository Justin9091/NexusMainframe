//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHRESOLUTIONEXCEPTION_HPP
#define NEXUSCORE_PATHRESOLUTIONEXCEPTION_HPP

#include <stdexcept>
#include <string>

/**
 * @brief Exception thrown when a file-system path cannot be resolved or validated.
 *
 * Thrown by PathManager, PathValidator, and IPathResolver implementations when
 * an environment variable is missing, a path is relative where absolute is
 * required, or a required directory does not exist.
 */
class PathResolutionException : public std::runtime_error {
public:
    /** @param message Human-readable description of the resolution failure. */
    explicit PathResolutionException(const std::string& message)
        : std::runtime_error(message) {}

    /** @param message Human-readable description of the resolution failure. */
    explicit PathResolutionException(const char* message)
        : std::runtime_error(message) {}
};

#endif //NEXUSCORE_PATHRESOLUTIONEXCEPTION_HPP