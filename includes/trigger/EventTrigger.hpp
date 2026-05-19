//
// Created by jusra on 19-5-2026.
//

#ifndef NEXUSCORE_EVENTTRIGGER_HPP
#define NEXUSCORE_EVENTTRIGGER_HPP

#include "event/EventBus.hpp"
#include "task/ITask.hpp"

class EventTrigger {
private:
    EventBus* _bus;
    std::string _eventName;
    int _eventId;

public:
    EventTrigger(EventBus& bus, std::string eventName, ITask& task);
    ~EventTrigger();

    EventTrigger(EventTrigger&& other) noexcept;
    EventTrigger& operator=(EventTrigger&& other) noexcept;

    EventTrigger(const EventTrigger&)            = delete;
    EventTrigger& operator=(const EventTrigger&) = delete;
};

#endif //NEXUSCORE_EVENTTRIGGER_HPP