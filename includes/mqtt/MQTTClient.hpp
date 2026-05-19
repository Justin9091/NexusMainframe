//
// Created by jusra on 12-1-2026.
//

#ifndef NEXUSCORE_MQTTCLIENT_HPP
#define NEXUSCORE_MQTTCLIENT_HPP

#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include "../event/EventBus.hpp"

/**
 * @brief Lightweight MQTT 3.1.1 client with EventBus integration.
 *
 * Manages a TCP connection to an MQTT broker and runs two background threads:
 * one for receiving packets (receiveLoop()) and one for keep-alive PINGs
 * (pingLoop()).  Incoming PUBLISH packets are forwarded to the EventBus as
 * events named @c "mqtt.message".
 *
 * @code
 * MQTTClient client(bus, "nexus-core", "192.168.2.161");
 * if (client.connect()) {
 *     client.subscribe("home/sensors/#");
 *     client.publish("home/status", "online");
 * }
 * @endcode
 *
 * @note Only one active connection per instance is supported.
 */
class MQTTClient {
public:
    /**
     * @param bus      EventBus to which incoming messages are published.
     * @param clientId MQTT client identifier string.
     * @param host     Broker hostname or IP address.
     * @param port     Broker TCP port (default 1883).
     */
    MQTTClient(
        EventBus&   bus,
        std::string clientId,
        std::string host,
        int         port = 1883
    );

    ~MQTTClient();

    /**
     * @brief Opens a TCP connection and sends the MQTT CONNECT packet.
     * @return @c true on successful CONNACK, @c false on any error.
     */
    bool connect();

    /** @return @c true if the client currently has an active connection. */
    bool isConnected();

    /** @brief Sends DISCONNECT and closes the socket; stops background threads. */
    void disconnect();

    /**
     * @brief Subscribes to an MQTT topic filter.
     * @param topic  Topic filter string; supports wildcards + and #.
     * @return @c true if the SUBSCRIBE packet was sent successfully.
     */
    bool subscribe(const std::string& topic);

    /**
     * @brief Publishes a message to a topic (QoS 0).
     * @param topic   MQTT topic string.
     * @param payload UTF-8 message payload.
     * @return @c true if the PUBLISH packet was sent successfully.
     */
    bool publish(const std::string& topic, const std::string& payload);

private:
    void receiveLoop();
    void pingLoop();
    void handlePacket(const std::vector<uint8_t>& data);
    bool sendPacket(const std::vector<uint8_t>& packet);
    std::vector<uint8_t> encodeString(const std::string& s);

    EventBus&   eventBus;
    std::string clientId;
    std::string host;
    int         port;
    bool        connected = false;

    int                  sock{-1};
    std::thread          rxThread;
    std::thread          pingThread;
    std::atomic<bool>    running{false};
};

#endif //NEXUSCORE_MQTTCLIENT_HPP