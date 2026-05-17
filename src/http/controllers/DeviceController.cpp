#include "http/controllers/DeviceController.hpp"
#include "server/HttpMethod.hpp"
#include "logging/WebLogger.hpp"
#include <nlohmann/json.hpp>

static WebLogger deviceLogger{"DeviceController"};

DeviceController::DeviceController(DeviceService& service)
    : _service(service) {}

std::vector<RouteEntry> DeviceController::routeTable() {
    using enum HttpMethod;
    return {
        route(&DeviceController::index,   GET,    "/api/devices",             "device_list",    "List all registered devices"),
        route(&DeviceController::create,  POST,   "/api/devices",             "device_create",  "Register a new device"),
        route(&DeviceController::show,    GET,    "/api/devices/:id",         "device_show",    "Get a single device by id"),
        route(&DeviceController::destroy, DELETE, "/api/devices/:id",         "device_destroy", "Delete a device by id"),
        route(&DeviceController::command, POST,   "/api/devices/:id/command", "device_command", "Send a command to a device"),
    };
}

void DeviceController::index(const httplib::Request&, httplib::Response& res) {
    auto devices = _service.list();
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& d : devices)
        arr.push_back(d.toJson());
    res.set_content(arr.dump(), "application/json");
}

void DeviceController::create(const httplib::Request& req, httplib::Response& res) {
    nlohmann::json body;
    try { body = nlohmann::json::parse(req.body); }
    catch (...) {
        res.status = 400;
        res.set_content(Result::badRequest("Invalid JSON body").toJson(), "application/json");
        return;
    }

    const auto result = _service.add(
        body.value("name", ""),
        body.value("type", "")
    );
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}

void DeviceController::show(const httplib::Request& req, httplib::Response& res) {
    const auto& id = req.path_params.at("id");
    const auto device = _service.get(id);
    if (!device) {
        res.status = 404;
        res.set_content(Result::notFound("Device '" + id + "' not found").toJson(), "application/json");
        return;
    }
    res.set_content(device->toJson().dump(), "application/json");
}

void DeviceController::destroy(const httplib::Request& req, httplib::Response& res) {
    const auto& id = req.path_params.at("id");
    const auto result = _service.remove(id);
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}

void DeviceController::command(const httplib::Request& req, httplib::Response& res) {
    const auto& id = req.path_params.at("id");

    nlohmann::json body;
    try { body = nlohmann::json::parse(req.body); }
    catch (...) {
        res.status = 400;
        res.set_content(Result::badRequest("Invalid JSON body").toJson(), "application/json");
        return;
    }

    if (!body.contains("command")) {
        res.status = 400;
        res.set_content(Result::badRequest("Field 'command' is required").toJson(), "application/json");
        return;
    }

    deviceLogger.info("Command '" + body["command"].get<std::string>() + "' for device '" + id + "'");

    nlohmann::json payload = body.value("payload", nlohmann::json::object());
    payload["_command"] = body["command"];

    const auto result = _service.command(id, payload);
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}
