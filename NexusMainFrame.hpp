#ifndef NEXUSMAINFRAME_HPP
#define NEXUSMAINFRAME_HPP

#include <memory>
#include <atomic>

#include "IPC/CommandServer.hpp"
#include "Modules/ModuleManager.hpp"
#include "mqtt/MQTTClient.hpp"
#include "Scheduler/Scheduler.hpp"
#include "server/HttpServer.hpp"

class NexusMainFrame {
private:
    std::unique_ptr<ModuleManager> _moduleManager;
    Scheduler _scheduler;
    std::unique_ptr<HttpServer> _httpServer;
    std::unique_ptr<MQTTClient> _mqttClient;

public:
    std::thread _mainThread;
    std::atomic<bool> _running{true};

    void run();
    void start();
    void stop();
};

#endif
