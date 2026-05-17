#pragma once
#include <vector>
#include "http/RouteEntry.hpp"

class HttpServer;

class IController {
public:
    virtual ~IController() = default;

    void registerRoutes(HttpServer& server);

protected:
    virtual std::vector<RouteEntry> routeTable() = 0;
};
