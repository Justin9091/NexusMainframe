//
// Created by jusra on 19-5-2026.
//

#include "trigger/EventTrigger.hpp"

EventTrigger::EventTrigger(EventBus &bus, std::string eventName, ITask &task)
    : _bus(&bus), _eventName(std::move(eventName)) {

    _eventId = bus.subscribe(_eventName, [&task](const Event &e) {
        task.execute();
    });
}

EventTrigger::~EventTrigger() {
    if (_bus && _eventId != -1)
        _bus->unsubscribe(_eventName, _eventId);
}

EventTrigger::EventTrigger(EventTrigger&& other) noexcept
    : _bus(other._bus), _eventName(std::move(other._eventName)), _eventId(other._eventId)
{
    other._bus     = nullptr;
    other._eventId = -1;
}

EventTrigger& EventTrigger::operator=(EventTrigger&& other) noexcept {
    if (this != &other) {
        if (_bus && _eventId != -1)
            _bus->unsubscribe(_eventName, _eventId);
        _bus       = other._bus;
        _eventName = std::move(other._eventName);
        _eventId   = other._eventId;
        other._bus     = nullptr;
        other._eventId = -1;
    }
    return *this;
}
