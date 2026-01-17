//
// Created by jusra on 16-1-2026.
//

#ifndef NEXUSCORE_LOADEDMODULEADAPTER_HPP
#define NEXUSCORE_LOADEDMODULEADAPTER_HPP

#include "config/Adapter/IConfigAdapter.hpp"
#include "Modules/LoadedModule.hpp"
#include <nlohmann/json.hpp>

class LoadedModuleAdapter : public IConfigAdapter<LoadedModule> {
public:
    // Struct → JSON
    nlohmann::json serialize(const LoadedModule& module) const override {
        return {
                { "name", module.name },
                { "path", module.path }
        };
    }

    // JSON → Struct
    LoadedModule deserialize(const nlohmann::json& j) const override {
        return LoadedModule{
            .name     = j.value("name", ""),
            .path     = j.value("path", ""),
            .instance = nullptr, // runtime-only
            .handle   = nullptr  // runtime-only
        };
    }
};

#endif // NEXUSCORE_LOADEDMODULEADAPTER_HPP
