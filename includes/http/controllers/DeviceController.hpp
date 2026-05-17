#pragma once
#include "http/Controller.hpp"
#include "services/DeviceService.hpp"

namespace httplib { struct Request; struct Response; }

/**
 * @brief HTTP controller for the `/api/devices` endpoints.
 *
 * Registered routes:
 *
 * | Method | Path                          | Handler  | Description              |
 * |--------|-------------------------------|----------|--------------------------|
 * | GET    | `/api/devices`                | index    | List all devices         |
 * | POST   | `/api/devices`                | create   | Register a new device    |
 * | GET    | `/api/devices/:id`            | show     | Get a single device      |
 * | DELETE | `/api/devices/:id`            | destroy  | Remove a device          |
 * | POST   | `/api/devices/:id/command`    | command  | Send a command to device |
 *
 * @see DeviceService
 */
class DeviceController : public Controller<DeviceController> {
public:
    explicit DeviceController(DeviceService& service);

protected:
    std::vector<RouteEntry> routeTable() override;

private:
    void index  (const httplib::Request& req, httplib::Response& res);
    void create (const httplib::Request& req, httplib::Response& res);
    void show   (const httplib::Request& req, httplib::Response& res);
    void destroy(const httplib::Request& req, httplib::Response& res);
    void command(const httplib::Request& req, httplib::Response& res);

    DeviceService& _service;
};
