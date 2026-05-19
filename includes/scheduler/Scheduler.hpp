//
// Created by jusra on 2-1-2026.
//

#ifndef NEXUSCORE_SCHEDULER_HPP
#define NEXUSCORE_SCHEDULER_HPP
#include <croncpp.h>
#include <string>

#include "task/ITask.hpp"

/**
 * @brief Internal descriptor for a cron-scheduled task.
 *
 * Constructed by Scheduler::scheduleTask(); not intended for direct use.
 */
struct ScheduledTask {
    ITask*            task;          ///< Non-owning pointer to the task to execute.
    cron::cronexpr    cronExpr;      ///< Parsed cron expression.
    std::time_t       nextExecution; ///< Unix timestamp of the next scheduled run.

    /**
     * @param t Pointer to the task; must outlive this ScheduledTask.
     * @param c Parsed cron expression from croncpp.
     */
    ScheduledTask(ITask* t, const cron::cronexpr& c)
        : task(t), cronExpr(c)
    {
        std::time_t now = std::time(nullptr);
        nextExecution = cron::cron_next(cronExpr, now);
    }
};

/**
 * @brief Cron-based task scheduler.
 *
 * Tasks are registered with a cron expression string and executed by calling
 * tick() from the main loop.  Uses croncpp for expression parsing.
 *
 * @code
 * Scheduler scheduler;
 * scheduler.scheduleTask(&myTask, "0 * * * *"); // every hour
 *
 * while (running) {
 *     scheduler.tick();
 *     std::this_thread::sleep_for(std::chrono::seconds(1));
 * }
 * @endcode
 */
class Scheduler {
public:
    /**
     * @brief Registers a task to run on a cron schedule.
     * @param task  Non-owning pointer; caller must keep the task alive.
     * @param time  Cron expression string (e.g. @c "0 * * * *").
     * @return @c true if the expression is valid and the task was registered.
     * @return @c false if the cron expression cannot be parsed.
     */
    bool scheduleTask(ITask* task, std::string time);

    /**
     * @brief Checks all scheduled tasks and executes any that are due.
     * Must be called repeatedly from the application main loop.
     */
    void tick();

private:
    std::vector<ScheduledTask> _tasks;
};

#endif //NEXUSCORE_SCHEDULER_HPP