#pragma once
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <atomic>
#include <vector>
#include <string>

#include "server/Endpoint.hpp"

// Generic MCP server (Streamable HTTP transport, 2025-03-26).
// Reads Endpoint metadata to expose tools and proxies tool calls to the HTTP API.
class McpServer {
public:
    McpServer(const std::vector<Endpoint>& routes, std::string apiBase = "http://localhost:8080");

    void start(int port = 9998);
    void stop();

private:
    nlohmann::json dispatch(const nlohmann::json& req);

    nlohmann::json handleInitialize(const nlohmann::json& id);
    nlohmann::json handleToolsList(const nlohmann::json& id);
    nlohmann::json handleToolsCall(const nlohmann::json& params, const nlohmann::json& id);

    // Resolve args → URL + body, then call the real HTTP API
    nlohmann::json callEndpoint(const Endpoint& ep, const nlohmann::json& args);

    static std::string buildUrl(const Endpoint& ep, const nlohmann::json& args);
    static nlohmann::json buildBody(const Endpoint& ep, const nlohmann::json& args);

    static nlohmann::json ok(const nlohmann::json& id, nlohmann::json result);
    static nlohmann::json err(const nlohmann::json& id, int code, const std::string& msg);
    static nlohmann::json text(const std::string& content);

    const std::vector<Endpoint>& _routes;
    std::string                  _apiBase;
    httplib::Server              _srv;
    std::thread                  _thread;
    std::atomic<bool>            _running{false};
};
