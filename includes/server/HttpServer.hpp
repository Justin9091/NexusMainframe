#ifndef NEXUSCORE_HTTPSERVER_HPP
#define NEXUSCORE_HTTPSERVER_HPP

#include <httplib.h>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>

#include "Modules/ModuleManager.hpp"
#include "Event/EventBus.hpp"
#include "Scheduler/Scheduler.hpp"
#include "mqtt/MQTTClient.hpp"
#include "Endpoint.hpp"
#include "http/IController.hpp"

/**
 * @brief Embedded HTTP server exposing the Nexus REST API on a configurable port.
 *
 * Wraps cpp-httplib and runs on a dedicated background thread.  Routes are
 * registered through IController implementations (use()) or directly as
 * Endpoint objects (addRoute()).
 *
 * Built-in routes (registered by registerRoutes()):
 * - GET  /api/status   — liveness probe
 * - GET  /api/health   — uptime, MQTT state, loaded modules
 * - GET  /api/metrics  — detailed runtime metrics
 *
 * @code
 * HttpServer srv(modules, bus, scheduler, mqtt);
 * srv.use(myController);
 * srv.start(8080);
 * // ...
 * srv.stop();
 * @endcode
 */
class HttpServer {
public:
    /**
     * @param modules   Reference to the module manager (used by health/metrics).
     * @param eventBus  Reference to the event bus.
     * @param scheduler Reference to the cron scheduler.
     * @param mqtt      Reference to the MQTT client (used by health endpoint).
     */
    HttpServer(ModuleManager& modules,
               EventBus& eventBus,
               Scheduler& scheduler,
               MQTTClient& mqtt);

    /**
     * @brief Starts the HTTP server on the given port.
     * Launches a background thread; returns immediately.
     * @param port TCP port to listen on (default 8080).
     */
    void start(int port = 8080);

    /** @brief Stops the server and joins the background thread. */
    void stop();

    /**
     * @brief Registers all routes from a controller.
     * @param controller Controller whose routeTable() is added to the server.
     */
    void use(IController& controller);

    /**
     * @brief Registers a single endpoint.
     * @param ep Endpoint to add; duplicates are silently ignored.
     */
    void addRoute(Endpoint ep);

    /** @brief Returns the list of all registered endpoints (read-only). */
    [[nodiscard]] const std::vector<Endpoint>& getRoutes() const noexcept { return _routes; }

private:
    void registerRoutes();
    void bindRoutes();

    std::string toJson(bool success, const std::string& output);
    long long   uptimeSeconds() const;

    ModuleManager&   _modules;
    EventBus&        _eventBus;
    Scheduler&       _scheduler;
    MQTTClient&      _mqtt;

    httplib::Server                        _server;
    std::thread                            _serverThread;
    std::atomic<bool>                      _running{false};
    std::chrono::steady_clock::time_point  _startTime;

    std::vector<Endpoint>                  _routes;
};

#endif // NEXUSCORE_HTTPSERVER_HPP