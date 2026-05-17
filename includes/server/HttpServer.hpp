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

class HttpServer {
public:
    HttpServer(ModuleManager& modules,
               EventBus& eventBus,
               Scheduler& scheduler,
               MQTTClient& mqtt);

    void start(int port = 8080);
    void stop();

    void use(IController& controller);
    void addRoute(Endpoint ep);

private:
    void registerRoutes();
    void bindRoutes();

    std::string toJson(bool success, const std::string& output);
    long long   uptimeSeconds() const;

public:
    [[nodiscard]] const std::vector<Endpoint>& getRoutes() const noexcept { return _routes; }

private:
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