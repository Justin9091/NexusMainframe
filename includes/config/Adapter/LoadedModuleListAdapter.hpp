//
// Created by jusra on 16-1-2026.
//

#ifndef NEXUSCORE_LOADEDMODULELISTADAPTER_HPP
#define NEXUSCORE_LOADEDMODULELISTADAPTER_HPP

#include "config/Adapter/IConfigAdapter.hpp"
#include "Modules/LoadedModule.hpp"

#include <nlohmann/json.hpp>
#include <vector>

class LoadedModuleListAdapter
    : public IConfigAdapter<std::vector<LoadedModule>> {
public:
    nlohmann::json serialize(
        const std::vector<LoadedModule>& modules) const override {

        nlohmann::json arr = nlohmann::json::array();
        for (const auto& m : modules) {
            arr.push_back({
                {"name", m.name},
                {"path", m.path}
            });
        }
        return arr;
    }

    std::vector<LoadedModule> deserialize(
        const nlohmann::json& j) const override {

        std::vector<LoadedModule> result;
        for (const auto& item : j) {
            result.push_back({
                item.at("name").get<std::string>(),
                item.at("path").get<std::string>(),
                nullptr,
                nullptr
            });
        }
        return result;
    }
};


#endif //NEXUSCORE_LOADEDMODULELISTADAPTER_HPP