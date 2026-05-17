#pragma once
#include <mutex>
#include <set>
#include <string>
#include <unordered_map>

#include "IService.hpp"
#include "server/WsServer.hpp"
#include "services/MqttService.hpp"

class WsService : public IService {
public:
    explicit WsService(MqttService& mqttSvc, int port = 8082);

    std::string_view getName() const override { return "WsService"; }
    void start()  override;
    void stop()   override;
    void update() override {}

private:
    bool topicMatches(const std::string& filter, const std::string& topic) const;
    void onWsMessage(int clientId, const std::string& raw);
    void onWsClose(int clientId);

    MqttService& _mqttSvc;
    int         _port;
    WsServer    _server;

    std::mutex _mu;
    std::unordered_map<int, std::set<std::string>> _clientSubs;  // clientId → filters
    std::unordered_map<std::string, std::set<int>> _filterClients; // filter → clientIds
};
