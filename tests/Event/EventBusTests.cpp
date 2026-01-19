#include <iostream>
#include <catch2/catch_test_macros.hpp>

#include "Event/EventBus.hpp"

TEST_CASE("Eventbus tests") {
    EventBus& bus = EventBus::getInstance();

    SECTION("EventBus is singleton") {
        EventBus& secondBus = EventBus::getInstance();

        EventBus* firstBusPtr = &bus;
        EventBus* secondBusPtr = &secondBus;

        REQUIRE(firstBusPtr == secondBusPtr);
    }

    SECTION("Subscribe to event") {
        REQUIRE_NOTHROW(bus.subscribe("test", [](const Event& event) {
            return;
        }));
    }

    SECTION("Publish event") {
        REQUIRE_NOTHROW(bus.publish({"test", {}}));
    }
}

TEST_CASE("Eventbus integration") {
    EventBus& bus = EventBus::getInstance();

    SECTION("Publish and react to event") {
        bus.subscribe("test", [](const Event& event) {
            std::cout << "Received event: " << event.name << "\n";
            REQUIRE(event.name == "test");
        });

        bus.publish({"test", {}});

        bus.dispatchPending();
    }
}