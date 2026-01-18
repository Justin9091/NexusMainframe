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
#include "Result/Result.hpp"

struct Event {
    std::string name;
    std::any data;
};

class EventBus {
public:
    using VoidCallback = std::function<void(const Event&)>;
    using ResultCallback = std::function<Result(const Event&)>;

    // Singleton access
    static EventBus& getInstance() {
        static EventBus instance;
        return instance;
    }

    // Delete copy constructor and assignment operator
    EventBus(const EventBus&) = delete;
    EventBus& operator=(const EventBus&) = delete;

    int subscribe(const std::string& eventName, VoidCallback cb);
    int subscribe(const std::string& eventName, ResultCallback cb);
    void unsubscribe(const std::string& eventName, int id);
    void publish(const Event& event);
    void dispatchPending();
    void shutdown();
    bool isActive() const;

private:
    // Private constructor
    EventBus() : _logger{"EventBus"} {}

    struct Listener {
        int id;
        VoidCallback cb;
    };

    Logger _logger;
    mutable std::mutex mtx;
    std::map<std::string, std::vector<Listener>> listeners;
    std::queue<Event> eventQueue;
    int nextId = 1;
    std::atomic<bool> isShuttingDown{false};
};

#endif