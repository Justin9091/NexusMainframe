#include "services/HttpService.hpp"
#include "Event/EventBus.hpp"

HttpService::HttpService(ModuleService& moduleSvc, MqttService& mqttSvc, Scheduler& scheduler,
                         DeviceService& deviceSvc, RoomService& roomSvc,
                         NexusConfig& config, int port)
    : _moduleSvc(moduleSvc), _mqttSvc(mqttSvc), _scheduler(scheduler),
      _deviceSvc(deviceSvc), _roomSvc(roomSvc), _config(config), _port(port),
      _moduleCtrl(moduleSvc), _deviceCtrl(deviceSvc), _roomCtrl(roomSvc), _configCtrl(config) {}

void HttpService::start() {
    _server = std::make_unique<HttpServer>(
        _moduleSvc.getManager(),
        EventBus::getInstance(),
        _scheduler,
        _mqttSvc.getClient()
    );
    _server->use(_moduleCtrl);
    _server->use(_deviceCtrl);
    _server->use(_roomCtrl);
    _server->use(_configCtrl);
    _server->start(_port);
}

void HttpService::stop() {
    if (_server) _server->stop();
}
