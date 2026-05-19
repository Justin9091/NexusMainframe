//
// Created by jusra on 2-1-2026.
//

#ifndef NEXUSCORE_ITASK_HPP
#define NEXUSCORE_ITASK_HPP

#include <string_view>

/**
 * @brief Interface for tasks that can be scheduled via the Scheduler.
 *
 * Implement execute() with the work to be performed on each scheduled trigger.
 *
 * @code
 * struct CleanupTask : ITask {
 *     void execute() override { purgeOldLogs(); }
 * };
 * scheduler.scheduleTask(&cleanup, "0 3 * * *"); // 03:00 daily
 * @endcode
 */
class ITask {
public:
    virtual ~ITask() = default;

    /** @brief Called by the Scheduler when the cron expression fires. */
    virtual void execute() = 0;

    virtual std::string_view name() const noexcept { return {}; }
};


#endif //NEXUSCORE_ITASK_HPP