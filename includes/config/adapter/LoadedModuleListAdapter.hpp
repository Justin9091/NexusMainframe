#ifndef NEXUSCORE_LOADEDMODULELISTADAPTER_HPP
#define NEXUSCORE_LOADEDMODULELISTADAPTER_HPP

#include "config/Adapter/IConfigAdapter.hpp"
#include "Modules/ModuleEntry.hpp"
#include <nlohmann/json.hpp>
#include <vector>

/**
 * @brief IConfigAdapter that serialises/deserialises a list of ModuleEntry objects.
 *
 * JSON representation: array of @c {"name":"…","path":"…"} objects.
 */
class LoadedModuleListAdapter : public IConfigAdapter<std::vector<ModuleEntry>> {
public:
    nlohmann::json serialize(const std::vector<ModuleEntry>& entries) const override {
        nlohmann::json arr = nlohmann::json::array();
        for (const auto& e : entries)
            arr.push_back({{"name", e.name}, {"path", e.path}});
        return arr;
    }

    std::vector<ModuleEntry> deserialize(const nlohmann::json& j) const override {
        std::vector<ModuleEntry> result;
        for (const auto& item : j)
            result.push_back({item.at("name").get<std::string>(),
                              item.at("path").get<std::string>()});
        return result;
    }
};

#endif // NEXUSCORE_LOADEDMODULELISTADAPTER_HPP
