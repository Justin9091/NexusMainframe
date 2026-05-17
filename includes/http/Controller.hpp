#pragma once
#include <string>
#include "http/IController.hpp"
#include "http/RouteEntry.hpp"
#include "server/HttpMethod.hpp"

// CRTP base that provides the route() binding helper.
// Concrete controllers extend Controller<Derived> instead of IController directly.
template<typename Derived>
class Controller : public IController {
protected:
    using HandlerFn = void (Derived::*)(const httplib::Request&, httplib::Response&);

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
