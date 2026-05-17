#include "http/IController.hpp"
#include "server/HttpServer.hpp"

void IController::registerRoutes(HttpServer& server) {
    for (const auto& entry : routeTable()) {
        Endpoint ep(entry.path(), entry.name(), entry.method(), entry.handler());
        if (!entry.description().empty())
            ep.describe(entry.description());
        server.addRoute(std::move(ep));
    }
}
