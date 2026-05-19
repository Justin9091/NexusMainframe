//
// Created by jusra on 19-5-2026.
//

#include "task/EventTask.hpp"
#include "event/EventBus.hpp"
#include "logging/WebLogger.hpp"

EventTask::EventTask(ITask& task, std::string completionEvent)
    : _task(task), _completionEvent(std::move(completionEvent))
{}

void EventTask::execute() {
    try {
        _task.execute();
        EventBus::getInstance().publish({_completionEvent, {}});
    } catch (const std::exception& e) {
        WebLogger("EventTask").error("Task execution failed: " + std::string{e.what()});
    } catch (...) {
        WebLogger("EventTask").error("Task execution failed with unknown error");
    }
}
