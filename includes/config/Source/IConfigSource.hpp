//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_ICONFIGSOURCE_HPP
#define NEXUSCORE_ICONFIGSOURCE_HPP

#include <string>
#include <unordered_map>
#include <variant>

#include "nlohmann/json_fwd.hpp"

using ConfigValue = std::variant<
    std::string,
    int,
    double,
    bool,
    nlohmann::json
>;

using ConfigMap = std::unordered_map<std::string, ConfigValue>;

class IConfigSource {
public:
    virtual ~IConfigSource() = default;
    virtual ConfigMap load(const std::string& filepath) = 0;
};

#endif //NEXUSCORE_ICONFIGSOURCE_HPP