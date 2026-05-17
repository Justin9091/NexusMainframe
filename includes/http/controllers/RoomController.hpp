#pragma once
#include "http/Controller.hpp"
#include "services/RoomService.hpp"

namespace httplib { struct Request; struct Response; }

/**
 * @brief HTTP controller for room and device-assignment endpoints.
 *
 * Exposes:
 * - GET    /api/rooms                              — list all rooms
 * - POST   /api/rooms                              — create a new room
 * - GET    /api/rooms/:id                          — get a room with expanded device objects
 * - DELETE /api/rooms/:id                          — delete a room
 * - POST   /api/rooms/:id/devices/:deviceId        — assign a device to a room
 * - DELETE /api/rooms/:id/devices/:deviceId        — remove a device from a room
 */
class RoomController : public Controller<RoomController> {
public:
    explicit RoomController(RoomService& service);

protected:
    std::vector<RouteEntry> routeTable() override;

private:
    void index       (const httplib::Request& req, httplib::Response& res);
    void create      (const httplib::Request& req, httplib::Response& res);
    void show        (const httplib::Request& req, httplib::Response& res);
    void destroy     (const httplib::Request& req, httplib::Response& res);
    void addDevice   (const httplib::Request& req, httplib::Response& res);
    void removeDevice(const httplib::Request& req, httplib::Response& res);

    RoomService& _service;
};
