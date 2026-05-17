#include "http/controllers/ModuleController.hpp"
#include "server/HttpMethod.hpp"
#include "logging/WebLogger.hpp"
#include <nlohmann/json.hpp>

static WebLogger logger{"ModuleController"};

ModuleController::ModuleController(ModuleService& service)
    : _service(service) {}

std::vector<RouteEntry> ModuleController::routeTable() {
    using enum HttpMethod;
    return {
        route(&ModuleController::index,   GET,  "/api/modules",               "modules_list",   "List all currently loaded modules"    ),
        route(&ModuleController::enable,  POST, "/api/modules/:name/enable",  "module_enable",  "Load and initialise a module by name" ),
        route(&ModuleController::disable, POST, "/api/modules/:name/disable", "module_disable", "Shut down and unload a module by name"),
    };
}

void ModuleController::index(const httplib::Request&, httplib::Response& res) {
    auto mods = _service.list();
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& m : mods)
        arr.push_back({{"name", m.name}, {"path", m.path}});
    res.set_content(arr.dump(), "application/json");
}

void ModuleController::enable(const httplib::Request& req, httplib::Response& res) {
    const auto& name = req.path_params.at("name");
    logger.info("Enable request for module '" + name + "'");
    const auto result = _service.enable(name);
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}

void ModuleController::disable(const httplib::Request& req, httplib::Response& res) {
    const auto& name = req.path_params.at("name");
    logger.info("Disable request for module '" + name + "'");
    const auto result = _service.disable(name);
    res.status = result.httpStatus();
    res.set_content(result.toJson(), "application/json");
}
