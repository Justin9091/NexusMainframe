//
// Created by jusra on 4-1-2026.
//
#include "../../../includes/config/Source/JsonConfigSource.hpp"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

ConfigMap JsonConfigSource::load(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }

    nlohmann::json j;
    file >> j;

    return parseJson(j);
}

ConfigMap JsonConfigSource::parseJson(const nlohmann::json& j) {
    ConfigMap result;

    for (const auto& [key, value] : j.items()) {
        if (value.is_string()) {
            result[key] = value.get<std::string>();
        }
        else if (value.is_number_integer()) {
            result[key] = value.get<int>();
        }
        else if (value.is_number_float()) {
            result[key] = value.get<double>();
        }
        else if (value.is_boolean()) {
            result[key] = value.get<bool>();
        }
    }

    return result;
}
