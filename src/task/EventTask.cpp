//
// Created by jusra on 19-5-2026.
//

#include "task/EventTask.hpp"
#include "logging/WebLogger.hpp"

EventTask::EventTask(ITask& task, EventBus& bus, std::string completionEvent)
    : _task(task), _bus(bus), _completionEvent(std::move(completionEvent))
{}

void EventTask::execute() {
    try {
        _task.execute();
        _bus.publish({_completionEvent, {}});
    } catch (const std::exception& e) {
        WebLogger("EventTask").error("Task execution failed: " + std::string{e.what()});
    } catch (...) {
        WebLogger("EventTask").error("Task execution failed with unknown error");
    }
}
