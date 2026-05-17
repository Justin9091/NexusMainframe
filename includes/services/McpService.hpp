#pragma once
#include <memory>
#include "IService.hpp"
#include "services/HttpService.hpp"
#include "mcp/McpServer.hpp"

class McpService : public IService {
public:
    explicit McpService(HttpService& httpSvc, int port = 9998);

    std::string_view getName() const override { return "McpService"; }
    void start() override;
    void stop() override;

private:
    HttpService& _httpSvc;
    int          _port;
    std::unique_ptr<McpServer> _server;
};
