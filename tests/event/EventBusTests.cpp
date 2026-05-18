#include <catch2/catch_test_macros.hpp>
#include "Event/EventBus.hpp"

TEST_CASE("EventBus is a singleton", "[eventbus]") {
    EventBus& a = EventBus::getInstance();
    EventBus& b = EventBus::getInstance();
    REQUIRE(&a == &b);
}

TEST_CASE("EventBus subscribe does not throw", "[eventbus]") {
    auto& bus = EventBus::getInstance();
    REQUIRE_NOTHROW(bus.subscribe("test.nothrow", [](const Event&) {}));
}

TEST_CASE("EventBus publish and dispatch delivers event to subscriber", "[eventbus]") {
    auto& bus = EventBus::getInstance();
    bool received = false;

    bus.subscribe("test.deliver", [&](const Event& e) {
        received = (e.name == "test.deliver");
    });

    bus.publish({"test.deliver", {}});
    bus.dispatchPending();

    REQUIRE(received);
}

TEST_CASE("EventBus delivers event to multiple subscribers", "[eventbus]") {
    auto& bus = EventBus::getInstance();
    int count = 0;

    bus.subscribe("test.multi", [&](const Event&) { ++count; });
    bus.subscribe("test.multi", [&](const Event&) { ++count; });

    bus.publish({"test.multi", {}});
    bus.dispatchPending();

    REQUIRE(count == 2);
}

TEST_CASE("EventBus passes event data to subscriber", "[eventbus]") {
    auto& bus = EventBus::getInstance();
    std::string received;

    bus.subscribe("test.data", [&](const Event& e) {
        received = std::any_cast<std::string>(e.data);
    });

    bus.publish({"test.data", std::string{"hello"}});
    bus.dispatchPending();

    REQUIRE(received == "hello");
}

TEST_CASE("EventBus unsubscribe stops delivery", "[eventbus]") {
    auto& bus = EventBus::getInstance();
    int count = 0;

    int id = bus.subscribe("test.unsub", [&](const Event&) { ++count; });

    bus.publish({"test.unsub", {}});
    bus.dispatchPending();
    REQUIRE(count == 1);

    bus.unsubscribe("test.unsub", id);

    bus.publish({"test.unsub", {}});
    bus.dispatchPending();
    REQUIRE(count == 1);
}

TEST_CASE("EventBus does not deliver to wrong event name", "[eventbus]") {
    auto& bus = EventBus::getInstance();
    bool received = false;

    bus.subscribe("test.correct", [&](const Event&) { received = true; });

    bus.publish({"test.other", {}});
    bus.dispatchPending();

    REQUIRE_FALSE(received);
}
