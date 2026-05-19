//
// Created by jusra on 19-5-2026.
//

#ifndef NEXUSCORE_EVENTTASK_HPP
#define NEXUSCORE_EVENTTASK_HPP
#include <string>
#include "ITask.hpp"

class EventTask : public ITask {
public:
    EventTask(ITask& task, std::string completionEvent);
    void execute() override;

private:
    ITask&      _task;
    std::string _completionEvent;
};

#endif //NEXUSCORE_EVENTTASK_HPP