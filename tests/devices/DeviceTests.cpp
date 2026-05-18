#include <catch2/catch_test_macros.hpp>
#include "devices/Device.hpp"

TEST_CASE("Device::toJson serializes all fields", "[device]") {
    Device d;
    d.id     = "abc-123";
    d.name   = "Living room light";
    d.type   = "light";
    d.online = true;
    d.state  = {{"on", true}, {"brightness", 80}};

    auto j = d.toJson();

    REQUIRE(j["id"]              == "abc-123");
    REQUIRE(j["name"]            == "Living room light");
    REQUIRE(j["type"]            == "light");
    REQUIRE(j["online"]          == true);
    REQUIRE(j["state"]["on"]     == true);
    REQUIRE(j["state"]["brightness"] == 80);
}

TEST_CASE("Device::fromJson deserializes all fields", "[device]") {
    nlohmann::json j = {
        {"id",     "xyz-789"},
        {"name",   "Sensor"},
        {"type",   "sensor"},
        {"online", false},
        {"state",  {{"temperature", 21.5}}}
    };

    Device d = Device::fromJson(j);

    REQUIRE(d.id                        == "xyz-789");
    REQUIRE(d.name                      == "Sensor");
    REQUIRE(d.type                      == "sensor");
    REQUIRE(d.online                    == false);
    REQUIRE(d.state["temperature"]      == 21.5);
}

TEST_CASE("Device JSON round-trip preserves all data", "[device]") {
    Device original;
    original.id     = "round-trip-id";
    original.name   = "Test device";
    original.type   = "switch";
    original.online = true;
    original.state  = {{"active", false}};

    Device restored = Device::fromJson(original.toJson());

    REQUIRE(restored.id              == original.id);
    REQUIRE(restored.name            == original.name);
    REQUIRE(restored.type            == original.type);
    REQUIRE(restored.online          == original.online);
    REQUIRE(restored.state["active"] == false);
}

TEST_CASE("Device::fromJson uses sensible defaults for missing fields", "[device]") {
    Device d = Device::fromJson(nlohmann::json::object());

    REQUIRE(d.id.empty());
    REQUIRE(d.name.empty());
    REQUIRE(d.type.empty());
    REQUIRE(d.online  == false);
    REQUIRE(d.state   == nlohmann::json::object());
}
