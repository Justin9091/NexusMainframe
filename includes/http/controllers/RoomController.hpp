#pragma once
#include "http/Controller.hpp"
#include "services/RoomService.hpp"

namespace httplib { struct Request; struct Response; }

class RoomController : public Controller<RoomController> {
public:
    explicit RoomController(RoomService& service);

protected:
    std::vector<RouteEntry> routeTable() override;

private:
    void index        (const httplib::Request& req, httplib::Response& res);
    void create       (const httplib::Request& req, httplib::Response& res);
    void show         (const httplib::Request& req, httplib::Response& res);
    void destroy      (const httplib::Request& req, httplib::Response& res);
    void addDevice    (const httplib::Request& req, httplib::Response& res);
    void removeDevice (const httplib::Request& req, httplib::Response& res);

    RoomService& _service;
};
