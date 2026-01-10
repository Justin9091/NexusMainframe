//
// Created by jusra on 4-1-2026.
//

#ifndef NEXUSCORE_JSONCONFIGSOURCE_HPP
#define NEXUSCORE_JSONCONFIGSOURCE_HPP

#include <string>
#include <nlohmann/json.hpp>

#include "IConfigSource.hpp"

class JsonConfigSource : public IConfigSource {
public:
    ConfigMap load(const std::string& filepath) override;

private:
    ConfigMap parseJson(const nlohmann::json& j);
};

#endif //NEXUSCORE_JSONCONFIGSOURCE_HPP