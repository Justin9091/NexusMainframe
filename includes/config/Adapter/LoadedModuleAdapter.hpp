#ifndef NEXUSCORE_LOADEDMODULEADAPTER_HPP
#define NEXUSCORE_LOADEDMODULEADAPTER_HPP

#include "config/Adapter/IConfigAdapter.hpp"
#include "Modules/ModuleEntry.hpp"
#include <nlohmann/json.hpp>

class LoadedModuleAdapter : public IConfigAdapter<ModuleEntry> {
public:
    nlohmann::json serialize(const ModuleEntry& entry) const override {
        return {{"name", entry.name}, {"path", entry.path}};
    }

    ModuleEntry deserialize(const nlohmann::json& j) const override {
        return {j.value("name", ""), j.value("path", "")};
    }
};

#endif // NEXUSCORE_LOADEDMODULEADAPTER_HPP
