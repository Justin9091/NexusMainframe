//
// Created by jusra on 3-5-2026.
//

#include "server/Endpoint.hpp"

Endpoint::Endpoint(std::string path, std::string name, HttpMethod method, Handler handler)
    : _path(std::move(path)),
      _name(std::move(name)),
      _method(method),
      _handler(std::move(handler))
{
    if (_path.empty())    throw std::invalid_argument("Endpoint path must not be empty.");
    if (_name.empty())    throw std::invalid_argument("Endpoint name must not be empty.");
    if (!_handler)        throw std::invalid_argument("Endpoint handler must not be null.");
}

const std::string& Endpoint::getPath()    const noexcept { return _path;    }
const std::string& Endpoint::getName()    const noexcept { return _name;    }
HttpMethod          Endpoint::getMethod()  const noexcept { return _method;  }
const Endpoint::Handler& Endpoint::getHandler() const noexcept { return _handler; }

void Endpoint::handle(const httplib::Request& req, httplib::Response& res) const {
    _handler(req, res);
}

std::string Endpoint::toString() const {
    return "[" + _name + "] " + httpMethodToString(_method) + " " + _path;
}

bool Endpoint::operator==(const Endpoint& other) const noexcept {
    return _method == other._method && _path == other._path;
}

bool Endpoint::operator!=(const Endpoint& other) const noexcept {
    return !(*this == other);
}