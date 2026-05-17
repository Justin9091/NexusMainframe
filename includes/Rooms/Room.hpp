#pragma once
#include <string>
#include <vector>
#include <nlohmann/json.hpp>

struct Room {
    std::string              id;
    std::string              name;
    std::vector<std::string> deviceIds;

    nlohmann::json toJson() const;
    static Room    fromJson(const nlohmann::json& j);
};
