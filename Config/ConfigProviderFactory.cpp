//
// Created by jusra on 4-1-2026.
//

#include "ConfigProviderFactory.hpp"

#include <filesystem>
#include <string>

#include "Source/JsonConfigSource.hpp"

ConfigProvider ConfigProviderFactory::create(const std::string &filepath) {
    auto ext = std::filesystem::path(filepath).extension().string();

    if (ext == ".json") {
        return ConfigProvider(std::make_unique<JsonConfigSource>());
    }

    throw std::runtime_error("Unsupported format: " + ext);
}
