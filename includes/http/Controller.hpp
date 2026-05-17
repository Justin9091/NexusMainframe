#pragma once
#include <string>
#include "http/IController.hpp"
#include "http/RouteEntry.hpp"
#include "server/HttpMethod.hpp"

/**
 * @brief CRTP base that provides the route() binding helper for controllers.
 *
 * Concrete controllers inherit @c Controller<Derived> and call route() inside
 * routeTable() to bind member functions to HTTP methods and paths without
 * boilerplate lambda captures.
 *
 * @tparam Derived  The concrete controller type (CRTP pattern).
 *
 * @code
 * class DeviceController : public Controller<DeviceController> {
 * protected:
 *     std::vector<RouteEntry> routeTable() override {
 *         return {
 *             route(&DeviceController::index, HttpMethod::GET, "/api/devices",
 *                   "devices.index", "List all devices"),
 *         };
 *     }
 *     void index(const httplib::Request& req, httplib::Response& res);
 * };
 * @endcode
 */
template<typename Derived>
class Controller : public IController {
protected:
    using HandlerFn = void (Derived::*)(const httplib::Request&, httplib::Response&);

    /**
     * @brief Creates a RouteEntry that binds a member function to an HTTP route.
     * @param fn          Pointer to the handler method in Derived.
     * @param method      HTTP method (GET, POST, …).
     * @param path        URL path pattern (e.g. @c "/api/devices/:id").
     * @param name        Stable identifier used by MCP and logs.
     * @param description Human-readable description exposed via MCP.
     * @return Configured RouteEntry ready for routeTable().
     */
    RouteEntry route(HandlerFn fn, HttpMethod method, std::string path,
                     std::string name, std::string description = {}) {
        return RouteEntry(
            method,
            std::move(path),
            std::move(name),
            [this, fn](const httplib::Request& req, httplib::Response& res) {
                (static_cast<Derived*>(this)->*fn)(req, res);
            },
            std::move(description)
        );
    }
};
