#pragma once
#include <filesystem>
#include <string>
#include <nlohmann/json.hpp>

/**
 * @brief Top-level application configuration, persisted as JSON.
 *
 * Contains sub-configurations for MQTT, HTTP, and WebSocket servers.
 * On first run, loadOrCreate() writes the default values to disk so the user
 * can override them.
 *
 * @code
 * auto cfg = NexusConfig::loadOrCreate("/home/user/.nexus/config.json");
 * // cfg.mqtt.host == "192.168.2.161"  (default)
 * cfg.http.port = 9090;
 * cfg.save("/home/user/.nexus/config.json");
 * @endcode
 */
struct NexusConfig {
    /** @brief MQTT broker connection parameters. */
    struct Mqtt {
        std::string host     = "192.168.2.161"; ///< Broker hostname or IP.
        int         port     = 1883;             ///< Broker TCP port.
        std::string clientId = "nexus-core";     ///< MQTT client identifier.
    } mqtt;

    /** @brief HTTP server parameters. */
    struct Http {
        int port = 8080; ///< TCP port for the REST API.
    } http;

    /** @brief WebSocket server parameters. */
    struct Ws {
        int port = 8082; ///< TCP port for the WebSocket server.
    } ws;

    /**
     * @brief Loads configuration from @p path, or creates the file with defaults.
     * @param path  Absolute path to the JSON config file.
     * @return Populated NexusConfig; defaults are used for missing keys.
     */
    static NexusConfig loadOrCreate(const std::filesystem::path& path);

    /**
     * @brief Writes the current configuration to @p path as pretty-printed JSON.
     * @param path  Absolute path to the target file.
     */
    void save(const std::filesystem::path& path) const;

    /** @brief Serialises the configuration to a JSON object. */
    nlohmann::json toJson() const;
};
