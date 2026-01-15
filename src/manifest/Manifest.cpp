//
// Created by jusra on 14-1-2026.
//

#include "manifest/Manifest.hpp"

#include <iostream>
#include <fstream>

#include "Event/EventBus.hpp"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

bool Manifest::save() {
    json j;
    j["enabled"] = json::array();

    for (const auto& name : _enabled) {
        j["enabled"].push_back(name);
    }

    std::ofstream file("manifest.json", std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open manifest.json for writing" << std::endl;
        return false;
    }

    try {
        file << j.dump(4); // 4 = pretty print
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error writing manifest.json: " << e.what() << std::endl;
        return false;
    }

    return true;
}


bool Manifest::load() {
    std::ifstream file("manifest.json");
    if (!file.is_open()) {
        return false; // eerste run? prima.
    }

    json j;
    try {
        file >> j;
        file.close();
    } catch (const std::exception& e) {
        std::cerr << "Error parsing manifest.json: " << e.what() << std::endl;
        file.close();
        return false; // corrupt bestand
    }

    _enabled.clear();

    if (j.contains("enabled") && j["enabled"].is_array()) {
        for (const auto& entry : j["enabled"]) {
            if (entry.is_string()) {
                _enabled.insert(entry.get<std::string>());
            }
        }
    }

    return true;
}

void Manifest::listen() {
    EventBus::getInstance().subscribe("event:enable", [this](const Event& event) {
        try {
            if (auto name = std::any_cast<std::string>(&event.data)) {
                if (_enabled.insert(*name).second) {
                    save();
                }
            }
        } catch (const std::bad_any_cast& e) {
            std::cerr << "Bad any_cast in event:enable: " << e.what() << std::endl;
        }
    });

    EventBus::getInstance().subscribe("event:disable", [this](const Event& event) {
        try {
            if (auto name = std::any_cast<std::string>(&event.data)) {
                if (_enabled.erase(*name) > 0) {
                    save();
                }
            }
        } catch (const std::bad_any_cast& e) {
            std::cerr << "Bad any_cast in event:disable: " << e.what() << std::endl;
        }
    });
}