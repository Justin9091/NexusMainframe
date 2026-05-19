//
// Created by jusra on 19-5-2026.
//

#ifndef NEXUSCORE_EVENTTASK_HPP
#define NEXUSCORE_EVENTTASK_HPP

#include <string>
#include "ITask.hpp"
#include "event/EventBus.hpp"

class EventTask : public ITask {
public:
    EventTask(ITask& task, EventBus& bus, std::string completionEvent);
    void execute() override;

private:
    ITask&      _task;
    EventBus&   _bus;
    std::string _completionEvent;
};

#endif //NEXUSCORE_EVENTTASK_HPP
