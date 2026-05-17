#include "services/McpService.hpp"

McpService::McpService(HttpService& httpSvc, int port)
    : _httpSvc(httpSvc), _port(port) {}

void McpService::start() {
    auto* srv = _httpSvc.getServer();
    if (!srv) return;

    _server = std::make_unique<McpServer>(srv->getRoutes());
    _server->start(_port);
}

void McpService::stop() {
    if (_server) _server->stop();
}
