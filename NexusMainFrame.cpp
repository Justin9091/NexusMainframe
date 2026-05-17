#include "NexusMainFrame.hpp"
#include "pathing/PathManager.hpp"
#include "Event/EventBus.hpp"
#include "services/ModuleService.hpp"
#include "services/MqttService.hpp"
#include "services/HttpService.hpp"
#include "services/WsService.hpp"
#include "services/McpService.hpp"
#include "services/LogService.hpp"
#include "services/DeviceService.hpp"
#include "services/RoomService.hpp"
#include "config/NexusConfig.hpp"
#include <iostream>
#include <thread>

void NexusMainFrame::addService(std::unique_ptr<IService> service) {
    _services.push_back(std::move(service));
}

void NexusMainFrame::run() {
    initPaths();
    registerBuiltinServices();
    startAll();

    _running = true;
    while (_running.load()) {
        for (auto& svc : _services) {
            try { svc->update(); }
            catch (const std::exception& e) { std::cerr << "[" << svc->getName() << "::update] " << e.what() << "\n"; }
            catch (...) { std::cerr << "[" << svc->getName() << "::update] unknown exception\n"; }
        }

        try { EventBus::getInstance().dispatchPending(); }
        catch (const std::exception& e) { std::cerr << "[EventBus] " << e.what() << "\n"; }

        try { _scheduler.tick(); }
        catch (const std::exception& e) { std::cerr << "[Scheduler] " << e.what() << "\n"; }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    stopAll();
}

void NexusMainFrame::stop() {
    _running = false;
}

void NexusMainFrame::initPaths() {
    auto& paths = PathManager::getInstance();
    paths.ensureExists("modules.builtin");
    paths.ensureExists("modules.downloaded");
    paths.ensureExists("config");
    paths.ensureExists("cache.downloads");
    paths.ensureExists("logs");
    paths.ensureExists("data.scans");
    paths.registerPath("modules.registry", "modules/available-modules.json");
    paths.registerPath("data.devices",  "data/devices.json");
    paths.registerPath("data.rooms",    "data/rooms.json");
    paths.registerPath("config.nexus",  "config/nexus.json");
}

void NexusMainFrame::registerBuiltinServices() {
    _services.push_back(std::make_unique<LogService>());

    _config = NexusConfig::loadOrCreate(PathManager::getInstance().get("config.nexus"));
    std::cout << "[NexusMainFrame] Config loaded — MQTT: " << _config.mqtt.host << ":" << _config.mqtt.port
              << " | HTTP: " << _config.http.port << " | WS: " << _config.ws.port << "\n";

    auto moduleSvc = std::make_unique<ModuleService>();
    auto mqttSvc   = std::make_unique<MqttService>(_config.mqtt.clientId, _config.mqtt.host, _config.mqtt.port);
    auto deviceSvc = std::make_unique<DeviceService>();
    auto roomSvc   = std::make_unique<RoomService>(*deviceSvc);

    ModuleService& moduleRef = *moduleSvc;
    MqttService&   mqttRef   = *mqttSvc;
    DeviceService& deviceRef = *deviceSvc;
    RoomService&   roomRef   = *roomSvc;

    auto httpSvc = std::make_unique<HttpService>(moduleRef, mqttRef, _scheduler, deviceRef, roomRef, _config, _config.http.port);
    HttpService& httpRef = *httpSvc;

    auto mcpSvc  = std::make_unique<McpService>(httpRef);
    auto wsSvc   = std::make_unique<WsService>(mqttRef, _config.ws.port);

    _services.push_back(std::move(moduleSvc));
    _services.push_back(std::move(mqttSvc));
    _services.push_back(std::move(deviceSvc));
    _services.push_back(std::move(roomSvc));
    _services.push_back(std::move(httpSvc));
    _services.push_back(std::move(mcpSvc));
    _services.push_back(std::move(wsSvc));
}

void NexusMainFrame::startAll() {
    for (auto& svc : _services) {
        try { svc->start(); }
        catch (const std::exception& e) { std::cerr << "[" << svc->getName() << "::start] " << e.what() << "\n"; throw; }
    }
}

void NexusMainFrame::stopAll() {
    EventBus::getInstance().shutdown();
    for (auto it = _services.rbegin(); it != _services.rend(); ++it) {
        try { (*it)->stop(); }
        catch (const std::exception& e) { std::cerr << "[" << (*it)->getName() << "::stop] " << e.what() << "\n"; }
    }
}
