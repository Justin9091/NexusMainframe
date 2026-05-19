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
    EventTask wrapped(inner, bus, "et.delegate");

    wrapped.execute();

    REQUIRE(inner.count == 1);
}

TEST_CASE("EventTask publiceert completion event na succesvolle execute", "[eventtask]") {
    auto& bus = EventBus::getInstance();
    bool received = false;
    int id = bus.subscribe("et.completed", [&](const Event&) { received = true; });

    CountingTask inner;
    EventTask wrapped(inner, bus, "et.completed");
    wrapped.execute();
    bus.dispatchPending();

    bus.unsubscribe("et.completed", id);
    REQUIRE(received);
}

TEST_CASE("EventTask publiceert geen event als inner task een exception gooit", "[eventtask]") {
    auto& bus = EventBus::getInstance();
    bool received = false;
    int id = bus.subscribe("et.threw", [&](const Event&) { received = true; });

    ThrowingTask inner;
    EventTask wrapped(inner, bus, "et.threw");
    REQUIRE_NOTHROW(wrapped.execute());
    bus.dispatchPending();

    bus.unsubscribe("et.threw", id);
    REQUIRE_FALSE(received);
}

TEST_CASE("EventTask laat exception niet propageren", "[eventtask]") {
    auto& bus = EventBus::getInstance();
    ThrowingTask inner;
    EventTask wrapped(inner, bus, "et.nopropagate");
    REQUIRE_NOTHROW(wrapped.execute());
}

// ── EventTrigger ──────────────────────────────────────────────────────────────

TEST_CASE("EventTrigger voert task uit wanneer event dispatched wordt", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;
    EventTrigger trigger(bus, "tr.fire", task);

    bus.publish({"tr.fire", {}});
    bus.dispatchPending();

    REQUIRE(task.count == 1);
}

TEST_CASE("EventTrigger voert task niet uit bij een ander event", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;
    EventTrigger trigger(bus, "tr.correct", task);

    bus.publish({"tr.other", {}});
    bus.dispatchPending();

    REQUIRE(task.count == 0);
}

TEST_CASE("EventTrigger unsubscribet automatisch bij destructie", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;

    {
        EventTrigger trigger(bus, "tr.raii", task);
        bus.publish({"tr.raii", {}});
        bus.dispatchPending();
        REQUIRE(task.count == 1);
    }

    bus.publish({"tr.raii", {}});
    bus.dispatchPending();
    REQUIRE(task.count == 1);
}

TEST_CASE("Meerdere EventTriggers op hetzelfde event triggeren elk hun eigen task", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask taskA, taskB;
    EventTrigger triggerA(bus, "tr.multi", taskA);
    EventTrigger triggerB(bus, "tr.multi", taskB);

    bus.publish({"tr.multi", {}});
    bus.dispatchPending();

    REQUIRE(taskA.count == 1);
    REQUIRE(taskB.count == 1);
}

TEST_CASE("Moved-from EventTrigger unsubscribet niet bij destructie", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;

    EventTrigger original(bus, "tr.movector", task);
    EventTrigger moved(std::move(original));

    bus.publish({"tr.movector", {}});
    bus.dispatchPending();

    REQUIRE(task.count == 1);
}

TEST_CASE("EventTrigger move assignment behoudt subscription", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask task;

    EventTrigger a(bus, "tr.moveassign", task);
    EventTrigger b = std::move(a);

    bus.publish({"tr.moveassign", {}});
    bus.dispatchPending();

    REQUIRE(task.count == 1);
}

TEST_CASE("EventTrigger move assignment verwijdert oude subscription", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    CountingTask taskA, taskB;

    EventTrigger a(bus, "tr.moveassign.old", taskA);
    EventTrigger b(bus, "tr.moveassign.new", taskB);
    b = std::move(a);

    // b's oude subscription (tr.moveassign.new) moet verwijderd zijn
    bus.publish({"tr.moveassign.new", {}});
    bus.dispatchPending();
    REQUIRE(taskB.count == 0);

    // b's nieuwe subscription (tr.moveassign.old, overgenomen van a) moet actief zijn
    bus.publish({"tr.moveassign.old", {}});
    bus.dispatchPending();
    REQUIRE(taskA.count == 1);
}

TEST_CASE("EventTrigger callback exception propageert niet buiten dispatchPending", "[eventtrigger]") {
    auto& bus = EventBus::getInstance();
    ThrowingTask task;
    EventTrigger trigger(bus, "tr.throw", task);

    bus.publish({"tr.throw", {}});
    // EventBus vangt of propageert exceptions via dispatchPending — gedrag vastleggen
    // zodat regressies zichtbaar zijn als EventBus-gedrag wijzigt
    REQUIRE_NOTHROW(bus.dispatchPending());
}
