#pragma once
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

#include "IService.hpp"
#include "server/WsServer.hpp"
#include "services/MqttService.hpp"

/**
 * @brief IService that bridges MQTT topics to WebSocket clients.
 *
 * Clients connect on the configured port and send JSON subscription commands
 * to select which MQTT topics they want to receive.  When a matching MQTT
 * message arrives on the EventBus, WsService forwards it as a JSON frame to
 * all subscribed WebSocket clients.
 *
 * WebSocket message protocol (client → server):
 * @code
 * {"action":"subscribe","topic":"home/sensors/#"}
 * {"action":"unsubscribe","topic":"home/sensors/#"}
 * @endcode
 *
 * Frame sent to clients on matching MQTT message:
 * @code
 * {"topic":"home/sensors/temp","payload":"23.5"}
 * @endcode
 */
class WsService : public IService {
public:
    /**
     * @param mqttSvc  MQTT service; WsService subscribes to its EventBus events.
     * @param port     TCP port for the WebSocket server (default 8082).
     */
    explicit WsService(MqttService& mqttSvc, int port = 8082);

    std::string_view getName() const override { return "WsService"; }

    /** @brief Starts the WebSocket server and connects to the EventBus. */
    void start() override;

    /** @brief Stops the WebSocket server and unregisters EventBus subscriptions. */
    void stop() override;

    void update() override {}

private:
    bool topicMatches(const std::string& filter, const std::string& topic) const;
    void onWsMessage(int clientId, const std::string& raw);
    void onWsClose(int clientId);

    MqttService& _mqttSvc;
    int          _port;
    WsServer     _server;

    std::mutex                                     _mu;
    std::unordered_map<int, std::set<std::string>> _clientSubs;    ///< clientId → topic filters.
    std::unordered_map<std::string, std::set<int>> _filterClients; ///< filter → subscribed clientIds.
};
