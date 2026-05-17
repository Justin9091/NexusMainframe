#include "config/NexusConfig.hpp"
#include <fstream>

nlohmann::json NexusConfig::toJson() const {
    return {
        {"mqtt", {
            {"host",      mqtt.host},
            {"port",      mqtt.port},
            {"client_id", mqtt.clientId},
        }},
        {"http", {
            {"port", http.port},
        }},
        {"ws", {
            {"port", ws.port},
        }},
    };
}

void NexusConfig::save(const std::filesystem::path& path) const {
    std::filesystem::create_directories(path.parent_path());
    std::ofstream f(path);
    f << toJson().dump(2);
}

NexusConfig NexusConfig::loadOrCreate(const std::filesystem::path& path) {
    NexusConfig cfg;

    if (!std::filesystem::exists(path)) {
        cfg.save(path);
        return cfg;
    }

    std::ifstream f(path);
    if (!f.is_open()) return cfg;

    try {
        auto j = nlohmann::json::parse(f);

        if (j.contains("mqtt")) {
            auto& m = j["mqtt"];
            cfg.mqtt.host     = m.value("host",      cfg.mqtt.host);
            cfg.mqtt.port     = m.value("port",      cfg.mqtt.port);
            cfg.mqtt.clientId = m.value("client_id", cfg.mqtt.clientId);
        }
        if (j.contains("http"))
            cfg.http.port = j["http"].value("port", cfg.http.port);

        if (j.contains("ws"))
            cfg.ws.port = j["ws"].value("port", cfg.ws.port);

    } catch (...) {}

    return cfg;
}
