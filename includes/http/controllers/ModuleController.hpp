#pragma once
#include "http/Controller.hpp"
#include "services/ModuleService.hpp"

namespace httplib { struct Request; struct Response; }

/**
 * @brief HTTP controller for module management endpoints.
 *
 * Exposes:
 * - GET  /api/modules                   — list all loaded modules
 * - POST /api/modules/:name/enable      — load and start a module
 * - POST /api/modules/:name/disable     — unload a module
 */
class ModuleController : public Controller<ModuleController> {
public:
    explicit ModuleController(ModuleService& service);

protected:
    std::vector<RouteEntry> routeTable() override;

private:
    void index  (const httplib::Request& req, httplib::Response& res);
    void enable (const httplib::Request& req, httplib::Response& res);
    void disable(const httplib::Request& req, httplib::Response& res);

    ModuleService& _service;
};
