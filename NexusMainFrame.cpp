#include <iostream>
#include <thread>
#include <exception>

#include "NexusMainFrame.hpp"
#include "pathing/PathManager.hpp"
#include "commands/CommandRegistry.hpp"
#include "commands/DisableModuleCommand.hpp"
#include "commands/DownloadCommand.hpp"
#include "commands/EnableModuleCommand.hpp"
#include "commands/HelpCommand.hpp"
#include "commands/ListCommand.hpp"
#include "commands/MonitorCommand.hpp"
#include "commands/ScanCommand.hpp"
#include "Modules/ModuleManager.hpp"
#include "mqtt/MQTTClient.hpp"
#include "event/EventBus.hpp"

void NexusMainFrame::start() {
    try {
        auto& paths = PathManager::getInstance();
        paths.ensureExists("modules.builtin");
        paths.ensureExists("modules.downloaded");
        paths.ensureExists("config");
        paths.ensureExists("cache.downloads");
        paths.ensureExists("logs");
        paths.ensureExists("data.scans");

        paths.registerPath("modules.registry", "modules/available-modules.json");

        _moduleManager = std::make_unique<ModuleManager>();

        // Register commands
        CommandRegistry& registry = CommandRegistry::getInstance();
        registry.registerCommand("help", std::make_unique<HelpCommand>());
        registry.registerCommand("list", std::make_unique<ListCommand>(*_moduleManager));
        registry.registerCommand("enable-module", std::make_unique<EnableModuleCommand>(*_moduleManager));
        registry.registerCommand("disable-module", std::make_unique<DisableModuleCommand>(*_moduleManager));
        registry.registerCommand("scan", std::make_unique<ScanCommand>());
        registry.registerCommand("monitor", std::make_unique<MonitorCommand>());
        registry.registerCommand("download", std::make_unique<DownloadCommand>());

        // MQTT Client
        _mqttClient = std::make_unique<MQTTClient>(EventBus::getInstance(), "nexus-core", "192.168.2.161", 1883);

        // Wrap MQTT event subscription
        EventBus::getInstance().subscribe("mqtt:event", [this](const Event &event) {
            try {
                std::any input = event.data;
                std::string name;
                std::string data;

                if (input.type() == typeid(std::string)) {
                    std::string str = std::any_cast<std::string>(input);
                    auto pos = str.find(' ');
                    if (pos != std::string::npos) {
                        name = str.substr(0, pos);
                        data = str.substr(pos + 1);
                    } else {
                        name = str;
                        data = "";
                    }
                }

                Event newEvent;
                newEvent.name = name;
                newEvent.data = data;
                EventBus::getInstance().publish(newEvent);
            } catch(const std::exception& e) {
                std::cerr << "[MQTT Event Callback Exception] " << e.what() << "\n";
            } catch(...) {
                std::cerr << "[MQTT Event Callback Exception] Unknown exception\n";
            }
        });

        // Start server (ZONDER aparte thread!)
        _server.start();
        _running = true;

    } catch(const std::exception& e) {
        std::cerr << "[Start Exception] " << e.what() << "\n";
        throw;
    } catch(...) {
        std::cerr << "[Start Exception] Unknown exception\n";
        throw;
    }
}

void NexusMainFrame::run() {
    try {
        start();

        // Event loop
        while (_running.load()) {
            try {
                EventBus::getInstance().dispatchPending();
            } catch(const std::exception& e) {
                std::cerr << "[Run EventBus Exception] " << e.what() << "\n";
            } catch(...) {
                std::cerr << "[Run EventBus Exception] Unknown exception\n";
            }

            try {
                _scheduler.tick();
            } catch(const std::exception& e) {
                std::cerr << "[Run Scheduler Exception] " << e.what() << "\n";
            } catch(...) {
                std::cerr << "[Run Scheduler Exception] Unknown exception\n";
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

    } catch(const std::exception& e) {
        std::cerr << "[Run Exception] " << e.what() << "\n";
    } catch(...) {
        std::cerr << "[Run Exception] Unknown exception\n";
    }

    stop();
}

void NexusMainFrame::stop() {
    try {
        _running = false;

        EventBus::getInstance().shutdown();

        if (_mqttClient) {
            try {
                _mqttClient->disconnect();
            } catch(const std::exception& e) {
                std::cerr << "[MQTT Disconnect Exception] " << e.what() << "\n";
            } catch(...) {
                std::cerr << "[MQTT Disconnect Exception] Unknown exception\n";
            }
            _mqttClient.reset();
        }

        if (_moduleManager) {
            for (auto &m: _moduleManager->getModules()) {
                try {
                    m.instance->shutdown();
                } catch(const std::exception& e) {
                    std::cerr << "[Module Shutdown Exception] " << e.what() << "\n";
                } catch(...) {
                    std::cerr << "[Module Shutdown Exception] Unknown exception\n";
                }
            }
        }

        _server.stop();
    } catch(const std::exception& e) {
        std::cerr << "[Stop Exception] " << e.what() << "\n";
    } catch(...) {
        std::cerr << "[Stop Exception] Unknown exception\n";
    }
}
