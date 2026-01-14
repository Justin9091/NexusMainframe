#include "NexusMainFrame.hpp"
#include <iostream>
#include <thread>
#include <sstream>

#include "commands/Command.hpp"
#include "commands/CommandRegistry.hpp"
#include "commands/EnableModuleCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/ListCommand.hpp"


void NexusMainFrame::start() {
    _moduleLoader = std::make_unique<ModuleLoader>();
    _moduleLoader->loadModulesFromDirectory("./modules");

    for (auto &m: _moduleLoader->getLoadedModules()) {
        m->initialize(_eventBus);
    }

    CommandRegistry& registry = CommandRegistry::getInstance();
    registry.registerCommand("help", std::make_unique<HelpCommand>());
    registry.registerCommand("list", std::make_unique<ListCommand>(_moduleLoader.get()));
    registry.registerCommand("enable-module", std::make_unique<EnableModuleCommand>(_moduleLoader.get()));

    _mqttClient = std::make_unique<MQTTClient>(_eventBus, "nexus-core", "192.168.2.161", 1883);

    if (_mqttClient->connect()) {
        _mqttClient->subscribe("event");
    }

    // Add event listener voor MQTT temperature data
    _eventBus.subscribe("mqtt:event", [this](const Event &event) {
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

        _eventBus.publish(newEvent);
    });

    _server.start();
}

void NexusMainFrame::stop() {
    _eventBus.shutdown();

    if (_mqttClient) {
        _mqttClient->disconnect();
        _mqttClient.reset();
    }

    for (auto &m: _moduleLoader->getLoadedModules())
        m->shutdown();

    _server.stop();
}

void NexusMainFrame::run() {
    start();

    while (_running.load()) {
        // if (!_mqttClient->isConnected()) _mqttClient->connect();

        _eventBus.dispatchPending();
        _scheduler.tick();

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    stop();
}
