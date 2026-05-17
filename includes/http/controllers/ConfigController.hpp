#pragma once
#include "http/Controller.hpp"
#include "config/NexusConfig.hpp"

namespace httplib { struct Request; struct Response; }

class ConfigController : public Controller<ConfigController> {
public:
    explicit ConfigController(NexusConfig& config);

protected:
    std::vector<RouteEntry> routeTable() override;

private:
    void show  (const httplib::Request& req, httplib::Response& res);
    void update(const httplib::Request& req, httplib::Response& res);

    NexusConfig& _config;
};
