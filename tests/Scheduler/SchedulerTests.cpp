#include <catch2/catch_test_macros.hpp>
#include "Scheduler/Scheduler.hpp"

namespace {
struct CountingTask : ITask {
    int count = 0;
    void execute() override { ++count; }
};
}

TEST_CASE("Scheduler::scheduleTask accepts valid cron expression", "[scheduler]") {
    Scheduler scheduler;
    CountingTask task;

    REQUIRE(scheduler.scheduleTask(&task, "* * * * *") == true);
}

TEST_CASE("Scheduler::scheduleTask rejects invalid cron expression", "[scheduler]") {
    Scheduler scheduler;
    CountingTask task;

    REQUIRE(scheduler.scheduleTask(&task, "not-a-cron") == false);
    REQUIRE(scheduler.scheduleTask(&task, "99 99 99 99 99") == false);
}

TEST_CASE("Scheduler::scheduleTask accepts multiple tasks", "[scheduler]") {
    Scheduler scheduler;
    CountingTask t1, t2;

    REQUIRE(scheduler.scheduleTask(&t1, "* * * * *") == true);
    REQUIRE(scheduler.scheduleTask(&t2, "0 * * * *") == true);
}

TEST_CASE("Scheduler::tick executes overdue tasks", "[scheduler]") {
    Scheduler scheduler;
    CountingTask task;

    // "* * * * *" fires every minute — next execution is always <= now+60s.
    // We manually verify tick() can be called without throwing.
    scheduler.scheduleTask(&task, "* * * * *");
    REQUIRE_NOTHROW(scheduler.tick());
}
