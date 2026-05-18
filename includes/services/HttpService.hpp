#pragma once
#include <memory>
#include "IService.hpp"
#include "server/HttpServer.hpp"
#include "Scheduler/Scheduler.hpp"
#include "services/ModuleService.hpp"
#include "services/MqttService.hpp"
#include "http/controllers/ModuleController.hpp"
#include "http/controllers/DeviceController.hpp"
#include "http/controllers/RoomController.hpp"
#include "http/controllers/ConfigController.hpp"
#include "services/DeviceService.hpp"
#include "services/RoomService.hpp"
#include "config/NexusConfig.hpp"

/**
 * @brief IService that owns and drives the HttpServer and all HTTP controllers.
 *
 * Wires together ModuleController, DeviceController, RoomController, and
 * ConfigController with their respective services, then starts the HTTP server.
 * McpService depends on this service to obtain the route list.
 */
class HttpService : public IService {
public:
    /**
     * @param moduleSvc  Module management service.
     * @param mqttSvc    MQTT service (used by health endpoint).
     * @param scheduler  Scheduler reference.
     * @param deviceSvc  Device service (forwarded to DeviceController).
     * @param roomSvc    Room service (forwarded to RoomController).
     * @param config     Application configuration (forwarded to ConfigController).
     * @param port       TCP port to listen on (default 8080).
     */
    HttpService(ModuleService& moduleSvc, MqttService& mqttSvc, Scheduler& scheduler,
                DeviceService& deviceSvc, RoomService& roomSvc,
                NexusConfig& config, int port = 8080);

    std::string_view getName() const override { return "HttpService"; }

    /** @brief Constructs the HttpServer and starts listening. */
    void start() override;

    /** @brief Stops the HttpServer. */
    void stop() override;

    /**
     * @brief Returns a non-owning pointer to the underlying HttpServer.
     * Used by McpService to retrieve the registered route list.
     */
    [[nodiscard]] HttpServer* getServer() const noexcept { return _server.get(); }

private:
    ModuleService& _moduleSvc;
    MqttService&   _mqttSvc;
    Scheduler&     _scheduler;
    int            _port;
    ModuleController _moduleCtrl;   // declared before _server: outlives it
    DeviceController _deviceCtrl;
    RoomController   _roomCtrl;
    ConfigController _configCtrl;
    std::unique_ptr<HttpServer> _server;
};
