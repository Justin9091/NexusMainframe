#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include "config/Source/JsonConfigSource.hpp"

namespace {
std::filesystem::path writeTempJson(const std::string& content) {
    auto path = std::filesystem::temp_directory_path() / "nexus_test_jsoncfg.json";
    std::ofstream f(path);
    f << content;
    return path;
}
}

TEST_CASE("JsonConfigSource loads string values", "[jsonconfigsource]") {
    auto path = writeTempJson(R"({"host": "localhost", "name": "nexus"})");

    JsonConfigSource src;
    auto map = src.load(path.string());

    REQUIRE(std::get<std::string>(map.at("host")) == "localhost");
    REQUIRE(std::get<std::string>(map.at("name")) == "nexus");

    std::filesystem::remove(path);
}

TEST_CASE("JsonConfigSource loads integer values", "[jsonconfigsource]") {
    auto path = writeTempJson(R"({"port": 8080, "timeout": 30})");

    JsonConfigSource src;
    auto map = src.load(path.string());

    REQUIRE(std::get<int>(map.at("port"))    == 8080);
    REQUIRE(std::get<int>(map.at("timeout")) == 30);

    std::filesystem::remove(path);
}

TEST_CASE("JsonConfigSource loads float values", "[jsonconfigsource]") {
    auto path = writeTempJson(R"({"ratio": 0.75})");

    JsonConfigSource src;
    auto map = src.load(path.string());

    REQUIRE(std::get<double>(map.at("ratio")) == 0.75);

    std::filesystem::remove(path);
}

TEST_CASE("JsonConfigSource loads boolean values", "[jsonconfigsource]") {
    auto path = writeTempJson(R"({"enabled": true, "debug": false})");

    JsonConfigSource src;
    auto map = src.load(path.string());

    REQUIRE(std::get<bool>(map.at("enabled")) == true);
    REQUIRE(std::get<bool>(map.at("debug"))   == false);

    std::filesystem::remove(path);
}

TEST_CASE("JsonConfigSource throws on missing file", "[jsonconfigsource]") {
    JsonConfigSource src;
    REQUIRE_THROWS_AS(src.load("/nonexistent/path/config.json"), std::runtime_error);
}

TEST_CASE("JsonConfigSource returns empty map for empty JSON object", "[jsonconfigsource]") {
    auto path = writeTempJson("{}");

    JsonConfigSource src;
    auto map = src.load(path.string());

    REQUIRE(map.empty());

    std::filesystem::remove(path);
}
