//
// Created by jusra on 2-1-2026.
//

#ifndef NEXUSCORE_SCHEDULER_HPP
#define NEXUSCORE_SCHEDULER_HPP
#include <croncpp.h>
#include <string>

#include "Task/ITask.hpp"

struct ScheduledTask {
    ITask* task;
    cron::cronexpr cronExpr;
    std::time_t nextExecution;

    ScheduledTask(ITask* t, const cron::cronexpr& c)
        : task(t), cronExpr(c)
    {
        std::time_t now = std::time(nullptr);
        nextExecution = cron::cron_next(cronExpr, now);
    }
};

class Scheduler {
public:
    bool scheduleTask(ITask *task, std::string time);
    void tick(); // wordt aangeroepen in je loop

private:
    std::vector<ScheduledTask> _tasks;
};

#endif //NEXUSCORE_SCHEDULER_HPP