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

/**
 * @brief Payload carried by every event on the EventBus.
 *
 * The @p data field can hold any type via std::any.
 * Subscribers cast it to the expected concrete type.
 */
struct Event {
    std::string name; ///< Unique event identifier, e.g. "mqtt.message".
    std::any    data; ///< Arbitrary payload; cast with std::any_cast<T>.
};

/**
 * @brief Thread-safe, singleton publish/subscribe event bus.
 *
 * Events are published synchronously into an internal queue and dispatched
 * via dispatchPending(), which must be called from the main loop.
 *
 * @code
 * auto& bus = EventBus::getInstance();
 * int id = bus.subscribe("mqtt.message", [](const Event& e) {
 *     auto msg = std::any_cast<std::string>(e.data);
 * });
 * bus.publish({"mqtt.message", std::string{"hello"}});
 * bus.dispatchPending(); // delivers to subscribers
 * bus.unsubscribe("mqtt.message", id);
 * @endcode
 *
 * @note All public methods are guarded by an internal mutex and safe to call
 *       from multiple threads.
 */
class EventBus {
public:
    using VoidCallback   = std::function<void(const Event&)>;   ///< Fire-and-forget handler.
    using ResultCallback = std::function<Result(const Event&)>; ///< Handler that returns a Result.

    /// Returns the process-wide singleton instance.
    static EventBus& getInstance() {
        static EventBus instance;
        return instance;
    }

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

    /**
     * @brief Subscribes a void callback to a named event.
     * @param eventName  Event identifier to listen for.
     * @param cb         Callback invoked on dispatchPending().
     * @return Subscription ID; pass to unsubscribe() to cancel.
     */
    int subscribe(const std::string& eventName, VoidCallback cb);

    /**
     * @brief Subscribes a result-returning callback to a named event.
     * @param eventName  Event identifier to listen for.
     * @param cb         Callback invoked on dispatchPending(); return value logged.
     * @return Subscription ID; pass to unsubscribe() to cancel.
     */
    int subscribe(const std::string& eventName, ResultCallback cb);

    /**
     * @brief Removes a previously registered subscription.
     * @param eventName  Event the subscription was registered for.
     * @param id         ID returned by subscribe().
     */
    void unsubscribe(const std::string& eventName, int id);

    /**
     * @brief Enqueues an event for delivery on the next dispatchPending() call.
     * @param event  Event to publish; copied into the internal queue.
     */
    void publish(const Event& event);

    /**
     * @brief Delivers all queued events to their subscribers.
     * Call this once per main-loop iteration from the owning thread.
     */
    void dispatchPending();

    /**
     * @brief Signals the bus to stop accepting new events.
     * publish() calls after shutdown() are silently dropped.
     */
    void shutdown();

    /// Returns @c true as long as shutdown() has not been called.
    bool isActive() const;

private:
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