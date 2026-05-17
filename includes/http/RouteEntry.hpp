#pragma once
#include <string>
#include "server/Endpoint.hpp"
#include "server/HttpMethod.hpp"

/**
 * @brief Immutable descriptor for a single HTTP route produced by a controller.
 *
 * Controllers return a list of RouteEntry objects from routeTable().
 * IController::registerRoutes() then converts each entry into an Endpoint
 * and registers it with the HttpServer.
 */
class RouteEntry {
public:
    /**
     * @param method       HTTP method (GET, POST, …).
     * @param path         URL path pattern (e.g. @c "/api/devices/:id").
     * @param name         Stable machine-readable identifier.
     * @param handler      Request handler callable.
     * @param description  Optional human-readable description.
     */
    RouteEntry(HttpMethod method, std::string path, std::string name,
               Endpoint::Handler handler, std::string description = {})
        : _method(method)
        , _path(std::move(path))
        , _name(std::move(name))
        , _handler(std::move(handler))
        , _description(std::move(description)) {}

    HttpMethod               method()      const { return _method; }      ///< HTTP method.
    const std::string&       path()        const { return _path; }        ///< URL path pattern.
    const std::string&       name()        const { return _name; }        ///< Stable route name.
    const Endpoint::Handler& handler()     const { return _handler; }     ///< Request handler.
    const std::string&       description() const { return _description; } ///< Route description.

private:
    HttpMethod        _method;
    std::string       _path;
    std::string       _name;
    Endpoint::Handler _handler;
    std::string       _description;
};
