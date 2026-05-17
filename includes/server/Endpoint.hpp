#ifndef NEXUSCORE_ENDPOINT_HPP
#define NEXUSCORE_ENDPOINT_HPP

#include <string>
#include <vector>
#include <functional>
#include <stdexcept>

#include <httplib.h>
#include <nlohmann/json.hpp>

#include "HttpMethod.hpp"

struct EndpointParam {
    std::string name;
    std::string in;          // "path", "query", "body"
    std::string type;        // "string", "number", "boolean", "object", "array"
    std::string description;
    bool        required = true;
};

class Endpoint {
public:
    using Handler = std::function<void(const httplib::Request&, httplib::Response&)>;

    Endpoint() = default;
    Endpoint(std::string path, std::string name, HttpMethod method, Handler handler);

    // Builder — returns *this for chaining
    Endpoint& describe(std::string description);
    Endpoint& param(std::string name, std::string in, std::string type,
                    std::string description, bool required = true);

    [[nodiscard]] const std::string&              getPath()        const noexcept;
    [[nodiscard]] const std::string&              getName()        const noexcept;
    [[nodiscard]] HttpMethod                       getMethod()      const noexcept;
    [[nodiscard]] const Handler&                   getHandler()     const noexcept;
    [[nodiscard]] const std::string&              getDescription() const noexcept;
    [[nodiscard]] const std::vector<EndpointParam>& getParams()    const noexcept;

    // Builds a JSON Schema object from registered params (used by MCP)
    [[nodiscard]] nlohmann::json buildInputSchema() const;

    void handle(const httplib::Request& req, httplib::Response& res) const;

    [[nodiscard]] std::string toString() const;

    bool operator==(const Endpoint& other) const noexcept;
    bool operator!=(const Endpoint& other) const noexcept;

private:
    std::string              _path;
    std::string              _name;
    HttpMethod               _method{};
    Handler                  _handler;
    std::string              _description;
    std::vector<EndpointParam> _params;
};

#endif //NEXUSCORE_ENDPOINT_HPP
