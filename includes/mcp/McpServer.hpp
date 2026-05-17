#pragma once
#include <httplib.h>
#include <nlohmann/json.hpp>
#include <thread>
#include <atomic>
#include <vector>
#include <string>

#include "server/Endpoint.hpp"

/**
 * @brief MCP server using the Streamable HTTP transport (spec 2025-03-26).
 *
 * Exposes each Endpoint registered with the HttpServer as an MCP tool.
 * Tool schemas are generated from Endpoint::buildInputSchema(); tool calls
 * are proxied to the REST API at @p apiBase.
 *
 * Listens on @p port (default 9998).  Clients connect with any MCP-compatible
 * agent (e.g. Claude Desktop) and can invoke Nexus REST endpoints as tools.
 *
 * @code
 * McpServer mcp(httpService.getServer()->getRoutes(), "http://localhost:8080");
 * mcp.start(9998);
 * @endcode
 */
class McpServer {
public:
    /**
     * @param routes   Read-only reference to the registered HTTP endpoints.
     * @param apiBase  Base URL of the Nexus REST API (default @c "http://localhost:8080").
     */
    McpServer(const std::vector<Endpoint>& routes, std::string apiBase = "http://localhost:8080");

    /**
     * @brief Starts the MCP HTTP server on a background thread.
     * @param port TCP port to listen on (default 9998).
     */
    void start(int port = 9998);

    /** @brief Stops the MCP server and joins the background thread. */
    void stop();

private:
    nlohmann::json dispatch(const nlohmann::json& req);

    nlohmann::json handleInitialize(const nlohmann::json& id);
    nlohmann::json handleToolsList(const nlohmann::json& id);
    nlohmann::json handleToolsCall(const nlohmann::json& params, const nlohmann::json& id);

    nlohmann::json callEndpoint(const Endpoint& ep, const nlohmann::json& args);

    static std::string     buildUrl (const Endpoint& ep, const nlohmann::json& args);
    static nlohmann::json  buildBody(const Endpoint& ep, const nlohmann::json& args);

    static nlohmann::json ok  (const nlohmann::json& id, nlohmann::json result);
    static nlohmann::json err (const nlohmann::json& id, int code, const std::string& msg);
    static nlohmann::json text(const std::string& content);

    const std::vector<Endpoint>& _routes;
    std::string                  _apiBase;
    httplib::Server              _srv;
    std::thread                  _thread;
    std::atomic<bool>            _running{false};
};
