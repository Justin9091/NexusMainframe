#include "services/MqttService.hpp"
#include "event/EventBus.hpp"
#include <any>
#include <iostream>

MqttService::MqttService(std::string clientId, std::string host, int port)
    : _clientId(std::move(clientId)), _host(std::move(host)), _port(port) {}

void MqttService::start() {
    _client = std::make_unique<MQTTClient>(EventBus::getInstance(), _clientId, _host, _port);
    _client->connect();

    EventBus::getInstance().subscribe("mqtt:event", [](const Event& event) {
        try {
            if (event.data.type() != typeid(std::string)) return;

            const auto& str = std::any_cast<const std::string&>(event.data);
            auto pos = str.find(' ');

            Event forwarded;
            forwarded.name = (pos != std::string::npos) ? str.substr(0, pos) : str;
            forwarded.data = (pos != std::string::npos) ? str.substr(pos + 1) : std::string{};
            EventBus::getInstance().publish(forwarded);
        } catch (const std::exception& e) {
            std::cerr << "[MqttService] " << e.what() << "\n";
        }
    });

    EventBus::getInstance().subscribe("mqtt:subscribe", [this](const Event& event) {
        try {
            if (event.data.type() != typeid(std::string)) return;
            const auto& topic = std::any_cast<const std::string&>(event.data);
            if (_client) _client->subscribe(topic);
        } catch (const std::exception& e) {
            std::cerr << "[MqttService] mqtt:subscribe error: " << e.what() << "\n";
        }
    });
}

void MqttService::stop() {
    if (_client) _client->disconnect();
}
