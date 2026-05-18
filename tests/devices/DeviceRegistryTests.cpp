#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include "devices/DeviceRegistry.hpp"

namespace {
Device makeDevice(std::string id, std::string name = "Test", std::string type = "light") {
    Device d;
    d.id   = std::move(id);
    d.name = std::move(name);
    d.type = std::move(type);
    return d;
}
}

TEST_CASE("DeviceRegistry starts empty", "[registry]") {
    DeviceRegistry reg{""};
    REQUIRE(reg.list().empty());
}

TEST_CASE("DeviceRegistry add and get", "[registry]") {
    DeviceRegistry reg{""};
    reg.add(makeDevice("d1", "Light", "light"));

    auto result = reg.get("d1");
    REQUIRE(result.has_value());
    REQUIRE(result->id   == "d1");
    REQUIRE(result->name == "Light");
    REQUIRE(result->type == "light");
}

TEST_CASE("DeviceRegistry get returns nullopt for unknown id", "[registry]") {
    DeviceRegistry reg{""};
    REQUIRE_FALSE(reg.get("nonexistent").has_value());
}

TEST_CASE("DeviceRegistry add rejects duplicate id", "[registry]") {
    DeviceRegistry reg{""};
    REQUIRE(reg.add(makeDevice("d1")) == true);
    REQUIRE(reg.add(makeDevice("d1")) == false);
    REQUIRE(reg.list().size() == 1);
}

TEST_CASE("DeviceRegistry list returns all devices", "[registry]") {
    DeviceRegistry reg{""};
    reg.add(makeDevice("d1"));
    reg.add(makeDevice("d2"));
    reg.add(makeDevice("d3"));

    REQUIRE(reg.list().size() == 3);
}

TEST_CASE("DeviceRegistry exists", "[registry]") {
    DeviceRegistry reg{""};
    reg.add(makeDevice("d1"));

    REQUIRE(reg.exists("d1")          == true);
    REQUIRE(reg.exists("nonexistent") == false);
}

TEST_CASE("DeviceRegistry remove", "[registry]") {
    DeviceRegistry reg{""};
    reg.add(makeDevice("d1"));

    REQUIRE(reg.remove("d1")           == true);
    REQUIRE(reg.remove("d1")           == false);
    REQUIRE(reg.exists("d1")           == false);
    REQUIRE(reg.list().empty());
}

TEST_CASE("DeviceRegistry updateState merges patch", "[registry]") {
    DeviceRegistry reg{""};
    Device d = makeDevice("d1");
    d.state  = {{"on", false}, {"brightness", 50}};
    reg.add(d);

    reg.updateState("d1", {{"on", true}, {"color", "red"}});

    auto updated = reg.get("d1");
    REQUIRE(updated.has_value());
    REQUIRE(updated->state["on"]         == true);
    REQUIRE(updated->state["brightness"] == 50);
    REQUIRE(updated->state["color"]      == "red");
}

TEST_CASE("DeviceRegistry updateState returns false for unknown id", "[registry]") {
    DeviceRegistry reg{""};
    REQUIRE(reg.updateState("ghost", {{"on", true}}) == false);
}

TEST_CASE("DeviceRegistry save and load round-trip", "[registry]") {
    auto tmp = std::filesystem::temp_directory_path() / "nexus_test_registry.json";

    {
        DeviceRegistry reg{tmp};
        reg.add(makeDevice("d1", "Saved light", "light"));
        reg.save();
    }

    {
        DeviceRegistry reg{tmp};
        reg.load();

        auto d = reg.get("d1");
        REQUIRE(d.has_value());
        REQUIRE(d->name == "Saved light");
    }

    std::filesystem::remove(tmp);
}
