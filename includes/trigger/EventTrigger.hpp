//
// Created by jusra on 19-5-2026.
//

#ifndef NEXUSCORE_EVENTTRIGGER_HPP
#define NEXUSCORE_EVENTTRIGGER_HPP

#include "event/EventBus.hpp"
#include "task/ITask.hpp"

class EventTrigger {
public:
    EventTrigger(EventBus& bus, std::string eventName, ITask& task);
    ~EventTrigger();

    EventTrigger(EventTrigger&& other) noexcept;
    EventTrigger& operator=(EventTrigger&& other) noexcept;

    EventTrigger(const EventTrigger&)            = delete;
    EventTrigger& operator=(const EventTrigger&) = delete;

private:
    EventBus*   _bus;
    ITask*      _task;
    std::string _eventName;
    int         _eventId = -1;
};

#endif //NEXUSCORE_EVENTTRIGGER_HPP
