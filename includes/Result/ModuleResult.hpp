#pragma once
#include "Result/Result.hpp"

class ModuleResult : public Result {
public:
    static ModuleResult alreadyLoaded(const std::string& name);
    static ModuleResult notLoaded    (const std::string& name);

private:
    using Result::Result;
};
