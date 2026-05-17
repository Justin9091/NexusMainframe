#include "services/WsService.hpp"
#include "Event/EventBus.hpp"

#include <nlohmann/json.hpp>
#include <iostream>

using json = nlohmann::json;

WsService::WsService(MqttService& mqttSvc, int port)
    : _mqttSvc(mqttSvc), _port(port) {}

void WsService::start() {
    _server.onMessage([this](int id, const std::string& msg){ onWsMessage(id, msg); });
    _server.onClose  ([this](int id)                        { onWsClose(id); });

    // Bridge: incoming MQTT messages → WebSocket clients
    EventBus::getInstance().subscribe("mqtt:message", [this](const Event& event) {
        try {
            auto& [topic, payload] = std::any_cast<const std::pair<std::string,std::string>&>(event.data);

            json frame;
            frame["type"]    = "message";
            frame["topic"]   = topic;
            frame["payload"] = payload;
            std::string text = frame.dump();

            std::lock_guard lock(_mu);
            for (auto& [filter, clients] : _filterClients) {
                if (!topicMatches(filter, topic)) continue;
                for (int clientId : clients)
                    _server.send(clientId, text);
            }
        } catch (...) {}
    });

    _server.start(_port);
}

void WsService::stop() {
    _server.stop();
}

void WsService::onWsMessage(int clientId, const std::string& raw) {
    json msg;
    try { msg = json::parse(raw); } catch (...) { return; }

    std::string type = msg.value("type", "");

    if (type == "subscribe") {
        std::string topic = msg.value("topic", "");
        if (topic.empty()) return;
        _mqttSvc.getClient().subscribe(topic);
        std::lock_guard lock(_mu);
        _clientSubs[clientId].insert(topic);
        _filterClients[topic].insert(clientId);
    }
    else if (type == "unsubscribe") {
        std::string topic = msg.value("topic", "");
        if (topic.empty()) return;
        std::lock_guard lock(_mu);
        _clientSubs[clientId].erase(topic);
        _filterClients[topic].erase(clientId);
        if (_filterClients[topic].empty()) _filterClients.erase(topic);
    }
    else if (type == "publish") {
        std::string topic   = msg.value("topic",   "");
        std::string payload = msg.value("payload", "");
        if (!topic.empty()) _mqttSvc.getClient().publish(topic, payload);
    }
}

void WsService::onWsClose(int clientId) {
    std::lock_guard lock(_mu);
    auto it = _clientSubs.find(clientId);
    if (it == _clientSubs.end()) return;
    for (const auto& f : it->second) {
        _filterClients[f].erase(clientId);
        if (_filterClients[f].empty()) _filterClients.erase(f);
    }
    _clientSubs.erase(clientId);
}

bool WsService::topicMatches(const std::string& filter, const std::string& topic) const {
    size_t fi=0, ti=0;
    while (fi<filter.size() && ti<topic.size()) {
        if (filter[fi]=='#') return true;
        if (filter[fi]=='+') {
            while (ti<topic.size()&&topic[ti]!='/') ti++;
            fi++;
        } else if (filter[fi]==topic[ti]) {
            fi++; ti++;
        } else {
            return false;
        }
    }
    if (fi<filter.size()&&filter[fi]=='/'&&fi+1<filter.size()&&filter[fi+1]=='#')
        return true;
    return fi==filter.size()&&ti==topic.size();
}
