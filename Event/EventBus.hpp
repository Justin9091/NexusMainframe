#ifndef EVENTBUS_HPP
#define EVENTBUS_HPP

#include <string>
#include <functional>
#include <map>
#include <vector>
#include <queue>
#include <mutex>
#include <any>
#include <atomic>

#include "Logger/Logger.hpp"

struct Event {
    std::string name;
    std::any data;
};

class EventBus {
public:
    using Callback = std::function<void(const Event&)>;

    int subscribe(const std::string& eventName, Callback cb);
    void unsubscribe(const std::string& eventName, int id);
    void publish(const Event& event);
    void dispatchPending();
    void shutdown();
    bool isActive() const;

private:
    struct Listener {
        int id;
        Callback cb;
    };

    Logger _logger{"EventBus"};
    mutable std::mutex mtx;
    std::map<std::string, std::vector<Listener>> listeners;
    std::queue<Event> eventQueue;
    int nextId = 1;
    std::atomic<bool> isShuttingDown{false};
};

#endif