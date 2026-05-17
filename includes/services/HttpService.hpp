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

class HttpService : public IService {
public:
    HttpService(ModuleService& moduleSvc, MqttService& mqttSvc, Scheduler& scheduler,
                DeviceService& deviceSvc, RoomService& roomSvc,
                NexusConfig& config, int port = 8080);

    std::string_view getName() const override { return "HttpService"; }
    void start() override;
    void stop() override;

    [[nodiscard]] HttpServer* getServer() const noexcept { return _server.get(); }

private:
    ModuleService& _moduleSvc;
    MqttService&   _mqttSvc;
    Scheduler&     _scheduler;
    DeviceService& _deviceSvc;
    RoomService&   _roomSvc;
    NexusConfig&   _config;
    int            _port;
    ModuleController _moduleCtrl;   // declared before _server: outlives it
    DeviceController _deviceCtrl;
    RoomController   _roomCtrl;
    ConfigController _configCtrl;
    std::unique_ptr<HttpServer> _server;
};
