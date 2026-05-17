#pragma once
#include "Result/Result.hpp"

/**
 * @brief Result specialisation with module-specific factory methods.
 *
 * Extends Result with named constructors whose messages include the module
 * name, making log output and HTTP responses more descriptive.
 */
class ModuleResult : public Result {
public:
    /**
     * @brief Returns a 409 Conflict result indicating the module is already loaded.
     * @param name Module name included in the error message.
     */
    static ModuleResult alreadyLoaded(const std::string& name);

    /**
     * @brief Returns a 404 Not Found result indicating the module is not loaded.
     * @param name Module name included in the error message.
     */
    static ModuleResult notLoaded(const std::string& name);

private:
    using Result::Result;
};
