#include "result/ModuleResult.hpp"

ModuleResult ModuleResult::alreadyLoaded(const std::string& name) {
    return {409, "Module '" + name + "' is already loaded"};
}

ModuleResult ModuleResult::notLoaded(const std::string& name) {
    return {409, "Module '" + name + "' is not loaded"};
}
