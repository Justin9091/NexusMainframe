#pragma once
#include <string>
#include "server/Endpoint.hpp"
#include "server/HttpMethod.hpp"

class RouteEntry {
public:
    RouteEntry(HttpMethod method, std::string path, std::string name,
               Endpoint::Handler handler, std::string description = {})
        : _method(method)
        , _path(std::move(path))
        , _name(std::move(name))
        , _handler(std::move(handler))
        , _description(std::move(description)) {}

    HttpMethod               method()      const { return _method; }
    const std::string&       path()        const { return _path; }
    const std::string&       name()        const { return _name; }
    const Endpoint::Handler& handler()     const { return _handler; }
    const std::string&       description() const { return _description; }

private:
    HttpMethod        _method;
    std::string       _path;
    std::string       _name;
    Endpoint::Handler _handler;
    std::string       _description;
};
