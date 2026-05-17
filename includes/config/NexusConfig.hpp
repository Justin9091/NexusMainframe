#pragma once
#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>

struct NexusConfig {
    struct Mqtt {
        std::string host     = "192.168.2.161";
        int         port     = 1883;
        std::string clientId = "nexus-core";
    } mqtt;

    struct Http {
        int port = 8080;
    } http;

    struct Ws {
        int port = 8082;
    } ws;

    // Loads config from path. Creates the file with defaults if it doesn't exist.
    static NexusConfig loadOrCreate(const std::filesystem::path& path);
    void save(const std::filesystem::path& path) const;

    nlohmann::json toJson() const;
};
