#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include "rooms/RoomRegistry.hpp"

namespace {
Room makeRoom(std::string id, std::string name = "Test Room") {
    Room r;
    r.id   = std::move(id);
    r.name = std::move(name);
    return r;
}
}

TEST_CASE("RoomRegistry starts empty", "[roomregistry]") {
    RoomRegistry reg{""};
    REQUIRE(reg.list().empty());
}

TEST_CASE("RoomRegistry add and get", "[roomregistry]") {
    RoomRegistry reg{""};
    reg.add(makeRoom("r1", "Kitchen"));

    auto result = reg.get("r1");
    REQUIRE(result.has_value());
    REQUIRE(result->id   == "r1");
    REQUIRE(result->name == "Kitchen");
}

TEST_CASE("RoomRegistry get returns nullopt for unknown id", "[roomregistry]") {
    RoomRegistry reg{""};
    REQUIRE_FALSE(reg.get("ghost").has_value());
}

TEST_CASE("RoomRegistry add rejects duplicate id", "[roomregistry]") {
    RoomRegistry reg{""};
    REQUIRE(reg.add(makeRoom("r1")) == true);
    REQUIRE(reg.add(makeRoom("r1")) == false);
    REQUIRE(reg.list().size() == 1);
}

TEST_CASE("RoomRegistry list returns all rooms", "[roomregistry]") {
    RoomRegistry reg{""};
    reg.add(makeRoom("r1"));
    reg.add(makeRoom("r2"));
    reg.add(makeRoom("r3"));

    REQUIRE(reg.list().size() == 3);
}

TEST_CASE("RoomRegistry exists", "[roomregistry]") {
    RoomRegistry reg{""};
    reg.add(makeRoom("r1"));

    REQUIRE(reg.exists("r1")    == true);
    REQUIRE(reg.exists("ghost") == false);
}

TEST_CASE("RoomRegistry remove", "[roomregistry]") {
    RoomRegistry reg{""};
    reg.add(makeRoom("r1"));

    REQUIRE(reg.remove("r1")    == true);
    REQUIRE(reg.remove("r1")    == false);
    REQUIRE(reg.exists("r1")    == false);
    REQUIRE(reg.list().empty());
}

TEST_CASE("RoomRegistry addDevice assigns device to room", "[roomregistry]") {
    RoomRegistry reg{""};
    reg.add(makeRoom("r1"));

    REQUIRE(reg.addDevice("r1", "d1") == true);

    auto room = reg.get("r1");
    REQUIRE(room.has_value());
    REQUIRE(room->deviceIds.size() == 1);
    REQUIRE(room->deviceIds[0] == "d1");
}

TEST_CASE("RoomRegistry addDevice rejects duplicate device", "[roomregistry]") {
    RoomRegistry reg{""};
    reg.add(makeRoom("r1"));

    REQUIRE(reg.addDevice("r1", "d1") == true);
    REQUIRE(reg.addDevice("r1", "d1") == false);
    REQUIRE(reg.get("r1")->deviceIds.size() == 1);
}

TEST_CASE("RoomRegistry addDevice returns false for unknown room", "[roomregistry]") {
    RoomRegistry reg{""};
    REQUIRE(reg.addDevice("ghost", "d1") == false);
}

TEST_CASE("RoomRegistry removeDevice", "[roomregistry]") {
    RoomRegistry reg{""};
    reg.add(makeRoom("r1"));
    reg.addDevice("r1", "d1");
    reg.addDevice("r1", "d2");

    REQUIRE(reg.removeDevice("r1", "d1")    == true);
    REQUIRE(reg.removeDevice("r1", "ghost") == false);

    auto room = reg.get("r1");
    REQUIRE(room->deviceIds.size() == 1);
    REQUIRE(room->deviceIds[0] == "d2");
}

TEST_CASE("RoomRegistry save and load round-trip", "[roomregistry]") {
    auto tmp = std::filesystem::temp_directory_path() / "nexus_test_rooms.json";

    {
        RoomRegistry reg{tmp};
        auto r = makeRoom("r1", "Saved Room");
        r.deviceIds = {"d1", "d2"};
        reg.add(r);
        reg.save();
    }

    {
        RoomRegistry reg{tmp};
        reg.load();

        auto r = reg.get("r1");
        REQUIRE(r.has_value());
        REQUIRE(r->name == "Saved Room");
        REQUIRE(r->deviceIds.size() == 2);
    }

    std::filesystem::remove(tmp);
}
