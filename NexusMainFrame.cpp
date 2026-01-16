#include "NexusMainFrame.hpp"
#include <iostream>
#include <thread>
#include <sstream>

#include "commands/Command.hpp"
#include "commands/CommandRegistry.hpp"
#include "commands/DisableModuleCommand.hpp"
#include "commands/EnableModuleCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/ListCommand.hpp"
#include "commands/ScanCommand.hpp"
#include "manifest/Manifest.hpp"


void NexusMainFrame::start() {
    _manifest = std::make_unique<Manifest>();
    _manifest->load();
    _manifest->listen();

    _moduleManager = std::make_unique<ModuleManager>();


    for (std::string enabled : _manifest->getEnabled()) {
        _moduleManager->load(enabled);
    }

    CommandRegistry& registry = CommandRegistry::getInstance();
    registry.registerCommand("help", std::make_unique<HelpCommand>());
    registry.registerCommand("list", std::make_unique<ListCommand>(*_moduleManager));
    registry.registerCommand("enable-module", std::make_unique<EnableModuleCommand>(*_moduleManager));
    registry.registerCommand("disable-module", std::make_unique<DisableModuleCommand>(*_moduleManager));
    registry.registerCommand("scan", std::make_unique<ScanCommand>());

    _mqttClient = std::make_unique<MQTTClient>(EventBus::getInstance(), "nexus-core", "192.168.2.161", 1883);

    // if (_mqttClient->connect()) {
        // _mqttClient->subscribe("event");
    // }

    // Add event listener voor MQTT temperature data
    EventBus::getInstance().subscribe("mqtt:event", [this](const Event &event) {
        std::any input = event.data;
        std::string name;
        std::string data;

        if (input.type() == typeid(std::string)) {
            std::string str = std::any_cast<std::string>(input);

            auto pos = str.find(' ');
            if (pos != std::string::npos) {
                name = str.substr(0, pos); // alles voor de spatie
                data = str.substr(pos + 1); // alles na de spatie
            } else {
                name = str; // geen spatie, alles is name
                data = ""; // data leeg
            }
        }

        Event newEvent;
        newEvent.name = name;
        newEvent.data = data;

        EventBus::getInstance().publish(newEvent);
    });

    _server.start();
}

void NexusMainFrame::stop() {
    EventBus::getInstance().shutdown();

    if (_mqttClient) {
        _mqttClient->disconnect();
        _mqttClient.reset();
    }

    for (auto &m: _moduleManager->getModules()) {
        m.instance->shutdown();
    }

    _server.stop();
}

void NexusMainFrame::run() {
    start();

    while (_running.load()) {
        // if (!_mqttClient->isConnected()) _mqttClient->connect();

        EventBus::getInstance().dispatchPending();
        _scheduler.tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    stop();
}
