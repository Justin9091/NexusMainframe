#pragma once
#include <vector>
#include "http/RouteEntry.hpp"

class HttpServer;

/**
 * @brief Base interface for HTTP route controllers.
 *
 * Controllers group related endpoints and register them with the HttpServer
 * via registerRoutes().  Concrete controllers should extend the CRTP helper
 * Controller<Derived> rather than implementing this interface directly.
 *
 * @see Controller
 */
class IController {
public:
    virtual ~IController() = default;

    /**
     * @brief Registers all routes from this controller with the given server.
     * Iterates routeTable() and calls HttpServer::addRoute() for each entry.
     * @param server  Server instance to register routes on.
     */
    void registerRoutes(HttpServer& server);

protected:
    /** @brief Returns the list of routes this controller exposes. */
    virtual std::vector<RouteEntry> routeTable() = 0;
};
