#include <catch2/catch_test_macros.hpp>
#include "rooms/Room.hpp"

TEST_CASE("Room::toJson serializes all fields", "[room]") {
    Room r;
    r.id        = "room-1";
    r.name      = "Living Room";
    r.deviceIds = {"d1", "d2"};

    auto j = r.toJson();

    REQUIRE(j["id"]   == "room-1");
    REQUIRE(j["name"] == "Living Room");
    REQUIRE(j["deviceIds"].size() == 2);
    REQUIRE(j["deviceIds"][0] == "d1");
    REQUIRE(j["deviceIds"][1] == "d2");
}

TEST_CASE("Room::fromJson deserializes all fields", "[room]") {
    nlohmann::json j = {
        {"id",        "room-2"},
        {"name",      "Kitchen"},
        {"deviceIds", {"d3", "d4", "d5"}}
    };

    Room r = Room::fromJson(j);

    REQUIRE(r.id   == "room-2");
    REQUIRE(r.name == "Kitchen");
    REQUIRE(r.deviceIds.size() == 3);
    REQUIRE(r.deviceIds[2] == "d5");
}

TEST_CASE("Room JSON round-trip preserves all data", "[room]") {
    Room original;
    original.id        = "round-trip";
    original.name      = "Bedroom";
    original.deviceIds = {"x", "y"};

    Room restored = Room::fromJson(original.toJson());

    REQUIRE(restored.id            == original.id);
    REQUIRE(restored.name          == original.name);
    REQUIRE(restored.deviceIds     == original.deviceIds);
}

TEST_CASE("Room::fromJson uses empty defaults for missing fields", "[room]") {
    Room r = Room::fromJson(nlohmann::json::object());

    REQUIRE(r.id.empty());
    REQUIRE(r.name.empty());
    REQUIRE(r.deviceIds.empty());
}

TEST_CASE("Room::toJson with empty deviceIds produces empty array", "[room]") {
    Room r;
    r.id   = "r1";
    r.name = "Empty";

    auto j = r.toJson();
    REQUIRE(j["deviceIds"].is_array());
    REQUIRE(j["deviceIds"].empty());
}
