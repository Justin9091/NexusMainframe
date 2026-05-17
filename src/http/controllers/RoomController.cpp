#include "http/controllers/RoomController.hpp"
#include "server/HttpMethod.hpp"
#include <nlohmann/json.hpp>

RoomController::RoomController(RoomService& service)
    : _service(service) {}

std::vector<RouteEntry> RoomController::routeTable() {
    using enum HttpMethod;
    return {
        route(&RoomController::index,        GET,    "/api/rooms",                       "room_list",          "List all rooms"),
        route(&RoomController::create,       POST,   "/api/rooms",                       "room_create",        "Create a new room"),
        route(&RoomController::show,         GET,    "/api/rooms/:id",                   "room_show",          "Get a room with its devices expanded"),
        route(&RoomController::destroy,      DELETE, "/api/rooms/:id",                   "room_destroy",       "Delete a room by id"),
        route(&RoomController::addDevice,    POST,   "/api/rooms/:id/devices/:deviceId", "room_add_device",    "Add a device to a room"),
        route(&RoomController::removeDevice, DELETE, "/api/rooms/:id/devices/:deviceId", "room_remove_device", "Remove a device from a room"),
    };
}

void RoomController::index(const httplib::Request&, httplib::Response& res) {
    auto rooms = _service.list();
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& r : rooms)
        arr.push_back(_service.expand(r));
    res.set_content(arr.dump(), "application/json");
}

void RoomController::create(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json body;
    try { body = nlohmann::json::parse(req.body); }
    catch (...) {
        res.status = 400;
        res.set_content(Result::badRequest("Invalid JSON body").toJson(), "application/json");
        return;
    }
    const auto result = _service.add(body.value("name", ""));
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}

void RoomController::show(const httplib::Request& req, httplib::Response& res) {
    const auto& id = req.path_params.at("id");
    const auto room = _service.get(id);
    if (!room) {
        res.status = 404;
        res.set_content(Result::notFound("Room '" + id + "' not found").toJson(), "application/json");
        return;
    }
    res.set_content(_service.expand(*room).dump(), "application/json");
}

void RoomController::destroy(const httplib::Request& req, httplib::Response& res) {
    const auto& id = req.path_params.at("id");
    const auto result = _service.remove(id);
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}

void RoomController::addDevice(const httplib::Request& req, httplib::Response& res) {
    const auto& roomId   = req.path_params.at("id");
    const auto& deviceId = req.path_params.at("deviceId");
    const auto result = _service.addDevice(roomId, deviceId);
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}

void RoomController::removeDevice(const httplib::Request& req, httplib::Response& res) {
    const auto& roomId   = req.path_params.at("id");
    const auto& deviceId = req.path_params.at("deviceId");
    const auto result = _service.removeDevice(roomId, deviceId);
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}
