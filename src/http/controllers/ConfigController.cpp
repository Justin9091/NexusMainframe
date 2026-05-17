#include "http/controllers/ConfigController.hpp"
#include "server/HttpMethod.hpp"
#include "pathing/PathManager.hpp"
#include <nlohmann/json.hpp>

ConfigController::ConfigController(NexusConfig& config)
    : _config(config) {}

std::vector<RouteEntry> ConfigController::routeTable() {
    using enum HttpMethod;
    return {
        route(&ConfigController::show,   GET,   "/api/config", "config_show",   "Return current configuration"),
        route(&ConfigController::update, PATCH, "/api/config", "config_update", "Update configuration and save to disk"),
    };
}

void ConfigController::show(const httplib::Request&, httplib::Response& res) {
    res.set_content(_config.toJson().dump(2), "application/json");
}

void ConfigController::update(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json body;
    try { body = nlohmann::json::parse(req.body); }
    catch (...) {
        res.status = 400;
        res.set_content(Result::badRequest("Invalid JSON body").toJson(), "application/json");
        return;
    }

    nlohmann::json restartRequired = nlohmann::json::array();

    if (body.contains("mqtt")) {
        auto& m = body["mqtt"];
        if (m.contains("host"))      { _config.mqtt.host     = m["host"];      restartRequired.push_back("mqtt.host"); }
        if (m.contains("port"))      { _config.mqtt.port     = m["port"];      restartRequired.push_back("mqtt.port"); }
        if (m.contains("client_id")) { _config.mqtt.clientId = m["client_id"]; restartRequired.push_back("mqtt.client_id"); }
    }
    if (body.contains("http")) {
        if (body["http"].contains("port")) { _config.http.port = body["http"]["port"]; restartRequired.push_back("http.port"); }
    }
    if (body.contains("ws")) {
        if (body["ws"].contains("port")) { _config.ws.port = body["ws"]["port"]; restartRequired.push_back("ws.port"); }
    }

    _config.save(PathManager::getInstance().get("config.nexus"));

    nlohmann::json response = {
        {"success", true},
        {"config",  _config.toJson()},
    };
    if (!restartRequired.empty())
        response["restart_required"] = restartRequired;

    res.set_content(response.dump(2), "application/json");
}
