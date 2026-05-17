#pragma once
#include <memory>
#include <string>
#include "IService.hpp"
#include "mqtt/MQTTClient.hpp"

/**
 * @brief IService wrapper around MQTTClient.
 *
 * Creates the MQTTClient on start() using the supplied connection parameters
 * and exposes it to other services (e.g. WsService) via getClient().
 */
class MqttService : public IService {
public:
    /**
     * @param clientId  MQTT client identifier string.
     * @param host      Broker hostname or IP address.
     * @param port      Broker TCP port (typically 1883).
     */
    MqttService(std::string clientId, std::string host, int port);

    std::string_view getName() const override { return "MqttService"; }

    /** @brief Constructs and connects the MQTTClient. */
    void start() override;

    /** @brief Disconnects and destroys the MQTTClient. */
    void stop() override;

    /** @brief Returns a reference to the underlying MQTTClient. */
    MQTTClient& getClient() { return *_client; }

private:
    std::string                 _clientId;
    std::string                 _host;
    int                         _port;
    std::unique_ptr<MQTTClient> _client;
};
