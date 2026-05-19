#include <catch2/catch_test_macros.hpp>
#include "task/EventTask.hpp"
#include "trigger/EventTrigger.hpp"
#include "event/EventBus.hpp"

// ── Stub ──────────────────────────────────────────────────────────────────────

struct CountingTask : ITask {
    int count = 0;
    void execute() override { ++count; }
};

struct ThrowingTask : ITask {
    void execute() override { throw std::runtime_error("task failed"); }
};

// ── EventTask ─────────────────────────────────────────────────────────────────

TEST_CASE("EventTask delegeert execute naar inner task", "[eventtask]") {
    auto& bus = EventBus::getInstance();
    CountingTask inner;
    EventTask wrapped(inner, "task.done");

    wrapped.execute();

    REQUIRE(inner.count == 1);
}

TEST_CASE("EventTask publiceert completion event na succesvolle execute", "[eventtask]") {
    auto& bus = EventBus::getInstance();
    bool received = false;
    bus.subscribe("task.completed", [&](const Event&) { received = true; });

    CountingTask inner;
    EventTask wrapped(inner, "task.completed");
    wrapped.execute();
    bus.dispatchPending();

    REQUIRE(received);
}

TEST_CASE("EventTask publiceert geen event als inner task een exception gooit", "[eventtask]") {
    auto& bus = EventBus::getInstance();
    bool received = false;
    bus.subscribe("task.threw", [&](const Event&) { received = true; });

    ThrowingTask inner;
    EventTask wrapped(inner, "task.threw");
    REQUIRE_NOTHROW(wrapped.execute());
    bus.dispatchPending();

    REQUIRE_FALSE(received);
}

TEST_CASE("EventTask laat exception niet propageren", "[eventtask]") {
    ThrowingTask inner;
    EventTask wrapped(inner, "task.nopropagate");
    REQUIRE_NOTHROW(wrapped.execute());
}

// ── EventTrigger ──────────────────────────────────────────────────────────────

TEST_CASE("EventTrigger voert task uit wanneer event dispatched wordt", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;
    EventTrigger trigger(bus, "trigger.fire", task);

    bus.publish({"trigger.fire", {}});
    bus.dispatchPending();

    REQUIRE(task.count == 1);
}

TEST_CASE("EventTrigger voert task niet uit bij een ander event", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;
    EventTrigger trigger(bus, "trigger.correct", task);

    bus.publish({"trigger.other", {}});
    bus.dispatchPending();

    REQUIRE(task.count == 0);
}

TEST_CASE("EventTrigger unsubscribet automatisch bij destructie", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;

    {
        EventTrigger trigger(bus, "trigger.raii", task);
        bus.publish({"trigger.raii", {}});
        bus.dispatchPending();
        REQUIRE(task.count == 1);
    }

    bus.publish({"trigger.raii", {}});
    bus.dispatchPending();
    REQUIRE(task.count == 1);
}

TEST_CASE("Meerdere EventTriggers op hetzelfde event triggeren elk hun eigen task", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask taskA, taskB;
    EventTrigger triggerA(bus, "trigger.multi", taskA);
    EventTrigger triggerB(bus, "trigger.multi", taskB);

    bus.publish({"trigger.multi", {}});
    bus.dispatchPending();

    REQUIRE(taskA.count == 1);
    REQUIRE(taskB.count == 1);
}

TEST_CASE("Moved-from EventTrigger unsubscribet niet bij destructie", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;

    EventTrigger original(bus, "trigger.move", task);
    EventTrigger moved(std::move(original));

    bus.publish({"trigger.move", {}});
    bus.dispatchPending();

    REQUIRE(task.count == 1);
}
