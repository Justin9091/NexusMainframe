#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include "config/NexusConfig.hpp"

TEST_CASE("NexusConfig has correct defaults", "[nexusconfig]") {
    NexusConfig cfg;

    REQUIRE(cfg.mqtt.host     == "192.168.2.161");
    REQUIRE(cfg.mqtt.port     == 1883);
    REQUIRE(cfg.mqtt.clientId == "nexus-core");
    REQUIRE(cfg.http.port     == 8080);
    REQUIRE(cfg.ws.port       == 8082);
}

TEST_CASE("NexusConfig::toJson serializes all sections", "[nexusconfig]") {
    NexusConfig cfg;
    auto j = cfg.toJson();

    REQUIRE(j["mqtt"]["host"]      == "192.168.2.161");
    REQUIRE(j["mqtt"]["port"]      == 1883);
    REQUIRE(j["mqtt"]["client_id"] == "nexus-core");
    REQUIRE(j["http"]["port"]      == 8080);
    REQUIRE(j["ws"]["port"]        == 8082);
}

TEST_CASE("NexusConfig save and loadOrCreate round-trip", "[nexusconfig]") {
    auto tmp = std::filesystem::temp_directory_path() / "nexus_test_config.json";

    {
        NexusConfig cfg;
        cfg.mqtt.host = "10.0.0.1";
        cfg.mqtt.port = 1884;
        cfg.http.port = 9090;
        cfg.ws.port   = 9091;
        cfg.save(tmp);
    }

    {
        auto cfg = NexusConfig::loadOrCreate(tmp);
        REQUIRE(cfg.mqtt.host == "10.0.0.1");
        REQUIRE(cfg.mqtt.port == 1884);
        REQUIRE(cfg.http.port == 9090);
        REQUIRE(cfg.ws.port   == 9091);
    }

    std::filesystem::remove(tmp);
}

TEST_CASE("NexusConfig::loadOrCreate creates file with defaults when missing", "[nexusconfig]") {
    auto tmp = std::filesystem::temp_directory_path() / "nexus_test_config_new.json";
    std::filesystem::remove(tmp);

    auto cfg = NexusConfig::loadOrCreate(tmp);

    REQUIRE(std::filesystem::exists(tmp));
    REQUIRE(cfg.mqtt.host == "192.168.2.161");
    REQUIRE(cfg.http.port == 8080);

    std::filesystem::remove(tmp);
}

TEST_CASE("NexusConfig::loadOrCreate falls back to defaults for missing keys", "[nexusconfig]") {
    auto tmp = std::filesystem::temp_directory_path() / "nexus_test_config_partial.json";

    {
        std::ofstream f(tmp);
        f << R"({"mqtt": {"host": "mybroker"}})";
    }

    auto cfg = NexusConfig::loadOrCreate(tmp);

    REQUIRE(cfg.mqtt.host == "mybroker");
    REQUIRE(cfg.mqtt.port == 1883);
    REQUIRE(cfg.http.port == 8080);

    std::filesystem::remove(tmp);
}
