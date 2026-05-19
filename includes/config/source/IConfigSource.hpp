//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_ICONFIGSOURCE_HPP
#define NEXUSCORE_ICONFIGSOURCE_HPP

#include <string>
#include <unordered_map>
#include <variant>

#include "nlohmann/json_fwd.hpp"

/**
 * @brief Variant type for a single configuration value.
 *
 * Supports primitive types and arbitrary JSON objects so structured config
 * sections can be stored alongside simple scalar values.
 */
using ConfigValue = std::variant<
    std::string,
    int,
    double,
    bool,
    nlohmann::json
>;

/** @brief Map of configuration key/value pairs loaded from a source. */
using ConfigMap = std::unordered_map<std::string, ConfigValue>;

/**
 * @brief Abstraction for loading configuration from a file.
 *
 * Implementations parse a file format and return a flat ConfigMap.
 * Currently only JsonConfigSource is provided.
 */
class IConfigSource {
public:
    virtual ~IConfigSource() = default;

    /**
     * @brief Reads configuration from a file and returns it as a ConfigMap.
     * @param filepath  Absolute path to the config file.
     * @return Flat map of key → ConfigValue pairs.
     * @throws std::runtime_error if the file cannot be read or parsed.
     */
    virtual ConfigMap load(const std::string& filepath) = 0;
};

#endif //NEXUSCORE_ICONFIGSOURCE_HPP