//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_JSONCONFIGSOURCE_HPP
#define NEXUSCORE_JSONCONFIGSOURCE_HPP

#include <string>
#include <nlohmann/json.hpp>

#include "IConfigSource.hpp"

/**
 * @brief IConfigSource implementation that reads JSON files.
 *
 * Parses the JSON file at the given path and flattens top-level keys into a
 * ConfigMap.  Nested objects are stored as @c nlohmann::json values in the
 * ConfigValue variant; scalars are mapped to their native C++ types.
 */
class JsonConfigSource : public IConfigSource {
public:
    /**
     * @brief Loads configuration from a JSON file.
     * @param filepath  Absolute path to the .json file.
     * @return ConfigMap with one entry per top-level JSON key.
     * @throws std::runtime_error if the file cannot be opened or is not valid JSON.
     */
    ConfigMap load(const std::string& filepath) override;

private:
    ConfigMap parseJson(const nlohmann::json& j);
};

#endif //NEXUSCORE_JSONCONFIGSOURCE_HPP