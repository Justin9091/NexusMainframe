#include "../../includes/Event/EventBus.hpp"
#include <algorithm>
#include <iostream>

int EventBus::subscribe(const std::string &eventName, VoidCallback cb) {
    std::lock_guard<std::mutex> lock(mtx);
    if (isShuttingDown) {
        return -1;
    }
    int id = nextId++;
    listeners[eventName].push_back({id, std::move(cb)});

    _logger.logInfo(
        "Subscribed to event: " + eventName +
        " (ID: " + std::to_string(id) +
        ", Total listeners: " + std::to_string(listeners[eventName].size()) +
        ")"
    );

    return id;
}

int EventBus::subscribe(const std::string &eventName, ResultCallback cb) {
    // Wrap ResultCallback in een VoidCallback
    return subscribe(eventName, [cb = std::move(cb), this, eventName](const Event& event) {
        Result result = cb(event);
        if (result != Result::SUCCESS) {
            _logger.logWarning("Result callback returned non-SUCCESS for event " + eventName);
        }
    });
}

void EventBus::unsubscribe(const std::string &eventName, int id) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = listeners.find(eventName);
    if (it != listeners.end()) {
        auto &vec = it->second;
        vec.erase(std::remove_if(vec.begin(), vec.end(),
                                 [id](const Listener &l) { return l.id == id; }), vec.end());

        _logger.logInfo("Unsubscribed from event: " + eventName + " (ID: " + std::to_string(id) + ")");

        if (vec.empty()) {
            listeners.erase(it);
        }
    }
}

void EventBus::publish(const Event &event) {
    std::lock_guard<std::mutex> lock(mtx);
    if (!isShuttingDown) {
        eventQueue.push(event);
        _logger.logInfo("Event published: " + event.name
                        + " (Queue size: " + std::to_string(eventQueue.size()) + ")");
    } else {
        _logger.logError("Event rejected (shutting down): " + event.name);
    }
}

void EventBus::dispatchPending() {
    std::queue<Event> temp;
    {
        std::lock_guard<std::mutex> lock(mtx);
        if (isShuttingDown) {
            _logger.logInfo("Dispatch skipped (shutting down)");
            return;
        }
        std::swap(temp, eventQueue);
    }

    while (!temp.empty()) {
        const auto &event = temp.front();

        // Kopieer alleen callbacks
        std::vector<VoidCallback> callbacksCopy;
        {
            std::lock_guard<std::mutex> lock(mtx);
            auto it = listeners.find(event.name);
            if (it != listeners.end()) {
                callbacksCopy.reserve(it->second.size());
                for (const auto &listener: it->second) {
                    callbacksCopy.push_back(listener.cb);
                }
                _logger.logInfo("Processing event: " + event.name
                        + " (" + std::to_string(callbacksCopy.size()) + " listener(s))");
            } else {
                _logger.logWarning("No listeners for event: " + event.name);
            }
        }

        // Roep callbacks aan zonder lock
        for (size_t i = 0; i < callbacksCopy.size(); ++i) {
            try {
                callbacksCopy[i](event);
            } catch (const std::exception &e) {
                _logger.logError("In listener " + std::to_string(i + 1)
                        + " for event " + event.name + ": "
                        + e.what());
            } catch (...) {
                _logger.logError("UNKNOWN ERROR in listener " + std::to_string(i + 1)
                        + " for event " + event.name);
            }
        }

        temp.pop();
    }
}

void EventBus::shutdown() {
    std::lock_guard<std::mutex> lock(mtx);
    isShuttingDown = true;

    _logger.logInfo("Shutting down (clearing " + std::to_string(eventQueue.size()) + " queued events)");

    while (!eventQueue.empty()) {
        eventQueue.pop();
    }

    listeners.clear();
}

bool EventBus::isActive() const {
    std::lock_guard<std::mutex> lock(mtx);
    return !isShuttingDown;
}