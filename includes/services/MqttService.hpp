#pragma once
#include <memory>
#include <string>
#include "IService.hpp"
#include "mqtt/MQTTClient.hpp"

class MqttService : public IService {
public:
    MqttService(std::string clientId, std::string host, int port);

    std::string_view getName() const override { return "MqttService"; }
    void start() override;
    void stop() override;

    MQTTClient& getClient() { return *_client; }

private:
    std::string _clientId;
    std::string _host;
    int         _port;
    std::unique_ptr<MQTTClient> _client;
};
