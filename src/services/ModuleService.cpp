#include "services/ModuleService.hpp"
#include "Result/ModuleResult.hpp"

void ModuleService::start() {
    _manager = std::make_unique<ModuleManager>();
    _manager->loadFromState();
}

void ModuleService::stop() {
    if (!_manager) return;
    for (auto& m : _manager->getModules())
        m->shutdown();
}

std::vector<ModuleService::ModuleInfo> ModuleService::list() const {
    std::vector<ModuleInfo> result;
    for (const auto& m : _manager->getModules())
        result.push_back({m.getName(), m.getPath()});
    return result;
}

bool ModuleService::isLoaded(const std::string& name) const {
    return _manager->isLoaded(name);
}

Result ModuleService::enable(const std::string& name) {
    if (_manager->isLoaded(name))
        return ModuleResult::alreadyLoaded(name);
    return _manager->load(name)
        ? Result::ok("Module '" + name + "' enabled")
        : Result::notFound("Failed to load module '" + name + "'");
}

Result ModuleService::disable(const std::string& name) {
    if (!_manager->isLoaded(name))
        return ModuleResult::notLoaded(name);
    return _manager->unload(name)
        ? Result::ok("Module '" + name + "' disabled")
        : Result::failed("Failed to disable module '" + name + "'");
}
