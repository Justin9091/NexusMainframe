#include "../../includes/Scheduler/Scheduler.hpp"
#include <iostream>

bool Scheduler::scheduleTask(ITask* task, std::string time) {
    try {
        auto cronExpr = cron::make_cron(time);
        _tasks.emplace_back(task, cronExpr);
        return true;
    } catch (cron::bad_cronexpr const & ex) {
        std::cerr << "Invalid cron expression: " << ex.what() << '\n';
        return false;
    }
}

void Scheduler::tick() {
    std::time_t now = std::time(nullptr);

    for (auto& st : _tasks) {
        if (now >= st.nextExecution) {
            st.task->execute(); // ITask heeft een execute() functie
            st.nextExecution = cron::cron_next(st.cronExpr, now);
        }
    }
}
