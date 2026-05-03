//
// Created by jusra on 3-5-2026.
//

#ifndef NEXUSCORE_ENDPOINT_HPP
#define NEXUSCORE_ENDPOINT_HPP

#include <string>
#include <functional>
#include <stdexcept>

#include <httplib.h>
#include "HttpMethod.hpp"

class Endpoint {
public:
    using Handler = std::function<void(const httplib::Request&, httplib::Response&)>;

    Endpoint() = default;
    Endpoint(std::string path, std::string name, HttpMethod method, Handler handler);

    [[nodiscard]] const std::string& getPath()    const noexcept;
    [[nodiscard]] const std::string& getName()    const noexcept;
    [[nodiscard]] HttpMethod          getMethod()  const noexcept;
    [[nodiscard]] const Handler&      getHandler() const noexcept;

    void handle(const httplib::Request& req, httplib::Response& res) const;

    [[nodiscard]] std::string toString() const;

    bool operator==(const Endpoint& other) const noexcept;
    bool operator!=(const Endpoint& other) const noexcept;

private:
    std::string _path;
    std::string _name;
    HttpMethod  _method;
    Handler     _handler;
};

#endif //NEXUSCORE_ENDPOINT_HPP