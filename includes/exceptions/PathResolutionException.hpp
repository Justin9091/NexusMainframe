//
// Created by jusra on 17-1-2026.
//

#ifndef NEXUSCORE_PATHRESOLUTIONEXCEPTION_HPP
#define NEXUSCORE_PATHRESOLUTIONEXCEPTION_HPP

#include <stdexcept>
#include <string>

class PathResolutionException : public std::runtime_error {
public:
    explicit PathResolutionException(const std::string& message)
        : std::runtime_error(message) {}

    explicit PathResolutionException(const char* message)
        : std::runtime_error(message) {}
};

#endif //NEXUSCORE_PATHRESOLUTIONEXCEPTION_HPP