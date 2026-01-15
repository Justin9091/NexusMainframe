#ifndef NEXUSMAINFRAME_HPP
#define NEXUSMAINFRAME_HPP

#include <memory>
#include <atomic>

#include "Event/EventBus.hpp"
#include "IPC/CommandServer.hpp"
#include "manifest/Manifest.hpp"
#include "Modules/ModuleLoader.hpp"
#include "mqtt/MQTTClient.hpp"
#include "Scheduler/Scheduler.hpp"

class NexusMainFrame {
private:
    std::unique_ptr<Manifest> _manifest = std::make_unique<Manifest>();
    std::unique_ptr<ModuleLoader> _moduleLoader;
    std::atomic<bool> _running{true};
    Scheduler _scheduler;
    CommandServer _server;
    std::unique_ptr<MQTTClient> _mqttClient;

public:
    void run();
    void start();
    void stop();
};

#endif
