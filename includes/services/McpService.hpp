#pragma once
#include <memory>
#include "IService.hpp"
#include "services/HttpService.hpp"
#include "mcp/McpServer.hpp"

/**
 * @brief IService wrapper around McpServer.
 *
 * Retrieves the registered routes from HttpService and starts an McpServer
 * that exposes them as MCP tools.  Must be added after HttpService.
 */
class McpService : public IService {
public:
    /**
     * @param httpSvc  HttpService to pull the route list from.
     * @param port     TCP port for the MCP server (default 9998).
     */
    explicit McpService(HttpService& httpSvc, int port = 9998);

    std::string_view getName() const override { return "McpService"; }

    /** @brief Constructs and starts the McpServer. */
    void start() override;

    /** @brief Stops the McpServer. */
    void stop() override;

private:
    HttpService&               _httpSvc;
    int                        _port;
    std::unique_ptr<McpServer> _server;
};
