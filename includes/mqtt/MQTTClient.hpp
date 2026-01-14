//
// Created by jusra on 12-1-2026.
//

#ifndef NEXUSCORE_MQTTCLIENT_HPP
#define NEXUSCORE_MQTTCLIENT_HPP

#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include "../Event/EventBus.hpp"

class MQTTClient {
public:
    MQTTClient(
        EventBus& bus,
        std::string clientId,
        std::string host,
        int port = 1883
    );

    ~MQTTClient();

    bool connect();
    bool isConnected();
    void disconnect();

    bool subscribe(const std::string& topic);
    bool publish(const std::string& topic, const std::string& payload);

private:
    void receiveLoop();
    void handlePacket(const std::vector<uint8_t>& data);

    bool sendPacket(const std::vector<uint8_t>& packet);
    std::vector<uint8_t> encodeString(const std::string& s);

private:
    EventBus& eventBus;
    std::string clientId;
    std::string host;
    int port;
    bool connected = false;

    int sock{-1};
    std::thread rxThread;
    std::atomic<bool> running{false};
};

#endif //NEXUSCORE_MQTTCLIENT_HPP